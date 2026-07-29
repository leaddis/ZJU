## Splines

工具箱的目标是实现Matlab中curve fitting Tool box的样条插值功能。

## 源码库结构
- `cmake` cmake 辅助脚本
- `deps` 放置内核二进制文件和头文件等依赖
- `scripts` 使用 m 脚本实现的功能函数
- `src` 放置源代码
- `test` 测试代码，用于编译测试例子

## 构建说明
- `cd splines`进入工程目录
- 创建其他依赖库的路径`mkdir -p deps/core`
- 获取`baltam_sdk`插件库，将其解压到`deps/core`目录下。

解压完毕后的结构：
```
Linux:
- deps/core
  - include
  - lib
  - src

Windows
- deps/core
  - include
  - lib
  - src
  - bin
```  
### Linux 下编译
进入到 splines 目录层后新建一个build 目录用于存储编译的文件
```
cd splines
mkdir build
```
进行 configure
```
cd build
cmake ..
```
进行编译：
```
make -jN # N 表示开启的线程数
```

### windows 下编译 (MinGW64)
进入到 splines 目录层后新建一个build 目录用于存储编译的文件
```
cd splines
mkdir build
```
进行 configure
```
cd build
cmake .. -G "MinGW Makefiles"
```
进行编译：
```
mingw32-make -jN # N 表示开启的线程数
```

### (附)支持的 cmake 开关:
* `CMAKE_BUILD_TYPE` 编译类型，一般为 `Debug`（默认），和 `Release`
* `BUILD_TEST` 是否编译测试单元，可以为 `OFF`（默认），或者 `ON`。注：设置为 `ON` 会增加较多编译的时间
* `EXPORT_SYMS` 是否导出库符号，可以为 `ON`（默认），或者 `OFF`。当 `BUILD_TEST` 为 `ON` 时，因为编译测试单元需要导出的符号，这个开关会被强制设置为 `ON`。当需要发布时，请将 `EXPORT_SYMS` 设置为 `OFF` 以便减小库文件大小和隐藏内部符号。
