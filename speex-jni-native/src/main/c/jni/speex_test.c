#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "speex/speex.h"
#include "speex/speex_preprocess.h"
#include "speex/speex_echo.h"

#include "speex/speex_header.h"

// SpeexBits bits;
void *dec_state;
SpeexPreprocessState *preprocess_state;
SpeexEchoState *echo_state;

int C_DENOISE = 1;
int C_NOISE_SUPPRESS = -25;
int frame_size;

/** 初始化 */
int init();

/** 解码
 * @param arr_in 输入数据
 * @param arr_out 输出数据
 * @param size 输入数据长度
 * @return 0 如果没有错误, 非0正值为jni错误，非0负值为其它错误
 */
int decode(char *arr_in, short *arr_out, int size);

/** 清理 */
void close();

int hex_to_int(char c);
void hstr_to_bytes(char *hstr, char *bytes, int length);

int main()
{
    // 初始化测试数据
    char *hstr = "19972f000039ce70001ce738100e738e31341040";
    char arr_in[2048];
    short arr_out[2048];
    int frame_length;

    hstr_to_bytes(hstr, arr_in, 40);

    // 测试
    if ((frame_length = init()) <= 0) {
        fprintf(stderr, "init err");
        return 1;
    }

    if (decode(arr_in, arr_out, frame_length) != 0)
    {
        fprintf(stderr, "decode err");
        return 2;
    }

    SpeexHeader speex_h;
    speex_init_header(&speex_h, 32000, 1, &speex_nb_mode);
    printf("speex_version: %s, speex_version_id: %d\n", speex_h.speex_version, speex_h.speex_version_id);

    close();

    return 0;
}

int init()
{
    /* 初始化比特率结构体 */
    // speex_bits_init(&bits);

    /* 定义并初始化解码器状态 */
    dec_state = speex_decoder_init(&speex_nb_mode);
    if (speex_decoder_ctl(dec_state, SPEEX_GET_FRAME_SIZE, &frame_size) != 0)
    {
        return 0;
    }

    /* 定义预处理及设置 */
    preprocess_state = speex_preprocess_state_init(frame_size, 8000);
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
    echo_state = speex_echo_state_init(frame_size, 800);
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
 * @param thiz
 * @param arr_in 输入数据
 * @param arr_out 输出数据
 * @param size 输入数据长度
 * @return 0 如果没有错误, 非0正值为jni错误，非0负值为其它错误
*/
int decode(char *arr_in, short *arr_out, int size)
{
    SpeexBits bits;

    /* 初始化比特率结构体 */
    speex_bits_init(&bits);

    /* 清空bit结构体，以便解码器处理后面的新一帧音频数据 */
    // speex_bits_reset(&bits);
    speex_bits_read_from(&bits, arr_in, size);
    if (speex_decode_int(dec_state, &bits, arr_out) != 0)
    {
        return -2;
    }
    
    speex_preprocess_run(preprocess_state, arr_out);

    /* 释放内存 */
    speex_bits_destroy(&bits);

    return 0;
}

void close()
{
    /* 销毁释放资源 */
    speex_echo_state_destroy(echo_state);
    speex_preprocess_state_destroy(preprocess_state);
    speex_decoder_destroy(dec_state);
    // speex_bits_destroy(&bits);
}

int hex_to_int(char c)
{
    if (c >= '0' && c <= '9') return (c - '0');
    if (c >= 'A' && c <= 'F') return (c - 'A' + 10);
    if (c >= 'a' && c <= 'f') return (c - 'a' + 10);
    return 0;
}

void hstr_to_bytes(char *hstr, char *bytes, int length)
{
    int i;
    for (i = 0; i < length; i += 2) {
        bytes[i / 2] = (char) ((hex_to_int(hstr[i]) << 4) 
            | hex_to_int(hstr[i + 1]));
    }
}
