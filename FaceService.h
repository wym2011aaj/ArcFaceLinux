#pragma once
#include "FDEngine.h"
#include "FTEngine.h"
#include "FREngine.h"
#include "AgeEngine.h"
#include "GenderEngine.h"
#include "utils.h"
#include "faceCommon.h"
#include "ZBase64.h" 
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

using std::cerr;
using namespace std;
using namespace cv;

typedef struct {
	std::string personId;
	AFR_FSDK_FACEMODEL faceModel;
} FACEPERSON;

typedef struct {
	std::string groupId;
	std::vector<FACEPERSON>  persons;
} FACEGROUP;

typedef struct {
	int type;//0ÐÂÔö£¬1É¾³ý
	std::string groupId;
	std::string personId;
} GROUPQUEUE;

typedef struct {
	FACEGROUP group;
	int start;
	int end;
	AFR_FSDK_FACEMODEL LocalFaceModels;
}FACEARG;

typedef struct {
	string personId;
	float similarScore;
	int success;
}FACE_RESULT;

std::string byteToInts(BYTE *b, int len);
void IntsTobyte(std::string str, BYTE *b, int len);
bool dirExists(string dirName_in);
bool readImage(Accept_Data* acceptData, ASVLOFFSCREEN* offInput, string sendData);
void saveGroup(std::vector<FACEGROUP> groups);
void savePerson(std::string groupId, Face_Item personFeature);
void savePerson(std::string groupId, Person_Feature personFeature);
void savePerson(std::string personId, std::string groupId, FACEPERSON person);
void delFile(string dirName_in);
void saveFile(string path, std::string content);
std::string readFile(string path);
void* start_faceRecognition0(void* arg);
void* start_faceRecognition1(void* arg);
void* start_faceRecognition2(void* arg);
void* start_faceRecognition3(void* arg);
void* start_faceRecognition4(void* arg);
class FaceService
{
private:
	FACEPERSON getPerson(FACEGROUP group, std::string personId);
	FACEGROUP getGroup(std::string groupId);
	void updatePerson(std::string personId, FACEPERSON person, std::string groupId);
	void delPerson(std::string personId, std::string groupId);
protected:
	FDEngine * mFDEngine;
	FTEngine*				mFTEngine;
	FREngine*				mFREngine;
	AgeEngine*				mAgeEngine;
	GenderEngine*			mGenderEngine;
	std::vector<FACEGROUP>  groups;
public:
	FaceService();
	~FaceService();
	int init();
	void uninit();
	void addQueue(string personId,string groupId,int type);
	void makeSendData(std::string sendJsonData, int connectfd,int index);
	std::string makeSendData(std::string sendJsonData);
	std::string faceDetect(Send_Data sendData);
	std::string faceAdd(Send_Data sendData);
	std::string faceUpdate(Send_Data sendData);
	std::string faceDel(Send_Data sendData);
	std::string faceRecognition(Send_Data sendData);
	std::string faceInit(Send_Data sendData);
};