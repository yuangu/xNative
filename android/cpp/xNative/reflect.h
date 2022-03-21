#include <stdint.h>

#pragma once
#include <jni.h>
#include <string>
#include <stdint.h>

jobject callObjectJavaMethod(jclass jClass, const char* javaClassName, const char* methodName, const char* methodSig, ...);
int64_t callLongJavaMethod(jclass jClass, const char* javaClassName, const char* methodName, const char* methodSig, ...);

jclass  Object_getClass(jclass jClass);

jclass Class_forName(const char* className);
jobject Class_newInstance(jclass jClass);
jarray Class_getConstructors(jclass jClass);
jarray Class_getMethods(jclass jClass);
jarray Class_getFields(jclass jClass);
jobject Class_getTypeName(jclass jClass);

jobject Method_getName(jclass jClass);
jobject Method_getParameterTypes(jclass jClass);
jobject  Method_getReturnType(jclass jClass);

jobject  Field_getName(jclass jClass);
jobject  Field_getType(jclass jClass);


jobject Constructor_getParameterTypes(jclass jClass);
jobject Constructor_getName(jclass jClass);