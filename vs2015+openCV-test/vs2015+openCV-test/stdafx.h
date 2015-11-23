// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include<highgui.h>
#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include<opencv2\highgui\highgui.hpp>
#include <iostream>
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <iostream>
using namespace std;

#define BYTE unsigned char

#define cvQueryHistValue_1D( hist, idx0 ) \
    ((float)cvGetReal1D( (hist)->bins, (idx0)))



// TODO:  在此处引用程序需要的其他头文件
