/**
 * Looking Glass
 * Copyright © 2017-2022 The Looking Glass Authors
 * https://looking-glass.io
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc., 59
 * Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "kb.h"
#include "cimgui.h"

const uint32_t linux_to_ps2[KEY_MAX] =
{
  [KEY_RESERVED]         /* = USB   0 */ = 0x000000,
  [KEY_ESC]              /* = USB  41 */ = 0x000001,
  [KEY_1]                /* = USB  30 */ = 0x000002,
  [KEY_2]                /* = USB  31 */ = 0x000003,
  [KEY_3]                /* = USB  32 */ = 0x000004,
  [KEY_4]                /* = USB  33 */ = 0x000005,
  [KEY_5]                /* = USB  34 */ = 0x000006,
  [KEY_6]                /* = USB  35 */ = 0x000007,
  [KEY_7]                /* = USB  36 */ = 0x000008,
  [KEY_8]                /* = USB  37 */ = 0x000009,
  [KEY_9]                /* = USB  38 */ = 0x00000A,
  [KEY_0]                /* = USB  39 */ = 0x00000B,
  [KEY_MINUS]            /* = USB  45 */ = 0x00000C,
  [KEY_EQUAL]            /* = USB  46 */ = 0x00000D,
  [KEY_BACKSPACE]        /* = USB  42 */ = 0x00000E,
  [KEY_TAB]              /* = USB  43 */ = 0x00000F,
  [KEY_Q]                /* = USB  20 */ = 0x000010,
  [KEY_W]                /* = USB  26 */ = 0x000011,
  [KEY_E]                /* = USB   8 */ = 0x000012,
  [KEY_R]                /* = USB  21 */ = 0x000013,
  [KEY_T]                /* = USB  23 */ = 0x000014,
  [KEY_Y]                /* = USB  28 */ = 0x000015,
  [KEY_U]                /* = USB  24 */ = 0x000016,
  [KEY_I]                /* = USB  12 */ = 0x000017,
  [KEY_O]                /* = USB  18 */ = 0x000018,
  [KEY_P]                /* = USB  19 */ = 0x000019,
  [KEY_LEFTBRACE]        /* = USB  47 */ = 0x00001A,
  [KEY_RIGHTBRACE]       /* = USB  48 */ = 0x00001B,
  [KEY_ENTER]            /* = USB  40 */ = 0x00001C,
  [KEY_LEFTCTRL]         /* = USB 224 */ = 0x00001D,
  [KEY_A]                /* = USB   4 */ = 0x00001E,
  [KEY_S]                /* = USB  22 */ = 0x00001F,
  [KEY_D]                /* = USB   7 */ = 0x000020,
  [KEY_F]                /* = USB   9 */ = 0x000021,
  [KEY_G]                /* = USB  10 */ = 0x000022,
  [KEY_H]                /* = USB  11 */ = 0x000023,
  [KEY_J]                /* = USB  13 */ = 0x000024,
  [KEY_K]                /* = USB  14 */ = 0x000025,
  [KEY_L]                /* = USB  15 */ = 0x000026,
  [KEY_SEMICOLON]        /* = USB  51 */ = 0x000027,
  [KEY_APOSTROPHE]       /* = USB  52 */ = 0x000028,
  [KEY_GRAVE]            /* = USB  53 */ = 0x000029,
  [KEY_LEFTSHIFT]        /* = USB 225 */ = 0x00002A,
  [KEY_BACKSLASH]        /* = USB  49 */ = 0x00002B,
  [KEY_Z]                /* = USB  29 */ = 0x00002C,
  [KEY_X]                /* = USB  27 */ = 0x00002D,
  [KEY_C]                /* = USB   6 */ = 0x00002E,
  [KEY_V]                /* = USB  25 */ = 0x00002F,
  [KEY_B]                /* = USB   5 */ = 0x000030,
  [KEY_N]                /* = USB  17 */ = 0x000031,
  [KEY_M]                /* = USB  16 */ = 0x000032,
  [KEY_COMMA]            /* = USB  54 */ = 0x000033,
  [KEY_DOT]              /* = USB  55 */ = 0x000034,
  [KEY_SLASH]            /* = USB  56 */ = 0x000035,
  [KEY_RIGHTSHIFT]       /* = USB 229 */ = 0x000036,
  [KEY_KPASTERISK]       /* = USB  85 */ = 0x000037,
  [KEY_LEFTALT]          /* = USB 226 */ = 0x000038,
  [KEY_SPACE]            /* = USB  44 */ = 0x000039,
  [KEY_CAPSLOCK]         /* = USB  57 */ = 0x00003A,
  [KEY_F1]               /* = USB  58 */ = 0x00003B,
  [KEY_F2]               /* = USB  59 */ = 0x00003C,
  [KEY_F3]               /* = USB  60 */ = 0x00003D,
  [KEY_F4]               /* = USB  61 */ = 0x00003E,
  [KEY_F5]               /* = USB  62 */ = 0x00003F,
  [KEY_F6]               /* = USB  63 */ = 0x000040,
  [KEY_F7]               /* = USB  64 */ = 0x000041,
  [KEY_F8]               /* = USB  65 */ = 0x000042,
  [KEY_F9]               /* = USB  66 */ = 0x000043,
  [KEY_F10]              /* = USB  67 */ = 0x000044,
  [KEY_NUMLOCK]          /* = USB  83 */ = 0x000045,
  [KEY_SCROLLLOCK]       /* = USB  71 */ = 0x000046,
  [KEY_KP7]              /* = USB  95 */ = 0x000047,
  [KEY_KP8]              /* = USB  96 */ = 0x000048,
  [KEY_KP9]              /* = USB  97 */ = 0x000049,
  [KEY_KPMINUS]          /* = USB  86 */ = 0x00004A,
  [KEY_KP4]              /* = USB  92 */ = 0x00004B,
  [KEY_KP5]              /* = USB  93 */ = 0x00004C,
  [KEY_KP6]              /* = USB  94 */ = 0x00004D,
  [KEY_KPPLUS]           /* = USB  87 */ = 0x00004E,
  [KEY_KP1]              /* = USB  89 */ = 0x00004F,
  [KEY_KP2]              /* = USB  90 */ = 0x000050,
  [KEY_KP3]              /* = USB  91 */ = 0x000051,
  [KEY_KP0]              /* = USB  98 */ = 0x000052,
  [KEY_KPDOT]            /* = USB  99 */ = 0x000053,
  [KEY_102ND]            /* = USB 100 */ = 0x000056,
  [KEY_F11]              /* = USB  68 */ = 0x000057,
  [KEY_F12]              /* = USB  69 */ = 0x000058,
  [KEY_RO]               /* = USB 135 */ = 0x000073,
  [KEY_HENKAN]           /* = USB 138 */ = 0x000079,
  [KEY_KATAKANAHIRAGANA] /* = USB 136 */ = 0x000070,
  [KEY_MUHENKAN]         /* = USB 139 */ = 0x00007B,
  [KEY_KPENTER]          /* = USB  88 */ = 0x00E01C,
  [KEY_RIGHTCTRL]        /* = USB 228 */ = 0x00E01D,
  [KEY_KPSLASH]          /* = USB  84 */ = 0x00E035,
  [KEY_SYSRQ]            /* = USB  70 */ = 0x00E037,
  [KEY_RIGHTALT]         /* = USB 230 */ = 0x00E038,
  [KEY_HOME]             /* = USB  74 */ = 0x00E047,
  [KEY_UP]               /* = USB  82 */ = 0x00E048,
  [KEY_PAGEUP]           /* = USB  75 */ = 0x00E049,
  [KEY_LEFT]             /* = USB  80 */ = 0x00E04B,
  [KEY_RIGHT]            /* = USB  79 */ = 0x00E04D,
  [KEY_END]              /* = USB  77 */ = 0x00E04F,
  [KEY_DOWN]             /* = USB  81 */ = 0x00E050,
  [KEY_PAGEDOWN]         /* = USB  78 */ = 0x00E051,
  [KEY_INSERT]           /* = USB  73 */ = 0x00E052,
  [KEY_DELETE]           /* = USB  76 */ = 0x00E053,
  [KEY_KPEQUAL]          /* = USB 103 */ = 0x000059,
  [KEY_PAUSE]            /* = USB  72 */ = 0x00E046,
  [KEY_KPCOMMA]          /* = USB 133 */ = 0x00007E,
  [KEY_HANGEUL]          /* = USB 144 */ = 0x0000F2,
  [KEY_HANJA]            /* = USB 145 */ = 0x0000F1,
  [KEY_YEN]              /* = USB 137 */ = 0x00007D,
  [KEY_LEFTMETA]         /* = USB 227 */ = 0x00E05B,
  [KEY_RIGHTMETA]        /* = USB 231 */ = 0x00E05C,
  [KEY_COMPOSE]          /* = USB 101 */ = 0x00E05D,
  [KEY_F13]              /* = USB 104 */ = 0x00005D,
  [KEY_F14]              /* = USB 105 */ = 0x00005E,
  [KEY_F15]              /* = USB 106 */ = 0x00005F,
  [KEY_PRINT]            /* = USB  70 */ = 0x00E037,
};

const char * linux_to_str[KEY_MAX] =
{
  [KEY_RESERVED]         = "KEY_RESERVED",
  [KEY_ESC]              = "KEY_ESC",
  [KEY_1]                = "KEY_1",
  [KEY_2]                = "KEY_2",
  [KEY_3]                = "KEY_3",
  [KEY_4]                = "KEY_4",
  [KEY_5]                = "KEY_5",
  [KEY_6]                = "KEY_6",
  [KEY_7]                = "KEY_7",
  [KEY_8]                = "KEY_8",
  [KEY_9]                = "KEY_9",
  [KEY_0]                = "KEY_0",
  [KEY_MINUS]            = "KEY_MINUS",
  [KEY_EQUAL]            = "KEY_EQUAL",
  [KEY_BACKSPACE]        = "KEY_BACKSPACE",
  [KEY_TAB]              = "KEY_TAB",
  [KEY_Q]                = "KEY_Q",
  [KEY_W]                = "KEY_W",
  [KEY_E]                = "KEY_E",
  [KEY_R]                = "KEY_R",
  [KEY_T]                = "KEY_T",
  [KEY_Y]                = "KEY_Y",
  [KEY_U]                = "KEY_U",
  [KEY_I]                = "KEY_I",
  [KEY_O]                = "KEY_O",
  [KEY_P]                = "KEY_P",
  [KEY_LEFTBRACE]        = "KEY_LEFTBRACE",
  [KEY_RIGHTBRACE]       = "KEY_RIGHTBRACE",
  [KEY_ENTER]            = "KEY_ENTER",
  [KEY_LEFTCTRL]         = "KEY_LEFTCTRL",
  [KEY_A]                = "KEY_A",
  [KEY_S]                = "KEY_S",
  [KEY_D]                = "KEY_D",
  [KEY_F]                = "KEY_F",
  [KEY_G]                = "KEY_G",
  [KEY_H]                = "KEY_H",
  [KEY_J]                = "KEY_J",
  [KEY_K]                = "KEY_K",
  [KEY_L]                = "KEY_L",
  [KEY_SEMICOLON]        = "KEY_SEMICOLON",
  [KEY_APOSTROPHE]       = "KEY_APOSTROPHE",
  [KEY_GRAVE]            = "KEY_GRAVE",
  [KEY_LEFTSHIFT]        = "KEY_LEFTSHIFT",
  [KEY_BACKSLASH]        = "KEY_BACKSLASH",
  [KEY_Z]                = "KEY_Z",
  [KEY_X]                = "KEY_X",
  [KEY_C]                = "KEY_C",
  [KEY_V]                = "KEY_V",
  [KEY_B]                = "KEY_B",
  [KEY_N]                = "KEY_N",
  [KEY_M]                = "KEY_M",
  [KEY_COMMA]            = "KEY_COMMA",
  [KEY_DOT]              = "KEY_DOT",
  [KEY_SLASH]            = "KEY_SLASH",
  [KEY_RIGHTSHIFT]       = "KEY_RIGHTSHIFT",
  [KEY_KPASTERISK]       = "KEY_KPASTERISK",
  [KEY_LEFTALT]          = "KEY_LEFTALT",
  [KEY_SPACE]            = "KEY_SPACE",
  [KEY_CAPSLOCK]         = "KEY_CAPSLOCK",
  [KEY_F1]               = "KEY_F1",
  [KEY_F2]               = "KEY_F2",
  [KEY_F3]               = "KEY_F3",
  [KEY_F4]               = "KEY_F4",
  [KEY_F5]               = "KEY_F5",
  [KEY_F6]               = "KEY_F6",
  [KEY_F7]               = "KEY_F7",
  [KEY_F8]               = "KEY_F8",
  [KEY_F9]               = "KEY_F9",
  [KEY_F10]              = "KEY_F10",
  [KEY_NUMLOCK]          = "KEY_NUMLOCK",
  [KEY_SCROLLLOCK]       = "KEY_SCROLLLOCK",
  [KEY_KP7]              = "KEY_KP7",
  [KEY_KP8]              = "KEY_KP8",
  [KEY_KP9]              = "KEY_KP9",
  [KEY_KPMINUS]          = "KEY_KPMINUS",
  [KEY_KP4]              = "KEY_KP4",
  [KEY_KP5]              = "KEY_KP5",
  [KEY_KP6]              = "KEY_KP6",
  [KEY_KPPLUS]           = "KEY_KPPLUS",
  [KEY_KP1]              = "KEY_KP1",
  [KEY_KP2]              = "KEY_KP2",
  [KEY_KP3]              = "KEY_KP3",
  [KEY_KP0]              = "KEY_KP0",
  [KEY_KPDOT]            = "KEY_KPDOT",
  [KEY_102ND]            = "KEY_102ND",
  [KEY_F11]              = "KEY_F11",
  [KEY_F12]              = "KEY_F12",
  [KEY_RO]               = "KEY_RO",
  [KEY_HENKAN]           = "KEY_HENKAN",
  [KEY_KATAKANAHIRAGANA] = "KEY_KATAKANAHIRAGANA",
  [KEY_MUHENKAN]         = "KEY_MUHENKAN",
  [KEY_KPENTER]          = "KEY_KPENTER",
  [KEY_RIGHTCTRL]        = "KEY_RIGHTCTRL",
  [KEY_KPSLASH]          = "KEY_KPSLASH",
  [KEY_SYSRQ]            = "KEY_SYSRQ",
  [KEY_RIGHTALT]         = "KEY_RIGHTALT",
  [KEY_HOME]             = "KEY_HOME",
  [KEY_UP]               = "KEY_UP",
  [KEY_PAGEUP]           = "KEY_PAGEUP",
  [KEY_LEFT]             = "KEY_LEFT",
  [KEY_RIGHT]            = "KEY_RIGHT",
  [KEY_END]              = "KEY_END",
  [KEY_DOWN]             = "KEY_DOWN",
  [KEY_PAGEDOWN]         = "KEY_PAGEDOWN",
  [KEY_INSERT]           = "KEY_INSERT",
  [KEY_DELETE]           = "KEY_DELETE",
  [KEY_KPEQUAL]          = "KEY_KPEQUAL",
  [KEY_PAUSE]            = "KEY_PAUSE",
  [KEY_KPCOMMA]          = "KEY_KPCOMMA",
  [KEY_HANGEUL]          = "KEY_HANGEUL",
  [KEY_HANJA]            = "KEY_HANJA",
  [KEY_YEN]              = "KEY_YEN",
  [KEY_LEFTMETA]         = "KEY_LEFTMETA",
  [KEY_RIGHTMETA]        = "KEY_RIGHTMETA",
  [KEY_COMPOSE]          = "KEY_COMPOSE",
  [KEY_F13]              = "KEY_F13",
  [KEY_F14]              = "KEY_F14",
  [KEY_F15]              = "KEY_F15",
  [KEY_PRINT]            = "KEY_PRINT",
};

const char * linux_to_display[KEY_MAX] =
{
  [KEY_RESERVED]         = "Reserved",
  [KEY_ESC]              = "Esc",
  [KEY_1]                = "1",
  [KEY_2]                = "2",
  [KEY_3]                = "3",
  [KEY_4]                = "4",
  [KEY_5]                = "5",
  [KEY_6]                = "6",
  [KEY_7]                = "7",
  [KEY_8]                = "8",
  [KEY_9]                = "9",
  [KEY_0]                = "0",
  [KEY_MINUS]            = "-",
  [KEY_EQUAL]            = "=",
  [KEY_BACKSPACE]        = "Backspace",
  [KEY_TAB]              = "Tab",
  [KEY_Q]                = "Q",
  [KEY_W]                = "W",
  [KEY_E]                = "E",
  [KEY_R]                = "R",
  [KEY_T]                = "T",
  [KEY_Y]                = "Y",
  [KEY_U]                = "U",
  [KEY_I]                = "I",
  [KEY_O]                = "O",
  [KEY_P]                = "P",
  [KEY_LEFTBRACE]        = "{",
  [KEY_RIGHTBRACE]       = "}",
  [KEY_ENTER]            = "Enter",
  [KEY_LEFTCTRL]         = "LCtrl",
  [KEY_A]                = "A",
  [KEY_S]                = "S",
  [KEY_D]                = "D",
  [KEY_F]                = "F",
  [KEY_G]                = "G",
  [KEY_H]                = "H",
  [KEY_J]                = "J",
  [KEY_K]                = "K",
  [KEY_L]                = "L",
  [KEY_SEMICOLON]        = ";",
  [KEY_APOSTROPHE]       = "'",
  [KEY_GRAVE]            = "`",
  [KEY_LEFTSHIFT]        = "LShift",
  [KEY_BACKSLASH]        = "\\",
  [KEY_Z]                = "Z",
  [KEY_X]                = "X",
  [KEY_C]                = "C",
  [KEY_V]                = "V",
  [KEY_B]                = "B",
  [KEY_N]                = "N",
  [KEY_M]                = "M",
  [KEY_COMMA]            = ",",
  [KEY_DOT]              = ".",
  [KEY_SLASH]            = "/",
  [KEY_RIGHTSHIFT]       = "RShift",
  [KEY_KPASTERISK]       = "*",
  [KEY_LEFTALT]          = "LAlt",
  [KEY_SPACE]            = "Space",
  [KEY_CAPSLOCK]         = "CapsLock",
  [KEY_F1]               = "F1",
  [KEY_F2]               = "F2",
  [KEY_F3]               = "F3",
  [KEY_F4]               = "F4",
  [KEY_F5]               = "F5",
  [KEY_F6]               = "F6",
  [KEY_F7]               = "F7",
  [KEY_F8]               = "F8",
  [KEY_F9]               = "F9",
  [KEY_F10]              = "F10",
  [KEY_NUMLOCK]          = "NumLock",
  [KEY_SCROLLLOCK]       = "ScrollLock",
  [KEY_KP7]              = "KP7",
  [KEY_KP8]              = "KP8",
  [KEY_KP9]              = "KP9",
  [KEY_KPMINUS]          = "KPMinus",
  [KEY_KP4]              = "KP4",
  [KEY_KP5]              = "KP5",
  [KEY_KP6]              = "KP6",
  [KEY_KPPLUS]           = "KPPlus",
  [KEY_KP1]              = "KP1",
  [KEY_KP2]              = "KP2",
  [KEY_KP3]              = "KP3",
  [KEY_KP0]              = "KP0",
  [KEY_KPDOT]            = "KPDOT",
  [KEY_102ND]            = "102ND",
  [KEY_F11]              = "F11",
  [KEY_F12]              = "F12",
  [KEY_RO]               = "RO",
  [KEY_HENKAN]           = "Henkan",
  [KEY_KATAKANAHIRAGANA] = "Kana",
  [KEY_MUHENKAN]         = "Muhenkan",
  [KEY_KPENTER]          = "KPEnter",
  [KEY_RIGHTCTRL]        = "RCtrl",
  [KEY_KPSLASH]          = "KPSlash",
  [KEY_SYSRQ]            = "SysRQ",
  [KEY_RIGHTALT]         = "RAlt",
  [KEY_HOME]             = "Home",
  [KEY_UP]               = "↑",
  [KEY_PAGEUP]           = "PageUp",
  [KEY_LEFT]             = "←",
  [KEY_RIGHT]            = "→",
  [KEY_END]              = "End",
  [KEY_DOWN]             = "↓",
  [KEY_PAGEDOWN]         = "PageDown",
  [KEY_INSERT]           = "Insert",
  [KEY_DELETE]           = "Delete",
  [KEY_KPEQUAL]          = "KPEqual",
  [KEY_PAUSE]            = "Pause",
  [KEY_KPCOMMA]          = "KPComma",
  [KEY_HANGEUL]          = "Hangul",
  [KEY_HANJA]            = "Hanja",
  [KEY_YEN]              = "Yen",
  [KEY_LEFTMETA]         = "LWin",
  [KEY_RIGHTMETA]        = "RWin",
  [KEY_COMPOSE]          = "Compose",
  [KEY_F13]              = "F13",
  [KEY_F14]              = "F14",
  [KEY_F15]              = "F15",
  [KEY_PRINT]            = "Print",
};

void initImGuiKeyMap(int * keymap)
{
  keymap[ImGuiKey_Tab        ] = KEY_TAB;
  keymap[ImGuiKey_LeftArrow  ] = KEY_LEFT;
  keymap[ImGuiKey_RightArrow ] = KEY_RIGHT;
  keymap[ImGuiKey_UpArrow    ] = KEY_UP;
  keymap[ImGuiKey_DownArrow  ] = KEY_DOWN;
  keymap[ImGuiKey_PageUp     ] = KEY_PAGEUP;
  keymap[ImGuiKey_PageDown   ] = KEY_PAGEDOWN;
  keymap[ImGuiKey_Home       ] = KEY_HOME;
  keymap[ImGuiKey_End        ] = KEY_END;
  keymap[ImGuiKey_Insert     ] = KEY_INSERT;
  keymap[ImGuiKey_Delete     ] = KEY_DELETE;
  keymap[ImGuiKey_Backspace  ] = KEY_BACKSPACE;
  keymap[ImGuiKey_Space      ] = KEY_SPACE;
  keymap[ImGuiKey_Enter      ] = KEY_ENTER;
  keymap[ImGuiKey_Escape     ] = KEY_SPACE;
  keymap[ImGuiKey_KeyPadEnter] = KEY_KPENTER;
  keymap[ImGuiKey_A          ] = KEY_A;
  keymap[ImGuiKey_C          ] = KEY_C;
  keymap[ImGuiKey_V          ] = KEY_V;
  keymap[ImGuiKey_X          ] = KEY_X;
  keymap[ImGuiKey_Y          ] = KEY_Y;
  keymap[ImGuiKey_Z          ] = KEY_Z;
}
