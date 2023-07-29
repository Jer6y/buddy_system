#include "../include/buddy.h"

void  list_add_tail(struct list_head* old ,struct list_head* new_list)
{
    assert( old!=0 && new_list!=0);
    old->next = new_list->next;
    new_list->next = old;
    old->prev = new_list;
    if(old->next!= NULL)
        old->next->prev = old;
}
void  list_add_head(struct list_head* old,struct list_head* new_list)
{
    assert(old !=0 && new_list !=0);
    old->next = new_list;
    old->prev = new_list->prev;
    new_list->prev = old;
    if(old->prev!=NULL)
        old->prev->next = old;
}
void  list_del(struct list_head* old )
{
    assert(old!=0);
    if((old->prev) !=0)
        (old->prev)->next = old->next;
    if((old->next) !=0)
        (old->next)->prev = old->prev;
    old->next =0;
    old->prev =0;
}
void  list_remove_to_tail(struct list_head* old,struct list_head* new_list)
{
    list_del(old);
    list_add_tail(old,new_list);
}
void  list_remove_to_head(struct list_head* old,struct list_head* new_list)
{
    list_del(old);
    list_add_head(old,new_list);
}
void  list_init(struct list_head* node)
{
    node->next = 0;
    node->prev = 0;
}
struct list_head* list_prev(struct list_head* node_ptr)
{
    return node_ptr->prev;
}
struct list_head* list_next(struct list_head* node_ptr)
{
    return node_ptr->next;
}
void  page_frame_set_id(int id,uint64 adrs,uint8 flag,int order)
{
    assert(id>=0 && id<TOTL_PAGS);
    if(order != PG_SET_KEEP_ORDER)
    pages[id].order = order;
    if(flag !=  PG_SET_KEEP_FLAG )
    pages[id].flags = flag;
    if(adrs !=  PG_SET_KEEP_ADDR )
    pages[id].page_addres = (void *)adrs;
}

void  page_frame_set_ptr(struct page_frame* ptr,uint64 adrs,uint8 flag,int order)
{
    assert(ptr!=0);
    if(order != PG_SET_KEEP_ORDER)
    ptr->order = order;
    if(flag != PG_SET_KEEP_FLAG )
    ptr->flags = flag;
    if(adrs != PG_SET_KEEP_ADDR)
    ptr->page_addres = (void *)adrs;
}

struct page_frame*  page_prev(struct page_frame* page)
{
    assert(page != 0);
    int id = page_frame_pg_to_id(page);
    if(id ==0) return NULL;
    int prev_id =  (id - (1<<(page->order-1)));
    return page_frame_id_to_pg(prev_id);
}
struct page_frame*  page_next(struct page_frame* page)
{
    assert(page != 0);
    int id = page_frame_pg_to_id(page);
    int next_id =  (id + (1<<(page->order-1)));
    if(next_id >= TOTL_PAGS) return NULL;
    return page_frame_id_to_pg(next_id);
}

struct page_frame*  page_frame_id_to_pg(int id)
{
    assert(id>=0 && id<TOTL_PAGS);
    return &(pages[id]);
}

int page_frame_pg_to_id(struct page_frame* pg)
{
    assert(pg !=0);
    return (int)GET_ID((uint64)(pg->page_addres));
}

struct page_frame*  page_split(struct page_frame* page)
{
    assert(page!=NULL && page->order != 1);
    int old_id = page_frame_pg_to_id(page);
    int new_id = old_id + ((1<<(page->order-1))>>1);
    assert(new_id >=0 && new_id <TOTL_PAGS);
    int new_order = page->order-1;
    
    page_frame_set_id(old_id,PG_SET_KEEP_ADDR,PG_SET_KEEP_FLAG,new_order);
    page_frame_set_id(new_id,(uint64)real_mem+new_id*PAGE_SIZE,0,new_order);
    return page_frame_id_to_pg(new_id);
}

void kinit()
{
    pthread_mutex_init(&(mem.mutex),NULL);
    mem.page_size =PAGE_SIZE;
    mem.page_nums =TOTL_PAGS;
    for(int i=0;i<TOTL_PAGS;i++)
    {
        list_init(&(pages[i].buddy_list));
        page_frame_set_id(i,0,0,0); 
    }
    for(int i=0;i<MAX_POOL_SIZE ;i++)
    {
        list_init((struct list_head*)(&(mem.pools[i].list)));
        mem.pools[i].pool_pg_nums = (1<<i);
        mem.pools[i].free_pages =0;
        pthread_mutex_init(&(mem.pools[i].mutex),NULL);
    }
    
    for(int i=0;i<(TOTL_PAGS/MAX_POOL_NPGS);i++)
    {
        page_frame_set_id(i*MAX_POOL_NPGS,((uint64)real_mem + i*MAX_POOL_NPGS*PAGE_SIZE),0,MAX_POOL_SIZE);
        list_add_tail(&(pages[i*MAX_POOL_NPGS].buddy_list),&(mem.pools[MAX_POOL_SIZE-1].list));
        mem.pools[MAX_POOL_SIZE-1].free_pages++;
    }

}

struct page_frame* kalloc(int order)
{
    if(order >MAX_POOL_SIZE || order<=0 ) return 0;
    int old_order = order;
    
    pthread_mutex_lock(&(mem.mutex));
    while(order <= MAX_POOL_SIZE ) 
    {
        if(mem.pools[order-1].free_pages <=0)  order++;
        else
            break;
    }
    if(order >MAX_POOL_SIZE)  
    {
        pthread_mutex_unlock(&(mem.mutex));
        return 0;
    }
    
    struct list_head* mem_ret = list_next((&(mem.pools[order-1].list)));
    assert(mem.pools[order-1].free_pages>=0);
    assert(mem_ret !=NULL);
    list_del(mem_ret); 
    mem.pools[order-1].free_pages--;

    

    struct page_frame* page_ptr = list_entry(mem_ret,struct page_frame,buddy_list);
    if(order != old_order)
    {
        while(1)
        {
            struct page_frame* page_tmp = page_split(page_ptr);
            list_add_tail(&(page_ptr->buddy_list),&(mem.pools[page_ptr->order-1].list)); 
            mem.pools[page_ptr->order-1].free_pages++;

            page_ptr = page_tmp;
            if(page_ptr->order == old_order)
                break;
        }
    }
    page_frame_set_ptr(page_ptr,PG_SET_KEEP_ADDR,PAGE_ALLOC,PG_SET_KEEP_ORDER);
    pthread_mutex_unlock(&(mem.mutex));
    return page_ptr;
}

static int is_next(uint64 a,uint64 b,int pg_size,int page_nums)
{
    if(a+pg_size*page_nums == b || b +pg_size*page_nums == a) return 1;
    return 0;
}

// void kfree(struct page_frame* page_ptr)
// {
//     pthread_mutex_lock(&(mem.mutex));
//     assert(page_ptr!=NULL && page_ptr->order != 0);
//     assert(page_ptr ->flags & PAGE_ALLOC);
//     assert(page_ptr->buddy_list.next==NULL && page_ptr->buddy_list.prev ==NULL);
//     int order = page_ptr->order;
    
//     if(order == MAX_POOL_SIZE)
//     {
//         page_frame_set_ptr(page_ptr,PG_SET_KEEP_ADDR,0,PG_SET_KEEP_ORDER);
//         list_add_tail(&(page_ptr->buddy_list),&(mem.pools[order-1].list));
//         mem.pools[order-1].free_pages++;
//     }
//     else
//     {
//         while(order <= MAX_POOL_SIZE)
//         {
//             struct list_head* ptr = list_next(&(mem.pools[order-1].list));
//             while(ptr!=NULL)
//             {
//                 struct page_frame* pg_tmp =  list_entry(ptr,struct page_frame,buddy_list);
//                 uint64 ad_tmp = (uint64)(pg_tmp->page_addres);
//                 uint64 ad_ptr = (uint64)(page_ptr->page_addres);
//                 if(is_next(ad_tmp,ad_ptr,PAGE_SIZE,(1<<(page_ptr->order-1))))
//                 {
//                     assert( pg_tmp->buddy_list.prev !=NULL);
//                     list_del(&(pg_tmp->buddy_list));
//                     mem.pools[(pg_tmp->order)-1].free_pages--;
//                     if(ad_tmp<ad_ptr)
//                     {
//                         page_frame_set_ptr(pg_tmp,PG_SET_KEEP_ADDR,0,order+1);
//                         page_frame_set_ptr(page_ptr,PG_SET_KEEP_ADDR,0,0);
//                         page_ptr = pg_tmp;
//                     }
//                     else
//                     {
//                         page_frame_set_ptr(page_ptr,PG_SET_KEEP_ADDR,0,order+1);
//                         page_frame_set_ptr(pg_tmp,PG_SET_KEEP_ADDR,0,0);
//                     }
//                     order++;
//                     break;
//                 }
//                 ptr = ptr->next;
//             }
//             if(ptr == NULL || order ==MAX_POOL_SIZE)
//             {
//                 page_frame_set_ptr(page_ptr,PG_SET_KEEP_ADDR,0,order);
//                 list_add_tail(&(page_ptr->buddy_list),&(mem.pools[order-1].list));
//                 mem.pools[order-1].free_pages++;
//                 break;
//             }
//         }
//     }
    
//     pthread_mutex_unlock(&(mem.mutex));
    
// }



void kfree(struct page_frame* page_ptr)
{
    assert(page_ptr!=NULL );
    assert((page_ptr->order >0 && page_ptr->order <= MAX_POOL_SIZE));
    assert(page_ptr ->flags & PAGE_ALLOC);
    assert(page_ptr->buddy_list.next==NULL && page_ptr->buddy_list.prev ==NULL);
    int order = page_ptr->order;
    pthread_mutex_lock(&(mem.mutex));
    if(order == MAX_POOL_SIZE)
    {
        page_frame_set_ptr(page_ptr,PG_SET_KEEP_ADDR,0,PG_SET_KEEP_ORDER);
        list_add_tail(&(page_ptr->buddy_list),&(mem.pools[order-1].list));
        mem.pools[order-1].free_pages++;
    }
    else
    {
        while(1)
        {
            struct page_frame* page_bef = page_prev(page_ptr);
            struct page_frame* page_nex = page_next(page_ptr);
            int left_merge =0,right_merge=0;
            if(page_bef != NULL)
                left_merge  = (page_bef->order == order && (page_bef->flags ==0));
            if(page_nex != NULL)
                right_merge = (page_nex->order == order && (page_nex->flags ==0));
            if(left_merge ==0 && right_merge ==0)
            {
                page_frame_set_ptr(page_ptr,PG_SET_KEEP_ADDR,0,PG_SET_KEEP_ORDER);
                list_add_tail(&(page_ptr->buddy_list),&(mem.pools[order-1].list));
                mem.pools[order-1].free_pages++;
                break;
            }
            else
            {
                if(left_merge==1)
                {
                    list_del(&(page_bef->buddy_list));
                    mem.pools[order-1].free_pages--;
                    page_frame_set_ptr(page_bef,PG_SET_KEEP_ADDR,0,order+1);
                    page_frame_set_ptr(page_ptr,PG_SET_KEEP_ADDR,0,0);
                    order++;
                    page_ptr = page_bef;
                }
                else
                {
                    list_del(&(page_nex->buddy_list));
                    mem.pools[order-1].free_pages--;
                    page_frame_set_ptr(page_ptr,PG_SET_KEEP_ADDR,0,order+1);
                    page_frame_set_ptr(page_nex,PG_SET_KEEP_ADDR,0,0);
                    order++;
                }
                if(order == MAX_POOL_SIZE)
                {
                    page_frame_set_ptr(page_ptr,PG_SET_KEEP_ADDR,0,PG_SET_KEEP_ORDER);
                    list_add_tail(&(page_ptr->buddy_list),&(mem.pools[order-1].list));
                    mem.pools[order-1].free_pages++;
                    break;
                }
            }
        }
    }
    
    pthread_mutex_unlock(&(mem.mutex));
}
