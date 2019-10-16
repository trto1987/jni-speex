#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "speex/speex.h"
#include "speex/speex_preprocess.h"
#include "speex/speex_echo.h"

void *dec_state;
SpeexPreprocessState *preprocess_state;
SpeexEchoState *echo_state;

int C_DENOISE = 1;
int C_NOISE_SUPPRESS = -25;
float echo_filter_rate = 0.1;

int frame_size;
int sampling_rate;

/** 初始化
 * @param jenv
 * @param jcls
 * @param mode modeID for the defined mode, 0 for narrowband mode, 
 * 1 for wideband mode, 2 for ultra-wideband mode
 */
JNIEXPORT jint JNICALL
Java_com_trto1987_speex_SpeexDecoder_init(JNIEnv *jenv, jclass jcls, jint mode)
{
    /* 定义并初始化解码器状态 */
    dec_state = speex_decoder_init(speex_lib_get_mode(mode));
    if (speex_decoder_ctl(dec_state, SPEEX_GET_FRAME_SIZE, &frame_size) != 0)
    {
        return 0;
    }

    /* 定义预处理及设置 */
    if (speex_decoder_ctl(dec_state, SPEEX_GET_SAMPLING_RATE, &sampling_rate) != 0)
    {
        return 0;
    }
    preprocess_state = speex_preprocess_state_init(frame_size, sampling_rate);
    if (preprocess_state == NULL)
    {
        return 0;
    }

    if (speex_preprocess_ctl(preprocess_state, SPEEX_PREPROCESS_SET_DENOISE, &C_DENOISE) != 0)
    {
        return 0;
    }
    if (speex_preprocess_ctl(
        preprocess_state, SPEEX_PREPROCESS_SET_NOISE_SUPPRESS, &C_NOISE_SUPPRESS) != 0)
    {
        return 0;
    }

    /* 回声消除 */
    echo_state = speex_echo_state_init(frame_size, echo_filter_rate * sampling_rate);
    if (echo_state == NULL)
    {
        return 0;
    }
    if (speex_preprocess_ctl(preprocess_state, SPEEX_PREPROCESS_SET_ECHO_STATE, echo_state) != 0)
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
 * @param size 输入数据长度
 * @return 0 如果没有错误, 非0正值为jni错误，非0负值为其它错误
 */
JNIEXPORT jint JNICALL
Java_com_trto1987_speex_SpeexDecoder_decode(
    JNIEnv *jenv, jobject jcls, 
    jbyteArray arr_in, jshortArray arr_out, jint size)
{
    jboolean is_copy;
    jbyte *buffer_in;
    short *buffer_out;
    SpeexBits bits;

    buffer_in = (*jenv)->GetByteArrayElements(jenv, arr_in, &is_copy);
    if ((*jenv)->ExceptionOccurred(jenv))
    {
        return 1;
    }

    buffer_out = (short *) calloc(sizeof(short), frame_size);
    if (buffer_out == NULL)
    {
        return -1;
    }

    /* 初始化比特率结构体 */
    speex_bits_init(&bits);

    speex_bits_read_from(&bits, buffer_in, size);
    if (speex_decode_int(dec_state, &bits, buffer_out) != 0)
    {
        return -2;
    }
    
    speex_preprocess_run(preprocess_state, buffer_out);

    /* 赋值 */
    (*jenv)->SetShortArrayRegion(jenv, arr_out, 0, frame_size, buffer_out);
    if ((*jenv)->ExceptionOccurred(jenv))
    {
        return 2;
    }

    /* 释放内存 */ 
    (*jenv)->ReleaseByteArrayElements(jenv, arr_in, buffer_in, 0);
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
Java_com_trto1987_speex_SpeexDecoder_destroy(JNIEnv *jenv, jclass jcls)
{
    /* 销毁释放资源 */
    speex_echo_state_destroy(echo_state);
    speex_preprocess_state_destroy(preprocess_state);
    speex_decoder_destroy(dec_state);
}
