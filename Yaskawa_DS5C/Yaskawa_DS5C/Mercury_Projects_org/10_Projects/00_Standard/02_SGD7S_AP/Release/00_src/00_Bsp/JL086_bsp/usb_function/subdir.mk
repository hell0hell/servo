################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/00_Bsp/JL086_bsp/usb_function/u2f_main.c 

OBJS += \
./00_src/00_Bsp/JL086_bsp/usb_function/u2f_main.o 

C_DEPS += \
./00_src/00_Bsp/JL086_bsp/usb_function/u2f_main.d 


# Each subdirectory must supply rules for building sources it contributes
00_src/00_Bsp/JL086_bsp/usb_function/u2f_main.o: E:/Yaskawa_DS5C/Mercury_Projects_org/00_src/00_Bsp/JL086_bsp/usb_function/u2f_main.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C ƒRƒ“ƒpƒCƒ‰'
	armcc -O2 -g --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


