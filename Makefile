CC=arm-none-eabi-gcc
MACH=cortex-m4
CFLAGS= -c -mcpu=$(MACH) -mthumb -std=gnu11 -g3 -Wall \
		-I Application/Inc -I Core/Inc -I Drivers/Inc -I Sensors/Inc \
		-I cmsis_core/CMSIS/Core/Include -I cmsis_f4/Include
LDFLAGS= -mcpu=$(MACH) -mthumb -nostdlib -T linker.ld -Wl,-Map=build/final.map

all: prereq build/gpio.o build/uart.o build/am312.o build/main.o build/startup.o build/final.elf build/final.bin

prereq:
	mkdir -p build

build/gpio.o: Drivers/Src/gpio.c
	$(CC) $(CFLAGS) -o $@ $^

build/uart.o: Drivers/Src/uart.c
	$(CC) $(CFLAGS) -o $@ $^

build/am312.o: Sensors/Src/am312.c
	$(CC) $(CFLAGS) -o $@ $^

build/main.o: Application/Src/main.c
	$(CC) $(CFLAGS) -o $@ $^

build/startup.o: Core/Startup/startup_stm32f446xx.c
	$(CC) $(CFLAGS) -o $@ $^

build/final.elf: build/gpio.o build/uart.o build/am312.o build/main.o build/startup.o
	$(CC) $(LDFLAGS) -o $@ $^

build/final.bin: build/final.elf
	arm-none-eabi-objcopy -O binary $< $@

flash: build/final.bin
	st-flash --reset write $< 0x8000000

clean:
	rm -rf build

cmsis_core:
	git clone --depth 1 -b v6.3.0 https://github.com/ARM-software/CMSIS_6 $@

cmsis_f4:
	git clone --depth 1 -b v2.6.8 https://github.com/STMicroelectronics/cmsis-device-f4 $@
