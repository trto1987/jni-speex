package com.trto1987.speex;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class SpeexDecoder {

    private static final Logger logger = LoggerFactory.getLogger(SpeexDecoder.class);

    private static SpeexDecoder instance;
    private static int frameSize;

    private SpeexDecoder() {}

    static {
        com.github.fommil.jni.JniLoader.load("speex-jni.so");

        logger.error("SpeexDecoder load native library successful.");

        instance = new SpeexDecoder();
    }

    public static int getFrameSize() {
        return frameSize;
    }

    public static SpeexDecoder getInstance() {
        return instance;
    }

    /**
     * init speex decoder with default speex mode.
     */
    public void init() {
        init(SpeexModes.SPEEX_MODE_NB);
    }

    /**
     * init speex decoder.
     * @param nbModes speex mode @see {@link SpeexModes}
     */
    public void init(SpeexModes nbModes) {
        frameSize = instance.init(nbModes.getMode());

        if (frameSize == 0) {
            logger.error("SpeexDecoder init error!!!");
        }
    }

    private native int init(int mode);

    /**
     * decode speex bytes
     * @param arrInput
     * @param arrOutput
     * @param size
     * @return 0 if no error occurs,
     * or positive numbers if any jni error occurs,
     * or negative numbers if other error occurs.
     */
    public native int decode(byte arrInput[], short arrOutput[], int size);

    /**
     * clear resource
     */
    public native void destroy();

}
