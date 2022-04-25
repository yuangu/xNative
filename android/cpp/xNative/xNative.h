#pragma once
#include <jni.h>
#include <cstdint>
#include "xDefine.h"

//创建实例对象
xNativeValue newObject(JNIEnv *env,const char* className, int args, xNativeValue* argv);

// 获取对象（类对象/实例对象）的属性值
xNativeValue  getAttribute(JNIEnv *env,xNativeValue* object,  const char* attributteName);

//设置属性值
void setAttribute(JNIEnv *env,xNativeValue* object,  const char* attributteName, xNativeValue* value);

// 实现Interface的方法
xNativeValue implements(JNIEnv *env,const char* name, void* obj );

//导入类
xNativeValue importClass(JNIEnv *env,const char* className);

//调用方法
xNativeValue invoke(JNIEnv *env,xNativeValue* object, const char* name,int args,  xNativeValue* argv);

