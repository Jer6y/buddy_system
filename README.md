# simple_memory_allocator

- 简单的可移植的memory_allocator for jerry   【当目前工作环境为free-standing的时候考虑使用】
- 本文档用于提示jerry 如何使用 ，如何配置，如何移植，以及后续的BUG修复工作

## API

### buddy_allocator

```C
M_API void  m_pool_init(m_pool_t* pool_handler,void* start_addr,int length);
```

- **功能说明:**  初始化一个伙伴分配器
- **参数说明:**
  - **pool_handler:**  类型为m_pool_t 的句柄，由用户自己开辟空间定义（可以在全局区 也可以在栈上）
  - **start_addr:** 需要管理的地址的起始地址
  - **length:** 需要管理的地址的长度 【尽量为PAGE_SIZE的整数倍，如果不是PAGE_SIZE的整数倍，可能并没有完全管理起来！】
- **返回值说明:** 
  - **none** 

```c
M_API void* m_pool_alloc(m_pool_t* pool_handler,int order);
```

- **功能说明:**  向一个伙伴分配器需求 2^(order-1) 大小的页
- **参数说明:**
  - **pool_handler:**  类型为m_pool_t 的句柄，由用户自己开辟空间定义（可以在全局区 也可以在栈上）
  - **order:** 想要分配的大小的阶 应该在 [1,M_MAX_ORDER 之间] [这些配置可以在 m_config.h 里面配置哦!!!!]
- **返回值说明:** 
  - **NULL :** 没有足够的空间分配了
  - **其他:** 所对应的页的起始地址

```c
M_API void  m_pool_free(m_pool_t* pool_handler,void* free_addr,int order);
```

- **功能说明:** 向伙伴分配器释放一个 2^(order-1)大小的页
- **参数说明:**
  - **pool_handler:**  类型为m_pool_t 的句柄，由用户自己开辟空间定义（可以在全局区 也可以在栈上）
  - **free_addr:**  需要释放的地址首地址
  - **order:** 对应的页的阶
- **返回值说明:**
  - **none** 

### slab 

```c
M_API m_slab_t* m_slab_init(void* start,int order,int obj_size);
```

- **功能说明:** 初始化一个slab分配器
- **参数说明:**
  - **start:**  空余空间的起始地址，由伙伴系统分配，也可以是一块自定义的地址，但是如果是自定义的，需要满足(2^(order-1))页大小
  - **order:** 对应的页的阶
  - **obj_size:** slab 管理的对象的块带线啊哦
- **返回值说明:**
  - 返回对应的slab分配器的句柄 【从何而来呢？当然是start 那一块抠出来的啦】

```c
M_API void      m_slab_free(m_slab_t* slab_handler,m_pool_t* pool_handler);
```

- **功能说明:** 释放一个slab分配器, 释放到对应的伙伴分配器的池里去
- **参数说明:**
  - **slab_handler:** slab分配器句柄
  - **pool_handler:**伙伴分配器的句柄
- **返回值说明:**
  - **none** 

```c
M_API void*     m_slab_obj_alloc(m_slab_t* slab_handler);
```

- **功能说明:** 向slab分配器请求分配一个对象
- **参数说明:**
  - **slab_handler:** slab分配器句柄
- **返回值说明:**
  - **NULL:** 没有空间了
  - **其余:**对应的对象的地址

```c
M_API void      m_slab_obj_free(m_slab_t* slab_handler,void* start_addr);
```

- **功能说明:** 向slab分配器释放一个对象
- **参数说明:**
  - **slab_handler:** slab分配器句柄
  - **start_addr:** 释放对象的地址
- **返回值说明:**
  - **none**





## 如何配置

- 查看**m_config.h**文件
  - M_POOL_SIZE -> 默认的伙伴分配器管理的内存大小
  - M_PAGE_SIZE -> 页大小
  - M_MAX_ORDER -> 最大阶数
  - M_DEFAULT_MMPOOL -> 是否开启默认伙伴分配器和默认的内存池子 【若无，则M_POOL_SIZE 无意义】
  - M_ARG_CHECK_FUNC -> 是否开启入口参数检查

### 如何移植

- 根据需求配置好资源量以及使能想要的模块
- 实现**m_port_user.h** 中要求实现的**宏**
  - EXIT_WHEN_ERROR_HAPPEN(number) -> 当发生一些错误时，需要执行的函数，number是错误码，比如开启的错误校验，如果应用于内核中，发生入口参数错误，需要直接 kernel panic
  - M_PRINTF_NORMAL -> LEVEL 级别为INFO的打印输出函数
  - M_PRINTF_WARN -> LEVEL级别为WARN的打印输出函数
  - M_PRINTF_ERROR -> LEVEL 级别的ERROR的打印输出函数
  - M_LOCK_T -> 锁数据结构
  - M_LOCK_INIT -> 锁初始化函数
  - M_LOCK -> 上锁函数
  - M_UNLOCK -> 解锁函数
  - M_ADDR_T -> 与架构相关的地址线长度变量，比如，如果字长是64位 应该是 unsigned long long
- 有一些宏是固定好了接口，需要根据接口实现
- 有一些宏使用可变宏参数，可以接入任意的参数，这个就可以用户来自己定义接口参数，比如M_LOCK_INIT
