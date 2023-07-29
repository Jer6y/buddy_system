#ifndef BUDDY_H
#define BUDDY_H

#include "types.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>


/**************************************宏定义****************************************/

#define PAGE_SIZE   16

#define TOTL_PAGS   1024

#define MAX_POOL_SIZE   6

#define MAX_POOL_NPGS   (1 <<(MAX_POOL_SIZE -1))

#define PAGE_ALLOC  (1<<0)

#define PG_SET_KEEP_ADDR  (0xffffffffffffffff)

#define PG_SET_KEEP_ORDER (-1)

#define PG_SET_KEEP_FLAG  (0xff)


/**************************************结构体定义****************************************/

struct list_head;
struct memory_unit;
struct memory_pool;
struct page_frame;


typedef struct list_head
{
    struct list_head* next;
    struct list_head* prev;
} list_head;

typedef struct page_frame
{
    struct      list_head buddy_list;
    void *      page_addres;
    uint8       flags;
    uint32      count;
    int         order;
}page_frame;

typedef struct memory_pool
{
    struct list_head    list            ;
    pthread_mutex_t     mutex           ;
    unsigned int        pool_pg_nums    ;
    unsigned int        free_pages      ;
} memory_pool;

typedef struct memory_unit
{
    struct memory_pool  pools[MAX_POOL_SIZE];
    pthread_mutex_t     mutex;
    unsigned int        page_nums;
    unsigned int        page_size;
} memory_unit;


/**************************************通用数据结构****************************************/

/**************************************通用数据结构宏**************************************/

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})




#define list_entry(ptr,type,member)   (container_of(ptr,type,member))

/**************************************通用数据结构函数***********************************/

void    list_add_tail(struct list_head* ,struct list_head* );
void    list_add_head(struct list_head* ,struct list_head* );
void    list_del(struct list_head* );
void    list_remove_to_tail(struct list_head* old,struct list_head* new_list);
void    list_remove_to_head(struct list_head* old,struct list_head* new_list);
void    list_init(struct list_head* node);
struct list_head* list_prev(struct list_head* node_ptr);
struct list_head* list_next(struct list_head* node_ptr);


/**************************************debug_util****************************************/


#define assert(x) ({ if(!(x)) assert_failed(__FILE__,__LINE__); })

void assert_failed(char* line,int line_id);








extern memory_unit          mem;
extern char                 real_mem[];
extern struct page_frame    pages[];



#define GET_ID(addrs)   (((uint64)(addrs) - (uint64)real_mem)/PAGE_SIZE)

void  page_frame_set_id(int id,uint64 adrs,uint8 flag,int order);
void  page_frame_set_ptr(struct page_frame* ptr,uint64 adrs,uint8 flag,int order);
struct page_frame*  page_frame_id_to_pg(int id);
int page_frame_pg_to_id(struct page_frame* pg);
struct page_frame*  page_split(struct page_frame* page);
struct page_frame*  page_prev(struct page_frame* page);
struct page_frame*  page_next(struct page_frame* page);

void                kinit();
struct page_frame*  kalloc(int order);
void                kfree(struct page_frame*);

#endif