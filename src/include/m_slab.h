#ifndef __M_SLAB_H
#define __M_SLAB_H


typedef struct m_slab_s {
    m_type_t obj_type;
    int      obj_all;
    int      obj_free;
    int      mem_free_head;
    int      mem_free_tail;
    int*     mem_index_start;
    void*    mem_obj_start;
    void*    r_start;
    int      order;
    int      obj_size;
    M_LOCK_T slab_lock;
    mln_list_t _node;
} m_slab_t;

M_API m_slab_t* m_slab_init(void* start,int order,int obj_size);

M_API void      m_slab_free(m_slab_t* slab_handler);

M_API void*     m_slab_obj_alloc(m_slab_t* slab_handler);

M_API void      m_slab_obj_free(m_slab_t* slab_handler,void* start_addr);


#endif