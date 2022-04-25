#include "reflect.h"

//=======================================
JNIEnv *getJniEnv();


std::string jstring2str(JNIEnv *env, jstring jstr) {
    char *rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("GB2312");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte *ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0) {
        rtn = (char *) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    std::string stemp(rtn);
    free(rtn);
    return stemp;
}

//============================================================================================
jobject
callObjectStaticJavaMethod(JNIEnv *env, const char *javaClassName, const char *methodName,
                           const char *methodSig,
                           ...) {
    jclass j_class = env->FindClass(javaClassName);
    if(j_class == nullptr)  return nullptr;
    jmethodID mid = env->GetStaticMethodID(j_class, methodName, methodSig);
    if(mid == nullptr) return nullptr;

    va_list args;
    va_start(args, methodSig);
    jobject ret = env->CallStaticObjectMethod(j_class, mid, args);

    va_end(args);
    return ret;
}

jobject
callObjectJavaMethod(JNIEnv *env, jclass jClass, const char *javaClassName, const char *methodName,
                     const char *methodSig, ...) {
    jclass j_class = env->FindClass(javaClassName);
    if(j_class == nullptr)  return nullptr;
    jmethodID mid = env->GetMethodID(j_class, methodName, methodSig);
    if(mid == nullptr) return nullptr;
    va_list args;

    va_start(args, methodSig);
    jobject ret = env->CallObjectMethod(jClass, mid, args);
    va_end(args);
    return ret;
}

jint
callIntJavaMethod(JNIEnv *env, jclass jClass, const char *javaClassName, const char *methodName,
                  const char *methodSig, ...) {

    jclass j_class = env->FindClass(javaClassName);
    if(j_class == nullptr)  return 0;
    jmethodID mid = env->GetMethodID(j_class, methodName, methodSig);
    if(mid == nullptr)  return 0;

    va_list args;
    va_start(args, methodSig);
    jint ret = env->CallIntMethod(jClass, mid, args);
    va_end(args);
    return ret;
}

jfloat
callFloatJavaMethod(JNIEnv *env, jclass jClass, const char *javaClassName, const char *methodName,
                    const char *methodSig, ...) {
    jclass j_class = env->FindClass(javaClassName);
    if(j_class == nullptr)  return 0;
    jmethodID mid = env->GetMethodID(j_class, methodName, methodSig);
    if(mid == nullptr)  return 0;

    va_list args;
    va_start(args, methodSig);
    jfloat ret = env->CallFloatMethod(jClass, mid, args);

    va_end(args);
    return ret;
}

jdouble
callDoubleJavaMethod(JNIEnv *env, jclass jClass, const char *javaClassName, const char *methodName,
                     const char *methodSig, ...) {
    jclass j_class = env->FindClass(javaClassName);
    if(j_class == nullptr)  return 0;
    jmethodID mid = env->GetMethodID(j_class, methodName, methodSig);
    if(mid == nullptr)  return 0;

    va_list args;
    va_start(args, methodSig);
    jdouble ret = env->CallDoubleMethod(jClass, mid, args);

    va_end(args);
    return ret;
}

jshort
callShortJavaMethod(JNIEnv *env, jclass jClass, const char *javaClassName, const char *methodName,
                    const char *methodSig, ...) {
    jclass j_class = env->FindClass(javaClassName);
    if(j_class == nullptr)  return 0;
    jmethodID mid = env->GetMethodID(j_class, methodName, methodSig);
    if(mid == nullptr)  return 0;

    va_list args;
    va_start(args, methodSig);
    jdouble ret = env->CallShortMethod(jClass, mid, args);

    va_end(args);
    return ret;
}

jbyte
callByteJavaMethod(JNIEnv *env, jclass jClass, const char *javaClassName, const char *methodName,
                   const char *methodSig, ...) {
    jclass j_class = env->FindClass(javaClassName);
    if(j_class == nullptr)  return 0;
    jmethodID mid = env->GetMethodID(j_class, methodName, methodSig);
    if(mid == nullptr)  return 0;

    va_list args;
    va_start(args, methodSig);
    jbyte ret = env->CallByteMethod(jClass, mid, args);

    va_end(args);
    return ret;
}

jboolean
callBooleanJavaMethod(JNIEnv *env, jclass jClass, const char *javaClassName, const char *methodName,
                      const char *methodSig, ...) {
    jclass j_class = env->FindClass(javaClassName);
    if(j_class == nullptr)  return 0;
    jmethodID mid = env->GetMethodID(j_class, methodName, methodSig);
    if(mid == nullptr)  return 0;

    va_list args;
    va_start(args, methodSig);
    jint ret = env->CallBooleanMethod(jClass, mid, args);

    va_end(args);
    return ret;
}

int64_t
callLongJavaMethod(JNIEnv *env, jclass jClass, const char *javaClassName, const char *methodName,
                   const char *methodSig, ...) {
    jclass j_class = env->FindClass(javaClassName);
    if(j_class == nullptr)  return 0;
    jmethodID mid = env->GetMethodID(j_class, methodName, methodSig);
    if(mid == nullptr)  return 0;

    va_list args;
    va_start(args, methodSig);

    int64_t ret = env->CallLongMethod(jClass, mid, args);

    va_end(args);
    return ret;
}

//============================================================================================
jclass Object_getClass(JNIEnv *env, jclass jClass) {
    return (jclass) callObjectJavaMethod(env, jClass, "java/lang/Object", "getClass",
                                         "()Ljava/lang/Class;");
}

//============================================================================================
jclass Class_forName(JNIEnv *env, const char *className) {

    jclass j_Class_class = env->FindClass("java/lang/Class");
    if( j_Class_class == nullptr)  return nullptr;
    jmethodID mforNameid = env->GetStaticMethodID(j_Class_class, "forName",
                                                  "(Ljava/lang/String;)Ljava/lang/Class;");
    if( mforNameid  == nullptr)  return nullptr;
    jobject jclassObj = env->CallStaticObjectMethod(j_Class_class, mforNameid,
                                                    env->NewStringUTF(className));
    return (jclass) jclassObj;
}

jobject Class_newInstance(JNIEnv *env, jclass jClass) {
    return (jobject) callObjectJavaMethod(env, jClass, "java/lang/Class",
                                          "newInstance", "()Ljava/lang/Object;");
}

jarray Class_getConstructors(JNIEnv *env, jclass jClass) {
    return (jarray) callObjectJavaMethod(env, jClass, "java/lang/Class",
                                         "getConstructors", "()[Ljava/lang/reflect/Constructor;");
}

jarray Class_getMethods(JNIEnv *env, jclass jClass) {
    return (jarray) callObjectJavaMethod(env, jClass, "java/lang/Class",
                                         "getMethods",
                                         "()[Ljava/lang/reflect/Method;");
}

jarray Class_getFields(JNIEnv *env, jclass jClass) {
    return (jarray) callObjectJavaMethod(env, jClass, "java/lang/Class",
                                         "getFields",
                                         "()[Ljava/lang/reflect/Field;");
}


jobject Class_getTypeName(JNIEnv *env, jclass jClass) {
    return (jobject) callObjectJavaMethod(env, jClass, "java/lang/Class",
                                          "getTypeName",
                                          "()Ljava/lang/String;");
}

//============================================================================================
jobject Method_getName(JNIEnv *env, jclass jClass) {
    return callObjectJavaMethod(env, jClass, "java/lang/reflect/Method", "getName",
                                "()Ljava/lang/String;");
}

jobject Method_getParameterTypes(JNIEnv *env, jclass jClass) {
    return callObjectJavaMethod(env, jClass, "java/lang/reflect/Method", "getParameterTypes",
                                "()[Ljava/lang/Class;");
}

jobject Method_getReturnType(JNIEnv *env, jclass jClass) {
    return callObjectJavaMethod(env, jClass, "java/lang/reflect/Method", "getReturnType",
                                "()Ljava/lang/Class;");
}

//===========================================================================================================
jobject Field_getName(JNIEnv *env, jclass jClass) {
    return callObjectJavaMethod(env, jClass, "java/lang/reflect/Field", "getName",
                                "()Ljava/lang/String;");
}

jobject Field_getType(JNIEnv *env, jclass jClass) {
    return callObjectJavaMethod(env, jClass, "java/lang/reflect/Field", "getType",
                                "()Ljava/lang/Class;");
}

//============================================================================================
jobject Constructor_getParameterTypes(JNIEnv *env, jclass jClass) {
    return callObjectJavaMethod(env, jClass, "java/lang/reflect/Constructor", "getParameterTypes",
                                "()[Ljava/lang/Class;");
}

jobject Constructor_getName(JNIEnv *env, jclass jClass) {
    return callObjectJavaMethod(env, jClass, "java/lang/reflect/Constructor", "getName",
                                "()Ljava/lang/String;");
}

//============================================================================================
jbyte Number_byteValue(JNIEnv *env, jclass jClass) {
    return callByteJavaMethod(env, jClass, "java/lang/Number", "byteValue", "()B");
}

jint Number_intValue(JNIEnv *env, jclass jClass) {
    return callIntJavaMethod(env, jClass, "java/lang/Number", "intValue", "()I");
}

jlong Number_longValue(JNIEnv *env, jclass jClass) {
    return callLongJavaMethod(env, jClass, "java/lang/Number", "longValue", "()J");
}

jfloat Number_floatValue(JNIEnv *env, jclass jClass) {
    return callFloatJavaMethod(env, jClass, "java/lang/Number", "floatValue", "()F");
}

jdouble Number_doubleValue(JNIEnv *env, jclass jClass) {
    return callDoubleJavaMethod(env, jClass, "java/lang/Number", "doubleValue", "()D");
}

jshort Number_shortValue(JNIEnv *env, jclass jClass) {
    return callShortJavaMethod(env, jClass, "java/lang/Number", "shortValue", "()S");
}

jboolean Boolean_booleanValue(JNIEnv *env, jclass jClass) {
    return callBooleanJavaMethod(env, jClass, "java/lang/Boolean", "booleanValue", "()Z");
}