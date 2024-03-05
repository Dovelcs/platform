// Define u_int as an unsigned integer type
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#ifndef _COMMON_H
#define _COMMON_H

#ifndef u_int 
#define u_int unsigned int
#endif 

#ifndef u_char
#define u_char unsigned char
#endif 

#ifndef u_short
#define u_short unsigned short
#endif

#ifndef u_long
#define u_long unsigned long
#endif

#ifndef u_llong
#define u_llong unsigned long long
#endif

#ifndef u_quad_t
#define u_quad_t unsigned long long
#endif


// 求最小值
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

// 求最大值
#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

// 求绝对值
#ifndef OFFSETOF
#define OFFSETOF(type, member) ((size_t) &((type *)0)->member)
#endif

//判断一个数是否在某个范围内
#define IN_RANGE(x, min, max) ((x) >= (min) && (x) <= (max))

//设置一个数的某个位
#define BIT_SET(x, bit) ((x) |= (1 << (bit)))

//清除一个数的某个位
#define BIT_CLEAR(x, bit) ((x) &= ~(1 << (bit)))

//判断一个数的某个位是否为1
#define BIT_IS_SET(x, bit) ((x) & (1 << (bit)))

//判断一个数的某个位是否为0
#define BIT_IS_CLEAR(x, bit) (!((x) & (1 << (bit))))

// 求数组的长度
#ifndef array_size
#define array_size(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif 

#ifndef lprintf
//打印行号函数名，并且可以像printf一样打印
#define lprintf(fmt, ...) printf("%s:%d "fmt, __func__, __LINE__, ##__VA_ARGS__)
#endif


// #ifndef PLINE
// #define PLINE printf("%s:%d\n", __func__, __LINE__)
// #endif



#endif // _COMMON_H