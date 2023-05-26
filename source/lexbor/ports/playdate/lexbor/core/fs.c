/*
 * Copyright (C) 2018 Alexander Borisov
 *
 * Author: Alexander Borisov <borisov@lexbor.com>
 */

#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif

#ifndef _BSD_SOURCE
#define _BSD_SOURCE
#endif

#include <string.h>

#include <pd_api.h>
extern PlaydateAPI *pd;

#include "lexbor/core/fs.h"

typedef struct {
    const lxb_char_t *dirpath;
    lexbor_fs_dir_opt_t opt;
    void *ctx;
} lexbor_fs_dir_arg;


void
lexbor_fs_dir_read_cb(const char *name, lexbor_fs_dir_arg *arg)
{
    lexbor_fs_file_type_t f_type = lexbor_fs_file_type((const lxb_char_t *) name);
    if (arg->opt & LEXBOR_FS_DIR_OPT_WITHOUT_DIR
        && f_type == LEXBOR_FS_FILE_TYPE_DIRECTORY)
    {
        return;
    }

    if (arg->opt & LEXBOR_FS_DIR_OPT_WITHOUT_FILE
        && f_type == LEXBOR_FS_FILE_TYPE_FILE)
    {
        return;
    }
    
    char *full_path;
    pd->system->format_string(&full_path, "%s%s", (const char *) opt->dirpath, name);
    
    lexbor_action_t action = callback((const lxb_char_t *) full_path,
                      strlen(full_path),
                      (const lxb_char_t *) name,
                      strlen(name), opt->ctx);
    free(full_path);
}

lxb_status_t
lexbor_fs_dir_read(const lxb_char_t *dirpath, lexbor_fs_dir_opt_t opt,
                   lexbor_fs_dir_file_f callback, void *ctx)
{
    lexbor_fs_dir_arg arg;
    
    arg.dirpath = dirpath;
    arg.opt = opt;
    arg.ctx = ctx;
    
    int err = pd->file->listfiles((const char *) dirpath, lexbor_fs_dir_read_cb, &arg, !(opt & LEXBOR_FS_DIR_OPT_WITHOUT_HIDDEN));
    if(err) {
        return LXB_STATUS_ERROR;
    }

    return LXB_STATUS_OK;
}

lexbor_fs_file_type_t
lexbor_fs_file_type(const lxb_char_t *full_path)
{
    FileStat sb;

    if (pd->file->stat((const char *) full_path, &sb) == -1) {
        return LEXBOR_FS_FILE_TYPE_UNDEF;
    }

    if (sb.isdir) {
        return LEXBOR_FS_FILE_TYPE_DIRECTORY;
    }
    else {
    	return LEXBOR_FS_FILE_TYPE_FILE;
    }
    
    return LEXBOR_FS_FILE_TYPE_UNDEF;
}

lxb_char_t *
lexbor_fs_file_easy_read(const lxb_char_t *full_path, size_t *len)
{
    int size;
    size_t nread;
    lxb_char_t *data;

    SDFile fh = pd->file->open((const char *) full_path, kFileRead | kFileReadData);
    if (fh == NULL) {
        goto error;
    }

    if (pd->file->seek(fh, 0, SEEK_END) != 0) {
        goto error_close;
    }

    size = pd->file->tell(fh);
    if (size < 0) {
        goto error_close;
    }

    if (pd->file->seek(fh, 0, SEEK_SET) != 0) {
        goto error_close;
    }

    data = lexbor_malloc(size + 1);
    if (data == NULL) {
        goto error_close;
    }

    nread = pd->file->read(fh, data, size);
    if (nread != (size_t) size) {
        lexbor_free(data);
        goto error_close;
    }
    
    pd->file->close(fh);
    
    data[size] = '\0';
    
    if (len != NULL) {
        *len = nread;
    }

    return data;

error_close:

    pd->file->close(fh);

error:

    if (len != NULL) {
        *len = 0;
    }

    return NULL;
}
