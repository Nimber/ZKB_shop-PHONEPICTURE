#pragma once
#include <stdio.h>
#include <iostream>
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "opencv2/core/core.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <stdlib.h>
#include <ctype.h>
#include <string>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <opencv\cvaux.h>

using namespace cv;
using namespace std;

//不带位置检测的颜色识别
//返回颜色信息 'Y'-黄色 'R'-红色 'G'-绿色 'B'-蓝色 'C'-灰色
char color_check(Mat img);
//带位置检测的颜色识别
//颜色信息 'Y'-黄色 'R'-红色 'G'-绿色 'B'-蓝色 'C'-灰色
//返回位置信息
bool color_location(Mat img, char colo, int* x, int* y);
//内置函数,检测某种颜色的面积
//包括'Y'-黄色 'R'-红色 'G'-绿色 'B'-蓝色 'C'-灰色
//返回像素数
double color_area(Mat img, char color, Mat *fini_img);
//检测模糊程度的函数
//返回值为模糊程度的量化,一般我们觉得5.6以上就已经有点模糊了
float VideoBlurDetect(Mat data);
//强制判断程序
int forcematch(int x[], int y[]);
//转换格子类型
int ichange(int ca);
//计算白色的大小
int white_size(Mat image);
//直方图颜色判断入口程序
char color_rect(Mat img);
//直方图阈值计算工具
float color_rect_val(Mat img, int h_min, int h_max, int s_min, int s_max,int v_min, int v_max);
//angle函数用来返回（两个向量之间找到角度的余弦值）
double angle(CvPoint* pt1, CvPoint* pt2, CvPoint* pt0);
// 返回图像中找到的所有轮廓序列，并且序列存储在内存存储器中
CvSeq* findSquares4(IplImage* img, CvMemStorage* storage);
//drawSquares函数用来画出在图像中找到的所有正方形轮廓
//void drawSquares(IplImage* img, CvSeq* squares);
//D区专用的图像识别程序
void D_detector(Mat img, Mat img_modA[], char obj[]);
//C区专用的图像识别程序
void C_detector(Mat img, Mat img_modA[], int position, char obj[]);
//检测颜色的位置信息
void color_detect(Mat img, char color, char obj[],Mat *fini_img);
//颜色识别方块
int find_block(Mat imgOriginal, char color, bool state[],Mat *fini_img);

//检测颜色的位置信息
void color_detect(Mat img, char color, char obj[]);
//颜色识别方块
int find_block(Mat imgOriginal, char color, bool state[]);
//模板匹配程序
//输入mod-模板；img-待匹配；method-匹配模式，选择3~5
//输出物体位于A B C D
char match2(Mat mod,Mat img,int method, int select);