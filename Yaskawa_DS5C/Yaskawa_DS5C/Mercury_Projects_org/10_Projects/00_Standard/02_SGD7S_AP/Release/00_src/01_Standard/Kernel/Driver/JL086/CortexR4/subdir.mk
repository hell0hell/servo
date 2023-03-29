################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Driver/JL086/CortexR4/DetectTemperature.c \
E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Driver/JL086/CortexR4/Dma.c \
E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Driver/JL086/CortexR4/Eeprom.c \
E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Driver/JL086/CortexR4/HwDeviceIf.c \
E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Driver/JL086/CortexR4/Uart.c 

OBJS += \
./00_src/01_Standard/Kernel/Driver/JL086/CortexR4/DetectTemperature.o \
./00_src/01_Standard/Kernel/Driver/JL086/CortexR4/Dma.o \
./00_src/01_Standard/Kernel/Driver/JL086/CortexR4/Eeprom.o \
./00_src/01_Standard/Kernel/Driver/JL086/CortexR4/HwDeviceIf.o \
./00_src/01_Standard/Kernel/Driver/JL086/CortexR4/Uart.o 

C_DEPS += \
./00_src/01_Standard/Kernel/Driver/JL086/CortexR4/DetectTemperature.d \
./00_src/01_Standard/Kernel/Driver/JL086/CortexR4/Dma.d \
./00_src/01_Standard/Kernel/Driver/JL086/CortexR4/Eeprom.d \
./00_src/01_Standard/Kernel/Driver/JL086/CortexR4/HwDeviceIf.d \
./00_src/01_Standard/Kernel/Driver/JL086/CortexR4/Uart.d 


# Each subdirectory must supply rules for building sources it contributes
00_src/01_Standard/Kernel/Driver/JL086/CortexR4/DetectTemperature.o: E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Driver/JL086/CortexR4/DetectTemperature.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C コンパイラ'
	armcc -O2 -g --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/01_Standard/Kernel/Driver/JL086/CortexR4/Dma.o: E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Driver/JL086/CortexR4/Dma.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C コンパイラ'
	armcc -O2 -g --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/01_Standard/Kernel/Driver/JL086/CortexR4/Eeprom.o: E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Driver/JL086/CortexR4/Eeprom.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C コンパイラ'
	armcc -O2 -g --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/01_Standard/Kernel/Driver/JL086/CortexR4/HwDeviceIf.o: E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Driver/JL086/CortexR4/HwDeviceIf.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C コンパイラ'
	armcc -O2 -g --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/01_Standard/Kernel/Driver/JL086/CortexR4/Uart.o: E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Driver/JL086/CortexR4/Uart.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C コンパイラ'
	armcc -O2 -g --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


