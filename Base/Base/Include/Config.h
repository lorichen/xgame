/**
* Created by pk 2008.01.02
*/

#ifndef RKT_CONFIG_H
#define RKT_CONFIG_H

/*
	该文件用于对整个引擎进行配置编译，下面是所有可以用于条件编译的宏定义
*/


/// 调试宏，定义此行则会加入调试信息（注：不同于编译器的_DEBUG宏）
//#define RKT_DEBUG

/// 是否打开性能测试宏
#define RKT_PROFILE

/// 是否支持UI测试
#define RKT_UI_TEST

/// 是否打开某些局部测试宏
#define RKT_TEST

/// 启用内存调试模式
#define MEMORY_DEBUG



#endif // RKT_CONFIG_H