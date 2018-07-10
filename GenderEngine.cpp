#include "GenderEngine.h"
GenderEngine::GenderEngine()
{
	pWorkMem = nullptr;
	hEngine = nullptr;
}
GenderEngine::~GenderEngine()
{
	if (hEngine != nullptr)
	{
		uninit();
	}
}
//初始化性别检测引擎
int GenderEngine::init()
{
	pWorkMem = (MByte *)malloc(WORKBUF_SIZE_GENDER);
	if (pWorkMem == nullptr)
	{
		return -1;
	}
	int ret = ASGE_FSDK_InitGenderEngine(APPID, GENDER_SDKKEY, pWorkMem, WORKBUF_SIZE_GENDER, &hEngine);
	return ret;
}
//销毁引擎，释放相应资源
int GenderEngine::uninit()
{
	int ret = ASGE_FSDK_UninitGenderEngine(hEngine);
	if (pWorkMem != nullptr)
	{
		free(pWorkMem);
	}
	return ret;
}
//检测静态图片中人物的性别
//in 输入的图像数据
//in 已检测到的脸部信息
//out 性别检测结果
int GenderEngine::GenderEstimationStaticImage(LPASVLOFFSCREEN pImgInfo, LPASGE_FSDK_GENDERFACEINPUT pFaceRes, LPASGE_FSDK_GENDERRESULT pGenderRes)
{
	int ret = ASGE_FSDK_GenderEstimation_StaticImage(hEngine, pImgInfo, pFaceRes, pGenderRes);
	return ret;
}
//检测动态视频中人物的性别
//in 输入的图像信息
//in 输入的图像中人脸信息， 需要事先用人脸引擎检测出
//out 性别检测结果
int GenderEngine::GenderEstimationPreview(LPASVLOFFSCREEN pImgInfo, LPASGE_FSDK_GENDERFACEINPUT pFaceRes, LPASGE_FSDK_GENDERRESULT pGenderRes)
{
	int ret = ASGE_FSDK_GenderEstimation_Preview(hEngine, pImgInfo, pFaceRes, pGenderRes);
	return ret;
}