//
// Created by kb_jay on 2019-12-09.
//
#include <jni.h>

#ifndef FFMEPEGDEMO_KJ_PLAYER_H
#define FFMEPEGDEMO_KJ_PLAYER_H

extern "C" {

void Java_com_kbjay_ffmepegdemo_KJPlayer_nativePlayVideo(JNIEnv *env, jobject obj, jstring path,
                                                         jobject surface);
};

#endif //FFMEPEGDEMO_KJ_PLAYER_H
