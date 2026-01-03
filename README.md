# Force-Controlled LED Bar + Buzzer (STM32 Cortex-M4)

A bare-metal firmware project that reads a force sensor (via ADC) and turns on a 10-step LED bar while sweeping a buzzer tone. Higher force increases both the number of lit LEDs and the PWM duty cycle driving the buzzer.

## Features
- ADC sampling with simple averaging for stability.
- 10-step LED bar visualization.
- PWM output for a buzzer with duty cycle tied to sensor input.
- UART logging for quick debugging.

## Hardware
- Board: B-L475E-IOT01A Discovery kit
- Microcontroller: STM32L475VGT6 (Cortex-M4)
- Force sensor: FSR402 on `A0` (PC5)
- LED bar: SHB10R on `D0`..`D10`
- Buzzer: passive buzzer on `D9` (PA15, TIM2_CH1)

## Compatibility
This firmware targets the STM32L475VGT6 on the B-L475E-IOT01A board and uses MCU-specific register addresses and pin mappings. It is not portable to other STM32 families or boards without updating the low-level drivers, pin map, and clock configuration.

## Pin Map (from `include/helper.h`)
- ADC input: `A0` (PC5)
- Buzzer PWM: `D9` (PA15, TIM2_CH1)
- LED bar: `D0`..`D10` (PA1, PA0, PD14, PB0, PA3, PB4, PB1, PA4, PB2, PA2)
- UART: configured in code as UART1 on PB6/PB7

## Prerequisites
You need a working ARM embedded toolchain and ST-Link tools.

Required:
- ARM GCC toolchain (`arm-none-eabi-gcc`)
- GNU Make
- ST-Link tools (`st-flash`)
- Git (to clone the repo)

Optional but useful:
- A serial terminal (e.g., `screen`, PuTTY, or minicom) to view UART logs

## Install Tools

### macOS (Homebrew)
```sh
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
brew install arm-none-eabi-gcc make stlink git
```

### Ubuntu / Debian
```sh
sudo apt update
sudo apt install -y gcc-arm-none-eabi make stlink-tools git
```

### Windows
1) Install the ARM GCC toolchain:
   - Download and install `gcc-arm-none-eabi` from Arm.
   - During install, enable "Add path to environment variable".
2) Install GNU Make:
   - Download a Windows build of Make and add it to your `PATH`.
3) Install ST-Link tools:
   - Download `stlink` and add `st-flash` to your `PATH`.
4) Install Git:
   - Download from https://git-scm.com/download/win and enable "Add to PATH".

Verify installs:
```sh
arm-none-eabi-gcc --version
make --version
st-flash --version
git --version
```

## Build
```sh
make
```

Outputs:
- `firmware.elf`
- `firmware.bin`

## Flash
```sh
make flash
```

## Run
Connect the FSR402 to `A0` in a voltage divider and apply force. The LED bar fills with more segments as force increases, and the buzzer duty cycle rises. UART prints the current duty cycle value.

## Notes
- The PWM base is set in `setup_pwm()` (see `src/timer.c`).
- ADC values are scaled to 0..100 and mapped to 10 LED steps.



## References
- https://github.com/cpq/bare-metal-programming-guide?tab=readme-ov-file
