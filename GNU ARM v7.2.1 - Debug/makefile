################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include led/subdir.mk
-include extdev/subdir.mk
-include emlib/subdir.mk
-include emdrv/subdir.mk
-include Drivers/subdir.mk
-include CMSIS/EFM32WG/subdir.mk
-include BSP/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: EFM32WG-Si446x.axf

# Tool invocations
EFM32WG-Si446x.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU ARM C Linker'
	arm-none-eabi-gcc -g -gdwarf-2 -mcpu=cortex-m4 -mthumb -T "EFM32WG-Si446x.ld" -Xlinker --gc-sections -Xlinker -Map="EFM32WG-Si446x.map" -mfpu=fpv4-sp-d16 -mfloat-abi=softfp --specs=nano.specs -o EFM32WG-Si446x.axf "./src/main.o" "./emlib/em_acmp.o" "./emlib/em_adc.o" "./emlib/em_aes.o" "./emlib/em_assert.o" "./emlib/em_burtc.o" "./emlib/em_can.o" "./emlib/em_cmu.o" "./emlib/em_core.o" "./emlib/em_cryotimer.o" "./emlib/em_crypto.o" "./emlib/em_csen.o" "./emlib/em_dac.o" "./emlib/em_dbg.o" "./emlib/em_dma.o" "./emlib/em_ebi.o" "./emlib/em_emu.o" "./emlib/em_gpcrc.o" "./emlib/em_gpio.o" "./emlib/em_i2c.o" "./emlib/em_iadc.o" "./emlib/em_idac.o" "./emlib/em_int.o" "./emlib/em_lcd.o" "./emlib/em_ldma.o" "./emlib/em_lesense.o" "./emlib/em_letimer.o" "./emlib/em_leuart.o" "./emlib/em_mpu.o" "./emlib/em_msc.o" "./emlib/em_opamp.o" "./emlib/em_pcnt.o" "./emlib/em_pdm.o" "./emlib/em_prs.o" "./emlib/em_qspi.o" "./emlib/em_rmu.o" "./emlib/em_rtc.o" "./emlib/em_rtcc.o" "./emlib/em_se.o" "./emlib/em_system.o" "./emlib/em_timer.o" "./emlib/em_usart.o" "./emlib/em_vcmp.o" "./emlib/em_vdac.o" "./emlib/em_wdog.o" "./Drivers/dmactrl.o" "./Drivers/dmadrv.o" "./Drivers/ezradio_api_lib_add.o" "./Drivers/gpiointerrupt.o" "./Drivers/nvm.o" "./Drivers/nvm_hal.o" "./Drivers/rtcdriver.o" "./Drivers/sleep.o" "./Drivers/spidrv.o" "./Drivers/uartdrv.o" "./Drivers/ustimer.o" "./CMSIS/EFM32WG/startup_gcc_efm32wg.o" "./CMSIS/EFM32WG/system_efm32wg.o" "./led/bsp_stk_leds.o" "./extdev/ext-device.o" "./emdrv/spidrv.o" "./Drivers/ezradio_api_lib.o" "./Drivers/ezradio_auto_ack_plugin.o" "./Drivers/ezradio_comm.o" "./Drivers/ezradio_crcerror_plugin.o" "./Drivers/ezradio_direct_receive_plugin.o" "./Drivers/ezradio_direct_transmit_plugin.o" "./Drivers/ezradio_hal.o" "./Drivers/ezradio_plugin_manager.o" "./Drivers/ezradio_pn9_plugin.o" "./Drivers/ezradio_receive_plugin.o" "./Drivers/ezradio_transmit_plugin.o" "./Drivers/ezradio_unmod_carrier_plugin.o" "./Drivers/tempdrv.o" "./BSP/bsp_stk.o" -Wl,--start-group -lgcc -lc -lnosys -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

	@echo 'Building hex file: EFM32WG-Si446x.hex'
	arm-none-eabi-objcopy -O ihex "EFM32WG-Si446x.axf" "EFM32WG-Si446x.hex"
	@echo ' '

	@echo 'Building bin file: EFM32WG-Si446x.bin'
	arm-none-eabi-objcopy -O binary "EFM32WG-Si446x.axf" "EFM32WG-Si446x.bin"
	@echo ' '

	@echo 'Building s37 file: EFM32WG-Si446x.s37'
	arm-none-eabi-objcopy -O srec "EFM32WG-Si446x.axf" "EFM32WG-Si446x.s37"
	@echo ' '

	@echo 'Running size tool'
	arm-none-eabi-size "EFM32WG-Si446x.axf"
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) EFM32WG-Si446x.axf
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
