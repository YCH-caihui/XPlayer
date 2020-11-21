#include <jni.h>
#include <string>

extern "C" {
#include "libavformat/avformat.h"
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_caihui_xplayer_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";

    AVFormatContext * formatContext = avformat_alloc_context();


    return env->NewStringUTF(hello.c_str());
}