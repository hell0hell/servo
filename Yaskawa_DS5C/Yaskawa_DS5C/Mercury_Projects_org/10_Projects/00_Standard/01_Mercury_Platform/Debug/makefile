################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := clean

# All of the sources participating in the build are defined here
-include sources.mk
-include 00_src/01_Standard/Kernel/Round/MsgIf/subdir.mk
-include 00_src/01_Standard/Kernel/Lib/subdir.mk
-include 00_src/01_Standard/Kernel/Driver/minizip/zlib/subdir.mk
-include 00_src/01_Standard/Kernel/Driver/minizip/memmng/subdir.mk
-include 00_src/01_Standard/Kernel/Driver/minizip/subdir.mk
-include 00_src/01_Standard/Kernel/Driver/JL086/UDL/M3/subdir.mk
-include 00_src/01_Standard/Kernel/Driver/JL086/CortexR4/subdir.mk
-include 00_src/01_Standard/Kernel/Driver/subdir.mk
-include 00_src/01_Standard/Kernel/Boot/MicroProgram/subdir.mk
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
all: ../../00_Products/Bin/01_Mercury_Platform

# Tool invocations
../../00_Products/Bin/01_Mercury_Platform: $(OBJS) ..\..\..\..\00_src\02_Custom\00_SGD7\00_SpecCommon\Sgd7SBoot.scat $(USER_OBJS) $(LIBS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM ƒŠƒ“ƒJ'
	armlink --cpu=Cortex-R4F --fpu=VFPv3_D16 --entry=Initialize --scatter="..\..\..\..\00_src\02_Custom\00_SGD7\00_SpecCommon\Sgd7SBoot.scat" --keep=MpConstTbl.o(MICRO_TABLE_*) --keep=MprgJL086.o(MICRO_TABLE_*) --keep=MprgJL086_w.o(MICRO_W_TABLE_*) --keep=jl100micro.o(M3_MICRO_TABLE_*) --feedback="../01_Mercury_Platform.fb" --callgraph --callgraph_file="../../00_Products/Bin/01_Mercury_Platform.html" --map --info=sizes --info=totals --list="../../00_Products/Bin/01_Mercury_Platform.list" -o "../../00_Products/Bin/01_Mercury_Platform" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(OBJS) $(C++_DEPS) $(ASM_DEPS) $(C_DEPS) $(CC_DEPS) $(ASM_UPPER_DEPS) $(CPP_DEPS) $(S_DEPS) $(EXECUTABLES) $(CXX_DEPS) $(C_UPPER_DEPS) $(S_UPPER_DEPS) ../../00_Products/Bin/01_Mercury_Platform 
	-@echo ' '

post-build:
	-..\makeBin.bat
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
