#include "AgeEngine.h"
AgeEngine::AgeEngine()
{
	pWorkMem = nullptr;
	hEngine = nullptr;
}
AgeEngine::~AgeEngine()
{
	if (hEngine != nullptr)
	{
		uninit();
	}
}
//初始化年龄检测引擎
int AgeEngine::init()
{
	pWorkMem = (MByte *)malloc(WORKBUF_SIZE_AGE);
	if (pWorkMem == nullptr)
	{
		return -1;
	}
	int ret = ASAE_FSDK_InitAgeEngine(APPID, AGE_SDKKEY, pWorkMem, WORKBUF_SIZE_AGE, &hEngine);
	return ret;
}
//销毁引擎，释放相应资源
int AgeEngine::uninit()
{
	int ret = ASAE_FSDK_UninitAgeEngine(hEngine);
	if (pWorkMem != nullptr)
	{
		free(pWorkMem);
	}
	return ret;
}
//检测静态图片中人物的年龄
//in 输入的图像数据
//in 已检测到的脸部信息
//out 年龄检测结果
int AgeEngine::AgeEstimationStaticImage(LPASVLOFFSCREEN pImgInfo, LPASAE_FSDK_AGEFACEINPUT pFaceRes, LPASAE_FSDK_AGERESULT pAgeRes)
{
	int ret = ASAE_FSDK_AgeEstimation_StaticImage(hEngine, pImgInfo, pFaceRes, pAgeRes);
	return ret;
}
//检测动态视频中人物的年龄
//in 输入的图像数据
//in 已检测到的脸部信息
//out 年龄检测结果
int AgeEngine::AgeEstimationPreview(LPASVLOFFSCREEN pImgInfo, LPASAE_FSDK_AGEFACEINPUT pFaceRes, LPASAE_FSDK_AGERESULT pAgeRes)
{
	int ret = ASAE_FSDK_AgeEstimation_Preview(hEngine, pImgInfo, pFaceRes, pAgeRes);
	return ret;
}