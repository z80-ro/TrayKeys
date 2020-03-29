/* 
 * MIT License
 * z80.ro
*/
#include "worker.h"

void worker::run() {
    uint32_t code;
    int selres;
    fd_set set;
    struct timeval timeout;

    device_changed = true;
    f = -1;

    while (1) {
        if (device_changed) {
            device_changed = false;
            if (f != -1) close(f);
            f = open("/dev/input/"+device.toLocal8Bit(), O_RDONLY);
            qDebug() << "New device opened: " << "/dev/input/"+device.toLocal8Bit() << " - result: " << f;
        }
        if (f != -1) {
            FD_ZERO(&set);
            FD_SET(f, &set);
            timeout.tv_sec = 0;
            timeout.tv_usec = 100000;   // every one tenth of a second

            selres = select(f+1, &set, nullptr, nullptr, &timeout);
            if (selres > 0) {
                if ((read_size = read(f, e, sizeof(struct input_event))) == sizeof(struct input_event)) {
                    //qDebug("v0: %x, t0: %x, v1: %x, t1: %x\n", e[0].value, e[0].type, e[1].value, e[1].type);
                    if (e[0].type == 1 && e[0].code < 0x300) {

                        code = e[0].code;

                        // Process helper keys first
                        if (code == 29 || code == 97) {
                            if (e[0].value) special_keys_state |= KCTRL;
                            else special_keys_state &= ~KCTRL;
                        } else if (code == 56 || code == 100) {
                            if (e[0].value) special_keys_state |= KALT;
                            else special_keys_state &= ~KALT;
                        } else if (code == 42 || code == 54) {
                            if (e[0].value) special_keys_state |= KSHIFT;
                            else special_keys_state &= ~KSHIFT;
                        } else {
                            if (!translate(code)) emit return_read(ar[code], e[0].value, special_keys_state);
                        }
                        qDebug() << "Value: " << e[0].code << ", Key: " << ar[e[0].code] << ", special keys: " << special_keys_state;
                    }
                };
            }
        }
    }
};

void worker::setup() {
    special_keys_state = 0;
}

void worker::restart_capture() {
    device_changed = true;
}

bool worker::translate(uint32_t code) {
    if (special_keys_state != 0) {
        // grave-tilda
        if (code == 41 && special_keys_state & KSHIFT) {
            emit return_read("~", e[0].value, special_keys_state & ~KSHIFT);
            return true;
        }
        // 1
        if (code == 2 && special_keys_state & KSHIFT) {
            emit return_read("!", e[0].value, special_keys_state & ~KSHIFT);
            return true;
        }
        // 2
        if (code == 3 && special_keys_state & KSHIFT) {
            emit return_read("@", e[0].value, special_keys_state & ~KSHIFT);
            return true;
        }
        // 3
        if (code == 4 && special_keys_state & KSHIFT) {
            emit return_read("#", e[0].value, special_keys_state & ~KSHIFT);
            return true;
        }
        // 4
        if (code == 5 && special_keys_state & KSHIFT) {
            emit return_read("$", e[0].value, special_keys_state & ~KSHIFT);
            return true;
        }
        // 5
        if (code == 6 && special_keys_state & KSHIFT) {
            emit return_read("%", e[0].value, special_keys_state & ~KSHIFT);
            return true;
        }
        // 6
        if (code == 7 && special_keys_state & KSHIFT) {
            emit return_read("^", e[0].value, special_keys_state & ~KSHIFT);
            return true;
        }
        // 7
        if (code == 8 && special_keys_state & KSHIFT) {
            emit return_read("&", e[0].value, special_keys_state & ~KSHIFT);
            return true;
        }
        // 8
        if (code == 9 && special_keys_state & KSHIFT) {
            emit return_read("*", e[0].value, special_keys_state & ~KSHIFT);
            return true;
        }
        // 9
        if (code == 10 && special_keys_state & KSHIFT) {
            emit return_read("(", e[0].value, special_keys_state & ~KSHIFT);
            return true;
        }
        // 0
        if (code == 11 && special_keys_state & KSHIFT) {
            emit return_read(")", e[0].value, special_keys_state & ~KSHIFT);
            return true;
        }
        // -
        if (code == 12 && special_keys_state & KSHIFT) {
            emit return_read("_", e[0].value, special_keys_state & ~KSHIFT);
            return true;
        }
        // =
        if (code == 13 && special_keys_state & KSHIFT) {
            emit return_read("+", e[0].value, special_keys_state & ~KSHIFT);
            return true;
        }
        // [
        if (code == 26 && special_keys_state & KSHIFT) {
            emit return_read("{", e[0].value, special_keys_state & ~KSHIFT);
            return true;
        }
        // ]
        if (code == 27 && special_keys_state & KSHIFT) {
            emit return_read("}", e[0].value, special_keys_state & ~KSHIFT);
            return true;
        }
        // ;
        if (code == 39 && special_keys_state & KSHIFT) {
            emit return_read(":", e[0].value, special_keys_state & ~KSHIFT);
            return true;
        }
        // '
        if (code == 40 && special_keys_state & KSHIFT) {
            emit return_read("\"", e[0].value, special_keys_state & ~KSHIFT);
            return true;
        }
        // backslash
        if (code == 43 && special_keys_state & KSHIFT) {
            emit return_read("|", e[0].value, special_keys_state & ~KSHIFT);
            return true;
        }
        // ,
        if (code == 51 && special_keys_state & KSHIFT) {
            emit return_read("<", e[0].value, special_keys_state & ~KSHIFT);
            return true;
        }
        // .
        if (code == 52 && special_keys_state & KSHIFT) {
            emit return_read(">", e[0].value, special_keys_state & ~KSHIFT);
            return true;
        }
        // slash
        if (code == 53 && special_keys_state & KSHIFT) {
            emit return_read("?", e[0].value, special_keys_state & ~KSHIFT);
            return true;
        }
    }
    return false;
}

void worker::set_strings() {
    ar[1] = "ESC";
    ar[2] = "1";
    ar[3] = "2";
    ar[4] = "3";
    ar[5] = "4";
    ar[6] = "5";
    ar[7] = "6";
    ar[8] = "7";
    ar[9] = "8";
    ar[10] = "9";
    ar[11] = "0";
    ar[12] = "-";
    ar[13] = "=";
    ar[14] = "BACKSPACE";
    ar[15] = "TAB";
    ar[16] = "Q";
    ar[17] = "W";
    ar[18] = "E";
    ar[19] = "R";
    ar[20] = "T";
    ar[21] = "Y";
    ar[22] = "U";
    ar[23] = "I";
    ar[24] = "O";
    ar[25] = "P";
    ar[26] = "[";
    ar[27] = "]";
    ar[28] = "ENTER";
    ar[29] = "LEFTCTRL";
    ar[30] = "A";
    ar[31] = "S";
    ar[32] = "D";
    ar[33] = "F";
    ar[34] = "G";
    ar[35] = "H";
    ar[36] = "J";
    ar[37] = "K";
    ar[38] = "L";
    ar[39] = ";";
    ar[40] = "'";
    ar[41] = "`";
    ar[42] = "LEFTSHIFT";
    ar[43] = "\\";
    ar[44] = "Z";
    ar[45] = "X";
    ar[46] = "C";
    ar[47] = "V";
    ar[48] = "B";
    ar[49] = "N";
    ar[50] = "M";
    ar[51] = ",";
    ar[52] = ".";
    ar[53] = "/";
    ar[54] = "RIGHTSHIFT";
    ar[55] = "KPASTERISK";
    ar[56] = "LEFTALT";
    ar[57] = "SPACE";
    ar[58] = "CAPSLOCK";
    ar[59] = "F1";
    ar[60] = "F2";
    ar[61] = "F3";
    ar[62] = "F4";
    ar[63] = "F5";
    ar[64] = "F6";
    ar[65] = "F7";
    ar[66] = "F8";
    ar[67] = "F9";
    ar[68] = "F10";
    ar[69] = "NUMLOCK";
    ar[70] = "SCROLLLOCK";
    ar[71] = "KP7";
    ar[72] = "KP8";
    ar[73] = "KP9";
    ar[74] = "KP-";
    ar[75] = "KP4";
    ar[76] = "KP5";
    ar[77] = "KP6";
    ar[78] = "KP+";
    ar[79] = "KP1";
    ar[80] = "KP2";
    ar[81] = "KP3";
    ar[82] = "KP0";
    ar[83] = "KP.";
    ar[85] = "ZENKAKUHANKAKU";
    ar[86] = "102ND";
    ar[87] = "F11";
    ar[88] = "F12";
    ar[89] = "RO";
    ar[90] = "KATAKANA";
    ar[91] = "HIRAGANA";
    ar[92] = "HENKAN";
    ar[93] = "KATAKANAHIRAGANA";
    ar[94] = "MUHENKAN";
    ar[95] = "KPJPCOMMA";
    ar[96] = "KPENTER";
    ar[97] = "RIGHTCTRL";
    ar[98] = "KPSLASH";
    ar[99] = "SYSRQ";
    ar[100] = "RIGHTALT";
    ar[101] = "LINEFEED";
    ar[102] = "HOME";
    ar[103] = "UPP";
    ar[104] = "PAGEUP";
    ar[105] = "LEFT";
    ar[106] = "RIGHT";
    ar[107] = "END";
    ar[108] = "DOWN";
    ar[109] = "PAGEDOWN";
    ar[110] = "INSERT";
    ar[111] = "DELETE";
    ar[112] = "MACRO";
    ar[113] = "MUTE";
    ar[114] = "VOLUMEDOWN";
    ar[115] = "VOLUMEUP";
    ar[116] = "POWER";
    ar[117] = "KPEQUAL";
    ar[118] = "KPPLUSMINUS";
    ar[119] = "PAUSE";
    ar[120] = "SCALE";
    ar[121] = "KPCOMMA";
    ar[122] = "HANGEUL";
    ar[123] = "HANJA";
    ar[124] = "YEN";
    ar[125] = "LEFTMETA";
    ar[126] = "RIGHTMETA";
    ar[127] = "COMPOSE";
    ar[128] = "STOP";
    ar[129] = "AGAIN";
    ar[130] = "PROPS";
    ar[131] = "UNDO";
    ar[132] = "FRONT";
    ar[133] = "COPY";
    ar[134] = "OPEN";
    ar[135] = "PASTE";
    ar[136] = "FIND";
    ar[137] = "CUT";
    ar[138] = "HELP";
    ar[139] = "MENU";
    ar[140] = "CALC";
    ar[141] = "SETUP";
    ar[142] = "SLEEP";
    ar[143] = "WAKEUP";
    ar[144] = "FILE";
    ar[145] = "SENDFILE";
    ar[146] = "DELETEFILE";
    ar[147] = "XFER";
    ar[148] = "PROG1";
    ar[149] = "PROG2";
    ar[150] = "WWW";
    ar[151] = "MSDOS";
    ar[152] = "COFFEE";
    ar[153] = "DIRECTION";
    ar[154] = "CYCLEWINDOWS";
    ar[155] = "MAIL";
    ar[156] = "BOOKMARKS";
    ar[157] = "COMPUTER";
    ar[158] = "BACK";
    ar[159] = "FORWARD";
    ar[160] = "CLOSECD";
    ar[161] = "EJECTCD";
    ar[162] = "EJECTCLOSECD";
    ar[163] = "NEXTSONG";
    ar[164] = "PLAYPAUSE";
    ar[165] = "PREVIOUSSONG";
    ar[166] = "STOPCD";
    ar[167] = "RECORD";
    ar[168] = "REWIND";
    ar[169] = "PHONE";
    ar[170] = "ISO";
    ar[171] = "CONFIG";
    ar[172] = "HOMEPAGE";
    ar[173] = "REFRESH";
    ar[174] = "EXIT";
    ar[175] = "MOVE";
    ar[176] = "EDIT";
    ar[177] = "SCROLLUP";
    ar[178] = "SCROLLDOWN";
    ar[179] = "KPLEFTPAREN";
    ar[180] = "KPRIGHTPAREN";
    ar[181] = "NEW";
    ar[182] = "REDO";
    ar[183] = "F13";
    ar[184] = "F14";
    ar[185] = "F15";
    ar[186] = "F16";
    ar[187] = "F17";
    ar[188] = "F18";
    ar[189] = "F19";
    ar[190] = "F20";
    ar[191] = "F21";
    ar[192] = "F22";
    ar[193] = "F23";
    ar[194] = "F24";
    ar[200] = "PLAYCD";
    ar[201] = "PAUSECD";
    ar[202] = "PROG3";
    ar[203] = "PROG4";
    ar[204] = "DASHBOARD";
    ar[205] = "SUSPEND";
    ar[206] = "CLOSE";
    ar[207] = "PLAY";
    ar[208] = "FASTFORWARD";
    ar[209] = "BASSBOOST";
    ar[210] = "PRINT";
    ar[211] = "HP";
    ar[212] = "CAMERA";
    ar[213] = "SOUND";
    ar[214] = "QUESTION";
    ar[215] = "EMAIL";
    ar[216] = "CHAT";
    ar[217] = "SEARCH";
    ar[218] = "CONNECT";
    ar[219] = "FINANCE";
    ar[220] = "SPORT";
    ar[221] = "SHOP";
    ar[222] = "ALTERASE";
    ar[223] = "CANCEL";
    ar[224] = "BRIGHTNESSDOWN";
    ar[225] = "BRIGHTNESSUP";
    ar[226] = "MEDIA";
    ar[227] = "SWITCHVIDEOMODE";
    ar[228] = "KBDILLUMTOGGLE";
    ar[229] = "KBDILLUMDOWN";
    ar[230] = "KBDILLUMUP";
    ar[231] = "SEND";
    ar[232] = "REPLY";
    ar[233] = "FORWARDMAIL";
    ar[234] = "SAVE";
    ar[235] = "DOCUMENTS";
    ar[236] = "BATTERY";
    ar[237] = "BLUETOOTH";
    ar[238] = "WLAN";
    ar[239] = "UWB";
    ar[240] = "UNKNOWN";
    ar[241] = "VIDEO_NEXT";
    ar[242] = "VIDEO_PREV";
    ar[243] = "BRIGHTNESS_CYCLE";
    ar[244] = "BRIGHTNESS_ZERO";
    ar[245] = "DISPLAY_OFF";
    ar[246] = "WWAN";
    ar[247] = "RFKILL";
    ar[248] = "MICMUTE";
    ar[0x160] = "OK";
    ar[0x161] = "SELECT";
    ar[0x162] = "GOTO";
    ar[0x163] = "CLEAR";
    ar[0x164] = "POWER2";
    ar[0x165] = "OPTION";
    ar[0x166] = "INFO";
    ar[0x167] = "TIME";
    ar[0x168] = "VENDOR";
    ar[0x169] = "ARCHIVE";
    ar[0x16a] = "PROGRAM";
    ar[0x16b] = "CHANNEL";
    ar[0x16c] = "FAVORITES";
    ar[0x16d] = "EPG";
    ar[0x16e] = "PVR";
    ar[0x16f] = "MHP";
    ar[0x170] = "LANGUAGE";
    ar[0x171] = "TITLE";
    ar[0x172] = "SUBTITLE";
    ar[0x173] = "ANGLE";
    ar[0x174] = "ZOOM";
    ar[0x175] = "MODE";
    ar[0x176] = "KEYBOARD";
    ar[0x177] = "SCREEN";
    ar[0x178] = "PC";
    ar[0x179] = "TV";
    ar[0x17a] = "TV2";
    ar[0x17b] = "VCR";
    ar[0x17c] = "VCR2";
    ar[0x17d] = "SAT";
    ar[0x17e] = "SAT2";
    ar[0x17f] = "CD";
    ar[0x180] = "TAPE";
    ar[0x181] = "RADIO";
    ar[0x182] = "TUNER";
    ar[0x183] = "PLAYER";
    ar[0x184] = "TEXT";
    ar[0x185] = "DVD";
    ar[0x186] = "AUX";
    ar[0x187] = "MP3";
    ar[0x188] = "AUDIO";
    ar[0x189] = "VIDEO";
    ar[0x18a] = "DIRECTORY";
    ar[0x18b] = "LIST";
    ar[0x18c] = "MEMO";
    ar[0x18d] = "CALENDAR";
    ar[0x18e] = "RED";
    ar[0x18f] = "GREEN";
    ar[0x190] = "YELLOW";
    ar[0x191] = "BLUE";
    ar[0x192] = "CHANNELUP";
    ar[0x193] = "CHANNELDOWN";
    ar[0x194] = "FIRST";
    ar[0x195] = "LAST";
    ar[0x196] = "AB";
    ar[0x197] = "NEXT";
    ar[0x198] = "RESTART";
    ar[0x199] = "SLOW";
    ar[0x19a] = "SHUFFLE";
    ar[0x19b] = "BREAK";
    ar[0x19c] = "PREVIOUS";
    ar[0x19d] = "DIGITS";
    ar[0x19e] = "TEEN";
    ar[0x19f] = "TWEN";
    ar[0x1a0] = "VIDEOPHONE";
    ar[0x1a1] = "GAMES";
    ar[0x1a2] = "ZOOMIN";
    ar[0x1a3] = "ZOOMOUT";
    ar[0x1a4] = "ZOOMRESET";
    ar[0x1a5] = "WORDPROCESSOR";
    ar[0x1a6] = "EDITOR";
    ar[0x1a7] = "SPREADSHEET";
    ar[0x1a8] = "GRAPHICSEDITOR";
    ar[0x1a9] = "PRESENTATION";
    ar[0x1aa] = "DATABASE";
    ar[0x1ab] = "NEWS";
    ar[0x1ac] = "VOICEMAIL";
    ar[0x1ad] = "ADDRESSBOOK";
    ar[0x1ae] = "MESSENGER";
    ar[0x1af] = "DISPLAYTOGGLE";
    ar[0x1b0] = "SPELLCHECK";
    ar[0x1b1] = "LOGOFF";
    ar[0x1b2] = "DOLLAR";
    ar[0x1b3] = "EURO";
    ar[0x1b4] = "FRAMEBACK";
    ar[0x1b5] = "FRAMEFORWARD";
    ar[0x1b6] = "CONTEXT_MENU";
    ar[0x1b7] = "MEDIA_REPEAT";
    ar[0x1b8] = "10CHANNELSUP";
    ar[0x1b9] = "10CHANNELSDOWN";
    ar[0x1ba] = "IMAGES";
    ar[0x1c0] = "DEL_EOL";
    ar[0x1c1] = "DEL_EOS";
    ar[0x1c2] = "INS_LINE";
    ar[0x1c3] = "DEL_LINE";
    ar[0x1d0] = "FN";
    ar[0x1d1] = "FN_ESC";
    ar[0x1d2] = "FN_F1";
    ar[0x1d3] = "FN_F2";
    ar[0x1d4] = "FN_F3";
    ar[0x1d5] = "FN_F4";
    ar[0x1d6] = "FN_F5";
    ar[0x1d7] = "FN_F6";
    ar[0x1d8] = "FN_F7";
    ar[0x1d9] = "FN_F8";
    ar[0x1da] = "FN_F9";
    ar[0x1db] = "FN_F10";
    ar[0x1dc] = "FN_F11";
    ar[0x1dd] = "FN_F12";
    ar[0x1de] = "FN_1";
    ar[0x1df] = "FN_2";
    ar[0x1e0] = "FN_D";
    ar[0x1e1] = "FN_E";
    ar[0x1e2] = "FN_F";
    ar[0x1e3] = "FN_S";
    ar[0x1e4] = "FN_B";
    ar[0x1f1] = "BRL_DOT1";
    ar[0x1f2] = "BRL_DOT2";
    ar[0x1f3] = "BRL_DOT3";
    ar[0x1f4] = "BRL_DOT4";
    ar[0x1f5] = "BRL_DOT5";
    ar[0x1f6] = "BRL_DOT6";
    ar[0x1f7] = "BRL_DOT7";
    ar[0x1f8] = "BRL_DOT8";
    ar[0x1f9] = "BRL_DOT9";
    ar[0x1fa] = "BRL_DOT10";
    ar[0x200] = "NUMERIC_0";
    ar[0x201] = "NUMERIC_1";
    ar[0x202] = "NUMERIC_2";
    ar[0x203] = "NUMERIC_3";
    ar[0x204] = "NUMERIC_4";
    ar[0x205] = "NUMERIC_5";
    ar[0x206] = "NUMERIC_6";
    ar[0x207] = "NUMERIC_7";
    ar[0x208] = "NUMERIC_8";
    ar[0x209] = "NUMERIC_9";
    ar[0x20a] = "NUMERIC_STAR";
    ar[0x20b] = "NUMERIC_POUND";
    ar[0x210] = "CAMERA_FOCUS";
    ar[0x211] = "WPS_BUTTON";
    ar[0x212] = "TOUCHPAD_TOGGLE";
    ar[0x213] = "TOUCHPAD_ON";
    ar[0x214] = "TOUCHPAD_OFF";
    ar[0x215] = "CAMERA_ZOOMIN";
    ar[0x216] = "CAMERA_ZOOMOUT";
    ar[0x217] = "CAMERA_UP";
    ar[0x218] = "CAMERA_DOWN";
    ar[0x219] = "CAMERA_LEFT";
    ar[0x21a] = "CAMERA_RIGHT";
    ar[0x21b] = "ATTENDANT_ON";
    ar[0x21c] = "ATTENDANT_OFF";
    ar[0x21d] = "ATTENDANT_TOGGLE";
    ar[0x21e] = "LIGHTS_TOGGLE";
    ar[0x230] = "ALS_TOGGLE";
    ar[0x2ff] = "MAX";
}

void worker::set_device(QString device) {
    this->device = device;
    this->new_device_was_set = 1;
}
