#include "xNative.h"
#include "reflect.h"
#include <map>
#include <vector>
#include "proxy.h"

std::string
replace_all(std::string str, const std::string &old_value, const std::string &new_value) {
    while (true) {
        std::string::size_type pos(0);
        if ((pos = str.find(old_value)) != std::string::npos)
            str.replace(pos, old_value.length(), new_value);
        else
            break;
    }
    return str;
}

std::string jstring2str(JNIEnv *env, jstring jstr);

std::map <std::string, std::string> SignatureMap = {
        {"boolean", "Z"},
        {"byte",    "B"},
        {"char",    "C"},
        {"double",  "D"},
        {"float",   "F"},
        {"int",     "I"},
        {"long",    "J"},
        {"short",   "S"},
        {"void",    "V"},
};

xNativeValue newObject(JNIEnv *env, const char *className, int args, xNativeValue *argv) {
    jclass jClass = Class_forName(env, className);
    jobjectArray array = (jobjectArray) Class_getConstructors(env, jClass);
    xNativeValue ret;

    jsize len = env->GetArrayLength(array);
    for (int i = 0; i < len; ++i) {
        jobject tmp = env->GetObjectArrayElement((jobjectArray) array, i);
        jobject name = Constructor_getName(env, (jclass) tmp);
        std::string nameStdStr = jstring2str(env, (jstring) name);
        printf("%s", nameStdStr.c_str());

        jarray parameterTypes = (jarray) Constructor_getParameterTypes(env, (jclass) tmp);

        if (env->GetArrayLength(parameterTypes) != args) {
            continue;
        }

        std::vector <jvalue> vaules;
        std::string sig = "(";
        for (int j = 0; j < args; j++) {
            jobject tmp2 = env->GetObjectArrayElement((jobjectArray) parameterTypes, j);
            jobject name2 = Class_getTypeName(env, (jclass) tmp2);
            std::string nameStdStr = jstring2str(env, (jstring) name2);
            std::map<std::string, std::string>::iterator it = SignatureMap.find(nameStdStr);
            if (it != SignatureMap.end()) {
                sig += it->second;
            } else {
                std::string out = replace_all(nameStdStr, std::string("."), std::string("/"));
                sig += std::string("L") + out + std::string(";");
            }
            vaules.push_back((*(argv + j)).jValue);
        }
        sig += ")V";
        jmethodID init = env->GetMethodID(jClass, "<init>", sig.c_str());
        jobject object = env->NewObjectA(jClass, init, vaules.size() == 0 ? nullptr : &(vaules[0]));

        ret.jValue.l = object;
        ret.type = NativeValueType_object;
        ret.isStaticClass = false;
        return ret;
    }
    return ret;
}

xNativeValue importClass(JNIEnv *env, const char *className) {
    xNativeValue ret;
    jclass jClass = Class_forName(env, className);
    ret.isStaticClass = true;
    ret.jValue.l = jClass;
    ret.type = NativeValueType_object;

    return ret;
}

xNativeValue invoke(JNIEnv *env, xNativeValue *object, const char *name, int args, xNativeValue *argv) {
    xNativeValue ret;
    if (object->type != NativeValueType_object) {
        // log for here
        return ret;
    }

    bool isStaticClass = object->isStaticClass;
    jclass jClass = nullptr;
    if (isStaticClass) {
        jClass = (jclass)(object->jValue.l);
    } else {
        jClass = Object_getClass(env, (jclass)(object->jValue.l));
    }

    jobjectArray array = (jobjectArray) Class_getMethods(env,  jClass);
    jsize len = env->GetArrayLength(array);
    std::string nameStdStr = name;

    for (int i = 0; i < len; ++i) {
        jobject tmp = env->GetObjectArrayElement((jobjectArray) array, i);
        jobject name = Method_getName(env, (jclass) tmp);
        std::string _nameStdStr = jstring2str(env, (jstring) name);
        if (_nameStdStr != nameStdStr) {
            continue;
        }
        jarray parameterTypes = (jarray) Method_getParameterTypes(env,  (jclass) tmp);

        if (env->GetArrayLength(parameterTypes) != args) {
            continue;
        }

        std::vector <jvalue> vaules;
        std::string sig = "(";
        for (int j = 0; j < args; j++) {
            jobject tmp2 = env->GetObjectArrayElement((jobjectArray) parameterTypes, j);
            jobject name2 = Class_getTypeName(env,  (jclass) tmp2);
            std::string nameStdStr = jstring2str(env, (jstring) name2);
            std::map<std::string, std::string>::iterator it = SignatureMap.find(nameStdStr);
            if (it != SignatureMap.end()) {
                sig += it->second;
            } else {
                std::string out = replace_all(nameStdStr, std::string("."), std::string("/"));
                sig += std::string("L") + out + std::string(";");
            }
            vaules.push_back((*(argv + j)).jValue);
        }

        //返回类型
        jobject returnType = Method_getReturnType(env, (jclass) tmp);
        jobject name2 = Class_getTypeName(env, (jclass) returnType);
        if(name2 == nullptr) return ret;
        std::string returnName = jstring2str(env, (jstring) name2);
        std::map<std::string, std::string>::iterator it = SignatureMap.find(returnName);
        sig += ")";
        if (it != SignatureMap.end()) {
            sig += it->second;
        } else {

            std::string out = replace_all(returnName, std::string("."), std::string("/"));

            sig += std::string("L") + out + std::string(";");
        }

        jmethodID jmethodId = nullptr;
        if (isStaticClass) {
            jmethodId = env->GetStaticMethodID(jClass, nameStdStr.c_str(), sig.c_str());
        } else {
            jmethodId = env->GetMethodID(jClass, nameStdStr.c_str(), sig.c_str());
        }

        if (returnName == "boolean") {
            if (isStaticClass) {
                ret.jValue.z = env->CallStaticBooleanMethodA((jclass)(object->jValue.l), jmethodId,
                                                             vaules.size() == 0 ? nullptr
                                                                                : &(vaules[0]));
            } else {
                ret.jValue.z = env->CallBooleanMethodA((jclass)(object->jValue.l), jmethodId,
                                                       vaules.size() == 0 ? nullptr : &(vaules[0]));
            }
            ret.type = NativeValueType_bool;
        } else if (returnName == "byte") {
            if (isStaticClass) {
                ret.jValue.b = env->CallStaticByteMethodA((jclass)(object->jValue.l), jmethodId,
                                                          vaules.size() == 0 ? nullptr
                                                                             : &(vaules[0]));

            } else {
                ret.jValue.b = env->CallByteMethodA((jclass)(object->jValue.l), jmethodId,
                                                    vaules.size() == 0 ? nullptr : &(vaules[0]));
            }
            ret.type = NativeValueType_uint8;
        } else if (returnName == "char") {
            if (isStaticClass) {
                ret.jValue.c = env->CallStaticCharMethodA((jclass)(object->jValue.l), jmethodId,
                                                          vaules.size() == 0 ? nullptr
                                                                             : &(vaules[0]));

            } else {
                ret.jValue.c = env->CallCharMethodA((jclass)(object->jValue.l), jmethodId,
                                                    vaules.size() == 0 ? nullptr : &(vaules[0]));
            }

            ret.type = NativeValueType_int8;
        } else if (returnName == "long") {
            if (isStaticClass) {
                ret.jValue.j = env->CallStaticLongMethodA((jclass)(object->jValue.l), jmethodId,
                                                          vaules.size() == 0 ? nullptr
                                                                             : &(vaules[0]));
            } else {
                ret.jValue.j = env->CallLongMethodA((jclass)(object->jValue.l), jmethodId,
                                                    vaules.size() == 0 ? nullptr : &(vaules[0]));
            }

            ret.type = NativeValueType_int64;
        } else if (returnName == "float") {

            if (isStaticClass) {
                ret.jValue.f = env->CallStaticFloatMethodA((jclass)(object->jValue.l), jmethodId,
                                                           vaules.size() == 0 ? nullptr
                                                                              : &(vaules[0]));
            } else {
                ret.jValue.f = env->CallFloatMethodA((jclass)(object->jValue.l), jmethodId,
                                                     vaules.size() == 0 ? nullptr : &(vaules[0]));
            }

            ret.type = NativeValueType_float;
        } else if (returnName == "short") {
            if (isStaticClass) {
                ret.jValue.s = env->CallStaticShortMethodA((jclass)(object->jValue.l), jmethodId,
                                                           vaules.size() == 0 ? nullptr
                                                                              : &(vaules[0]));
            } else {
                ret.jValue.s = env->CallShortMethodA((jclass)(object->jValue.l), jmethodId,
                                                     vaules.size() == 0 ? nullptr : &(vaules[0]));
            }

            ret.type = NativeValueType_int16;
        } else if (returnName == "double") {
            if (isStaticClass) {
                ret.jValue.d = env->CallStaticDoubleMethodA((jclass)(object->jValue.l), jmethodId,
                                                            vaules.size() == 0 ? nullptr
                                                                               : &(vaules[0]));
            } else {
                ret.jValue.d = env->CallDoubleMethodA((jclass)(object->jValue.l), jmethodId,
                                                      vaules.size() == 0 ? nullptr : &(vaules[0]));
            }
            ret.type = NativeValueType_double;
        } else if (returnName == "void") {
            if (isStaticClass) {
                env->CallStaticVoidMethodA((jclass)(object->jValue.l), jmethodId,
                                           vaules.size() == 0 ? nullptr : &(vaules[0]));
            } else {
                env->CallVoidMethodA((jclass)(object->jValue.l), jmethodId,
                                     vaules.size() == 0 ? nullptr : &(vaules[0]));
            }
            ret.type = NativeValueType_void;
        } else if (returnName == "int") {
            if (object->isStaticClass) {
                ret.jValue.i = env->CallStaticIntMethodA((jclass)(object->jValue.l), jmethodId,
                                                         vaules.size() == 0 ? nullptr
                                                                            : &(vaules[0]));
            } else {
                ret.jValue.i = env->CallIntMethodA((jclass)(object->jValue.l), jmethodId,
                                                   vaules.size() == 0 ? nullptr : &(vaules[0]));
            }
            ret.type = NativeValueType_int32;
        } else {
            if (isStaticClass) {
                ret.jValue.l = env->CallStaticObjectMethodA((jclass)(object->jValue.l), jmethodId,
                                                            vaules.size() == 0 ? nullptr
                                                                               : &(vaules[0]));
            } else {
                ret.jValue.l = env->CallObjectMethodA((jclass)(object->jValue.l), jmethodId,
                                                      vaules.size() == 0 ? nullptr : &(vaules[0]));
            }
            ret.isStaticClass = false;
            ret.type = NativeValueType_object;
        }

        return ret;
    }
    return ret;
}

xNativeValue implements(JNIEnv *env, const char *name, void *obj) {
    xNativeValue ret;
    ret.jValue.l = create_proxy_instance(env,name, obj);
    ret.type = NativeValueType_object;
    return ret;
}

xNativeValue getAttribute(JNIEnv *env, xNativeValue *object, const char *attributteName) {

    xNativeValue ret;
    jclass jClass = Object_getClass(env,(jclass)(object->jValue.l));
    jobjectArray array = (jobjectArray) Class_getFields(env,jClass);
    jsize len = env->GetArrayLength(array);
    std::string nameStdStr = attributteName;
    for (int i = 0; i < len; ++i) {
        jobject tmp = env->GetObjectArrayElement((jobjectArray) array, i);
        jobject name = Field_getName(env,(jclass) tmp);
        std::string _nameStdStr = jstring2str(env, (jstring) name);
        if (_nameStdStr != nameStdStr) {
            continue;
        }
        jobject name2 = Field_getType(env,(jclass) tmp);
        std::string nameStdStr = jstring2str(env, (jstring) name2);

        std::map<std::string, std::string>::iterator it = SignatureMap.find(nameStdStr);
        std::string sig = "";
        if (it != SignatureMap.end()) {
            sig += it->second;
        } else {
            std::string out = replace_all(nameStdStr, std::string("."), std::string("/"));
            sig += std::string("L") + out + std::string(";");
        }

        jfieldID fieldId = env->GetFieldID(jClass, attributteName, sig.c_str());

        if (nameStdStr == "boolean") {
            ret.jValue.z = env->GetBooleanField((jclass)(object->jValue.l), fieldId);
            ret.type = NativeValueType_bool;
        } else if (nameStdStr == "byte") {
            ret.jValue.b = env->GetByteField((jclass)(object->jValue.l), fieldId);
            ret.type = NativeValueType_uint8;
        } else if (nameStdStr == "char") {
            ret.jValue.c = env->GetCharField((jclass)(object->jValue.l), fieldId);
            ret.type = NativeValueType_int8;
        } else if (nameStdStr == "long") {
            ret.jValue.j = env->GetLongField((jclass)(object->jValue.l), fieldId);
            ret.type = NativeValueType_int64;
        } else if (nameStdStr == "float") {
            ret.jValue.f = env->GetFloatField((jclass)(object->jValue.l), fieldId);
            ret.type = NativeValueType_float;
        } else if (nameStdStr == "short") {
            ret.jValue.s = env->GetShortField((jclass)(object->jValue.l), fieldId);
            ret.type = NativeValueType_int16;
        } else if (nameStdStr == "double") {
            ret.jValue.d = env->GetDoubleField((jclass)(object->jValue.l), fieldId);
            ret.type = NativeValueType_double;
        } else if (nameStdStr == "int") {
            ret.jValue.i = env->GetIntField((jclass)(object->jValue.l), fieldId);
            ret.type = NativeValueType_int32;
        } else {
            ret.jValue.l = env->GetObjectField((jclass)(object->jValue.l), fieldId);
            ret.type = NativeValueType_object;
        }

        break;
    }
    return ret;
}

void setAttribute(JNIEnv *env, xNativeValue *object, const char *attributteName, xNativeValue *value) {

    jclass jClass = Object_getClass(env,(jclass)(object->jValue.l));
    jobjectArray array = (jobjectArray) Class_getFields(env,jClass);
    jsize len = env->GetArrayLength(array);
    std::string nameStdStr = attributteName;
    for (int i = 0; i < len; ++i) {
        jobject tmp = env->GetObjectArrayElement((jobjectArray) array, i);
        jobject name = Field_getName(env,(jclass) tmp);
        std::string _nameStdStr = jstring2str(env, (jstring) name);
        if (_nameStdStr != nameStdStr) {
            continue;
        }
        jobject name2 = Field_getType(env, (jclass) tmp);
        std::string nameStdStr = jstring2str(env, (jstring) name2);

        std::map<std::string, std::string>::iterator it = SignatureMap.find(nameStdStr);
        std::string sig = "";
        if (it != SignatureMap.end()) {
            sig += it->second;
        } else {
            std::string out = replace_all(nameStdStr, std::string("."), std::string("/"));
            sig += std::string("L") + out + std::string(";");
        }

        jfieldID fieldId = env->GetFieldID(jClass, attributteName, sig.c_str());

        if (nameStdStr == "boolean") {
            env->SetBooleanField((jclass)(object->jValue.l), fieldId, value->jValue.z);
        } else if (nameStdStr == "byte") {
            env->SetByteField((jclass)(object->jValue.l), fieldId, value->jValue.b);
        } else if (nameStdStr == "char") {
            env->SetCharField((jclass)(object->jValue.l), fieldId, value->jValue.c);
        } else if (nameStdStr == "long") {
            env->SetLongField((jclass)(object->jValue.l), fieldId, value->jValue.j);
        } else if (nameStdStr == "float") {
            env->SetFloatField((jclass)(object->jValue.l), fieldId, value->jValue.f);
        } else if (nameStdStr == "short") {
            env->SetShortField((jclass)(object->jValue.l), fieldId, value->jValue.s);
        } else if (nameStdStr == "double") {
            env->SetDoubleField((jclass)(object->jValue.l), fieldId, value->jValue.d);
        } else if (nameStdStr == "int") {
            env->SetIntField((jclass)(object->jValue.l), fieldId, value->jValue.i);
        } else {
            env->SetObjectField((jclass)(object->jValue.l), fieldId, value->jValue.l);
        }

        break;
    }
}