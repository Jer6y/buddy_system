# buddy_system
Buddy_System 内存管理 C

最后提供的API: <br/>
struct page_frame* kalloc(int order);         // -> 分配2^(order-1)页 返回描述符地址  <br/>
void  kfree(struct page_frame* );             //  -> 释放 struct page_frame* 指向的块  <br/>
(this two API is Thread-Safety)  <br/>
 <br/>
// 线程安全做的比较暴力 ( 一把大锁保平安 ) (注释的版本是分几把锁进行安全保护  还未测试) <br/>
// 数据结构申明和函数申明 在/sr/include/buddy.h 里 <br/>
// ( buddy_system的ALLOC和FREE算法是 根据wikipeda 的描述YY出来的，时间都是O(1) 的) <br/>
 <br/>
 <br/>
后续得在buddy_system上做slab <br/>
