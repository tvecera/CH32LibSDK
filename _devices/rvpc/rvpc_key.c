// ****************************************************************************
//
//                      RVPC - PS/2 Keyboard Support
//
// ****************************************************************************

#include "../../includes.h"

#if USE_KEY

volatile u32 PS2_Data[2] = {0};
volatile u8 PS2_BitCount = 0;
volatile u8 PS2_PrevClk = 1;
volatile u8 PS2_ExtendedBreakFlag = 0;

// ONE BYTE for all key states!
volatile u8 KeyState = 0;

volatile u16 KeyBuf[KEYBUF_SIZE];
volatile u8 KeyWriteOff = 0;
volatile u8 KeyReadOff = 0;

static const u8 ScanCodes[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, '`', 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 'q', '1', 0x00, 0x00, 0x00, 'z', 's', 'a', 'w', '2', 0x00,
    0x00, 'c', 'x', 'd', 'e', '4', '3', 0x00, 0x00, ' ', 'v', 'f', 't', 'r', '5', 0x00,
    0x00, 'n', 'b', 'h', 'g', 'y', '6', 0x00, 0x00, 0x00, 'm', 'j', 'u', '7', '8', 0x00,
    0x00, ',', 'k', 'i', 'o', '0', '9', 0x00, 0x00, '.', '/', 'l', ';', 'p', '-', 0x00,
    0x00, 0x00, '\'', 0x00, '[', '=', 0x00, 0x00, 0x00, 0x00, 0x0D, ']', 0x00, '\\', 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00,
};

// Map scancode to KEY_*
static u8 ScanToKey(const u16 scancode) {
    const u8 low = scancode & 0xFF;
    const u8 high = (scancode >> 8) & 0xFF;

    if (high == 0x00) {
        switch (low) {
            case 0x1C:
            case 0x29:
            case 0x5A: return KEY_A;
            case 0x32: return KEY_B;
            case 0x22: return KEY_X;
            case 0x35: return KEY_Y;
            default: return NOKEY;
        }
    }

    if (high == 0xE0) {
        switch (low) {
            case 0x6B: return KEY_LEFT;
            case 0x74: return KEY_RIGHT;
            case 0x75: return KEY_UP;
            case 0x72: return KEY_DOWN;
            default: return NOKEY;
        }
    }

    return NOKEY;
}

u8 KeyGetChar(const u32 scancode) {
    if (scancode == NOKEY) return NOKEY;
    const u8 index = (u8)(scancode & 0xFF);
    if (index < sizeof(ScanCodes)) {
        return ScanCodes[index];
    }
    return NOKEY;
}

u8 KeyGet() {
    u16 scancode;
    u8 key;
    int loop_count = 0;

    while (loop_count < KEYBUF_SIZE) {
        loop_count++;

        if (KeyReadOff == KeyWriteOff) {
            return NOKEY;
        }

        scancode = KeyBuf[KeyReadOff];

        KeyReadOff++;
        if (KeyReadOff >= KEYBUF_SIZE) {
            KeyReadOff = 0;
        }

        key = ScanToKey(scancode);

        // Return only make codes (break codes already processed in ASM)
        if (key != NOKEY) {
            return key;
        }
    }

    return NOKEY;
}

// check if joystick is pressed (KEY_RIGHT, KEY_UP, KEY_LEFT, KEY_DOWN, KEY_A, KEY_B, KEY_X, KEY_Y)
Bool JoyPressed(const int key) {
    switch (key) {
        case KEY_RIGHT:
            return KeyPressed(KEY_RIGHT);
        case KEY_UP:
            return KeyPressed(KEY_UP);
        case KEY_LEFT:
            return KeyPressed(KEY_LEFT);
        case KEY_DOWN:
            return KeyPressed(KEY_DOWN);
        case KEY_A:
            return KeyPressed(KEY_A);
        case KEY_B:
            return KeyPressed(KEY_B);
        case KEY_X:
            return KeyPressed(KEY_X);
        case KEY_Y:
            return KeyPressed(KEY_Y);
        default:
            return False;
    }
}

// get joystick (returns NOKEY or KEY_RIGHT, KEY_UP, KEY_LEFT, KEY_DOWN, KEY_A, KEY_B, KEY_X, KEY_Y)
u8 JoyGet() {
    u8 key;
    while (True) {
        key = KeyGet();
        switch (key) {
            case KEY_RIGHT:
                return KEY_RIGHT;
            case KEY_UP:
                return KEY_UP;
            case KEY_LEFT:
                return KEY_LEFT;
            case KEY_DOWN:
                return KEY_DOWN;
            case KEY_A:
                return KEY_A;
            case KEY_B:
                return KEY_B;
            case KEY_X:
                return KEY_X;
            case KEY_Y:
                return KEY_Y;
            case NOKEY:
                return NOKEY;
        }
    }
}

u16 KeyGetRaw() {
    if (KeyReadOff == KeyWriteOff) return NOKEY;

    const u16 scancode = KeyBuf[KeyReadOff];

    KeyReadOff++;
    if (KeyReadOff >= KEYBUF_SIZE) {
        KeyReadOff = 0;
    }

    return scancode;
}

void KeyFlush() {
    KeyWriteOff = 0;
    KeyReadOff = 0;
    KeyState = 0;

    PS2_Data[0] = 0;
    PS2_Data[1] = 0;
    PS2_BitCount = 0;
    PS2_PrevClk = 1;

    WaitMs(100);
}

void KeyWaitNoPressed() {
    while (KeyState != 0) {
        WaitMs(10);
    }
}

void KeyWaitPressed() {
    while (KeyNonePressed()) {
    }
}

void KeyInit() {
    RCC_PAClkEnable();
    RCC_PDClkEnable();

    GPIO_Mode(PA2, GPIO_MODE_IN);
    GPIO_Out0(PD1);
    GPIO_Mode(PD1, GPIO_MODE_IN);

    KeyFlush();
}

void KeyTerm() {
    GPIO_PinReset(PA2);
    GPIO_PinReset(PD1);
}

#endif // USE_KEY
