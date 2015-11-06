// photo.cpp : 定义控制台应用程序的入口点。



#include"stdafx.h"
using namespace std;
//using namespace cv;




void checkLine(IplImage * img)
{
	IplImage *img2 = cvLoadImage("D:\\p2.jpg", CV_LOAD_IMAGE_ANYCOLOR);
	cvShowImage("22", img2);
	CvMemStorage *storage = cvCreateMemStorage(0);//0 表示内存采用默认大小  
	CvSeq * lines = NULL;
	cvSmooth(img, img, CV_BLUR, 2, 2, 6, 5);
	cvShowImage("videos", img);
	//cvErode(img, img, 0, 1);

	//cvDilate(img, img, 0, 1);
	cvShowImage("videos1", img);
	lines = cvHoughLines2(img, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI / 180, 100, 50, 50);
	double max_lineA = 0;
	CvPoint *max_line;
	for (int i = 0; i<lines->total; i++)
	{
		CvPoint * line = (CvPoint *)cvGetSeqElem(lines, i);

		double max_x = (line[0].x - line[1].x)*(line[0].x - line[1].x);
		double max_y = (line[0].y - line[1].y)*(line[0].y - line[1].y);
		//printf("%d %d\n", &max_x, max_y);
		double max_lineB = sqrt((max_x + max_y));
		//printf("%d \n", max_lineB);
		if (max_lineA<max_lineB)
		{
			max_lineA = max_lineB;
			//printf("%lf\n", &max_lineA);
			max_line = line;
		}

		//cvLine(img, max_line[0], max_line[1], CV_RGB(0, 255, 0), 1, CV_AA);
	}

	cvLine(img2, max_line[0], max_line[1], CV_RGB(0, 255, 255), 3, CV_AA);
	cvShowImage("video", img2);

}
void main()
{
	IplImage *img = cvLoadImage("D:\\p2.jpg", 0);

	//cvCvtColor(img, img, CV_BGR2HSV);
	//cvCvtColor(img, img, CV_BGR2GRAY);
	cvCanny(img, img, 100, 200, 3);
	cvNamedWindow("video", 1);

	//cvCvtColor(imga, imga, CV_GRAY2BGR);
	checkLine(img);


	cvWaitKey();

}


//通过求平均值降噪
//int main()
//{
//	CvCapture* capture=cvCaptureFromFile("media.avi");
//	IplImage* frame=  NULL;
//	IplImage * imgsum =NULL;
//	
//	int start=301;
//	int end=304;
//	cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, start);
//	
//	int count = start;
//	while( cvGrabFrame(capture) && count <= end )
//	{
//		frame = cvRetrieveFrame(capture);// 获取当前帧
//		if(imgsum==NULL){
//			imgsum=cvCreateImage(cvGetSize(frame),IPL_DEPTH_32F,3);
//			cvZero(imgsum);
//		}
//		cvAcc(frame,imgsum);
//
//		char testname[100];
//		sprintf(testname,"%s%d%s","image",count,".jpg");
//		cvShowImage(testname,frame);
//		cvSaveImage(testname,frame);
//		
//		count++;
//	}
//	IplImage * imgavg = cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,3);
//	cvConvertScale(imgsum,imgavg,1.0/4.0);
//	
//	cvShowImage("imageavg",imgavg);
//	cvSaveImage("imageavg_4.jpg",imgavg);
//	
//	cvWaitKey(0);
//	cvReleaseCapture(&capture);
//	return 0;
//}


////通过求平均二次曝光
//int main()
//{
//	IplImage* image1=  cvLoadImage("psu3.jpg");
//	IplImage* image2=  cvLoadImage("psu4.jpg");
//	
//	IplImage * imgsum =cvCreateImage(cvGetSize(image1),IPL_DEPTH_32F,3);
//	cvZero(imgsum);
//	cvAcc(image1,imgsum);
//	cvAcc(image2,imgsum);
//
//	IplImage * imgavg = cvCreateImage(cvGetSize(image1),IPL_DEPTH_8U,3);
//	cvConvertScale(imgsum,imgavg,1.0/2.0);
//
//	cvShowImage("imageavg",imgavg);
//	cvSaveImage("avg.jpg",imgavg);
//
//	cvWaitKey(0);
//	cvReleaseImage(&image1);
//	cvReleaseImage(&image2);
//	cvReleaseImage(&imgsum);
//	cvReleaseImage(&imgavg);
//	return 0;
//}

void FillInternalContours(IplImage *pBinary, double dAreaThre)
{
	double dConArea;
	CvSeq *pContour = NULL;
	CvSeq *pConInner = NULL;
	CvMemStorage *pStorage = NULL;
	// 执行条件   
	if (pBinary)
	{
		// 查找所有轮廓   
		pStorage = cvCreateMemStorage(0);
		cvFindContours(pBinary, pStorage, &pContour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
		// 填充所有轮廓   
		cvDrawContours(pBinary, pContour, CV_RGB(255, 255, 255), CV_RGB(255, 255, 255), 2, CV_FILLED, 8, cvPoint(0, 0));
		// 外轮廓循环   
		int wai = 0;
		int nei = 0;
		for (; pContour != NULL; pContour = pContour->h_next)
		{
			wai++;
			// 内轮廓循环   
			for (pConInner = pContour->v_next; pConInner != NULL; pConInner = pConInner->h_next)
			{
				nei++;
				// 内轮廓面积   
				dConArea = fabs(cvContourArea(pConInner, CV_WHOLE_SEQ));
				printf("%f\n", dConArea);
			}
			CvRect rect = cvBoundingRect(pContour, 0);
			cvRectangle(pBinary, cvPoint(rect.x, rect.y),
				cvPoint(rect.x + rect.width, rect.y + rect.height),
				CV_RGB(255, 255, 255), 1, 8, 0);
		}

		printf("wai = %d, nei = %d", wai, nei);
		cvReleaseMemStorage(&pStorage);
		pStorage = NULL;
	}
}
/*
//减去背景
int main()
{
IplImage* pFrame = NULL;
IplImage* pFrImg = NULL;
IplImage* pBkImg = NULL;

CvMat* pFrameMat = NULL;
CvMat* pFrMat = NULL;
CvMat* pBkMat = NULL;

CvCapture* pCapture = NULL;

int nFrmNum = 0;

//创建窗口
cvNamedWindow("video", 1);
cvNamedWindow("foreground", 1);

pCapture = cvCaptureFromFile("d:\\VID_20150916_090729.mp4");
while (pFrame = cvQueryFrame(pCapture))
{
nFrmNum++;

//如果是第一帧，需要申请内存，并初始化
if (nFrmNum == 1)
{
pBkImg = cvCreateImage(cvSize(pFrame->width, pFrame->height), IPL_DEPTH_8U, 1);
pFrImg = cvCreateImage(cvSize(pFrame->width, pFrame->height), IPL_DEPTH_8U, 1);

pBkMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
pFrMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
pFrameMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);

//转化成单通道图像再处理
cvCvtColor(pFrame, pBkImg, CV_BGR2GRAY);
cvCvtColor(pFrame, pFrImg, CV_BGR2GRAY);

cvConvert(pFrImg, pFrameMat);
cvConvert(pFrImg, pFrMat);
cvConvert(pFrImg, pBkMat);
}
else
{
cvCvtColor(pFrame, pFrImg, CV_BGR2GRAY);
cvConvert(pFrImg, pFrameMat);
//高斯滤波先，以平滑图像
cvSmooth(pFrameMat, pFrameMat, CV_GAUSSIAN, 3, 5, 5);



//当前帧跟背景图相减
cvAbsDiff(pFrameMat, pBkMat, pFrMat);

//二值化前景图
cvThreshold(pFrMat, pFrImg, 30, 255.0, CV_THRESH_BINARY);
//进行形态学滤波，去掉噪音
cvErode(pFrImg, pFrImg, 0, 6);
cvDilate(pFrImg, pFrImg, 0, 6);

//更新背景
cvRunningAvg(pFrameMat, pBkMat, 0.001, 0);
//将背景转化为图像格式，用以显示
cvConvert(pBkMat, pBkImg);

FillInternalContours(pFrImg, 300);
//显示图像
cvShowImage("video", pFrame);
cvShowImage("foreground", pFrImg);

//如果有按键事件，则跳出循环
//此等待也为cvShowImage函数提供时间完成显示
//等待时间可以根据CPU速度调整
if (cvWaitKey(1) >= 0)
break;


}

}
//销毁窗口
cvDestroyWindow("video");
cvDestroyWindow("foreground");

//释放图像和矩阵
cvReleaseImage(&pFrImg);
cvReleaseImage(&pBkImg);

cvReleaseMat(&pFrameMat);
cvReleaseMat(&pFrMat);
cvReleaseMat(&pBkMat);

cvReleaseCapture(&pCapture);

return 0;
}
*/


/*
#include "stdafx.h"
#include <opencv2\opencv.hpp>
#include <iostream>
#include <string>
#include "cxcore.h"
#include "cv.h"
#include "highgui.h"

using namespace cv;
using namespace std;

// 内轮廓填充
// 参数:
// 1. pBinary: 输入二值图像，单通道，位深IPL_DEPTH_8U。
// 2. dAreaThre: 面积阈值，当内轮廓面积小于等于dAreaThre时，进行填充。
void FillInternalContours(IplImage *pBinary, double dAreaThre)
{
double dConArea;
CvSeq *pContour = NULL;
CvSeq *pConInner = NULL;
CvMemStorage *pStorage = NULL;
// 执行条件
if (pBinary)
{
// 查找所有轮廓
pStorage = cvCreateMemStorage(0);
cvFindContours(pBinary, pStorage, &pContour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
// 填充所有轮廓
cvDrawContours(pBinary, pContour, CV_RGB(255, 255, 255), CV_RGB(255, 255, 255), 2, CV_FILLED, 8, cvPoint(0, 0));
// 外轮廓循环
int wai = 0;
int nei = 0;
for (; pContour != NULL; pContour = pContour->h_next)
{
wai++;
// 内轮廓循环
for (pConInner = pContour->v_next; pConInner != NULL; pConInner = pConInner->h_next)
{
nei++;
// 内轮廓面积
dConArea = fabs(cvContourArea(pConInner, CV_WHOLE_SEQ));
printf("%f\n", dConArea);
}
CvRect rect = cvBoundingRect(pContour, 0);
cvRectangle(pBinary, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height), CV_RGB(255, 255, 255), 1, 8, 0);
}

printf("wai = %d, nei = %d", wai, nei);
cvReleaseMemStorage(&pStorage);
pStorage = NULL;
}
}
int Otsu(IplImage* src)
{
int height = src->height;
int width = src->width;

//histogram
float histogram[256] = { 0 };
for (int i = 0; i < height; i++)
{
unsigned char* p = (unsigned char*)src->imageData + src->widthStep * i;
for (int j = 0; j < width; j++)
{
histogram[*p++]++;
}
}
//normalize histogram
int size = height * width;
for (int i = 0; i < 256; i++)
{
histogram[i] = histogram[i] / size;
}

//average pixel value
float avgValue = 0;
for (int i = 0; i < 256; i++)
{
avgValue += i * histogram[i];  //整幅图像的平均灰度
}

int threshold;
float maxVariance = 0;
float w = 0, u = 0;
for (int i = 0; i < 256; i++)
{
w += histogram[i];  //假设当前灰度i为阈值, 0~i 灰度的像素(假设像素值在此范围的像素叫做前景像素) 所占整幅图像的比例
u += i * histogram[i];  // 灰度i 之前的像素(0~i)的平均灰度值： 前景像素的平均灰度值

float t = avgValue * w - u;
float variance = t * t / (w * (1 - w));
if (variance > maxVariance)
{
maxVariance = variance;
threshold = i;
}
}

return threshold;
}

int main()
{
IplImage *img = cvLoadImage("D:\\15.jpg", 0);
IplImage *bin = cvCreateImage(cvGetSize(img), 8, 1);

int thresh = Otsu(img);
cvThreshold(img, bin, thresh, 255, CV_THRESH_BINARY);

FillInternalContours(bin, 200);

cvNamedWindow("img");
cvShowImage("img", img);

cvNamedWindow("result");
cvShowImage("result", bin);

cvWaitKey(-1);

cvReleaseImage(&img);
cvReleaseImage(&bin);

return 0;
}
*/

/*
int main()
{
//两幅图像相减得到不同的地方
Mat imgA = imread("D:\\15.jpg");


cvtColor(imgA, imgA, CV_BGR2GRAY);
Mat contours;
Canny(imgA, contours, 250, 350);
threshold(contours, contours, 128, 255, THRESH_BINARY);

imshow("result", contours);


Mat imgB = imread("D:\\db.jpg");//两幅图像的大小需要一致
Mat img_resultA, img_resultB;
subtract(imgA, imgB, img_resultA);
subtract(imgB, imgA, img_resultB);
add(img_resultA, img_resultB, img_resultA);


//对生成图像进行轮廓计算
cvtColor(imgA, imgA, CV_BGR2GRAY);
cvThreshold(imgA, imgA, f_thresh, 255, CV_THRESH_BINARY);
vector<vector<Point>> contours;
// find
findContours(imgA, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
// draw

Mat result(imgA.size(), CV_8U, Scalar(0));
drawContours(result, contours, -1, Scalar(255), 0);
//Rect r = boundingRect(Mat(contours[0]));
//rectangle(result, r, Scalar(255), 2);
namedWindow("contours");
imshow("contours", result);
//imshow("result", img_resultA);

waitKey();

return 0;
}
*/