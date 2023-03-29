################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/ScanA/IntScanA.c \
E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/ScanA/MotorLessTest.c 

OBJS += \
./00_src/01_Standard/Kernel/ScanA/IntScanA.o \
./00_src/01_Standard/Kernel/ScanA/MotorLessTest.o 

C_DEPS += \
./00_src/01_Standard/Kernel/ScanA/IntScanA.d \
./00_src/01_Standard/Kernel/ScanA/MotorLessTest.d 


# Each subdirectory must supply rules for building sources it contributes
00_src/01_Standard/Kernel/ScanA/IntScanA.o: E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/ScanA/IntScanA.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C コンパイラ'
	armcc --cpu=Cortex-R4F --littleend --thumb --apcs=/interwork --fpu=VFPv3_D16 -DCSW_NETIF_ANLGPLS_TYPE -I"../../../../00_src\00_Bsp\\JL086_bsp" -I"../../../../00_src\00_Bsp\\JL086_bsp\CPUPeripheral" -I"../../../../00_src\00_Bsp\\JL086_bsp\usb_function" -I"../../../../00_src\01_Standard\Kernel\Boot\MicroProgram" -I"../../../../00_src\01_Standard\Communications" -I"../../../../00_src\01_Standard\Communications\AP\Motion" -I"../../../../00_src\01_Standard\Communications\AP\ScanN" -I"../../../../00_src\01_Standard\Communications\AP\XlsTbl" -I"../../../../00_src\01_Standard\BaseControls" -I"../../../../00_src\01_Standard\BaseSequences" -I"../../../../00_src\01_Standard\Kernel\Include" -I"../../../../00_src\01_Standard\Kernel\Lib" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\CortexR4" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\UDL\Servo" -I"../../../../00_src\01_Standard\Kernel\Driver" -I"../../../../00_src\01_Standard\Kernel\Boot\Config" -I../../../../00_src/01_Standard/Kernel/ScanA -I"../../../../00_src\01_Standard\Kernel\ScanB" -I"../../../../00_src\01_Standard\Kernel\ScanC" -I"../../../../00_src\01_Standard\Kernel\Round\Registers" -I"../../../../00_src\01_Standard\Kernel\Round\UtilityFuns" -I"../../../../00_src\01_Standard\Kernel\Round\MsgIf" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\DataTrace" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\AnalogMonitor" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\Registers" -I"../../../../00_src\02_Custom\00_SGD7\01_SGD7S_AP" -O1 -Otime --feedback="../02_SGD7S_AP.fb" -g --signed_chars --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/01_Standard/Kernel/ScanA/MotorLessTest.o: E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/01_Standard/Kernel/ScanA/MotorLessTest.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C コンパイラ'
	armcc --cpu=Cortex-R4F --littleend --thumb --apcs=/interwork --fpu=VFPv3_D16 -DCSW_NETIF_ANLGPLS_TYPE -I"../../../../00_src\00_Bsp\\JL086_bsp" -I"../../../../00_src\00_Bsp\\JL086_bsp\CPUPeripheral" -I"../../../../00_src\00_Bsp\\JL086_bsp\usb_function" -I"../../../../00_src\01_Standard\Kernel\Boot\MicroProgram" -I"../../../../00_src\01_Standard\Communications" -I"../../../../00_src\01_Standard\Communications\AP\Motion" -I"../../../../00_src\01_Standard\Communications\AP\ScanN" -I"../../../../00_src\01_Standard\Communications\AP\XlsTbl" -I"../../../../00_src\01_Standard\BaseControls" -I"../../../../00_src\01_Standard\BaseSequences" -I"../../../../00_src\01_Standard\Kernel\Include" -I"../../../../00_src\01_Standard\Kernel\Lib" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\CortexR4" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\UDL\Servo" -I"../../../../00_src\01_Standard\Kernel\Driver" -I"../../../../00_src\01_Standard\Kernel\Boot\Config" -I../../../../00_src/01_Standard/Kernel/ScanA -I"../../../../00_src\01_Standard\Kernel\ScanB" -I"../../../../00_src\01_Standard\Kernel\ScanC" -I"../../../../00_src\01_Standard\Kernel\Round\Registers" -I"../../../../00_src\01_Standard\Kernel\Round\UtilityFuns" -I"../../../../00_src\01_Standard\Kernel\Round\MsgIf" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\DataTrace" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\AnalogMonitor" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\Registers" -I"../../../../00_src\02_Custom\00_SGD7\01_SGD7S_AP" -O1 -Otime --feedback="../02_SGD7S_AP.fb" -g --signed_chars --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


