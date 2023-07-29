#include "include/dt.h"
#include <time.h>
#include <semaphore.h>


void dump_page(struct page_frame* page)
{
    if(page->order !=0)
    {
        if(page->flags &PAGE_ALLOC)
        {
            assert(((page->buddy_list).next==NULL && (page->buddy_list).prev==NULL));
            printf("已分配: 起始页号:%d ,块大小:%d \n",(int)GET_ID(page->page_addres),(1<<(page->order-1)));
        }
        else 
            printf("未分配: 起始页号:%d ,块大小:%d \n",(int)GET_ID(page->page_addres),(1<<(page->order-1)));
    }
    else 
    {
        assert(page->flags ==0 );
        assert(((page->buddy_list).next==NULL && (page->buddy_list).prev==NULL));
    }
}
void _dump()
{
        int cnt =0;
        pthread_mutex_lock(&(mem.mutex));
        printf("---------------DUMP_THREAD:start---------------\n");
        for(int i=0;i<TOTL_PAGS;i++)
        {
            if(pages[i].order!=0) cnt++;
            dump_page(&(pages[i]));
        }
        printf("-----------------------------------------------\n");
        printf("未分配: %d, 已分配: %d\n",cnt,TOTL_PAGS -cnt);
        for(int i=0;i<MAX_POOL_SIZE;i++)
        {
            printf("-----------------------------------------------\n");
            printf("%d号池: 剩下页数:%d \n",i,mem.pools[i].free_pages);
            struct list_head* ptr = list_next(&(mem.pools[i].list)); 
            while(ptr !=NULL)
            {
                dump_page((struct page_frame *)ptr);
                ptr = list_next(ptr);
            }
            printf("-----------------------------------------------\n");
        }
        printf("---------------DUMP_THREAD:end-----------------\n");
        fflush(0);
        pthread_mutex_unlock(&(mem.mutex));
}

void* dump_thread(void* arg)
{
    
    while(1)
    {
        _dump();
        sleep(1);
    }
    return NULL;
}



void* thread_tmp(void* arg)
{
    int n = *((int *)arg);
    struct page_frame *tmp[150] = {0};
    int ptr=0;
    for(ptr=0;ptr<30;)
    {
        int s = (rand()%(MAX_POOL_SIZE-2))+1;
        struct page_frame* t  = kalloc(s);
        if(t !=NULL)
            tmp[ptr++] = t;
        if(ptr %5) sleep(1);
    }
    ptr--;
    for(;ptr>=0;ptr--)
        kfree(tmp[ptr]);

    return NULL;
}

int main()
{
    srand((unsigned)time( NULL ));
    kinit();
    pthread_t a1;
    pthread_t a2;
    pthread_t a3;
    pthread_t a4;
    int n1=1;
    int n2=2;
    int n3=3;
    pthread_create(&a1,NULL,thread_tmp,&n1);
    pthread_create(&a2,NULL,thread_tmp,&n2);
    pthread_create(&a3,NULL,thread_tmp,&n3);
    pthread_create(&a4,NULL,dump_thread,NULL);
    
    pthread_join(a1,NULL);
    pthread_join(a2,NULL);
    pthread_join(a3,NULL);
    pthread_join(a4,NULL);
}