// AudioRecorderWrapper.cpp
#include "AudioRecorder.cpp"
#include "AudioRecorderWrapper.h"

extern "C" {

JNIEXPORT jlong JNICALL
Java_com_example_cosmo388_MainActivity_createAudioRecorder(JNIEnv *env, jobject instance) {
    return reinterpret_cast<jlong>(new AudioRecorder());
}

JNIEXPORT void JNICALL
Java_com_example_cosmo388_MainActivity_startRecording(JNIEnv *env, jobject instance, jlong recorderHandle) {
    AudioRecorder *recorder = reinterpret_cast<AudioRecorder *>(recorderHandle);
    if (recorder) {
        recorder->callRecorderStream();
    }
}

JNIEXPORT void JNICALL
Java_com_example_cosmo388_MainActivity_writeAudio(JNIEnv *env, jobject instance, jstring date, jlong recorderHandle) {
    AudioRecorder *recorder = reinterpret_cast<AudioRecorder *>(recorderHandle);
    const char *DATE = env->GetStringUTFChars(date, nullptr);
    if (DATE != nullptr) {
        recorder->writeAudio(DATE);
        env->ReleaseStringUTFChars(date, DATE);
    }
}

JNIEXPORT void JNICALL
Java_com_example_cosmo388_MainActivity_startPlayback(JNIEnv *env, jobject instance, jlong recorderHandle) {
    AudioRecorder *recorder = reinterpret_cast<AudioRecorder *>(recorderHandle);
    if (recorder) {
        recorder->callPlayBackStream();
    }
}

JNIEXPORT void JNICALL
Java_com_example_cosmo388_MainActivity_stopRecording(JNIEnv *env, jobject instance, jlong recorderHandle) {
    AudioRecorder *recorder = reinterpret_cast<AudioRecorder *>(recorderHandle);
    if (recorder) {
        recorder->stopAllStream();
    }
}

JNIEXPORT void JNICALL
Java_com_example_cosmo388_MainActivity_getPath(JNIEnv *env, jobject instance, jstring storagePath, jlong recorderHandle) {
    AudioRecorder *recorder = reinterpret_cast<AudioRecorder *>(recorderHandle);
    const char *path = env->GetStringUTFChars(storagePath, nullptr);
    if (path != nullptr) {
        recorder->setSystemPath(path);
        env->ReleaseStringUTFChars(storagePath, path);
    }
}

JNIEXPORT void JNICALL
        Java_com_example_cosmo388_MainActivity_getIRPath(JNIEnv *env, jobject instance, jstring irPath, jlong recorderHandle) {
    AudioRecorder *recorder = reinterpret_cast<AudioRecorder *>(recorderHandle);
    const char *path = env->GetStringUTFChars(irPath, nullptr);
    if (path != nullptr) {
        recorder->getIRPath(path);
        env->ReleaseStringUTFChars(irPath, path);
    }
}

JNIEXPORT void JNICALL
Java_com_example_cosmo388_MainActivity_setVolume(JNIEnv *env, jobject instance, jlong recorder_handle,
                                                 jfloat volume, jint Channel){

    AudioRecorder *recorder = reinterpret_cast<AudioRecorder *>(recorder_handle);
    recorder->setVolume(volume, Channel);

}

}

