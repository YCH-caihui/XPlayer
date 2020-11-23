#include <jni.h>
#include <string>
#include <android/log.h>

extern "C" {
#include "libavformat/avformat.h"

}


double r2d(AVRational rational) {
    return rational.den == 0 ? 0 : (double )rational.num / (double )rational.den ;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_caihui_xplayer_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";

    AVFormatContext  *ic = NULL;
    int re = avformat_open_input(&ic, "storage/emulated/0/DCIM/Camera/a.mp4", NULL, NULL);

    if(re != 0) {
        char buf[1024];
        av_strerror(re, buf, sizeof(buf) - 1);
        __android_log_print(ANDROID_LOG_DEBUG, "caihui", "error %s", buf);
    }

    re = avformat_find_stream_info(ic, 0);

    //总时长
    int totalMs = ic -> duration / AV_TIME_BASE / 1000;
    __android_log_print(ANDROID_LOG_INFO, "ffcaihui",  "duration: %d", totalMs);

    int videoStreamIndex = -1;
    int audioStreamIndex = -1;

    //获取音视频流信息
    for( int i = 0; i < ic->nb_streams; i++ ) {
        AVStream *as = ic->streams[i];
        __android_log_print(ANDROID_LOG_DEBUG, "ffcaihui", " streams index:%d   codecType: %d", i,AVMEDIA_TYPE_VIDEO);
        if(as->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStreamIndex = i;
            //音频信息
            __android_log_print(ANDROID_LOG_INFO, "ffcaihui", "sample_rate: %d", as->codecpar->sample_rate);

        } else if(as->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStreamIndex = i;
            //视频信息
            __android_log_print(ANDROID_LOG_INFO, "ffcaihui", "width: %d  height: %d", as->codecpar->width, as->codecpar->height);
            //帧率
            __android_log_print(ANDROID_LOG_DEBUG, "ffcaihui", "frame %f  frameSize %d", r2d(as->avg_frame_rate), as->codecpar->frame_size);

        }
    }

    //malloc AVPacket 并初始化
    AVPacket  *pkt = av_packet_alloc();

    while(true) {
        int re = av_read_frame(ic, pkt);
        if(re != 0) {
            int ms = 3000;
            long long pos = (double)ms / (double)1000 * r2d(ic->streams[pkt->stream_index]->time_base);
            av_seek_frame(ic, pkt->stream_index, pos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME );
            break;
        }


        if(pkt->stream_index == videoStreamIndex) {
            //视频
            __android_log_print(ANDROID_LOG_INFO, "ffcaihui", "视频");
        } else if(pkt->stream_index == audioStreamIndex) {
            //音频
            __android_log_print(ANDROID_LOG_INFO, "ffcaihui", "音频");
        }

        __android_log_print(ANDROID_LOG_INFO, "ffcaihui", "pktsize: %d  pts:%lld   dts:%lld   realTime:%f ", pkt->size, pkt->pts, pkt->dts,
                            pkt->pts * r2d(ic->streams[pkt->stream_index]->time_base)) * 1000;


        //释放引用计数-1. 为0释放空间
        av_packet_unref(pkt);
    }

    av_packet_free(&pkt);

    if(ic){
        avformat_close_input(&ic);
        ic = nullptr;
    }

    return env->NewStringUTF(hello.c_str());
}