# SimpleCompiler
简单的类C语言编译器，支持基本的C语言语法规则，经过了块内DAG优化，并最终生成Mips汇编指令

实现的语法规则简要概括如下：
+ 变量声明 int a;
+ 数组声明 int a[10][20];
+ 表达式运算 a = a + b;
+ 条件判断语句
+ while循环语句
+ 函数声明与调用
