#include "../include/buddy.h"


static void dump_page(struct page_frame* page)
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


static void _dump()
{
    
        int cnt =0;
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
        
}

void assert_failed(char* line,int line_id)
{
    _dump();
    printf("\n------failed-----\n");
    printf("cause :%s %d\n",line,line_id);
    raise(SIGINT);
}