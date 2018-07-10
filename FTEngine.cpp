#include "FTEngine.h"

FTEngine::FTEngine()
{
	nScale = 16;
	nMaxFace = 12;
	pWorkMem = nullptr;
	hEngine = nullptr;
}
FTEngine::~FTEngine()
{
	if (hEngine != nullptr)
	{
		uninit();
	}
}
//初始化脸部检测引擎 视频
int FTEngine::init()
{
	pWorkMem = (MByte *)malloc(WORKBUF_SIZE_FT);
	if (pWorkMem == nullptr)
	{
		return -1;
	}
	int ret = 0;
	ret = AFT_FSDK_InitialFaceEngine(APPID, FT_SDKKEY, pWorkMem, WORKBUF_SIZE_FT, &hEngine, AFT_FSDK_OPF_0_HIGHER_EXT, nScale, nMaxFace);
	return ret;
}
//销毁引擎，释放相应资源
int FTEngine::uninit()
{
	int ret = 0;
	ret = AFT_FSDK_UninitialFaceEngine(hEngine);
	if (pWorkMem != nullptr)
	{
		free(pWorkMem);
	}
	return ret;
}
//根据输入的图像检测人脸，一般用于视频检测，多帧方式
//in 带检测图像信息
//out 人脸检测结果
int FTEngine::FaceTracking(LPASVLOFFSCREEN pImgData, LPAFT_FSDK_FACERES *pFaceRes)
{
	int ret;
	ret = AFT_FSDK_FaceFeatureDetect(hEngine, pImgData, pFaceRes);
	return ret;
}