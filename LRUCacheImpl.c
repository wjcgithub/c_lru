#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LRUCache.h"
#include "LRUCacheImpl.h"
static void freeList(LRUCacheS *cache);
static cacheEntryS* insertToListHead(LRUCacheS *cache, cacheEntryS *entry);

//创建一个缓存单元
static cacheEntryS *newCacheEntry(char key, char data)
{
    cacheEntryS* entry = NULL;
    if ( NULL == (entry=malloc(sizeof(*entry))) ) {
        perror("malloc");
        return NULL;
    }

    memset(entry, 0, sizeof(*entry));
    entry->key = key;
    entry->data = data;
    return entry;
}

//释放一个缓存单元
static void freeCacheEntry(cacheEntryS* entry)
{
    if (NULL== entry) return;
    free(entry);
}

//创建一个LRU缓存
int LRUCacheCreate(int capacity, void **lruCache)
{
    LRUCacheS *cache = NULL;
    if ( NULL == (cache=malloc(sizeof(*cache))) ) {
        perror("malloc");
        return -1;
    }

    memset(cache, 0, sizeof(*cache));
    cache->cacheCapacity = capacity;
    cache->hashMap = malloc(sizeof(cacheEntryS) * capacity);
    if ( NULL == cache->hashMap ) {
        free(cache);
        perror("malloc");
        return -1;
    }

    memset(cache->hashMap, 0, sizeof(cacheEntryS) * capacity);
    *lruCache = cache;
    return 0;
}

//释放一个LRU缓存
int LRUCacheDestory(void *lruCache)
{
    LRUCacheS *cache = (LRUCacheS*)lruCache;
    if (NULL == cache) return 0;

    //free hashMap
    if (cache->hashMap)
        free(cache->hashMap);

    //free linklist
    freeList(cache);
    free(cache);
    return 0;

}

//释放整个链表
static void freeList(LRUCacheS *cache)
{
    //链表为空
    if (0 == cache->lruListSize) return;

    cacheEntryS *entry = cache->lruListHead;
    //遍历删除链表上的所有节点
    while(entry) {
        cacheEntryS *temp = entry->lruListNext;
        freeCacheEntry(entry);
        entry = temp;
    }
    cache->lruListSize = 0;
}

/* *******************************************************
* 双向链表相关接口及实现
******************************************************* */
/*从双向链表中删除指定节点*/
static void removeFromList(LRUCacheS *cache, cacheEntryS *entry)
{
    //链表为空
    if (cache->lruListSize==0)
        return ;

    if ( entry == cache->lruListHead && entry == cache->lruListTail ) {
        //链表中仅剩当前一个节点
        cache->lruListHead = cache->lruListTail = NULL;
    } else if (entry == cache->lruListHead) {
        //欲删除的节点位于表头
        cache->lruListHead = entry->lruListNext;
        cache->lruListHead->lruListPrev = NULL;
    } else if (entry = cache->lruListTail) {
        //欲删除的节点位于表尾
        cache->lruListTail = entry->lruListPrev;
        cache->lruListTail->lruListNext = NULL;
    } else {
        //其他非表头表尾情况,直接删除
        entry->lruListPrev->lruListNext = entry->lruListNext;
        entry->lruListNext->lruListPrev = entry->lruListPrev;
    }

    //删除成功, 链表节点数减1
    cache->lruListSize--;
}

//将节点插入到链表的表头
static cacheEntryS* insertToListHead(LRUCacheS *cache, cacheEntryS *entry)
{

}
