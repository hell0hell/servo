################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/02_Custom/00_SGD7/00_SpecCommon/Registers/InfoRegTblMercury.c \
E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/02_Custom/00_SGD7/00_SpecCommon/Registers/OpeRegTblMercury.c \
E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/02_Custom/00_SGD7/00_SpecCommon/Registers/TrcRegTblIngram.c \
E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/02_Custom/00_SGD7/00_SpecCommon/Registers/UnRegTblMercury.c 

OBJS += \
./00_src/02_Custom/00_SGD7/00_SpecCommon/Registers/InfoRegTblMercury.o \
./00_src/02_Custom/00_SGD7/00_SpecCommon/Registers/OpeRegTblMercury.o \
./00_src/02_Custom/00_SGD7/00_SpecCommon/Registers/TrcRegTblIngram.o \
./00_src/02_Custom/00_SGD7/00_SpecCommon/Registers/UnRegTblMercury.o 

C_DEPS += \
./00_src/02_Custom/00_SGD7/00_SpecCommon/Registers/InfoRegTblMercury.d \
./00_src/02_Custom/00_SGD7/00_SpecCommon/Registers/OpeRegTblMercury.d \
./00_src/02_Custom/00_SGD7/00_SpecCommon/Registers/TrcRegTblIngram.d \
./00_src/02_Custom/00_SGD7/00_SpecCommon/Registers/UnRegTblMercury.d 


# Each subdirectory must supply rules for building sources it contributes
00_src/02_Custom/00_SGD7/00_SpecCommon/Registers/InfoRegTblMercury.o: E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/02_Custom/00_SGD7/00_SpecCommon/Registers/InfoRegTblMercury.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C コンパイラ'
	armcc --cpu=Cortex-R4F --littleend --thumb --apcs=/interwork --fpu=VFPv3_D16 -DCSW_NETIF_CM_TYPE -Dxxx_NON_EEPROM_ -Dxxx_USB_FUNC_USE_ -DxxxCSW_MICRO_DEBUG_MODE -Dxxx_ASIP_DEBUG_BUF_USE_ -DSVFIF_CMDOPT -D_CMDOPT_OPE_DBG_ -D_CMDOPT_ALM_DBG_ -DMCCSW_SGDVCMOP -I"../../../../00_src\00_Bsp\JL086_bsp" -I"../../../../00_src\00_Bsp\JL086_bsp\CPUPeripheral" -I"../../../../00_src\00_Bsp\\JL086_bsp\usb_function" -I"../../../../00_src\01_Standard\MicroProgram" -I"../../../../00_src\01_Standard\Communications" -I"../../../../00_src\01_Standard\Communications/DPM" -I"../../../../00_src\01_Standard\Communications/DPM/XlsTbl" -I"../../../../00_src\01_Standard\Communications/DPM/Mlink" -I"../../../../00_src\01_Standard\Communications/DPM/Motion" -I"../../../../00_src\01_Standard\BaseControls" -I"../../../../00_src\01_Standard\BaseSequences" -I"../../../../00_src\01_Standard\Kernel\Include" -I"../../../../00_src\01_Standard\Kernel\Lib" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\CortexR4" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\UDL\Servo" -I"../../../../00_src\01_Standard\Kernel\Driver" -I"../../../../00_src\01_Standard\Kernel\Boot\Config" -I"../../../../00_src\01_Standard\Kernel\ScanB" -I"../../../../00_src\01_Standard\Kernel\ScanC" -I"../../../../00_src\01_Standard\Kernel\Round\Registers" -I"../../../../00_src\01_Standard\Kernel\Round\UtilityFuns" -I"../../../../00_src\01_Standard\Kernel\Round\MsgIf" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\DataTrace" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\Registers" -I"../../../../00_src\02_Custom\00_SGD7\04_SGD7S_CM" -I"../../../../00_src\00_Bsp\ARM9_bsp\driver\cpu" -I"../../../../00_src\00_Bsp\ARM9_bsp\driver\intc" -I"../../../../00_src\00_Bsp\ARM9_bsp\include" -O1 -Otime --feedback="../05_SGD7S_CM.fb" -g --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/02_Custom/00_SGD7/00_SpecCommon/Registers/OpeRegTblMercury.o: E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/02_Custom/00_SGD7/00_SpecCommon/Registers/OpeRegTblMercury.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C コンパイラ'
	armcc --cpu=Cortex-R4F --littleend --thumb --apcs=/interwork --fpu=VFPv3_D16 -DCSW_NETIF_CM_TYPE -Dxxx_NON_EEPROM_ -Dxxx_USB_FUNC_USE_ -DxxxCSW_MICRO_DEBUG_MODE -Dxxx_ASIP_DEBUG_BUF_USE_ -DSVFIF_CMDOPT -D_CMDOPT_OPE_DBG_ -D_CMDOPT_ALM_DBG_ -DMCCSW_SGDVCMOP -I"../../../../00_src\00_Bsp\JL086_bsp" -I"../../../../00_src\00_Bsp\JL086_bsp\CPUPeripheral" -I"../../../../00_src\00_Bsp\\JL086_bsp\usb_function" -I"../../../../00_src\01_Standard\MicroProgram" -I"../../../../00_src\01_Standard\Communications" -I"../../../../00_src\01_Standard\Communications/DPM" -I"../../../../00_src\01_Standard\Communications/DPM/XlsTbl" -I"../../../../00_src\01_Standard\Communications/DPM/Mlink" -I"../../../../00_src\01_Standard\Communications/DPM/Motion" -I"../../../../00_src\01_Standard\BaseControls" -I"../../../../00_src\01_Standard\BaseSequences" -I"../../../../00_src\01_Standard\Kernel\Include" -I"../../../../00_src\01_Standard\Kernel\Lib" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\CortexR4" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\UDL\Servo" -I"../../../../00_src\01_Standard\Kernel\Driver" -I"../../../../00_src\01_Standard\Kernel\Boot\Config" -I"../../../../00_src\01_Standard\Kernel\ScanB" -I"../../../../00_src\01_Standard\Kernel\ScanC" -I"../../../../00_src\01_Standard\Kernel\Round\Registers" -I"../../../../00_src\01_Standard\Kernel\Round\UtilityFuns" -I"../../../../00_src\01_Standard\Kernel\Round\MsgIf" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\DataTrace" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\Registers" -I"../../../../00_src\02_Custom\00_SGD7\04_SGD7S_CM" -I"../../../../00_src\00_Bsp\ARM9_bsp\driver\cpu" -I"../../../../00_src\00_Bsp\ARM9_bsp\driver\intc" -I"../../../../00_src\00_Bsp\ARM9_bsp\include" -O1 -Otime --feedback="../05_SGD7S_CM.fb" -g --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/02_Custom/00_SGD7/00_SpecCommon/Registers/TrcRegTblIngram.o: E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/02_Custom/00_SGD7/00_SpecCommon/Registers/TrcRegTblIngram.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C コンパイラ'
	armcc --cpu=Cortex-R4F --littleend --thumb --apcs=/interwork --fpu=VFPv3_D16 -DCSW_NETIF_CM_TYPE -Dxxx_NON_EEPROM_ -Dxxx_USB_FUNC_USE_ -DxxxCSW_MICRO_DEBUG_MODE -Dxxx_ASIP_DEBUG_BUF_USE_ -DSVFIF_CMDOPT -D_CMDOPT_OPE_DBG_ -D_CMDOPT_ALM_DBG_ -DMCCSW_SGDVCMOP -I"../../../../00_src\00_Bsp\JL086_bsp" -I"../../../../00_src\00_Bsp\JL086_bsp\CPUPeripheral" -I"../../../../00_src\00_Bsp\\JL086_bsp\usb_function" -I"../../../../00_src\01_Standard\MicroProgram" -I"../../../../00_src\01_Standard\Communications" -I"../../../../00_src\01_Standard\Communications/DPM" -I"../../../../00_src\01_Standard\Communications/DPM/XlsTbl" -I"../../../../00_src\01_Standard\Communications/DPM/Mlink" -I"../../../../00_src\01_Standard\Communications/DPM/Motion" -I"../../../../00_src\01_Standard\BaseControls" -I"../../../../00_src\01_Standard\BaseSequences" -I"../../../../00_src\01_Standard\Kernel\Include" -I"../../../../00_src\01_Standard\Kernel\Lib" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\CortexR4" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\UDL\Servo" -I"../../../../00_src\01_Standard\Kernel\Driver" -I"../../../../00_src\01_Standard\Kernel\Boot\Config" -I"../../../../00_src\01_Standard\Kernel\ScanB" -I"../../../../00_src\01_Standard\Kernel\ScanC" -I"../../../../00_src\01_Standard\Kernel\Round\Registers" -I"../../../../00_src\01_Standard\Kernel\Round\UtilityFuns" -I"../../../../00_src\01_Standard\Kernel\Round\MsgIf" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\DataTrace" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\Registers" -I"../../../../00_src\02_Custom\00_SGD7\04_SGD7S_CM" -I"../../../../00_src\00_Bsp\ARM9_bsp\driver\cpu" -I"../../../../00_src\00_Bsp\ARM9_bsp\driver\intc" -I"../../../../00_src\00_Bsp\ARM9_bsp\include" -O1 -Otime --feedback="../05_SGD7S_CM.fb" -g --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/02_Custom/00_SGD7/00_SpecCommon/Registers/UnRegTblMercury.o: E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/02_Custom/00_SGD7/00_SpecCommon/Registers/UnRegTblMercury.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C コンパイラ'
	armcc --cpu=Cortex-R4F --littleend --thumb --apcs=/interwork --fpu=VFPv3_D16 -DCSW_NETIF_CM_TYPE -Dxxx_NON_EEPROM_ -Dxxx_USB_FUNC_USE_ -DxxxCSW_MICRO_DEBUG_MODE -Dxxx_ASIP_DEBUG_BUF_USE_ -DSVFIF_CMDOPT -D_CMDOPT_OPE_DBG_ -D_CMDOPT_ALM_DBG_ -DMCCSW_SGDVCMOP -I"../../../../00_src\00_Bsp\JL086_bsp" -I"../../../../00_src\00_Bsp\JL086_bsp\CPUPeripheral" -I"../../../../00_src\00_Bsp\\JL086_bsp\usb_function" -I"../../../../00_src\01_Standard\MicroProgram" -I"../../../../00_src\01_Standard\Communications" -I"../../../../00_src\01_Standard\Communications/DPM" -I"../../../../00_src\01_Standard\Communications/DPM/XlsTbl" -I"../../../../00_src\01_Standard\Communications/DPM/Mlink" -I"../../../../00_src\01_Standard\Communications/DPM/Motion" -I"../../../../00_src\01_Standard\BaseControls" -I"../../../../00_src\01_Standard\BaseSequences" -I"../../../../00_src\01_Standard\Kernel\Include" -I"../../../../00_src\01_Standard\Kernel\Lib" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\CortexR4" -I"../../../../00_src\01_Standard\Kernel\Driver\JL086\UDL\Servo" -I"../../../../00_src\01_Standard\Kernel\Driver" -I"../../../../00_src\01_Standard\Kernel\Boot\Config" -I"../../../../00_src\01_Standard\Kernel\ScanB" -I"../../../../00_src\01_Standard\Kernel\ScanC" -I"../../../../00_src\01_Standard\Kernel\Round\Registers" -I"../../../../00_src\01_Standard\Kernel\Round\UtilityFuns" -I"../../../../00_src\01_Standard\Kernel\Round\MsgIf" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\DataTrace" -I"../../../../00_src\02_Custom\00_SGD7\00_SpecCommon\Registers" -I"../../../../00_src\02_Custom\00_SGD7\04_SGD7S_CM" -I"../../../../00_src\00_Bsp\ARM9_bsp\driver\cpu" -I"../../../../00_src\00_Bsp\ARM9_bsp\driver\intc" -I"../../../../00_src\00_Bsp\ARM9_bsp\include" -O1 -Otime --feedback="../05_SGD7S_CM.fb" -g --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


