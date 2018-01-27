################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=
LINKER_SCRIPT_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS := 


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../Accelerometer.c \
../Quadcopter.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
Accelerometer.o \
Quadcopter.o

OBJS_AS_ARGS +=  \
Accelerometer.o \
Quadcopter.o

C_DEPS +=  \
Accelerometer.d \
Quadcopter.d

C_DEPS_AS_ARGS +=  \
Accelerometer.d \
Quadcopter.d

OUTPUT_FILE_PATH +=Quadcopter.elf

OUTPUT_FILE_PATH_AS_ARGS +=Quadcopter.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

LINKER_SCRIPT_DEP+= 


# AVR32/GNU C Compiler





./%.o: .././%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -funsigned-char -funsigned-bitfields -DNDEBUG  -I"../../../../Include"  -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -mrelax -Wall -mmcu=atmega1284p -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP) $(LIB_DEP) $(LINKER_SCRIPT_DEP)
	@echo Building target: $@
	@echo Invoking: AVR/GNU Linker : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="Quadcopter.map" -Wl,--start-group -Wl,-lm -Wl,-lTrace -Wl,-lUART -Wl,-lPWM -Wl,-lTransceiver -Wl,-lSPI -Wl,-lExtInt -Wl,-lTWI -Wl,-lADXL345 -Wl,-lHMC5883L -Wl,-lQuadcopterLib -Wl,-lCommon  -Wl,--end-group -Wl,-L"../../../../Library/PWM/PWM/Release" -Wl,-L"../../../../Library/Trace/Trace/Release" -Wl,-L"../../../../Library/Transceiver/Transceiver/Release" -Wl,-L"../../../../Library/UART/UART/Release" -Wl,-L"../../../../Library/SPI/SPI/Release" -Wl,-L"../../../../Library/ExtInt/ExtInt/Release" -Wl,-L"../../../../Library/TWI/TWI/Release" -Wl,-L"../../../../Library/ADXL345/ADXL345/Release" -Wl,-L"../../../../Library/HMC5883L/HMC5883L/Release" -Wl,-L"../../../../Library/QuadcopterLib/QuadcopterLib/Release" -Wl,-L"../../../../Library/Common/Common/Release"  -Wl,--gc-sections -mrelax -mmcu=atmega1284p  
	@echo Finished building target: $@
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "Quadcopter.elf" "Quadcopter.hex"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "Quadcopter.elf" "Quadcopter.eep" || exit 0
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "Quadcopter.elf" > "Quadcopter.lss"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "Quadcopter.elf" "Quadcopter.srec"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1061\avr8-gnu-toolchain\bin\avr-size.exe" "Quadcopter.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "Quadcopter.elf" "Quadcopter.a" "Quadcopter.hex" "Quadcopter.lss" "Quadcopter.eep" "Quadcopter.map" "Quadcopter.srec" "Quadcopter.usersignatures"
	