//file name main.c
//LRU 测试缓存用例
#include <stdio.h>
#include <stdlib.h>
#include "LRUCache.h"

//错误处理宏
#define HANDLE_ERROR(msg) \
    do{fprintf(stderr, "%s fail. \n", msg); exit(-1);}while(0)

//封装缓存数据存储接口, 此处我们让data同时充当key的角色
#define LRUCACHE_PUTDATA(cache, data) \
do {\
    if ( 0 != LRUCacheSet(cache, data, data)) \
        fprintf(stderr, "put (%c, %c) to cache fail. \n", data, data); \
    else \
        fprintf(stdout, "put (%c, %c) to cache success. \n", data, data); \
    } \
    \
}while(0)

//封装缓存数据获取接口
#define LRUCACHE_GETDATA(cache, key) \
do{ \
    char data = LRUCacheGet(cache, key); \
    if ( '\0' == data ) \
        fprintf(stderr, "get data (Key:%c) from cache fail. \n", key); \
    else if ( key == data ) \
        fprintf(stdpout, "got (%c, %c) from cache success. \n", key, data); \
}while(0)
