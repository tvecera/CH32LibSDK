// ****************************************************************************
//
//                        RVPC - VGA display driver for CH32V002J4M6
//
// ****************************************************************************
#include "../../includes.h"

#if USE_DISP		// 1=use display support

u8 FrameBuf[FRAMESIZE];     // display graphics buffer
volatile u32 DispLine;      // current display line
volatile u32 DispFrame = 0; // current frame
volatile u8* FrameBufAddr;  // current pointer to graphics buffer
volatile u32 DispTimTest;   // test - get TIM-CNT value at start of image

#if USE_RCA
// ============================================================================
// RCA Composite Video Implementation (PAL-like timing)
// ============================================================================
// This variant bit-bangs a single PD6 pin with precise cycle timing to build
// a composite-like signal compatible with standard RCA video inputs.
//
// TIMING CALCULATIONS:
// -------------------
// HSI Oscillator: ~24 MHz default, trimmed to ~50 MHz (RCC_HSITrim(31))
// Timer clock: ~50 MHz (from trimmed HSI - note: HSI is not crystal-accurate!)
// TIM2_InitPWM(1, 1, 3194-1, 235, False):
//   - Prescaler = 1 (timer runs at full HSI speed)
//   - Period = 3194-1 = 3193 (ARR register)
//   - HSYNC pulse width = 235 timer clocks
//
// IMPORTANT: Period value calibration
//   Theoretical ideal for PAL: 3200 (for exactly 15.625 kHz at 50 MHz)
//   Actual value used: 3194 (empirically determined via oscilloscope)
//
//   Why 3194 instead of 3200?
//   The HSI oscillator does not run at exactly 50 MHz despite trim setting.
//   Value 3194 was measured with oscilloscope to achieve line rate between
//   15.625-15.650 kHz, which is optimal for PAL composite video sync.
//
//   Each MCU may vary slightly - adjust this value if your display shows
//   rolling or unstable sync. Measure HSYNC frequency and adjust period:
//     - If line rate < 15.625 kHz: decrease period (e.g., 3193, 3192...)
//     - If line rate > 15.650 kHz: increase period (e.g., 3195, 3196...)
//
// Line timing:
//   Line period = 3194 / 50_000_000 = 63.88 us
//   Line rate = 50_000_000 / 3194 ≈ 15.65 kHz
//   (Target: PAL ~15.625 kHz - very close!)
//
// HSYNC pulse:
//   HSYNC width = 235 / 50_000_000 = 4.7 us
//   (PAL target: ~4.7 us, NTSC target: ~4.8 us - perfect!)
//
// Active video starts at TIM2_Comp2(560):
//   Delay from line start = 560 / 50_000_000 = 11.2 us
//   This accounts for HSYNC (4.7 us) + back porch (~6.5 us)
//
// Pixel timing (SendByteBitbangDiv8 function):
//   Each bit takes ~18 cycles at 50 MHz = 0.36 us/pixel
//   One byte (8 pixels) = ~144 cycles = 2.88 us
//   16 bytes per line = 16 * 2.88 us = 46.08 us active video
//
// Frame timing:
//   Total lines per frame = 312 (close to PAL's 312.5)
//   Frame rate = 15.65 kHz / 312 ≈ 50.16 Hz (PAL target: 50 Hz)
//   Active lines = 256 (64 source lines * 4 vertical scaling)
//   VBlank top = 29 lines (shifts image down for better centering)
//   VSync pulse = lines 302-310 (8 lines, ~510 us)
//
// RECOMMENDED WIRING (for 5V supply):
// ----------------------------------
//  PD6 (Pin 1) -> 330Ω resistor -> VIDEO signal (RCA center pin)
//  PC1 (Pin 5) -> 680Ω resistor -> SYNC combined with VIDEO
//  GND (Pin 8) -> GND (RCA shield)
//
// The resistor network creates proper composite video levels:
//  - Sync level: ~0.3V (680Ω divider dominates)
//  - Black level: ~0.3V (video off, sync off)
//  - White level: ~1V (330Ω pulls up when video is high)
//
// ============================================================================

#define TOTAL_LINES         312
#define VSYNC_START_LINE    302
#define VSYNC_END_LINE      310

// Frame timing constants
#if VMODE == 0
#define ACTIVE_LINES        256  // 64 source lines * 4 vertical scale
#define VBLANK_TOP_LINES     29  // shift the image down
#elif VMODE == 1
#define ACTIVE_LINES        240  // 120 source lines * 2 vertical scale
#define VBLANK_TOP_LINES     37  // shift the image down for proper centering
#elif VMODE == 8
#define ACTIVE_LINES        252  // 18 text rows * 14 scanlines per row
#define VBLANK_TOP_LINES     30  // shift the image down for proper centering in safe area
#endif

#define PD6_MASK            (1 << 6)
#define GPIOD_BCR           (*(volatile u32*)(GPIOD_BASE + 0x14))

volatile u32 line_counter = 0;

#if VMODE == 8
// Font line lookup table for 14 scanlines per character row
// 6 font lines doubled, 2 font lines single
static const u8 font_line_lut[14] = {
    0, 0,
    1, 1,
    2,
    3, 3,
    4,
    5, 5,
    6, 6,
    7, 7
};

// Global counters for constant timing (avoids division/modulo)
static u8 current_char_row = 0;
static u8 current_line_in_row = 0;
#endif

#if VMODE == 0
// Fast send of 8 pixels on PD6 - based on send_byte_bitbang_div8_slow
// Each bit takes 14 cycles for proper composite video timing
// PD6 = bit 6 in GPIOD, so we shift data bits to position 6
static inline void SendByteBitbangDiv8(u8 data) {
    register u32 gpiod_data = GPIOD_BASE + 0x0C; // GPIOD OUTDR offset

    asm volatile(
        ".align	2,,	                \n"
        // Bit 7 (MSB) -> shift left 1 to move bit 7 to bit 6 position
        "andi   a2, %0, 0x80        \n" // [1] mask bit 7 (0x80)
        "c.srli a2, 1               \n" // [1] shift right 1: 0x80 -> 0x40 (bit 6)
        "sw     a2, 0(%1)           \n" // [2] write to GPIOD DATA
        "c.li   a3, 4               \n" // [1] load loop counter
        "1: c.addi a3, -1           \n" // [1] decrement
        "c.bnez a3, 1b              \n" // [2/1] loop: 4*3=12, +1 load +1 exit = 14 cycles

        // Bit 6 -> already at bit 6 position
        "andi   a2, %0, 0x40        \n" // [1] mask bit 6 (0x40)
        "sw     a2, 0(%1)           \n" // [2] no shift needed
        "c.li   a3, 4               \n" // [1] load loop counter
        "2: c.addi a3, -1           \n" // [1] decrement
        "c.bnez a3, 2b              \n" // [2/1] loop: 5*3=15 cycles

        // Bit 5 -> shift left 1
        "andi   a2, %0, 0x20        \n" // [1] mask bit 5 (0x20)
        "c.slli a2, 1               \n" // [1] shift left 1: 0x20 -> 0x40
        "sw     a2, 0(%1)           \n"
        "c.li   a3, 4               \n" // [1] load loop counter
        "3: c.addi a3, -1           \n" // [1] decrement
        "c.bnez a3, 3b              \n" // [2/1] loop: 4*3=12, +1 load +1 exit = 14 cycles

        // Bit 4 -> shift left 2
        "andi   a2, %0, 0x10        \n" // [1] mask bit 4 (0x10)
        "c.slli a2, 2               \n" // [1] shift left 2: 0x10 -> 0x40
        "sw     a2, 0(%1)           \n"
        "c.li   a3, 4               \n" // [1] load loop counter
        "4: c.addi a3, -1           \n" // [1] decrement
        "c.bnez a3, 4b              \n" // [2/1] loop: 4*3=12, +1 load +1 exit = 14 cycles

        // Bit 3 -> shift left 3
        "andi   a2, %0, 0x08        \n" // [1] mask bit 3 (0x08)
        "c.slli a2, 3               \n" // [1] shift left 3: 0x08 -> 0x40
        "sw     a2, 0(%1)           \n"
        "c.li   a3, 4               \n" // [1] load loop counter
        "5: c.addi a3, -1           \n" // [1] decrement
        "c.bnez a3, 5b              \n" // [2/1] loop: 4*3=12, +1 load +1 exit = 14 cycles

        // Bit 2 -> shift left 4
        "andi   a2, %0, 0x04        \n" // [1] mask bit 2 (0x04)
        "c.slli a2, 4               \n" // [1] shift left 4: 0x04 -> 0x40
        "sw     a2, 0(%1)           \n"
        "c.li   a3, 4               \n" // [1] load loop counter
        "6: c.addi a3, -1           \n" // [1] decrement
        "c.bnez a3, 6b              \n" // [2/1] loop: 4*3=12, +1 load +1 exit = 14 cycles

        // Bit 1 -> shift left 5
        "andi   a2, %0, 0x02        \n" // [1] mask bit 1 (0x02)
        "c.slli a2, 5               \n" // [1] shift left 5: 0x02 -> 0x40
        "sw     a2, 0(%1)           \n"
        "c.li   a3, 4               \n" // [1] load loop counter
        "7: c.addi a3, -1           \n" // [1] decrement
        "c.bnez a3, 7b              \n" // [2/1] loop: 4*3=12, +1 load +1 exit = 14 cycles

        // Bit 0 (LSB) -> shift left 6
        "andi   a2, %0, 0x01        \n" // [1] mask bit 0 (0x01)
        "c.slli a2, 6               \n" // [1] shift left 6: 0x01 -> 0x40
        "sw     a2, 0(%1)           \n"
        "c.li   a3, 2               \n" // [1] load loop counter
        "8: c.addi a3, -1           \n" // [1] decrement
        "c.bnez a3, 8b              \n" // [2/1] loop: 2*3=6, +1 load +1 exit = 8 cycles
        :
        : "r" (data), "r" (gpiod_data)
        : "a2", "a3", "memory"
    );
}

#elif VMODE == 1

// Fast send of 8 pixels on PD6 for VMODE 1 (160x120)
// Optimized timing: ~13 cycles/pixel (vs 14 in VMODE 0) to fit 160 pixels on PAL line
// Reduced loop counter (3 instead of 4) makes image narrower to prevent overscan
// PD6 = bit 6 in GPIOD, so we shift data bits to position 6
static inline void SendByteBitbangFast(u8 data) {
    register u32 gpiod_data = GPIOD_BASE + 0x0C; // GPIOD OUTDR offset

    asm volatile(
        ".align	2,,	                \n"
        // Bit 7 (MSB) -> shift right 1 to move bit 7 to bit 6 position
        "andi   a2, %0, 0x80        \n" // [1] mask bit 7 (0x80)
        "c.srli a2, 1               \n" // [1] shift right 1: 0x80 -> 0x40 (bit 6)
        "sw     a2, 0(%1)           \n" // [2] write to GPIOD DATA
        "c.li   a3, 3               \n" // [1] load loop counter = 3
        "1: c.addi a3, -1           \n" // [1] decrement
        "c.bnez a3, 1b              \n" // [2/1] loop: 3+3+2=8, total: 1+1+2+1+8 = 13 cycles/pixel

        // Bit 6 -> already at bit 6 position (no shift needed)
        "andi   a2, %0, 0x40        \n" // [1] mask bit 6 (0x40)
        "sw     a2, 0(%1)           \n" // [2] no shift needed
        "c.li   a3, 3               \n" // [1] load loop counter = 3
        "2: c.addi a3, -1           \n" // [1] decrement
        "c.bnez a3, 2b              \n" // [2/1] loop: 3+3+2=8, total: 1+2+1+8 = 12 cycles/pixel

        // Bit 5 -> shift left 1
        "andi   a2, %0, 0x20        \n" // [1] mask bit 5 (0x20)
        "c.slli a2, 1               \n" // [1] shift left 1: 0x20 -> 0x40
        "sw     a2, 0(%1)           \n" // [2]
        "c.li   a3, 3               \n" // [1] load loop counter = 3
        "3: c.addi a3, -1           \n" // [1] decrement
        "c.bnez a3, 3b              \n" // [2/1] loop: 3+3+2=8, total: 1+1+2+1+8 = 13 cycles/pixel

        // Bit 4 -> shift left 2
        "andi   a2, %0, 0x10        \n" // [1] mask bit 4 (0x10)
        "c.slli a2, 2               \n" // [1] shift left 2: 0x10 -> 0x40
        "sw     a2, 0(%1)           \n" // [2]
        "c.li   a3, 3               \n" // [1] load loop counter = 3
        "4: c.addi a3, -1           \n" // [1] decrement
        "c.bnez a3, 4b              \n" // [2/1] loop: 3+3+2=8, total: 1+1+2+1+8 = 13 cycles/pixel

        // Bit 3 -> shift left 3
        "andi   a2, %0, 0x08        \n" // [1] mask bit 3 (0x08)
        "c.slli a2, 3               \n" // [1] shift left 3: 0x08 -> 0x40
        "sw     a2, 0(%1)           \n" // [2]
        "c.li   a3, 3               \n" // [1] load loop counter = 3
        "5: c.addi a3, -1           \n" // [1] decrement
        "c.bnez a3, 5b              \n" // [2/1] loop: 3+3+2=8, total: 1+1+2+1+8 = 13 cycles/pixel

        // Bit 2 -> shift left 4
        "andi   a2, %0, 0x04        \n" // [1] mask bit 2 (0x04)
        "c.slli a2, 4               \n" // [1] shift left 4: 0x04 -> 0x40
        "sw     a2, 0(%1)           \n" // [2]
        "c.li   a3, 3               \n" // [1] load loop counter = 3
        "6: c.addi a3, -1           \n" // [1] decrement
        "c.bnez a3, 6b              \n" // [2/1] loop: 3+3+2=8, total: 1+1+2+1+8 = 13 cycles/pixel

        // Bit 1 -> shift left 5
        "andi   a2, %0, 0x02        \n" // [1] mask bit 1 (0x02)
        "c.slli a2, 5               \n" // [1] shift left 5: 0x02 -> 0x40
        "sw     a2, 0(%1)           \n" // [2]
        "c.li   a3, 3               \n" // [1] load loop counter = 3
        "7: c.addi a3, -1           \n" // [1] decrement
        "c.bnez a3, 7b              \n" // [2/1] loop: 3+3+2=8, total: 1+1+2+1+8 = 13 cycles/pixel

        // Bit 0 (LSB) -> shift left 6
        "andi   a2, %0, 0x01        \n" // [1] mask bit 0 (0x01)
        "c.slli a2, 6               \n" // [1] shift left 6: 0x01 -> 0x40
        "sw     a2, 0(%1)           \n" // [2]
        "c.li   a3, 2               \n" // [1] load loop counter = 2
        "8: c.addi a3, -1           \n" // [1] decrement
        "c.bnez a3, 8b              \n" // [2/1] loop: 3+2=5, total: 1+1+2+1+5 = 10 cycles/pixel
        :
        : "r" (data), "r" (gpiod_data)
        : "a2", "a3", "memory"
    );
}

#elif VMODE == 8
// Send 8 pixels from font character on PD6 for text mode
// Font format: 4 bytes per character (2 pixels per byte in nibbles)
// Each character rendering: 4 bytes * 8 cycles = 32 cycles
// Total for 23 chars: 23 * 32 = 736 cycles
static inline void SendCharBitbangTextMode(const u8* font_data) {
    register u32 gpiod_outdr = GPIOD_BASE + 0x0C;

    asm volatile(
        ".align	2,,	                    \n"
        // Byte 0: pixels 0-1
        "lbu    t0, 0(%0)               \n"  // [2] Load byte
        "srli   a2, t0, 4               \n"  // [1] Extract pixel 0 (high nibble)
        "slli   a2, a2, 4               \n"  // [1] Shift to bit 6 (nibble 0x4 -> 0x40)
        "sw     a2, 0(%1)               \n"  // [2] Send pixel 0
        "c.li   a3, 2                   \n"  // [1] delay
        "1: c.addi a3, -1               \n"  // [1]
        "c.bnez a3, 1b                  \n"  // [1] -> 8 cycles total

        "andi   a2, t0, 0x0f            \n"  // [1] Extract pixel 1 (low nibble)
        "slli   a2, a2, 4               \n"  // [1] Shift to bit 6 (nibble 0x4 -> 0x40)
        "sw     a2, 0(%1)               \n"  // [2] Send pixel 1
        "c.li   a3, 2                   \n"  // [1]
        "2: c.addi a3, -1               \n"  // [1]
        "c.bnez a3, 2b                  \n"  // [1] -> 8 cycles total

        // Byte 1: pixels 2-3
        "lbu    t0, 1(%0)               \n"  // [2]
        "srli   a2, t0, 4               \n"  // [1]
        "slli   a2, a2, 4               \n"  // [1]
        "sw     a2, 0(%1)               \n"  // [2]
        "c.li   a3, 2                   \n"  // [1]
        "3: c.addi a3, -1               \n"  // [1]
        "c.bnez a3, 3b                  \n"  // [1]

        "andi   a2, t0, 0x0f            \n"  // [1]
        "slli   a2, a2, 4               \n"  // [1]
        "sw     a2, 0(%1)               \n"  // [2]
        "c.li   a3, 2                   \n"  // [1]
        "4: c.addi a3, -1               \n"  // [1]
        "c.bnez a3, 4b                  \n"  // [1]

        // Byte 2: pixels 4-5
        "lbu    t0, 2(%0)               \n"  // [2]
        "srli   a2, t0, 4               \n"  // [1]
        "slli   a2, a2, 4               \n"  // [1]
        "sw     a2, 0(%1)               \n"  // [2]
        "c.li   a3, 2                   \n"  // [1]
        "5: c.addi a3, -1               \n"  // [1]
        "c.bnez a3, 5b                  \n"  // [1]

        "andi   a2, t0, 0x0f            \n"  // [1]
        "slli   a2, a2, 4               \n"  // [1]
        "sw     a2, 0(%1)               \n"  // [2]
        "c.li   a3, 2                   \n"  // [1]
        "6: c.addi a3, -1               \n"  // [1]
        "c.bnez a3, 6b                  \n"  // [1]

        // Byte 3: pixels 6-7
        "lbu    t0, 3(%0)               \n"  // [2]
        "srli   a2, t0, 4               \n"  // [1]
        "slli   a2, a2, 4               \n"  // [1]
        "sw     a2, 0(%1)               \n"  // [2]
        "c.li   a3, 2                   \n"  // [1]
        "7: c.addi a3, -1               \n"  // [1]
        "c.bnez a3, 7b                  \n"  // [1]

        "andi   a2, t0, 0x0f            \n"  // [1]
        "slli   a2, a2, 4               \n"  // [1]
        "sw     a2, 0(%1)               \n"  // [2]
        :
        : "r" (font_data), "r" (gpiod_outdr)
        : "a2", "t0", "memory"
    );
}
#endif

HANDLER void NOFLASH(TIM2_IRQHandler(void));

#if VMODE == 0
// Graphics mode interrupt handler
void TIM2_IRQHandler(void) {
    TIM2->INTFR &= ~(1 << 2); // clear CC2

    u32 line = line_counter++;
    if (line >= TOTAL_LINES) {
        line_counter = 0;
        line = 0;
        DispFrame++; // increment frame counter
        SoundScan();
    }

    if (line < VBLANK_TOP_LINES) {
        GPIOD_BCR = PD6_MASK;
        return;
    }

    if (line < (VBLANK_TOP_LINES + ACTIVE_LINES)) {
        u32 active_line = line - VBLANK_TOP_LINES; // 0..255
        u32 y = active_line >> 2; // 0..63 (source line)
        const u8* ptr = &FrameBuf[y * WIDTHBYTE];

        for (u8 i = 0; i < WIDTHBYTE; i++) {
            SendByteBitbangDiv8(*ptr++);
        }

        GPIOD_BCR = PD6_MASK;
        return;
    }

    if (line < VSYNC_START_LINE) {
        GPIOD_BCR = PD6_MASK;
        return;
    }

    if (line == VSYNC_START_LINE) {
        TIM2->CH1CVR = 1600; // long HSYNC
        GPIOD_BCR = PD6_MASK;
        return;
    }

    if (line < VSYNC_END_LINE) {
        GPIOD_BCR = PD6_MASK;
        return;
    }

    TIM2->CH1CVR = 235; // return to normal HSYNC
    GPIOD_BCR = PD6_MASK;
}

#elif VMODE == 1
// Graphics mode 160x120 interrupt handler
void TIM2_IRQHandler(void) {
    TIM2->INTFR &= ~(1 << 2); // clear CC2

    u32 line = line_counter++;
    if (line >= TOTAL_LINES) {
        line_counter = 0;
        line = 0;
        DispFrame++; // increment frame counter
        SoundScan();
    }
    DispLine = line; // update current display line for VSync check

    if (line < VBLANK_TOP_LINES) {
        GPIOD_BCR = PD6_MASK;
        return;
    }

    if (line < (VBLANK_TOP_LINES + ACTIVE_LINES)) {
        u32 active_line = line - VBLANK_TOP_LINES; // 0..239 (240 scanlines total)
        u32 y = active_line >> 1; // 0..119 (source line with 2x vertical scale)
        const u8* ptr = &FrameBuf[y * WIDTHBYTE]; // 20 bytes per line

        for (u8 i = 0; i < WIDTHBYTE; i++) {
            SendByteBitbangFast(*ptr++);
        }

        GPIOD_BCR = PD6_MASK;
        return;
    }

    if (line < VSYNC_START_LINE) {
        GPIOD_BCR = PD6_MASK;
        return;
    }

    if (line == VSYNC_START_LINE) {
        TIM2->CH1CVR = 1600; // long HSYNC
        GPIOD_BCR = PD6_MASK;
        return;
    }

    if (line < VSYNC_END_LINE) {
        GPIOD_BCR = PD6_MASK;
        return;
    }

    TIM2->CH1CVR = 235; // return to normal HSYNC
    GPIOD_BCR = PD6_MASK;
}

#elif VMODE == 8
// Text mode interrupt handler
void TIM2_IRQHandler(void) {
    TIM2->INTFR &= ~(1 << 2); // clear CC2

    u32 line = line_counter++;
    if (line >= TOTAL_LINES) {
        line_counter = 0;
        line = 0;
        DispFrame++; // increment frame counter
        SoundScan();
    }
    DispLine = line; // update current display line for VSync check

    if (line < VBLANK_TOP_LINES) {
        GPIOD_BCR = PD6_MASK;
        return;
    }

    if (line < (VBLANK_TOP_LINES + ACTIVE_LINES)) {
        u32 active_line = line - VBLANK_TOP_LINES; // 0..251 (252 scanlines total)

        // Reset counters at start of active area
        if (active_line == 0) {
            current_char_row = 0;
            current_line_in_row = 0;
        }

        // Use global counters for constant timing (no division/modulo)
        u32 char_row = current_char_row;
        u32 font_line = font_line_lut[current_line_in_row];

        const u8* text_row = &FrameBuf[char_row * WIDTHBYTE]; // 23 characters

        // Calculate font row offset
        // For 128-char font: each row = 128 chars * 4 bytes = 512 bytes
        const u8* font_row = &DrawFont[font_line * 128 * 4];

        for (u8 ch = 0; ch < WIDTHBYTE; ch++) {
            u8 char_code = text_row[ch];
            const u8* char_glyph = &font_row[char_code * 4]; // 4 bytes per character
            SendCharBitbangTextMode(char_glyph);
        }

        // Update counters for next scanline
        current_line_in_row++;
        if (current_line_in_row >= 14) {
            current_line_in_row = 0;
            current_char_row++;
        }

        GPIOD_BCR = PD6_MASK;
        return;
    }

    if (line < VSYNC_START_LINE) {
        GPIOD_BCR = PD6_MASK;
        return;
    }

    if (line == VSYNC_START_LINE) {
        TIM2->CH1CVR = 1600; // long HSYNC
        GPIOD_BCR = PD6_MASK;
        return;
    }

    if (line < VSYNC_END_LINE) {
        GPIOD_BCR = PD6_MASK;
        return;
    }

    TIM2->CH1CVR = 235; // return to normal HSYNC
    GPIOD_BCR = PD6_MASK;
}
#endif

// wait for VSync scanline
void WaitVSync() {
    while (DispIsVSync()) {
    }
    while (!DispIsVSync()) {
    }
}

// Initialize videomode
void DispInit() {
    FrameBufAddr = FrameBuf; // current pointer to graphics buffer
#if VMODE == 8
    current_char_row = 0;
    current_line_in_row = 0;
#endif
    RCC_HSITrim(31); // ~50 MHz
    RCC_PCClkEnable();
    RCC_PDClkEnable();

    GPIO_Mode(PC1, GPIO_MODE_AF_FAST);
    GPIO_Mode(PD6, GPIO_MODE_OUT_FAST);

#if CH32V003
    GPIO_Remap_TIM2(2);
#else
    GPIO_Remap_TIM2(1);
#endif

    TIM2_InitPWM(1, 1, 3194 - 1, 235, False);
    TIM2_Comp2(560);
    TIM2_OC2Mode(TIM_COMP_FREEZE);
    TIM2_OC2PreEnable();
    TIM2_CC2Enable();
    TIM2_CC2IntClr();
    TIM2_CC2IntEnable();

    NVIC_IRQEnable(IRQ_TIM2);
}

// Terminate videomode
void DispTerm() {
    // disable capture compare interrupt (channel 2)
    NVIC_IRQDisable(IRQ_TIM2); // disable interrupt service
    TIM2_CC2IntDisable(); // disable capture compare of channel 2
    TIM2_CC2Disable(); // disable compare output
    TIM2_CC2IntClr(); // enable capture compare of channel 2

    // disable timer
    TIM2_Disable();

    // Reset timer to default setup
    TIM2_Reset();

    // reset GPIO pins
    GPIO_PinReset(PD6);
    GPIO_PinReset(PC1);

    // reset HSI trim to default value 24 MHz
    RCC_HSITrim(16);
}

#else
// wait for VSync scanline
void WaitVSync() {
    while (DispIsVSync()) {
    }
    while (!DispIsVSync()) {
    }
}

// Initialize videomode
void DispInit() {
    DispLine = 0; // current display line
    FrameBufAddr = FrameBuf; // current pointer to graphics buffer

    // trim HSI oscillator to 25MHz
    RCC_HSITrim(31);

    // initialize GPIO pins (note: clocks are setup at config.h)
    // - PC2 video out, GPIO push-pull output, fast speed
    RCC_AFIClkEnable();
    RCC_PCClkEnable();
    RCC_PDClkEnable();

    // PC2 - VSYNC, GPIO push-pull output, fast speed
    GPIO_Mode(PD6, GPIO_MODE_OUT_FAST);
    // PC1 - HSYNC, Timer 2 Channel 1 output (remap 2)
    GPIO_Mode(PC1, GPIO_MODE_AF_FAST);

    // PC1 - VIDEO output, GPIO push-pull output, fast speed
    GPIO_Mode(PC2, GPIO_MODE_OUT_FAST);

// Remap Timer 2 (output channel 1 to PC1)
#if CH32V003
GPIO_Remap_TIM2(2);
#else
GPIO_Remap_TIM2(1);
#endif

// -----------------------------------------------------------------------------
// HSYNC/VSYNC timing targets (reference)
// -----------------------------------------------------------------------------
// VMODE 0 and 1 target VGA 640x480@60 Hz timing (@ 50 MHz timer base):
// Line period T_H = 31.77756 us => 50e6 * T_H ~ 1600 timer clocks
// HSYNC pulse ~3.81331 us => ~192 clocks
// Back porch ~1.90665 us => ~96 clocks
// Active start = 192 + 96 = 288 clocks
//
// VMODE 8 target PAL-like 720x576@~48.02 Hz timing (@ 50 MHz timer base):
// Text mode: 23 chars * 18 lines (with unroll 5x: 871 cycles active rendering)
// Line period T_H = 32 us => 50e6 * T_H = 1600 timer clocks
// HSYNC pulse ~3.81331 us => ~192 clocks
// Back porch ~1.1 us => ~55 clocks
// Active start = 192 + 55 = 247 clocks
// Front porch = ~73 clocks (1.46 us)

TIM2_InitPWM(1, 1, 1600 - 1, 192, False);

// Setup interrupt on Timer 2 channel 1 to display image
//  128x64: 8 clock cycles per pixel, 64 clock cycles per character, active 16 characters, total 25 characters
//  160x120: 8 clock cycles per pixel, 64 clock cycles per character, active 20 characters, total 25 characters
#if VMODE == 0
TIM2_Comp2(288 - 60); // set compare value
#elif VMODE == 1
TIM2_Comp2(288 - 60); // set compare value
#elif VMODE == 8
TIM2_Comp2(247 + 5); // set compare value
#endif
TIM2_OC2Mode(TIM_COMP_FREEZE); // set compare mode
TIM2_OC2PreEnable(); // enable preload compare register
TIM2_CC2Enable(); // enable compare output
TIM2_CC2IntClr(); // clear interrupt request
TIM2_CC2IntEnable(); // enable capture compare of channel 2
NVIC_IRQEnable(IRQ_TIM2); // enable interrupt service
}

// Terminate videomode
void DispTerm() {
    // disable capture compare interrupt (channel 2)
    NVIC_IRQDisable(IRQ_TIM2); // disable interrupt service
    TIM2_CC2IntDisable(); // disable capture compare of channel 2
    TIM2_CC2Disable(); // disable compare output
    TIM2_CC2IntClr(); // enable capture compare of channel 2

    // disable timer
    TIM2_Disable();

    // Reset timer to default setup
    TIM2_Reset();

    // reset GPIO pins
    GPIO_PinReset(PD6);
    GPIO_PinReset(PC1);
    GPIO_PinReset(PC2);

    // reset HSI trim to default value 24 MHz
    RCC_HSITrim(16);
}
#endif // USE_RCA

#endif // USE_DISP
