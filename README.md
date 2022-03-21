### 说明
在cocos creator里使用上Native.js的项目
详情参考：https://www.html5plus.org/doc/zh_cn/android.html



### 一. 安装方法

1. 将android/cpp下的文件拷贝至frameworks/runtime-src/Classes下

2. 将android/java下的文件拷贝至frameworks/runtime-src/proj.android-studio/app/src下

3. 修改frameworks/runtime-src/proj.android-studio/jni/CocosAndroid.mk在LOCAL_SRC_FILES后面添加如文件

```
	../../Classes/xNative/proxy.cpp \
	../../Classes/xNative/reflect.cpp \
    ../../Classes/xNative/xNative.cpp  \
	../../Classes/xNative/cocos_export.cpp
```

4. 修改frameworks/runtime-src/Classes/jsb_module_register.cpp 

在jsb_register_all_modules函数上面加上下面代码
```
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
extern "C"
{
bool js_register_plus_andorid(se::Object* global) ;
}
#endif
```

在jsb_register_all_module函数里加上。
```
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
   se->addRegisterCallback(js_register_plus_andorid);
#endif
```

### 二、使用方法
