#include "cocos_export.h"
#include "cocos/scripting/js-bindings/jswrapper/SeApi.h"
#include "cocos/scripting/js-bindings/manual/jsb_conversions.hpp"
#include "platform/CCApplication.h"
#include "base/CCScheduler.h"
#include "xNative/xNative.h"
#include "xNative/reflect.h"
#include <vector>

std::string jstring2str(JNIEnv *env, jstring jstr);

class Jvalue : public cocos2d::Ref, public xNativeValue
{
public:
    void operator=(const xNativeValue &v)
    {
        type = v.type;
        jValue = v.jValue;
        isStaticClass = v.isStaticClass;

        if(v.type == NativeValueType_object)
        {
            JNIEnv* env =  cocos2d::JniHelper::getEnv();
            jValue.l = env->NewGlobalRef(jValue.l);
        }
    }

    virtual ~Jvalue()
    {
        if(type == NativeValueType_object)
        {
            JNIEnv* env = (env == nullptr?cocos2d::JniHelper::getEnv():env);
            env->DeleteGlobalRef(jValue.l);
        }
    }

};

xNativeValue toxNativeValue(const se::Value &nodeVal)
{
    xNativeValue ret;
    if (nodeVal.isNumber())
    {
        ret.jValue.d = nodeVal.toNumber();
        ret.type = NativeValueType_double;
    }

    else if (nodeVal.isBoolean())
    {
        ret.jValue.b = nodeVal.toBoolean();
        ret.type = NativeValueType_uint8;
    }
    else if (nodeVal.isString())
    {
        std::string str = nodeVal.toString();
        ret.jValue.l = cocos2d::JniHelper::getEnv()->NewStringUTF(str.c_str());
        ret.type = NativeValueType_object;
    }
    else if (nodeVal.isObject())
    {
        Jvalue *child = (Jvalue *)nodeVal.toObject()->getPrivateData();
        ret.jValue = child->jValue;
        ret.type = child->type;
    }

    return ret;
}

void toSeValue(const xNativeValue &val, se::Value *ret)
{
    if (val.type == NativeValueType_bool)
    {
        ret->setBoolean(val.jValue.z);
    }
    else if (val.type == NativeValueType_uint8)
    {
        ret->setUint8(val.jValue.b);
    }
    else if (val.type == NativeValueType_int8)
    {
        ret->setUint8(val.jValue.c);
    }
    else if (val.type == NativeValueType_int16)
    {
        ret->setInt16(val.jValue.s);
    }
    else if (val.type == NativeValueType_int32)
    {
        ret->setInt32(val.jValue.i);
    }
    else if (val.type == NativeValueType_int64)
    {
        ret->setInt32(val.jValue.j);
    }
    else if (val.type == NativeValueType_float)
    {
        ret->setFloat(val.jValue.f);
    }
    else if (val.type == NativeValueType_double)
    {
        ret->setNumber(val.jValue.d);
    }
    else if (val.type == NativeValueType_object)
    {
        if(val.jValue.l == nullptr)
        {
            ret->setNull();
            return;
        }

        JNIEnv *env = cocos2d::JniHelper::getEnv();

        jclass jClass = Object_getClass(env, (jclass)(val.jValue.l));

        jobject name2 = Class_getTypeName(env, jClass);
        std::string nameStdStr = jstring2str(env, (jstring) name2);
        if(nameStdStr == std::string("java.lang.String"))
        {
            ret->setString(jstring2str(env, (jstring) val.jValue.l));
        }else{
            Jvalue *v = new Jvalue;
            *v = val;
            native_ptr_to_seval<Jvalue>((Jvalue *)v, ret);
        }
    }
}

extern "C"
{
    static bool xNative_newObject(se::State &s)
    {
        const auto &args = s.args();
        int argc = (int)args.size();
        CC_UNUSED bool ok = true;
        if (argc > 0)
        {
            Jvalue *ret = new Jvalue;
            std::string className;
            seval_to_std_string(args[0], &className);

            std::vector<xNativeValue> values;
            for (int i = 1; i < argc; ++i)
            {
                values.push_back(toxNativeValue(args[i]));
            }

            JNIEnv *env = cocos2d::JniHelper::getEnv();

            *ret = newObject(env, className.c_str(), values.size(), values.size() == 0 ? nullptr : &(values[0]));

            ok &= native_ptr_to_seval<Jvalue>((Jvalue *)ret, &s.rval());
            return true;
        }

        SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);

        return false;
    }
    SE_BIND_FUNC(xNative_newObject);

    static bool xNative_importClass(se::State &s)
    {
        const auto &args = s.args();
        int argc = (int)args.size();
        CC_UNUSED bool ok = true;
        if (argc == 1)
        {
            Jvalue *ret = new Jvalue;
            std::string className;
            seval_to_std_string(args[0], &className);

            JNIEnv *env = cocos2d::JniHelper::getEnv();
            *ret = importClass(env, className.c_str());

            ok &= native_ptr_to_seval<Jvalue>((Jvalue *)ret, &s.rval());
            return true;
        }

        SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);

        return false;
    }
    SE_BIND_FUNC(xNative_importClass);

    static bool xNative_invoke(se::State &s)
    {
        const auto &args = s.args();
        int argc = (int)args.size();
        CC_UNUSED bool ok = true;
        if (argc > 1)
        {

            Jvalue *arg0 = nullptr;
            if (args[0].isObject())
            {
                ok &= seval_to_native_ptr(args[0], &arg0);
            }
            else
            {
                return false;
            }

            std::string methodName;
            seval_to_std_string(args[1], &methodName);

            std::vector<xNativeValue> values;
            for (int i = 2; i < argc; ++i)
            {
                values.push_back(toxNativeValue(args[i]));
            }

            JNIEnv *env = cocos2d::JniHelper::getEnv();
            xNativeValue val = invoke(env, (xNativeValue *)arg0, methodName.c_str(), values.size(), values.size() == 0 ? nullptr : &(values[0]));
            toSeValue(val, &s.rval());
            return true;
        }
        SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
        return false;
    }
    SE_BIND_FUNC(xNative_invoke);

    static bool xNative_implements(se::State &s)
    {
        const auto &args = s.args();
        int argc = (int)args.size();
        CC_UNUSED bool ok = true;
        if (argc == 2)
        {
            std::string intefaceName;
            seval_to_std_string(args[0], &intefaceName);

            // 内存泄露
            Jvalue *ret = new Jvalue;
            se::Value *ptr = new se::Value(args[1]);

            JNIEnv* env = cocos2d::JniHelper::getEnv();
            *ret = implements(env,intefaceName.c_str(), ptr);

            ok &= native_ptr_to_seval<Jvalue>((Jvalue *)ret, &s.rval());

            return true;
        }
        SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
        return false;
    }
    SE_BIND_FUNC(xNative_implements);

    static bool xNative_getAttribute(se::State &s)
    {
        const auto &args = s.args();
        int argc = (int)args.size();
        CC_UNUSED bool ok = true;
        if (argc == 2)
        {
            Jvalue *arg0 = nullptr;
            if (args[0].isObject())
            {
                ok &= seval_to_native_ptr(args[0], &arg0);
            }
            else
            {
                return false;
            }

            std::string attributeName;
            seval_to_std_string(args[1], &attributeName);

            JNIEnv* env = cocos2d::JniHelper::getEnv();
            xNativeValue value = getAttribute(env, (xNativeValue *)arg0, attributeName.c_str());

            toSeValue(value, &s.rval());

            return true;
        }
        return false;
    }
    SE_BIND_FUNC(xNative_getAttribute);

    static bool xNative_setAttribute(se::State &s)
    {
        const auto &args = s.args();
        int argc = (int)args.size();
        CC_UNUSED bool ok = true;
        if (argc == 3)
        {
            Jvalue *arg0 = nullptr;
            if (args[0].isObject())
            {
                ok &= seval_to_native_ptr(args[0], &arg0);
            }
            else
            {
                return false;
            }

            std::string attributeName;
            seval_to_std_string(args[1], &attributeName);

            xNativeValue value = toxNativeValue(args[2]);
            JNIEnv* env = cocos2d::JniHelper::getEnv();
            setAttribute(env, (xNativeValue *)arg0, attributeName.c_str(), &value);

            return true;
        }
        return false;
    }
    SE_BIND_FUNC(xNative_setAttribute);

    static bool xNative_runtimeMainActivity(se::State &s)
    {
        const auto &args = s.args();
        int argc = (int)args.size();

        xNativeValue ret;
        ret.type = NativeValueType_object;
        ret.isStaticClass = false;
        JNIEnv* env = cocos2d::JniHelper::getEnv();
        ret.jValue.l = callObjectStaticJavaMethod( env, "org/cocos2dx/lib/Cocos2dxActivity", "getContext", "()Landroid/content/Context;");
        toSeValue(ret, &s.rval());

        return true;
    }
    SE_BIND_FUNC(xNative_runtimeMainActivity);


    bool js_register_plus_andorid(se::Object *global)
    {
        se::HandleObject obj(se::Object::createPlainObject());
        se::Value objVal;
        objVal.setObject(obj);
        global->setProperty("plus", objVal);

        auto cls = se::Class::create("android", obj.get(), nullptr, nullptr);
        cls->defineStaticFunction("newObject", _SE(xNative_newObject));
        cls->defineStaticFunction("importClass", _SE(xNative_importClass));
        cls->defineStaticFunction("invoke", _SE(xNative_invoke));
        cls->defineStaticFunction("implements", _SE(xNative_implements));
        cls->defineStaticFunction("getAttribute", _SE(xNative_getAttribute));
        cls->defineStaticFunction("setAttribute", _SE(xNative_setAttribute));
        cls->defineStaticFunction("runtimeMainActivity", _SE(xNative_runtimeMainActivity));
        cls->install();

        auto cls2 = se::Class::create("Jvalue", obj.get(), nullptr, nullptr);
        cls2->install();
        JSBClassType::registerClass<Jvalue>(cls2);

        //        auto cls = se::Class::create("android", obj.get(), nullptr, nullptr);
        //        cls->install();

        return true;
    }
}

void script_invoke0(void *ptr, std::string name, std::vector<xNativeValue>& invoke_args)
{
    auto scheduler = cocos2d::Application::getInstance()->getScheduler();
    scheduler->performFunctionInCocosThread([=]()
                                            {
        se::ScriptEngine::getInstance()->clearException();
        se::AutoHandleScope hs;

        se::Object *object = ((se::Value *) ptr)->toObject();
        se::Value func;
        object->getProperty(name.c_str(), &func);
        if (func.isObject() && func.toObject()->isFunction()) {
            se::ValueArray args;

            for(auto it = invoke_args.begin(); it != invoke_args.end(); ++it)
            {
                se::Value v;
                toSeValue(*it, &v);
                args.push_back(v);

                if(v.isString())
                {
                    cocos2d::JniHelper::getEnv()->DeleteGlobalRef(it->jValue.l);
                }
            }

            func.toObject()->call(args, nullptr);
        } });
}
