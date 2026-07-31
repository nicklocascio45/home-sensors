CC=arm-none-eabi-gcc
MACH=cortex-m4
CFLAGS= -c -mcpu=$(MACH) -mthumb -std=gnu11 -g3 -Wall -I Application/Inc -I Core/Inc -I Drivers/Inc -I Sensors/Inc
LDFLAGS= -mcpu=$(MACH) -mthumb -nostdlib -T linker.ld -Wl,-Map=build/final.map

all: prereq build/gpio.o build/main.o build/startup.o build/final.elf build/final.bin

prereq:
	mkdir -p build

build/gpio.o: Drivers/Src/gpio.c
	$(CC) $(CFLAGS) -o $@ $^

build/main.o: Application/Src/main.c
	$(CC) $(CFLAGS) -o $@ $^

build/startup.o: Core/Startup/startup_stm32f446xx.c
	$(CC) $(CFLAGS) -o $@ $^

build/final.elf: build/gpio.o build/main.o build/startup.o
	$(CC) $(LDFLAGS) -o $@ $^

build/final.bin: build/final.elf
	arm-none-eabi-objcopy -O binary $< $@

flash: build/final.bin
	st-flash --reset write $< 0x8000000

clean:
	rm -rf build
