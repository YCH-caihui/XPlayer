#include <jni.h>
#include <string>
#include <android/log.h>

extern "C" {
#include "libavformat/avformat.h"

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


    av_dump_format(ic, 0, nullptr, 0);


    //获取音视频流信息
    for( int i = 0; i < ic->nb_streams; i++ ) {
        AVStream *as = ic->streams[i];
        if(as->codecpar->codec_type = AVMEDIA_TYPE_AUDIO) {
            //音频信息
        } else if(as->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            //视频信息
        }
    }



    if(ic){
        avformat_close_input(&ic);
        ic = nullptr;
    }

    return env->NewStringUTF(hello.c_str());
}