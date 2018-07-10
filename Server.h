#pragma once
#ifndef SERVER_H
#define SERVER_H
#include "FaceService.h" 

using namespace std;

typedef struct  _ARG {
	int connfd;
	struct sockaddr_in client;
}ARG;                    //�ͻ��˽ṹ��

typedef struct  _ServerARG {
	int connfd;
	int index;
}SERVERARG;                    //����˶˽ṹ��

class Server
{
public:
	Server(int port);
	~Server();
	Server(const Server &) = delete;
	Server & operator=(const Server &) = delete;
	void WaitForClient();
	void DoClose();
private:
	int sock_svr;
	int sock_clt;
	int ret_val;
	int addr_len;
	char buf_ip[IP_BUF_SIZE];
};

#endif