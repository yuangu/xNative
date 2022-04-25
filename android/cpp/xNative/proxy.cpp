#include "proxy.h"
#include "reflect.h"
#include "xNative.h"
#include "vector"

JNIEnv *getJniEnv();

std::string jstring2str(JNIEnv *env, jstring jstr);

void setJniEnv(JNIEnv *env);


// 到脚本上执行
void script_invoke0(void *ptr, std::string name, std::vector<xNativeValue> &invoke_args);

extern "C" {
JNIEXPORT jobject
JNICALL Java_com_seantone_xNative_NativeInvocationHandler_invoke0
        (JNIEnv *env, jobject j_this, jobject j_proxy, jobject j_method, jobjectArray args) {

    // 函数名
    jobject name = Method_getName(env, (jclass) j_method);
    if(name == nullptr) return nullptr;

    std::string _nameStdStr = jstring2str(env, (jstring) name);

    void *ptr = (void *) callLongJavaMethod(env, (jclass) j_this,
                                            "com/seantone/xNative/NativeInvocationHandler",
                                            "getPythonObjectPointer", "()J");

    std::vector<xNativeValue> invoke_args;
    jsize len = args == nullptr ? 0 : env->GetArrayLength(args);
    for (int i = 0; i < len; ++i) {
        jobject tmp = env->GetObjectArrayElement((jobjectArray) args, i);
        jclass tmp2 = Object_getClass(env, (jclass) tmp);
        jobject name = Class_getTypeName(env, (jclass) tmp2);

        std::string nameStdStr = jstring2str(env, (jstring) name);

        xNativeValue ret;
        if (nameStdStr == "java.lang.Boolean") {
            ret.jValue.z = Boolean_booleanValue(env, (jclass) tmp);
            ret.type = NativeValueType_bool;
            ret.isStaticClass = false;
        } else if (nameStdStr == "java.lang.Byte") {
            ret.jValue.b = Number_byteValue(env, (jclass) tmp);
            ret.type = NativeValueType_uint8;
            ret.isStaticClass = false;
//            } else if (nameStdStr == "char") {
//                ret.jValue.c = Number_charValue((jclass)tmp);
//                ret.type = NativeValueType_int8;
        } else if (nameStdStr == "java.lang.Long") {
            ret.jValue.j = Number_longValue(env, (jclass) tmp);
            ret.type = NativeValueType_int64;
            ret.isStaticClass = false;
        } else if (nameStdStr == "java.lang.Float") {
            ret.jValue.f = Number_floatValue(env, (jclass) tmp);
            ret.type = NativeValueType_float;
            ret.isStaticClass = false;
        } else if (nameStdStr == "java.lang.Short") {
            ret.jValue.s = Number_shortValue(env, (jclass) tmp);
            ret.type = NativeValueType_int16;
            ret.isStaticClass = false;
        } else if (nameStdStr == "java.lang.Double") {
            ret.jValue.d = Number_doubleValue(env, (jclass) tmp);
            ret.type = NativeValueType_double;
            ret.isStaticClass = false;
        } else if (nameStdStr == "java.lang.Integer") {
            ret.jValue.i = Number_intValue(env, (jclass) tmp);
            ret.type = NativeValueType_int32;
            ret.isStaticClass = false;
        } else {
            ret.jValue.l = env->NewGlobalRef(tmp); //tmp;
            ret.type = NativeValueType_object;
            ret.isStaticClass = false;
        }
        invoke_args.push_back(ret);
    }

    script_invoke0(ptr, _nameStdStr, invoke_args);
    return nullptr;
}
}

jobject create_proxy_instance(JNIEnv *env, const char *className, void *p) {
    jclass j_class = env->FindClass("com/seantone/xNative/NativeInvocationHandler");
    jmethodID mid = env->GetMethodID(j_class, "<init>", "(J)V");
    jlong ptr = (jlong) (p);
    jobject object = env->NewObject(j_class, mid, ptr);

    //    //ClassLoader  classLoader = Thread.currentThread().getContextClassLoader()
    jclass j_Thread_class = env->FindClass("java/lang/Thread");
    jmethodID mcurrentThreadId = env->GetStaticMethodID(j_Thread_class, "currentThread",
                                                        "()Ljava/lang/Thread;");
    jclass mThreadClass = (jclass) (env->CallStaticObjectMethod(j_Thread_class, mcurrentThreadId));
    jmethodID mgetContextClassLoaderId = env->GetMethodID(j_Thread_class, "getContextClassLoader",
                                                          "()Ljava/lang/ClassLoader;");
    jobject classLoader = env->CallObjectMethod(mThreadClass, mgetContextClassLoaderId);

    // Class.forName("")
    jclass j_Class_class = env->FindClass("java/lang/Class");
    jmethodID mforNameid = env->GetStaticMethodID(j_Class_class, "forName",
                                                  "(Ljava/lang/String;)Ljava/lang/Class;");
    jobject jclassObj = env->CallStaticObjectMethod(j_Class_class, mforNameid,
                                                    env->NewStringUTF(className));

    // new Class[]
    jobjectArray classArray = env->NewObjectArray(1, j_Class_class, NULL);
    env->SetObjectArrayElement(classArray, 0, jclassObj);

    // Proxy.newProxyInstance
    jclass j_Proxy_class = env->FindClass("java/lang/reflect/Proxy");
    jmethodID mnewProxyInstanceId = env->GetStaticMethodID(j_Proxy_class, "newProxyInstance",
                                                           "(Ljava/lang/ClassLoader;[Ljava/lang/Class;Ljava/lang/reflect/InvocationHandler;)Ljava/lang/Object;");

    jobject handler = env->CallStaticObjectMethod(j_Proxy_class, mnewProxyInstanceId, classLoader,
                                                  classArray, (jclass) object);
    return handler;
}