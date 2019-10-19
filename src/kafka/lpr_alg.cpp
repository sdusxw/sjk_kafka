#include "lpr_alg.h"
#include "rv_anpr_interface.h"
#include "jpg_codec.h"
#include "common.h"
#include <libyuv.h>
#include <string.h>
#include <stdio.h>
#include <iostream>

using namespace std;

#define WIDTH           4096            // Max image width
#define HEIGHT          2160            // Max image height

EyeJpegCodec ejc;
static unsigned char mem1[0x4000];                // 16K
static unsigned char mem2[100*1024*1024];            // 100M

//全局算法句柄
void *g_lprhandle;
//车牌识别初始化
bool vlpr_init()
{
    //打印算法版本信息
    char  szversion[64] ={0};
    RV_GETVERSION((unsigned char *)szversion) ;
    printf ("version:%s\n" ,szversion);
    //初始化算法句柄
    g_lprhandle =   RV_CREATE_LPR(WIDTH,HEIGHT, "test");
    if(g_lprhandle == NULL)
    {
        printf ("algorithm initialize error \n") ;
        return false;
    }
    
    return true;
}
//车牌颜色转化
//蓝 1   3
//黄 2   4
//白 3   2
//黑 4   3
//绿 5   5
//默认蓝色，没有黄绿色
//因为目前几乎没有黑色拍照，所有黑色拍照按照蓝牌处理
int pcolor_transfer(int c)
{
    int r=3;
    switch (c) {
        case 1:
            r=3;
            break;
        case 2:
            r=4;
            break;
        case 3:
            r=2;
            break;
        case 4:
            r=3;
            break;
        case 5:
            r=5;
            break;
    }
    return r;
}
//车牌识别，输入为jpg图像指针和长度，输出识别结果，识别结果结构体需要调用方分配
bool vlpr_analyze(const unsigned char *pImage, int len, PVPR pVPR)
{
    int w=0;int h=0; int c=4;
    char *argb_buf = (char *)malloc(WIDTH*HEIGHT*4);
    if(argb_buf==NULL)
    {
        return false;
    }
    //JPEG转为ARGB
    bool ret = ejc.JpegUnCompress((char *)pImage, len, (char *)argb_buf,
                                  WIDTH*HEIGHT*4, w, h, c);
    if(!ret)
    {
        //JPG解码失败释放缓存
        if(argb_buf)
        {
            free(argb_buf);
            argb_buf=NULL;
        }
        return false;
    }
    //ARGB转为NV12
    uint8_t *y_data=(uint8_t *)malloc((int)(w*h*1.5));
    uint8_t *uv_data=y_data+w*h;
    libyuv::ARGBToNV12((const uint8_t*)argb_buf, w*c, y_data, w, uv_data, w, w, h);
    //开始识别车牌
    RV_ANPRRESULT anprresult[8] = { 0 }; //buff 必须大于最多识别的车牌数
    RV_RECRECT     rectroi;
    rectroi.left = 0; rectroi.right = w -1;
    rectroi.top =h /4; rectroi.bottom = h - 32;
    //识别区域 尽量避开图片上叠加的文字
    int nPlateNum = 1;
    int nRet = RV_RECFRAME_SINGLE(g_lprhandle ,y_data, w, h, RV_YUV420SP_UVUV , rectroi, anprresult, &nPlateNum);
    //识别之后释放NV12缓存
    if(y_data)
    {
        free(y_data);
        y_data=NULL;
    }
    //识别之后释放ARGB缓存
    if(argb_buf)
    {
        free(argb_buf);
        argb_buf=NULL;
    }
    if(nRet >= 0)
    {
        if (nPlateNum == 0) {
            return false;
        }
        //车牌结果输出
        std::string plate_gbk=anprresult[0].platenum;
        std::string plate_utf8;
        gbk2utf8(plate_gbk, plate_utf8);
        strcpy(pVPR->license, plate_utf8.c_str());
        //车牌颜色处理
        std::string pcolor_gbk=anprresult[0].platecolor;
        std::string pcolor_utf8;
        gbk2utf8(pcolor_gbk, pcolor_utf8);
        strcpy(pVPR->color, pcolor_utf8.c_str());
        pVPR->nColor = pcolor_transfer(anprresult[0].platecolorindex);
        //车牌类型
        pVPR->nType = anprresult[0].platetype;
        //置信度
        pVPR->nConfidence = anprresult[0].platereliability;
        //车牌坐标
        pVPR->left = anprresult[0].platerect.left;
        pVPR->right = anprresult[0].platerect.right;
        pVPR->top = anprresult[0].platerect.top;
        pVPR->bottom = anprresult[0].platerect.bottom;
        return true;
    }
    else{
        return false;
    }
}
