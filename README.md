# STM32L475 Bare-Metal Firmware — Force Sensor + LED Bar + Buzzer

[![Language: C99](https://img.shields.io/badge/Language-C99-blue.svg?style=flat-square&logo=c&logoColor=white)](https://en.wikipedia.org/wiki/C99)
[![Architecture: ARM Cortex-M4](https://img.shields.io/badge/Architecture-ARM%20Cortex--M4-orange.svg?style=flat-square&logo=arm&logoColor=white)](https://developer.arm.com/Processors/Cortex-M4)
[![MCU: STM32L475VGT6](https://img.shields.io/badge/MCU-STM32L475VGT6-002B49.svg?style=flat-square&logo=stmicroelectronics&logoColor=white)](https://www.st.com/en/microcontrollers-microprocessors/stm32l475vg.html)
[![Board: B-L475E-IOT01A](https://img.shields.io/badge/Board-B--L475E--IOT01A-blueviolet.svg?style=flat-square)](https://www.st.com/en/evaluation-tools/b-l475e-iot01a.html)
[![Build: GNU Make](https://img.shields.io/badge/Build-GNU%20Make-brightgreen.svg?style=flat-square&logo=gnu&logoColor=white)](https://www.gnu.org/software/make/)
[![Toolchain: arm-none-eabi-gcc](https://img.shields.io/badge/Toolchain-arm--none--eabi--gcc-informational.svg?style=flat-square)](https://developer.arm.com/downloads/-/gnu-rm)
[![Flash: st-flash](https://img.shields.io/badge/Flashing-st--flash%20(ST--Link)-yellow.svg?style=flat-square)](https://github.com/stlink-org/stlink)

A production-grade, bare-metal C firmware engine running directly on silicon without vendor HAL or RTOS abstractions—featuring asynchronous register-level ADC sampling, quadratic sensor linearization, non-blocking ring-buffered UART telemetry, and hardware PWM audio-visual transducer control on an ARM Cortex-M4.

---

## Table of Contents

1. [Project Overview](#project-overview)
2. [Key Skills Demonstrated](#key-skills-demonstrated)
3. [System Architecture](#system-architecture)
4. [Hardware Configuration & Pinout](#hardware-configuration--pinout)
5. [Driver & Subsystem Implementation](#driver--subsystem-implementation)
6. [Engineering Design Decisions](#engineering-design-decisions)
   - [Non-Blocking ADC Initialization State Machine](#1-non-blocking-adc-initialization-state-machine)
   - [Circular Ring Buffer for UART Telemetry](#2-circular-ring-buffer-for-uart-telemetry)
   - [FSR402 Quadratic Scaling & Deadzone Modeling](#3-fsr402-quadratic-scaling--deadzone-modeling)
7. [Building & Compilation](#building--compilation)
8. [Target Flashing](#target-flashing)
9. [UART Telemetry & Debugging](#uart-telemetry--debugging)
10. [Repository Structure](#repository-structure)
11. [References & Technical Documentation](#references--technical-documentation)

---

## Project Overview

This repository contains bare-metal firmware developed in C99 for the **STMicroelectronics STM32L475VGT6** (ARM Cortex-M4 with FPU, 80 MHz) on the **B-L475E-IOT01A Discovery Kit**. 

The application reads an **Interlink Electronics FSR402** force-sensitive resistor through a 12-bit Successive Approximation Register (SAR) Analog-to-Digital Converter (ADC), conditions the signal with multi-sample averaging and mathematical curve linearization, and maps the resulting force magnitude to:
1. **A 10-segment LED bar graph display (Lite-On SHB10R)** providing immediate visual force feedback.
2. **A variable-duty hardware PWM output on General-Purpose Timer 2 (TIM2)** driving a passive piezoelectric transducer for acoustic feedback.
3. **A high-speed non-blocking UART telemetry link (115200 baud)** transmitting calibrated actuation duty-cycle values to a host console.

### What Makes This Firmware Technically Distinct

Unlike standard microcontroller reference designs that rely on heavy vendor abstractions (such as STM32Cube HAL, LL drivers, mbed OS, or Arduino cores), this project is engineered **entirely from raw silicon primitives**:
- **Zero Third-Party Middleware:** Contains a custom GNU linker script (`link.ld`) and C vector table initialization (`startup.c`). Execution begins directly at the hardware reset vector (`_reset`).
- **Deterministic Cooperative Multitasking:** Implements an asynchronous event loop driven by the ARM Cortex-M SysTick core timer. Peripheral initialization, sampling sequences, and serial writes are split into discrete non-blocking states to eliminate CPU busy-wait spinning.
- **Hardware-Accurate Transducer Linearization:** Translates the non-linear hyperbolic conductance curve of piezoresistive polymer sensors into an intuitive, linear tactile response by combining a quadratic transfer function with a 30% noise-floor rejection threshold.

---

## Key Skills Demonstrated

Hiring managers and technical leads evaluate embedded systems portfolios for deep, low-level hardware mastery and robust software design. This project highlights:

- **Bare-metal C (no HAL, no RTOS):** Direct C99 firmware execution targeting bare silicon with zero vendor libraries, runtime wrappers, or operating system dependencies.
- **Direct ARM Cortex-M register manipulation:** Configuration of memory-mapped control and status registers (`RCC`, `GPIO`, `ADC`, `TIM2`, `UART`, `SysTick`) using custom type-safe structures, explicit bitwise masks, and `volatile` access semantics.
- **Non-blocking peripheral drivers (ADC, UART, PWM):** Asynchronous, poll-based driver interfaces that maintain continuous control-loop throughput without task starvation or CPU stalls.
- **ADC state-machine initialization:** Multi-stage asynchronous hardware sequencing (`IDLE` &rarr; `WAIT_REG` &rarr; `WAIT_CAL` &rarr; `WAIT_RDY` &rarr; `DONE`) satisfying internal voltage regulator setup and hardware calibration times without blocking delays.
- **SysTick-based software timers:** Millisecond-resolution software timer scheduler (`timer_expired`) running off a 1 kHz core SysTick interrupt with arithmetic overflow handling.
- **Circular ring buffer for UART TX:** 256-byte lockless FIFO ring buffer decoupled from the hardware transmit register (`TDR`) and status flags (`ISR_TXE`), preventing serial telemetry from throttling real-time I/O.
- **Custom linker script and startup code:** Memory layout specification (`FLASH` at `0x08000000`, `SRAM` at `0x20000000`), vector table linking (`.vectors`), runtime memory initialization (`.bss` zeroing, `.data` LMA-to-VMA copy), and heap limit tracking (`_end`).
- **ARM GCC toolchain + Makefile build system:** Professional compilation flags (`-Wall -Wextra -Werror -Wconversion -Wshadow -ffunction-sections -fdata-sections -Os`), dead-code elimination via linker garbage collection (`--gc-sections`), and artifact generation (`.elf`, `.bin`, `.map`).
- **ST-Link flashing workflow:** Target firmware deployment and board reset automation using the open-source `st-flash` utility.
- **FSR402 non-linear sensor characterization (quadratic scaling + deadzone):** Mathematical modeling of piezoresistive physical dynamics, incorporating quadratic polynomial expansion and a 30% baseline threshold to reject mechanical resting creep.

---

## System Architecture

The firmware executes as an asynchronous super-loop scheduled via a 1 ms monotonic SysTick counter. All sensing, digital filtering, actuator driving, and telemetry transmission execute cooperatively without blocking calls.

```
 +---------------------------------------------------------------------------------------------------+
 |                                   STM32L475VGT6 BARE-METAL FIRMWARE                               |
 +---------------------------------------------------------------------------------------------------+
                                                   |
                                                   v
                                       +-----------------------+
                                       | Hardware Reset Vector |
                                       | _reset() (startup.c)  |
                                       +-----------------------+
                                                   |
                         +-------------------------+-------------------------+
                         | Initialize .data from Flash LMA -> SRAM VMA       |
                         | Zero-fill uninitialized .bss segment              |
                         | Set Main Stack Pointer (MSP) to _estack (96 KB)   |
                         +-------------------------+-------------------------+
                                                   |
                                                   v
                                          +-----------------+
                                          |   main() Entry  |
                                          +-----------------+
                                                   |
      +--------------------------------------------+--------------------------------------------+
      | Peripheral Initializations:                                                             |
      | - UART1 (PB6/PB7) @ 115200 Baud (RCC_APB2ENR)                                           |
      | - SysTick Core Timer @ 1 kHz Tick Rate (CLOCK / 1000)                                   |
      | - Async ADC1 State Machine Start on Pin A0 (PC5, ASCR, ADC_CR_ADVREGEN)                 |
      | - TIM2 PWM Setup on Pin D9 (PA15, AF1 TIM2_CH1, ARR=1000, PSC=4)                       |
      | - 10-LED Bar GPIO Configuration (D0..D10 Output Push-Pull)                              |
      +--------------------------------------------+--------------------------------------------+
                                                   |
                                                   v
 +====================================== NON-BLOCKING EVENT LOOP ===================================+
 |                                                                                                  |
 |  [Tick Time: now = s_ticks]                                                                      |
 |                                                                                                  |
 |  1. ADC Async Initialization Poll                                                                |
 |     +-------------------------------------------------------------------------------------+      |
 |     | IDLE -> WAIT_REG (1ms) -> WAIT_CAL (ADCAL) -> WAIT_RDY (ADRDY) -> DONE (adc_init)    |      |
 |     +-------------------------------------------------------------------------------------+      |
 |                                                                                                  |
 |  2. Periodic ADC Sampling Engine (Period: 2 ms)                                                  |
 |     +-------------------+          +-------------------+          +----------------------+       |
 |     |  timer_expired?   | --Yes--> | Trigger Conversion| --Poll-> | Sample Conversion    |       |
 |     |  (adc_timer, 2ms) |          | ADC->CR |= BIT(2) |          | Ready (ADC_ISR_EOC)  |       |
 |     +-------------------+          +-------------------+          +----------------------+       |
 |                                                                               |                  |
 |                                                                        Store in Accumulator      |
 |                                                                               v                  |
 |  3. 8-Sample Digital Filter & Mathematical Linearization (When adc_count >= 8)                   |
 |     +-------------------------------------------------------------------------------------+      |
 |     |  adc_avg = adc_accumulator / 8                                                      |      |
 |     |  scaled  = (adc_avg * adc_avg) * 100 / (4095 * 4095)       [Quadratic Squaring]     |      |
 |     |  duty    = (scaled <= 30) ? 0 : ((scaled - 30) * 100) / 70  [30% Deadzone Removal]  |      |
 |     +-------------------------------------------------------------------------------------+      |
 |                                                   |                                              |
 |                   +-------------------------------+-------------------------------+              |
 |                   v                                                               v              |
 |  4. Visual Actuation: 10-LED Bar Graph                       5. Acoustic Actuation: PWM Buzzer   |
 |     +-----------------------------------+                       +--------------------------+     |
 |     | level = duty_cycle / 10           |                       | set_duty_cycle(tim2, val)|     |
 |     | Drive GPIOs D0..D10 via BSRR      |                       | Writes directly to CCR1  |     |
 |     | (1 to 10 discrete threshold bars) |                       | (0 to 1000 ARR count)    |     |
 |     +-----------------------------------+                       +--------------------------+     |
 |                                                                                                  |
 |  6. Periodic Telemetry Pipeline (Period: 100 ms)                                                 |
 |     +-------------------------------------------------------------------------------------+      |
 |     | timer_expired?(uart_timer, 100ms) -> snprintf formatted duty-cycle packet            |      |
 |     | Enqueue into 256-byte circular ring buffer (uart_tx_enqueue)                        |      |
 |     +-------------------------------------------------------------------------------------+      |
 |                                                   |                                              |
 |  7. Serial Transmitter Service Drain              v                                              |
 |     +-------------------------------------------------------------------------------------+      |
 |     | While (Buffer NOT Empty AND UART1->ISR & TXE) -> uart_write_byte_nb(UART1, byte)    |      |
 |     +-------------------------------------------------------------------------------------+      |
 +==================================================================================================+
```

---

## Hardware Configuration & Pinout

The firmware interfaces directly with the hardware on the **B-L475E-IOT01A** board and external breadboard peripherals:

| Component | Physical Device | Protocol / Interface | MCU Pin | Arduino Alias | Hardware Register / Mode |
|---|---|---|---|---|---|
| **MCU** | STM32L475VGT6 | ARM Cortex-M4 @ 80 MHz | Silicon | — | Internal flash (1 MB), SRAM1 (96 KB) |
| **Debugger** | ST-LINK/V2-1 | SWD / Virtual COM Port | Internal | — | On-board USB Mini-B interface |
| **Force Sensor** | Interlink FSR402 | Analog Voltage (Divider) | `PC5` | `A0` | `ADC1_IN14`, `GPIO_MODE_ANALOG`, `ASCR` enabled |
| **Buzzer** | Passive Piezo Transducer | Hardware Timer PWM | `PA15` | `D9` | `TIM2_CH1`, `GPIO_MODE_AF` (AF1), Push-Pull |
| **LED Bar Bit 1** | Lite-On SHB10R (Segment 1) | Digital Output | `PA1` | `D0` | `GPIOA_MODER` (Output), atomic `BSRR` |
| **LED Bar Bit 2** | Lite-On SHB10R (Segment 2) | Digital Output | `PA0` | `D1` | `GPIOA_MODER` (Output), atomic `BSRR` |
| **LED Bar Bit 3** | Lite-On SHB10R (Segment 3) | Digital Output | `PD14` | `D2` | `GPIOD_MODER` (Output), atomic `BSRR` |
| **LED Bar Bit 4** | Lite-On SHB10R (Segment 4) | Digital Output | `PB0` | `D3` | `GPIOB_MODER` (Output), atomic `BSRR` |
| **LED Bar Bit 5** | Lite-On SHB10R (Segment 5) | Digital Output | `PA3` | `D4` | `GPIOA_MODER` (Output), atomic `BSRR` |
| **LED Bar Bit 6** | Lite-On SHB10R (Segment 6) | Digital Output | `PB4` | `D5` | `GPIOB_MODER` (Output), atomic `BSRR` |
| **LED Bar Bit 7** | Lite-On SHB10R (Segment 7) | Digital Output | `PB1` | `D6` | `GPIOB_MODER` (Output), atomic `BSRR` |
| **LED Bar Bit 8** | Lite-On SHB10R (Segment 8) | Digital Output | `PA4` | `D7` | `GPIOA_MODER` (Output), atomic `BSRR` |
| **LED Bar Bit 9** | Lite-On SHB10R (Segment 9) | Digital Output | `PB2` | `D8` | `GPIOB_MODER` (Output), atomic `BSRR` |
| **LED Bar Bit 10** | Lite-On SHB10R (Segment 10) | Digital Output | `PA2` | `D10` | `GPIOA_MODER` (Output), atomic `BSRR` |
| **UART Debug TX** | USB VCP Interface | Asynchronous Serial TX | `PB6` | — | `UART1_TX`, `GPIO_MODE_AF` (AF7), 115200 Baud |
| **UART Debug RX** | USB VCP Interface | Asynchronous Serial RX | `PB7` | — | `UART1_RX`, `GPIO_MODE_AF` (AF7), 115200 Baud |

> [!NOTE]
> Pin `D9` (`PA15`) is dedicated to `TIM2_CH1` PWM generation for the buzzer. Consequently, the 10th LED bar segment is mapped to header pin `D10` (`PA2`), preserving timer channel routing without electrical conflict.

---

## Driver & Subsystem Implementation

Every driver module interacts with peripheral silicon via direct C pointer dereferencing of structured register layouts:

| Subsystem / Driver | Source / Header Files | Core Registers Manipulated | Architectural Description |
|---|---|---|---|
| **Reset & Startup** | `startup.c`, `link.ld` | Vectors table (`.vectors`), MSP stack | Defines initial vector table (16 system + 91 peripheral interrupts). Copies initialized `.data` from Flash LMA to SRAM VMA, zeroes `.bss` memory, and invokes `main()`. |
| **Clock Control** | `src/rcc.c`, `include/rcc.h` | `RCC->AHB2ENR`, `RCC->APB1ENR1`, `RCC->APB2ENR` | Controls peripheral clock gating for GPIO ports A/B/C/D, ADC1, General-Purpose Timer 2, UART1, and system configuration logic. |
| **GPIO Subsystem** | `src/gpio.c`, `include/gpio.h` | `GPIO->MODER`, `GPIO->BSRR`, `GPIO->AFR[2]`, `GPIO->ASCR` | Sets pin directions (Input, Output, Alternate Function, Analog). Performs jitter-free, single-cycle atomic bit set/reset operations using `BSRR`. |
| **SAR ADC Driver** | `src/adc.c`, `include/adc.h` | `ADC->CR`, `ADC->ISR`, `ADC->DR`, `ADC->SMPR2`, `ADC->SQR1`, `ADC_COMMON->CCR` | Implements an asynchronous 5-state initialization engine. Drives 12-bit conversion start (`ADSTART`), checks End-Of-Conversion (`EOC`), and extracts converted raw samples from `ADC->DR`. |
| **PWM Timer Engine** | `src/timer.c`, `include/timer.h` | `TIM2->CR1`, `TIM2->CCMR1`, `TIM2->CCER`, `TIM2->ARR`, `TIM2->PSC`, `TIM2->CCR1` | Sets up 32-bit TIM2 in PWM Mode 1 (active while counter < CCR1) with preload enabled. Sets 1 kHz PWM switching base (`ARR=1000`, `PSC=4`) and updates duty cycle dynamically via `set_duty_cycle()`. |
| **SysTick Scheduler** | `src/systick.c`, `include/systick.h` | `SYSTICK->CTRL`, `SYSTICK->LOAD`, `SYSTICK->VAL` | Programs the ARM Cortex-M internal 24-bit down-counter for a 1 ms tick rate. Handles `SysTick_Handler` ISR and provides non-blocking software timer evaluation (`timer_expired`). |
| **UART Telemetry** | `src/uart.c`, `include/uart.h` | `UART1->CR1`, `UART1->BRR`, `UART1->ISR`, `UART1->TDR` | Configures 115200 baud 8N1 transmission. Provides non-blocking byte transmission (`uart_write_byte_nb`) governed by hardware Transmit Data Register Empty (`TXE`) status polling. |
| **POSIX Syscalls** | `src/syscalls.c` | Heap limit pointer `_end` | Implements minimal newlib runtime bridges (`_sbrk`, `_fstat`, `_isatty`, `_write`, `_exit`) enabling standard C library string formatting (`snprintf`) without bloat. |
| **Hardware Mapping** | `include/helper.h` | Port/Pin packing macros | Implements Arduino-to-STM32 port/bit translations, bit masking primitives (`BIT`), and fast inline assembly spin delays (`spin`). |

---

## Engineering Design Decisions

### 1. Non-Blocking ADC Initialization State Machine

The STM32L4 SAR ADC architecture features an internal voltage regulator that must be stabilized, followed by a hardware self-calibration routine, before the converter can be enabled. Traditional blocking implementations halt the core using delay loops:

```
Blocking Sequence (Vendor HAL approach):
Enable Reg -> Busy-wait (20 us) -> Calibrate -> Busy-wait (completion) -> Enable -> Busy-wait (ADRDY)
(CPU is stalled for milliseconds during boot!)
```

To eliminate boot-time stalling, `src/adc.c` encapsulates initialization in a state machine queried on each pass of the main event loop:

$$\text{State Transition:}\quad \text{IDLE} \longrightarrow \text{WAIT\_REG} \xrightarrow{\Delta t \ge 1\text{ ms}} \text{WAIT\_CAL} \xrightarrow{\text{CAL done}} \text{WAIT\_RDY} \xrightarrow{\text{ADRDY=1}} \text{DONE}$$

```c
// Direct register non-blocking poll extracted from src/adc.c
bool adc_init_async_poll(uint32_t now) {
    switch (s_adc_state) {
        case ADC_INIT_IDLE:
            return false;
        case ADC_INIT_WAIT_REG:
            if (!timer_expired(&s_adc_wait, 1, now)) return false;
            ADC->CR |= BIT(31);                         // Trigger calibration (ADCAL)
            s_adc_state = ADC_INIT_WAIT_CAL;
            return false;
        case ADC_INIT_WAIT_CAL:
            if (ADC->CR & BIT(31)) return false;        // Wait until ADCAL bit clears
            ADC->SMPR2 = (7U << 12);                    // Sample time: 640.5 ADC clock cycles
            ADC->SQR1 = (14U << 6);                     // Sequence 1: Channel 14 (PC5)
            ADC->ISR |= BIT(0);                         // Clear ADRDY
            ADC->CR |= BIT(0);                          // Enable ADC (ADEN)
            s_adc_state = ADC_INIT_WAIT_RDY;
            return false;
        case ADC_INIT_WAIT_RDY:
            if ((ADC->ISR & BIT(0)) == 0) return false; // Verify ADRDY flag
            s_adc_state = ADC_INIT_DONE;
            return true;
        case ADC_INIT_DONE:
            return true;
    }
}
```

**Technical Benefit:** The main loop continues servicing communication and visual tasks while the analog hardware reaches electrical equilibrium.

---

### 2. Circular Ring Buffer for UART Telemetry

At 115200 baud, each serial byte requires approximately $86.8\ \mu\text{s}$ to transmit. Writing a 28-byte debug string (`"DUTY CYCLE VALUE: 100\r\n"`) synchronously stalls the processor for:

$$T_{\text{stall}} = 28\text{ bytes} \times 86.8\ \mu\text{s/byte} \approx 2.43\text{ ms}$$

In a 2 ms control loop, blocking writes introduce massive latency jitter and drop sensor sample deadlines. 

To solve this, `main.c` integrates a **256-byte First-In-First-Out (FIFO) circular ring buffer**:
- `uart_tx_enqueue()` writes formatted characters into `s_uart_tx_buf[]` in $\mathcal{O}(1)$ time without touching UART hardware registers.
- `uart_tx_service()` drains available bytes to `UART1->TDR` only when the hardware transmit register is empty (`BIT(7)` of `UART1->ISR` is asserted).
- If the buffer ever fills under burst load, subsequent bytes drop gracefully without memory corruption or CPU deadlock.

```
       +------------------- 256-Byte Circular Ring Buffer -------------------+
       |                                                                     |
       |     [Tail]                                    [Head]                |
       |       |                                         |                   |
       v       v                                         v                   v
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
   |   |   | D | U | T | Y |   | C | Y | C | L | E | : |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
               |                                         ^
               | Drained by uart_tx_service()            | Appended by uart_tx_enqueue()
               | when UART1->ISR & TXE is set            | on periodic 100 ms timer
               v                                         |
     [UART1 Hardware TDR]                     [snprintf Telemetry Packet]
```

---

### 3. FSR402 Quadratic Scaling & Deadzone Modeling

The Interlink Electronics FSR402 is a polymer thick film (PTF) sensor. Its electrical resistance $R_{\text{FSR}}$ decreases inversely with applied mechanical force $F$:

$$R_{\text{FSR}} \propto \frac{1}{F}$$

When placed in a standard voltage divider with pull-down resistor $R_M$ ($10\ \text{k}\Omega$):

$$V_{\text{ADC}} = V_{\text{DD}} \cdot \left(\frac{R_M}{R_M + R_{\text{FSR}}}\right)$$

This configuration produces an aggressive logarithmic-like curve: light touch triggers a sudden jump in voltage, followed by rapid saturation at medium pressures. Additionally, mechanical pre-load and resting surface contact introduce an erratic zero-force baseline.

```
 RAW FSR VOLTAGE (Uncorrected)             CORRECTED RESPONSE (Quadratic + Deadzone)
 ADC Volts                               Duty Cycle %
  3.3V |           /'''''' Saturation     100% |                         /
       |         /'                            |                       /'
       |       /'                              |                     /'
       |     /'                                |                   /'
       |   /'   Hyperbolic jump                |                 /'  Natural, linear
  0.0V +-----------------------           0%   +--------+------/'    tactile feel
       0g                    1kg               0g     300g (Deadzone) 1kg
             Applied Force                           Applied Force
```

The firmware applies two mathematical stages to achieve an intuitive, human-linear response:

1. **Quadratic Transfer Function:** Squaring the normalized ADC sample expands sensitivity across higher force bands while dampening hypersensitivity to light touches:
   $$\text{scaled} = \frac{\text{adc\_avg}^2 \times 100}{4095^2}$$
2. **30% Deadzone Floor & Dynamic Rescaling:** Clamps resting mechanical noise below 30% to zero and rescales the remaining $70\%$ dynamic range cleanly across the full $0\text{--}100\%$ control spectrum:
   $$\text{duty\_cycle} = \begin{cases} 0, & \text{scaled} \le 30 \\ \dfrac{(\text{scaled} - 30) \times 100}{70}, & \text{scaled} > 30 \end{cases}$$

---

## Building & Compilation

### Toolchain Prerequisites

Ensure the following cross-development packages are installed on your host system:

#### macOS (via Homebrew)
```bash
brew install arm-none-eabi-gcc make stlink git
```

#### Ubuntu / Debian
```bash
sudo apt-get update
sudo apt-get install -y gcc-arm-none-eabi make stlink-tools git
```

#### Windows (via xPack or MSYS2)
1. Download and install [xPack GNU Arm Embedded GCC](https://xpack.github.io/dev-tools/arm-none-eabi-gcc/).
2. Install GNU Make via [MSYS2](https://www.msys2.org/) or [Chocolatey](https://chocolatey.org/) (`choco install make`).
3. Download ST-Link tools via [stlink-org](https://github.com/stlink-org/stlink/releases) and ensure `arm-none-eabi-gcc`, `make`, and `st-flash` are present in your system `PATH`.

Verify toolchain availability:
```bash
arm-none-eabi-gcc --version
make --version
st-flash --version
```

---

### Compilation Commands

The project uses a clean Makefile optimized for small code footprint and strict compiler verification:

```bash
# Clean previous build artifacts
make clean

# Compile release build (-Os, dead-code removal, no debug overhead)
make

# Compile debug build (-O0, -g3, -DDEBUG)
make DEBUG=1
```

### Compiler & Linker Flag Breakdown

The `Makefile` enforces strict C safety standards:
- `-Wall -Wextra -Werror`: Promotes all warnings to errors, ensuring clean code.
- `-Wconversion -Wshadow -Wdouble-promotion`: Prevents implicit numeric truncation, variable shadowing, and unintended single-to-double float conversions.
- `-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16`: Targets the Cortex-M4 hardware floating-point unit directly.
- `-ffunction-sections -fdata-sections -Wl,--gc-sections`: Emits individual sections for functions and data, allowing the linker to strip unused symbols.
- `-nostartfiles -nostdlib --specs=nano.specs -lc -lgcc`: Uses small-footprint newlib-nano without bloated default runtime startup objects.

Build output artifacts generated in the root directory:
- `firmware.elf`: ELF executable containing debug symbols and section headers.
- `firmware.bin`: Raw binary flash image ready for target programming.
- `firmware.elf.map`: Linker map file verifying memory section placement and symbol sizes.

---

## Target Flashing

Connect the **B-L475E-IOT01A Discovery Kit** to your host workstation via the USB ST-LINK port (CN1).

Deploy the firmware binary to internal Flash memory at origin address `0x08000000`:

```bash
make flash
```

Alternatively, invoke `st-flash` directly:

```bash
st-flash --reset write firmware.bin 0x08000000
```

### Expected Flashing Output:
```text
st-flash 1.7.0
2026-10-14T20:00:00 INFO common.c: Loading device: STM32L475xx
2026-10-14T20:00:00 INFO common.c: Flash page at : 0x08000000; size: 2048
2026-10-14T20:00:00 INFO common.c: Target memory: 0x08000000
2026-10-14T20:00:00 INFO common.c: Starting Flash write for VL/F0/F3/F1_XL/L4 core...
2026-10-14T20:00:00 INFO flash_loader.c: Successfully loaded flash loader in sram
2026-10-14T20:00:00 INFO flash_loader.c: Clear flash cr flags
2026-10-14T20:00:00 INFO common.c: Starting verification of write complete...
2026-10-14T20:00:00 INFO common.c: Flash written and verified! jolly good!
```

> [!TIP]
> If `st-flash` reports `Cannot connect to target`, hold the black hardware **RESET** button (B2) on the discovery board while running `make flash`, then release it when the flashing tool establishes contact.

---

## UART Telemetry & Debugging

The firmware continuously logs real-time duty cycle telemetry through UART1 mapped to the on-board ST-LINK Virtual COM Port (VCP).

### Serial Port Parameters

| Parameter | Configuration |
|---|---|
| **Baud Rate** | `115200` |
| **Data Bits** | `8` |
| **Parity** | `None` (`N`) |
| **Stop Bits** | `1` |
| **Flow Control** | `None` |

### Opening a Serial Session

Identify the virtual serial port assigned by your host OS:
- **macOS:** `/dev/tty.usbmodem*`
- **Linux:** `/dev/ttyACM*`
- **Windows:** `COMx` (check Device Manager &rarr; Ports)

Connect using your preferred terminal client:

#### Using `tio`
```bash
tio /dev/tty.usbmodem14202 -b 115200
```

#### Using `minicom`
```bash
minicom -D /dev/ttyACM0 -b 115200
```

#### Using `screen`
```bash
screen /dev/tty.usbmodem* 115200
# (To exit screen: press Ctrl-A followed by K, then confirm Y)
```

### Live Stream Output Sample

```text
DUTY CYCLE VALUE: 0
DUTY CYCLE VALUE: 0
DUTY CYCLE VALUE: 14
DUTY CYCLE VALUE: 38
DUTY CYCLE VALUE: 65
DUTY CYCLE VALUE: 92
DUTY CYCLE VALUE: 100
DUTY CYCLE VALUE: 100
DUTY CYCLE VALUE: 45
DUTY CYCLE VALUE: 0
```

---

## Repository Structure

```
M4_practice/
├── Makefile             # GNU Make build script with cross-compiler toolchain flags
├── README.md            # Comprehensive project documentation and engineering portfolio
├── include/             # Hardware register definitions and peripheral driver headers
│   ├── adc.h            # SAR ADC register structure and async state machine prototypes
│   ├── gpio.h           # GPIO port structures and pin mode/alternate function enums
│   ├── helper.h         # Board-level Arduino pin aliases and bit manipulation macros
│   ├── rcc.h            # Reset & Clock Control (RCC) memory-mapped register map
│   ├── systick.h        # ARM Cortex-M core SysTick register map and timer API
│   ├── timer.h          # General-Purpose Timer (TIM2) register structures and PWM API
│   └── uart.h           # Universal Asynchronous Receiver-Transmitter (UART) register map
├── link.ld              # Custom GNU Linker script (Flash/SRAM layout, vector table, heap)
├── main.c               # Non-blocking super-loop, sensor linearization, and ring buffer
├── src/                 # Low-level driver implementations (direct register writes)
│   ├── adc.c            # Asynchronous ADC state machine and sampling driver
│   ├── gpio.c           # GPIO configuration, atomic BSRR write, and alternate function mux
│   ├── helper.c         # Auxiliary board initialization helper routines
│   ├── rcc.c            # Peripheral clock gate enabling implementations
│   ├── syscalls.c       # Minimal POSIX/newlib stubs (_sbrk heap allocator, _write)
│   ├── systick.c        # 1 ms SysTick interrupt handler and timer expiry evaluation
│   ├── timer.c          # TIM2 hardware PWM configuration and duty cycle register updates
│   └── uart.c           # Register-level UART1 initialization and non-blocking transmit
└── startup.c            # Reset vector handler (_reset), memory initialization, vector table
```

---

## References & Technical Documentation

1. **STMicroelectronics RM0351:** *STM32L47xxx, STM32L48xxx, STM32L49xxx, and STM32L4Axxx Reference Manual* — Advanced ARM-based 32-bit MCUs.
2. **STMicroelectronics DS10198:** *STM32L475xx Datasheet* — Ultra-low-power ARM Cortex-M4 32-bit MCU+FPU.
3. **STMicroelectronics UM2153:** *Discovery kit for IoT node, multi-channel communication with STM32L4 (B-L475E-IOT01A)* User Manual.
4. **Interlink Electronics:** *FSR 400 Series Integration Guide and Evaluation Specifications* — Force Sensing Resistor characterization.
5. **ARM Limited:** *ARM Cortex-M4 Devices Generic User Guide* (ARM DUI 0553A).
6. **CPQ / Bare-Metal Guide:** *Bare-Metal Programming Guide for ARM Microcontrollers* ([GitHub Repository](https://github.com/cpq/bare-metal-programming-guide)).
