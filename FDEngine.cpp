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
//��ʼ�������������
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
//�������棬�ͷ���Ӧ��Դ
int FDEngine::uninit()
{
	int ret = AFD_FSDK_UninitialFaceEngine(hEngine);
	if (pWorkMem != nullptr)
	{
		free(pWorkMem);
	}
	return ret;
}
//���������ͼ���������λ�ã�һ�����ھ�̬ͼ����
//in �����ͼ����Ϣ
//out ���������
int FDEngine::FaceDetection(LPASVLOFFSCREEN pImgData, LPAFD_FSDK_FACERES *pFaceRes)
{
	int ret = AFD_FSDK_StillImageFaceDetection(hEngine, pImgData, pFaceRes);
	return ret;
}