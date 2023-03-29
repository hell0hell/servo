################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := clean

# All of the sources participating in the build are defined here
-include sources.mk
-include 00_src/02_Custom/00_SGD7/04_SGD7S_CM/subdir.mk
-include 00_src/02_Custom/00_SGD7/00_SpecCommon/Registers/subdir.mk
-include 00_src/02_Custom/00_SGD7/00_SpecCommon/DataTrace/subdir.mk
-include 00_src/01_Standard/Kernel/ScanC/subdir.mk
-include 00_src/01_Standard/Kernel/ScanB/subdir.mk
-include 00_src/01_Standard/Kernel/ScanA/subdir.mk
-include 00_src/01_Standard/Kernel/Round/UtilityFuns/subdir.mk
-include 00_src/01_Standard/Kernel/Round/Registers/subdir.mk
-include 00_src/01_Standard/Kernel/Round/OpeIf/subdir.mk
-include 00_src/01_Standard/Kernel/Round/MsgIf/subdir.mk
-include 00_src/01_Standard/Kernel/Round/Core/subdir.mk
-include 00_src/01_Standard/Kernel/Round/Apl/RegEdit/subdir.mk
-include 00_src/01_Standard/Kernel/Round/Apl/PrmDownload/subdir.mk
-include 00_src/01_Standard/Kernel/Round/Apl/Jog/subdir.mk
-include 00_src/01_Standard/Kernel/Round/Apl/AnlgRefManAdj/subdir.mk
-include 00_src/01_Standard/Kernel/Round/Apl/AnlgRefAutoAdj/subdir.mk
-include 00_src/01_Standard/Kernel/Round/Apl/AlmTrc/subdir.mk
-include 00_src/01_Standard/Kernel/Round/subdir.mk
-include 00_src/01_Standard/Kernel/Lib/subdir.mk
-include 00_src/01_Standard/Kernel/Driver/JL086/UDL/Servo/subdir.mk
-include 00_src/01_Standard/Kernel/Driver/JL086/CortexR4/subdir.mk
-include 00_src/01_Standard/Kernel/Driver/subdir.mk
-include 00_src/01_Standard/Kernel/Boot/Config/subdir.mk
-include 00_src/01_Standard/Kernel/Boot/subdir.mk
-include 00_src/01_Standard/Communications/DPM/Motion/subdir.mk
-include 00_src/01_Standard/Communications/DPM/Mlink/subdir.mk
-include 00_src/01_Standard/Communications/DPM/subdir.mk
-include 00_src/01_Standard/BaseSequences/subdir.mk
-include 00_src/01_Standard/BaseControls/subdir.mk
-include 00_src/00_Bsp/JL086_bsp/usb_function/subdir.mk
-include 00_src/00_Bsp/JL086_bsp/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: ../../00_Products/Bin/05_SGD7S_CM.axf

# Tool invocations
../../00_Products/Bin/05_SGD7S_CM.axf: $(OBJS) ..\..\..\..\00_src\02_Custom\00_SGD7\04_SGD7S_CM\Sgd7S_CM.scat $(USER_OBJS) $(LIBS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM ƒŠƒ“ƒJ'
	armlink --cpu=Cortex-R4F --fpu=VFPv3_D16 --entry=MercuryMain --scatter="..\..\..\..\00_src\02_Custom\00_SGD7\04_SGD7S_CM\Sgd7S_CM.scat" --feedback="../05_SGD7S_CM.fb" --callgraph --callgraph_file="../../00_Products/Bin/05_SGD7S_CM.html" --map --info=sizes --info=totals --info=stack --list="../../00_Products/Bin/05_SGD7S_CM.list" -o "../../00_Products/Bin/05_SGD7S_CM.axf" "--via=makefile.armlink.inputs"
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) 00_src/02_Custom/00_SGD7/04_SGD7S_CM/M2IdRegTblMercury.o 00_src/02_Custom/00_SGD7/04_SGD7S_CM/PrmTblMercury.o 00_src/02_Custom/00_SGD7/00_SpecCommon/Registers/InfoRegTblMercury.o 00_src/02_Custom/00_SGD7/00_SpecCommon/Registers/OpeRegTblMercury.o 00_src/02_Custom/00_SGD7/00_SpecCommon/Registers/TrcRegTblIngram.o 00_src/02_Custom/00_SGD7/00_SpecCommon/Registers/UnRegTblMercury.o 00_src/02_Custom/00_SGD7/00_SpecCommon/DataTrace/DataTraceCalc.o 00_src/02_Custom/00_SGD7/00_SpecCommon/DataTrace/XlsTrcDef.o 00_src/01_Standard/Kernel/ScanC/AlarmManager.o 00_src/01_Standard/Kernel/ScanC/AlarmTable.o 00_src/01_Standard/Kernel/ScanC/IntScanC.o 00_src/01_Standard/Kernel/ScanB/DataTraceManager.o 00_src/01_Standard/Kernel/ScanB/IntScanB.o 00_src/01_Standard/Kernel/ScanA/IntScanA.o 00_src/01_Standard/Kernel/ScanA/MotorLessTest.o 00_src/01_Standard/Kernel/Round/UtilityFuns/AdvancedAutoTuning.o 00_src/01_Standard/Kernel/Round/UtilityFuns/AutoNotchSet.o 00_src/01_Standard/Kernel/Round/UtilityFuns/FnAbsScaleSet.o 00_src/01_Standard/Kernel/Round/UtilityFuns/FnAtypeVibTune.o 00_src/01_Standard/Kernel/Round/UtilityFuns/FnDCVoltageAdj.o 00_src/01_Standard/Kernel/Round/UtilityFuns/FnEasyFFT.o 00_src/01_Standard/Kernel/Round/UtilityFuns/FnInitVibDetLv.o 00_src/01_Standard/Kernel/Round/UtilityFuns/FnJatOffLine.o 00_src/01_Standard/Kernel/Round/UtilityFuns/FnJog.o 00_src/01_Standard/Kernel/Round/UtilityFuns/FnMfcTuning.o 00_src/01_Standard/Kernel/Round/UtilityFuns/FnMotorCurrentAdj.o 00_src/01_Standard/Kernel/Round/UtilityFuns/FnMotorPrmDownload.o 00_src/01_Standard/Kernel/Round/UtilityFuns/FnMultiTurnSet.o 00_src/01_Standard/Kernel/Round/UtilityFuns/FnOnePrmTuning.o 00_src/01_Standard/Kernel/Round/UtilityFuns/FnPrgJog.o 00_src/01_Standard/Kernel/Round/UtilityFuns/FnSetTuneLessLvl.o 00_src/01_Standard/Kernel/Round/UtilityFuns/FnSysConfig.o 00_src/01_Standard/Kernel/Round/UtilityFuns/FnTableCommand.o 00_src/01_Standard/Kernel/Round/UtilityFuns/FnVibMonitor.o 00_src/01_Standard/Kernel/Round/UtilityFuns/FunListDef.o 00_src/01_Standard/Kernel/Round/UtilityFuns/FunManager.o 00_src/01_Standard/Kernel/Round/UtilityFuns/OnlineFFT.o 00_src/01_Standard/Kernel/Round/UtilityFuns/TuneLib.o 00_src/01_Standard/Kernel/Round/Round.o 00_src/01_Standard/Kernel/Round/Registers/InfoRegCalc.o 00_src/01_Standard/Kernel/Round/Registers/NetworkIdRegCalc.o 00_src/01_Standard/Kernel/Round/Registers/OpeRegCalc.o 00_src/01_Standard/Kernel/Round/Registers/PnPrmCalc.o 00_src/01_Standard/Kernel/Round/Registers/RegAccessIf.o 00_src/01_Standard/Kernel/Round/Registers/UnRegCalc.o 00_src/01_Standard/Kernel/Round/OpeIf/LcdOpFun.o 00_src/01_Standard/Kernel/Round/OpeIf/LcdOpMain.o 00_src/01_Standard/Kernel/Round/OpeIf/LcdOpPrmMon.o 00_src/01_Standard/Kernel/Round/OpeIf/PnlOp.o 00_src/01_Standard/Kernel/Round/MsgIf/FirmDownload.o 00_src/01_Standard/Kernel/Round/MsgIf/MemobusFunctions.o 00_src/01_Standard/Kernel/Round/MsgIf/MsgMngr.o 00_src/01_Standard/Kernel/Round/MsgIf/SigmaMemobus.o 00_src/01_Standard/Kernel/Round/Core/FunMngr.o 00_src/01_Standard/Kernel/Round/Core/InfMngr.o 00_src/01_Standard/Kernel/Round/Core/MemMngr.o 00_src/01_Standard/Kernel/Round/Core/MonMngr.o 00_src/01_Standard/Kernel/Round/Core/ObjMngr.o 00_src/01_Standard/Kernel/Round/Core/OpeMngr.o 00_src/01_Standard/Kernel/Round/Core/PrmMngr.o 00_src/01_Standard/Kernel/Round/Apl/RegEdit/RegEdit.o 00_src/01_Standard/Kernel/Round/Apl/PrmDownload/PrmFileDownload.o 00_src/01_Standard/Kernel/Round/Apl/Jog/FunJog.o 00_src/01_Standard/Kernel/Round/Apl/AnlgRefManAdj/AnlgRefManAdj.o 00_src/01_Standard/Kernel/Round/Apl/AnlgRefAutoAdj/AnlgRefAutoAdj.o 00_src/01_Standard/Kernel/Round/Apl/AlmTrc/FunAlmTrc.o 00_src/01_Standard/Kernel/Lib/KLib.o 00_src/01_Standard/Kernel/Lib/MLib.o 00_src/01_Standard/Kernel/Driver/EepDefConfig.o 00_src/01_Standard/Kernel/Driver/EepromIF.o 00_src/01_Standard/Kernel/Driver/EepromIF_PrmDownload.o 00_src/01_Standard/Kernel/Driver/FlashIF.o 00_src/01_Standard/Kernel/Driver/MotorPrmCfg.o 00_src/01_Standard/Kernel/Driver/NetworkIf.o 00_src/01_Standard/Kernel/Driver/SencScanService.o 00_src/01_Standard/Kernel/Driver/SifMbus.o 00_src/01_Standard/Kernel/Driver/UartMsgIF.o 00_src/01_Standard/Kernel/Driver/UsbMsgIF.o 00_src/01_Standard/Kernel/Driver/JL086/UDL/Servo/JL086ServoIpHw.o 00_src/01_Standard/Kernel/Driver/JL086/UDL/Servo/SerialEncIf.o 00_src/01_Standard/Kernel/Driver/JL086/CortexR4/DetectTemperature.o 00_src/01_Standard/Kernel/Driver/JL086/CortexR4/Dma.o 00_src/01_Standard/Kernel/Driver/JL086/CortexR4/Eeprom.o 00_src/01_Standard/Kernel/Driver/JL086/CortexR4/HwDeviceIf.o 00_src/01_Standard/Kernel/Driver/JL086/CortexR4/SPI_EepromDrv.o 00_src/01_Standard/Kernel/Driver/JL086/CortexR4/Uart.o 00_src/01_Standard/Kernel/Driver/JL086/CortexR4/uDMATransfer.o 00_src/01_Standard/Kernel/Boot/MercuryMain.o 00_src/01_Standard/Kernel/Boot/Config/IniPrmCalc.o 00_src/01_Standard/Kernel/Boot/Config/MercuryGlobals.o 00_src/01_Standard/Kernel/Boot/Config/SystemConfigulation.o 00_src/01_Standard/Communications/DPM/DpmCtrl.o 00_src/01_Standard/Communications/DPM/DpmOpTasks.o 00_src/01_Standard/Communications/DPM/DpmRegAcc.o 00_src/01_Standard/Communications/DPM/M2CfgTbl.o 00_src/01_Standard/Communications/DPM/TaskCfg.o 00_src/01_Standard/Communications/DPM/Motion/AccFilter.o 00_src/01_Standard/Communications/DPM/Motion/MtApi.o 00_src/01_Standard/Communications/DPM/Motion/MtCtrl.o 00_src/01_Standard/Communications/DPM/Motion/MtGenerator.o 00_src/01_Standard/Communications/DPM/Motion/MtPRM.o 00_src/01_Standard/Communications/DPM/Motion/PRMBank.o 00_src/01_Standard/Communications/DPM/Motion/Position.o 00_src/01_Standard/Communications/DPM/Mlink/CmdAnalyzer.o 00_src/01_Standard/Communications/DPM/Mlink/CmdProc.o 00_src/01_Standard/Communications/DPM/Mlink/CmdTable.o 00_src/01_Standard/Communications/DPM/Mlink/ComAlmMngr.o 00_src/01_Standard/Communications/DPM/Mlink/ComManager.o 00_src/01_Standard/Communications/DPM/Mlink/ComRegMngr.o 00_src/01_Standard/Communications/DPM/Mlink/RspManager.o 00_src/01_Standard/Communications/DPM/Mlink/RspProc.o 00_src/01_Standard/Communications/DPM/Mlink/UtilityLib.o 00_src/01_Standard/BaseSequences/AlmLatch.o 00_src/01_Standard/BaseSequences/BaseEnableSequence.o 00_src/01_Standard/BaseSequences/CheckAlarm.o 00_src/01_Standard/BaseSequences/CheckMotSts.o 00_src/01_Standard/BaseSequences/LifeTimePrediction.o 00_src/01_Standard/BaseSequences/OverheatManager.o 00_src/01_Standard/BaseSequences/PowerManager.o 00_src/01_Standard/BaseSequences/RemVibFreq.o 00_src/01_Standard/BaseSequences/SequenceIo.o 00_src/01_Standard/BaseSequences/SequenceMain.o 00_src/01_Standard/BaseControls/BacklashComp.o 00_src/01_Standard/BaseControls/BaseControlMain.o 00_src/01_Standard/BaseControls/BaseControlPrmCalc.o 00_src/01_Standard/BaseControls/BaseControls.o 00_src/01_Standard/BaseControls/BaseLoop.o 00_src/01_Standard/BaseControls/BprmCalc.o 00_src/01_Standard/BaseControls/CtrlPrmTransducer.o 00_src/01_Standard/BaseControls/ExControls.o 00_src/01_Standard/BaseControls/ExCtrlPrmCalc.o 00_src/01_Standard/BaseControls/FieldWeakening.o 00_src/01_Standard/BaseControls/GainChange.o 00_src/01_Standard/BaseControls/JatOffLine.o 00_src/01_Standard/BaseControls/MicroCyclicIf.o 00_src/01_Standard/BaseControls/MicroPrmCalc.o 00_src/01_Standard/BaseControls/ModelControl.o 00_src/01_Standard/BaseControls/MotorPoleFind.o 00_src/01_Standard/BaseControls/PosManager.o 00_src/01_Standard/BaseControls/ProgramJog.o 00_src/01_Standard/BaseControls/ResVibCtrl.o 00_src/01_Standard/BaseControls/TableDrive.o 00_src/00_Bsp/JL086_bsp/usb_function/u2f_main.o 00_src/00_Bsp/JL086_bsp/Interrupt.o 00_src/00_Bsp/JL086_bsp/InterruptTbl.o 00_src/00_Bsp/JL086_bsp/Interrupts.o 00_src/00_Bsp/JL086_bsp/JL086Core.o 00_src/00_Bsp/JL086_bsp/SFlash.o 00_src/00_Bsp/JL086_bsp/Stubs.o 00_src/00_Bsp/JL086_bsp/UsbFuncDriver.o 00_src/00_Bsp/JL086_bsp/nppfahbmemsramsp1.o " $(C++_SRCS:../%.c++=%.d)" " $(ASM_SRCS:../%.asm=%.d)" " $(C_SRCS:../%.c=%.d)" " $(CC_SRCS:../%.cc=%.d)" " $(ASM_UPPER_SRCS:../%.ASM=%.d)" " $(CPP_SRCS:../%.cpp=%.d)" 
	-$(RM) " $(S_SRCS:../%.s=%.d)" " $(CXX_SRCS:../%.cxx=%.d)" " $(C_UPPER_SRCS:../%.C=%.d)" " $(S_UPPER_SRCS:../%.S=%.d)" ../../00_Products/Bin/05_SGD7S_CM.axf 
	-@echo ' '

post-build:
	-..\makeBin.bat
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
