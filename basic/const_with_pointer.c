#include <stdio.h>

int main(int argc, char *argv[])
{
    int a=3;
    int b=10;
    /*定义指向const的指针（指针指向的内容不能被修改）*/
    const int *p1;
    int const *p2;

    /*定义const指针（由于指针本身的值不能改变所以必须初始化）*/
    int * const p3=&a;

    /*指针本身和它指向的内容都不能被改变，也要初始化*/
    const int * const p4=&a;
    int const * const p5=&b;

    p1=p2=&a;           //正确
    *p1=*p2=8;          //不正确（指针指向的内容不能被修改）

    *p3=5;              //正确
    p3=p1;              //不正确（指针本身的值不能被修改）

    p4=p5;              //error
    *p4=*p5=10;         //error

    return 0;
}
