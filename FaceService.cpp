#include "FaceService.h"
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex0 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex4 = PTHREAD_MUTEX_INITIALIZER;

FaceService::FaceService()
{
}
FaceService::~FaceService()
{
}

std::vector<FACEGROUP>     groupsbak;
std::vector<GROUPQUEUE>     groupQueues;

std::vector<string> stringToSpilt(std::string str)
{
	std::string::size_type pos;
	const char *pattern = ",";
	int size = str.size();
	int index = 0;
	std::vector<string> ret;
	for (int i = 0; i<size; i++)
	{
		pos = str.find(pattern, i);
		if (pos<size)
		{
			std::string s = str.substr(i, pos - i);
			ret.push_back(s);
			i = pos;
			index++;
		}
	}
	return ret;
}

std::string readFile(string path)
{
	if (dirExists(path))
	{
		std::ifstream in(path);
		if (in.is_open())
		{
			std::ostringstream tmp;
			tmp << in.rdbuf();
			in.close();
			return tmp.str();
		}
	}
	return "";
}

void saveFile(string path, std::string content)
{
	createDirectory(path);
	ofstream out(path);
	if (out.is_open())
	{
		out << content;
		out.close();
	}
}

void delFile(string dirName_in)
{
	if (dirExists(dirName_in))
	{
		remove(dirName_in.c_str());
	}
}

string getFaceFilePath(std::string fileName)
{
	return "./data/" + fileName + ".bak";
}

void FaceService::addQueue(string personId, string groupId, int type)
{
	GROUPQUEUE queue;
	queue.groupId = groupId;
	queue.personId = personId;
	queue.type = type;
	groupQueues.push_back(queue);
}

void FaceService::delPerson(std::string personId, std::string groupId)
{
	for (int i = 0; i < groups.size(); i++)
	{
		if (groups[i].groupId.compare(groupId) == 0)
		{
			for (std::vector<FACEPERSON>::iterator iter = groups[i].persons.begin(); iter != groups[i].persons.end();)
			{
				if (iter->personId == personId)
				{
					addQueue(personId, groupId, 1);
					groups[i].persons.erase(iter);
				}
				else
				{
					iter++;
				}
			}
		}
	}
	delFile(getFaceFilePath(groupId + "/" + personId));
}

void savePerson(std::string groupId, Face_Item personFeature)
{
	PersonFeatures personFeatures;
	personFeatures.featureSize = personFeature.featureSize;
	personFeatures.features = personFeature.features;
	std::string personJson = PersonToJson(personFeatures);
	saveFile(getFaceFilePath(groupId + "/" + personFeature.personId), personJson);
}

void savePerson(std::string groupId, Person_Feature personFeature)
{
	PersonFeatures personFeatures;
	personFeatures.featureSize = personFeature.featureSize;
	personFeatures.features = personFeature.features;
	std::string personJson = PersonToJson(personFeatures);
	saveFile(getFaceFilePath(groupId + "/" + personFeature.personId), personJson);
}

void savePerson(std::string personId, std::string groupId, FACEPERSON person)
{
	PersonFeatures personFeatures;
	personFeatures.featureSize = person.faceModel.lFeatureSize;
	personFeatures.features = byteToInts(person.faceModel.pbFeature, person.faceModel.lFeatureSize);
	std::string personJson = PersonToJson(personFeatures);
	saveFile(getFaceFilePath(groupId + "/" + personId), personJson);
}

void saveGroup(std::vector<FACEGROUP> groups)
{
	InitGroups initGroups;
	initGroups.groupSize = groups.size();
	for (int i = 0; i < initGroups.groupSize; i++)
	{
		GroupPersonIds groupPerson;
		groupPerson.groupId = groups[i].groupId;
		for (int j = 0; j < groups[i].persons.size(); j++)
		{
			groupPerson.personIds.append(groups[i].persons[j].personId).append(",");
		}
		initGroups.groupPersons.push_back(groupPerson);
	}
	std::string groupsJson = GroupsToJson(initGroups);
	saveFile(getFaceFilePath(GROUPFILE), groupsJson);
}

FACEPERSON FaceService::getPerson(FACEGROUP group, std::string personId)
{
	for (int i = 0; i < group.persons.size(); i++)
	{
		if (group.persons[i].personId.compare(personId) == 0)
			return group.persons[i];
	}
	FACEPERSON person;
	person.personId = personId;
	return person;
}

void FaceService::updatePerson(std::string personId, FACEPERSON person, std::string groupId)
{
	savePerson(personId, groupId, person);
	for (int i = 0; i < groups.size(); i++)
	{
		if (groups[i].groupId.compare(groupId) == 0)
		{
			for (int j = 0; j < groups[i].persons.size(); j++)
			{
				if (groups[i].persons[j].personId.compare(personId) == 0)
				{
					groups[i].persons[j] = person;
					return;
				}
			}
			addQueue(personId, groupId, 0);
			groups[i].persons.push_back(person);
			return;
		}
	}
	FACEGROUP group;
	group.groupId = groupId;
	group.persons.push_back(person);
	groups.push_back(group);
	addQueue(personId, groupId,0);

}

FACEGROUP FaceService::getGroup(std::string groupId)
{
	for (int i = 0; i < groups.size(); i++)
	{
		if (groups[i].groupId.compare(groupId) == 0)
			return groups[i];
	}
	FACEGROUP group;
	group.groupId = groupId;
	return group;
}

bool dirExists(string dirName_in)
{
	int ftyp = access(dirName_in.c_str(), 0);

	if (0 == ftyp)
		return true;   // this is a directory!  
	else
		return false;    // this is not a directory!  
}

void* start_group_queue(void* arg)
{
	while (true)
	{
		int size= groupQueues.size();
		if (size > 0)
		{
			for (std::vector<GROUPQUEUE>::iterator iter = groupQueues.begin(); iter != groupQueues.end();)
			{
				bool isiter = true;
				for (int i = 0; i < groupsbak.size(); i++)
				{
					if (groupsbak[i].groupId==iter->groupId)
					{
						for (std::vector<FACEPERSON>::iterator piter = groupsbak[i].persons.begin(); piter != groupsbak[i].persons.end();)
						{
							if (piter->personId == iter->personId)
							{
								if (iter->type == 1)
								{
									groupsbak[i].persons.erase(piter);
								}
								isiter = false;
								break;
							}
							else
							{
								piter++;
							}
						}
						if(iter->type == 0&& isiter)
						{
							FACEPERSON person;
							person.personId = iter->personId;
							groupsbak[i].persons.push_back(person);
							isiter = false;
						}
						break;
					}
				}
				if (iter->type == 0 && isiter)
				{
					FACEGROUP group;
					group.groupId = iter->groupId;
					FACEPERSON person;
					person.personId = iter->personId;
					group.persons.push_back(person);
					groupsbak.push_back(group);
				}
				groupQueues.erase(iter);
			}
			saveGroup(groupsbak);
		}
		else
		{
			usleep(1000);
		}
	}
}

FREngine* mFREngine0;
FREngine* mFREngine1;
FREngine* mFREngine2;
FREngine* mFREngine3;
FREngine* mFREngine4;
FACEARG* arg0;
FACEARG* arg1;
FACEARG* arg2;
FACEARG* arg3;
FACEARG* arg4;
FACE_RESULT faceResult0;
FACE_RESULT faceResult1;
FACE_RESULT faceResult3;
FACE_RESULT faceResult4;
FACE_RESULT faceResult2;

int FaceService::init()
{
	// 初始化引擎
	int ret = 0;
	mFDEngine = new FDEngine();
	ret = mFDEngine->init();
	if (ret != MOK)
	{
		printf("初始化FD引擎失败,错误码: %d, 程序将关闭!", ret);
		uninit();
		system("pause");
		exit(1);
	}
	mFREngine = new FREngine();
	ret = mFREngine->init();
	if (ret != MOK)
	{
		printf("初始化FR引擎失败,错误码: %d, 程序将关闭!", ret);
		uninit();
		system("pause");
		exit(1);
	}
	//mFREngine0 = new FREngine();
	//ret = mFREngine0->init();

	//mFREngine1 = new FREngine();
	//ret = mFREngine1->init();

	//mFREngine2 = new FREngine();
	//ret = mFREngine2->init();
	//mFREngine3 = new FREngine();
	//ret = mFREngine3->init();
	//mFREngine4 = new FREngine();
	//ret = mFREngine4->init();

	//pthread_t  thread0;
	//pthread_t  thread1;
	//pthread_t  thread2;
	//pthread_t  thread3;
	//pthread_t  thread4;
	//arg0 = new FACEARG();
	//arg1 = new FACEARG();
	//arg2 = new FACEARG();
	//arg3 = new FACEARG();
	//arg4 = new FACEARG();
	//faceResult1.success = 1;
	//faceResult0.success = 1;
	//faceResult2.success = 1;
	//faceResult3.success = 1;
	//faceResult4.success = 1;
	//int connectfd=1;
	//pthread_create(&thread0, NULL, start_faceRecognition0, (void*)connectfd);
	//pthread_create(&thread1, NULL, start_faceRecognition1, (void*)connectfd);
	//pthread_create(&thread2, NULL, start_faceRecognition2, (void*)connectfd);
	//pthread_create(&thread3, NULL, start_faceRecognition3, (void*)connectfd);
	//pthread_create(&thread4, NULL, start_faceRecognition4, (void*)connectfd);

	mAgeEngine = new AgeEngine();
	ret = mAgeEngine->init();
	if (ret != MOK)
	{
		printf("初始化Age引擎失败,错误码: %d, 程序将关闭!", ret);
		uninit();
		system("pause");
		exit(1);
	}
	mGenderEngine = new GenderEngine();
	ret = mGenderEngine->init();
	if (ret != MOK)
	{
		printf("初始化Gender引擎失败,错误码: %d, 程序将关闭!", ret);
		uninit();
		system("pause");
		exit(1);
	}
	std::string groupJson = readFile(getFaceFilePath(GROUPFILE));
	if (groupJson != "")
	{
		InitGroups initGroups;
		TranslateGroupJson(groupJson.c_str(), &initGroups);
		for (int i = 0; i < initGroups.groupPersons.size(); i++)
		{
			FACEGROUP group;
			FACEGROUP groupbak;
			group.groupId = initGroups.groupPersons[i].groupId;
			groupbak.groupId = initGroups.groupPersons[i].groupId;
			std::vector<string> personIds = stringToSpilt(initGroups.groupPersons[i].personIds);
			for (int j = 0; j < personIds.size(); j++)
			{
				std::string personJson = readFile(getFaceFilePath(group.groupId + "/" + personIds[j]));
				if (personJson != "")
				{
					PersonFeatures person;
					TranslatePersonJson(personJson.c_str(), &person);
					FACEPERSON facePerson;
					FACEPERSON facePersonbak;
					facePerson.personId = personIds[j];
					facePersonbak.personId = personIds[j];
					facePerson.faceModel.lFeatureSize = person.featureSize;
					facePerson.faceModel.pbFeature = new MByte[person.featureSize];
					IntsTobyte(person.features, facePerson.faceModel.pbFeature, person.featureSize);
					group.persons.push_back(facePerson);
					groupbak.persons.push_back(facePersonbak);
				}
			}
			groups.push_back(group);
			groupsbak.push_back(groupbak);
		}
	}
	pthread_t  thread;
	if (pthread_create(&thread, NULL, start_group_queue, (void*)this)) {        //创建线程，以客户端连接为参数，start_routine为线程执行函数
		perror("Pthread_create() error");
		exit(1);
	}
	return ret;
}
void FaceService::uninit()
{
	if (mFDEngine != nullptr)
	{
		mFDEngine->uninit();
		delete mFDEngine;
		mFDEngine = nullptr;
	}
	if (mFREngine != nullptr)
	{
		mFREngine->uninit();
		delete mFREngine;
		mFREngine = nullptr;
	}
	if (mAgeEngine != nullptr)
	{
		mAgeEngine->uninit();
		delete mAgeEngine;
		mAgeEngine = nullptr;
	}
	if (mGenderEngine != nullptr)
	{
		mGenderEngine->uninit();
		delete mGenderEngine;
		mGenderEngine = nullptr;
	}
}

void FaceService::makeSendData(std::string sendJsonData, int connectfd, int index)
{
	Send_Data sendData;
	TranslateJson(sendJsonData.c_str(), &sendData);
	recordLog("Message typeCode: " + sendData.typeCode);
	std::string json;
	if (sendData.typeCode == FACEDETECT)
	{
		json = faceDetect(sendData);
	}
	else if (sendData.typeCode == FACEADD)
	{
		json = faceAdd(sendData);
	}
	else if (sendData.typeCode == FACEUPDATE)
	{
		json = faceUpdate(sendData);
	}
	else if (sendData.typeCode == FACEDEL)
	{
		json = faceDel(sendData);
	}
	else if (sendData.typeCode == FACERECOGNITION)
	{
		json = faceRecognition(sendData);
	}
	else if (sendData.typeCode == FACEINIT)
	{
		json = faceInit(sendData);
	}
	if(!sendData.url.empty())
		sendUrl(json, sendData.url);
	//string key = getstring(connectfd)+"_"+getstring(index);
	//connect_data[key] = json;
}

std::string FaceService::makeSendData(std::string sendJsonData)
{
	Send_Data sendData;
	TranslateJson(sendJsonData.c_str(), &sendData);
	recordLog("Message typeCode: "+ sendData.typeCode);
	std::string json;
	if (sendData.typeCode == FACEDETECT)
	{
		json = faceDetect(sendData);
	}
	else if (sendData.typeCode == FACEADD)
	{
		json = faceAdd(sendData);
	}
	else if (sendData.typeCode == FACEUPDATE)
	{
		json = faceUpdate(sendData);
	}
	else if (sendData.typeCode == FACEDEL)
	{
		json = faceDel(sendData);
	}
	else if (sendData.typeCode == FACERECOGNITION)
	{
		json = faceRecognition(sendData);
	}
	else if (sendData.typeCode == FACEINIT)
	{
		json = faceInit(sendData);
	}
	return json;
}

bool readImage(Accept_Data* acceptData, ASVLOFFSCREEN* offInput, string base64Pic)
{
	acceptData->errorMsg = "none";
	ZBase64 base64;
	int blength;
	vector<uchar> dvecImg = base64.Decode(base64Pic.c_str(), base64Pic.length(), blength);
	Mat tmp = imdecode(dvecImg, CV_LOAD_IMAGE_COLOR);
	offInput->u32PixelArrayFormat = ASVL_PAF_RGB24_B8G8R8;
	offInput->i32Width = tmp.cols;
	offInput->i32Height = tmp.rows;
	MUInt8* ppu8Plane = (MUInt8*)malloc(tmp.cols*tmp.rows*tmp.channels() * sizeof(uchar));
	if (!ppu8Plane) {
		recordLog("readImage read image fail");
		acceptData->resCode = "1";
		acceptData->errorMsg = "read image fail";
		return false;
	}
	memcpy(ppu8Plane, tmp.data, tmp.cols*tmp.rows*tmp.channels() * sizeof(uchar));
	if (!ppu8Plane)
	{
		recordLog("readImage2 read image fail");
		acceptData->resCode = "1";
		acceptData->errorMsg = "read image fail";
		return false;
	}
	offInput->ppu8Plane[0] = ppu8Plane;
	if (ASVL_PAF_I420 == offInput->u32PixelArrayFormat) {
		offInput->pi32Pitch[0] = offInput->i32Width;
		offInput->pi32Pitch[1] = offInput->i32Width / 2;
		offInput->pi32Pitch[2] = offInput->i32Width / 2;
		offInput->ppu8Plane[1] = offInput->ppu8Plane[0] + offInput->pi32Pitch[0] * offInput->i32Height;
		offInput->ppu8Plane[2] = offInput->ppu8Plane[1] + offInput->pi32Pitch[1] * offInput->i32Height / 2;
	}
	else if (ASVL_PAF_RGB24_B8G8R8 == offInput->u32PixelArrayFormat) {
		offInput->pi32Pitch[0] = offInput->i32Width * 3;
	}
	tmp.release();
	// 读取图片数据信息存入ASVLOFFSCREEN结构体
	return true;
}

std::string FaceService::faceDetect(Send_Data sendData)
{
	Accept_Data acceptData;
	acceptData.uuid = sendData.uuid;
	acceptData.resCode = "0";
	acceptData.errorMsg = "none";
	ASVLOFFSCREEN offInput = { 0 };
	if (!readImage(&acceptData, &offInput, sendData.typeData.base64Pic))
	{
		return DataToJson(acceptData);
	}
	LPAFD_FSDK_FACERES faceRes;
	FDEngine tempFDEngine;
	int fdret = tempFDEngine.init();
	if (fdret != 0) {
		fprintf(stderr, "fail to AFD_FSDK_InitialFaceEngine(): 0x%x\r\n", fdret);
		acceptData.resCode = "1";
		acceptData.errorMsg = "FDEngine except!";
	}
	int mRet = tempFDEngine.FaceDetection(&offInput, &faceRes);
	if (faceRes->nFace <= 0 || mRet != MOK)
	{
		recordLog("image no face");
		acceptData.resultMsg.faceNum = faceRes->nFace;
		acceptData.resCode = "1";
		acceptData.errorMsg = "image no face";
		free(offInput.ppu8Plane[0]);
		return DataToJson(acceptData);
	}
	acceptData.resultMsg.faceNum = faceRes->nFace;
	for (int i = 0; i < faceRes->nFace; i++)
	{
		Face_Item faceItem;
		faceItem.imageX = faceRes->rcFace[i].left;
		faceItem.imageY = faceRes->rcFace[i].top;
		faceItem.imageW = faceRes->rcFace[i].right - faceRes->rcFace[i].left;
		faceItem.imageH = faceRes->rcFace[i].bottom - faceRes->rcFace[i].top;
		faceItem.faceOrient = faceRes->lfaceOrient[i];
		acceptData.resultMsg.faceInfos.push_back(faceItem);
	}
	free(offInput.ppu8Plane[0]);
	return DataToJson(acceptData);
}

std::string byteToInts(BYTE *b, int len)
{
	std::string s;
	for (int i = 0; i < len; i++)
	{
		s.append(to_string(b[i]));
		s.append(",");
	}
	return s;
}

void IntsTobyte(std::string str, BYTE *b, int len)
{
	std::string::size_type pos;
	const char *pattern = ",";
	int size = str.size();
	int index = 0;
	for (int i = 0; i<size; i++)
	{
		pos = str.find(pattern, i);
		if (pos<size)
		{
			std::string s = str.substr(i, pos - i);
			b[index] = atoi(s.c_str());
			i = pos;
			index++;
		}
	}
}

std::string FaceService::faceAdd(Send_Data sendData)
{
	Accept_Data acceptData;
	acceptData.uuid = sendData.uuid;
	acceptData.resCode = "0";
	acceptData.errorMsg = "none";
	ASVLOFFSCREEN offInput = { 0 };
	if (!readImage(&acceptData, &offInput, sendData.typeData.base64Pic))
	{
		return DataToJson(acceptData);
	}
	LPAFD_FSDK_FACERES faceRes;
	FDEngine tempFDEngine;
	int fdret = tempFDEngine.init();
	if (fdret != 0) {
		fprintf(stderr, "fail to AFD_FSDK_InitialFaceEngine(): 0x%x\r\n", fdret);
		acceptData.resCode = "1";
		acceptData.errorMsg = "FDEngine except!";
	}
	int mRet = tempFDEngine.FaceDetection(&offInput, &faceRes);
	if (faceRes->nFace <= 0 || mRet != MOK)
	{
		recordLog("faceAdd image no face");
		acceptData.resultMsg.faceNum = faceRes->nFace;
		acceptData.resCode = "1";
		acceptData.errorMsg = "image no face";
		free(offInput.ppu8Plane[0]);
		return DataToJson(acceptData);
	}
	//标识返回的json格式结构体类型
	acceptData.type = 1;
	acceptData.resultMsg.faceNum = faceRes->nFace;
	for (int i = 0; i < faceRes->nFace; i++)
	{
		Face_Item faceItem;
		faceItem.imageX = faceRes->rcFace[i].left;
		faceItem.imageY = faceRes->rcFace[i].top;
		faceItem.imageW = faceRes->rcFace[i].right - faceRes->rcFace[i].left;
		faceItem.imageH = faceRes->rcFace[i].bottom - faceRes->rcFace[i].top;
		faceItem.faceOrient = faceRes->lfaceOrient[i];
		acceptData.resultMsg.faceInfos.push_back(faceItem);
	}
	if (acceptData.resultMsg.faceNum > 1)
	{
		acceptData.resCode = "1";
		acceptData.errorMsg = "do not more face";
		free(offInput.ppu8Plane[0]);
		return DataToJson(acceptData);
	}
	FACEGROUP group = getGroup(sendData.typeData.groupId);
	FREngine tempFREngine;
	int frdret = tempFREngine.init();
	if (frdret != 0) {
		fprintf(stderr, "fail to tempFREngine init: 0x%x\r\n", frdret);
		acceptData.resCode = "1";
		acceptData.errorMsg = "FREngine except!";
	}
	// 对检测到的人脸进行FR人脸特征提取，将提取的人脸特征信息存入人脸库
	for (int i = 0; i < faceRes->nFace; i++)
	{
		FACEPERSON person;
		person.personId = sendData.typeData.personId;
		AFR_FSDK_FACEMODEL LocalFaceModels;
		AFR_FSDK_FACEINPUT faceLocation;
		faceLocation.rcFace = faceRes->rcFace[i];
		faceLocation.lOrient = faceRes->lfaceOrient[i];
		mRet = tempFREngine.ExtractFRFeature(&offInput, &faceLocation, &LocalFaceModels);
		if (mRet != MOK)
		{
			recordLog("faceAdd FRFeature fail");
			acceptData.resCode = "1";
			acceptData.errorMsg = "FRFeature fail";
			free(offInput.ppu8Plane[0]);
			return DataToJson(acceptData);
		}
		person.faceModel.lFeatureSize = LocalFaceModels.lFeatureSize;
		person.faceModel.pbFeature = new MByte[person.faceModel.lFeatureSize];
		//LocalFaceModels.pbFeature指向人脸特征信息，将其数据拷贝到人脸库，如果不拷贝出来，用相同的引擎进行特征提取时会覆盖上一次得到的人脸特征信息
		memcpy(person.faceModel.pbFeature, LocalFaceModels.pbFeature, person.faceModel.lFeatureSize);
		acceptData.resultMsg.faceInfos[i].featureSize = LocalFaceModels.lFeatureSize;
		acceptData.resultMsg.faceInfos[i].features = byteToInts(person.faceModel.pbFeature, LocalFaceModels.lFeatureSize);
		acceptData.resultMsg.faceInfos[i].personId = sendData.typeData.personId;
		updatePerson(person.personId, person, sendData.typeData.groupId);
		group.persons.push_back(person);
	}
	free(offInput.ppu8Plane[0]);
	return DataToJson(acceptData);
}

std::string FaceService::faceUpdate(Send_Data sendData)
{
	Accept_Data acceptData;
	acceptData.uuid = sendData.uuid;
	acceptData.resCode = "0";
	acceptData.errorMsg = "none";
	ASVLOFFSCREEN offInput = { 0 };
	if (!readImage(&acceptData, &offInput, sendData.typeData.base64Pic))
	{
		return DataToJson(acceptData);
	}
	LPAFD_FSDK_FACERES faceRes;
	FDEngine tempFDEngine;
	int fdret = tempFDEngine.init();
	if (fdret != 0) {
		fprintf(stderr, "fail to AFD_FSDK_InitialFaceEngine(): 0x%x\r\n", fdret);
		acceptData.resCode = "1";
		acceptData.errorMsg = "FDEngine except!";
	}
	int mRet = tempFDEngine.FaceDetection(&offInput, &faceRes);
	if (faceRes->nFace <= 0 || mRet != MOK)
	{
		recordLog("faceUpdate image no face");
		acceptData.resultMsg.faceNum = faceRes->nFace;
		acceptData.resCode = "1";
		acceptData.errorMsg = "image no face";
		free(offInput.ppu8Plane[0]);
		return DataToJson(acceptData);
	}
	//标识返回的json格式结构体类型
	acceptData.type = 1;
	acceptData.resultMsg.faceNum = faceRes->nFace;
	for (int i = 0; i < faceRes->nFace; i++)
	{
		Face_Item faceItem;
		faceItem.imageX = faceRes->rcFace[i].left;
		faceItem.imageY = faceRes->rcFace[i].top;
		faceItem.imageW = faceRes->rcFace[i].right - faceRes->rcFace[i].left;
		faceItem.imageH = faceRes->rcFace[i].bottom - faceRes->rcFace[i].top;
		faceItem.faceOrient = faceRes->lfaceOrient[i];
		acceptData.resultMsg.faceInfos.push_back(faceItem);
	}
	if (acceptData.resultMsg.faceNum > 1)
	{
		acceptData.resCode = "1";
		acceptData.errorMsg = "donot more face";
		free(offInput.ppu8Plane[0]);
		return DataToJson(acceptData);
	}
	FACEGROUP group = getGroup(sendData.typeData.groupId);
	FREngine tempFREngine;
	int frdret = tempFREngine.init();
	if (fdret != 0) {
		fprintf(stderr, "fail to tempFREngine init: 0x%x\r\n", frdret);
		acceptData.resCode = "1";
		acceptData.errorMsg = "FREngine except!";
	}
	// 对检测到的人脸进行FR人脸特征提取，将提取的人脸特征信息存入人脸库
	for (int i = 0; i < faceRes->nFace; i++)
	{
		FACEPERSON person = getPerson(group, sendData.typeData.personId);
		AFR_FSDK_FACEMODEL LocalFaceModels;
		AFR_FSDK_FACEINPUT faceLocation;
		faceLocation.rcFace = faceRes->rcFace[i];
		faceLocation.lOrient = faceRes->lfaceOrient[i];
		mRet = tempFREngine.ExtractFRFeature(&offInput, &faceLocation, &LocalFaceModels);
		if (mRet != MOK)
		{
			recordLog("faceUpdate FRFeature fail");
			acceptData.resCode = "1";
			acceptData.errorMsg = "FRFeature fail";
			free(offInput.ppu8Plane[0]);
			return DataToJson(acceptData);
		}
		person.faceModel.lFeatureSize = LocalFaceModels.lFeatureSize;
		person.faceModel.pbFeature = new MByte[person.faceModel.lFeatureSize];
		//LocalFaceModels.pbFeature指向人脸特征信息，将其数据拷贝到人脸库，如果不拷贝出来，用相同的引擎进行特征提取时会覆盖上一次得到的人脸特征信息
		memcpy(person.faceModel.pbFeature, LocalFaceModels.pbFeature, person.faceModel.lFeatureSize);
		acceptData.resultMsg.faceInfos[i].featureSize = LocalFaceModels.lFeatureSize;
		acceptData.resultMsg.faceInfos[i].features = byteToInts(person.faceModel.pbFeature, LocalFaceModels.lFeatureSize);
		updatePerson(person.personId, person, sendData.typeData.groupId);
	}
	free(offInput.ppu8Plane[0]);
	return DataToJson(acceptData);
}

std::string FaceService::faceDel(Send_Data sendData)
{
	Accept_Data acceptData;
	acceptData.uuid = sendData.uuid;
	acceptData.resCode = "0";
	acceptData.errorMsg = "none";
	//标识返回的json格式结构体类型
	acceptData.type = 0;
	FACEGROUP group = getGroup(sendData.typeData.groupId);
	std::string::size_type pos;
	const char *pattern = ",";
	int size = sendData.typeData.personId.size();
	int index = 0;
	vector<string> persons;
	for (int i = 0; i<size; i++)
	{
		pos = sendData.typeData.personId.find(pattern, i);
		if (pos<size)
		{
			std::string s = sendData.typeData.personId.substr(i, pos - i);
			persons.push_back(s);
			i = pos;
			index++;
		}
	}
	vector<FACEPERSON>::iterator it = group.persons.begin();
	for (; it != group.persons.end();)
	{
		bool ret = true;
		for (size_t i = 0; i < persons.size(); i++)
		{
			if (it->personId == persons.at(i))
			{
				delPerson(it->personId, group.groupId);
				ret = false;
				//删除指定元素，返回指向删除元素的下一个元素的位置的迭代器
				it = group.persons.erase(it);
			}
		}
		if (ret)
			++it;
	}
	return DataToJson(acceptData);
}

void* start_faceRecognition0(void* arg)
{
	pthread_detach(pthread_self());
	pthread_mutex_lock(&mutex0);
	TYPEINT connectfd;
	connectfd = (TYPEINT)arg;
	while (true)
	{
		if (faceResult0.success == 0)
		{
			vector<FACEPERSON> persons = arg0->group.persons;
			float maxScore = 0;
			for (int i = arg0->start; i<arg0->end; i++)
			{
				float fSimilScore = 0.0f;
				mFREngine0->FacePairMatching(&(persons[i].faceModel), &(arg0->LocalFaceModels), &fSimilScore);
				if (fSimilScore >= maxScore)
				{
					//可以指定超过识别率才返回55
					maxScore = fSimilScore;
					faceResult0.similarScore = maxScore;
					faceResult0.personId = persons[i].personId;
				}
			}
			faceResult0.success = 1;
		}
		usleep(1);
	}
	pthread_mutex_unlock(&mutex0);
	pthread_exit(NULL);
}

void* start_faceRecognition1(void* arg)
{
	pthread_detach(pthread_self());
	pthread_mutex_lock(&mutex1);
	TYPEINT connectfd;
	connectfd = (TYPEINT)arg;
	while (true)
	{
		if (faceResult1.success == 0)
		{
			vector<FACEPERSON> persons = arg1->group.persons;
			float maxScore = 0;
			for (int i = arg1->start; i<arg1->end; i++)
			{
				float fSimilScore = 0.0f;
				mFREngine1->FacePairMatching(&(persons[i].faceModel), &(arg1->LocalFaceModels), &fSimilScore);
				if (fSimilScore >= maxScore)
				{
					//可以指定超过识别率才返回55
					maxScore = fSimilScore;
					faceResult1.similarScore = maxScore;
					faceResult1.personId = persons[i].personId;
				}
			}
			faceResult1.success = 1;
		}
		usleep(1);
	}
	pthread_mutex_unlock(&mutex1);
	pthread_exit(NULL);
}

void* start_faceRecognition2(void* arg)
{
	pthread_detach(pthread_self());
	pthread_mutex_lock(&mutex2);
	TYPEINT connectfd;
	connectfd = (TYPEINT)arg;
	while (true)
	{
		if (faceResult2.success == 0)
		{
			vector<FACEPERSON> persons = arg2->group.persons;
			float maxScore = 0;
			for (int i = arg2->start; i<arg2->end; i++)
			{
				float fSimilScore = 0.0f;
				mFREngine2->FacePairMatching(&(persons[i].faceModel), &(arg2->LocalFaceModels), &fSimilScore);
				if (fSimilScore >= maxScore)
				{
					//可以指定超过识别率才返回55
					maxScore = fSimilScore;
					faceResult2.similarScore = maxScore;
					faceResult2.personId = persons[i].personId;
				}
			}
			faceResult2.success = 1;
		}
		usleep(1);
	}
	pthread_mutex_unlock(&mutex2);
	pthread_exit(NULL);
}

void* start_faceRecognition3(void* arg)
{
	pthread_detach(pthread_self());
	pthread_mutex_lock(&mutex3);
	TYPEINT connectfd;
	connectfd = (TYPEINT)arg;
	while (true)
	{
		if (faceResult3.success == 0)
		{
			vector<FACEPERSON> persons = arg3->group.persons;
			float maxScore = 0;
			for (int i = arg3->start; i<arg3->end; i++)
			{
				float fSimilScore = 0.0f;
				mFREngine3->FacePairMatching(&(persons[i].faceModel), &(arg3->LocalFaceModels), &fSimilScore);
				if (fSimilScore >= maxScore)
				{
					//可以指定超过识别率才返回55
					maxScore = fSimilScore;
					faceResult3.similarScore = maxScore;
					faceResult3.personId = persons[i].personId;
				}
			}
			faceResult3.success = 1;
		}
		usleep(1);
	}
	pthread_mutex_unlock(&mutex3);
	pthread_exit(NULL);
}

void* start_faceRecognition4(void* arg)
{
	pthread_detach(pthread_self());
	pthread_mutex_lock(&mutex4);
	TYPEINT connectfd;
	connectfd = (TYPEINT)arg;
	while (true)
	{
		if (faceResult4.success == 0)
		{
			vector<FACEPERSON> persons = arg4->group.persons;
			float maxScore = 0;
			for (int i = arg4->start; i<arg4->end; i++)
			{
				float fSimilScore = 0.0f;
				mFREngine4->FacePairMatching(&(persons[i].faceModel), &(arg4->LocalFaceModels), &fSimilScore);
				if (fSimilScore >= maxScore)
				{
					//可以指定超过识别率才返回55
					maxScore = fSimilScore;
					faceResult4.similarScore = maxScore;
					faceResult4.personId = persons[i].personId;
				}
			}
			faceResult4.success = 1;
		}
		usleep(1);
	}
	pthread_mutex_unlock(&mutex4);
	pthread_exit(NULL);
}

std::string FaceService::faceRecognition(Send_Data sendData)
{
	pthread_mutex_lock(&mutex);
	clock_t start_time = clock();
	Accept_Data acceptData;
	acceptData.uuid = sendData.uuid;
	acceptData.resCode = "0";
	acceptData.errorMsg = "none";
	ASVLOFFSCREEN offInput = { 0 };
	//标识返回的json格式结构体类型
	acceptData.type = 2;
	if (readImage(&acceptData, &offInput, sendData.typeData.base64Pic))
	{
		LPAFD_FSDK_FACERES faceRes;
		int mRet = mFDEngine->FaceDetection(&offInput, &faceRes);
		if (faceRes->nFace <= 0 || mRet != MOK)
		{
			recordLog("FaceDetection image no face! mRet:" + getstring(mRet));
			acceptData.resultMsg.faceNum = faceRes->nFace;
			acceptData.resCode = "1";
			acceptData.errorMsg = "no face!";
		}
		else
		{
			acceptData.resultMsg.faceNum = faceRes->nFace;
			for (int i = 0; i < faceRes->nFace; i++)
			{
				Face_Item faceItem;
				faceItem.imageX = faceRes->rcFace[i].left;
				faceItem.imageY = faceRes->rcFace[i].top;
				faceItem.imageW = faceRes->rcFace[i].right - faceRes->rcFace[i].left;
				faceItem.imageH = faceRes->rcFace[i].bottom - faceRes->rcFace[i].top;
				faceItem.faceOrient = faceRes->lfaceOrient[i];
				faceItem.similarScore = 0;
				acceptData.resultMsg.faceInfos.push_back(faceItem);
			}
			FACEGROUP group = getGroup(sendData.typeData.groupId);
			// 对检测到的人脸进行FR人脸特征提取，将提取的人脸特征信息存入人脸库
			for (int i = 0; i < faceRes->nFace; i++)
			{
				FACEPERSON person;
				person.personId = sendData.typeData.personId;
				AFR_FSDK_FACEMODEL LocalFaceModels;
				AFR_FSDK_FACEINPUT faceLocation;
				faceLocation.rcFace = faceRes->rcFace[i];
				faceLocation.lOrient = faceRes->lfaceOrient[i];
				mRet = mFREngine->ExtractFRFeature(&offInput, &faceLocation, &LocalFaceModels);
				if (mRet != MOK)
				{
					recordLog("faceRecognition FRFeature fail mRet:" + getstring(mRet));
					acceptData.resCode = "1";
					acceptData.errorMsg = "FRFeature fail";
					break;
				}
				person.faceModel.lFeatureSize = LocalFaceModels.lFeatureSize;
				person.faceModel.pbFeature = new MByte[person.faceModel.lFeatureSize];
				memcpy(person.faceModel.pbFeature, LocalFaceModels.pbFeature, person.faceModel.lFeatureSize);
				float maxScore = 0;



				/*arg0->start = 0;
				arg0->end = group.persons.size() / 5;
				arg0->LocalFaceModels = LocalFaceModels;
				arg0->group = group;
				arg1->start = group.persons.size() / 5;
				arg1->end = group.persons.size()*2/5;
				arg1->LocalFaceModels = LocalFaceModels;
				arg1->group = group;
				arg2->start = group.persons.size()*2 / 5;
				arg2->end = group.persons.size()*3/5;
				arg2->LocalFaceModels = LocalFaceModels;
				arg2->group = group;
				arg3->start = group.persons.size()*3 /5;
				arg3->end = group.persons.size()*4/5;
				arg3->LocalFaceModels = LocalFaceModels;
				arg3->group = group;
				arg4->start = group.persons.size()*4 /5;
				arg4->end = group.persons.size();
				arg4->LocalFaceModels = LocalFaceModels;
				arg4->group = group;
				faceResult1.success = 0;
				faceResult0.success = 0;
				faceResult2.success = 0;
				faceResult3.success = 0;
				faceResult4.success = 0;
				while (true)
				{
					if (faceResult1.success == 1 && faceResult0.success == 1 && faceResult2.success == 1
						&& faceResult3.success == 1 && faceResult4.success == 1)
					{
						FACE_RESULT faceResult;
						if (faceResult1.similarScore > faceResult0.similarScore)
						{
							faceResult.similarScore = faceResult1.similarScore;
							faceResult.personId = faceResult1.personId;
						}
						else
						{
							faceResult.similarScore = faceResult0.similarScore;
							faceResult.personId = faceResult0.personId;
						}
						if (faceResult2.similarScore > faceResult.similarScore)
						{
							faceResult.similarScore = faceResult2.similarScore;
							faceResult.personId = faceResult2.personId;
						}
						if (faceResult3.similarScore > faceResult.similarScore)
						{
							faceResult.similarScore = faceResult3.similarScore;
							faceResult.personId = faceResult3.personId;
						}
						if (faceResult4.similarScore > faceResult.similarScore)
						{
							faceResult.similarScore = faceResult4.similarScore;
							faceResult.personId = faceResult4.personId;
						}
						acceptData.resultMsg.faceInfos[i].similarScore = faceResult.similarScore * 100;
						acceptData.resultMsg.faceInfos[i].personId = faceResult.personId;
						break;
					}
					usleep(1);
				}*/

				for (std::vector<FACEPERSON>::iterator iter = group.persons.begin(); iter != group.persons.end();)
				{
					float fSimilScore = 0.0f;
					mFREngine->FacePairMatching(&(iter->faceModel), &LocalFaceModels, &fSimilScore);
					if (fSimilScore >= maxScore)
					{
						//可以指定超过识别率才返回55
						maxScore = fSimilScore;
						acceptData.resultMsg.faceInfos[i].similarScore = maxScore * 100;
						acceptData.resultMsg.faceInfos[i].personId = iter->personId;
					}
					++iter;
				}
				delete person.faceModel.pbFeature;
			}
		}
		free(offInput.ppu8Plane[0]);
	}
	clock_t end_time = clock();
	acceptData.takingTime=static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC * 1000;
	pthread_mutex_unlock(&mutex);
	return  DataToJson(acceptData);
}
std::string FaceService::faceInit(Send_Data sendData)
{
	Accept_Data acceptData;
	acceptData.uuid = sendData.uuid;
	acceptData.resCode = "0";
	acceptData.errorMsg = "none";
	//标识返回的json格式结构体类型
	acceptData.type = 0;
	FACEGROUP group;
	group.groupId = sendData.typeData.groupId;
	for (std::vector<FACEGROUP>::iterator iter = groupsbak.begin(); iter != groupsbak.end();)
	{
		if (iter->groupId.compare(sendData.typeData.groupId) == 0)
		{
			groupsbak.erase(iter);
		}
		else
		{
			iter++;
		}
	}
	for (int i = 0; i < sendData.typeData.personSize; i++)
	{
		FACEPERSON person;
		Person_Feature personFeature = sendData.typeData.personFeatures[i];
		person.personId = personFeature.personId;
		person.faceModel.lFeatureSize = personFeature.featureSize;
		person.faceModel.pbFeature = new MByte[personFeature.featureSize];
		IntsTobyte(personFeature.features, person.faceModel.pbFeature, personFeature.featureSize);
		savePerson(group.groupId, personFeature);
		addQueue(person.personId, group.groupId,0);
		group.persons.push_back(person);
	}
	bool cr_groups=true;
	for (int j = 0; j < groups.size(); j++)
	{
		if (groups[j].groupId.compare(sendData.typeData.groupId) == 0)
		{
			groups[j] = group;
			cr_groups = false;
		}
	}
	if (cr_groups)
	{
		groups.push_back(group);
	}
	return DataToJson(acceptData);
}