#pragma once
#include "faceCommon.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

using namespace std;
using namespace rapidjson;

#define MAX 8 /* number of simultaneous transfers */  
#define CNT sizeof(urls)/sizeof(char*) /* total number of transfers to do */  

int32_t createDirectory(string directoryPath);
void TranslateJson(const char* strData, LPSend_Data sendData);
std::string DataToJson(Accept_Data acceptData);
std::string PersonToJson(PersonFeatures person);
void TranslatePersonJson(const char* strData, LPPersonFeatures person);
std::string GroupsToJson(InitGroups groups);
void TranslateGroupJson(const char* strData, LPInitGroups groups);
string getstring(const int n);
void recordLog(string log);
void recordLog(const int n);
std::string outPutString(std::string content, int n, int l);
int sendUrl(string json, string url);
inline string URLDecode(const string &sIn);
inline string URLEncode(const string &sIn);