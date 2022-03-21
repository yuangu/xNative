#include "reflect.h"
//=======================================
JNIEnv *getJniEnv();


std::string jstring2str(JNIEnv* env, jstring jstr)
{
    char*   rtn   =   NULL;
    jclass   clsstring   =   env->FindClass("java/lang/String");
    jstring   strencode   =   env->NewStringUTF("GB2312");
    jmethodID   mid   =   env->GetMethodID(clsstring,   "getBytes",   "(Ljava/lang/String;)[B");
    jbyteArray   barr=   (jbyteArray)env->CallObjectMethod(jstr,mid,strencode);
    jsize   alen   =   env->GetArrayLength(barr);
    jbyte*   ba   =   env->GetByteArrayElements(barr,JNI_FALSE);
    if(alen   >   0)
    {
        rtn   =   (char*)malloc(alen+1);
        memcpy(rtn,ba,alen);
        rtn[alen]=0;
    }
    env->ReleaseByteArrayElements(barr,ba,0);
    std::string stemp(rtn);
    free(rtn);
    return   stemp;
}

//============================================================================================
jobject callObjectJavaMethod(jclass jClass, const char* javaClassName, const char* methodName, const char* methodSig, ...)
{
    va_list args;
    va_start(args, methodSig);

    JNIEnv* env = getJniEnv();
    jclass j_class = env->FindClass(javaClassName);
    jmethodID mid = env->GetMethodID(j_class,methodName,methodSig);
    jobject ret = env->CallObjectMethod(jClass, mid, args);

    va_end(args);
    return ret;
}


int64_t  callLongJavaMethod(jclass jClass, const char* javaClassName, const char* methodName, const char* methodSig, ...)
{
    va_list args;
    va_start(args, methodSig);

    JNIEnv* env = getJniEnv();
    jclass j_class = env->FindClass(javaClassName);
    jmethodID mid = env->GetMethodID(j_class,methodName,methodSig);
    int64_t ret = env->CallLongMethod(jClass, mid, args);

    va_end(args);
    return ret;
}
//============================================================================================
jclass  Object_getClass(jclass jClass)
{
    return  (jclass )callObjectJavaMethod(jClass, "java/lang/Object", "getClass", "()Ljava/lang/Class;");
}
//============================================================================================
jclass Class_forName(const char* className)
{
    JNIEnv* env = getJniEnv();
    jclass j_Class_class = env->FindClass("java/lang/Class");
    jmethodID mforNameid = env->GetStaticMethodID(j_Class_class,"forName","(Ljava/lang/String;)Ljava/lang/Class;");
    jobject jclassObj = env->CallStaticObjectMethod(j_Class_class,  mforNameid , env->NewStringUTF(className));
    return (jclass)jclassObj;
}

jobject Class_newInstance(jclass jClass)
{
    return  (jobject)callObjectJavaMethod(jClass, "java/lang/Class",
                                          "newInstance","()Ljava/lang/Object;");
}

jarray Class_getConstructors(jclass jClass)
{
    return  (jarray)callObjectJavaMethod(jClass, "java/lang/Class",
                                         "getConstructors","()[Ljava/lang/reflect/Constructor;");
}

jarray Class_getMethods(jclass jClass)
{
    return  (jarray)callObjectJavaMethod(jClass, "java/lang/Class",
                                         "getMethods",
                                         "()[Ljava/lang/reflect/Method;");
}

jarray Class_getFields(jclass jClass)
{
     return  (jarray)callObjectJavaMethod(jClass, "java/lang/Class",
                                         "getFields",
                                         "()[Ljava/lang/reflect/Field;");
}


jobject  Class_getTypeName(jclass jClass)
{
    return  (jobject)callObjectJavaMethod(jClass, "java/lang/Class",
                                          "getTypeName",
                                          "()Ljava/lang/String;");
}
//============================================================================================
jobject Method_getName(jclass jClass)
{
    return  callObjectJavaMethod(jClass, "java/lang/reflect/Method", "getName", "()Ljava/lang/String;");
}

jobject Method_getParameterTypes(jclass jClass)
{
    return   callObjectJavaMethod(jClass, "java/lang/reflect/Method","getParameterTypes", "()[Ljava/lang/Class;");
}
jobject  Method_getReturnType(jclass jClass)
{
    return  callObjectJavaMethod(jClass, "java/lang/reflect/Method", "getReturnType", "()Ljava/lang/Class;");
}
//===========================================================================================================
jobject  Field_getName(jclass jClass)
{
     return  callObjectJavaMethod(jClass, "java/lang/reflect/Field", "getName", "()Ljava/lang/String;");
}

jobject  Field_getType(jclass jClass)
{
    return  callObjectJavaMethod(jClass, "java/lang/reflect/Field", "getType", "()Ljava/lang/Class;");
}
//============================================================================================
jobject Constructor_getParameterTypes(jclass jClass)
{
    return   callObjectJavaMethod(jClass, "java/lang/reflect/Constructor","getParameterTypes", "()[Ljava/lang/Class;");
}

jobject Constructor_getName(jclass jClass)
{
    return   callObjectJavaMethod(jClass, "java/lang/reflect/Constructor","getName", "()Ljava/lang/String;");
}
