################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Lib/KLib.c \
E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Lib/MLib.c 

OBJS += \
./00_src/01_Standard/Kernel/Lib/KLib.o \
./00_src/01_Standard/Kernel/Lib/MLib.o 

C_DEPS += \
./00_src/01_Standard/Kernel/Lib/KLib.d \
./00_src/01_Standard/Kernel/Lib/MLib.d 


# Each subdirectory must supply rules for building sources it contributes
00_src/01_Standard/Kernel/Lib/KLib.o: E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Lib/KLib.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C++ コンパイラ'
	armcc --cpu=Cortex-R4F --littleend --thumb --apcs=/interwork --fpu=VFPv3_D16 -D_MERCURY_PLATFORM_ -I../../../../00_src/00_Bsp/JL086_bsp -I../../../../00_src/00_Bsp/JL086_bsp/CPUPeripheral -I../../../../00_src/00_Bsp/JL086_bsp/usb_function -I../../../../00_src/01_Standard/Kernel/Driver -I../../../../00_src/01_Standard/Kernel/Driver/JL086/CortexR4 -I../../../../00_src/01_Standard/Kernel/Driver/JL086/UDL/servo -I../../../../00_src/01_Standard/Kernel/Driver/minizip -I../../../../00_src/01_Standard/Kernel/Driver/minizip/memmng -I../../../../00_src/01_Standard/Kernel/Driver/minizip/zlib -I../../../../00_src/01_Standard/Kernel/Include -I../../../../00_src/01_Standard/Kernel/Lib -I../../../../00_src/01_Standard/Kernel/Round/MsgIf -I../../../../00_src/02_Custom/00_SGD7/00_SpecCommon -O1 -Otime --feedback="../01_Mercury_Platform.fb" -g --signed_chars --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/01_Standard/Kernel/Lib/MLib.o: E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/Lib/MLib.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C++ コンパイラ'
	armcc --cpu=Cortex-R4F --littleend --thumb --apcs=/interwork --fpu=VFPv3_D16 -D_MERCURY_PLATFORM_ -I../../../../00_src/00_Bsp/JL086_bsp -I../../../../00_src/00_Bsp/JL086_bsp/CPUPeripheral -I../../../../00_src/00_Bsp/JL086_bsp/usb_function -I../../../../00_src/01_Standard/Kernel/Driver -I../../../../00_src/01_Standard/Kernel/Driver/JL086/CortexR4 -I../../../../00_src/01_Standard/Kernel/Driver/JL086/UDL/servo -I../../../../00_src/01_Standard/Kernel/Driver/minizip -I../../../../00_src/01_Standard/Kernel/Driver/minizip/memmng -I../../../../00_src/01_Standard/Kernel/Driver/minizip/zlib -I../../../../00_src/01_Standard/Kernel/Include -I../../../../00_src/01_Standard/Kernel/Lib -I../../../../00_src/01_Standard/Kernel/Round/MsgIf -I../../../../00_src/02_Custom/00_SGD7/00_SpecCommon -O1 -Otime --feedback="../01_Mercury_Platform.fb" -g --signed_chars --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


