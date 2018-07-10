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
//��ʼ���������
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
//�������棬�ͷ���Ӧ��Դ
int FREngine::uninit()
{
	int ret = AFR_FSDK_UninitialEngine(hEngine);
	if (pWorkMem != nullptr)
	{
		free(pWorkMem);
	}
	return ret;
}
//��ȡ������������ 
//in�����ͼ������ 
//in�Ѽ�⵽��������Ϣ
//out��ȡ������������Ϣ
int FREngine::ExtractFRFeature(LPASVLOFFSCREEN pImgData, LPAFR_FSDK_FACEINPUT pFaceRes, LPAFR_FSDK_FACEMODEL pFaceModels)
{
	int ret = AFR_FSDK_ExtractFRFeature(hEngine, pImgData, pFaceRes, pFaceModels);
	return ret;
}
//���������Ƚ� 
//in��������������Ϣ
//in���Ƚϵ�����������Ϣ
//out�����������Ƴ̶���ֵ
int FREngine::FacePairMatching(AFR_FSDK_FACEMODEL *reffeature, AFR_FSDK_FACEMODEL *probefeature, MFloat *pfSimilScore)
{
	int ret = AFR_FSDK_FacePairMatching(hEngine, reffeature, probefeature, pfSimilScore);
	return ret;
}
