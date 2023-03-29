################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/00_Bsp/JL086_bsp/InterruptTbl.c \
E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/00_Bsp/JL086_bsp/Interrupts.c \
E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/00_Bsp/JL086_bsp/JL086Core.c \
E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/00_Bsp/JL086_bsp/SFlash.c \
E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/00_Bsp/JL086_bsp/UsbFuncDriver.c \
E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/00_Bsp/JL086_bsp/elf.c 

S_SRCS += \
E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/00_Bsp/JL086_bsp/init_SGD7.s 

OBJS += \
./00_src/00_Bsp/JL086_bsp/InterruptTbl.o \
./00_src/00_Bsp/JL086_bsp/Interrupts.o \
./00_src/00_Bsp/JL086_bsp/JL086Core.o \
./00_src/00_Bsp/JL086_bsp/SFlash.o \
./00_src/00_Bsp/JL086_bsp/UsbFuncDriver.o \
./00_src/00_Bsp/JL086_bsp/elf.o \
./00_src/00_Bsp/JL086_bsp/init_SGD7.o 

C_DEPS += \
./00_src/00_Bsp/JL086_bsp/InterruptTbl.d \
./00_src/00_Bsp/JL086_bsp/Interrupts.d \
./00_src/00_Bsp/JL086_bsp/JL086Core.d \
./00_src/00_Bsp/JL086_bsp/SFlash.d \
./00_src/00_Bsp/JL086_bsp/UsbFuncDriver.d \
./00_src/00_Bsp/JL086_bsp/elf.d 

S_DEPS += \
./00_src/00_Bsp/JL086_bsp/init_SGD7.d 


# Each subdirectory must supply rules for building sources it contributes
00_src/00_Bsp/JL086_bsp/InterruptTbl.o: E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/00_Bsp/JL086_bsp/InterruptTbl.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C++ コンパイラ'
	armcc --cpu=Cortex-R4F --littleend --thumb --apcs=/interwork --fpu=VFPv3_D16 -D_MERCURY_PLATFORM_ -I../../../../00_src/00_Bsp/JL086_bsp -I../../../../00_src/00_Bsp/JL086_bsp/CPUPeripheral -I../../../../00_src/00_Bsp/JL086_bsp/usb_function -I../../../../00_src/01_Standard/Kernel/Driver -I../../../../00_src/01_Standard/Kernel/Driver/JL086/CortexR4 -I../../../../00_src/01_Standard/Kernel/Driver/JL086/UDL/servo -I../../../../00_src/01_Standard/Kernel/Driver/minizip -I../../../../00_src/01_Standard/Kernel/Driver/minizip/memmng -I../../../../00_src/01_Standard/Kernel/Driver/minizip/zlib -I../../../../00_src/01_Standard/Kernel/Include -I../../../../00_src/01_Standard/Kernel/Lib -I../../../../00_src/01_Standard/Kernel/Round/MsgIf -I../../../../00_src/02_Custom/00_SGD7/00_SpecCommon -O1 -Otime --feedback="../01_Mercury_Platform.fb" -g --signed_chars --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/00_Bsp/JL086_bsp/Interrupts.o: E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/00_Bsp/JL086_bsp/Interrupts.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C++ コンパイラ'
	armcc --cpu=Cortex-R4F --littleend --thumb --apcs=/interwork --fpu=VFPv3_D16 -D_MERCURY_PLATFORM_ -I../../../../00_src/00_Bsp/JL086_bsp -I../../../../00_src/00_Bsp/JL086_bsp/CPUPeripheral -I../../../../00_src/00_Bsp/JL086_bsp/usb_function -I../../../../00_src/01_Standard/Kernel/Driver -I../../../../00_src/01_Standard/Kernel/Driver/JL086/CortexR4 -I../../../../00_src/01_Standard/Kernel/Driver/JL086/UDL/servo -I../../../../00_src/01_Standard/Kernel/Driver/minizip -I../../../../00_src/01_Standard/Kernel/Driver/minizip/memmng -I../../../../00_src/01_Standard/Kernel/Driver/minizip/zlib -I../../../../00_src/01_Standard/Kernel/Include -I../../../../00_src/01_Standard/Kernel/Lib -I../../../../00_src/01_Standard/Kernel/Round/MsgIf -I../../../../00_src/02_Custom/00_SGD7/00_SpecCommon -O1 -Otime --feedback="../01_Mercury_Platform.fb" -g --signed_chars --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/00_Bsp/JL086_bsp/JL086Core.o: E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/00_Bsp/JL086_bsp/JL086Core.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C++ コンパイラ'
	armcc --cpu=Cortex-R4F --littleend --thumb --apcs=/interwork --fpu=VFPv3_D16 -D_MERCURY_PLATFORM_ -I../../../../00_src/00_Bsp/JL086_bsp -I../../../../00_src/00_Bsp/JL086_bsp/CPUPeripheral -I../../../../00_src/00_Bsp/JL086_bsp/usb_function -I../../../../00_src/01_Standard/Kernel/Driver -I../../../../00_src/01_Standard/Kernel/Driver/JL086/CortexR4 -I../../../../00_src/01_Standard/Kernel/Driver/JL086/UDL/servo -I../../../../00_src/01_Standard/Kernel/Driver/minizip -I../../../../00_src/01_Standard/Kernel/Driver/minizip/memmng -I../../../../00_src/01_Standard/Kernel/Driver/minizip/zlib -I../../../../00_src/01_Standard/Kernel/Include -I../../../../00_src/01_Standard/Kernel/Lib -I../../../../00_src/01_Standard/Kernel/Round/MsgIf -I../../../../00_src/02_Custom/00_SGD7/00_SpecCommon -O1 -Otime --feedback="../01_Mercury_Platform.fb" -g --signed_chars --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/00_Bsp/JL086_bsp/SFlash.o: E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/00_Bsp/JL086_bsp/SFlash.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C++ コンパイラ'
	armcc --cpu=Cortex-R4F --littleend --thumb --apcs=/interwork --fpu=VFPv3_D16 -D_MERCURY_PLATFORM_ -I../../../../00_src/00_Bsp/JL086_bsp -I../../../../00_src/00_Bsp/JL086_bsp/CPUPeripheral -I../../../../00_src/00_Bsp/JL086_bsp/usb_function -I../../../../00_src/01_Standard/Kernel/Driver -I../../../../00_src/01_Standard/Kernel/Driver/JL086/CortexR4 -I../../../../00_src/01_Standard/Kernel/Driver/JL086/UDL/servo -I../../../../00_src/01_Standard/Kernel/Driver/minizip -I../../../../00_src/01_Standard/Kernel/Driver/minizip/memmng -I../../../../00_src/01_Standard/Kernel/Driver/minizip/zlib -I../../../../00_src/01_Standard/Kernel/Include -I../../../../00_src/01_Standard/Kernel/Lib -I../../../../00_src/01_Standard/Kernel/Round/MsgIf -I../../../../00_src/02_Custom/00_SGD7/00_SpecCommon -O1 -Otime --feedback="../01_Mercury_Platform.fb" -g --signed_chars --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/00_Bsp/JL086_bsp/UsbFuncDriver.o: E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/00_Bsp/JL086_bsp/UsbFuncDriver.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C++ コンパイラ'
	armcc --cpu=Cortex-R4F --littleend --thumb --apcs=/interwork --fpu=VFPv3_D16 -D_MERCURY_PLATFORM_ -I../../../../00_src/00_Bsp/JL086_bsp -I../../../../00_src/00_Bsp/JL086_bsp/CPUPeripheral -I../../../../00_src/00_Bsp/JL086_bsp/usb_function -I../../../../00_src/01_Standard/Kernel/Driver -I../../../../00_src/01_Standard/Kernel/Driver/JL086/CortexR4 -I../../../../00_src/01_Standard/Kernel/Driver/JL086/UDL/servo -I../../../../00_src/01_Standard/Kernel/Driver/minizip -I../../../../00_src/01_Standard/Kernel/Driver/minizip/memmng -I../../../../00_src/01_Standard/Kernel/Driver/minizip/zlib -I../../../../00_src/01_Standard/Kernel/Include -I../../../../00_src/01_Standard/Kernel/Lib -I../../../../00_src/01_Standard/Kernel/Round/MsgIf -I../../../../00_src/02_Custom/00_SGD7/00_SpecCommon -O1 -Otime --feedback="../01_Mercury_Platform.fb" -g --signed_chars --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/00_Bsp/JL086_bsp/elf.o: E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/00_Bsp/JL086_bsp/elf.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C++ コンパイラ'
	armcc --cpu=Cortex-R4F --littleend --thumb --apcs=/interwork --fpu=VFPv3_D16 -D_MERCURY_PLATFORM_ -I../../../../00_src/00_Bsp/JL086_bsp -I../../../../00_src/00_Bsp/JL086_bsp/CPUPeripheral -I../../../../00_src/00_Bsp/JL086_bsp/usb_function -I../../../../00_src/01_Standard/Kernel/Driver -I../../../../00_src/01_Standard/Kernel/Driver/JL086/CortexR4 -I../../../../00_src/01_Standard/Kernel/Driver/JL086/UDL/servo -I../../../../00_src/01_Standard/Kernel/Driver/minizip -I../../../../00_src/01_Standard/Kernel/Driver/minizip/memmng -I../../../../00_src/01_Standard/Kernel/Driver/minizip/zlib -I../../../../00_src/01_Standard/Kernel/Include -I../../../../00_src/01_Standard/Kernel/Lib -I../../../../00_src/01_Standard/Kernel/Round/MsgIf -I../../../../00_src/02_Custom/00_SGD7/00_SpecCommon -O1 -Otime --feedback="../01_Mercury_Platform.fb" -g --signed_chars --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/00_Bsp/JL086_bsp/init_SGD7.o: E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/00_Bsp/JL086_bsp/init_SGD7.s
	@echo 'Building file: $<'
	@echo 'Invoking: ARM アセンブラ'
	armasm --cpu=Cortex-R4F --littleend --thumb --apcs=/interwork --fpu=VFPv3_D16 -g --md --depend_format=unix_escaped -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


