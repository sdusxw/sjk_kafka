
#ifndef _RV_ANPR_INTERFACE_
#define _RV_ANPR_INTERFACE_
// #############################################################################
// *****************************************************************************
//             Copyright (c) 2008-2018, 
//             This is the defination of  lpr  sdk 
// *****************************************************************************
// #############################################################################
//
// File:    rv_anpr_interface.h
// Author:  wang.fei
// First Created: 2006/08/20

// Description: Common definations.
// Change log:
//    <#> <date> <reason> <what are changed>

//          2015.6                 modify segment  ,add some wj or army sampers to ocrlib
//          2015.7                 add airport rec,  add switch to open or close airport plate and other green plate 
//          2015.12                modify some function to use  his ive 
//          2017.8                 modify  jz platform ,mstar platform
//          

// -----------------------------------------------------------------------------


#ifdef WIN32

#ifndef LPR_API  
#define LPR_API __declspec (dllexport)  
#endif

#else
#ifndef LPR_API  
#define LPR_API __attribute__ ((visibility ("default")))  
#endif
#endif 

#ifdef __cplusplus
extern "C"
{
#endif

#define   RV_PLATELEN   16
#define   RV_COLORLEN   8
#define   RV_VTYPELEN    256
#define   RV_MAXLETTER    12

	//#define  USEFOURPOS 


	/*************************************************/
	//plate type  define 
#define   PLATE_TYPE_UNKONW							 0x000000   //未知
#define   PLATE_TYPE_NORMAL_BLUE					 0x000001  //普通蓝牌
#define   PLATE_TYPE_NORMAL_YELLOW					 0x000002 //普通黄牌
#define   PLATE_TYPE_POLICE                                 0x000004//警车车牌
#define   PLATE_TYPE_WUJING                                0x000008 //武警车牌
#define   PLATE_TYPE_DWYELLOW                        0x000010 //双层黄牌
#define   PLATE_TYPE_MOTOR                                  0x000020//摩托车车牌
#define   PLATE_TYPE_XUE                                        0x000040 //教练车车牌
#define  PLATE_TYPE_MILITARY                               0x000080 //军车车牌
#define  PLATE_TYPE_PERSONAL                             0x000100 //个性车牌
#define  PLATE_TYPE_GANGAO                                 0x000200 //港澳车牌
#define  PLATE_TYPE_EMBASSY                                 0x000400 //使馆车牌

#define  PLATE_TYPE_EMBASSY_NEW							0x000800 //使馆车牌
#define  PLATE_TYPE_NEWENERGY                           0x001000  //新能源

#define  PLATE_TYPE_AIR_PLANE                             0x002000 // 民航 1
#define  PLATE_TYPE_AIR_PLANE_1                           0x004000  //民航 2 

#define  PLATE_TYPE_JUN_AIR                              0x008000  //军用机场

#define  PLATE_TYPE_NEWENERGYBIG                             0x010000  ///新能源大   
#define  PLATE_TYPE_GANGKOU                                  0x020000  ///港口
#define  PLATE_TYPE_POLICE_WHITE                             0x040000  /// jingche
#define   PLATE_TYPE_JUN_CUSTOM                             0x080000
#define   PLATE_TYPE_YINGJI                                 0x100000
 



/*************************************************/
	// color 
#define  PLATE_COLOR_BLUE                               0x1
#define PLATE_COLOR_YELLOW                          0x2
#define PLATE_COLOR_WHITE                                0x3
#define  PLATE_COLOR_BLACK                             0x4
#define   PLATE_COLOR_GREEN                            0x5


#define  RV_BGR								0x01  //## BGR BGR 
#define  RV_YUV420P							0x02  //## yyyy u  v 
#define  RV_YUV422P					        0x03  //##  yyyy uu vv 
#define  RV_UYVY422I						0x04  //## uyvy uyvy 
#define  RV_YUV420SP_UVUV                   0x05 //## yyyy   uv
#define  RV_YUV420SP_VUVU                   0x06 //## yyyy   vu 
#define  RV_RGB                             0x07 //## RGB RGB
#define  RV_RGBA                             0x08 //## RGBA RGBA
#define  RV_BGRA                             0x09 //## RGBA RGBA


	typedef struct
	{
		int x;
		int y;
	}RV_REC_POINT;

	typedef struct
	{
		RV_REC_POINT  start;
		RV_REC_POINT  end;
	}RV_REC_LINE;


	typedef struct
	{
		int pointnum;
		RV_REC_POINT   points[8];
	}RV_HARD_ROIS;


	typedef struct
	{
		int left;
		int top;
		int right;
		int bottom;
	}RV_RECRECT;


	typedef struct
	{
		float score;
		int x1;
		int y1;
		int x2;
		int y2;
		float area;
		float ppoint[10];
	}FACE_RECT;




	typedef struct
	{
		RV_RECRECT platerect;
		RV_RECRECT carrect;
#ifdef FACELIB
		FACE_RECT   facerect;
#endif 
		char     szplatenum[16];
		char     szplatecolor[8];
		float    fplatereal;
		int   nStatus;  // 目标状态，0 为正常识别区域，1 为预测出来的
	}RV_HISPOS;






#define  MAX_TRACKNUM    			32
#define  MAX_POS                   128

	// 跟踪结果结构体
	typedef struct  _trackresult
	{
		int   trackid; //唯一的跟踪ID号从0-1024 循环
		int   status; //当前目标状态 <0 为无状态   其他是碰线 离线 离开 直行 左转  右转 (参考宏定义)
		RV_RECRECT  m_lastrect; //  最后一个跟踪框的位置
#ifdef FACELIB
		FACE_RECT   m_lastface;
#endif 
		RV_REC_POINT m_lastcenter; //最后一个跟踪框的中心点 < 车牌中心 ，没有车牌是车辆中心>
		int   m_speed; // 当前目标速度
		int   m_movex;
		int   m_movey;
		int   m_loopno;//  目标的车道号  根据第一次检测到的判断(需要设置车道分割线)
		int    m_posnum;// 历史轨个数
		int    m_curobjectlight;// 当前目标亮度
		RV_HISPOS    m_objectrect[MAX_POS]; // 历史轨迹框的位置

	}trackresult;


	// 跟踪列表
	typedef struct  _trackresultlist
	{
		int  tracknum; // 当前跟踪的目标的个数
		trackresult m_trackresult[MAX_TRACKNUM]; //每一个目标的 结果
	}trackresultlist_t, *ptrackresultlist_t;


	typedef enum
	{
		RV_YUV_PLANAR_422 = 0x01,
		RV_YUV_SEMIPLANAR_422 = 0x02,
		RV_YUV_SEMIPLANAR_420_VU = 0x03,
		RV_YUV_PLANAR_420 = 0x04,
		RV_YUV_SEMIPLANAR_420_UV = 0x05,
		RV_BGR24 = 0x06,

	} RV_YUV_FORMAT_E;

	typedef struct
	{
		RV_YUV_FORMAT_E  enPixelFormat;
		int  s32Width;
		int s32Height;
		int s32Stride;
		int s32Stride1;
		int s32Stride2;
		unsigned char  *pu8YData;   // Y ,R  
		unsigned char  *pu8UData;   //U,G
		unsigned char  *pu8VData;   //V,B
		unsigned char  *pu8UVData;
		unsigned int   u32YDataPhyAddr;
		unsigned int   u32UDataPhyAddr;
		unsigned int   u32VDataPhyAddr;
		unsigned int   u32UVDataPhyAddr;
		unsigned int   PoolID;
	} RV_YUV_IMAGE_S;

	typedef struct _tagRv_AnprRsult
	{
		char 	platenum[RV_PLATELEN];//## plate num   GB2312
		char    platecolor[RV_COLORLEN];//## plate color GB2312
		char    carcolor[RV_COLORLEN];//## plate color GB2312
		int       platecolorindex;  //##  plate color index 
		int       platetype; //##  plate type  
		float  	  platereliability;  // ## plate realbility 
		float        plateletterreal[RV_MAXLETTER]; // letter realbility 
		RV_RECRECT  	platerect;    // ## plate position  left  top right bottom
#ifdef USEFOURPOS
		RV_REC_POINT    platepoint[4]; // only for test 

		RV_RECRECT      letterrect[RV_MAXLETTER];
		RV_REC_POINT    letterpoint[RV_MAXLETTER][4]; // only for test 
#endif 
		char     cartype[RV_VTYPELEN]; 	//## car type  (if rec this ,just support BGR mode)
		float    cartypereliability;  //## car type   realbility 
		float     vertangle;//##  plate vert angle 
		float      horzangle;//##plate  horz angle 
	}RV_ANPRRESULT, *PRV_ANPRRESULT;



	typedef struct
	{
		char 	platenum[RV_PLATELEN];//## plate num   GB2312
		char platecolor[RV_COLORLEN];//## plate color GB2312
		char carcolor[RV_COLORLEN];//## plate color GB2312
		int   platecolorindex;  //##  plate color index 
		float  	platereliability;  // ## plate realbility 
		float        plateletterreal[RV_MAXLETTER]; // letter realbility 
		RV_RECRECT  	platerect;    // ## plate position  left  top right bottom  
		char     cartype[RV_VTYPELEN]; 	//## car type  (if rec this ,just support BGR mode,only support x86 now )
		float    cartypereliability;  //## car type   realbility 
		float     vertangle;//##  plate vert angle 
		float      horzangle;//##plate  horz angle 

	}RV_ANPRRESULT_V1, *PRV_ANPRRESULT_V1;



#define  MAX_HANZI  4  
	typedef struct
	{
		int  nIndex; // 汉字index
		//权重 范围  (-1)  -(+1)  默认为0 ，大于0 增加权重，小于0 减少权重
		//如果要默认汉字功能，可以将权重设置到最高.
		//比如 设置 京 权重增加 可以设置 为 0.8 ，降低晋识别效果 可以设置为 -0.5
		float   nWeight;
	}RV_HANZIPARAM;


	typedef struct
	{
		int  nCount;
		RV_HANZIPARAM  g_hanziparam[MAX_HANZI];
	}RV_HANZIPARAMLIST;

	typedef struct
	{
		//范围 0.4- 1.4  <>
		//为了保证速度，这里可以设置合理的第一次缩放参数，
		//对每一个目标就会调整初始缩放比例。比如 0.4   
		//视频识别的基缩放参数， 视频识别会自动调整ratio ，
		float   fBaseRatio;

		// 输出超时时间 ，单位S ，同一个车牌输出一次后 超过nTimeOut 才会输出
		unsigned int    nTimeOut;
		int                    nMove;  //开启运动检测 // 默认开启
		int                    nDetectNoPlate; //开启无牌车检测  默认关闭
		int                    nFiterDirection; //过滤 方向 0 不过滤 ，1 过滤向上的 2 过滤向下的
		int                    nSuperBigAngle;
		int                    nDetectCar;
		int                    nAeWithCar;

	}RV_VIDEORECPARAM;


	typedef enum
	{
		BUF_IMAGE_TYPE_U8C1 = 0x00,
		BUF_IMAGE_TYPE_S8C1 = 0x01,
		BUF_IMAGE_TYPE_YUV420SP = 0x02, /*YUV420 SemiPlanar*/
		BUF_IMAGE_TYPE_YUV422SP = 0x03, /*YUV422 SemiPlanar*/
		BUF_IMAGE_TYPE_YUV420P = 0x04, /*YUV420 Planar */
		BUF_IMAGE_TYPE_YUV422P = 0x05, /*YUV422 planar */
		BUF_IMAGE_TYPE_S8C2_PACKAGE = 0x06,
		BUF_IMAGE_TYPE_S8C2_PLANAR = 0x07,
		BUF_IMAGE_TYPE_S16C1 = 0x08,
		BUF_IMAGE_TYPE_U16C1 = 0x09,
		BUF_IMAGE_TYPE_U8C3_PACKAGE = 0x0A,
		BUF_IMAGE_TYPE_U8C3_PLANAR = 0x0B,
		BUF_IMAGE_TYPE_S32C1 = 0x0C,
		BUF_IMAGE_TYPE_U32C1 = 0x0D,
		BUF_IMAGE_TYPE_S64C1 = 0x0E,
		BUF_IMAGE_TYPE_U64C1 = 0x0F,
		BUF_IMAGE_TYPE_BUTT = 0xFF
	} BUF_IMAGE_TYPE_E;



	typedef struct
	{
		BUF_IMAGE_TYPE_E enType;
		void   *pu32PhyAddr[3];
		unsigned char   *pu8Addr[3];
		unsigned short  u16Stride[3];
		unsigned short u16Width;
		unsigned short u16Height;
		unsigned short u16Reserved;     /*Can be used such as elemSize*/
	} BUF_IMAGE_S;



	typedef void*           RV_LPRHANDLE;

	typedef  void*           RV_FACEHANDLE;

	// log

	//日志回调定义如下

	typedef int(*LprLogFun)(int level, const char *fName, const char *func, int nLine, const char *fmt, ...);


	typedef int(*ImgOutPut_Rec)(RV_LPRHANDLE   lprid, RV_YUV_IMAGE_S  *pImage, RV_ANPRRESULT *struPlateResult, int  nDirection, int nCount, int  havecar ,int  dx ,int dy ,int mode ,int nTotalCount , void *pUser);  //回调


	typedef int(*ImgOutPut_FACE)(RV_YUV_IMAGE_S  *pImage,FACE_RECT  facerect,  void *pUser);  //人脸回调




	typedef int(*ImgSetFun)(int  maxshut, int maxgain);



	LPR_API int  RV_SET_LOGFUN_LPR(void * pLogFun);

	LPR_API int  RV_SET_IMGSETFUN_LPR(void * pFun);


	/**************************************************************************
	名称:	RV_CREATE_LPR
	功能:  创建识别句柄
	输入参数:	char * password       识别授权密码 140字节【测试版随意输入一个字符串】
	返回:	RV_LPRHANDLE  识别句柄  >0 为成功，<=0 为失败
	**************************************************************************/
	LPR_API RV_LPRHANDLE    RV_CREATE_LPR(int nWidth, int nHeight, char *password);

	/**************************************************************************
	名称:	RV_GET_DEVID
	功能: 获取设备ID号 仅嵌入式有效
	输入参数:	unsigned char * devid      存储设备id的内存地址，>=12字节
	输出参数:	unsigned char * devid      成功输出设备ID号
	返回:	int     =0 成功  !0 失败
	**************************************************************************/
	LPR_API int             RV_GET_DEVID(unsigned char *devid);

	/**************************************************************************
	名称:	RV_SET_DAYNIGHT
	功能:   设置当前是白天还是夜间
	输入参数:	int day       0 表示夜间 1 表示白天

	返回:	int     =0 成功  !0 失败
	**************************************************************************/

	LPR_API int             RV_SET_DAYNIGHT(int day);


	/**************************************************************************
	名称:	RV_RELEASE_LPR
	功能: 释放识别
	输入参数:	void* lprid     创建返回的识别句柄
	返回:	int     =0 成功  !0 失败
	**************************************************************************/
	LPR_API int			  RV_RELEASE_LPR(void* lprid);

	/**************************************************************************
	名称:	RV_SET_DEFAULT_LPR
	功能: 设置缺省汉字及识别类型
	输入参数:	void* lprid     创建返回的识别句柄
	输入参数:	int  DefaultHan      缺省汉字
	输入参数:	int  RecType          识别类型 0 为全部，不建议使用0 ， 方法:PLATE_TYPE_NORMAL_BLUE| PLATE_TYPE_NORMAL_YELLOW
	返回:	int     =0 成功  !0 失败
	**************************************************************************/

	LPR_API int			  RV_SET_DEFAULT_LPR(void* lprid, int  DefaultHan, int RecType);

	/**************************************************************************
	名称:	RV_SET_MASK_LPR
	功能:  设置多边形区域，建议改变参数时候再调用 【仅对视频识别有效】
	输入参数:	void* lprid     创建返回的识别句柄
	输入参数  RV_HARD_ROIS *rois   多边形区域， 设置后可有效排除 多边形区域以外的车牌
	输入参数   int nWidth, int nHeight 识别图像的宽度高度
	返回:	int     =0 成功  !0 失败
	**************************************************************************/
	LPR_API int            RV_SET_MASK_LPR(void* lprid, RV_HARD_ROIS *rois, int nWidth, int nHeight);
	 
 
	/**************************************************************************
	名称:	RV_SETBASERATIO_LPR
	功能:  设置基本缩放参数， 比如识别图像1080P，车牌尺寸基本都大于300，可设置为0.4  ，即车牌会变成120
	输入参数:	void* lprid     创建返回的识别句柄
	输入参数  float fBase    缩放比例 ，0.4-1.4
	返回:	int     =0 成功  !0 失败
	**************************************************************************/
	LPR_API int	 RV_SETBASERATIO_LPR(void* lprid, float  fBase);

 
 

	/**************************************************************************
	名称:	RV_PROCESS_LPR_VIDEO
	功能:   视频识别
	输入参数:	void* lprid     创建返回的识别句柄
	输入参数  RV_YUV_IMAGE_S * pImg  输入图像，见定义
	输入参数   RV_RECRECT recrect，初始检测区域，可以配置为多边形的外接矩形
	输入参数   RV_REC_LINE  nSnapLine   需要输出车牌时候的线 ，原始坐标
	输出参数:  trackresultlist_t *ptracklist  目标跟踪结果，可以根据目标自行判断输出
	输出参数:  RV_ANPRRESULT pLprResult  车牌输出结果，返回值 为1 是有效
	返回:	1 当前有目标正常输出。 0 其他 。< 0 异常
	**************************************************************************/
	LPR_API int	 RV_PROCESS_LPR_VIDEO(void* lprid, RV_YUV_IMAGE_S * pImg, RV_RECRECT recrect, RV_REC_LINE  nSnapLine, trackresultlist_t *ptracklist, RV_ANPRRESULT *pLprResult);
	 

	/**************************************************************************
	名称:	RV_SET_VIDEOPARAM_LPR
	功能:   设置视频识别的一些参数
	输入参数:	void* lprid     创建返回的识别句柄
	输入参数  RV_VIDEORECPARAM  vParam 具体见定义
	返回:	int     =0 成功  !0 失败
	**************************************************************************/
	LPR_API int  RV_SET_VIDEOPARAM_LPR(void* lprid, RV_VIDEORECPARAM *vParam);


	/**************************************************************************
	名称:	RV_GETVERSION
	功能:   获取算法版本号
	输出参数:	unsigned char  * Version 版本号
	返回:	int     =0 成功  !0 失败
	**************************************************************************/
	LPR_API int	  RV_GETVERSION(unsigned char  * Version);
	 


	/**************************************************************************
	名称:	RV_REG_REC_CALLBACK
	功能:   注册视频识别回调函数 ，当有车辆抓拍需要输出时候会触发此函数
	输入参数:	 void* lprid  ，初始化车牌句柄
	输入参数:   ImgOutPut_Rec  pFun  回调定义，见定义内容
	输入参数: void *pUse  用户参数，透明传出
	返回:	int     =0 成功  !0 失败

	注意： 此函数可以不调用，如果调用，在回调中不要处理耗时过高的操作。
	**************************************************************************/
	LPR_API int     RV_REG_REC_CALLBACK(void* lprid, ImgOutPut_Rec  pFun, void *pUser);


 

	/**************************************************************************
	名称:	RV_RECFRAME_SINGLE
	功能:   单帧车牌识别接口 
 	输入参数:	void* lprid     创建返回的识别句柄
	输入参数:  pImage   输入图像， 目前仅支持yuv420sp -uv  or vu
	输入参数:  int nwidth, int nheight frametype 宽度高度 ，图像类型 现在仅能是 5 or 6
	输入参数   RV_RECRECT recrect，初始检测区域，可以配置为多边形的外接矩形
	
	输出参数:  RV_ANPRRESULT *m_plate  车牌输出结果， 
	输入输入：int *platenum 需要识别到的车牌个数 和实际识别到车牌个数  
	返回:	识别到的车牌数 <0 异常
	**************************************************************************/
	 
	//单帧识别接口  
	LPR_API int  RV_RECFRAME_SINGLE(void* lprid, unsigned char *pImage, int nwidth, int nheight, int frametype, RV_RECRECT recrect, RV_ANPRRESULT *m_plate, int *platenum);


	LPR_API int  RV_CHECKPASS(char  *password);


	// 420sp 图像缩放为 1/2 *1/2 
	LPR_API int	 RV_PROCESS_YUV420SP_RESIZE_HALF(unsigned char *pSrcY, unsigned char *pSrcUV, int nWidth, int nHeight, unsigned char *pDstY, unsigned char *pDstUv, int nNewWidth, int nNewHeight);


	// 缩放图像 单通道   ，neon 优化
	LPR_API int	 RV_PROCESS_RESIZE(unsigned char *pSrc, int nWidth, int nHeight, unsigned char *pDst, int nNewWidth, int nNewHeight);




	// /////////////////人脸检测相关接口///////////////////////////////////

	LPR_API int     RV_CREATE_FACE(int nWidth, int nHeight, int nMinFace, int nMaxFace, char *password);

	LPR_API int     RV_DETECT_FACE(RV_YUV_IMAGE_S * pImg, RV_RECRECT recrect ,FACE_RECT *facerect);

	LPR_API int     RV_SET_MASK_FACE(  RV_HARD_ROIS *rois, int nWidth, int nHeight);
	LPR_API int     RV_SET_FACE_FUN(ImgOutPut_FACE pFun, void *pUser);
	

	LPR_API int     RV_DETECT_FACE_VIDEO(RV_YUV_IMAGE_S * pImg, unsigned char *pTrackImg, RV_RECRECT recrect, trackresultlist_t *ptracklist);

	LPR_API int     RV_FAST_COPY(unsigned char *psrc, unsigned char *pdst, RV_RECRECT  rects, int width, int height);

	//获取缓冲队列中的内存，应用层作为复用内存
	LPR_API unsigned char *  RV_GET_BUFLIST();

	LPR_API    int   RV_YUV2RGB_ROI(unsigned char *pYuv, int nwidth, int nheight, unsigned char *pRgb, RV_RECRECT roi);



#ifdef __cplusplus
}
#endif

#endif