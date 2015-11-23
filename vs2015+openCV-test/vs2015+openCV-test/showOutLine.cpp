#include"stdafx.h"

bool graFilterMid(IplImage* &image, int k)
{
	if (image == NULL)
	{
		printf("\nFilterMidû�д���ͼ��");
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


			//ð������
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


/*======================================================================*/
 /* OTSU global thresholding routine */
 /*======================================================================*/
int otsu2(IplImage *image)
{
	int w = image->width;
	int h = image->height;

	unsigned char*np; // ͼ��ָ��  
	unsigned char pixel;
	int thresholdValue = 1; // ��ֵ  
	int ihist[256]; // ͼ��ֱ��ͼ��256����  

	int i, j, k; // various counters  
	int n, n1, n2, gmin, gmax;
	double m1, m2, sum, csum, fmax, sb;

	// ��ֱ��ͼ����...  
	memset(ihist, 0, sizeof(ihist));

	gmin = 255; gmax = 0;
	// ����ֱ��ͼ  
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
		sum += k * ihist[k]; /* x*f(x) ������*/
		n += ihist[k]; /* f(x) ���� */
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
	 cout << "The Threshold of this Image in Otsu is:" << thre2 << endl;//�����ʾ��ֵ  
	 cvThreshold(imgOtsu, imgOtsu, thre2, 255, CV_THRESH_BINARY); // ��ֵ��   
	 cvNamedWindow("imgOtsu", CV_WINDOW_AUTOSIZE);
	 //cvShowImage("imgOtsu", imgOtsu);//��ʾͼ��  
}
/*============================================================================
 = �������ݣ��������ֵ�ָ�
 = �޸�����:2009-3-3
 = ����:crond123
 = ����:http://blog.csdn.net/crond123/
 = E_Mail:crond123@163.com
 ===============================================================================*/
// ���㵱ǰλ�õ�������  
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
	 cvShowImage("MaxEntroyThreshold", imgMaxEntropy);//��ʾͼ��  
}


/*============================================================================
 = �������ݣ�����ȫ����ֵ��
 ==============================================================================*/
int BasicGlobalThreshold(int*pg, int start, int end)
{ // ����ȫ����ֵ��  
	int i, t, t1, t2, k1, k2;
	double u, u1, u2;
	t = 0;
	u = 0;
	for (i = start; i<end; i++)
	{
		t += pg[i];
		u += i*pg[i];
	}
	k2 = (int)(u / t); // ����˷�Χ�Ҷȵ�ƽ��ֵ   
	do
	{
		k1 = k2;
		t1 = 0;
		u1 = 0;
		for (i = start; i <= k1; i++)
		{ // ����ͻҶ�����ۼӺ�  
			t1 += pg[i];
			u1 += i*pg[i];
		}
		t2 = t - t1;
		u2 = u - u1;
		if (t1)
			u1 = u1 / t1; // ����ͻҶ����ƽ��ֵ  
		else
			u1 = 0;
		if (t2)
			u2 = u2 / t2; // ����߻Ҷ����ƽ��ֵ  
		else
			u2 = 0;
		k2 = (int)((u1 + u2) / 2); // �õ��µ���ֵ����ֵ  
	} while (k1 != k2); // ����δ�ȶ�������  
						//cout<<"The Threshold of this Image in BasicGlobalThreshold is:"<<k1<<endl;  
	return(k1); // ������ֵ  
}

void use(IplImage *imgBasicGlobalThreshold)
{
	int pg[256], i, thre;
	for (i = 0; i<256; i++) pg[i] = 0;
	for (i = 0; i<imgBasicGlobalThreshold->imageSize; i++) // ֱ��ͼͳ��  
		pg[(BYTE)imgBasicGlobalThreshold->imageData[i]]++;
	thre = BasicGlobalThreshold(pg, 0, 256); // ȷ����ֵ  
	cout << "The Threshold of this Image in BasicGlobalThreshold is:" << thre << endl;//�����ʾ��ֵ  
	cvThreshold(imgBasicGlobalThreshold, imgBasicGlobalThreshold, thre, 255, CV_THRESH_BINARY); // ��ֵ��   
	cvNamedWindow("BasicGlobalThreshold", CV_WINDOW_AUTOSIZE);
	cvShowImage("BasicGlobalThreshold", imgBasicGlobalThreshold);//��ʾͼ��  
}

//�ҵ��˶�����
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

	//��������
	cvMoveWindow("video", 30, 0);
	cvMoveWindow("background", 360, 0);
	cvMoveWindow("foreground", 690, 0);

	pCapture = cvCaptureFromFile("d:\\����\\VID_20150914_160002.mp4");
	//	pCapture = cvCreateCameraCapture(0);
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
			graFilterMid(pFrImg, 1);
			cvConvert(pFrImg, pFrameMat);
			//������˹�˲�����ƽ��ͼ��  
			
			cvSmooth(pFrImg, pFrImg, CV_GAUSSIAN, 3, 0, 0);
			
			//��ǰ֡������ͼ���  
			cvAbsDiff(pFrameMat, pBkMat, pFrMat);
			//��ֵ��ǰ��ͼ  
			//use(pFrImg);
			//useMaxEntropy(pFrImg);
			cvThreshold(pFrMat, pFrImg, 10, 255.0, CV_THRESH_BINARY);
			////������̬ѧ�˲���ȥ������
			cvErode(pFrImg, pFrImg, 0, 6);
			cvDilate(pFrImg, pFrImg, 0, 7);

			//���±���  
			cvRunningAvg(pFrameMat, pBkMat, 0.003, 0);
			//������ת��Ϊͼ���ʽ��������ʾ  
			cvConvert(pBkMat, pBkImg);
			pFrame->origin = IPL_ORIGIN_BL;
			pFrImg->origin = IPL_ORIGIN_BL;
			pBkImg->origin = IPL_ORIGIN_BL;
			
			FillInternalContours(pFrImg, 300);
			cvShowImage("video", pFrame);
			//cvShowImage("background", pBkImg);
			cvShowImage("foreground", pFrImg);
			//����а����¼���������ѭ��  
			//�˵ȴ�ҲΪcvShowImage�����ṩʱ�������ʾ  
			//�ȴ�ʱ����Ը���CPU�ٶȵ���  
			if (cvWaitKey(2) >= 0)
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

}

int main()
{
	//ss();
	show();
	return 0;
}