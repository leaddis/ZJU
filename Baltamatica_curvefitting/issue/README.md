# 北太天元开发过程bug记录

## 2023.4.1
- 修改`csapi`时发现，大概率为对一个 const 指针进行`matrix_numerical_cast`操作时会引起软件闪退