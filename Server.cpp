#include "Server.h"
#include <signal.h>
using namespace std;

FaceService* faceService;

//使用pthread线程库
int process_cli(int connectfd, struct sockaddr_in client);    //客户端请求处理函数
void* start_routine(void* arg);        //线程函数

Server::Server(int port)
{
	cout << "Initializing server...start\n";
	clock_t start_time = clock();
	// 初始化引擎
	int ret = 0;
	faceService = new FaceService();
	ret = faceService->init();
	if (ret != MOK)
	{
		printf("初始化引擎失败,错误码: %d, 程序将关闭！", ret);
		DoClose();
		system("pause");
		exit(1);
	}
	
	struct sockaddr_in server;     //服务器地址信息结构体

	if ((sock_svr = socket(AF_INET, SOCK_STREAM, 0)) == -1) { //调用socket，创建监听客户端的socket
		perror("Creating socket failed.");
		exit(1);
	}

	int opt = SO_REUSEADDR;
	setsockopt(sock_svr, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));    //设置socket属性，端口可以重用
																		  //初始化服务器地址结构体
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sock_svr, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1) {    //调用bind，绑定地址和端口
		perror("Bind error.");
		exit(1);
	}

	if (listen(sock_svr, BACKLOG) == -1) {      //调用listen，开始监听
		perror("listen() error\n");
		exit(1);
	}
	clock_t end_time = clock();
	long takingTime = static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC * 1000;
	cout << "Initializing server...end takingtime"<< takingTime<<endl;
}

void* process_make(void* arg)
{
	pthread_detach(pthread_self());
	SENDDATA* send_data;
	send_data = (SENDDATA*)arg;
	faceService->makeSendData(send_data->jsonstr, send_data->connectfd, send_data->index);
	delete send_data;
	pthread_exit(NULL);
}

pthread_t  thread_pro;

void* process_cin(void* arg)
{
	pthread_t  thread_mk;
	pthread_detach(pthread_self());
	TYPEINT connectfd;
	connectfd = (TYPEINT)arg;
	string ss;
	int snd_result = 0;
	char ret_statu[1];
	char ret_length[4];
	int retLength = 0;
	int index = 0;
	do
	{
		//memset(ret_statu, 0, 1);
		//memset(ret_length, 0, 4);
		int RecvSize = ::recv(connectfd, ret_statu, 1, 0);
		if (SOCKET_ERROR == RecvSize||RecvSize == 0)
		{
			cout << "RecvSize:" << RecvSize;
			perror(" error");
			break;
		}
		string retStatu(ret_statu, 0, 1);
		if (ret_statu[0] == 't')
		{
			cout << "ret_statu again" << endl;
			snd_result = ::send(connectfd, "t", 1, 0);
			ss = "";
			if(snd_result==1)
				continue;
			else
			{
				break;
			}
		}
		else if (ret_statu[0] != 'e'&&ret_statu[0] != 'n')
		{
			cout << "retStatu is error:"<< retStatu << endl;
			break;
		}
		RecvSize = ::recv(connectfd, ret_length, 4, 0);
		retLength = atoi(ret_length);
		if (retLength<1)
		{
			string err_ret(ret_length, 0, 4);
			err_ret += " retLength parameter is error";
			cout << "retLength parameter is error:" << err_ret << endl;
			break;
		}
		char* buffer = new char[retLength];
		int nTotal = 0;        //已经读入缓冲区的字节数
		int nLeft = retLength;        //剩下数据的字节数
		int nBytes = 0;        //当前已读数据在缓冲区的位置
		while (nTotal != retLength)    //已经读入缓冲区的字节数不等于需要读入的大小时
		{
			RecvSize = recv(connectfd, &buffer[nBytes], nLeft, 0);//接收数据
			if (SOCKET_ERROR == RecvSize)//读操作失败
			{
				break;
			}
			if (0 == RecvSize)
			{
				break;//函数退出
			}
			nTotal += RecvSize;
			nLeft -= RecvSize;
			nBytes += RecvSize;
		}
		string temp(buffer, 0, retLength);
		ss += temp;
		delete[]buffer;
		if (SOCKET_ERROR == RecvSize)
		{
			break;
		}
		if (ret_statu[0] == 'e')
		{
			SENDDATA* send_data;
			send_data = new SENDDATA();
			send_data->jsonstr = ss;
			send_data->index = index;
			send_data->connectfd = connectfd;
			pthread_create(&thread_mk, NULL, process_make, (void*)send_data);
			recordLog(getstring(connectfd)+" Client requests to end the connection... length:"+ getstring(ss.length()));
			index++;
			snd_result = ::send(connectfd, "e", 1, 0);
			continue;
		}
		else if(ret_statu[0] == 'n')
		{
			snd_result = ::send(connectfd, "n", 1, 0);
		}
	} while (1);
	cout << "close process_cin "<< connectfd << endl;
	int ret_val = ::shutdown(connectfd, SD_SEND);
	close(connectfd);
	if (ret_val == SOCKET_ERROR)
	{
		perror("Failed to hutdown the client socket! error");
	}
	pthread_exit(NULL);
}

int process_cli(int connectfd, sockaddr_in client)
{
	string ss;
	int snd_result = 0;
	char ret_statu[1];
	char ret_length[4];
	int retLength = 0;
	int index=0;
	recordLog("process_cli:connectfd:" +getstring(connectfd) );
	do
	{
		//memset(ret_statu, 0, 1);
		//memset(ret_length, 0, 4);
		int RecvSize = ::recv(connectfd, ret_statu, 1, 0);
		string retStatu(ret_statu,0,1);
		if (ret_statu[0] != 'e'&&ret_statu[0] != 'n')
		{
			if (ret_statu[0] == 't')
			{
				if (pthread_create(&thread_pro, NULL, process_cin, (void*)connectfd)) {        //创建线程，以客户端连接为参数，start_routine为线程执行函数
					perror("Pthread_create() process_cin error");
					close(connectfd);
				}
				return 0;
			}
			string err_ret(ret_statu, 0, 1);
			err_ret += " retStatu parameter is error";
			::send(connectfd, err_ret.c_str(), err_ret.length(), 0);
			break;
		}
		RecvSize = ::recv(connectfd, ret_length, 4, 0);
		retLength = atoi(ret_length);
		if (retLength<1)
		{
			string err_ret(ret_length, 0, 4);
			err_ret += " retLength parameter is error";
			::send(connectfd, err_ret.c_str(), err_ret.length(), 0);
			break;
		}
		char* buffer = new char[retLength];

		int nTotal = 0;        //已经读入缓冲区的字节数
		int nLeft = retLength;        //剩下数据的字节数
		int nBytes = 0;        //当前已读数据在缓冲区的位置

		while (nTotal != retLength)    //已经读入缓冲区的字节数不等于需要读入的大小时
		{
			RecvSize = recv(connectfd, &buffer[nBytes], nLeft, 0);//接收数据
			if (SOCKET_ERROR == RecvSize)//读操作失败
			{
				break;
			}
			if (0 == RecvSize)
			{
				break;//函数退出
			}
			nTotal += RecvSize;
			nLeft -= RecvSize;
			nBytes += RecvSize;
		}
		string temp(buffer,0, retLength);
		//recordLog( "Message received thread:"+getstring(connectfd)+" line:"+getstring(index)+" ret_statu:"+ retStatu+" ret_length:"+ getstring(retLength)+" nBytes:"+ getstring(nBytes));
		index++;
		ss += temp;
		delete []buffer;
		if (SOCKET_ERROR == RecvSize)
		{
			break;
		}
		if (ret_statu[0] == 'e')
		{
			snd_result = ::send(connectfd, "e", 1, 0);
			recordLog("Message received thread:" + getstring(connectfd) + " length: " + getstring(ss.length()));
			string json=faceService->makeSendData(ss);
			recordLog("send thread:" + getstring(connectfd) + " Message: " + (json.length()>100?json.substr(0,100): json));
			int j = 0;
			while (true)
			{
				string sendjson = outPutString(json, j, MSG_BUF_SIZE);
				j++;
				snd_result = ::send(connectfd, sendjson.c_str(), sendjson.length(), 0);
				if (snd_result == SOCKET_ERROR)
				{
					perror("Failed to send message to client! error");
					break;
				}
				char sedn_ret[1];
				//memset(sedn_ret, 0, 1);
				::recv(connectfd, sedn_ret, 1, 0);
				string sednTemp(sedn_ret,0,1);
				recordLog("received thread:" + getstring(connectfd)+"sedn_ret: " + sednTemp);
				if (sedn_ret[0] != 'n')
				{
					break;
				}
			}
			recordLog("Client requests to close the connection...");
			break;
		}
		else if (ret_statu[0] == 'n')
		{
			snd_result = ::send(connectfd, "n", 1, 0);
		}
		else
		{
			cerr << "返回结果不正确 " << ret_statu;
			break;
		}
	} while (ret_statu[0] == 'n');
	//
	int ret_val = ::shutdown(connectfd, SD_SEND);
	if (ret_val == SOCKET_ERROR)
	{
		perror("Failed to hutdown the client socket! error");
		close(connectfd);
		return 1;
	}
	close(connectfd);
	return 0;
}

void* start_routine(void* arg)
{
	pthread_detach(pthread_self());
	ARG *info;
	info = (ARG *)arg;
	process_cli(info->connfd, info->client);
	delete info;
	pthread_exit(NULL);
}

void Server::WaitForClient()
{
	pthread_t  thread;        //线程体变量
	int sin_size;
	sin_size = sizeof(struct sockaddr_in);
	ARG *arg;            //客户端结构体变量
	struct sockaddr_in client;     //客户端地址信息结构体
	while (true)
	{
		if ((sock_clt = accept(sock_svr, (struct sockaddr *)&client, (socklen_t *)&sin_size)) == -1) {
			//调用accept，返回与服务器连接的客户端描述符
			perror("accept() error\n");
			exit(1);
		}
		::inet_ntop(client.sin_family, &client, buf_ip, IP_BUF_SIZE);
		string bufIp(buf_ip);
		//uint16_t sinPort=::ntohs(client.sin_port);
		recordLog("A new client connected...IP address: "+ bufIp+", port number: ");
		arg = new  ARG;
		arg->connfd = sock_clt;
		memcpy(&arg->client, &client, sizeof(client));
		sigset_t signal_mask;
		sigemptyset(&signal_mask);
		sigaddset(&signal_mask, SIGPIPE);
		int rc = pthread_sigmask(SIG_BLOCK, &signal_mask, NULL);
		if (rc != 0) {
			perror("block sigpipe error");
			close(sock_clt);
		}
		else if (pthread_create(&thread, NULL, start_routine, (void*)arg)) {        //创建线程，以客户端连接为参数，start_routine为线程执行函数
			perror("Pthread_create() error");
			close(sock_clt);
		}
	}
}

void Server::DoClose()
{
	if (faceService != nullptr)
	{
		faceService->uninit();
		delete faceService;
		faceService = nullptr;
	}
}

Server::~Server()
{
	DoClose();
	::close(sock_svr);
	::close(sock_clt);
	cout << "Socket closed..." << endl;
}