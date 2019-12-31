#include "./include/kj_player.h"
#include <string>
#include <android/native_window_jni.h>
#include <zconf.h>
#include <android/log.h>

extern "C" {
#include "libavcodec/avcodec.h"
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
}


#define TAG "kjplayer"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)

/**
 *  获取视频文件中的视频流，渲染到surface上
 */
extern "C"
JNIEXPORT void JNICALL
Java_com_kbjay_ffmepegdemo_KJPlayer_nativePlayVideo(JNIEnv *env, jobject instance, jstring path_,
                                                    jobject surface) {

    const char *path = env->GetStringUTFChars(path_, JNI_FALSE);
    LOGV("start play: %s", path);
    //1：初始化format context获取信息 (avformat_open_input 初始化AVFormatConext)
    AVFormatContext *pAvFormatContext = avformat_alloc_context();
    //设置读取文件的配置参数
    AVDictionary *options = NULL;
    av_dict_set(&options, "timeout", "3000000", 0);
    avformat_open_input(&pAvFormatContext, path, NULL, &options);

    //2：找到视频流Id
    avformat_find_stream_info(pAvFormatContext, NULL);
    //找到视频流的id
    int video_stream_id = -1;
    for (int i = 0; i < pAvFormatContext->nb_streams; ++i) {
        if (pAvFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_id = i;
            break;
        }
    }
    //打印duration
    int duration = static_cast<int>(pAvFormatContext->duration);
    LOGV("duration: %d", duration);

    //3：根据id获取视频流(avcodec_open2 初始化AVCodecContext)
    AVCodecParameters *pAVCodecParameters = pAvFormatContext->streams[video_stream_id]->codecpar;
    AVCodec *pCodec = avcodec_find_decoder(pAVCodecParameters->codec_id);
    AVCodecContext *pAVCodecContext = avcodec_alloc_context3(pCodec);
    //将AVCodecParameters结构体中码流参数拷贝到AVCodecContext结构体中
    avcodec_parameters_to_context(pAVCodecContext, pAVCodecParameters);
    //初始化pAVCodecContext
    avcodec_open2(pAVCodecContext, pCodec, NULL);

    //使用surface初始化nativeWindow用于绘制
    ANativeWindow *pANativeWindow = ANativeWindow_fromSurface(env, surface);
    ANativeWindow_setBuffersGeometry(pANativeWindow, pAVCodecContext->width,
                                     pAVCodecContext->height, WINDOW_FORMAT_RGBA_8888);

    SwsContext *pSwsContext = sws_getContext(pAVCodecContext->width, pAVCodecContext->height,
                                             pAVCodecContext->pix_fmt,
                                             pAVCodecContext->width, pAVCodecContext->height,
                                             AV_PIX_FMT_RGBA,
                                             SWS_BILINEAR, NULL, NULL, NULL);

    AVPacket *pPacket = av_packet_alloc();
    ANativeWindow_Buffer outBuffer;

    uint8_t *dst_data[4];
    int dst_linesize[4];

    av_image_alloc(dst_data, dst_linesize, pAVCodecContext->width, pAVCodecContext->height,
                   AV_PIX_FMT_RGBA, 1);

    while (av_read_frame(pAvFormatContext, pPacket) >= 0) {
        avcodec_send_packet(pAVCodecContext, pPacket);
        AVFrame *pFrame = av_frame_alloc();
        int ret = avcodec_receive_frame(pAVCodecContext, pFrame);
        if (ret == AVERROR(EAGAIN)) {
            continue;
        } else if (ret < 0) {
            break;
        }

        if (pPacket->stream_index == video_stream_id) {
            if (ret == 0) {
                ANativeWindow_lock(pANativeWindow, &outBuffer, NULL);
                sws_scale(pSwsContext, pFrame->data, pFrame->linesize, 0, pFrame->height, dst_data,
                          dst_linesize);

                int32_t destStrede = outBuffer.stride * 4;
                uint8_t *src_data = dst_data[0];
                int src_linesize = dst_linesize[0];

                uint8_t *dst = (uint8_t *) outBuffer.bits;

                for (int i = 0; i < outBuffer.height; ++i) {
                    memcpy(dst + i * destStrede, src_data + i * src_linesize, destStrede);
                }
                ANativeWindow_unlockAndPost(pANativeWindow);
                //16ms
                usleep(1000 * 16);

                av_frame_free(&pFrame);
            }
        }
    }

    av_freep(&dst_data[0]);
    ANativeWindow_release(pANativeWindow);
    avcodec_close(pAVCodecContext);
    avformat_free_context(pAvFormatContext);
    env->ReleaseStringUTFChars(path_, path);
}