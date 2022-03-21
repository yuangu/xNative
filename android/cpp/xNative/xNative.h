#pragma once
#include <jni.h>
#include <cstdint>
#include "xDefine.h"

void setJniEnv(JNIEnv *env);
JNIEnv *getJniEnv();

//创建实例对象
xNativeValue newObject(const char* className, int args, xNativeValue* argv);

// 获取对象（类对象/实例对象）的属性值
xNativeValue  getAttribute(xNativeValue* object,  const char* attributteName);

//设置属性值
void setAttribute(xNativeValue* object,  const char* attributteName, xNativeValue* value);

// 实现Interface的方法
xNativeValue implements(const char* name, void* obj );

//导入类
xNativeValue importClass(const char* className);

//调用方法
xNativeValue invoke(xNativeValue* object, const char* name,int args,  xNativeValue* argv);

