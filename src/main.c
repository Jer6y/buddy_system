#include "include/memory.h"
#include <time.h>

// int global =0;
// void* thread_(void* arg)
// {
//     while(global == 0) ; 
//     void* tmp[10] = {0};
//     int   order_t[10] = {0};
//     for(int i=0;i<10;i++)
//     {  
//         int order =1 + rand()%2;
//         order_t[i]=order;
//         tmp[i] = m_pool_alloc(&d_mmpool_handler,order);
//     }    
//     for(int i=0;i<10;i++)
//     {
//         if(tmp[i]!= NULL)
//         {
//             m_pool_free(&d_mmpool_handler,tmp[i],order_t[i]);
//         }
//     }
//     return NULL;
// }
int main()
{
    srand((unsigned)time(NULL));
    module_memory_init();
    void* start = m_pool_alloc(&d_mmpool_handler,3);

    m_slab_t* slab_handler = m_slab_init(start,3,16);
    void* tmp[1000];
    for(int i=0;i<1000;i++)
    {
        tmp[i] = m_slab_obj_alloc(slab_handler);
        if(i==813)
        {
            i=813;
        }
    }
    for(int i=0;i<1000;i++)
    {
        if(tmp[i] != (void*)0)
        {
            m_slab_obj_free(slab_handler,tmp[i]);
        }
        if(i==813)
        {
            i=813;
        }
    }

    m_slab_free(slab_handler);
    
    return 0;
}