// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include<cv.h>

#include <opencv2\video\background_segm.hpp>

#include <highgui.h>
#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2\video\background_segm.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <iostream>
#include <ctype.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
//C
#include <stdio.h>
//C++
#include <iostream>
#include <sstream>

#include <stdio.h>
#include <string>
using namespace cv;
using namespace std;

#define BYTE unsigned char

#define cvQueryHistValue_1D( hist, idx0 ) \
    ((float)cvGetReal1D( (hist)->bins, (idx0)))
#define cvQueryHistValue_2D( hist, idx0, idx1 )   cvGetReal2D( (hist)->bins, (idx0), (idx1) )

#define cvQueryHistValue_3D( hist, idx0, idx1 )   cvGetReal3D( (hist)->bins, (idx0), (idx1) )



// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
