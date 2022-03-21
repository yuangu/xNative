#include "proxy.h"
#include "reflect.h"

JNIEnv *getJniEnv();
std::string jstring2str(JNIEnv *env, jstring jstr);
void setJniEnv(JNIEnv *env);


// 到脚本上执行
void script_invoke0(void* ptr, std::string name);

extern "C" {
JNIEXPORT jobject
    JNICALL Java_com_seantone_xNative_NativeInvocationHandler_invoke0
            (JNIEnv *env, jobject j_this, jobject j_proxy, jobject j_method, jobjectArray args) {

        setJniEnv(env);

        // 函数名
        jobject name = Method_getName((jclass)j_method);
        std::string _nameStdStr = jstring2str(env, (jstring)name);

        if(args != nullptr)
        {
            int argv = env->GetArrayLength(args);
            for(int i = 0; i < argv; ++i)
            {
                jobject tmp = env->GetObjectArrayElement((jobjectArray)args, i);

                printf("hh");
            }
        }

        void* ptr = (void*)callLongJavaMethod((jclass)j_this, "com/seantone/xNative/NativeInvocationHandler",
                                              "getPythonObjectPointer","()J");

        script_invoke0( ptr, _nameStdStr );
        return nullptr;
    }
}



jobject create_proxy_instance(const char* className, void* p){
    // new NativeInvocationHandler()
    JNIEnv* env = getJniEnv();
    jclass j_class = env->FindClass("com/seantone/xNative/NativeInvocationHandler");
    jmethodID mid =env->GetMethodID(j_class,"<init>", "(J)V");
    jlong ptr = (jlong)(p);
    jobject object = env->NewObject(j_class, mid, ptr);

    //    //ClassLoader  classLoader = Thread.currentThread().getContextClassLoader()
    jclass j_Thread_class = env->FindClass("java/lang/Thread");
    jmethodID mcurrentThreadId =env->GetStaticMethodID(j_Thread_class,"currentThread", "()Ljava/lang/Thread;");
    jclass mThreadClass =(jclass)( env->CallStaticObjectMethod(j_Thread_class, mcurrentThreadId));
    jmethodID mgetContextClassLoaderId = env->GetMethodID(j_Thread_class, "getContextClassLoader", "()Ljava/lang/ClassLoader;");
    jobject classLoader = env->CallObjectMethod(mThreadClass, mgetContextClassLoaderId);

    // Class.forName("")
    jclass j_Class_class = env->FindClass("java/lang/Class");
    jmethodID mforNameid = env->GetStaticMethodID(j_Class_class,"forName","(Ljava/lang/String;)Ljava/lang/Class;");
    jobject jclassObj = env->CallStaticObjectMethod(j_Class_class,  mforNameid , env->NewStringUTF(className));

    // new Class[]
    jobjectArray  classArray = env->NewObjectArray(1, j_Class_class, NULL);
    env->SetObjectArrayElement(classArray, 0, jclassObj);

    // Proxy.newProxyInstance
    jclass j_Proxy_class = env->FindClass("java/lang/reflect/Proxy");
    jmethodID mnewProxyInstanceId =env->GetStaticMethodID(j_Proxy_class,"newProxyInstance",
                                                          "(Ljava/lang/ClassLoader;[Ljava/lang/Class;Ljava/lang/reflect/InvocationHandler;)Ljava/lang/Object;");

    jobject handler = env->CallStaticObjectMethod(j_Proxy_class, mnewProxyInstanceId, classLoader, classArray, (jclass)object  );
    return handler;
}