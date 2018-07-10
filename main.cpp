#include "Server.h"

int main(int argc, char *argv[])
{
	int port = argc>1?atoi(argv[1]): PORT;
	if (port < 80)
		port = PORT;
	cout << "argc:" << argc << "\n port" << port << endl;
	Server svr(port);
	curl_global_init(CURL_GLOBAL_ALL);
	svr.WaitForClient();
	return 0;
}