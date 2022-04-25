#include <stdint.h>

#pragma once
#include <jni.h>
#include <string>
#include <stdint.h>

jobject callObjectStaticJavaMethod( JNIEnv *env , const char* javaClassName, const char* methodName, const char* methodSig, ...);
jobject callObjectJavaMethod(JNIEnv *env ,jclass jClass, const char* javaClassName, const char* methodName, const char* methodSig, ...);
int64_t callLongJavaMethod(JNIEnv *env ,jclass jClass, const char* javaClassName, const char* methodName, const char* methodSig, ...);

jclass  Object_getClass(JNIEnv *env ,jclass jClass);

jclass Class_forName(JNIEnv *env ,const char* className);
jobject Class_newInstance(JNIEnv *env ,jclass jClass);
jarray Class_getConstructors(JNIEnv *env ,jclass jClass);
jarray Class_getMethods(JNIEnv *env ,jclass jClass);
jarray Class_getFields(JNIEnv *env ,jclass jClass);
jobject Class_getTypeName(JNIEnv *env ,jclass jClass);

jobject Method_getName(JNIEnv *env ,jclass jClass);
jobject Method_getParameterTypes(JNIEnv *env ,jclass jClass);
jobject  Method_getReturnType(JNIEnv *env ,jclass jClass);

jobject  Field_getName(JNIEnv *env ,jclass jClass);
jobject  Field_getType(JNIEnv *env ,jclass jClass);

jbyte     Number_byteValue(JNIEnv *env ,jclass jClass);
jint      Number_intValue(JNIEnv *env ,jclass jClass);
jlong     Number_longValue(JNIEnv *env ,jclass jClass);
jfloat    Number_floatValue(JNIEnv *env ,jclass jClass);
jdouble   Number_doubleValue(JNIEnv *env ,jclass jClass);
jshort   Number_shortValue(JNIEnv *env ,jclass jClass);

jboolean Boolean_booleanValue(JNIEnv *env ,jclass jClass);

jobject Constructor_getParameterTypes(JNIEnv *env ,jclass jClass);
jobject Constructor_getName(JNIEnv *env ,jclass jClass);