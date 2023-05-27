if(NOT DEFINED LEXBOR_OPTIMIZATION_LEVEL)
	set(LEXBOR_OPTIMIZATION_LEVEL "-O2")
endif()

# find the SDK
set(ENVSDK $ENV{PLAYDATE_SDK_PATH})

if (NOT ${ENVSDK} STREQUAL "")
	# Convert path from Windows
	file(TO_CMAKE_PATH ${ENVSDK} SDK)
else ()
	execute_process(
        COMMAND bash -c "egrep '^\\s*SDKRoot' $HOME/.Playdate/config"
        COMMAND head -n 1
        COMMAND cut -c9-
        OUTPUT_VARIABLE SDK
        OUTPUT_STRIP_TRAILING_WHITESPACE
	)
endif ()

set(HEAP_SIZE 8388208)
set(STACK_SIZE 61800)
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -x assembler-with-cpp -D__HEAP_SIZE=${HEAP_SIZE} -D__STACK_SIZE=${STACK_SIZE}")

set(MCFLAGS "-mthumb -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-sp-d16 -D__FPU_USED=1")
set(LEXBOR_C_FLAGS "${LEXBOR_C_FLAGS} -DTARGET_PLAYDATE=1 -DTARGET_EXTENSION=1 -I${SDK}/C_API -std=c11 -Wall -Wno-unknown-pragmas -Wdouble-promotion -falign-functions=16 -fomit-frame-pointer -gdwarf-2 -fverbose-asm -ffunction-sections -fdata-sections ${MCFLAGS}")