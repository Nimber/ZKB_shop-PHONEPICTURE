#include "objdetector.h"
IplImage* praw;

#define threshhold_mod9_3 0.91	//美年达
#define threshhold_mod10_3 0.91	//健怡可乐
#define threshhold_mod11_3 0.91	//雪碧
#define threshhold_mod12_3 0.98	//养乐多
#define threshhold_mod13_3 0.81	//QQ星
#define threshhold_mod14_3 0.92	//蒙牛纯牛奶

#define threshhold_mod9_4 100000000	//美年达
#define threshhold_mod10_4 15000000	//健怡可乐
#define threshhold_mod11_4 	11000000//雪碧
#define threshhold_mod12_4 	4500000//养乐多
#define threshhold_mod13_4  200000000	//QQ星
#define threshhold_mod14_4  200000000	//蒙牛纯牛奶

#define threshhold_mod9_5  0.70	//美年达
#define threshhold_mod10_5 0.70	//健怡可乐
#define threshhold_mod11_5 0.7	//雪碧
#define threshhold_mod12_5 0.4	//养乐多
#define threshhold_mod13_5 0.5	//QQ星
#define threshhold_mod14_5 0.5	//蒙牛纯牛奶

//H范围
//H范围
#define Y_max_H 45
#define Y_min_H 15
#define R_max_H 255//10
#define R_min_H 235//-10
#define G_max_H 140
#define G_min_H 45
#define B_max_H 170//160
#define B_min_H 140//150
#define C_max_H 180
#define C_min_H 0
//S范围
#define Y_max_S 255
#define Y_min_S 111//175
#define R_max_S 255
#define R_min_S 50//125
#define G_max_S 255
#define G_min_S 20//125
#define B_max_S 255
#define B_min_S 155
#define C_max_S 225

#define C_min_S 25
//V范围
#define Y_max_V 255
#define Y_min_V 111//180
#define R_max_V 255
#define R_min_V 50//80
#define G_max_V 225
#define G_min_V 20//80
#define B_max_V 225
#define B_min_V 40
#define C_max_V 225
#define C_min_V 80
//判断颜色是否存在的要求
#define THR_NUM_COLOR 1100
//在图像中会看到下面这样六个格子,我们只取最右边的四个格子
/*
        COL1     COL2       COL3
┌────────┬─────────┬──────────┐ ROW1
│        │    A    │     B    │
│        │         │          │
├────────┼─────────┼──────────┤ ROW2
│        │    C    │     D    │
│        │         │          │
└────────┴─────────┴──────────┙ ROW3
下面是这些数值的定义
*/
#define COL1 0
#define COL2 198
#define COL3 700
#define ROW1 133
#define ROW2 283
#define ROW3 510

void contourpt(vector<Point> contours, int*x, int*y);


char color_check(Mat img)
{
	double A[4];
	int index=0;
	Mat temp;
	cout << "yellow" << endl;
	A[0] = color_area(img, 'Y', &temp);
	cout << "red" << endl;
	A[1] = color_area(img, 'R', &temp);
	cout << "green" << endl;
	A[2] = color_area(img, 'G', &temp);
	cout << "blue" << endl;
	A[3] = color_area(img, 'B', &temp);
	for (int i = 0;i < 4;i++) {
		if (A[i] > A[index]) {
			index = i;
		}
	}
	switch (index)
	{
	case 0:cout << "yellow!"<< endl;return 'Y';break;
	case 1:cout << "red!" << endl;return 'R';break;
	case 2:cout << "green!" << endl;return 'G';break;
	case 3:cout << "blue!" << endl;return 'B';break;
	default:
		return-1;
		break;
	}
}

bool color_location(Mat img,char colo, int* x, int* y)
{
	vector<vector<Point>> contours;
	Mat thred;
	int max;
	int ptx, pty;
	color_area(img, colo, &thred);
	findContours(thred, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);		//寻找边缘
	max = 0;
	for (int i = 0; i < contours.size();i++) {
		if (contours[i].size()>contours[max].size()) {
			max = i;
		}
	}
	contourpt(contours[max], x, y);
	cout << *x << "," << *y << endl;
	
	return false;
}

double color_area(Mat img, char color, Mat *fini_img)
{
	int iLowH;
	int iHighH;

	int iLowS;
	int iHighS;

	int iLowV;
	int iHighV;

	switch (color) {
	case 'Y':iLowH = Y_min_H;iHighH = Y_max_H;iLowS = Y_min_S;iLowV = Y_min_V;iHighS = Y_max_S;iHighV = Y_max_V;break;
	case 'R':iLowH = R_min_H;iHighH = R_max_H;iLowS = R_min_S;iLowV = R_min_V;iHighS = R_max_S;iHighV = R_max_V;break;
	case 'G':iLowH = G_min_H;iHighH = G_max_H;iLowS = G_min_S;iLowV = G_min_V;iHighS = G_max_S;iHighV = G_max_V;break;
	case 'B':iLowH = B_min_H;iHighH = B_max_H;iLowS = B_min_S;iLowV = B_min_V;iHighS = B_max_S;iHighV = B_max_V;break;
	default:return -1;
	}

	Mat imgOriginal = img;

	Mat imgHSV;
	vector<Mat> hsvSplit;
	cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV_FULL); //Convert the captured frame from BGR to HSV  

												  //因为我们读取的是彩色图，直方图均衡化需要在HSV空间做  
	split(imgHSV, hsvSplit);
	equalizeHist(hsvSplit[2], hsvSplit[2]);
	merge(hsvSplit, imgHSV);
	Mat imgThresholded;

	inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image  
	//cvWaitKey(-1);
	//return white_size(imgThresholded);
	imshow("uu", imgThresholded);
																								  //开操作 (去除一些噪点)  
	Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
	morphologyEx(imgThresholded, imgThresholded, MORPH_OPEN, element);
	imshow("ld", imgThresholded);

	//闭操作 (连接一些连通域)  
	element = getStructuringElement(MORPH_RECT, Size(40, 40));
	morphologyEx(imgThresholded, imgThresholded, MORPH_CLOSE, element);
	imshow("iii", imgThresholded);
	cvWaitKey(-1);

	*fini_img = imgThresholded;
	imshow("ld", imgThresholded);
	if (white_size(imgThresholded) != 0) {
		vector<vector<Point>> contours;
		findContours(imgThresholded, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		vector<Point> approx;
		approxPolyDP(contours[0], approx, 5, true);
		imshow("Thresholded Image", imgThresholded); //show the thresholded image  
		drawContours(imgThresholded, contours, 0, (0, 0, 255));
		cvWaitKey(-1);
		return contourArea(approx);
		/*IplImage *cag;
		cag = &IplImage(imgThresholded);
		CvSeqReader reader;
		IplImage* cpy = cvCloneImage(cag);
		int i;
		CvMemStorage* storage = cvCreateMemStorage(0);
		CvSeq* squares=findSquares4(cag, storage);
		cvStartReadSeq(squares, &reader, 0);

		// read 4 sequence elements at a time (all vertices of a square)
		for (i = 0; i < squares->total; i += 4)
		{
			CvPoint pt[4], *rect = pt;
			int count = 4;

			// read 4 vertices
			CV_READ_SEQ_ELEM(pt[0], reader);
			CV_READ_SEQ_ELEM(pt[1], reader);
			CV_READ_SEQ_ELEM(pt[2], reader);
			CV_READ_SEQ_ELEM(pt[3], reader);

			cout << sqrt(double((pt[0].x - pt[1].x)*(pt[0].x - pt[1].x) + (pt[0].y - pt[1].y)*(pt[0].y - pt[1].y))) << "," << sqrt(double((pt[1].x - pt[2].x)*(pt[1].x - pt[2].x) + (pt[1].y - pt[2].y)*(pt[1].y - pt[2].y))) << endl;
		}*/
	}
	else {
		cvWaitKey(-1);
		return 0;
	}
}

float VideoBlurDetect(Mat img)
{
	IplImage datau(img);
	IplImage *data = &datau;
	int width = data->widthStep;
	int height = data->height;
	ushort* sobelTable = new ushort[width*height];
	memset(sobelTable, 0, width*height*sizeof(ushort));

	int i, j, mul;

	uchar* udata = (uchar*)data->imageData;
	for (i = 1, mul = i*width; i < height - 1; i++, mul += width)
		for (j = 1; j < width - 1; j++)

			sobelTable[mul + j] = abs(udata[mul + j - width - 1] + 2 * udata[mul + j - 1] + udata[mul + j - 1 + width] - \
				udata[mul + j + 1 - width] - 2 * udata[mul + j + 1] - udata[mul + j + width + 1]);

	for (i = 1, mul = i*width; i < height - 1; i++, mul += width)
		for (j = 1; j < width - 1; j++)
			if (sobelTable[mul + j] < 50 || sobelTable[mul + j] <= sobelTable[mul + j - 1] || \
				sobelTable[mul + j] <= sobelTable[mul + j + 1]) sobelTable[mul + j] = 0;

	int totLen = 0;
	int totCount = 1;

	uchar suddenThre = 50;
	uchar sameThre = 3;

	for (i = 1, mul = i*width; i < height - 1; i++, mul += width)
	{
		for (j = 1; j < width - 1; j++)
		{
			if (sobelTable[mul + j])
			{
				int   count = 0;
				uchar tmpThre = 5;
				uchar max = udata[mul + j] > udata[mul + j - 1] ? 0 : 1;

				for (int t = j; t > 0; t--)
				{
					count++;
					if (abs(udata[mul + t] - udata[mul + t - 1]) > suddenThre)
						break;

					if (max && udata[mul + t] > udata[mul + t - 1])
						break;

					if (!max && udata[mul + t] < udata[mul + t - 1])
						break;

					int tmp = 0;
					for (int s = t; s > 0; s--)
					{
						if (abs(udata[mul + t] - udata[mul + s]) < sameThre)
						{
							tmp++;
							if (tmp > tmpThre) break;
						}
						else break;
					}

					if (tmp > tmpThre) break;
				}

				max = udata[mul + j] > udata[mul + j + 1] ? 0 : 1;

				for (int t = j; t < width; t++)
				{
					count++;
					if (abs(udata[mul + t] - udata[mul + t + 1]) > suddenThre)
						break;

					if (max && udata[mul + t] > udata[mul + t + 1])
						break;

					if (!max && udata[mul + t] < udata[mul + t + 1])
						break;

					int tmp = 0;
					for (int s = t; s < width; s++)
					{
						if (abs(udata[mul + t] - udata[mul + s]) < sameThre)
						{
							tmp++;
							if (tmp > tmpThre) break;
						}
						else break;
					}

					if (tmp > tmpThre) break;
				}
				count--;

				totCount++;
				totLen += count;
			}
		}
	}

	float result = (float)totLen / totCount;
	delete[] sobelTable;
	return result;
}

int forcematch(int x[], int y[])
{
	int max_index[4] = {0,0,0,0};
	if (x[0]<COL2&&y[0]>ROW2) {
		//在A格子中
		max_index[0]++;
	}
	else if (x[0] > COL2&&y[0] > ROW2) {
		//在B格子中
		max_index[1]++;
	}
	else if (x[0] < COL2&&y[0] < ROW2) {
		//在C格子中
		max_index[2]++;
	}
	else if (x[0] > COL2&&y[0] < ROW2) {
		//在D格子中
		max_index[3]++;
	}
	if (x[1]<COL2&&y[1]>ROW2) {
		//在A格子中
		max_index[0]++;
	}
	else if (x[1] > COL2&&y[1] > ROW2) {
		//在B格子中
		max_index[1]++;
	}
	else if (x[1] < COL2&&y[1] < ROW2) {
		//在C格子中
		max_index[2]++;
	}
	else if (x[1] > COL2&&y[1] < ROW2) {
		//在D格子中
		max_index[3]++;
	}
	if (x[2]<COL2&&y[2]>ROW2) {
		//在A格子中
		max_index[0]++;
	}
	else if (x[2] > COL2&&y[2] > ROW2) {
		//在B格子中
		max_index[1]++;
	}
	else if (x[2] < COL2&&y[2] < ROW2) {
		//在C格子中
		max_index[2]++;
	}
	else if (x[2] > COL2&&y[2] < ROW2) {
		//在D格子中
		max_index[3]++;
	}
	if (x[3]<COL2&&y[3]>ROW2) {
		//在A格子中
		max_index[0]++;
	}
	else if (x[3] > COL2&&y[3] > ROW2) {
		//在B格子中
		max_index[1]++;
	}
	else if (x[3] < COL2&&y[3] < ROW2) {
		//在C格子中
		max_index[2]++;
	}
	else if (x[3] > COL2&&y[3] < ROW2) {
		//在D格子中
		max_index[3]++;
	}
	for (int i = 0;i < 4;i++) {
		if (max_index[i] >= 2) {
			return i;
		}
	}
	return -1;
}

int ichange(int ca)
{
	switch (ca)
	{
	case 0: return 2;
	case 1: return 0;
	case 2: return 3;
	case 3: return 1;
	default:
		break;
	}
	return -1;
}

int white_size(Mat image)
{
	int sum=0;
	MatConstIterator_<uchar> it_in = image.begin<uchar>();
	MatConstIterator_<uchar> itend_in = image.end<uchar>();
	while (it_in != itend_in)
	{
		if ((*it_in) == 255) {
			sum++;
		}
		it_in++;
	}
	cout << "white size: " << sum << endl;
	return sum;
}

char color_rect(Mat img)
{
	int iLowH;
	int iHighH;

	int iLowS;
	int iHighS;

	int iLowV;
	int iHighV;

	float Y, R, G, B;

	//检测黄色范围
	iLowH = Y_min_H;iHighH = Y_max_H;iLowS = Y_min_S;iLowV = Y_min_V;iHighS = Y_max_S;iHighV = Y_max_V;
	Y= color_rect_val(img, iLowH, iHighH, iLowS, iHighS, iLowV, iHighV);
	//检测红色范围
	iLowH = R_min_H;iHighH = R_max_H;iLowS = R_min_S;iLowV = R_min_V;iHighS = R_max_S;iHighV = R_max_V;
	R = color_rect_val(img, iLowH, iHighH, iLowS, iHighS,iLowV, iHighV);
	//检测绿色范围
	iLowH = G_min_H;iHighH = G_max_H;iLowS = G_min_S;iLowV = G_min_V;iHighS = G_max_S;iHighV = G_max_V;
	G = color_rect_val(img, iLowH, iHighH, iLowS, iHighS, iLowV, iHighV);
	//检测蓝色范围
	iLowH = B_min_H;iHighH = B_max_H;iLowS = B_min_S;iLowV = B_min_V;iHighS = B_max_S;iHighV = B_max_V;
	B = color_rect_val(img, iLowH, iHighH, iLowS, iHighS, iLowV, iHighV);
	if (Y >= R && Y >= G && Y >= B && Y>=THR_NUM_COLOR) {
		return 'Y';
	}
	if (R >= Y && R >= G && R >= B && R >= THR_NUM_COLOR) {
		return 'R';
	}
	if (G >= Y && G >= R && G >= B && G >= THR_NUM_COLOR) {
		return 'G';
	}
	if (B >= Y && B >= R && B >= G && B >= THR_NUM_COLOR) {
		return 'B';
	}
	return -1;
}

float color_rect_val(Mat img, int h_min, int h_max, int s_min, int s_max,int v_min, int v_max)
{
	Mat img_hsv;
	cvtColor(img, img_hsv, CV_BGR2HSV_FULL);
	imshow("hsv", img_hsv);

	/*//把H通道分为1个bin，把S通道等分为1bin
	int hbins = 1;
	int sbins = 1;
	int vbins = 1;
	int histSize[] = { hbins,sbins,vbins };

	//H的取值范围
	float hranges[] = { h_min,h_max };
	//S的取值范围
	float sranges[] = { s_min,s_max };//s_min,s_max };
	//V的取值范围
	float vranges[] = { v_min,v_max };//v_min,v_max };
	const float* ranges[] = { hranges,sranges,vranges };
	
	MatND hist;
	//根据图像第一个通道，第二通道和第三通道，计算二维直方图
	int channels[] = { 0,1,2 };
	calcHist(&img,1, channels, Mat(), hist, 3, histSize, ranges,true, false);*/
	Scalar lower(h_min, 0, 0); //s_min, v_min);
	Scalar upper(h_max, 255, 255);// s_max, v_max);

	Mat1b mask;
	inRange(img_hsv, lower, upper, mask);
	int cnz = countNonZero(mask);
	imshow("cbdueudeiue", mask);
	cvWaitKey(-1);
	//输出数据
	cout <<"["<< cnz <<"]"<< endl;
	return cnz;
}

void contourpt(vector<Point> contours, int*x, int*y)
{
	int sum_x = 0, sum_y = 0;
	for (int i = 0;i < contours.size();i++) {
		sum_x += contours[i].x;
		sum_y += contours[i].y;
	}
	*x = sum_x / contours.size();
	*y = sum_y / contours.size();
}

CvSeq* findSquares4(IplImage* img, CvMemStorage* storage)
{
	int thresh = 50;

	CvSeq* contours;
	int i, c, l, N = 11;
	CvSize sz = cvSize(img->width & -2, img->height & -2);
	IplImage* timg = cvCloneImage(img);
	IplImage* gray = cvCreateImage(sz, 8, 1);
	IplImage* pyr = cvCreateImage(cvSize(sz.width / 2, sz.height / 2), 8, 3);
	IplImage* tgray;
	CvSeq* result;
	double s, t;
	// 创建一个空序列用于存储轮廓角点
	CvSeq* squares = cvCreateSeq(0, sizeof(CvSeq), sizeof(CvPoint), storage);

	cvSetImageROI(timg, cvRect(0, 0, sz.width, sz.height));
	// 过滤噪音
	cvPyrDown(timg, pyr, 7);
	cvPyrUp(pyr, timg, 7);
	tgray = cvCreateImage(sz, 8, 1);

	// 红绿蓝3色分别尝试提取
	for (c = 0; c < 1; c++)
	{
		// 提取 the c-th color plane
		cvSetImageCOI(timg, c);
		cvCopy(timg, tgray, 0);

		// 尝试各种阈值提取得到的（N=11）
		for (l = 0; l < N; l++)
		{
			// apply Canny. Take the upper threshold from slider
			// Canny helps to catch squares with gradient shading  
			if (l == 0)
			{
				cvCanny(tgray, gray, 0, thresh, 5);
				//使用任意结构元素膨胀图像
				cvDilate(gray, gray, 0, 1);
			}
			else
			{
				// apply threshold if l!=0:
				cvThreshold(tgray, gray, (l + 1) * 255 / N, 255, CV_THRESH_BINARY);
			}

			// 找到所有轮廓并且存储在序列中
			cvFindContours(gray, storage, &contours, sizeof(CvContour),
				CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));

			// 遍历找到的每个轮廓contours
			while (contours)
			{
				//用指定精度逼近多边形曲线
				result = cvApproxPoly(contours, sizeof(CvContour), storage,
					CV_POLY_APPROX_DP, cvContourPerimeter(contours)*0.02, 0);


				if (result->total == 4 &&
					fabs(cvContourArea(result, CV_WHOLE_SEQ)) > 500 &&
					fabs(cvContourArea(result, CV_WHOLE_SEQ)) < 100000 &&
					cvCheckContourConvexity(result))
				{
					s = 0;

					for (i = 0; i < 5; i++)
					{
						// find minimum angle between joint edges (maximum of cosine)
						if (i >= 2)
						{
							t = fabs(angle(
								(CvPoint*)cvGetSeqElem(result, i),
								(CvPoint*)cvGetSeqElem(result, i - 2),
								(CvPoint*)cvGetSeqElem(result, i - 1)));
							s = s > t ? s : t;
						}
					}

					// if 余弦值 足够小，可以认定角度为90度直角
					//cos0.1=83度，能较好的趋近直角
					if (s < 0.1)
						for (i = 0; i < 4; i++)
							cvSeqPush(squares,
								(CvPoint*)cvGetSeqElem(result, i));
				}

				// 继续查找下一个轮廓
				contours = contours->h_next;
			}
		}
	}
	cvReleaseImage(&gray);
	cvReleaseImage(&pyr);
	cvReleaseImage(&tgray);
	cvReleaseImage(&timg);

	return squares;
}

void D_detector(Mat img, Mat img_modA[], char obj[])
{
	char result[3];
	int A, B, C, D;
	//识别养乐多//注释掉
	A = 0;
	B = 0;
	C = 0;
	D = 0;
	result[0] = match2(img_modA[12], img, 3,12);
	result[1] = match2(img_modA[12], img, 4,12);
	result[2] = match2(img_modA[12], img, 5,12);
	for (int i = 0; i < 3; i++) {
		if (result[i] == 'A') {
			A++;
		}
		else if (result[i] == 'B') {
			B++;
		}
		else if (result[i] == 'C') {
			C++;
		}
		else if (result[i] == 'D') {
			D++;
		}
	}
	if (A > 2) {
		obj[2] = 12;
	}
	else if (B > 2) {
		obj[0] = 12;
	}
	else if (C > 2) {
		obj[3] = 12;
	}
	else if (D > 2) {
		obj[1] = 12;
	}

	//识别QQ星
	A = 0;
	B = 0;
	C = 0;
	D = 0;
	result[0] = match2(img_modA[13], img, 3,13);
	result[1] = match2(img_modA[13], img, 4,13);
	result[2] = match2(img_modA[13], img, 5,13);
	for (int i = 0; i < 3; i++) {
		if (result[i] == 'A') {
			A++;
		}
		else if (result[i] == 'B') {
			B++;
		}
		else if (result[i] == 'C') {
			C++;
		}
		else if (result[i] == 'D') {
			D++;
		}
	}
	if (A > 2) {
		obj[2] = 13;
	}
	else if (B > 2) {
		obj[0] = 13;
	}
	else if (C > 2) {
		obj[3] = 13;
	}
	else if (D > 2) {
		obj[1] = 13;
	}
	
	//识别纯牛奶
	A = 0;
	B = 0;
	C = 0;
	D = 0;
	result[0] = match2(img_modA[14], img, 3,14);
	result[1] = match2(img_modA[14], img, 4,14);
	result[2] = match2(img_modA[14], img, 5,14);
	for (int i = 0; i < 3; i++) {
		if (result[i] == 'A') {
			A++;
		}
		else if (result[i] == 'B') {
			B++;
		}
		else if (result[i] == 'C') {
			C++;
		}
		else if (result[i] == 'D') {
			D++;
		}
	}
	if (A > 2) {
		obj[2] = 14;
	}
	else if (B > 2) {
		obj[0] = 14;
	}
	else if (C > 2) {
		obj[3] = 14;
	}
	else if (D > 2) {
		obj[1] = 14;
	}
}

void C_detector(Mat img, Mat img_modA[], int position, char obj[])
{
	char result[3];
	int A, B, C, D;
	if (position == 6) {
		//芬达与美年达
		A = 0;
		B = 0;
		C = 0;
		D = 0;
		result[0] = match2(img_modA[9], img, 3,9);
		result[1] = match2(img_modA[9], img, 4,9);
		result[2] = match2(img_modA[9], img, 5,9);
		for (int i = 0; i < 3; i++) {
			if (result[i] == 'A') {
				A++;
			}
			else if (result[i] == 'B') {
				B++;
			}
			else if (result[i] == 'C') {
				C++;
			}
			else if (result[i] == 'D') {
				D++;
			}
		}
		if (A > 2) {
			obj[2] = 9;
		}
		else if (B > 2) {
			obj[0] = 9;
		}
		else if (C > 2) {
			obj[3] = 9;
		}
		else if (D > 2) {
			obj[1] = 9;
		}
		return;
	}
	else if (position == 7) {
		//健怡可乐
		A = 0;
		B = 0;
		C = 0;
		D = 0;
		result[0] = match2(img_modA[10], img, 3,10);
		result[1] = match2(img_modA[10], img, 4,10);
		result[2] = match2(img_modA[10], img, 5,10);
		for (int i = 0; i < 3; i++) {
			if (result[i] == 'A') {
				A++;
			}
			else if (result[i] == 'B') {
				B++;
			}
			else if (result[i] == 'C') {
				C++;
			}
			else if (result[i] == 'D') {
				D++;
			}
		}
		if (A > 2) {
			obj[2] = 10;
		}
		else if (B > 2) {
			obj[0] = 10;
		}
		else if (C > 2) {
			obj[3] = 10;
		}
		else if (D > 2) {
			obj[1] = 10;
		}
		return;
	}
	else if (position == 8) {
		//雪碧
		A = 0;
		B = 0;
		C = 0;
		D = 0;
		result[0] = match2(img_modA[11], img, 3,11);
		result[1] = match2(img_modA[11], img, 4,11);
		result[2] = match2(img_modA[11], img, 5,11);
		for (int i = 0; i < 3; i++) {
			if (result[i] == 'A') {
				A++;
			}
			else if (result[i] == 'B') {
				B++;
			}
			else if (result[i] == 'C') {
				C++;
			}
			else if (result[i] == 'D') {
				D++;
			}
		}
		if (A > 2) {
			obj[2] = 11;
		}
		else if (B > 2) {
			obj[0] = 11;
		}
		else if (C > 2) {
			obj[3] = 11;
		}
		else if (D > 2) {
			obj[1] = 11;
		}
		return;
	}
}

void color_detect(Mat img, char color, char obj[], Mat *fini_img)
{
	bool state[] = { false,false,false,false };
	find_block(img, color, state, fini_img);
	for (int i = 0; i < 4; i++) {
		if (state[i] == true) {
			obj[i] = 1;
		}
		else {
			obj[i] = -1;
		}
	}
	return;
}

void color_detect(Mat img, char color, char obj[])
{
	bool state[] = { false,false,false,false };
	find_block(img, color, state);
	for (int i = 0; i < 4; i++) {
		if (state[i] == true) {
			obj[i] = 1;
		}
		else {
			obj[i] = -1;
		}
	}
	return;
}


int find_block(Mat imgOriginal, char color, bool state[], Mat *fini_img)
{
	int iLowH;
	int iHighH;

	int iLowS;
	int iHighS;

	int iLowV;
	int iHighV;

	switch (color) {
	case 'Y':iLowH = Y_min_H; iHighH = Y_max_H; iLowS = Y_min_S; iLowV = Y_min_V; iHighS = Y_max_S; iHighV = Y_max_V; break;
	case 'R':iLowH = R_min_H; iHighH = R_max_H; iLowS = R_min_S; iLowV = R_min_V; iHighS = R_max_S; iHighV = R_max_V; break;
	case 'G':iLowH = G_min_H; iHighH = G_max_H; iLowS = G_min_S; iLowV = G_min_V; iHighS = G_max_S; iHighV = G_max_V; break;
	case 'B':iLowH = B_min_H; iHighH = B_max_H; iLowS = B_min_S; iLowV = B_min_V; iHighS = B_max_S; iHighV = B_max_V; break;
	default:return-1;
	}


	Mat imgHSV;
	Mat imgThresholded, MORPH_OPEN_img, MORPH_RECT_img;

	vector<Mat> hsvSplit;
	cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV_FULL); //Convert the captured frame from BGR to HSV  

	split(imgHSV, hsvSplit);
	equalizeHist(hsvSplit[2], hsvSplit[2]);
	merge(hsvSplit, imgHSV);


	inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image  


																								  //闭操作 (连接一些连通域)  
	Mat element = getStructuringElement(MORPH_RECT, Size(15, 15));
	morphologyEx(imgThresholded, MORPH_OPEN_img, MORPH_CLOSE, element);


	//开操作 (去除一些噪点)  
	element = getStructuringElement(MORPH_RECT, Size(10, 10));
	morphologyEx(MORPH_OPEN_img, MORPH_RECT_img, MORPH_OPEN, element);



	//通过二值图像寻找轮廓及其质心。
	int Threshold_area = 1000;

	Mat cannyOutput;
	Mat imgGray = MORPH_RECT_img;

	Canny(imgGray, cannyOutput, 0, 50);


	vector< vector<Point> > contours;
	findContours(cannyOutput, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
	//cout << "size of contours:" << contours.size() << endl;
	if (contours.size() == 0) {
		return 0;
	}
	else {
		Mat area(1, contours.size(), CV_32FC1);
		vector<vector<Point> >::iterator itc = contours.begin();
		for (int i = 0; i<(int)contours.size(); i++)//将有符号的contours.size()强制转换位非符号的；
		{
			area.at<float>(i) = contourArea(contours[i]);

			if (area.at<float>(i)>Threshold_area) {
				Moments mom = moments(Mat(*itc));
				float pt[2] = { mom.m10 / mom.m00,mom.m01 / mom.m00 };
				if ((pt[0] < imgGray.cols / 2) && (pt[1] < imgGray.rows / 2))
					state[2] = true;
				if ((pt[0] < imgGray.cols / 2) && (pt[1] > imgGray.rows / 2) && (pt[1] < imgGray.rows))
					state[3] = true;
				if ((pt[0]<imgGray.cols) && (pt[0] > imgGray.cols / 2) && (pt[1] < imgGray.rows / 2))
					state[0] = true;
				if ((pt[0]<imgGray.cols) && (pt[0] > imgGray.cols / 2) && (pt[1] > imgGray.rows / 2) && (pt[1] < imgGray.rows))
					state[1] = true;

			}
			//cout << "area " << i << " =" << area.at<float>(i) << endl;
			itc++;
		}

	}

	Mat result=imgOriginal;
	drawContours(result, contours, -1, Scalar(0), 2);
	addWeighted(imgOriginal, 1, result, 0.5, 0, (*fini_img));

	//(*fini_img) = result;
	//find_block(Threshold_img, state);
}

int find_block(Mat imgOriginal, char color, bool state[])
{
	int iLowH;
	int iHighH;

	int iLowS;
	int iHighS;

	int iLowV;
	int iHighV;

	switch (color) {
	case 'Y':iLowH = Y_min_H; iHighH = Y_max_H; iLowS = Y_min_S; iLowV = Y_min_V; iHighS = Y_max_S; iHighV = Y_max_V; break;
	case 'R':iLowH = R_min_H; iHighH = R_max_H; iLowS = R_min_S; iLowV = R_min_V; iHighS = R_max_S; iHighV = R_max_V; break;
	case 'G':iLowH = G_min_H; iHighH = G_max_H; iLowS = G_min_S; iLowV = G_min_V; iHighS = G_max_S; iHighV = G_max_V; break;
	case 'B':iLowH = B_min_H; iHighH = B_max_H; iLowS = B_min_S; iLowV = B_min_V; iHighS = B_max_S; iHighV = B_max_V; break;
	default:return-1;
	}


	Mat imgHSV;
	Mat imgThresholded, MORPH_OPEN_img, MORPH_RECT_img;

	vector<Mat> hsvSplit;
	cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV_FULL); //Convert the captured frame from BGR to HSV  

	split(imgHSV, hsvSplit);
	equalizeHist(hsvSplit[2], hsvSplit[2]);
	merge(hsvSplit, imgHSV);


	inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image  


																								  //闭操作 (连接一些连通域)  
	Mat element = getStructuringElement(MORPH_RECT, Size(15, 15));
	morphologyEx(imgThresholded, MORPH_OPEN_img, MORPH_CLOSE, element);


	//开操作 (去除一些噪点)  
	element = getStructuringElement(MORPH_RECT, Size(10, 10));
	morphologyEx(MORPH_OPEN_img, MORPH_RECT_img, MORPH_OPEN, element);



	//通过二值图像寻找轮廓及其质心。
	int Threshold_area = 1000;

	Mat cannyOutput;
	Mat imgGray = MORPH_RECT_img;

	Canny(imgGray, cannyOutput, 0, 50);


	vector< vector<Point> > contours;
	findContours(cannyOutput, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
	//cout << "size of contours:" << contours.size() << endl;
	if (contours.size() == 0) {
		return 0;
	}
	else {
		Mat area(1, contours.size(), CV_32FC1);
		vector<vector<Point> >::iterator itc = contours.begin();
		for (int i = 0; i<(int)contours.size(); i++)//将有符号的contours.size()强制转换位非符号的；
		{
			area.at<float>(i) = contourArea(contours[i]);

			if (area.at<float>(i)>Threshold_area) {
				Moments mom = moments(Mat(*itc));
				float pt[2] = { mom.m10 / mom.m00,mom.m01 / mom.m00 };
				if ((pt[0] < imgGray.cols / 2) && (pt[1] < imgGray.rows / 2))
					state[2] = true;
				if ((pt[0] < imgGray.cols / 2) && (pt[1] > imgGray.rows / 2) && (pt[1] < imgGray.rows))
					state[3] = true;
				if ((pt[0]<imgGray.cols) && (pt[0] > imgGray.cols / 2) && (pt[1] < imgGray.rows / 2))
					state[0] = true;
				if ((pt[0]<imgGray.cols) && (pt[0] > imgGray.cols / 2) && (pt[1] > imgGray.rows / 2) && (pt[1] < imgGray.rows))
					state[1] = true;

			}
			//cout << "area " << i << " =" << area.at<float>(i) << endl;
			itc++;
		}

	}
	//find_block(Threshold_img, state);
}

char match2(Mat mod, Mat img, int method,int select)
{
	//【1】给局部变量初始化
	Mat srcImage;
	Mat g_srcImage = img;
	Mat g_templateImage = mod;
	Mat g_resultImage;
	double threshh;

	//中心点坐标
	int x, y;
	//结果
	char result = -1;
	g_srcImage.copyTo(srcImage);

	//【2】初始化用于结果输出的矩阵
	int resultImage_cols = g_srcImage.cols - g_templateImage.cols + 1;
	int resultImage_rows = g_srcImage.rows - g_templateImage.rows + 1;
	g_resultImage.create(resultImage_cols, resultImage_rows, CV_32FC1);

	//【3】进行匹配和标准化
	matchTemplate(g_srcImage, g_templateImage, g_resultImage, method);
	//normalize(g_resultImage, g_resultImage, 0, 1, NORM_MINMAX, -1, Mat());

	//【4】通过函数 minMaxLoc 定位最匹配的位置
	double minValue; double maxValue; Point minLocation; Point maxLocation;
	Point matchLocation;
	minMaxLoc(g_resultImage, &minValue, &maxValue, &minLocation, &maxLocation, Mat());

	//【5】只用方法3~5，不需要比较，数值越大匹配效果越好
	matchLocation = maxLocation;

	//【6】绘制出矩形，并显示最终结果，
	//测试用
	rectangle(srcImage, matchLocation, Point(matchLocation.x + g_templateImage.cols, matchLocation.y + g_templateImage.rows), Scalar(0, 0, 255), 2, 8, 0);
	rectangle(g_resultImage, matchLocation, Point(matchLocation.x + g_templateImage.cols, matchLocation.y + g_templateImage.rows), Scalar(0, 0, 255), 2, 8, 0);

	//测试用，看阈值
	cout << "threshold " << maxValue << endl;

	//选择阈值
	if (method == 3) {
		switch (select) {
		case 9:threshh = threshhold_mod9_3; break;
		case 10:threshh = threshhold_mod10_3; break;
		case 11:threshh = threshhold_mod11_3; break;
		case 12:threshh = threshhold_mod12_3; break;
		case 13:threshh = threshhold_mod13_3; break;
		case 14:threshh = threshhold_mod14_3; break;
		default:return result;
		}
	}
	else if (method == 4) {
		switch (select) {
		case 9:threshh = threshhold_mod9_4; break;
		case 10:threshh = threshhold_mod10_4; break;
		case 11:threshh = threshhold_mod11_4; break;
		case 12:threshh = threshhold_mod12_4; break;
		case 13:threshh = threshhold_mod13_4; break;
		case 14:threshh = threshhold_mod14_4; break;
		default:return result;
		}
	}
	else if (method == 5) {
		switch (select) {
		case 9:threshh = threshhold_mod9_5; break;
		case 10:threshh = threshhold_mod10_5; break;
		case 11:threshh = threshhold_mod11_5; break;
		case 12:threshh = threshhold_mod12_5; break;
		case 13:threshh = threshhold_mod13_5; break;
		case 14:threshh = threshhold_mod14_5; break;
		default:return result;
		}
	}
	else {
		threshh = -1;
	}
	//计算质心坐标
	if (maxValue > threshh) {
		x = matchLocation.x + g_templateImage.cols / 2;
		y = matchLocation.y + g_templateImage.rows / 2;
		cout << x << "," << y << endl;
		if (x<COL2&&y>ROW2) {
			//在A格子中
			result = 'C';
		}
		else if (x > COL2&&y > ROW2) {
			//在B格子中
			result = 'D';
		}
		else if (x < COL2&&y < ROW2) {
			//在C格子中
			result = 'A';
		}
		else if (x > COL2&&y < ROW2) {
			//在D格子中
			result = 'B';
		}
	}
	
	//Test
	imshow("Method", srcImage);
	imshow("Result", g_resultImage);
	return result;
}

double angle(CvPoint* pt1, CvPoint* pt2, CvPoint* pt0)
{
	double dx1 = pt1->x - pt0->x;
	double dy1 = pt1->y - pt0->y;
	double dx2 = pt2->x - pt0->x;
	double dy2 = pt2->y - pt0->y;
	return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

