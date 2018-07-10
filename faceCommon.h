#pragma once
//����ҵ�����ͱ��
#ifndef _FACESERVICE_H_
#define _FACESERVICE_H_
#include <string>
#include <vector>
#include <stdlib.h>
#include <time.h> 
#include <sstream>
#include <unistd.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <stdio.h>         
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <curl/multi.h>

#define PORT 12346           //�������˿�
#define BACKLOG 200           //listen���еȴ���������
#define MAXDATASIZE 1024    //��������С
#define IP_BUF_SIZE 129
#define SOCKET_ERROR (-1)
#define SD_SEND         0x01
#define MSG_BUF_SIZE 1024

#ifdef WIN32
#define TYPEINT int
#include <io.h>
#include <direct.h> 
#else
#define TYPEINT long
#include <unistd.h>
#include <sys/stat.h>
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

typedef unsigned char       BYTE;

#define MAX_PATH_LEN 256

#ifdef WIN32
#define ACCESS(fileName,accessMode) _access(fileName,accessMode)
#define MKDIR(path) _mkdir(path)
#else
#define ACCESS(fileName,accessMode) access(fileName,accessMode)
#define MKDIR(path) mkdir(path,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)
#endif

#define APPID "J2V3ixxJJ2MScCMd9KSwrnwiJBWDxqmuiaKjDy1m8fDo"
//����׷��
#define FT_SDKKEY		"2tmKhJbmJdvfSQw9FAP6bLASVwCDN5tpZZdXef6S4zKi"
//�������
#define FD_SDKKEY		"2tmKhJbmJdvfSQw9FAP6bLAZfLTQmY9eDdVv1hWVXDka"
//����ʶ��
#define FR_SDKKEY		"2tmKhJbmJdvfSQw9FAP6bLB4JwW1xUWZ1wV64f5imC8T"
//����ʶ��
#define AGE_SDKKEY		"2tmKhJbmJdvfSQw9FAP6bLBJdk2RC1j94jSSTKwqQoNy"
//�Ա�ʶ��
#define GENDER_SDKKEY	"2tmKhJbmJdvfSQw9FAP6bLBRo9HahTax1D6hQt695B72"

#define GROUPFILE "group"
#define FACEDETECT "detect"
#define FACEADD "addFace"
#define FACEUPDATE "updateFace"
#define FACEDEL "delFace"
#define FACERECOGNITION "recognition"
#define FACEINIT "init"
#define FACEFEATURE "feature"
#define FACECOMPARISON "comparison"

#ifdef __cplusplus
extern "C" {
#endif
	typedef struct {
		std::string jsonstr;
		int connectfd;
		int index;
	}SENDDATA;

	typedef struct {
		std::string features;
		int featureSize;
	}PersonFeatures, *LPPersonFeatures;

	typedef struct {
		std::string personIds;
		std::string groupId;
	}GroupPersonIds;

	typedef struct {
		int groupSize;
		std::vector<GroupPersonIds> groupPersons;
	}InitGroups, *LPInitGroups;

	typedef struct {
		std::string features;
		int featureSize;
		std::string personId;
	}Person_Feature;

	typedef struct {
		std::string base64Pic;
		std::string base64PicProbe;
		std::string groupId;
		std::string personId;
		int personSize;
		std::vector<Person_Feature> personFeatures;
	}Type_Data;

	typedef struct {
		std::string typeCode;
		std::string imagePath;
		std::string uuid;
		std::string url;
		Type_Data typeData;
	}Send_Data, *LPSend_Data;

	typedef struct {
		signed int imageX;
		signed int imageY;
		signed int imageW;
		signed int imageH;
		signed int faceOrient;
		std::string features;
		signed int featureSize;
		std::string personId;
		std::string type;
		signed int similarScore;
	}Face_Item;

	typedef struct {
		signed int faceNum;
		signed int similarScore;
		std::vector<Face_Item> faceInfos;
	}Result_Msg;

	typedef struct {
		const char* resCode;
		const char* errorMsg;
		signed int type;
		std::string uuid;
		Result_Msg resultMsg;
		long takingTime;
	}Accept_Data, *LPAccept_Data;
#ifdef __cplusplus
}
#endif
#endif

