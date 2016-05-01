#include "objdetector.h"
#include <stdlib.h>
#include <strstream>

//检测模式匹配函数
int XXmain()
{
	Mat img1 = imread("D:/MOD/A/14.jpg", CV_LOAD_IMAGE_COLOR);
	Mat img2 = imread("C:/Users/NiSheng/Desktop/Newfolder(4)/113.jpg", CV_LOAD_IMAGE_COLOR);
	cout<<match2(img1, img2, 3,13)<<endl;
	cvWaitKey(-1);
	cout<<match2(img1, img2, 4,13)<<endl;
	cvWaitKey(-1);
	cout<<match2(img1, img2, 5,13)<<endl;
	cvWaitKey(-1);
	system("pause");
	return 0;
}

//检测有色木块
int imain()
{
	Mat img_1 = imread("D:/COLOR/4.jpg", CV_LOAD_IMAGE_COLOR);
	Mat img_2;
	cout << "Y," << color_area(img_1, 'Y', &img_2) << endl;
	cout << "R," << color_area(img_1, 'R', &img_2) << endl;
	cout << "G," << color_area(img_1, 'G', &img_2) << endl;
	cout << "B," << color_area(img_1, 'B', &img_2) << endl;
	system("pause");
	return 0;
}

int main()
{
	strstream ss;
	string path;
	Mat img= imread("C:/Users/NiSheng/Desktop/objrecog/0.jpg", CV_LOAD_IMAGE_COLOR);
	Mat Camera_Matrix;           // From calibration_data.xml
	Mat Distortion_Coefficients; // From calibration_data.xml
	Mat imageUndistorted;
	//读取设置
	FileStorage fs;
	fs.open("C:/Users/NiSheng/Desktop/camera_calib.xml", FileStorage::READ);
	fs["Camera_Matrix"] >> Camera_Matrix;
	fs["Distortion_Coefficients"] >> Distortion_Coefficients;
	fs.release();
	//处理畸变
	for (int i = 1; i < 572; i++) {
		ss << "C:/Users/NiSheng/Desktop/objrecog/" << i << ".jpg" << endl;
		ss >> path;
		img = imread(path, CV_LOAD_IMAGE_COLOR);
		undistort(img, imageUndistorted, Camera_Matrix, Distortion_Coefficients);
		ss << "C:/Users/NiSheng/Desktop/objrecog/und" << i << ".jpg" << endl;
		ss >> path;
		imwrite(path, imageUndistorted);
		cout << i << endl;
		imshow("orig", img);
		imshow("undist", imageUndistorted);
		cvWaitKey(-1);
	}
}