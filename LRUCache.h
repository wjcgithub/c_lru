/*
* LRU 缓存接口
*/
#ifndef LRUCACHE_H_INCLUDED
#define LRUCACHE_H_INCLUDED

//创建L缓存
int LRUCacheCreate(int capacity, void **lruCache);
//销毁LRU缓存
int LRUCacheDestory(void *lruCache);
//将数据放入LRU缓存中
int LRUCacheSet(void *lruCache, char key, char data);
//从缓存中获取数据
char LRUCacheGet(void *lruCache, char key);
//打印缓存中的数据,按照访问时间从新到旧的顺序输出
void LRUCachePrint(void *lruCache);

#endif // LRUCACHE_H_INCLUDED
