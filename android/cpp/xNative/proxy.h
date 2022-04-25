#pragma once
#include <jni.h>


jobject create_proxy_instance(JNIEnv *env, const char* className, void* ptr);