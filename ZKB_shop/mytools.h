#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip> 
using namespace std;

//计算整型数字的位数
//输入: int n
//输出: n的位数, 负号将记为一位
//例如: count_int(1234)将会输出4
int count_int(int n);

//该程序将坐标转换为字符串形式
//输入: 坐标a,b,抓取方向(注意一定要大写!!!!!!!!!!!!!!)
//输出: 字符串aa;bb;flag
//例如: assemble_point(1, 3, 'A')输出"01;03;3"
string assemble_point(int a, int b, char direct);

//该程序将字符串转换成int
//输入:字符串形式数字
//输出:Int
int strtoint(string str);

//从字符串转为char数组
//输入: string为类型的字符串
//输出: char数组为类型的字符串
void strtochar(char ch[], string str);

//从char数组转为字符串
//输出: char数组为类型的字符串
//输入: string为类型的字符串
void chartostr(string *str, char ch[], int num);

//从浮点数转换为字符串
//////////////////////////////////////////没写完!
string floattostr(float num, int sci_cut);

//从方向判定机械臂转盘角度
int dirtodeg(char ch);

//获取柜子的uni_ID
//输入柜子编号如"A5"
//返回柜子的uni_ID
int getuniid(string str);

//获取柜子的名字
//只能输入从0~47的数,否则后果自负
string getnamefromuniid(int tt);

int dirtodegcc(char ch);