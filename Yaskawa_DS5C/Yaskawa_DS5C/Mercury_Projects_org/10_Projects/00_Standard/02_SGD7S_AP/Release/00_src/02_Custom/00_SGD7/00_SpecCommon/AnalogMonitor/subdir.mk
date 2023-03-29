################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/02_Custom/00_SGD7/00_SpecCommon/AnalogMonitor/AnalogMonitorCalc.c \
E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/02_Custom/00_SGD7/00_SpecCommon/AnalogMonitor/XlsAmonDef.c 

OBJS += \
./00_src/02_Custom/00_SGD7/00_SpecCommon/AnalogMonitor/AnalogMonitorCalc.o \
./00_src/02_Custom/00_SGD7/00_SpecCommon/AnalogMonitor/XlsAmonDef.o 

C_DEPS += \
./00_src/02_Custom/00_SGD7/00_SpecCommon/AnalogMonitor/AnalogMonitorCalc.d \
./00_src/02_Custom/00_SGD7/00_SpecCommon/AnalogMonitor/XlsAmonDef.d 


# Each subdirectory must supply rules for building sources it contributes
00_src/02_Custom/00_SGD7/00_SpecCommon/AnalogMonitor/AnalogMonitorCalc.o: E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/02_Custom/00_SGD7/00_SpecCommon/AnalogMonitor/AnalogMonitorCalc.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C コンパイラ'
	armcc -O2 -g --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

00_src/02_Custom/00_SGD7/00_SpecCommon/AnalogMonitor/XlsAmonDef.o: E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/02_Custom/00_SGD7/00_SpecCommon/AnalogMonitor/XlsAmonDef.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C コンパイラ'
	armcc -O2 -g --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


