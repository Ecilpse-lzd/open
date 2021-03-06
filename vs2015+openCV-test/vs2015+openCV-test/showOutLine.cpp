#include"stdafx.h"

bool graFilterMid(IplImage* &image, int k)
{
	if (image == NULL)
	{
		printf("\nFilterMid没有传入图像");
		return false;
	}
	uchar *ImagePix = (uchar *)image->imageData;
	int m = (k - 1) / 2;
	for (int x = m; x<image->height - m; ++x)
	{
		for (int y = m; y<image->width - m; ++y)
		{
			uchar PixArray[100];
			int t = 0;
			for (int i = -m; i<m + 1; ++i)
			{
				for (int j = -m; j<m + 1; j++)
				{
					PixArray[t++] = ((uchar *)image->imageData)[(x + i)*image->widthStep + y + j];
				}
			}


			//冒泡排序
			for (int i = 0; i<k*k - 1; ++i)
				for (int j = 0; j<k*k - i - 1; ++j)
				{
					if (PixArray[j]>PixArray[j + 1])
					{
						uchar k = PixArray[j];
						PixArray[j] = PixArray[j + 1];
						PixArray[j + 1] = k;
					}
				}
			ImagePix[x*image->widthStep + y] = PixArray[(k*k - 1) / 2];
		}
	}
	return true;
}

void FillInternalContours(IplImage *pBinary, IplImage *pBack)
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
		int centerW, centerH;
		CvRect temporaryRect;
		double mexArea = 0;
		for (; pContour != NULL; pContour = pContour->h_next)
		{

			wai++;
			CvRect rect = cvBoundingRect(pContour, 0);

			dConArea = fabs(cvContourArea(pContour, CV_WHOLE_SEQ));
			if (mexArea<dConArea)
			{
				mexArea = dConArea;
				temporaryRect = rect;
			}
		}
		cvRectangle(pBack, cvPoint(temporaryRect.x, temporaryRect.y),
			cvPoint(temporaryRect.x + temporaryRect.width, temporaryRect.y + temporaryRect.height),
			CV_RGB(255, 0, 255), 4, 8, 0);

		centerW = temporaryRect.x + temporaryRect.width / 2;
		centerH = temporaryRect.y + temporaryRect.height / 2;
		printf("轮廓面积%f,中点坐标X : %d,Y : %d\n", mexArea, centerW, centerH);
		cvReleaseMemStorage(&pStorage);
		pStorage = NULL;
	}
}

//找到运动物体
void show()
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
	cvMoveWindow("video", 30, 0);
	cvMoveWindow("background", 360, 0);
	cvMoveWindow("foreground", 690, 0);

	//pCapture = cvCaptureFromFile("d:\\测试\\345.mp4");
		pCapture = cvCreateCameraCapture(1);
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
			cvWaitKey();
		}
		else
		{
			cvCvtColor(pFrame, pFrImg, CV_BGR2GRAY);
			graFilterMid(pFrImg, 1);

			//先做高斯滤波，以平滑图像  

			cvSmooth(pFrImg, pFrImg, CV_GAUSSIAN, 3, 0, 0);
			cvConvert(pFrImg, pFrameMat);
			//当前帧跟背景图相减  
			cvAbsDiff(pFrameMat, pBkMat, pFrMat);
			//二值化前景图  
			//use(pFrImg);
			//useMaxEntropy(pFrImg);
			cvThreshold(pFrMat, pFrImg, 80, 255.0, CV_THRESH_BINARY);
			////进行形态学滤波，去掉噪音
			cvErode(pFrImg, pFrImg, 0, 10);
			//cvDilate(pFrImg, pFrImg, 0, 7);

			//更新背景  
			//cvRunningAvg(pFrameMat, pBkMat, 0.003, 0);
			//将背景转化为图像格式，用以显示  
			cvConvert(pBkMat, pBkImg);
			pFrame->origin = IPL_ORIGIN_BL;
			pFrImg->origin = IPL_ORIGIN_BL;
			pBkImg->origin = IPL_ORIGIN_BL;

			FillInternalContours(pFrImg, pFrame);
			cvShowImage("video", pFrame);
			//cvShowImage("background", pBkImg);
			cvShowImage("foreground", pFrImg);
			//如果有按键事件，则跳出循环  
			//此等待也为cvShowImage函数提供时间完成显示  
			//等待时间可以根据CPU速度调整  
			if (cvWaitKey(10) >= 0)
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

}

void refineSegments(const Mat& img, Mat& mask, Mat& dst)
{
	int niters = 3;

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	int threshold_value = 0;
	int threshold_type = 3;;
	Mat temp;
	Mat temp1;

	dilate(mask, temp, Mat(), Point(-1, -1), niters);//膨胀，3*3的element，迭代次数为niters
	erode(temp, temp, Mat(), Point(-1, -1), niters * 3);//腐蚀
	dilate(temp, temp, Mat(), Point(-1, -1), niters);

	threshold(temp, temp1, threshold_value, 255, threshold_type);

	findContours(temp1, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);//找轮廓

	dst = Mat::zeros(img.size(), CV_8UC3);

	if (contours.size() == 0)
		return;

	// iterate through all the top-level contours,
	// draw each connected component with its own random color
	int idx = 0, largestComp = 0;
	double maxArea = 0;

	for (; idx >= 0; idx = hierarchy[idx][0])//这句没怎么看懂
	{
		const vector<Point>& c = contours[idx];
		double area = fabs(contourArea(Mat(c)));
		if (area > maxArea)
		{
			maxArea = area;
			largestComp = idx;//找出包含面积最大的轮廓
		}
	}
	Scalar color(0, 255, 0);
	drawContours(dst, contours, largestComp, color, CV_FILLED, 8, hierarchy);
}



void testMOG2()
{
	VideoCapture capture(0);
	
	Mat frame ,back, fgimg;
	int niters = 3;
	Ptr<BackgroundSubtractor> MOG2 = createBackgroundSubtractorMOG2();
	bool isupdate =true,isrun = false; 
	int frameNumber = 0;
	while (true)
	{
		++frameNumber;
		if (frameNumber>500 && !isrun)
		{
			cout << "学习结束";
			isupdate = false;
			isrun = true;
		}
		capture.read(frame);
		MOG2->apply(frame,back,isupdate?0.008:0);

		if(isrun)
		{
			refineSegments(frame, back, fgimg);
			imshow("sh", fgimg);
			MOG2->getBackgroundImage(back);
			imshow("sh1", back);
		}
		MOG2->getBackgroundImage(back);
		imshow("sh1", back);
		waitKey(5);
	}
}

int main()
{
	//ss();
	//gm();
	//dt_background();
	show();
	//testMOG2();
	return 0;
}