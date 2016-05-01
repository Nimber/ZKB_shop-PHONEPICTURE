//系统头文件
#include <windows.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <sstream>
#include <strstream>
#include <time.h>
//自定义头文件
#include "data.h"
#include "vision.h"
#include "myport.h"
#include "detector/detector/objdetector.h"

using namespace std;
using namespace cv;


#define TOTAL_OBJ 15					//所有需要抓取的项目之和
#define HAND_CONTROL_PORT 4 			//手臂部控制的arduino端口
#define BOTTOM_CONTROL_PORT 5			//底部控制的arduino端口
#define BO 9600							//波特率
#define CAMERA 0						//相机序号

SYSTEMTIME timestamp;

//任意两点间的路径规划函数
bool myrouter(int from_A, int from_B, int to_A, int to_B, port_com* com);

int main()
{	
	strstream ss;
	string path;
	Mat img = imread("C:/Users/NiSheng/Desktop/objrecog/0.jpg", CV_LOAD_IMAGE_COLOR);
	Mat Camera_Matrix;           // From calibration_data.xml
	Mat Distortion_Coefficients; // From calibration_data.xml
	Mat imageUndistorted;
	//读取设置
	FileStorage fs;
	fs.open("C:/Users/NiSheng/Desktop/camera_calib.xml", FileStorage::READ);
	fs["Camera_Matrix"] >> Camera_Matrix;
	fs["Distortion_Coefficients"] >> Distortion_Coefficients;
	fs.release();
	//初始化程序
	int finish = 0;
	port_com hand_com(HAND_CONTROL_PORT);
	port_com bott_com(BOTTOM_CONTROL_PORT);
	int ca;
	int timeout;
	cout << "请输入摄像头的序号:" << endl;
	cin >> ca;
	VideoCapture cap(ca);
	if (!cap.isOpened())
	{
		cout << "无法读取摄像头" << endl;
		return -1;
	}

	Mat frame;
	cap >> frame;
	cap >> frame;
	cap >> frame;
	cap >> frame;
	cap >> frame;
	cap >> frame;
	cap >> frame;
	cap >> frame;
	cap >> frame;
	cap >> frame;
	cap >> frame;
	cap >> frame;
	cap >> frame;
	cap >> frame;
	cap >> frame;
	int round_now = 0;						//当前已经拍摄的照片计数
	int temp_int;
	//在下面载入所有所需要的样板照片]
	stringstream ss;
	string path;
	//no photo run
	for (int h = 9; h < 12; h++) {
		ss << "D:\\MOD\\A" << h << ".jpg";
		ss >> path;
		obj_modA[h] = imread(path);
	}
	int current_A;
	int current_B;
	//存放了当前需要抓取的物品的obj_ID
	int current_obj;
	//存放了需要抓取的物品所在柜子的字符串信息
	string current_case_str;
	int case_h_l;
	//存放了当前物品的uni_ID
	int current_case;
	//临时存放位置信息
	int temp_A = 0, temp_B = 0;
	int temp_char_no;
	///////////////////////////////////////////////////////////////////////////
	//暂停一段时间,并等待下面完成初始化
	hand_com.read_start();
	bott_com.read_start();
	cout << "单片机准备完成" << endl;
	///////////////////////////////////////////////////////////////////////////
	//整场初始化
	hand_com.send_string("init", 20);		//进行初始化, 如果初始化失败就不继续下去
	bott_com.send_string("init");
	bott_com.check_ready();			//等待位置初始化完成
	hand_com.check_ready();			//等待手臂初始化完成
	current_A = 1;
	current_B = 2;
	cout << "上下位置初始化完成" << endl;
	Mat detect_img;
	///////////////////////////////////////////////////////////////////////////
	//A区
	//抓取至少4个黄色的方块
	int yellow_blk = 0;//指示已经完成抓取的黄色木块数
	char cur_obj[4];//存放对应物品的信息
	for (int rd = 0; rd < 3; rd++) {
		cur_obj[0] = -1;
		cur_obj[1] = -1;
		cur_obj[2] = -1;
		cur_obj[3] = -1;
		hand_com.send_int(dirtodegcc('A') + 100);
		myrouter(current_A, current_B, round_A[rd], round_B[rd], &bott_com);
		hand_com.check_ready();			//等待手臂初始化完成
		hand_com.send_int(5, 20);
		bott_com.check_ready();		//等待底部运动到
		current_A = round_A[rd];
		current_B = round_B[rd];
		hand_com.check_ready();			//等待手臂初始化完成
		//拍照片
		do {
			for (int i = 0; i < 1; i++) {
				GetLocalTime(&timestamp);
				//ss << timestamp.wHour << "l" << timestamp.wMinute<< "l" << timestamp.wSecond << ".jpg";
				static char  picname[100];
				sprintf(picname, "%d %d %d.jpg", timestamp.wHour, timestamp.wMinute, timestamp.wSecond);
				cout << picname;
				imwrite(picname, frame);
				imshow("picture", frame);
				cvWaitKey(1);
				//destroyWindow("picture");
			}
			cap >> frame;
		} while (VideoBlurDetect(frame) > 5.6);
		hand_com.send_int(8);
		undistort(frame, frame, Camera_Matrix, Distortion_Coefficients);
		color_detect(frame, 'Y', cur_obj,&detect_img);
		imshow("picture", detect_img);
		cvWaitKey(1);
		//destroyWindow("detect_img");
		hand_com.check_ready();
		for (int temp = 0; temp < 4; temp++) {
			if (cur_obj[temp] != -1) {
				current_case = 4 * rd + temp;
				current_case_str = getnamefromuniid(current_case);
				case_h_l = (strtoint(current_case_str) % 2 == 0) ? 2 : 4;
				temp_A = posi_A[current_case];
				temp_B = posi_B[current_case];
				myrouter(current_A, current_B, temp_A, temp_B, &bott_com);								//运行到目标位置
				hand_com.send_int(dirtodeg('A' + current_case / 12) + 100);										//旋转到目标位置
				bott_com.check_ready();															//等待底部运动到
				current_A = temp_A;																		//更新当前坐标
				current_B = temp_B;
				hand_com.check_ready();															//查看手部是否运动完成
				hand_com.send_int(case_h_l, 20);												//发送要抓取的物品的位置信息.如'4'
				hand_com.check_ready();															//查看手部是否运动完成
				temp_A = chart_A[1];															//更新放置处的坐标
				temp_B = chart_B[1];
				myrouter(current_A, current_B, temp_A, temp_B, &bott_com);								//运行到目标位置
																										//hand_com.send_int(chart_C[current_obj]+100);											//上面开始旋转
				bott_com.check_ready();															//等待底部运动到
				current_A = temp_A;																		//更新当前坐标
				current_B = temp_B;
				hand_com.send_int(3);																	//放
				hand_com.send_int(1);
				hand_com.check_ready();															//查看手部是否运动完成
				cout << "当前已完成物品" << finish << endl;
				yellow_blk++;
			}
		}
		if (yellow_blk >= 4) break;
	}
	///////////////////////////////////////////////////////////////////////////
	//D区
	//尽可能多地抓取物品
	int num_D_done = 0;//已经抓取的D区的物品数
	for (int rd = 0; rd < 3; rd++) {
		cur_obj[0] = -1;
		cur_obj[1] = -1;
		cur_obj[2] = -1;
		cur_obj[3] = -1;
		hand_com.send_int(dirtodegcc('D') + 100);
		myrouter(current_A, current_B, round_A[11 - rd], round_B[11 - rd], &bott_com);
		hand_com.check_ready();			//等待手臂初始化完成
		hand_com.send_int(5, 20);
		bott_com.check_ready();		//等待底部运动到
		hand_com.check_ready();			//等待手臂初始化完成
		current_A = round_A[11 - rd];
		current_B = round_B[11 - rd];
		//拍照片
		do {
			for (int i = 0; i < 1; i++) {
				GetLocalTime(&timestamp);
				//ss << timestamp.wHour << "l" << timestamp.wMinute<< "l" << timestamp.wSecond << ".jpg";
				static char  picname[100];
				sprintf(picname, "%d %d %d.jpg", timestamp.wHour, timestamp.wMinute, timestamp.wSecond);
				cout << picname;
				imwrite(picname, frame);
				imshow("picture", frame);
				cvWaitKey(1);
				//destroyWindow("picture");
			}
			cap >> frame;
		} while (VideoBlurDetect(frame) > 5.6);
		hand_com.send_int(8);
		timeout = 0;
		do {
			undistort(frame, frame, Camera_Matrix, Distortion_Coefficients);
			D_detector(frame, obj_modA, cur_obj);
			cap >> frame;
		} while (cur_obj[0] == -1 && cur_obj[1] == -1 && cur_obj[2] == -1 && cur_obj[3] == -1 && (timeout++) < 4);
		hand_com.check_ready();
		for (int temp = 0; temp < 4; temp++) {
			if (cur_obj[temp] != -1) {
				current_case = (11 - rd) * 4 + temp;
				cout << "current case:" << current_case << endl;
				current_case_str = getnamefromuniid(current_case);
				case_h_l = (strtoint(current_case_str) % 2 == 0) ? 2 : 4;
				temp_A = posi_A[current_case];
				temp_B = posi_B[current_case];
				myrouter(current_A, current_B, temp_A, temp_B, &bott_com);								//运行到目标位置
				hand_com.send_int(dirtodeg('A' + current_case / 12) + 100);										//旋转到目标位置
				bott_com.check_ready();															//等待底部运动到
				current_A = temp_A;																		//更新当前坐标
				current_B = temp_B;
				hand_com.check_ready();															//查看手部是否运动完成
				hand_com.send_int(case_h_l, 20);												//发送要抓取的物品的位置信息.如'4'
				hand_com.check_ready();															//查看手部是否运动完成
				temp_A = chart_A[cur_obj[temp]];															//更新放置处的坐标
				temp_B = chart_B[cur_obj[temp]];
				myrouter(current_A, current_B, temp_A, temp_B, &bott_com);								//运行到目标位置
																										//hand_com.send_int(chart_C[current_obj]+100);											//上面开始旋转
				bott_com.check_ready();															//等待底部运动到
				current_A = temp_A;																		//更新当前坐标
				current_B = temp_B;
				hand_com.send_int(3);																	//放
				if (cur_obj[temp] < 6) {
					temp_char_no = 1;
				}
				else {
					temp_char_no = cur_obj[temp] - 4;
				}
				hand_com.send_int(temp_char_no);
				hand_com.check_ready();															//查看手部是否运动完成
				cout << "当前已完成物品" << finish << endl;
				num_D_done++;
			}
		}
		if (num_D_done > 3) break;
	}

	///////////////////////////////////////////////////////////////////////////
	//C区
	//逆序抓取物品
	for (int rd = 0; rd < 3; rd++) {
		cur_obj[0] = -1;
		cur_obj[1] = -1;
		cur_obj[2] = -1;
		cur_obj[3] = -1;
		hand_com.send_int(dirtodegcc('C') + 100);
		myrouter(current_A, current_B, round_A[8 - rd], round_B[8 - rd], &bott_com);
		hand_com.check_ready();			//等待手臂初始化完成
	E2:		hand_com.send_int(5, 20);
		bott_com.check_ready();		//等待底部运动到
		current_A = round_A[8 - rd];
		current_B = round_B[8 - rd];
		hand_com.check_ready();			//等待手臂初始化完成
		//拍照片
		do {
			for (int i = 0; i < 1; i++) {
				GetLocalTime(&timestamp);
				//ss << timestamp.wHour << "l" << timestamp.wMinute<< "l" << timestamp.wSecond << ".jpg";
				static char  picname[100];
				sprintf(picname, "%d %d %d.jpg", timestamp.wHour, timestamp.wMinute, timestamp.wSecond);
				cout << picname;
				imwrite(picname, frame);
				imshow("picture", frame);
				cvWaitKey(1);
				//destroyWindow("picture");
			}
			cap >> frame;
		} while (VideoBlurDetect(frame) > 5.6);
		hand_com.send_int(8);
		timeout = 0;
		//识别物品
		do{
			undistort(frame, frame, Camera_Matrix, Distortion_Coefficients);
			C_detector(frame, obj_modA, 8 - rd, cur_obj);
			cap >> frame;
		} while (cur_obj[0] == -1 && cur_obj[1] == -1 && cur_obj[2] == -1 && cur_obj[3] == -1 && (timeout++) < 4);
		//放物品
		hand_com.check_ready();
		for (int temp = 0; temp < 4; temp++) {
			if (cur_obj[temp] != -1) {
				current_case = (8 - rd) * 4 + temp;
				current_case_str = getnamefromuniid(current_case);
				case_h_l = (strtoint(current_case_str) % 2 == 0) ? 2 : 4;
				temp_A = posi_A[current_case];
				temp_B = posi_B[current_case];
				myrouter(current_A, current_B, temp_A, temp_B, &bott_com);								//运行到目标位置
				hand_com.send_int(dirtodeg('A' + current_case / 12) + 100);										//旋转到目标位置
				bott_com.check_ready();															//等待底部运动到
				current_A = temp_A;																		//更新当前坐标
				current_B = temp_B;
				hand_com.check_ready();															//查看手部是否运动完成
				hand_com.send_int(case_h_l, 20);												//发送要抓取的物品的位置信息.如'4'
				hand_com.check_ready();															//查看手部是否运动完成
				temp_A = chart_A[cur_obj[temp]];															//更新放置处的坐标
				temp_B = chart_B[cur_obj[temp]];
				myrouter(current_A, current_B, temp_A, temp_B, &bott_com);								//运行到目标位置
																										//hand_com.send_int(chart_C[current_obj]+100);											//上面开始旋转
				bott_com.check_ready();															//等待底部运动到
				current_A = temp_A;																		//更新当前坐标
				current_B = temp_B;
				hand_com.send_int(3);																	//放
				if (cur_obj[temp] < 6) {
					temp_char_no = 1;
				}
				else {
					temp_char_no = cur_obj[temp] - 4;
				}
				hand_com.send_int(temp_char_no);
				hand_com.check_ready();															//查看手部是否运动完成
				cout << "当前已完成物品" << finish << endl;
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////
	//B区
	//抓取颜色木块
	//蓝色木块
	cur_obj[0] = -1;
	cur_obj[1] = -1;
	cur_obj[2] = -1;
	cur_obj[3] = -1;
	hand_com.send_int(dirtodegcc('B') + 100);
	myrouter(current_A, current_B, round_A[5], round_B[5], &bott_com);
	hand_com.check_ready();			//等待手臂初始化完成
	hand_com.send_int(5, 20);
	bott_com.check_ready();		//等待底部运动到
	current_A = round_A[5];
	current_B = round_B[5];
	hand_com.check_ready();			//等待手臂初始化完成
	//拍照片
	do {
		for (int i = 0; i < 1; i++) {
			GetLocalTime(&timestamp);
			//ss << timestamp.wHour << "l" << timestamp.wMinute<< "l" << timestamp.wSecond << ".jpg";
			static char  picname[100];
			sprintf(picname, "%d %d %d.jpg", timestamp.wHour, timestamp.wMinute, timestamp.wSecond);
			cout << picname;
			imwrite(picname, frame);
			imshow("picture", frame);
			cvWaitKey(1);
			//destroyWindow("picture");
		}
		cap >> frame;
	} while (VideoBlurDetect(frame) > 5.6);
	hand_com.send_int(8);
	undistort(frame, frame, Camera_Matrix, Distortion_Coefficients);
	color_detect(frame, 'B', cur_obj, &detect_img);
	imshow("picture", detect_img);
	hand_com.check_ready();

	//识别物品
	//color_detect(frame, 'B', cur_obj);
	//放物品
	for (int temp = 0; temp < 4; temp++) {
		if (cur_obj[temp] != -1) {
			current_case = 5 * 4 + temp;
			current_case_str = getnamefromuniid(current_case);
			case_h_l = (strtoint(current_case_str) % 2 == 0) ? 2 : 4;
			temp_A = posi_A[current_case];
			temp_B = posi_B[current_case];
			myrouter(current_A, current_B, temp_A, temp_B, &bott_com);								//运行到目标位置
			hand_com.send_int(dirtodeg('A' + current_case / 12) + 100);										//旋转到目标位置
			bott_com.check_ready();															//等待底部运动到
			current_A = temp_A;																		//更新当前坐标
			current_B = temp_B;
			hand_com.check_ready();															//查看手部是否运动完成
			hand_com.send_int(case_h_l, 20);												//发送要抓取的物品的位置信息.如'4'
			hand_com.check_ready();															//查看手部是否运动完成
			temp_A = chart_A[8];															//更新放置处的坐标
			temp_B = chart_B[8];
			myrouter(current_A, current_B, temp_A, temp_B, &bott_com);								//运行到目标位置
																									//hand_com.send_int(chart_C[current_obj]+100);											//上面开始旋转
			bott_com.check_ready();															//等待底部运动到
			current_A = temp_A;																		//更新当前坐标
			current_B = temp_B;
			hand_com.send_int(3);																	//放
			hand_com.send_int(4);
			hand_com.check_ready();															//查看手部是否运动完成
			cout << "当前已完成物品" << finish << endl;
		}
	}
	//绿色
	cur_obj[0] = -1;
	cur_obj[1] = -1;
	cur_obj[2] = -1;
	cur_obj[3] = -1;
	hand_com.send_int(dirtodegcc('B') + 100);
	myrouter(current_A, current_B, round_A[4], round_B[4], &bott_com);
	hand_com.check_ready();			//等待手臂初始化完成
	hand_com.send_int(5, 20);
	bott_com.check_ready();		//等待底部运动到
	current_A = round_A[4];
	current_B = round_B[4];
	hand_com.check_ready();			//等待手臂初始化完成
	//拍照片
	do {
		for (int i = 0; i < 1; i++) {
			GetLocalTime(&timestamp);
			//ss << timestamp.wHour << "l" << timestamp.wMinute<< "l" << timestamp.wSecond << ".jpg";
			static char  picname[100];
			sprintf(picname, "%d %d %d.jpg", timestamp.wHour, timestamp.wMinute, timestamp.wSecond);
			cout << picname;
			imwrite(picname, frame);
			imshow("picture", frame);
			cvWaitKey(1);
			//destroyWindow("picture");
		}
		cap >> frame;
	} while (VideoBlurDetect(frame) > 5.6);
	hand_com.send_int(8);
	//Mat detect_img;

	color_detect(frame, 'G', cur_obj, &detect_img);
	imshow("picture", detect_img);
	hand_com.check_ready();

	//识别物品
	undistort(frame, frame, Camera_Matrix, Distortion_Coefficients);
	color_detect(frame, 'G', cur_obj);
	//放物品
	for (int temp = 0; temp < 4; temp++) {
		if (cur_obj[temp] != -1) {
			current_case = 4 * 4 + temp;
			current_case_str = getnamefromuniid(current_case);
			case_h_l = (strtoint(current_case_str) % 2 == 0) ? 2 : 4;
			temp_A = posi_A[current_case];
			temp_B = posi_B[current_case];
			myrouter(current_A, current_B, temp_A, temp_B, &bott_com);								//运行到目标位置
			hand_com.send_int(dirtodeg('A' + current_case / 12) + 100);										//旋转到目标位置
			bott_com.check_ready();															//等待底部运动到
			current_A = temp_A;																		//更新当前坐标
			current_B = temp_B;
			hand_com.check_ready();															//查看手部是否运动完成
			hand_com.send_int(case_h_l, 20);												//发送要抓取的物品的位置信息.如'4'
			hand_com.check_ready();															//查看手部是否运动完成
			temp_A = chart_A[7];															//更新放置处的坐标
			temp_B = chart_B[7];
			myrouter(current_A, current_B, temp_A, temp_B, &bott_com);								//运行到目标位置
																									//hand_com.send_int(chart_C[current_obj]+100);											//上面开始旋转
			bott_com.check_ready();															//等待底部运动到
			current_A = temp_A;																		//更新当前坐标
			current_B = temp_B;
			hand_com.send_int(3);																	//放
			hand_com.send_int(3);
			hand_com.check_ready();															//查看手部是否运动完成
			cout << "当前已完成物品" << finish << endl;
		}
	}
	//红色
	cur_obj[0] = -1;
	cur_obj[1] = -1;
	cur_obj[2] = -1;
	cur_obj[3] = -1;
	hand_com.send_int(dirtodegcc('B') + 100);
	myrouter(current_A, current_B, round_A[3], round_B[3], &bott_com);
	hand_com.check_ready();			//等待手臂初始化完成
	hand_com.send_int(5, 20);
	bott_com.check_ready();		//等待底部运动到
	current_A = round_A[3];
	current_B = round_B[3];
	hand_com.check_ready();			//等待手臂初始化完成
									//拍照片
	do {
		for (int i = 0; i < 1; i++) {
			GetLocalTime(&timestamp);
			//ss << timestamp.wHour << "l" << timestamp.wMinute<< "l" << timestamp.wSecond << ".jpg";
			static char  picname[100];
			sprintf(picname, "%d %d %d.jpg", timestamp.wHour, timestamp.wMinute, timestamp.wSecond);
			cout << picname;
			imwrite(picname, frame);
			imshow("picture", frame);
			cvWaitKey(1);
			//destroyWindow("picture");
		}
		cap >> frame;
	} while (VideoBlurDetect(frame) > 5.6);
	hand_com.send_int(8);
	//Mat detect_img;
	color_detect(frame, 'R', cur_obj, &detect_img);
	imshow("picture", detect_img);
	hand_com.check_ready();
	//识别物品
	undistort(frame, frame, Camera_Matrix, Distortion_Coefficients);
	color_detect(frame, 'R', cur_obj);
	//放物品
	for (int temp = 0; temp < 4; temp++) {
		if (cur_obj[temp] != -1) {
			current_case = 3 * 4 + temp;
			current_case_str = getnamefromuniid(current_case);
			case_h_l = (strtoint(current_case_str) % 2 == 0) ? 2 : 4;
			temp_A = posi_A[current_case];
			temp_B = posi_B[current_case];
			myrouter(current_A, current_B, temp_A, temp_B, &bott_com);								//运行到目标位置
			hand_com.send_int(dirtodeg('A' + current_case / 12) + 100);										//旋转到目标位置
			bott_com.check_ready();															//等待底部运动到
			current_A = temp_A;																		//更新当前坐标
			current_B = temp_B;
			hand_com.check_ready();															//查看手部是否运动完成
			hand_com.send_int(case_h_l, 20);												//发送要抓取的物品的位置信息.如'4'
			hand_com.check_ready();															//查看手部是否运动完成
			temp_A = chart_A[6];															//更新放置处的坐标
			temp_B = chart_B[6];
			myrouter(current_A, current_B, temp_A, temp_B, &bott_com);								//运行到目标位置
																									//hand_com.send_int(chart_C[current_obj]+100);											//上面开始旋转
			bott_com.check_ready();															//等待底部运动到
			current_A = temp_A;																		//更新当前坐标
			current_B = temp_B;
			hand_com.send_int(3);																	//放
			hand_com.send_int(2);
			hand_com.check_ready();															//查看手部是否运动完成
			cout << "当前已完成物品" << finish << endl;
		}
	}
	///////////////////////////////////////////////////////////////////////////
	//A区
	//检查是否有遗漏的物品
	int rd = 0;
	while (yellow_blk<6) {
		cur_obj[0] = -1;
		cur_obj[1] = -1;
		cur_obj[2] = -1;
		cur_obj[3] = -1;
		hand_com.send_int(dirtodegcc('A') + 100);
		myrouter(current_A, current_B, round_A[2 - rd], round_B[2 - rd], &bott_com);
		hand_com.check_ready();			//等待手臂初始化完成
		hand_com.send_int(5, 20);
		bott_com.check_ready();		//等待底部运动到
		current_A = round_A[2-rd];
		current_B = round_B[2-rd];
		hand_com.check_ready();			//等待手臂初始化完成
		//拍照片
		do {
			for (int i = 0; i < 1; i++) {
				GetLocalTime(&timestamp);
				//ss << timestamp.wHour << "l" << timestamp.wMinute<< "l" << timestamp.wSecond << ".jpg";
				static char  picname[100];
				sprintf(picname, "%d %d %d.jpg", timestamp.wHour, timestamp.wMinute, timestamp.wSecond);
				cout << picname;
				imwrite(picname, frame);
				imshow("picture", frame);
				cvWaitKey(1);
				//destroyWindow("picture");
			}
			cap >> frame;
		} while (VideoBlurDetect(frame) > 5.6);
		hand_com.send_int(8);
		undistort(frame, frame, Camera_Matrix, Distortion_Coefficients);
		color_detect(frame, 'Y', cur_obj);
		hand_com.check_ready();
		for (int temp = 0; temp < 4; temp++) {
			if (cur_obj[temp] != -1) {
				current_case = 4 * (2 - rd) + temp;
				current_case_str = getnamefromuniid(current_case);
				case_h_l = (strtoint(current_case_str) % 2 == 0) ? 2 : 4;
				temp_A = posi_A[current_case];
				temp_B = posi_B[current_case];
				myrouter(current_A, current_B, temp_A, temp_B, &bott_com);								//运行到目标位置
				hand_com.send_int(dirtodeg('A' + current_case / 12) + 100);										//旋转到目标位置
				bott_com.check_ready();															//等待底部运动到
				current_A = temp_A;																		//更新当前坐标
				current_B = temp_B;
				hand_com.check_ready();															//查看手部是否运动完成
				hand_com.send_int(case_h_l, 20);												//发送要抓取的物品的位置信息.如'4'
				hand_com.check_ready();															//查看手部是否运动完成
				temp_A = chart_A[1];															//更新放置处的坐标
				temp_B = chart_B[1];
				myrouter(current_A, current_B, temp_A, temp_B, &bott_com);								//运行到目标位置
																										//hand_com.send_int(chart_C[current_obj]+100);											//上面开始旋转
				bott_com.check_ready();															//等待底部运动到
				current_A = temp_A;																		//更新当前坐标
				current_B = temp_B;
				hand_com.send_int(3);																	//放
				hand_com.send_int(1);
				hand_com.check_ready();															//查看手部是否运动完成
				cout << "当前已完成物品" << finish << endl;
				yellow_blk++;
			}
		}
	}
	hand_com.send_int(0);
	hand_com.check_ready();															//查看手部是否运动完成
	cout << "整个过程已完成" << endl;
	system("pause");
}

//任意两点间的路径规划函数
bool myrouter(int from_A, int from_B, int to_A, int to_B, port_com* com)
{
	cout << from_A << "," << from_B << "->" << to_A << "," << to_B << endl;
	int temp;
	//输入参数不正确
	if (from_A < 0 || from_B < 0 || from_A>9 || from_B>9) {
		cout << "Input error, please check!" << endl;
		return false;
	}
	//起点或是终点在不规范否位置
	if ((from_A > 2 && from_A < 7) && (from_B>2 && from_B < 7) || (to_A>2 && to_A < 7) && (to_B>2 && to_B < 7)) {
		cout << "Origin or destination is in the stuck!" << endl;
		return false;
	}
	//直走
	if (from_A == to_A || from_B == to_B) {
		com->send_string(assemble_point(to_A, to_B, 0));
		return true;
	}
	//小步走
	if ((from_A <= 2 && to_A <= 2) || (from_A >= 7 && to_A >= 7)) {
		com->send_string(assemble_point(from_A, to_B, 0));
		com->check_ready();
		com->send_string(assemble_point(to_A, to_B, 0));
		return true;
	}
	if ((from_B <= 2 && to_B <= 2) || (from_B >= 7 && to_B >= 7)) {
		com->send_string(assemble_point(to_A, from_B, 0));
		com->check_ready();
		com->send_string(assemble_point(to_A, to_B, 0));
		return true;
	}
	//跨区域走
	if (((to_A <= 2) || (to_A >= 7)) && ((from_B <= 2) || (from_B >= 7))) {
		com->send_string(assemble_point(to_A, from_B, 0));
		com->check_ready();
		com->send_string(assemble_point(to_A, to_B, 0));
		return true;
	}
	if (((to_B <= 2) || (to_B >= 7)) && ((from_A <= 2) || (from_A >= 7))) {
		com->send_string(assemble_point(from_A, to_B, 0));
		com->check_ready();
		com->send_string(assemble_point(to_A, to_B, 0));
		return true;
	}
	cout << "Cannot perform a step! Check input!" << endl;
	return false;
}