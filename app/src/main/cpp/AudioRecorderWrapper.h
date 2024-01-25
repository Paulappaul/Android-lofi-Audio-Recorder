//
// Created by alcin on 12/18/2023.
//

#ifndef OBOE_AUDIORECORDERWRAPPER_H
#define OBOE_AUDIORECORDERWRAPPER_H

#include <jni.h>

extern "C" {
JNIEXPORT jlong JNICALL
Java_com_example_cosmo388_MainActivity_createAudioRecorder(JNIEnv *env, jobject instance);

JNIEXPORT void JNICALL
Java_com_example_cosmo388_MainActivity_startRecording(JNIEnv *env, jobject instance,
                                                      jlong recorderHandle);

JNIEXPORT void JNICALL
Java_com_example_cosmo388_MainActivity_startPlayback(JNIEnv *env, jobject instance,
                                                     jlong recorderHandle);

JNIEXPORT void JNICALL
Java_com_example_cosmo388_MainActivity_stopRecording(JNIEnv *env, jobject instance,
                                                     jlong recorderHandle);

JNIEXPORT void JNICALL
Java_com_example_cosmo388_MainActivity_getPath(JNIEnv *env, jobject instance, jstring storagePath,
                                               jlong recorderHandle);

JNIEXPORT void JNICALL
Java_com_example_cosmo388_MainActivity_writeAudio(JNIEnv *env, jobject instance, jstring date,
                                                  jlong recorderHandle);

JNIEXPORT void JNICALL
Java_com_example_cosmo388_MainActivity_getIRPath(JNIEnv *env, jobject instance, jstring irPath, jlong recorderHandle);

JNIEXPORT void JNICALL
Java_com_example_cosmo388_MainActivity_setVolume(JNIEnv *env, jobject instance, jlong recorder_handle,
                                                 jfloat volume, jint channel);


}




#endif //OBOE_AUDIORECORDERWRAPPER_H
