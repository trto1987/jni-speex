package com.trto1987.speex;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class Speex {

    private static final Logger logger = LoggerFactory.getLogger(Speex.class);

    private static Speex instance;
    private static int frameSize;

    private Speex() {}

    static {
        com.github.fommil.jni.JniLoader.load("oecarspeex.so");

        logger.error("Speex load oecarspeex.so successful.");

        instance = new Speex();
        frameSize = instance.init();

        if (frameSize == 0) {
            logger.error("Speex init error.");
        }
    }

    public static int getFrameSize() {
        return frameSize;
    }

    public static Speex getInstance() {
        return instance;
    }

    /**
     * 初始化
     * @return <t>int</t> 每桢数据长度，0为错误
     */
    native int init();

    /**
     * 解码
     * @param arrInput
     * @param arrOutput
     * @param size
     * @return <t>int</t> 0 如果没有错误, 非0正值为jni错误，非0负值为其它错误
     */
    public native int decode(byte arrInput[], short arrOutput[], int size);

//    public native int encode(short lin[], int offset, byte encoded[], int size);

    /**
     * 清理
     */
    native void close();

}
