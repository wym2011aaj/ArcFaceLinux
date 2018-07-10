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
//��ʼ������������
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
//�������棬�ͷ���Ӧ��Դ
int AgeEngine::uninit()
{
	int ret = ASAE_FSDK_UninitAgeEngine(hEngine);
	if (pWorkMem != nullptr)
	{
		free(pWorkMem);
	}
	return ret;
}
//��⾲̬ͼƬ�����������
//in �����ͼ������
//in �Ѽ�⵽��������Ϣ
//out ��������
int AgeEngine::AgeEstimationStaticImage(LPASVLOFFSCREEN pImgInfo, LPASAE_FSDK_AGEFACEINPUT pFaceRes, LPASAE_FSDK_AGERESULT pAgeRes)
{
	int ret = ASAE_FSDK_AgeEstimation_StaticImage(hEngine, pImgInfo, pFaceRes, pAgeRes);
	return ret;
}
//��⶯̬��Ƶ�����������
//in �����ͼ������
//in �Ѽ�⵽��������Ϣ
//out ��������
int AgeEngine::AgeEstimationPreview(LPASVLOFFSCREEN pImgInfo, LPASAE_FSDK_AGEFACEINPUT pFaceRes, LPASAE_FSDK_AGERESULT pAgeRes)
{
	int ret = ASAE_FSDK_AgeEstimation_Preview(hEngine, pImgInfo, pFaceRes, pAgeRes);
	return ret;
}