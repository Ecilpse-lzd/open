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

void FillInternalContours(IplImage *pBinary, IplImage *pBack)
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
		cvRectangle(pBack, cvPoint(temporaryRect.x, temporaryRect.y),
			cvPoint(temporaryRect.x + temporaryRect.width, temporaryRect.y + temporaryRect.height),
			CV_RGB(255, 0, 255), 4, 8, 0);
		
		centerW = temporaryRect.x + temporaryRect.width/2;
		centerH = temporaryRect.y + temporaryRect.height/2;
		printf("�������%f,�е�����X : %d,Y : %d\n", mexArea,centerW,centerH);
		cvReleaseMemStorage(&pStorage);
		pStorage = NULL;
	}
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

	pCapture = cvCaptureFromFile("d:\\����\\345.mp4");
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
			cvWaitKey();
		}
		else
		{
			cvCvtColor(pFrame, pFrImg, CV_BGR2GRAY);
			graFilterMid(pFrImg, 1);
			
			//������˹�˲�����ƽ��ͼ��  
			
			cvSmooth(pFrImg, pFrImg, CV_GAUSSIAN, 3, 0, 0);
			cvConvert(pFrImg, pFrameMat);
			//��ǰ֡������ͼ���  
			cvAbsDiff(pFrameMat, pBkMat, pFrMat);
			//��ֵ��ǰ��ͼ  
			//use(pFrImg);
			//useMaxEntropy(pFrImg);
			cvThreshold(pFrMat, pFrImg, 80, 255.0, CV_THRESH_BINARY);
			////������̬ѧ�˲���ȥ������
			cvErode(pFrImg, pFrImg, 0, 10);
			//cvDilate(pFrImg, pFrImg, 0, 7);

			//���±���  
			//cvRunningAvg(pFrameMat, pBkMat, 0.003, 0);
			//������ת��Ϊͼ���ʽ��������ʾ  
			cvConvert(pBkMat, pBkImg);
			pFrame->origin = IPL_ORIGIN_BL;
			pFrImg->origin = IPL_ORIGIN_BL;
			pBkImg->origin = IPL_ORIGIN_BL;
			
			FillInternalContours(pFrImg, pFrame);
			cvShowImage("video", pFrame);
			//cvShowImage("background", pBkImg);
			cvShowImage("foreground", pFrImg);
			//����а����¼���������ѭ��  
			//�˵ȴ�ҲΪcvShowImage�����ṩʱ�������ʾ  
			//�ȴ�ʱ����Ը���CPU�ٶȵ���  
			if (cvWaitKey(10) >= 0)
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
	//gm();
	//dt_background();
	show();
	return 0;
}