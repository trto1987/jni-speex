#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "speex/speex.h"

void *encoder_state;

int frame_size;
int qlty;

/** 初始化
 * @param jenv
 * @param jcls
 * @param mode modeID for the defined mode, 0 for narrowband mode, 
 * 1 for wideband mode, 2 for ultra-wideband mode
 * @param quality 0-10(include 10)
 */
JNIEXPORT jint JNICALL
Java_com_trto1987_speex_SpeexEncoder_init(JNIEnv *jenv, jclass jcls, jint mode, jint quality)
{
    qlty = quality;

    /* 初始化解码器 */
    encoder_state = speex_encoder_init(speex_lib_get_mode(mode));
    if (speex_encoder_ctl(encoder_state, SPEEX_GET_FRAME_SIZE, &frame_size) != 0)
    {
        return 0;
    }

    if (speex_encoder_ctl(encoder_state, SPEEX_SET_QUALITY, &qlty) != 0)
    {
        return 0;
    }

    return frame_size;
}

/** 解码
 * @param jenv
 * @param jcls
 * @param arr_in 输入数据
 * @param arr_out 输出数据
 * @return 0 如果没有错误, 非0正值为jni错误，非0负值为其它错误
 */
JNIEXPORT jint JNICALL
Java_com_trto1987_speex_SpeexEncoder_encode(
    JNIEnv *jenv, jobject jcls, jshortArray arr_in, jcharArray arr_out)
{
    jint size;
    jboolean is_copy;
    short *buffer_in;
    char *buffer_out;
    SpeexBits bits;

    buffer_in = (*jenv)->GetShortArrayElements(jenv, arr_in, &is_copy);
    if ((*jenv)->ExceptionOccurred(jenv))
    {
        return 1;
    }

    buffer_out = (short *) calloc(sizeof(char), size);
    if (buffer_out == NULL)
    {
        return -1;
    }

    /* 初始化比特率结构体 */
    speex_bits_init(&bits);

    if (speex_encode_int(encoder_state, buffer_in, &bits) != 0)
    {
        return -2;
    }

    size = speex_bits_write(&bits, buffer_out, speex_bits_nbytes(&bits));

    /* 赋值 */
    (*jenv)->SetCharArrayRegion(jenv, arr_out, 0, size, buffer_out);
    if ((*jenv)->ExceptionOccurred(jenv))
    {
        return 2;
    }

    /* 释放内存 */ 
    (*jenv)->ReleaseShortArrayElements(jenv, arr_in, buffer_in, 0);
    if ((*jenv)->ExceptionOccurred(jenv))
    {
        return 3;
    }
    free(buffer_out);
    speex_bits_destroy(&bits);

    return 0;
}

/** 清理
 * @param jenv
 * @param jcls
 */
JNIEXPORT void JNICALL
Java_com_trto1987_speex_SpeexEncoder_destroy(JNIEnv *jenv, jclass jcls)
{
    /* 销毁释放资源 */
    speex_encoder_destroy(encoder_state);
}
