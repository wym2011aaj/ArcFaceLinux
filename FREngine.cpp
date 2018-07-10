#include "FREngine.h"

FREngine::FREngine()
{
	pWorkMem = nullptr;
	hEngine = nullptr;
}
FREngine::~FREngine()
{
	if(hEngine!=nullptr)
		uninit();
}
//初始化引擎参数
int FREngine::init()
{
	pWorkMem = (MByte *)malloc(WORKBUF_SIZE_FR);
	if (pWorkMem == nullptr)
	{
		return -1;
	}
	int ret = AFR_FSDK_InitialEngine(APPID, FR_SDKKEY, pWorkMem, WORKBUF_SIZE_FR, &hEngine);
	return ret;
}
//销毁引擎，释放相应资源
int FREngine::uninit()
{
	int ret = AFR_FSDK_UninitialEngine(hEngine);
	if (pWorkMem != nullptr)
	{
		free(pWorkMem);
	}
	return ret;
}
//获取脸部特征参数 
//in输入的图像数据 
//in已检测到的脸部信息
//out提取的脸部特征信息
int FREngine::ExtractFRFeature(LPASVLOFFSCREEN pImgData, LPAFR_FSDK_FACEINPUT pFaceRes, LPAFR_FSDK_FACEMODEL pFaceModels)
{
	int ret = AFR_FSDK_ExtractFRFeature(hEngine, pImgData, pFaceRes, pFaceModels);
	return ret;
}
//脸部特征比较 
//in已有脸部特征信息
//in被比较的脸部特征信息
//out脸部特征相似程度数值
int FREngine::FacePairMatching(AFR_FSDK_FACEMODEL *reffeature, AFR_FSDK_FACEMODEL *probefeature, MFloat *pfSimilScore)
{
	int ret = AFR_FSDK_FacePairMatching(hEngine, reffeature, probefeature, pfSimilScore);
	return ret;
}
