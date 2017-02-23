#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LRUCache.h"
#include "LRUCacheImpl.h"
static void freeList(LRUCacheS *cache);
//static cacheEntryS* insertToListHead(LRUCacheS *cache, cacheEntryS *entry);

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
    cacheEntryS *removeEntry = NULL;

    if (++cache->lruListSize > cache->cacheCapacity) {
        //如果缓存满了, 即链表当前节点数已等于缓存容量, 那么需要先删除链表表尾节点, 即淘汰最久没有被访问到的缓存数据单元
        removeEntry = cache->lruListTail;
        removeFromList(cache, cache->lruListTail);
    }

    if (cache->lruListHead==NULL && cache->lruListTail==NULL) {
        //如果当前链表为空链表
        cache->lruListHead = cache->lruListTail = entry;
    } else {
        //当前链表非空, 插入到表头
        entry->lruListNext = cache->lruListHead;
        entry->lruListPrev = NULL;
        cache->lruListHead->lruListPrev = entry;
        cache->lruListHead = entry;
    }

    return removeEntry;
}

//用于保证最近访问的节点总是位于链表表头
static void updateLRUList(LRUCacheS *cache, cacheEntryS *entry)
{
    //将节点从链表中摘除
    removeFromList(cache, entry);
    //将节点插入到链表表头
    insertToListHead(cache, entry);
}


/* *******************************************************
* 哈希表相关接口及实现
******************************************************* */
//哈希函数
static int hashKey(LRUCacheS *cache, char key)
{
    return (int)key%cache->cacheCapacity;
}

//从哈希表获取缓存单元
static cacheEntryS* getValueFromHashMap(LRUCacheS *cache, int key)
{
    //使用哈希函数确认数据存放的位置
    cacheEntryS *entry = cache->hashMap[hashKey(cache,key)];

    //遍历查询槽内链表, 找到准确的数据项
    while (entry) {
        //如果没有哈希冲突的那么直接就能定位到
        if (entry->key == key)
            break;

        //否则如果有哈希冲突的话, 就循环这个冲突链上的每个元素, 直到找到和key匹配的元素
        entry = entry->hashListNext;
    }

    return entry;
}

//向哈希表中插入缓存单元
static void insertentryToHashMap(LRUCacheS *cache, cacheEntryS *entry)
{
    //使用哈希函数确认数据存放的位置, 并查看当前位置是否已经有数据项了
    cacheEntryS *n = cache->hashMap[hashKey(cache, entry->key)];
    if (n != NULL) {
        //说明有哈希冲突了, 那么就要将新加入的数据项与当前项连成新的链表, 并且当前新插入的数据要当表头
        entry->hashListNext = n;
        n->hashListPrev = entry;
    }

    //如果当前数据存放位置为NULL, 那么就将当前数据项加入到当前位置
    cache->hashMap[hashKey(cache,entry->key)] = entry;
}

//从哈希表删除缓存单元
static void removeEntryFromHashMap(LRUCacheS *cache, cacheEntryS *entry)
{
    //无需做任何删除操作的情况
    if (NULL ==  cache || NULL ==  entry || NULL ==  cache->hashMap) return ;

    //使用哈希函数定位数据存放在哪个位置
    cacheEntryS *n = cache->hashMap[hashKey(cache, entry->key)];
    //bianli链表, 找到预删除节点,将节点从链表删除
    while (n) {
        if (n->key = entry->key) {
            //找到并删除
            if (n->hashListPrev) {
                //说明不是头节点
                n->hashListPrev->hashListNext = n->hashListNext;
            } else {
                //说明是头节点
                cache->hashMap[hashKey(cache, entry->key)] = n->hashListNext;
            }

            //将新节点的上一个节点,指向原来节点的上一个节点, 可能有值, 也可能为NULL
            if (n->hashListNext)
                 n->hashListNext->hashListPrev = n->hashListPrev;

             return ;
        }

        n = n->hashListNext;
    }
}


/* *******************************************************
* 缓存操作接口及实现
******************************************************* */
int LRUCacheSet(void *lruCache, char key, char data)
{
    LRUCacheS *cache = (LRUCacheS *)lruCache;

    //从哈希表查找数据是否已经在缓存中
    cacheEntryS *entry = getValueFromHashMap(cache, key);
    if (entry != NULL) {
        //说明数据已经在缓存中了
        entry->data = data;
        updateLRUList(cache, entry);
    } else {
        //数据没在缓存中, 新建缓存单元
        entry = newCacheEntry(key, data);

        //将新建缓存单元插入链表表头
        cacheEntryS *removeEntry = insertToListHead(cache, entry);
        if (NULL != removeEntry) {
            //新建缓存单元过程中, 发生缓存满了的情况, 需要淘汰最久没有被访问到的缓存数据单元
            removeEntryFromHashMap(cache, removedEntry);
            freeCacheEntry(removedEntry);
        }

        /*将新建缓存单元插入哈希表*/
        insertentryToHashMap(cache, entry);
    }

    return 0;
}
