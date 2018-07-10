#include "utils.h"
#include <ctime>

using std::cout;
using std::endl;

int32_t createDirectory(string directoryPath)
{
	uint32_t dirPathLen = directoryPath.length();
	if (dirPathLen > MAX_PATH_LEN)
	{
		return -1;
	}
	char tmpDirPath[MAX_PATH_LEN] = { 0 };
	for (uint32_t i = 0; i < dirPathLen; ++i)
	{
		tmpDirPath[i] = directoryPath[i];
		if (tmpDirPath[i] == '\\' || tmpDirPath[i] == '/')
		{
			if (ACCESS(tmpDirPath, 0) != 0)
			{
				int32_t ret = MKDIR(tmpDirPath);
				if (ret != 0)
				{
					return ret;
				}
			}
		}
	}
	return 0;
}

string getstring(const int n)
{
	std::stringstream newstr;
	newstr << n;
	return newstr.str();
}

void recordLog(const int n)
{
	string s = getstring(n);
	recordLog(s);
}

void recordLog(string log)
{
	//cout << log << endl;
	char buf[12] = { 0 };
	tm * local;
	time_t now_time;
	now_time = time(NULL);
	local = localtime(&now_time);
	strftime(buf, 12, "%Y%m%d%H", local);
	string path;
	path.append("log/log").append(buf).append(".txt");
	createDirectory(path);
	char buf2[24] = { 0 };
	strftime(buf2, 24, "%Y-%m-%d %H:%M:%S", local);
	ofstream ofile(path, ios::app);//定义输出文件 ios::app表示在原文件末尾追加
	ofile << buf2 << ":" << log << endl;   //标题写入文件
	ofile.close();
}

Value getIntValue(int intValue)
{
	Value _intValue(kNumberType);
	if (intValue >= 0)
		_intValue.SetInt(intValue);
	else
	{
		_intValue.SetNull();
	}
	return _intValue;
}

Value getDoubleValue(long longValue)
{
	Value _intValue(kNumberType);
	if (longValue >= 0)
		_intValue.SetDouble(longValue);
	else
	{
		_intValue.SetNull();
	}
	return _intValue;
}

Value getStringValue(std::string strValue, Document::AllocatorType& allocator)
{
	Value _strValue(kStringType);
	_strValue.SetString(strValue.data(), allocator);
	return _strValue;
}

Value getFaceInfo(Face_Item faceItem, Document::AllocatorType& allocator)
{
	Value faceInfo(kObjectType);
	faceInfo.AddMember("imageX", getIntValue(faceItem.imageX), allocator);
	faceInfo.AddMember("imageY", getIntValue(faceItem.imageY), allocator);
	faceInfo.AddMember("imageH", getIntValue(faceItem.imageH), allocator);
	faceInfo.AddMember("imageW", getIntValue(faceItem.imageW), allocator);
	faceInfo.AddMember("faceOrient", getIntValue(faceItem.faceOrient), allocator);
	faceInfo.AddMember("featureSize", getIntValue(faceItem.featureSize), allocator);
	faceInfo.AddMember("similarScore", getIntValue(faceItem.similarScore), allocator);
	if (faceItem.featureSize > 0)
	{
		faceInfo.AddMember("features", getStringValue(faceItem.features, allocator), allocator);
	}
	faceInfo.AddMember("personId", getStringValue(faceItem.personId, allocator), allocator);
	return faceInfo;
}

Value getResultMsg(Result_Msg resultMsg, Document::AllocatorType& allocator)
{
	Value resultIde1(kObjectType);
	resultIde1.AddMember("faceNum", getIntValue(resultMsg.faceNum), allocator);
	if (resultMsg.faceNum > 0)
	{
		Value faceInfos(kArrayType);
		for (int i = 0; i < resultMsg.faceNum; i++)
		{
			Value faceInfo = getFaceInfo(resultMsg.faceInfos[i], allocator);
			//往数组里放元素
			faceInfos.PushBack(faceInfo, allocator);
		}
		resultIde1.AddMember("faceInfos", faceInfos, allocator);
	}
	return resultIde1;
}

Value getResultMsg1(Result_Msg resultMsg, Document::AllocatorType& allocator)
{
	Value faceInfo(kObjectType);
	faceInfo.AddMember("faceNum", getIntValue(resultMsg.faceNum), allocator);
	Face_Item faceItem = resultMsg.faceInfos[0];
	faceInfo.AddMember("imageX", getIntValue(faceItem.imageX), allocator);
	faceInfo.AddMember("imageY", getIntValue(faceItem.imageY), allocator);
	faceInfo.AddMember("imageH", getIntValue(faceItem.imageH), allocator);
	faceInfo.AddMember("imageW", getIntValue(faceItem.imageW), allocator);
	faceInfo.AddMember("faceOrient", getIntValue(faceItem.faceOrient), allocator);
	faceInfo.AddMember("featureSize", getIntValue(faceItem.featureSize), allocator);
	if (faceItem.featureSize > 0)
	{
		faceInfo.AddMember("features", getStringValue(faceItem.features, allocator), allocator);
	}
	return faceInfo;
}

std::string DataToJson(Accept_Data acceptData)
{
	Document document;
	Document::AllocatorType& allocator = document.GetAllocator();
	//根
	Value root(kObjectType);


	//一个值，类型为string
	Value resCode(kStringType);
	//设置value的值
	resCode.SetString(acceptData.resCode, allocator);
	//将值name放到root中，并用"name"作为key
	root.AddMember("resCode", resCode, allocator);
	if(!acceptData.uuid.empty())
		root.AddMember("uuid", getStringValue(acceptData.uuid, allocator), allocator);
	if (acceptData.takingTime > 0)
		root.AddMember("takingTime", getDoubleValue(acceptData.takingTime), allocator);
	//一个值，类型为string
	Value errorMsg(kStringType);
	//设置value的值
	errorMsg.SetString(acceptData.errorMsg, allocator);
	//将值name放到root中，并用"name"作为key
	root.AddMember("errorMsg", errorMsg, allocator);
	if (acceptData.type == 1)
	{
		Value resultMsg = getResultMsg1(acceptData.resultMsg, allocator);
		root.AddMember("resultMsg", resultMsg, allocator);
	}
	else if (acceptData.type != 0)
	{
		Value resultMsg = getResultMsg(acceptData.resultMsg, allocator);
		root.AddMember("resultMsg", resultMsg, allocator);
	}

	//输出字符串
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	root.Accept(writer);
	std::string reststring = buffer.GetString();
	return reststring;
}

void TranslateJson(const char* strData, LPSend_Data lpsendData)
{
	Document m_doc;
	m_doc.Parse<0>(strData);
	if (m_doc.HasParseError()&& m_doc.HasMember("typeCode"))
	{
		cout << "GetParaseError:" << m_doc.GetParseError() << endl;
	}
	lpsendData->typeCode = m_doc["typeCode"].GetString();
	if (lpsendData->typeCode == FACEDETECT || lpsendData->typeCode == FACEADD || lpsendData->typeCode == FACEUPDATE || lpsendData->typeCode == FACERECOGNITION
		|| lpsendData->typeCode == FACEFEATURE || lpsendData->typeCode == FACECOMPARISON)
	{
		lpsendData->typeData.base64Pic = m_doc["typeData"]["base64Pic"].GetString();
	}
	if (lpsendData->typeCode == FACECOMPARISON)
	{
		lpsendData->typeData.base64PicProbe = m_doc["typeData"]["base64PicProbe"].GetString();
	}
	if (lpsendData->typeCode == FACEADD || lpsendData->typeCode == FACEUPDATE || lpsendData->typeCode == FACEDEL)
	{
		lpsendData->typeData.groupId = m_doc["typeData"]["groupId"].GetString();
		lpsendData->typeData.personId = m_doc["typeData"]["personId"].GetString();
	}
	if (lpsendData->typeCode == FACERECOGNITION)
	{
		lpsendData->typeData.groupId = m_doc["typeData"]["groupId"].GetString();
	}
	if (lpsendData->typeCode == FACEINIT)
	{
		lpsendData->typeData.groupId = m_doc["typeData"]["groupId"].GetString();
		lpsendData->typeData.personSize = m_doc["typeData"]["personSize"].GetInt();
		for (int i = 0; i < lpsendData->typeData.personSize; i++)
		{
			const Value& object = m_doc["typeData"]["personFeatures"][i];
			Person_Feature personFeature;
			personFeature.personId = object["personId"].GetString();
			personFeature.featureSize = object["featureSize"].GetInt();
			personFeature.features = object["features"].GetString();
			lpsendData->typeData.personFeatures.push_back(personFeature);
		}
	}
	if (m_doc.HasMember("uuid")&& !m_doc["uuid"].IsNull())
		lpsendData->uuid = m_doc["uuid"].GetString();
	if(m_doc.HasMember("url") && !m_doc["url"].IsNull())
		lpsendData->url = m_doc["url"].GetString();
}

std::string PersonToJson(PersonFeatures person)
{
	Document document;
	Document::AllocatorType& allocator = document.GetAllocator();
	//根
	Value root(kObjectType);
	root.AddMember("featureSize", getIntValue(person.featureSize), allocator);
	root.AddMember("features", getStringValue(person.features, allocator), allocator);

	//输出字符串
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	root.Accept(writer);
	std::string reststring = buffer.GetString();
	return reststring;
}

void TranslatePersonJson(const char* strData, LPPersonFeatures person)
{
	Document m_doc;
	m_doc.Parse<0>(strData);
	if (m_doc.HasParseError())
	{
		cout << "GetParaseError:" << m_doc.GetParseError() << endl;
	}
	person->features = m_doc["features"].GetString();
	person->featureSize = m_doc["featureSize"].GetInt();
}

std::string GroupsToJson(InitGroups groups)
{
	Document document;
	Document::AllocatorType& allocator = document.GetAllocator();
	//根
	Value root(kObjectType);
	root.AddMember("groupSize", getIntValue(groups.groupSize), allocator);
	//persons
	Value groupPersons(kArrayType);
	for (int i = 0; i < groups.groupSize; i++)
	{
		Value groupPerson(kObjectType);
		groupPerson.AddMember("personIds", getStringValue(groups.groupPersons[i].personIds, allocator), allocator);
		groupPerson.AddMember("groupId", getStringValue(groups.groupPersons[i].groupId, allocator), allocator);
		//往数组里放元素
		groupPersons.PushBack(groupPerson, allocator);
	}
	root.AddMember("groupPersons", groupPersons, allocator);

	//输出字符串
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	root.Accept(writer);
	std::string reststring = buffer.GetString();
	return reststring;
}

void TranslateGroupJson(const char* strData, LPInitGroups groups)
{
	Document m_doc;
	m_doc.Parse<0>(strData);
	if (m_doc.HasParseError())
	{
		cout << "GetParaseError:" << m_doc.GetParseError() << endl;
	}

	groups->groupSize = m_doc["groupSize"].GetInt();
	if (groups->groupSize > 0)
	{
		for (int i = 0; i < groups->groupSize; i++)
		{
			const Value& object = m_doc["groupPersons"][i];
			GroupPersonIds groupPerson;
			groupPerson.groupId = object["groupId"].GetString();
			groupPerson.personIds = object["personIds"].GetString();
			groups->groupPersons.push_back(groupPerson);
		}
	}
}

std::string outPutString(std::string content, int n, int l)
{
	if (content.length() > n * l)
	{
		std::string json;
		if (content.length() > (n + 1) * l)
		{
			json = content.substr(n*l, l);
			std::string jsonLength = std::to_string(l);
			json = jsonLength + json;
			for (int j = 0; j < (4 - jsonLength.length()); j++)
			{
				json = "0" + json;
			}
			json = "n" + json;
		}
		else
		{
			json = content.substr(n*l, content.length() - n * l);
			std::string jsonLength = std::to_string(content.length() - n * l);
			json = jsonLength + json;
			for (int j = 0; j < (4 - jsonLength.length()); j++)
			{
				json = "0" + json;
			}
			json = "e" + json;
		}
		return json;
	}
	return "e0000";
}


/*此函数读取libcurl发送数据后的返回信息，如果不设置此函数，
那么返回值将会输出到控制台，影响程序性能*/
static size_t cb(char *d, size_t n, size_t l, void *p)
{
	/* take care of the data here, ignored in this example */
	(void)d;
	(void)p;
	string ret(d, 0, n*l);
	cout << "ret:" << ret << endl;
	return n * l;
}

//设置单个easy handler的属性添加单个easy handler到multi handler中，  
static void init(CURLM *cm,string url,string content)
{
	CURL *eh = curl_easy_init();
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept:text/html,application/xhtml+xml,application/xml");
	curl_easy_setopt(eh, CURLOPT_WRITEFUNCTION, cb);
	curl_easy_setopt(eh, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(eh, CURLOPT_URL, url.c_str());
	curl_easy_setopt(eh, CURLOPT_POSTFIELDS, content.c_str());
	curl_easy_setopt(eh, CURLOPT_PRIVATE, url.c_str());
	curl_easy_setopt(eh, CURLOPT_VERBOSE, 0L);

	//添加easy handler 到multi handler中  
	curl_multi_add_handle(cm, eh);
	delete[]headers;
}

int sendUrl(string json,string url)
{
	CURL *curl;
	CURLcode res;
	string content = "&acceptData="+ URLEncode(json);
	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept:text/html,application/xhtml+xml,application/xml");
	curl = curl_easy_init();
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, content.c_str());    // 指定post内容
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());   // 指定url
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
	return EXIT_SUCCESS;
}

inline BYTE toHex(const BYTE &x)
{
	return x > 9 ? x - 10 + 'A' : x + '0';
}

inline BYTE fromHex(const BYTE &x)
{
	return isdigit(x) ? x - '0' : x - 'A' + 10;
}

inline string URLEncode(const string &sIn)
{
	string sOut;
	for (size_t ix = 0; ix < sIn.size(); ix++)
	{
		BYTE buf[4];
		memset(buf, 0, 4);
		if (isalnum((BYTE)sIn[ix]))
		{
			buf[0] = sIn[ix];
		}
		//else if ( isspace( (BYTE)sIn[ix] ) ) //貌似把空格编码成%20或者+都可以  
		//{  
		//    buf[0] = '+';  
		//}  
		else
		{
			buf[0] = '%';
			buf[1] = toHex((BYTE)sIn[ix] >> 4);
			buf[2] = toHex((BYTE)sIn[ix] % 16);
		}
		sOut += (char *)buf;
	}
	return sOut;
};

inline string URLDecode(const string &sIn)
{
	string sOut;
	for (size_t ix = 0; ix < sIn.size(); ix++)
	{
		BYTE ch = 0;
		if (sIn[ix] == '%')
		{
			ch = (fromHex(sIn[ix + 1]) << 4);
			ch |= fromHex(sIn[ix + 2]);
			ix += 2;
		}
		else if (sIn[ix] == '+')
		{
			ch = ' ';
		}
		else
		{
			ch = sIn[ix];
		}
		sOut += (char)ch;
	}
	return sOut;
}