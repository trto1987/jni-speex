package com.trto1987.speex;

public enum SpeexModes {
    /** narrowband mode */
    SPEEX_MODE_NB,
    /** wideband mode */
    SPEEX_MODE_WB,
    /** ultra-wideband mode */
    SPEEX_MODE_UWB,
    ;

    public int getMode() {
        return mode;
    }

    int mode;
}
