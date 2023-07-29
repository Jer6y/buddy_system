#ifndef DT_H
#define DT_H

#include "buddy.h"

memory_unit mem;
char real_mem[PAGE_SIZE*TOTL_PAGS]__attribute__((aligned(4096))) ={0};
unsigned long long tmp_cnt[TOTL_PAGS]={0};
int ptr =0;
struct page_frame pages[TOTL_PAGS]; 

#endif