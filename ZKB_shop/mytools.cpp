#include "mytools.h"

int count_int(int n)
{
	int a = 0;
	if (n < 0) {
		n = -n;
		a++;
	}
	if (n == 0) {
		return 1;
	}
	if (n > 0 && n < 10) {
		return a + 1;
	}
	if (n > 9 && n < 100) {
		return a + 2;
	}
	if (n > 99 && n < 1000) {
		return a + 3;
	}
	if (n > 999 && n < 10000) {
		return a + 4;
	}
	if (n > 9999 && n < 100000) {
		return a + 5;
	}
	if (n > 99999 && n < 1000000) {
		return a + 6;
	}
	if (n > 999999 && n < 10000000) {
		return a + 7;
	}
	if (n > 9999999 && n < 100000000) {
		return a + 8;
	}
	if (n > 99999999 && n < 1000000000) {
		return a + 9;
	}
	if (n > 999999999 && n < 10000000000) {
		return a + 10;
	}
	return 0;
}

string assemble_point(int a, int b, char direct)
{
	string data;
	stringstream ss;
	int dir = 0;
	switch (direct) {
	case 'A': dir = 3;break;
	case 'B': dir = 2;break;
	case 'C': dir = 4;break;
	case 'D': dir = 1;break;
	default:dir = 0;break;
	}
	ss << setfill('0');
	ss << setw(2) << a << ";" << setw(2) << b << ";" << dir;
	ss >> data;
	return data;
}

//�ó����ַ���ת����int
//����:�ַ�����ʽ����
//���:Int
int strtoint(string str)
{
	stringstream ss;
	int result;
	ss << str;
	ss >> result;
	return result;
}

//���ַ���תΪchar����
//����: stringΪ���͵��ַ���
//���: char����Ϊ���͵��ַ���
void strtochar(char ch[], string str)
{
	stringstream ss;
	ss << str;
	ss >> ch;
	return;
}

//��char����תΪ�ַ���
//����: char����Ϊ���͵��ַ���
//���: stringΪ���͵��ַ���
void chartostr(string *str, char ch[], int num)
{
	stringstream ss;
	for (int i = 0;i < num;i++) {
		ss << ch[i];
	}
	ss >> *str;
	return;
}

//�Ӹ�����ת��Ϊ�ַ���
//////////////////////////////////////////ûд��!
string floattostr(float num, int sci_cut)
{
	stringstream ss;
	string result;
	ss << num;
	ss >> result;
	return result;
}

//�ӷ����ж���е��ת�̽Ƕ�
int dirtodeg(char ch)
{
	int dir;
	switch (ch) {
	case 'A': dir = -90;break;
	case 'B': dir = 0;break;
	case 'C': dir = 90;break;
	case 'D': dir = 180;break;
	default:dir = 0;break;
	}
	return dir;
}

//��ȡ���ӵ�uni_ID
//������ӱ����"A5"
//���ع��ӵ�uni_ID
int getuniid(string str)
{
	if (str.size() > 3 || str.size() < 2)	return -1;
	else {
		const char *M;
		M = str.data();
		return (M[0] - 'A') * 12 + strtoint(str.substr(1)) - 1;
	}
}

//��ȡ���ӵ�����
//ֻ�������0~47����,�������Ը�
string getnamefromuniid(int tt)
{
	int dir = tt / 12;
	int remain = tt - 12 * dir + 1;
	char a;
	switch (dir) {
	case 0: a = 'A';break;
	case 1: a = 'B';break;
	case 2: a = 'C';break;
	case 3: a = 'D';break;
	default:a = 0;
	}
	stringstream ss;
	ss << remain;
	string result;
	ss >> result;
	return result;
}

int dirtodegcc(char ch)
{
	int dir;
	switch (ch) {
	case 'A': dir = 180; break;
	case 'B': dir = -90; break;
	case 'C': dir = 0; break;
	case 'D': dir = 90; break;
	default:dir = 0; break;
	}
	return dir;
}
