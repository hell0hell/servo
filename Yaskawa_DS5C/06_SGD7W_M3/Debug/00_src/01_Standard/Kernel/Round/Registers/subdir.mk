################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/01_Standard/Kernel/Round/Registers/InfoRegCalc.c \
E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/01_Standard/Kernel/Round/Registers/NetworkIdRegCalc.c \
E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/01_Standard/Kernel/Round/Registers/OpeRegCalc.c \
E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/01_Standard/Kernel/Round/Registers/PnPrmCalc.c \
E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/01_Standard/Kernel/Round/Registers/RegAccessIf.c \
E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/01_Standard/Kernel/Round/Registers/UnRegCalc.c 

C_DEPS += \
./00_src/01_Standard/Kernel/Round/Registers/InfoRegCalc.d \
./00_src/01_Standard/Kernel/Round/Registers/NetworkIdRegCalc.d \
./00_src/01_Standard/Kernel/Round/Registers/OpeRegCalc.d \
./00_src/01_Standard/Kernel/Round/Registers/PnPrmCalc.d \
./00_src/01_Standard/Kernel/Round/Registers/RegAccessIf.d \
./00_src/01_Standard/Kernel/Round/Registers/UnRegCalc.d 

OBJS += \
./00_src/01_Standard/Kernel/Round/Registers/InfoRegCalc.o \
./00_src/01_Standard/Kernel/Round/Registers/NetworkIdRegCalc.o \
./00_src/01_Standard/Kernel/Round/Registers/OpeRegCalc.o \
./00_src/01_Standard/Kernel/Round/Registers/PnPrmCalc.o \
./00_src/01_Standard/Kernel/Round/Registers/RegAccessIf.o \
./00_src/01_Standard/Kernel/Round/Registers/UnRegCalc.o 


# Each subdirectory must supply rules for building sources it contributes
00_src/01_Standard/Kernel/Round/Registers/InfoRegCalc.o: E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/01_Standard/Kernel/Round/Registers/InfoRegCalc.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C ����ѥ���'
	armcc --cpu=Cortex-R4F --littleend --thumb --apcs=/interwork --fpu=VFPv3_D16 -DCSW_SGD7W_TYPE -DCSW_NETIF_M3_TYPE -I"../../../../00_src\00_Bsp\\JL086_bsp" -I"../../../../00_src\00_Bsp\\JL086_bsp\CPUPeripheral" -I"../../../../00_src\00_Bsp\\JL086_bsp\usb_function" -I"../../../../00_src\01_Standard\MicroProgram" -I"../../../../00_src\01_Standard\Communications" -I"../../../../00_src\01_Standard\Communications\M3" -I"../../../../00_src\01_Standard\Communications\M3\MLink" -I"../../../../00_src\01_Standard\Communications\M3\Motion" -I"../../../../00_src\01_Standard\BaseControls" -I"../../../../00_src\01_Standard\BaseSequences" -I"../../../../00_src\01_Standard\Kernel\Include" -I"../../../../00_src\01_Standard\Kernel\Lib" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\CortexR4" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\UDL\Servo" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\UDL\M3" -I"../../../../00_src\01_Standard\Kernel\Driver" -I"../../../../00_src\01_Standard\Kernel\Boot\Config" -I../../../../00_src/01_Standard/Kernel/ScanA -I"../../../../00_src\01_Standard\Kernel\ScanB" -I"../../../../00_src\01_Standard\Kernel\ScanC" -I"../../../../00_src\01_Standard\Kernel\Round\Registers" -I"../../../../00_src\01_Standard\Kernel\Round\UtilityFuns" -I"../../../../00_src\01_Standard\Kernel\Round\MsgIf" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\DataTrace" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\AnalogMonitor" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\Registers" -I"../../../../00_src\02_Custom\00_SGD7\05_SGD7W_M3" -O1 -Ospace --feedback="../06_SGD7W_M3.fb" -g --signed_chars --md --depend_format=unix_escaped --no_depend_system_headers --depend_dir="00_src/01_Standard/Kernel/Round/Registers" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/01_Standard/Kernel/Round/Registers/NetworkIdRegCalc.o: E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/01_Standard/Kernel/Round/Registers/NetworkIdRegCalc.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C ����ѥ���'
	armcc --cpu=Cortex-R4F --littleend --thumb --apcs=/interwork --fpu=VFPv3_D16 -DCSW_SGD7W_TYPE -DCSW_NETIF_M3_TYPE -I"../../../../00_src\00_Bsp\\JL086_bsp" -I"../../../../00_src\00_Bsp\\JL086_bsp\CPUPeripheral" -I"../../../../00_src\00_Bsp\\JL086_bsp\usb_function" -I"../../../../00_src\01_Standard\MicroProgram" -I"../../../../00_src\01_Standard\Communications" -I"../../../../00_src\01_Standard\Communications\M3" -I"../../../../00_src\01_Standard\Communications\M3\MLink" -I"../../../../00_src\01_Standard\Communications\M3\Motion" -I"../../../../00_src\01_Standard\BaseControls" -I"../../../../00_src\01_Standard\BaseSequences" -I"../../../../00_src\01_Standard\Kernel\Include" -I"../../../../00_src\01_Standard\Kernel\Lib" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\CortexR4" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\UDL\Servo" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\UDL\M3" -I"../../../../00_src\01_Standard\Kernel\Driver" -I"../../../../00_src\01_Standard\Kernel\Boot\Config" -I../../../../00_src/01_Standard/Kernel/ScanA -I"../../../../00_src\01_Standard\Kernel\ScanB" -I"../../../../00_src\01_Standard\Kernel\ScanC" -I"../../../../00_src\01_Standard\Kernel\Round\Registers" -I"../../../../00_src\01_Standard\Kernel\Round\UtilityFuns" -I"../../../../00_src\01_Standard\Kernel\Round\MsgIf" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\DataTrace" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\AnalogMonitor" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\Registers" -I"../../../../00_src\02_Custom\00_SGD7\05_SGD7W_M3" -O1 -Ospace --feedback="../06_SGD7W_M3.fb" -g --signed_chars --md --depend_format=unix_escaped --no_depend_system_headers --depend_dir="00_src/01_Standard/Kernel/Round/Registers" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/01_Standard/Kernel/Round/Registers/OpeRegCalc.o: E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/01_Standard/Kernel/Round/Registers/OpeRegCalc.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C ����ѥ���'
	armcc --cpu=Cortex-R4F --littleend --thumb --apcs=/interwork --fpu=VFPv3_D16 -DCSW_SGD7W_TYPE -DCSW_NETIF_M3_TYPE -I"../../../../00_src\00_Bsp\\JL086_bsp" -I"../../../../00_src\00_Bsp\\JL086_bsp\CPUPeripheral" -I"../../../../00_src\00_Bsp\\JL086_bsp\usb_function" -I"../../../../00_src\01_Standard\MicroProgram" -I"../../../../00_src\01_Standard\Communications" -I"../../../../00_src\01_Standard\Communications\M3" -I"../../../../00_src\01_Standard\Communications\M3\MLink" -I"../../../../00_src\01_Standard\Communications\M3\Motion" -I"../../../../00_src\01_Standard\BaseControls" -I"../../../../00_src\01_Standard\BaseSequences" -I"../../../../00_src\01_Standard\Kernel\Include" -I"../../../../00_src\01_Standard\Kernel\Lib" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\CortexR4" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\UDL\Servo" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\UDL\M3" -I"../../../../00_src\01_Standard\Kernel\Driver" -I"../../../../00_src\01_Standard\Kernel\Boot\Config" -I../../../../00_src/01_Standard/Kernel/ScanA -I"../../../../00_src\01_Standard\Kernel\ScanB" -I"../../../../00_src\01_Standard\Kernel\ScanC" -I"../../../../00_src\01_Standard\Kernel\Round\Registers" -I"../../../../00_src\01_Standard\Kernel\Round\UtilityFuns" -I"../../../../00_src\01_Standard\Kernel\Round\MsgIf" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\DataTrace" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\AnalogMonitor" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\Registers" -I"../../../../00_src\02_Custom\00_SGD7\05_SGD7W_M3" -O1 -Ospace --feedback="../06_SGD7W_M3.fb" -g --signed_chars --md --depend_format=unix_escaped --no_depend_system_headers --depend_dir="00_src/01_Standard/Kernel/Round/Registers" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/01_Standard/Kernel/Round/Registers/PnPrmCalc.o: E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/01_Standard/Kernel/Round/Registers/PnPrmCalc.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C ����ѥ���'
	armcc --cpu=Cortex-R4F --littleend --thumb --apcs=/interwork --fpu=VFPv3_D16 -DCSW_SGD7W_TYPE -DCSW_NETIF_M3_TYPE -I"../../../../00_src\00_Bsp\\JL086_bsp" -I"../../../../00_src\00_Bsp\\JL086_bsp\CPUPeripheral" -I"../../../../00_src\00_Bsp\\JL086_bsp\usb_function" -I"../../../../00_src\01_Standard\MicroProgram" -I"../../../../00_src\01_Standard\Communications" -I"../../../../00_src\01_Standard\Communications\M3" -I"../../../../00_src\01_Standard\Communications\M3\MLink" -I"../../../../00_src\01_Standard\Communications\M3\Motion" -I"../../../../00_src\01_Standard\BaseControls" -I"../../../../00_src\01_Standard\BaseSequences" -I"../../../../00_src\01_Standard\Kernel\Include" -I"../../../../00_src\01_Standard\Kernel\Lib" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\CortexR4" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\UDL\Servo" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\UDL\M3" -I"../../../../00_src\01_Standard\Kernel\Driver" -I"../../../../00_src\01_Standard\Kernel\Boot\Config" -I../../../../00_src/01_Standard/Kernel/ScanA -I"../../../../00_src\01_Standard\Kernel\ScanB" -I"../../../../00_src\01_Standard\Kernel\ScanC" -I"../../../../00_src\01_Standard\Kernel\Round\Registers" -I"../../../../00_src\01_Standard\Kernel\Round\UtilityFuns" -I"../../../../00_src\01_Standard\Kernel\Round\MsgIf" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\DataTrace" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\AnalogMonitor" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\Registers" -I"../../../../00_src\02_Custom\00_SGD7\05_SGD7W_M3" -O1 -Ospace --feedback="../06_SGD7W_M3.fb" -g --signed_chars --md --depend_format=unix_escaped --no_depend_system_headers --depend_dir="00_src/01_Standard/Kernel/Round/Registers" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/01_Standard/Kernel/Round/Registers/RegAccessIf.o: E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/01_Standard/Kernel/Round/Registers/RegAccessIf.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C ����ѥ���'
	armcc --cpu=Cortex-R4F --littleend --thumb --apcs=/interwork --fpu=VFPv3_D16 -DCSW_SGD7W_TYPE -DCSW_NETIF_M3_TYPE -I"../../../../00_src\00_Bsp\\JL086_bsp" -I"../../../../00_src\00_Bsp\\JL086_bsp\CPUPeripheral" -I"../../../../00_src\00_Bsp\\JL086_bsp\usb_function" -I"../../../../00_src\01_Standard\MicroProgram" -I"../../../../00_src\01_Standard\Communications" -I"../../../../00_src\01_Standard\Communications\M3" -I"../../../../00_src\01_Standard\Communications\M3\MLink" -I"../../../../00_src\01_Standard\Communications\M3\Motion" -I"../../../../00_src\01_Standard\BaseControls" -I"../../../../00_src\01_Standard\BaseSequences" -I"../../../../00_src\01_Standard\Kernel\Include" -I"../../../../00_src\01_Standard\Kernel\Lib" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\CortexR4" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\UDL\Servo" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\UDL\M3" -I"../../../../00_src\01_Standard\Kernel\Driver" -I"../../../../00_src\01_Standard\Kernel\Boot\Config" -I../../../../00_src/01_Standard/Kernel/ScanA -I"../../../../00_src\01_Standard\Kernel\ScanB" -I"../../../../00_src\01_Standard\Kernel\ScanC" -I"../../../../00_src\01_Standard\Kernel\Round\Registers" -I"../../../../00_src\01_Standard\Kernel\Round\UtilityFuns" -I"../../../../00_src\01_Standard\Kernel\Round\MsgIf" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\DataTrace" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\AnalogMonitor" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\Registers" -I"../../../../00_src\02_Custom\00_SGD7\05_SGD7W_M3" -O1 -Ospace --feedback="../06_SGD7W_M3.fb" -g --signed_chars --md --depend_format=unix_escaped --no_depend_system_headers --depend_dir="00_src/01_Standard/Kernel/Round/Registers" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/01_Standard/Kernel/Round/Registers/UnRegCalc.o: E:/18_ZHD_Servo/SGD7_WORK/SGD7Soft/Mercury_Projects_F0090016/00_src/01_Standard/Kernel/Round/Registers/UnRegCalc.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C ����ѥ���'
	armcc --cpu=Cortex-R4F --littleend --thumb --apcs=/interwork --fpu=VFPv3_D16 -DCSW_SGD7W_TYPE -DCSW_NETIF_M3_TYPE -I"../../../../00_src\00_Bsp\\JL086_bsp" -I"../../../../00_src\00_Bsp\\JL086_bsp\CPUPeripheral" -I"../../../../00_src\00_Bsp\\JL086_bsp\usb_function" -I"../../../../00_src\01_Standard\MicroProgram" -I"../../../../00_src\01_Standard\Communications" -I"../../../../00_src\01_Standard\Communications\M3" -I"../../../../00_src\01_Standard\Communications\M3\MLink" -I"../../../../00_src\01_Standard\Communications\M3\Motion" -I"../../../../00_src\01_Standard\BaseControls" -I"../../../../00_src\01_Standard\BaseSequences" -I"../../../../00_src\01_Standard\Kernel\Include" -I"../../../../00_src\01_Standard\Kernel\Lib" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\CortexR4" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\UDL\Servo" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\UDL\M3" -I"../../../../00_src\01_Standard\Kernel\Driver" -I"../../../../00_src\01_Standard\Kernel\Boot\Config" -I../../../../00_src/01_Standard/Kernel/ScanA -I"../../../../00_src\01_Standard\Kernel\ScanB" -I"../../../../00_src\01_Standard\Kernel\ScanC" -I"../../../../00_src\01_Standard\Kernel\Round\Registers" -I"../../../../00_src\01_Standard\Kernel\Round\UtilityFuns" -I"../../../../00_src\01_Standard\Kernel\Round\MsgIf" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\DataTrace" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\AnalogMonitor" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\Registers" -I"../../../../00_src\02_Custom\00_SGD7\05_SGD7W_M3" -O1 -Ospace --feedback="../06_SGD7W_M3.fb" -g --signed_chars --md --depend_format=unix_escaped --no_depend_system_headers --depend_dir="00_src/01_Standard/Kernel/Round/Registers" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

