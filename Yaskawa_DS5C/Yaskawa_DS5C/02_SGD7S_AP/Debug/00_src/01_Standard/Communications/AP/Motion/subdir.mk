################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/01_Standard/Communications/AP/Motion/MtApi.c \
E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/01_Standard/Communications/AP/Motion/MtCtrl.c \
E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/01_Standard/Communications/AP/Motion/MtGenerator.c \
E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/01_Standard/Communications/AP/Motion/MtPRM.c \
E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/01_Standard/Communications/AP/Motion/Position.c 

C_DEPS += \
./00_src/01_Standard/Communications/AP/Motion/MtApi.d \
./00_src/01_Standard/Communications/AP/Motion/MtCtrl.d \
./00_src/01_Standard/Communications/AP/Motion/MtGenerator.d \
./00_src/01_Standard/Communications/AP/Motion/MtPRM.d \
./00_src/01_Standard/Communications/AP/Motion/Position.d 

OBJS += \
./00_src/01_Standard/Communications/AP/Motion/MtApi.o \
./00_src/01_Standard/Communications/AP/Motion/MtCtrl.o \
./00_src/01_Standard/Communications/AP/Motion/MtGenerator.o \
./00_src/01_Standard/Communications/AP/Motion/MtPRM.o \
./00_src/01_Standard/Communications/AP/Motion/Position.o 


# Each subdirectory must supply rules for building sources it contributes
00_src/01_Standard/Communications/AP/Motion/MtApi.o: E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/01_Standard/Communications/AP/Motion/MtApi.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C �R���p�C��'
	armcc --cpu=Cortex-R4F --littleend --thumb --apcs=/interwork --fpu=VFPv3_D16 -DCSW_NETIF_ANLGPLS_TYPE -I"../../../../00_src\00_Bsp\\JL086_bsp" -I"../../../../00_src\00_Bsp\\JL086_bsp\CPUPeripheral" -I"../../../../00_src\00_Bsp\\JL086_bsp\usb_function" -I"../../../../00_src\01_Standard\Kernel\Boot\MicroProgram" -I"../../../../00_src\01_Standard\Communications" -I"../../../../00_src\01_Standard\Communications\AP\Motion" -I"../../../../00_src\01_Standard\Communications\AP\ScanN" -I"../../../../00_src\01_Standard\Communications\AP\XlsTbl" -I"../../../../00_src\01_Standard\BaseControls" -I"../../../../00_src\01_Standard\BaseSequences" -I"../../../../00_src\01_Standard\Kernel\Include" -I"../../../../00_src\01_Standard\Kernel\Lib" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\CortexR4" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\UDL\Servo" -I"../../../../00_src\01_Standard\Kernel\Driver" -I"../../../../00_src\01_Standard\Kernel\Boot\Config" -I../../../../00_src/01_Standard/Kernel/ScanA -I"../../../../00_src\01_Standard\Kernel\ScanB" -I"../../../../00_src\01_Standard\Kernel\ScanC" -I"../../../../00_src\01_Standard\Kernel\Round\Registers" -I"../../../../00_src\01_Standard\Kernel\Round\UtilityFuns" -I"../../../../00_src\01_Standard\Kernel\Round\MsgIf" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\DataTrace" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\AnalogMonitor" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\Registers" -I"../../../../00_src\02_Custom\00_SGD7\01_SGD7S_AP" -O1 -Otime --feedback="../02_SGD7S_AP.fb" -g --signed_chars --md --depend_format=unix_escaped --no_depend_system_headers --depend_dir="00_src/01_Standard/Communications/AP/Motion" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/01_Standard/Communications/AP/Motion/MtCtrl.o: E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/01_Standard/Communications/AP/Motion/MtCtrl.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C �R���p�C��'
	armcc --cpu=Cortex-R4F --littleend --thumb --apcs=/interwork --fpu=VFPv3_D16 -DCSW_NETIF_ANLGPLS_TYPE -I"../../../../00_src\00_Bsp\\JL086_bsp" -I"../../../../00_src\00_Bsp\\JL086_bsp\CPUPeripheral" -I"../../../../00_src\00_Bsp\\JL086_bsp\usb_function" -I"../../../../00_src\01_Standard\Kernel\Boot\MicroProgram" -I"../../../../00_src\01_Standard\Communications" -I"../../../../00_src\01_Standard\Communications\AP\Motion" -I"../../../../00_src\01_Standard\Communications\AP\ScanN" -I"../../../../00_src\01_Standard\Communications\AP\XlsTbl" -I"../../../../00_src\01_Standard\BaseControls" -I"../../../../00_src\01_Standard\BaseSequences" -I"../../../../00_src\01_Standard\Kernel\Include" -I"../../../../00_src\01_Standard\Kernel\Lib" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\CortexR4" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\UDL\Servo" -I"../../../../00_src\01_Standard\Kernel\Driver" -I"../../../../00_src\01_Standard\Kernel\Boot\Config" -I../../../../00_src/01_Standard/Kernel/ScanA -I"../../../../00_src\01_Standard\Kernel\ScanB" -I"../../../../00_src\01_Standard\Kernel\ScanC" -I"../../../../00_src\01_Standard\Kernel\Round\Registers" -I"../../../../00_src\01_Standard\Kernel\Round\UtilityFuns" -I"../../../../00_src\01_Standard\Kernel\Round\MsgIf" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\DataTrace" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\AnalogMonitor" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\Registers" -I"../../../../00_src\02_Custom\00_SGD7\01_SGD7S_AP" -O1 -Otime --feedback="../02_SGD7S_AP.fb" -g --signed_chars --md --depend_format=unix_escaped --no_depend_system_headers --depend_dir="00_src/01_Standard/Communications/AP/Motion" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/01_Standard/Communications/AP/Motion/MtGenerator.o: E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/01_Standard/Communications/AP/Motion/MtGenerator.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C �R���p�C��'
	armcc --cpu=Cortex-R4F --littleend --thumb --apcs=/interwork --fpu=VFPv3_D16 -DCSW_NETIF_ANLGPLS_TYPE -I"../../../../00_src\00_Bsp\\JL086_bsp" -I"../../../../00_src\00_Bsp\\JL086_bsp\CPUPeripheral" -I"../../../../00_src\00_Bsp\\JL086_bsp\usb_function" -I"../../../../00_src\01_Standard\Kernel\Boot\MicroProgram" -I"../../../../00_src\01_Standard\Communications" -I"../../../../00_src\01_Standard\Communications\AP\Motion" -I"../../../../00_src\01_Standard\Communications\AP\ScanN" -I"../../../../00_src\01_Standard\Communications\AP\XlsTbl" -I"../../../../00_src\01_Standard\BaseControls" -I"../../../../00_src\01_Standard\BaseSequences" -I"../../../../00_src\01_Standard\Kernel\Include" -I"../../../../00_src\01_Standard\Kernel\Lib" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\CortexR4" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\UDL\Servo" -I"../../../../00_src\01_Standard\Kernel\Driver" -I"../../../../00_src\01_Standard\Kernel\Boot\Config" -I../../../../00_src/01_Standard/Kernel/ScanA -I"../../../../00_src\01_Standard\Kernel\ScanB" -I"../../../../00_src\01_Standard\Kernel\ScanC" -I"../../../../00_src\01_Standard\Kernel\Round\Registers" -I"../../../../00_src\01_Standard\Kernel\Round\UtilityFuns" -I"../../../../00_src\01_Standard\Kernel\Round\MsgIf" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\DataTrace" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\AnalogMonitor" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\Registers" -I"../../../../00_src\02_Custom\00_SGD7\01_SGD7S_AP" -O1 -Otime --feedback="../02_SGD7S_AP.fb" -g --signed_chars --md --depend_format=unix_escaped --no_depend_system_headers --depend_dir="00_src/01_Standard/Communications/AP/Motion" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/01_Standard/Communications/AP/Motion/MtPRM.o: E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/01_Standard/Communications/AP/Motion/MtPRM.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C �R���p�C��'
	armcc --cpu=Cortex-R4F --littleend --thumb --apcs=/interwork --fpu=VFPv3_D16 -DCSW_NETIF_ANLGPLS_TYPE -I"../../../../00_src\00_Bsp\\JL086_bsp" -I"../../../../00_src\00_Bsp\\JL086_bsp\CPUPeripheral" -I"../../../../00_src\00_Bsp\\JL086_bsp\usb_function" -I"../../../../00_src\01_Standard\Kernel\Boot\MicroProgram" -I"../../../../00_src\01_Standard\Communications" -I"../../../../00_src\01_Standard\Communications\AP\Motion" -I"../../../../00_src\01_Standard\Communications\AP\ScanN" -I"../../../../00_src\01_Standard\Communications\AP\XlsTbl" -I"../../../../00_src\01_Standard\BaseControls" -I"../../../../00_src\01_Standard\BaseSequences" -I"../../../../00_src\01_Standard\Kernel\Include" -I"../../../../00_src\01_Standard\Kernel\Lib" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\CortexR4" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\UDL\Servo" -I"../../../../00_src\01_Standard\Kernel\Driver" -I"../../../../00_src\01_Standard\Kernel\Boot\Config" -I../../../../00_src/01_Standard/Kernel/ScanA -I"../../../../00_src\01_Standard\Kernel\ScanB" -I"../../../../00_src\01_Standard\Kernel\ScanC" -I"../../../../00_src\01_Standard\Kernel\Round\Registers" -I"../../../../00_src\01_Standard\Kernel\Round\UtilityFuns" -I"../../../../00_src\01_Standard\Kernel\Round\MsgIf" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\DataTrace" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\AnalogMonitor" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\Registers" -I"../../../../00_src\02_Custom\00_SGD7\01_SGD7S_AP" -O1 -Otime --feedback="../02_SGD7S_AP.fb" -g --signed_chars --md --depend_format=unix_escaped --no_depend_system_headers --depend_dir="00_src/01_Standard/Communications/AP/Motion" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/01_Standard/Communications/AP/Motion/Position.o: E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/01_Standard/Communications/AP/Motion/Position.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C �R���p�C��'
	armcc --cpu=Cortex-R4F --littleend --thumb --apcs=/interwork --fpu=VFPv3_D16 -DCSW_NETIF_ANLGPLS_TYPE -I"../../../../00_src\00_Bsp\\JL086_bsp" -I"../../../../00_src\00_Bsp\\JL086_bsp\CPUPeripheral" -I"../../../../00_src\00_Bsp\\JL086_bsp\usb_function" -I"../../../../00_src\01_Standard\Kernel\Boot\MicroProgram" -I"../../../../00_src\01_Standard\Communications" -I"../../../../00_src\01_Standard\Communications\AP\Motion" -I"../../../../00_src\01_Standard\Communications\AP\ScanN" -I"../../../../00_src\01_Standard\Communications\AP\XlsTbl" -I"../../../../00_src\01_Standard\BaseControls" -I"../../../../00_src\01_Standard\BaseSequences" -I"../../../../00_src\01_Standard\Kernel\Include" -I"../../../../00_src\01_Standard\Kernel\Lib" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\CortexR4" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\UDL\Servo" -I"../../../../00_src\01_Standard\Kernel\Driver" -I"../../../../00_src\01_Standard\Kernel\Boot\Config" -I../../../../00_src/01_Standard/Kernel/ScanA -I"../../../../00_src\01_Standard\Kernel\ScanB" -I"../../../../00_src\01_Standard\Kernel\ScanC" -I"../../../../00_src\01_Standard\Kernel\Round\Registers" -I"../../../../00_src\01_Standard\Kernel\Round\UtilityFuns" -I"../../../../00_src\01_Standard\Kernel\Round\MsgIf" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\DataTrace" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\AnalogMonitor" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\Registers" -I"../../../../00_src\02_Custom\00_SGD7\01_SGD7S_AP" -O1 -Otime --feedback="../02_SGD7S_AP.fb" -g --signed_chars --md --depend_format=unix_escaped --no_depend_system_headers --depend_dir="00_src/01_Standard/Communications/AP/Motion" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

