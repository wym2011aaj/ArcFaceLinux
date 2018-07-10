#include "FDEngine.h"
FDEngine::FDEngine()
{
	nScale = 16;
	nMaxFace = 12;
	pWorkMem = nullptr;
	hEngine = nullptr;
}
FDEngine::~FDEngine()
{
	if (hEngine != nullptr)
	{
		uninit();
	}
}
//初始化脸部检测引擎
int FDEngine::init()
{
	pWorkMem = (MByte *)malloc(WORKBUF_SIZE_FD);
	if (pWorkMem == nullptr)
	{
		return -1;
	}
	int ret = AFD_FSDK_InitialFaceEngine(APPID, FD_SDKKEY, pWorkMem, WORKBUF_SIZE_FD, &hEngine, AFD_FSDK_OPF_0_HIGHER_EXT, nScale, nMaxFace);
	return ret;
}
//销毁引擎，释放相应资源
int FDEngine::uninit()
{
	int ret = AFD_FSDK_UninitialFaceEngine(hEngine);
	if (pWorkMem != nullptr)
	{
		free(pWorkMem);
	}
	return ret;
}
//根据输入的图像检测出人脸位置，一般用于静态图像检测
//in 待检测图像信息
//out 人脸检测结果
int FDEngine::FaceDetection(LPASVLOFFSCREEN pImgData, LPAFD_FSDK_FACERES *pFaceRes)
{
	int ret = AFD_FSDK_StillImageFaceDetection(hEngine, pImgData, pFaceRes);
	return ret;
}