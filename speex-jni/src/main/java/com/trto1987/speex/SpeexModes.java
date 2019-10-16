package com.trto1987.speex;

public enum SpeexModes {
    /** narrowband mode */
    SPEEX_MODE_NB(0),
    /** wideband mode */
    SPEEX_MODE_WB(1),
    /** ultra-wideband mode */
    SPEEX_MODE_UWB(2),
    ;

    SpeexModes(int mode) {
        this.mode = mode;
    }

    public int getMode() {
        return mode;
    }

    int mode;
}
