# 北太天元-曲线拟合插件开发日志

## 2024.8.29（钱周越）

-  `aveknt` 函数实现以及文档完成
- 添加 `aveknt.h`
- 添加 `aveknt.cpp`

## 2024.8.29（陈震翔）

-  `aptknt`、`augknt` 函数实现以及文档完成
- 添加 `aptknt\aptknt.h`
- 添加 `aptknt\aptknt.cpp`
- 添加 `augknt\augknt.h`
- 添加 `augknt\augknt.cpp`

## 2024.8.29 （边辰昊）

- `sorted/sorted.cpp` 修改
- 修改原因：空向量不用特判

## 2024.8.27 （边辰昊）

- `sorted/sorted.cpp` 添加
- `sorted/sorted.h` 添加
- `knt2mlt/knt2mlt.cpp` 添加
- `knt2mlt/knt2mlt.h` 添加

## 2024.8.20 （边辰昊）

- `Splines/Base_Spline.cpp` 修改
- `fnval/fnval.cpp` 修改
- `Splines/SpapiSolver.h` 修改
- 修改原因：
  调整较多，整体维持B样条区间左闭右开
  
## 2024.8.8 （边辰昊）

- `csape.cpp`中240行：
  ```cpp
  if (pX->cols() <= 4)
  ```
- 修改为：
  ```cpp
  if (pX->cols() < 4)
  ```
- 修改原因：
  调整横坐标矩阵列数的检查逻辑，确保只有在列数小于4时才进行报错
- `csape.cpp`中108行：
- ```cpp
  c.derivative2 = csape_left_default_value(x_left_default,y_left_default);
  ```
- 修改为：
  ```cpp
  c.derivative1 = csape_left_default_value(x_left_default,y_left_default);
  ```
- 修改原因：
  左边界条件是derivative1

## 2023.8.18（叶景文）
- 将`spapisolver.h`中的矩阵计算从`eigen3`转为`lapacke`<font color=red>***待测试***</font>

## 2023.8.13（叶景文）
- 将`spline.h`和`spline.cpp`中三阶ppform样条的矩阵计算从`eigen3`转为`lapacke`<font color=red>***待测试***</font>

## 2023.6.21（徐同杉）
- 新增测试`test_spmak.m`和`test_spapi.m`
- 修改了`spapi`和`spmak`文档中的几处小错误和不清楚的地方


## 2023.6.17（曹绍祯）
- 新增`fnval`的内部调用形式
  ```cpp
  // 内部调用，调用者需保证输入的样条结构体的合法性，返回 S(x),
  // 如要得到多个点的值，请使用 vector 参数重载形式
  double fnval(const baltam::structure & S, double x);
  // 内部调用，调用者需保证输入的样条结构体的合法性，返回 [y_1 , ... , y_n] = S([x_1 , ... , x_n])
  std::vector<double> fnval(const baltam::structure & S, const std::vector<double> & X);
  ```
- 修改`Base_Spline::operator()(double x)`的实现，因为内部实现在重节点处存储了很多零多项式，因此对于节点为$[0,0,0,0,1]$的这个基函数来说会有 $B(0) = 0$，但是更合理的是 $B(0) =1$，因此在求值内部新增检查是否落在零多项式里边的功能，这样实现视为存储的多余的零多项式不存在，具体修改为
  ```cpp
  double Base_Spline::operator()(double x) const
  {
    // ......
    for (int i = 0; i <= N; i++)
    {
        if (x <= knot(i))
        {
            int j = i; // 如果这是一个重结点（跟前边结点相同），就往下顺延
            while(knot(j-1) == knot(j) && j< N) // j < N 不能超过结点下标界
                j++;
            return pp[j](x);
        }
    }
    return 0; // never reached
  }
  ```  
  若有更好的想法或者发现这样实现有问题随时改动。

## 2023.6.17（叶景文）
- 新增`fnbrk`截取区间的功能以及对B样条的支持,并新增相应测试
- 新增`csape`的测试
- 对`fnbrk`截取区间的功能新增函数说明
## 2023.6.15（徐同杉）

- `spapi/spapi.cpp` 新增 `baltam::structure *spapi(std::vector<double> knots, std::vector<double> x, std::vector<double> y)`, 用于内部调用，返回一个B样条结构体指针 

## 2023.6.15（杨钧尹）

- 更新了 `fn2fm` 的实现方法，待其所依赖的 `fnder, fnval, spapi` 修改接口后进行测试

  - pp 转 B 采用以下方式：
    - B样条插值节点调用 **augknt(pp_knots, order)** 得到。
    - **order** 保持不变, **length(knots)**=order+number. 而由1已知length(knots)，因此可以求得其他几项属性值。
    - 构造所得的新节点序列及其插值条件按照 **spapi()** 的规则生成B样条。

  - B 转 pp 采用以下方式
    - 由 number 和 length(knots), 使用 **bspline()** 给出 B 样条的各项 Base_Spline 的 pp 形式结构体。
    2. 对各段按照 coefs 给出的系数进行累加，从而得到各段的多项式的系数。

## 2023.6.13（徐同杉）

- `SpapiSolver.h`新增`SpapiSolver(k,x,y)`用以实现`spapi(k,x,y)`
- 对`splines_common.cpp`中`augknt`的错误进行改正，对`aptknt`进行了修改


## 2023.6.13（曹绍祯）
- `fnder`函数在B格式下的求导功能完成，可以通过`spapi`函数中第一个用例的测试，积分部分待实现

- `fnder`函数文档中新增实现说明

- 根据`fnder`函数实现说明，其在求导过后两侧低一阶的基函数的结点若是全部相同的，返回结果将会把这样收尾处的基函数剔除
  $$
  \begin{equation}
    \begin{aligned}
        S^\prime(x) &= \sum_{i=1}^{m-n+1} c_i \left[B_i^{n}(x)\right]^\prime \\
            &= \sum_{i=1}^{m-n+1} c_i \left[\alpha_i B_i^{n-1}(x) - \beta_i B_{i+1}^{n-1}(x)\right] \\
            &= c_1 \alpha_1 B_1^{n-1}(x) + \sum_{i=2}^{m-n+1}(c_i \alpha_i -c_{i-1}\beta_{i-1})B_{i}^{n-1}(x) 
               - c_{m-n+1} \beta_{m-n+1} B_{m-n+2}^{n-1}(x)
    \end{aligned}
  \end{equation}
  $$
  即以上公式中$B_1^{n-1}(x),B_{m-n+2}^{n-1}(x)$的结点是全部相同的话，计算的结果将会被赋为（这是根据在MATLAB中实验看出的）
  $$
  \begin{equation}
        S^\prime(x) = \sum_{i=2}^{m-n+1}(c_i \alpha_i -c_{i-1}\beta_{i-1})B_{i}^{n-1}(x) 
  \end{equation}
  $$
  以
  ```matlab
  knots = [0, 0, 0, 0, 1, 2, 2, 2, 2];
  x = [0, 1, 1, 1, 2];
  y = [2, 0, 1, 2, -1];
  s = spapi(knots, x, y);
  ```
  为例，这里有
  $$
  S(x) = \sum_{i=1}^ 5 a_i B_i^3(x),\quad 
  \left[ B_1^3(x) \right]^\prime = \frac{3}{0-0}B_1^2(x) - \frac{3}{1-0}B_2^2(x),\quad 
  \mathrm{knot} \left\{ B_1^2(x) \right\} = [0,0,0,0]
  $$
  于是$B_1^2(x)$将会从计算结果中消失。这里我担心的一个问题是：这样的求导之后结点都相同的基函数一定是出现在区间端点处吗？目前的实现是认为以上问题的答案是肯定的。
## 2023.6.13（杨钧尹）

- 于 `splines_common` 完成 `aptknt`, `aveknt`, `augknt` 等依赖函数
- 初步实现 `fn2fm` 中除 `coefs` 以外的结构体属性输出

## 2023.6.12（曹绍祯）
- fnval完成，简单测试通过
- 修改合法样条结构体判断`src/splines_common.cpp/`下
  ```cpp
    bool is_legal_spline_structure(cont baltam::structure &s);
  ```
  现在支持 pp 和 B 两种格式
- 修改`Spline/Base_Spline.cpp`下的
  ```cpp
  double Base_Spline::operator()(double x) const
  {
      //...
      if (x < knot(-1) || x > knot(N))
        return 0;
      //...
  }
  ```
  部分，将小（大）于等于号改成小（大）于号。
  
  但是此函数的实现仍需讨论，这里在 `Base_Spline` 中存了很多零多项式，在求值时很不方便，若第一个结点是重结点，则这个点处的值一定返回0，这样的实现是否合理？（问题的来源为在  `fnval`函数，以`spapi`文档中第一个例子为例，`fnval(s,0)`应为2，但是在当前实现下为0）
## 2023.6.3（徐同杉）
- 新增 `src/Splines/SpapiSolver.h`，用于实现spapi函数（写的比较草率，非常可能有错，以后慢慢改...）
- 对 `src/Splines/Base_Spline.cpp` 中的 `double Base_Spline::d(int degree, double x) const` 函数进行了一点小修改，以适应spapi函数的编写
- 新增`spapi`第一种功能的实现，很多地方有待讨论

## 2023.5.30（叶景文）
- 完成`fnbrk`实现，新增`fnbrk`测试`test_fnbrk.m`

## 2023.5.26（杨钧尹）

- 在函数说明文档中提供了`spapi`的部分具体实现方式

## 2023.5.24（徐同杉）

- 完成`spmak`实现，<font color=red>***待测试***</font>
- 修改`spmak`说明文档，新增对节点重数过多时的描述

## 2023.5.23（杨钧尹）

- 对部分函数进行了单增排列和参数检查上的功能优化

## 2023.5.19（叶景文）

- 修改`Base_spline`，现在基样条函数支持重节点的情况
- 修改`bspline`在重节点情况输出与`matlab`一致
## 2023.5.18 会议记录
- `ba_obj` 中存放一个 `void *` 类型的指针，这种类型的指针可以被转化为任意类型的指针，用来指向其保存的数据的内存位置
  - 在构造 `ba_obj` 对象的时候传给其构造函数的指针尽量指向堆内存
  - 若要指向栈内存，对象的生命周期需要开发者自行维护。
  - 注：若使用栈内存构造一个 `ba_obj` 对象，在离开当前作用域之后该 `ba_obj` 对象仍被使用，则其内部指针指向的数据其实已经被释放，此时很可能会引发软件闪退问题
- `ba_obj` 的设计初衷是用来保存和传递矩阵的，不要对 `ba_obj` 对象写下 `get< int / double >()`
- `ba_obj` 的构造函数的参数中的指针参数也应该指向堆内存
- `ba_obj` 不需要指针参数的构造函数内部也是重新 `new` 了堆内存赋值给其保存的指针的
- 软件中的 `int,double` 被视为 $1 \times 1$ 的 `int,double` 矩阵，不存在C++中独立的 `int,double`
- `baltam::structure` 中保存一个 `std::map<std::string, ba_obj_ptr>` 作为存储数据的数据结构
  - 其 `map` 的 `value` 是指向 `ba_obj` 的智能指针
  - 在给 `baltam::structure` 使用 `set_field(key,value_ptr)` 函数添加字段 `key` 的时候，数据指针`value_ptr` 应指向堆内存

- 测试脚本的两个目的
  - 给不熟悉相关理论的用户一个快速上手的示例
  - 测试函数的功能
  
- 对于错误输入的处理并不一定要与MATLAB相同，可以更多的考虑相关理论的背景，报错与否一个重要的考量是用户友好
## 2023.5.16（叶景文）
- 新增`bspline`测试`test_bspline.m`

## 2023.5.15（曹绍祯）
- 将`ppmak,Spline.cpp`中对样条结构体赋值中的数据类型调整为`int`
- `fnder`函数debug成功，不再出现闪退情形，今后结构体的创建和生成时请注意
    - 样条结构体内的`dim,coefs,pieces`类型均设为`int`
    - 取结构体`s`的`"key"`元素时使用`auto X = s.get_field("key")->as_int()`或者其他`as_***()`函数，不要使用`auto X = s.get_field("key")->get<T>()`语句
    - 根据已有的结构体创建新的结构体时，新的结构体不能和已有结构体共享数据，要将已有的数据复制一份交给新的结构体，比如在`fnder`函数中求导时，导函数的节点与原函数的节点即使是相同的，也要将原函数的结点数组复制一份再赋给导函数：
    ```cpp
    auto d_breaks = S->get_field("breaks")->get<matrix<double>>();// 旧的结构体的结点数据
    auto d_breaksTemp = new matrix<double> (*d_breaks); // 结果不能与输入共享数据，否则闪退
    res->set_field("breaks",new ba_obj(ba_double_mat,d_breaksTemp));
    ```
- `fnder`函数仍未实现，闪退问题真的层出不绝，修改过后的`fnder`函数94行会出现闪退问题，这里`fnder_implement`想根据参数`degree`对输入的样条结构体`const baltam::structure &S`进行处理得到一个新的结构体，但是又遇到了闪退问题。。。


## 2023.5.14（徐同杉）
- 新增`spmak`函数说明
- 新增`spmak`函数实现(不完整)，具体细节有待进一步讨论。
- 修改`csapi,ppmak`测试 `test_csapi.m` 和 `test_ppmak.m`

## 2023.5.14（曹绍祯）
- 修改`fnbrk,spapi`函数说明中一些笔误，进行格式微调

## 2023.5.13（杨钧尹）
- 新增`spapi`函数说明

## 2023.5.12（叶景文）
- 新增`fnbrk`函数说明

## 2023.5.11（叶景文）
- 增加`fnder_case2`空实现,以防编译报错
- 修改了`bspline`函数的一些错误

## 2023.5.9（杨钧尹）
- 新增测试 `test_fnval.m` 和 `test_fnder.m`

## 2023.4.30（徐同杉）
- 修改 `ppmak` 参数检查方式和具体实现细节
- 更新了 `ppmak` 函数说明文档，新增对 `[pp1,pp2,...] = ppmak(Breaks,Coefs)` 的说明
- 新建文件夹 `test/in_baltam` ，用于存放北太天元中的测试脚本
- 新增测试 `test_csapi.m` 和 `test_ppmak.m`

## 2023.4.28（叶景文）
- 新增 `bspline` 函数实现，<font color=red>***待测试***</font>
- 修改 `Base_Spline.h`和`Spline.cpp`，使其不再支持模板，使得生成B样条基曲线时不需要输入阶数

## 2023.4.25（徐同杉）
- 对 `ppmak` 中的参数检查作出修改，统一使用 `spline_parameter_check_double()`
- 对 `ppmak` 新增函数 `[pp1,pp2,...] = ppmak(Breaks,Coefs)`， 具体细节须进一步讨论，说明文档暂未书写

## 2023.4.24（杨钧尹）
- 优化了函数说明文档，统一了表达方式和字体样式
- 更正了`fnder`和`ppmak`文档中的错误表述

## 2023.4.20（曹绍祯）
- 新增 `bspline` 函数说明文档
- 将 `spline_parameter_check_double()` 由函数转成宏定义，避免隐藏 bug，使用方式为
  ```cpp
  const_ba_obj_rawptr xTemp = in_args[0]; // 此赋值是必要的，否则 xTemp 为一个空指针，有隐藏 bug
  const_ba_obj_ptr pX_smart_ptr;
  spline_parameter_check_double(in_args[0], xTemp, pX_smart_ptr)
  // 第一个参数为输入的指针，第二个参数为临时指针，第三个参数为防止内存泄露的智能指针
  ```
- 将使用 `spline_parameter_check_double()` 函数的函数，如 `csapi,fnval` 做了对应修改

## 2023.4.12（会议记录）
- 介绍开发环境与插件的机制及开发规范，具体可见
  [C++ 开发规范](http://183.66.214.98:20005/numerical_computation/style_guide/-/wikis/C++-%E5%BC%80%E5%8F%91%E8%A7%84%E8%8C%83)
  [mac操作系统使用安装北太天元](https://www.bilibili.com/video/BV1SW4y1j71Y/?spm_id_from=333.999.0.0&vd_source=ee756967a7f488a76fc48a6117203f55)
- 内核的使用文档，具体可见 `spline/deps/core/share/doc/html`
- 报bug的方式：在gitlab中的议题中报
- 介绍源码的调试方法，文档可见 
  [CLion调试baltam依赖的动态库](http://183.66.214.98:20005/numerical_computation/style_guide/-/wikis/CLion%E8%B0%83%E8%AF%95baltam%E4%BE%9D%E8%B5%96%E7%9A%84%E5%8A%A8%E6%80%81%E5%BA%93)
- 讲解test测试脚本的书写和保留及函数说明文档的书写

## 2023.4.9（杨钧尹）

- 在`.gitignore`新增了对部分开发环境下冗余文件的忽略



## 2023.4.9（曹绍祯）
- 修改函数说明文档中一下小的笔误，对代码加上缩进
- 删除了杂乱文件`.DS_Store`
- 对 `fnder` 函数的说明进行修正
- 在 `src/splines_common.h(.cpp)` 中新增函数 
  ```cpp
  bool is_legal_spline_structure(cont baltam::structure &s);
  ```
  判断一个结构体是否为一个合法的结构体，可以在其他函数中减少代码重复
- 实现 `fnder` 遇到了导致软件闪退的 bug，<font color=red>***暂时实现是错误的，需要讨论解决完善***</font>

## 2023.4.7（杨钧尹）
- 新增 `fn2fm` 函数功能 `ns = fn2fm(s,form)`的使用文档
- 新增 `fnder` 函数功能 `ds = fnder(s,order)`的使用文档
- 注释了`Base_Spline.h`和`fitCurve.h`


## 2023.4.6（徐同杉）
- 新增 `ppmak` 函数说明
- 新增 `ppmak` 函数实现，<font color=red>***待测试***</font>

## 2023.4.5（叶景文）
- 注释了`spline.h`和`spline.cpp`


## 2023.4.1（曹绍祯）
- 新增 `fnval` 函数实现，目前只支持了三次 ppForm 样条且实现应该比较 naive，<font color=red>***待测试***</font>
  
- 修改 `spline/BasicHeadFile/Polynomial.h(.cpp)` 的多项式实现，以匹配 `fnval` 函数在读取一个结构体生成多项式时的操作，软件中输出的样条结构体中的系数是按照 $p_i(x)=a_3(x-x_i)^3 +a_2(x-x_i)^2 + a_1(x-x_i) +a_0$的形式而非 $p_i(x)=a_3x^3+a_2x^2+a_1x+a_0$ 的形式，因此新增以下多项式构造函数
    ```cpp 
    Polynomial(double x0,const std::vector<double> & coefs,bool ascend = true);
    ```
    来生成多项式 $p(x)=\sum_{k=0}^{n}a_k (x-x_0)^k$ `(ascend == true)`或多项式 $p(x)=\sum_{k=0}^n a_{n-k}(x-x_0)^k$ `(ascend == false)`。

- 在 `spline/Splines/Spline.h(.cpp)` 新增从从结构体构造样条的构造函数
    ```cpp
    Spline<3, SplineType::ppForm>::Spline(const baltam::structure &s);
    ```
    来配合 `fnval` 函数的实现。

- 在 `spline/src/` 中新增 `spline_common.cpp` 文件，定义检查一个输入参数是否为 double 矩阵或者是否可以转化为 double 矩阵的函数 `spline_parameter_check_double()` ，以此避免不同函数进行参数检查时的代码重复， `namespace baltam::spline` 中的函数进行此种参数检查时都应使用此函数

- `csape.cpp`函数<font color=red>***待重构（减少代码重复）***</font>
## 2023.4.1（叶景文）
- 新增 `csapi` 函数功能 `values = csapi(x,y,xx)`的使用文档





## 2023.4.1（曹绍祯）
- 修改`/spline/src/caspi/csapi.cpp`中`case1,case2`函数名为`csapi_case1,csapi_case2`，以防与其他类似实现的函数混淆
- 修改`/spline/src/caspi/csapi.cpp`中`case1,case2`的实现，避免代码重复，新增参数检查函数`csapi_parameter_check,csapi_construct_InterpCondition`来做重复性的工作
- 修改 `/spline/src/Splines/Spline.cpp` 中底层一阶pp样条`operator()`运算符实现，使其可延拓到定义域之外，BSpline形式的样条相关函数暂时还未开发，实现暂时不修改
  




## 2023.3.31（徐同杉）
- 修改 `/spline/src/Splines/Spline.cpp` 中底层三阶pp样条`operator()`运算符实现，使其可延拓到定义域之外，其它样条的 () 运算符<font color=red>***待修改***</font>
- 新增 `csapi` 函数功能 `values = csapi(x,y,xx)`，<font color=red>***待测试***</font>



## 2023.3.24（曹绍祯）
- 开会新增`/spline/src/helloword` 玩具程序
- 配置开发环境说明




## 2023.3.23（曹绍祯）
- 将开发日志放在根目录下，从 `/spline/document` 中删除
- 更新 `/spline/test` 中的文件，测试脚本均要在北太天元和 MATLAB 中分别测试
- 在 `/spline/src/BasicHeadFile` 中新增多项式插值功能
- 在 `/spline/src/csape` 中完善 `csape` 函数默认边界条件值的功能，<font color=red>***待测试***</font>




## 2023.3.22（曹绍祯）
- 新增 `fnval` 函数实现，高维样条仍不支持
- 新增 `fnval` 函数说明，但是缺少示例
- 修改函数说明文档源代码书写方式，暂时使用 `lstlisting` 宏包
