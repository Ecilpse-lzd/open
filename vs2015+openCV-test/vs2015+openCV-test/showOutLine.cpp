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
		int centerW,centerH;
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
		cvRectangle(pBinary, cvPoint(temporaryRect.x, temporaryRect.y),
			cvPoint(temporaryRect.x + temporaryRect.width, temporaryRect.y + temporaryRect.height),
			CV_RGB(255, 255, 255), 1, 8, 0);
		printf("轮廓面积%f\n", mexArea);
		
		cvReleaseMemStorage(&pStorage);
		pStorage = NULL;
	}
}


/*======================================================================*/
 /* OTSU global thresholding routine */
 /*======================================================================*/
int otsu2(IplImage *image)
{
	int w = image->width;
	int h = image->height;

	unsigned char*np; // 图像指针  
	unsigned char pixel;
	int thresholdValue = 1; // 阈值  
	int ihist[256]; // 图像直方图，256个点  

	int i, j, k; // various counters  
	int n, n1, n2, gmin, gmax;
	double m1, m2, sum, csum, fmax, sb;

	// 对直方图置零...  
	memset(ihist, 0, sizeof(ihist));

	gmin = 255; gmax = 0;
	// 生成直方图  
	for (i = 0; i < h; i++)
	{
		np = (unsigned char*)(image->imageData + image->widthStep*i);
		for (j = 0; j < w; j++)
		{
			pixel = np[j];
			ihist[pixel]++;
			if (pixel > gmax) gmax = pixel;
			if (pixel < gmin) gmin = pixel;
		}
	}

	// set up everything  
	sum = csum = 0.0;
	n = 0;

	for (k = 0; k <= 255; k++)
	{
		sum += k * ihist[k]; /* x*f(x) 质量矩*/
		n += ihist[k]; /* f(x) 质量 */
	}

	if (!n)
	{
		// if n has no value, there is problems...  
		//fprintf (stderr, "NOT NORMAL thresholdValue = 160\n");  
		thresholdValue = 160;
		goto L;
	}

	// do the otsu global thresholding method  
	fmax = -1.0;
	n1 = 0;
	for (k = 0; k <255; k++)
	{
		n1 += ihist[k];
		if (!n1) { continue; }
		n2 = n - n1;
		if (n2 == 0) { break; }
		csum += k *ihist[k];
		m1 = csum / n1;
		m2 = (sum - csum) / n2;
		sb = n1 * n2 *(m1 - m2) * (m1 - m2);
		/* bbg: note: can be optimized. */
		if (sb > fmax)
		{
			fmax = sb;
			thresholdValue = k;
		}
	}

L:
	for (i = 0; i < h; i++)
	{
		np = (unsigned char*)(image->imageData + image->widthStep*i);
		for (j = 0; j < w; j++)
		{
			if (np[j] >= thresholdValue)
				np[j] = 255;
			else np[j] = 0;
		}
	}

	//cout<<"The Threshold of this Image in Otsu is:"<<thresholdValue<<endl;  
	return(thresholdValue);
}

void useOtsu2(IplImage *imgOtsu)
{
	
	int thre2;
	 thre2 = otsu2(imgOtsu);
	 cout << "The Threshold of this Image in Otsu is:" << thre2 << endl;//输出显示阀值  
	 cvThreshold(imgOtsu, imgOtsu, thre2, 255, CV_THRESH_BINARY); // 二值化   
	 cvNamedWindow("imgOtsu", CV_WINDOW_AUTOSIZE);
	 //cvShowImage("imgOtsu", imgOtsu);//显示图像  
}

// 计算当前位置的能量熵  
int HistogramBins = 256;
float HistogramRange1[2] = { 0,255 };
float *HistogramRange[1] = { &HistogramRange1[0] };
typedef
enum { back, object } entropy_state;



double caculateCurrentEntropy(CvHistogram * Histogram1, int cur_threshold, entropy_state state)
{
	int start, end;
	if (state == back) {
		start = 0; end = cur_threshold;
	}
	else {
		start = cur_threshold; end = 256;
	}
	int  total = 0;
	for (int i = start; i<end; i++) {
		total += (int)cvQueryHistValue_1D(Histogram1, i);
	}
	double cur_entropy = 0.0;
	for (int i = start; i<end; i++) {
		if ((int)cvQueryHistValue_1D(Histogram1, i) == 0)
			continue;
		double percentage = cvQueryHistValue_1D(Histogram1, i) / total;
		cur_entropy += -percentage*logf(percentage);
	}
	return cur_entropy;
}
void  MaxEntropy(IplImage *src, IplImage *dst)
{
	assert(src != NULL);
	assert(src->depth == 8 && dst->depth == 8);
	assert(src->nChannels == 1);
	CvHistogram * hist = cvCreateHist(1, &HistogramBins, CV_HIST_ARRAY, HistogramRange);
	cvCalcHist(&src, hist);
	double maxentropy = -1.0;
	int max_index = -1;
	for (int i = 0; i<HistogramBins; i++) {
		double cur_entropy =
			caculateCurrentEntropy(hist, i, object) + caculateCurrentEntropy(hist, i, back);
		if (cur_entropy>maxentropy) {
			maxentropy = cur_entropy;
			max_index = i;
		}
	}
	cvThreshold(src, dst, (double)max_index, 255, CV_THRESH_BINARY);
	//cvReleaseHist(&hist);
}
void useMaxEntropy(IplImage *imgMaxEntropy)
{
	 MaxEntropy(imgMaxEntropy, imgMaxEntropy);
	 cvNamedWindow("MaxEntroyThreshold", CV_WINDOW_AUTOSIZE);
	 cvShowImage("MaxEntroyThreshold", imgMaxEntropy);//显示图像  
}


/*============================================================================
 = 代码内容：基本全局阈值法
 ==============================================================================*/
int BasicGlobalThreshold(int*pg, int start, int end)
{   
	int i, t, t1, t2, k1, k2;
	double u, u1, u2;
	t = 0;
	u = 0;
	for (i = start; i<end; i++)
	{
		t += pg[i];
		u += i*pg[i];
	}
	k2 = (int)(u / t); // 计算此范围灰度的平均值   
	do
	{
		k1 = k2;
		t1 = 0;
		u1 = 0;
		for (i = start; i <= k1; i++)
		{ // 计算低灰度组的累加和  
			t1 += pg[i];
			u1 += i*pg[i];
		}
		t2 = t - t1;
		u2 = u - u1;
		if (t1)
			u1 = u1 / t1; // 计算低灰度组的平均值  
		else
			u1 = 0;
		if (t2)
			u2 = u2 / t2; // 计算高灰度组的平均值  
		else
			u2 = 0;
		k2 = (int)((u1 + u2) / 2); // 得到新的阈值估计值  
	} while (k1 != k2); // 数据未稳定，继续  
						//cout<<"The Threshold of this Image in BasicGlobalThreshold is:"<<k1<<endl;  
	return(k1); // 返回阈值  
}

void use(IplImage *imgBasicGlobalThreshold)
{
	int pg[256], i, thre;
	for (i = 0; i<256; i++) pg[i] = 0;
	for (i = 0; i<imgBasicGlobalThreshold->imageSize; i++) // 直方图统计  
		pg[(BYTE)imgBasicGlobalThreshold->imageData[i]]++;
	thre = BasicGlobalThreshold(pg, 0, 256); // 确定阈值  
	cout << "The Threshold of this Image in BasicGlobalThreshold is:" << thre << endl;//输出显示阀值  
	cvThreshold(imgBasicGlobalThreshold, imgBasicGlobalThreshold, thre, 255, CV_THRESH_BINARY); // 二值化   
	cvNamedWindow("BasicGlobalThreshold", CV_WINDOW_AUTOSIZE);
	cvShowImage("BasicGlobalThreshold", imgBasicGlobalThreshold);//显示图像  
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

	pCapture = cvCaptureFromFile("d:\\测试\\345.mp4");
	//	pCapture = cvCreateCameraCapture(0);
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
			
			FillInternalContours(pFrImg, 300);
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




static void refineSegments(const Mat& img, Mat& mask, Mat& dst)
{
	int niters = 3;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Mat temp;
	dilate(mask, temp, Mat(), Point(-1, -1), niters);
	erode(temp, temp, Mat(), Point(-1, -1), niters * 2);
	dilate(temp, temp, Mat(), Point(-1, -1), niters);
	findContours(temp, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
	dst = Mat::zeros(img.size(), CV_8UC3);
	if (contours.size() == 0)
		return;
	// iterate through all the top-level contours,
	// draw each connected component with its own random color
	int idx = 0, largestComp = 0;
	double maxArea = 0;
	for (; idx >= 0; idx = hierarchy[idx][0])
	{
		const vector<Point>& c = contours[idx];
		double area = fabs(contourArea(Mat(c)));
		if (area > maxArea)
		{
			maxArea = area;
			largestComp = idx;
		}
	}
	Scalar color(0, 0, 255);
	drawContours(dst, contours, largestComp, color, FILLED, LINE_8, hierarchy);
}



Ptr<BackgroundSubtractor> pMOG;
Ptr<BackgroundSubtractor> pMOG2;
int dt_background()
{
	VideoCapture cap;
	bool update_bg_model = true;

	

	cap.open(0);

	if (!cap.isOpened())
	{
		printf("\nCan not open camera or video file\n");
		return -1;
	}

	Mat tmp_frame, bgmask, out_frame;

	cap >> tmp_frame;
	if (!tmp_frame.data)
	{
		printf("can not read data from the video source\n");
		return -1;
	}

	namedWindow("video", 1);
	namedWindow("segmented", 1);

	Ptr<BackgroundSubtractorMOG2> bgsubtractor = createBackgroundSubtractorMOG2();
	bgsubtractor->setVarThreshold(10);

	for (;;)
	{
		cap >> tmp_frame;
		if (!tmp_frame.data)
			break;
		bgsubtractor->apply(tmp_frame, bgmask, update_bg_model ? -1 : 0);
		refineSegments(tmp_frame, bgmask, out_frame);
		imshow("video", tmp_frame);
		imshow("segmented", out_frame);
		int keycode = waitKey(1);
		if (keycode == 27)
			break;
		if (keycode == ' ')
		{
			update_bg_model = !update_bg_model;
			printf("\n\t>‘背景学习’技术的状态 = %d\n", update_bg_model);
		}
	}

	return 0;
}




 //this is a sample for foreground detection functions
string src_img_name = "WavingTrees/b00";
const char *src_img_name1;
Mat img, fgmask, fgimg;
int i = -1;
char chari[500];
bool update_bg_model = true;
bool pause = false;

//第一种gmm,用的是KaewTraKulPong, P. and R. Bowden (2001).
//An improved adaptive background mixture model for real-time tracking with shadow detection.
BackgroundSubtractorMOG2* bg_model = createBackgroundSubtractorMOG2();

//void refineSegments(const Mat& img, Mat& mask, Mat& dst)
//{
//	int niters = 3;
//
//	vector<vector<Point> > contours;
//	vector<Vec4i> hierarchy;
//	Mat temp;
//	dilate(mask, temp, Mat(), Point(-1, -1), niters);//膨胀，3*3的element，迭代次数为niters
//	erode(temp, temp, Mat(), Point(-1, -1), niters * 2);//腐蚀
//	dilate(temp, temp, Mat(), Point(-1, -1), niters);
//
//	findContours(temp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);//找轮廓
//
//	dst = Mat::zeros(img.size(), CV_8UC3);
//	if (contours.size() == 0)
//		return;
//
//	// iterate through all the top-level contours,
//	// draw each connected component with its own random color
//	int idx = 0, largestComp = 0;
//	double maxArea = 0;
//
//	for (; idx >= 0; idx = hierarchy[idx][0])//这句没怎么看懂
//	{
//		const vector<Point>& c = contours[idx];
//		double area = fabs(contourArea(Mat(c)));
//		if (area > maxArea)
//		{
//			maxArea = area;
//			largestComp = idx;//找出包含面积最大的轮廓
//		}
//	}
//	Scalar color(0, 255, 0);
//	drawContours(dst, contours, largestComp, color, CV_FILLED, 8, hierarchy);
//}

//int gm()
//{
//	//bg_model.noiseSigma = 10;
//	img = imread("WavingTrees/b00000.bmp");
//	if (img.empty())
//	{
//		namedWindow("image", 1);//不能更改窗口
//		namedWindow("foreground image", 1);
//		namedWindow("mean background image", 1);
//	}
//	for (;;)
//	{
//		if (!pause)
//		{
//			i++;
//			itoa(i, chari, 10);
//			if (i<10)
//			{
//				src_img_name += "00";
//			}
//			else if (i<100)
//			{
//				src_img_name += "0";
//			}
//			else if (i>285)
//			{
//				i = -1;
//			}
//			if (i >= 230)
//				update_bg_model = false;
//			else update_bg_model = true;
//
//			src_img_name += chari;
//			src_img_name += ".bmp";
//
//			img = imread(src_img_name);
//			if (img.empty())
//				break;
//
//			//update the model
//			//bg_model->(img, fgmask, update_bg_model ? 0.005 : 0);//计算前景mask图像，其中输出fgmask为8-bit二进制图像，第3个参数为学习速率
//			//refineSegments(img, fgmask, fgimg);
//
//			imshow("image", img);
//			imshow("foreground image", fgimg);
//
//			src_img_name = "WavingTrees/b00";
//
//		}
//		char k = (char)waitKey(80);
//		if (k == 27) break;
//
//		if (k == ' ')
//		{
//			pause = !pause;
//		}
//	}
//
//	return 0;
//}





int main()
{
	//ss();
	//gm();
	//dt_background();
	show();
	return 0;
}