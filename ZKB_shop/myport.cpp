#include "myport.h"
#include "Windows.h"

port_com::port_com(int n)
{
	if (!mySerialPort.InitPort(n))
	{
		std::cout << "initPort fail !" << std::endl;
		system("pause");
	}
}

port_com::~port_com()
{
	
}

bool port_com::send_int(int n,int timeout)
{
	char data[12];				//存放需要发送的字符串
	int num = count_int(n);		//存放数字的长度
	_itoa_s(n, data, 10);			//将数字转换成字符串
	int temp, count=0;			//计数器
	bool state=false;			//发送状态指示
	while (state=(mySerialPort.WriteData((unsigned char*)data, num) == false) && count<timeout) {
		//cout << "send_int:" << ++count << "try in sending int: " << n << endl;
	}
	if (state == true) {
		cout << n<<":send_int:Error to send data due to cannot send data from PC! Please check your connection and port choice." << endl;
		cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
		return false;
	}	
	else {
		count = 0;
		do {
			if (!timed_read_int(&temp)) {
				//cout << "send_int:PC can not read port or time out. This is the " << ++count << " try. Total " << timeout << "." << endl;
			}
			else {
				if (temp == n) {
					cout <<n<< ":send_int:successfully send and receive data" << endl;
					return true;
				}
				else {
					cout <<n<< ":send_int:Verify error, destination may receive wrong data!" << endl;
					cout << "Data is " << temp << endl;
					string stemp;
					timed_read_all(&stemp);
					cout << "Maybe error is: " << temp << endl;
					error_out_str_con();
					cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
					return false;
				}
			}
		} while (count < timeout);
		cout << "send_int:Time out, destination have no respond to the data!" << endl;
		return false;
	}
}

bool port_com::send_string(std::string str,int timeout)
{
	int num=str.size();
	bool state;
	int count = 0;
	char ch[100];
	strtochar(ch, str);
	string temp;
	//下面是发送数据
	while (state = (mySerialPort.WriteData((unsigned char*)ch, num) == false) && count<timeout) {
		//cout <<"send_string:"<< ++count << "try in sending string: " << str << endl;
	}
	if (state == true) {
		cout << "send_string: Error to send data due to cannot send data from PC! Please check your connection and port choice." << endl;
		cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
		return false;
	}
	//下面是接受回执
	else {
		count = 0;
		while (count < timeout) {
			if (!timed_read_str(&temp, num)) {
				//cout << "send_string:PC can not read port or time out. This is the " << ++count << " try. Total " << timeout << "." << endl;
			}
			else {
				if (temp == str) {
					cout << "send_string:successfully send and receive data" << endl;
					return true;
				}
				else {
					cout << "send_string:Verify error, destination may receive wrong data!" << endl;
					cout << "Data is " << temp << endl;
					timed_read_all(&temp);
					cout << "Maybe error is: " << temp<<endl;
					error_out_str_con();
					cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
					return false;
				}
			}
		}
	}

}

bool port_com::read_int(int * num)
{
	int a;
	while(timed_read_int(&a)==false);
	*num = a;
	cout << a<<endl;
	return true;
}

bool port_com::read_start()
{
	string a;
again:	while (timed_read_str(&a,1) == false);
	if (a != "S") goto again;
	timed_read_str(&a, 4);
	if (a != "tart") goto again;
	return false;
}

bool port_com::check_ready()
{
	char temp;
wait_ready:	while (mySerialPort.GetBytesInCOM() == 0);
	while (mySerialPort.ReadChar(temp) == true) {
		if (temp >= '0'&&temp <= '9') {
			break;
		}
	}
	if (temp != '1') {
		goto wait_ready;
	}
	cout << "res ready" << endl;
	return false;
}

bool port_com::timed_read_int(int * num, float timeout)
{
	clock_t start, end;				//计时变量
	double dur;
	char temp;						//暂存读入的信息
	char ch[20];
	char* pt=ch;
	//下面等待数据到来
	start = clock();
	while (mySerialPort.GetBytesInCOM() == 0) {
		end = clock();
		dur = (double)(end - start);
		if (dur / CLOCKS_PER_SEC > timeout) {
			//cout << "Time out for timed_read_int" << endl;
			return false;
		}
	}
	//下面读取第一个整数
	while (mySerialPort.ReadChar(temp) == true) {
		if (temp >= '0'&&temp <= '9') {
			*pt = temp;
			pt++;
			break;
		}
	}
	while (mySerialPort.ReadChar(temp) == true) {
		if (temp >= '0'&&temp <= '9') {
			*pt = temp;
			pt++;
		}
		else {
			break;
		}
	}
	*num=strtoint(ch);
	return true;
}

bool port_com::timed_read_str(string *str, int length, float timeout)
{
	clock_t start, end;				//计时变量
	double dur;
	char temp;						//暂存读入的信息
	char ch[100];
	char* pt = ch;
	//下面等待数据到来
	start = clock();
	while (mySerialPort.GetBytesInCOM() == 0) {
		end = clock();
		dur = (double)(end - start);
		if (dur / CLOCKS_PER_SEC > timeout) {
			//cout << "Time out for timed_read_str" << endl;
			return false;
		}
	}
	//Sleep(200);
	for (int i = 0;i < length;i++) {
		mySerialPort.ReadChar(temp);
		if (temp == '\r' || temp == '\n') {
			i--;
			continue;
		}
		*pt = temp;
		pt++;
	}
	chartostr(str, ch, length);
	cout << *str;
	return true;
}

bool port_com::timed_read_all(string * str, float timeout)
{
	clock_t start, end;				//计时变量
	double dur;
	char temp;						//暂存读入的信息
	char ch[100];
	char* pt = ch;
	int i = 0;
	//下面等待数据到来
	start = clock();
	while (mySerialPort.GetBytesInCOM() == 0) {
		end = clock();
		dur = (double)(end - start);
		if (dur / CLOCKS_PER_SEC > timeout) {
			//cout << "Time out for timed_read_int" << endl;
			return false;
		}
	}
	//Sleep(200);
	while (mySerialPort.GetBytesInCOM() != 0) {
		mySerialPort.ReadChar(temp);
		*pt = temp;
		pt++;
	}
	chartostr(str, ch, 99);
	cout << *str;
	return true;
}

bool port_com::error_out_str_con()
{
	mySerialPort.OpenListenThread();
	return false;
}

