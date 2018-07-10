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
//��ʼ���Ա�������
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
//�������棬�ͷ���Ӧ��Դ
int GenderEngine::uninit()
{
	int ret = ASGE_FSDK_UninitGenderEngine(hEngine);
	if (pWorkMem != nullptr)
	{
		free(pWorkMem);
	}
	return ret;
}
//��⾲̬ͼƬ��������Ա�
//in �����ͼ������
//in �Ѽ�⵽��������Ϣ
//out �Ա�����
int GenderEngine::GenderEstimationStaticImage(LPASVLOFFSCREEN pImgInfo, LPASGE_FSDK_GENDERFACEINPUT pFaceRes, LPASGE_FSDK_GENDERRESULT pGenderRes)
{
	int ret = ASGE_FSDK_GenderEstimation_StaticImage(hEngine, pImgInfo, pFaceRes, pGenderRes);
	return ret;
}
//��⶯̬��Ƶ��������Ա�
//in �����ͼ����Ϣ
//in �����ͼ����������Ϣ�� ��Ҫ�����������������
//out �Ա�����
int GenderEngine::GenderEstimationPreview(LPASVLOFFSCREEN pImgInfo, LPASGE_FSDK_GENDERFACEINPUT pFaceRes, LPASGE_FSDK_GENDERRESULT pGenderRes)
{
	int ret = ASGE_FSDK_GenderEstimation_Preview(hEngine, pImgInfo, pFaceRes, pGenderRes);
	return ret;
}