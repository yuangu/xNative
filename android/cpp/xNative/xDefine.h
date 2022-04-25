#pragma once
#include <cstdint>

enum xNativeValueType
{
    NativeValueType_void = 0,
    NativeValueType_bool ,
    NativeValueType_uint8,
    NativeValueType_int8,
    NativeValueType_uint16,
    NativeValueType_int16,
    NativeValueType_int32,
    NativeValueType_int64,
    NativeValueType_float,
    NativeValueType_double,
    NativeValueType_object,
};

typedef struct xNativeValue
{
    xNativeValue()
    {
        isStaticClass = false;
    }

    xNativeValueType type;
    jvalue jValue;
    bool isStaticClass;  // 用于区分invoke是调用静态方法还是实例方法
} xNativeValue;
