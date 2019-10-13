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

        logger.error("Speex load native library successful.");

        instance = new SpeexDecoder();
        frameSize = instance.init();

        if (frameSize == 0) {
            logger.error("Speex init error!!!");
        }
    }

    public static int getFrameSize() {
        return frameSize;
    }

    public static SpeexDecoder getInstance() {
        return instance;
    }

    private native int init();

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

//    public native int encode(short lin[], int offset, byte encoded[], int size);

    /**
     * clear resource
     */
    public native void destroy();

}
