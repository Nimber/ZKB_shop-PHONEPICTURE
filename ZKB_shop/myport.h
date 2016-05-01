#pragma once
//该文件中包含了所有需要用于串口通讯的内容
//使用时请将以下文件放入对应的文件夹中
//所有的CPP和H
#include "mytools.h"
#include <stdlib.h>
#include <ctime>
#include "stdafx.h"
#include "SerialPort.h"
using namespace std;

//单一串口通讯
class port_com
{
public:
	port_com(int n);
	~port_com();
	//通过串口发送整型数值
	//输入：n 整型数字; timeout 为尝试的次数,尝试5次后会自动跳出程序,并且发送false
	//输出：布尔类型，true为发送成功，false为发送失败
	bool send_int(int n, int timeout=10);
	//通过串口发送字符串
	//输入：str 待发送的字符串; timeout 为尝试的次数, 默认尝试10次后会自动跳出程序,并且发送false
	//输出：布尔类型，true为发送成功，false为发送失败
	bool send_string(string str, int timeout = 10);
	//通过串口发送浮点数
	//输入: num浮点数类型数, timeout为尝试的次数, 默认尝试10次后会自动跳出程序,并且发送false
	//输出: 布尔类型,true为发送成功, false为发送失败
	//在arduino端需要使用parseFloat()进行读取, 发送的内容为浮点数
	//bool send_float(float num, int timeout = 10);
	//独占形式读串口整型
	//返回:num为读出的整型数 返回值为是否读取到 true为读取到 false为未读取到
	//独占方式是指在收到串口数据前,程序停留在这一行,每次读取完成后会将结果打印到命令行
	bool read_int(int *num);
	bool read_start();
	bool check_ready();
//private:
	CSerialPort mySerialPort;
	//带时间限制的读串口整型
	//num为输出的数, timeout为不停尝试的时间,返回值若为true则是正常读取,false为不正常读取
	bool timed_read_int(int *num, float timeout = 0.5f);
	//带时间限制的读串口整型
	//num为输出的数, length为要接受的字符串的长度, timeout为不停尝试的时间,返回值若为true则是正常读取,false为不正常读取
	bool timed_read_str(string *str, int length, float timeout = 0.5f);
	bool timed_read_all(string *str, float timeout = 0.5f);
	bool error_out_str_con();
};