// photo.cpp : �������̨Ӧ�ó������ڵ㡣



#include"stdafx.h"
using namespace std;
//using namespace cv;




void checkLine(IplImage * img)
{
	IplImage *img2 = cvLoadImage("D:\\p2.jpg", CV_LOAD_IMAGE_ANYCOLOR);
	cvShowImage("22", img2);
	CvMemStorage *storage = cvCreateMemStorage(0);//0 ��ʾ�ڴ����Ĭ�ϴ�С  
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


//ͨ����ƽ��ֵ����
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
//		frame = cvRetrieveFrame(capture);// ��ȡ��ǰ֡
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


////ͨ����ƽ�������ع�
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
	// ִ������   
	if (pBinary)
	{
		// ������������   
		pStorage = cvCreateMemStorage(0);
		cvFindContours(pBinary, pStorage, &pContour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
		// �����������   
		cvDrawContours(pBinary, pContour, CV_RGB(255, 255, 255), CV_RGB(255, 255, 255), 2, CV_FILLED, 8, cvPoint(0, 0));
		// ������ѭ��   
		int wai = 0;
		int nei = 0;
		for (; pContour != NULL; pContour = pContour->h_next)
		{
			wai++;
			// ������ѭ��   
			for (pConInner = pContour->v_next; pConInner != NULL; pConInner = pConInner->h_next)
			{
				nei++;
				// ���������   
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
//��ȥ����
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

//��������
cvNamedWindow("video", 1);
cvNamedWindow("foreground", 1);

pCapture = cvCaptureFromFile("d:\\VID_20150916_090729.mp4");
while (pFrame = cvQueryFrame(pCapture))
{
nFrmNum++;

//����ǵ�һ֡����Ҫ�����ڴ棬����ʼ��
if (nFrmNum == 1)
{
pBkImg = cvCreateImage(cvSize(pFrame->width, pFrame->height), IPL_DEPTH_8U, 1);
pFrImg = cvCreateImage(cvSize(pFrame->width, pFrame->height), IPL_DEPTH_8U, 1);

pBkMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
pFrMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);
pFrameMat = cvCreateMat(pFrame->height, pFrame->width, CV_32FC1);

//ת���ɵ�ͨ��ͼ���ٴ���
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
//��˹�˲��ȣ���ƽ��ͼ��
cvSmooth(pFrameMat, pFrameMat, CV_GAUSSIAN, 3, 5, 5);



//��ǰ֡������ͼ���
cvAbsDiff(pFrameMat, pBkMat, pFrMat);

//��ֵ��ǰ��ͼ
cvThreshold(pFrMat, pFrImg, 30, 255.0, CV_THRESH_BINARY);
//������̬ѧ�˲���ȥ������
cvErode(pFrImg, pFrImg, 0, 6);
cvDilate(pFrImg, pFrImg, 0, 6);

//���±���
cvRunningAvg(pFrameMat, pBkMat, 0.001, 0);
//������ת��Ϊͼ���ʽ��������ʾ
cvConvert(pBkMat, pBkImg);

FillInternalContours(pFrImg, 300);
//��ʾͼ��
cvShowImage("video", pFrame);
cvShowImage("foreground", pFrImg);

//����а����¼���������ѭ��
//�˵ȴ�ҲΪcvShowImage�����ṩʱ�������ʾ
//�ȴ�ʱ����Ը���CPU�ٶȵ���
if (cvWaitKey(1) >= 0)
break;


}

}
//���ٴ���
cvDestroyWindow("video");
cvDestroyWindow("foreground");

//�ͷ�ͼ��;���
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

// ���������
// ����:
// 1. pBinary: �����ֵͼ�񣬵�ͨ����λ��IPL_DEPTH_8U��
// 2. dAreaThre: �����ֵ�������������С�ڵ���dAreaThreʱ��������䡣
void FillInternalContours(IplImage *pBinary, double dAreaThre)
{
double dConArea;
CvSeq *pContour = NULL;
CvSeq *pConInner = NULL;
CvMemStorage *pStorage = NULL;
// ִ������
if (pBinary)
{
// ������������
pStorage = cvCreateMemStorage(0);
cvFindContours(pBinary, pStorage, &pContour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
// �����������
cvDrawContours(pBinary, pContour, CV_RGB(255, 255, 255), CV_RGB(255, 255, 255), 2, CV_FILLED, 8, cvPoint(0, 0));
// ������ѭ��
int wai = 0;
int nei = 0;
for (; pContour != NULL; pContour = pContour->h_next)
{
wai++;
// ������ѭ��
for (pConInner = pContour->v_next; pConInner != NULL; pConInner = pConInner->h_next)
{
nei++;
// ���������
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
avgValue += i * histogram[i];  //����ͼ���ƽ���Ҷ�
}

int threshold;
float maxVariance = 0;
float w = 0, u = 0;
for (int i = 0; i < 256; i++)
{
w += histogram[i];  //���赱ǰ�Ҷ�iΪ��ֵ, 0~i �Ҷȵ�����(��������ֵ�ڴ˷�Χ�����ؽ���ǰ������) ��ռ����ͼ��ı���
u += i * histogram[i];  // �Ҷ�i ֮ǰ������(0~i)��ƽ���Ҷ�ֵ�� ǰ�����ص�ƽ���Ҷ�ֵ

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
//����ͼ������õ���ͬ�ĵط�
Mat imgA = imread("D:\\15.jpg");


cvtColor(imgA, imgA, CV_BGR2GRAY);
Mat contours;
Canny(imgA, contours, 250, 350);
threshold(contours, contours, 128, 255, THRESH_BINARY);

imshow("result", contours);


Mat imgB = imread("D:\\db.jpg");//����ͼ��Ĵ�С��Ҫһ��
Mat img_resultA, img_resultB;
subtract(imgA, imgB, img_resultA);
subtract(imgB, imgA, img_resultB);
add(img_resultA, img_resultB, img_resultA);


//������ͼ�������������
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