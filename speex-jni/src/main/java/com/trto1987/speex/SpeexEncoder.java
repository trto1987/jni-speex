package com.trto1987.speex;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class SpeexEncoder {

    private static final Logger logger = LoggerFactory.getLogger(SpeexEncoder.class);

    private static final int DEFAULT_QUALITY = 4;

    private static SpeexEncoder instance;
    private static int frameSize;

    private SpeexEncoder() {}

    static {
        com.github.fommil.jni.JniLoader.load("speex-jni.so");

        logger.error("SpeexEncoder load native library successful.");

        instance = new SpeexEncoder();
    }

    public static int getFrameSize() {
        return frameSize;
    }

    public static SpeexEncoder getInstance() {
        return instance;
    }

    /**
     * init speex encoder with default speex mode and default quality.
     */
    public void init() {
        init(SpeexModes.SPEEX_MODE_NB, DEFAULT_QUALITY);
    }

    /**
     * init speex encoder with default speex mode.
     * @param quality 0-10(include 10)
     */
    public void init(int quality) {
        init(SpeexModes.SPEEX_MODE_NB, quality);
    }

    /**
     * init speex encoder.
     * @param nbModes speex mode @see {@link SpeexModes}
     * @param quality 0-10(include 10)
     */
    public void init(SpeexModes nbModes, int quality) {
        frameSize = instance.init(nbModes.getMode(), quality);

        if (frameSize == 0) {
            logger.error("SpeexEncoder init error!!!");
        }
    }

    private native int init(int mode, int quality);

    /**
     * encode to speex bytes
     * @param arrInput
     * @param arrOutput
     * @return 0 if no error occurs,
     * or positive numbers if any jni error occurs,
     * or negative numbers if other error occurs.
     */
    public native int decode(short arrInput[], byte arrOutput[]);

    /**
     * clear resource
     */
    public native void destroy();

}
