################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BSP/bsp_stk.c 

OBJS += \
./BSP/bsp_stk.o 

C_DEPS += \
./BSP/bsp_stk.d 


# Each subdirectory must supply rules for building sources it contributes
BSP/bsp_stk.o: ../BSP/bsp_stk.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM C Compiler'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -std=c99 '-DHAL_CONFIG=1' '-DSL_WSTK6201A=1' '-DDEBUG=1' '-DEFM32WG990F256=1' -I"C:/Users/SinaR/SimplicityStudio/v4_workspace/EFM32WG-Si446x/hal-config" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//platform/halconfig/inc/hal-config" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/common/halconfig" -I"C:/Users/SinaR/SimplicityStudio/v4_workspace/EFM32WG-Si446x/RadioConfig" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//platform/emdrv/ezradiodrv/si4463/inc" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//platform/emdrv/ezradiodrv/common/inc" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//platform/emdrv/ezradiodrv/common/inc/si4x6x" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//platform/emdrv/ezradiodrv/plugins/inc" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/common" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/common/bsp" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//platform/emdrv/common/inc" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//platform/emdrv/dmadrv/config" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//platform/emdrv/ezradiodrv/config" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//platform/emdrv/nvm/config" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//platform/emdrv/rtcdrv/config" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//platform/emdrv/spidrv/config" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//platform/emdrv/tempdrv/config" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//platform/emdrv/uartdrv/config" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//platform/emdrv/ustimer/config" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//platform/emdrv/dmadrv/inc" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//platform/emdrv/gpiointerrupt/inc" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//platform/emdrv/nvm/inc" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//platform/emdrv/rtcdrv/inc" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//platform/emdrv/sleep/inc" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//platform/emdrv/spidrv/inc" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//platform/emdrv/uartdrv/inc" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//platform/emdrv/ustimer/inc" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//platform/emdrv/tempdrv/inc" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//platform/emlib/inc" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//platform/CMSIS/Include" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/EFM32GG_DK3750/config" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/EFM32WG_STK3800/config" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/SLWSTK6200A_EZR32LG/config" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/SLWSTK6201A_EZR32LG/config" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//platform/Device/SiliconLabs/EFM32WG/Include" -I"E:/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.5//hardware/kit/common/drivers" -O0 -Wall -c -fmessage-length=0 -mno-sched-prolog -fno-builtin -ffunction-sections -fdata-sections -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -MMD -MP -MF"BSP/bsp_stk.d" -MT"BSP/bsp_stk.o" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


