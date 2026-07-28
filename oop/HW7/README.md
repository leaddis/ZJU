# Memory Pool Allocator Project

## 1. 项目概述
实现一个自定义内存分配器，使用内存池优化小块内存分配的效率，并将其应用于 `std::vector`。

## 2. 文件结构
```
HW7 ├─ src │ 
        ├─ allocator.h │ 
        ├─ buffer.h │ 
        └─ memory_pool.h 
    ├─ test │ 
        ├─ PTA_test.cpp │ 
        ├─ PTA_stl_test.cpp │ 
        ├─ test_allocator.cpp │ 
        ├─ test_buffer.cpp │ 
        ├─ test_memory_pool.cpp │
        └─ TestObject.h 
    ├─ makefile 
    └─ README.md
```
## 3. 构建与运行
我电脑上的结果已经展示在了report中，我的系统是Ubuntu20.04。
### Makefile
```bash
make
```
随后可以在终端中看到PTA和stl之间的差异。
## 4. 分别构造测试


```bash
make PTA
make stl
make allocator
make buffer
make mp
```

## 5.删除
```bash
make clean
```

