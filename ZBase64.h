#pragma once
#include <string>  
#include <opencv2/opencv.hpp>

using namespace std;

class ZBase64
{
public:
	/*����
	DataByte
	[in]��������ݳ���,���ֽ�Ϊ��λ
	*/
	string Encode(const unsigned char* Data, int DataByte);
	/*����
	DataByte
	[in]��������ݳ���,���ֽ�Ϊ��λ
	OutByte
	[out]��������ݳ���,���ֽ�Ϊ��λ,�벻Ҫͨ������ֵ����
	������ݵĳ���
	*/
	vector<uchar> Decode(const char* Data, int DataByte, int& OutByte);
};