/*
   MT4 EA dll for communication suit.
   Author: Zhao Zhe
   Date: 2010-8-24

*/

#include "winsock2.h"
#include <mstcpip.h>
#include <openssl/rand.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define MT4_EXPORT extern "C" _declspec(dllexport)
#pragma comment(lib,"ws2_32.lib")

#define SERVER_PORT 32347
#define AUTH_SERVER_PORT 32348

struct Order
{
    char symbol[16];
	int cmd;
	int slippage;
	int operate;  // the open or close mask
	int ticket;  // the ticket number of the master order
	double volume; 
	double percent;
	double price;
	double stoploss;
	double takeprofit;
};

struct AuthPair
{
    char Uesrname[20];
	char Password[20];
};


struct AuthPairEAID
{
	char Username[20];
	char Password[20];
	long EAID;
};

#pragma data_seg(".shared")
double EAFastBid = 0;
double EAFastAsk = 0;
double EASlowBid = 0;
double EASlowAsk = 0;
#pragma data_seg()
#pragma comment(linker,"/SECTION:.shared,RWS")


SSL *ssl = NULL;
SSL_CTX *ctx = NULL;

MT4_EXPORT void __stdcall SetEAFastBid(double bid)
{
	EAFastBid = bid;
}

MT4_EXPORT void __stdcall SetEASlowBid(double bid)
{
	EASlowBid = bid;
}

MT4_EXPORT void __stdcall SetEASlowAsk(double ask)
{
	EASlowAsk = ask;
}

MT4_EXPORT double __stdcall CalPriceDistance()
{
	return EAFastBid - EASlowBid;
}

MT4_EXPORT double __stdcall GetEASlowBid()
{
	return EASlowBid;
}

MT4_EXPORT double __stdcall GetEASlowAsk()
{
	return EASlowAsk;
}

void ParseSymbol(char *symbol, char *csymbol)
{
	int len,i = 0,j = 0;
	len = (int)strlen(csymbol);
	while(i<len)
	{
		if( (csymbol[i]>'A' && csymbol[i]<'Z') || (csymbol[i]>'a' && csymbol[i]<'z'))
			symbol[j++] = csymbol[i];
		i++;
	}
}

MT4_EXPORT void __stdcall InitSharedFastBuy(char *csymbol)
{
	int len;
	char hFileName[128];
	char symbol[10];
	memset(symbol,0,10);
	ParseSymbol(symbol,csymbol);
	strcpy_s(hFileName,"MT4SharedFastBuy");
	strcat_s(hFileName,symbol);
	len = (int)strlen(hFileName) + 1;
	wchar_t *wText = new wchar_t[len];
	memset(wText,0,len);
	::MultiByteToWideChar(CP_ACP,NULL,hFileName,-1,wText,len);
	
	LPCWSTR s_hFileName = wText;

	HANDLE s_hFileMap = CreateFileMapping((HANDLE)0xFFFFFFFF,NULL,PAGE_READWRITE,0,sizeof(double),s_hFileName);
	delete []wText;
}

MT4_EXPORT void __stdcall InitSharedFastSell(char *csymbol)
{
	int len;
	char hFileName[128];
	char symbol[10];
	memset(symbol,0,10);
	ParseSymbol(symbol,csymbol);
	strcpy_s(hFileName,"MT4SharedFastSell");
	strcat_s(hFileName,symbol);
	len = (int)strlen(hFileName) + 1;
	wchar_t *wText = new wchar_t[len];
	memset(wText,0,len);
	::MultiByteToWideChar(CP_ACP,NULL,hFileName,-1,wText,len);
	
	LPCWSTR s_hFileName = wText;

	HANDLE s_hFileMap = CreateFileMapping((HANDLE)0xFFFFFFFF,NULL,PAGE_READWRITE,0,sizeof(double),s_hFileName);
	delete []wText;
}

MT4_EXPORT void __stdcall CloseSharedFastBuy(char *csymbol)
{
	
}

MT4_EXPORT void __stdcall CloseSharedFastSell(char *csymbol)
{
	
}

MT4_EXPORT void __stdcall InitSharedSlowSell(char *csymbol)
{
	int len;
	char hFileName[128];
	char symbol[10];
	memset(symbol,0,10);
	ParseSymbol(symbol,csymbol);
	strcpy_s(hFileName,"MT4SharedSlowSell");
	strcat_s(hFileName,symbol);
	len = (int)strlen(hFileName) + 1;
	wchar_t *wText = new wchar_t[len];
	memset(wText,0,len);
	::MultiByteToWideChar( CP_ACP,NULL,hFileName,-1,wText,len);
	
	LPCWSTR s_hFileName = wText;

	HANDLE s_hFileMap = CreateFileMapping((HANDLE)0xFFFFFFFF,NULL,PAGE_READWRITE,0,sizeof(double),s_hFileName);
	delete []wText;
}

MT4_EXPORT void __stdcall InitSharedSlowBuy(char *csymbol)
{
	int len;
	char hFileName[128];
	char symbol[10];
	memset(symbol,0,10);
	ParseSymbol(symbol,csymbol);
	strcpy_s(hFileName,"MT4SharedSlowBuy");
	strcat_s(hFileName,symbol);
	len = (int)strlen(hFileName) + 1;
	wchar_t *wText = new wchar_t[len];
	memset(wText,0,len);
	::MultiByteToWideChar( CP_ACP,NULL,hFileName,-1,wText,len);
	
	LPCWSTR s_hFileName = wText;

	HANDLE s_hFileMap = CreateFileMapping((HANDLE)0xFFFFFFFF,NULL,PAGE_READWRITE,0,sizeof(double),s_hFileName);
	delete []wText;
}

MT4_EXPORT void __stdcall SetSharedFastSell(double bid,char *csymbol)
{
	int len;
	char hFileName[128];
	char symbol[10];
	memset(symbol,0,10);
	ParseSymbol(symbol,csymbol);
	strcpy_s(hFileName,"MT4SharedFastSell");
	strcat_s(hFileName,symbol);
	len = (int)strlen(hFileName) + 1;
	wchar_t *wText = new wchar_t[len];
	memset(wText,0,len);
	::MultiByteToWideChar(CP_ACP,NULL,hFileName,-1,wText,len);
	
	LPCWSTR s_hFileName = wText;

	HANDLE hFile = OpenFileMapping(FILE_MAP_WRITE | FILE_MAP_READ,FALSE,s_hFileName);
	delete []wText;
	if(hFile != NULL)
	{
		LPVOID lpView = MapViewOfFile(hFile,FILE_MAP_WRITE | FILE_MAP_READ,0,0,0);
		if((double*)lpView != NULL)
		{
			*((double*)lpView) = bid;
		}
		UnmapViewOfFile(lpView);
	}
	CloseHandle(hFile);
}

MT4_EXPORT void __stdcall SetSharedFastBuy(double bid,char *csymbol)
{
	int len;
	char hFileName[128];
	char symbol[10];
	memset(symbol,0,10);
	ParseSymbol(symbol,csymbol);
	strcpy_s(hFileName,"MT4SharedFastBuy");
	strcat_s(hFileName,symbol);
	len = (int)strlen(hFileName) + 1;
	wchar_t *wText = new wchar_t[len];
	memset(wText,0,len);
	::MultiByteToWideChar(CP_ACP,NULL,hFileName,-1,wText,len);
	
	LPCWSTR s_hFileName = wText;

	HANDLE hFile = OpenFileMapping(FILE_MAP_WRITE | FILE_MAP_READ,FALSE,s_hFileName);
	delete []wText;
	if(hFile != NULL)
	{
		LPVOID lpView = MapViewOfFile(hFile,FILE_MAP_WRITE | FILE_MAP_READ,0,0,0);
		if((double*)lpView != NULL)
		{
			*((double*)lpView) = bid;
		}
		UnmapViewOfFile(lpView);
	}
	CloseHandle(hFile);
}

MT4_EXPORT void __stdcall SetSharedSlowBuy(double bid, char *csymbol)
{
	int len;
	char hFileName[128];
	char symbol[10];
	memset(symbol,0,10);
	ParseSymbol(symbol,csymbol);
	strcpy_s(hFileName,"MT4SharedSlowBuy");
	strcat_s(hFileName,symbol);
	len = (int)strlen(hFileName) + 1;
	wchar_t *wText = new wchar_t[len];
	memset(wText,0,len);
	::MultiByteToWideChar(CP_ACP,NULL,hFileName,-1,wText,len);
	
	LPCWSTR s_hFileName = wText;

	HANDLE hFile = OpenFileMapping(FILE_MAP_WRITE | FILE_MAP_READ,FALSE,s_hFileName);
	delete []wText;
	if(hFile != NULL)
	{
		LPVOID lpView = MapViewOfFile(hFile,FILE_MAP_WRITE | FILE_MAP_READ,0,0,0);
		if((double*)lpView != NULL)
		{
			*((double*)lpView) = bid;
		}
		UnmapViewOfFile(lpView);
	}
	CloseHandle(hFile);
}

MT4_EXPORT void __stdcall SetSharedSlowSell(double bid, char *csymbol)
{
	int len;
	char hFileName[128];
	char symbol[10];
	memset(symbol,0,10);
	ParseSymbol(symbol,csymbol);
	strcpy_s(hFileName,"MT4SharedSlowSell");
	strcat_s(hFileName,symbol);
	len = (int)strlen(hFileName) + 1;
	wchar_t *wText = new wchar_t[len];
	memset(wText,0,len);
	::MultiByteToWideChar(CP_ACP,NULL,hFileName,-1,wText,len);
	
	LPCWSTR s_hFileName = wText;

	HANDLE hFile = OpenFileMapping(FILE_MAP_WRITE | FILE_MAP_READ,FALSE,s_hFileName);
	delete []wText;
	if(hFile != NULL)
	{
		LPVOID lpView = MapViewOfFile(hFile,FILE_MAP_WRITE | FILE_MAP_READ,0,0,0);
		if((double*)lpView != NULL)
		{
			*((double*)lpView) = bid;
		}
		UnmapViewOfFile(lpView);
	}
	CloseHandle(hFile);
}

MT4_EXPORT double __stdcall GetSharedFastBuy(char *csymbol)
{
	int len;
	double ask = 0;
	char hFileName[128];
	char symbol[10];
	memset(symbol,0,10);
	ParseSymbol(symbol,csymbol);
	strcpy_s(hFileName,"MT4SharedFastBuy");
	strcat_s(hFileName, symbol);
	len = (int)strlen(hFileName) + 1;
	wchar_t *wText = new wchar_t[len];
	memset(wText,0,len);
	::MultiByteToWideChar(CP_ACP,NULL,hFileName,-1,wText,len);
	
	LPCWSTR s_hFileName = wText;

	HANDLE hFile = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE,FALSE,s_hFileName);
	delete []wText;
	if(hFile != NULL)
	{
		LPVOID lpView = MapViewOfFile(hFile,FILE_MAP_READ | FILE_MAP_WRITE,0,0,0);
		if((double*)lpView != NULL)
		{
			ask = *((double*)lpView);
		}
		UnmapViewOfFile(lpView);
	}
	CloseHandle(hFile);	
	return ask;
}

MT4_EXPORT double __stdcall GetSharedFastSell(char *csymbol)
{
	int len;
	double bid = 0;
	char hFileName[128];
	char symbol[10];
	memset(symbol,0,10);
	ParseSymbol(symbol,csymbol);
	strcpy_s(hFileName,"MT4SharedFastSell");
	strcat_s(hFileName, symbol);
	len = (int)strlen(hFileName) + 1;
	wchar_t *wText = new wchar_t[len];
	memset(wText,0,len);
	::MultiByteToWideChar(CP_ACP,NULL,hFileName,-1,wText,len);
	
	LPCWSTR s_hFileName = wText;

	HANDLE hFile = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE,FALSE,s_hFileName);
	delete []wText;
	if(hFile != NULL)
	{
		LPVOID lpView = MapViewOfFile(hFile,FILE_MAP_READ | FILE_MAP_WRITE,0,0,0);
		if((double*)lpView != NULL)
		{
			bid = *((double*)lpView);
		}
		UnmapViewOfFile(lpView);
	}
	CloseHandle(hFile);	
	return bid;
}

MT4_EXPORT double __stdcall GetSharedSlowBuy(char *csymbol)
{
	int len;
	double ask = 0;
	char hFileName[128];
	char symbol[10];
	memset(symbol,0,10);
	ParseSymbol(symbol,csymbol);
	strcpy_s(hFileName,"MT4SharedSlowBuy");
	strcat_s(hFileName,symbol);
	len = (int)strlen(hFileName) + 1;
	wchar_t *wText = new wchar_t[len];
	memset(wText,0,len);
	::MultiByteToWideChar(CP_ACP,NULL,hFileName,-1,wText,len);
	
	LPCWSTR s_hFileName = wText;

	HANDLE hFile = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE,FALSE,s_hFileName);
	delete []wText;
	if(hFile != NULL)
	{
		LPVOID lpView = MapViewOfFile(hFile,FILE_MAP_READ | FILE_MAP_WRITE,0,0,0);
		if((double*)lpView != NULL)
		{
			ask = *((double*)lpView);
		}
		UnmapViewOfFile(lpView);
	}
	CloseHandle(hFile);	
	return ask;
}

MT4_EXPORT double __stdcall GetSharedSlowSell(char *csymbol)
{
	int len;
	double bid = 0;
	char hFileName[128];
	char symbol[10];
	memset(symbol,0,10);
	ParseSymbol(symbol,csymbol);
	strcpy_s(hFileName,"MT4SharedSlowSell");
	strcat_s(hFileName,symbol);
	len = (int)strlen(hFileName) + 1;
	wchar_t *wText = new wchar_t[len];
	memset(wText,0,len);
	::MultiByteToWideChar(CP_ACP,NULL,hFileName,-1,wText,len);
	
	LPCWSTR s_hFileName = wText;

	HANDLE hFile = OpenFileMapping(FILE_MAP_READ | FILE_MAP_WRITE,FALSE,s_hFileName);
	delete []wText;
	if(hFile != NULL)
	{
		LPVOID lpView = MapViewOfFile(hFile,FILE_MAP_READ | FILE_MAP_WRITE,0,0,0);
		if((double*)lpView != NULL)
		{
			bid = *((double*)lpView);
		}
		UnmapViewOfFile(lpView);
	}
	CloseHandle(hFile);	
	return bid;
}

MT4_EXPORT int __stdcall InitNetwork()
{
    WSADATA wsaData;

	SSL_library_init();
	ctx = SSL_CTX_new(SSLv23_client_method());

	if(ctx == NULL)
	{
		return -1;
	}

    return WSAStartup(WINSOCK_VERSION,&wsaData);
}

MT4_EXPORT int __stdcall DeInitNetwork()
{
	if(ctx != NULL)
	{
		SSL_CTX_free(ctx);
		ctx = NULL;
	}
	WSACleanup();
	return 0;
}

MT4_EXPORT int __stdcall CreateSendSocket()
{
	SOCKET ClientSock;

	ClientSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (ClientSock == INVALID_SOCKET)
		return -1;

	return ClientSock;
}

MT4_EXPORT int __stdcall CreateRecvSocket()
{
	SOCKET ClientSock;
	sockaddr_in RecvAddr;
	int iResult;

	ClientSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (ClientSock == INVALID_SOCKET)
		return -1;

	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(SERVER_PORT);
	RecvAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	iResult = bind(ClientSock, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));
	if (iResult != 0)
		return -1;

	return ClientSock;
}

MT4_EXPORT int __stdcall SendOrderNative(int ClientSock, char *symbol, int cmd, 
								   int slippage, double volume, double percent, double price, 
								   double stoploss, double takeprofit, int operate, int ticket)
{
	int length;
	struct Order order;
	sockaddr_in RecvAddr;
	SOCKET sock = (SOCKET)ClientSock;

    memset(&order, 0, sizeof(struct Order));
	strcpy_s(order.symbol, symbol);
	order.cmd = cmd;
	order.slippage = slippage;
	order.volume = volume;
	order.percent = percent;
	order.price = price;
	order.stoploss = stoploss;
	order.takeprofit = takeprofit;
	order.operate = operate;
	order.ticket = ticket;

	RecvAddr.sin_family = AF_INET;
	RecvAddr.sin_port = htons(SERVER_PORT);
	RecvAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	length = sendto(sock, (char *)&order, sizeof(struct Order), 0, (SOCKADDR*)&RecvAddr, sizeof(RecvAddr));
    
	if(length == sizeof(struct Order))
	{
	    return 0;
	}
	else
	    return -1;
}

MT4_EXPORT int __stdcall RecvOrderNative(int ClientSock, char *symbol, int *cmd, 
										 int *slippage, double *volume, double *percent, double *price, 
										 double *stoploss, double *takeprofit, int *operate, int *ticket)
{
    struct Order order;
    SOCKET sock = (SOCKET)ClientSock;
	sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof(SenderAddr);
	int length;

    memset(&order, 0, sizeof(struct Order));
	length = recvfrom(sock, (char *)&order, sizeof(order), 0, (SOCKADDR*)&SenderAddr, &SenderAddrSize);

	if (length == sizeof(struct Order))
	{
		strcpy(symbol, order.symbol);
	    *cmd = order.cmd;
	    *slippage = order.slippage;
	    *volume = order.volume;
		*percent = order.percent;
	    *price = order.price;
	    *stoploss = order.stoploss;
	    *takeprofit = order.takeprofit;
		*operate = order.operate;
		*ticket = order.ticket;
	    return 0;
	}
	else if(length < 0)
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

MT4_EXPORT int __stdcall ConnectToHost(char *HostName)
{
	struct hostent *hp;
	struct sockaddr_in sa;
    SOCKET ClientSock;
	bool keepalive = true;

	ClientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if( ClientSock == INVALID_SOCKET)
		return -1;

	sa.sin_family = AF_INET;
	sa.sin_port = htons(SERVER_PORT);
    if(inet_addr(HostName) == INADDR_NONE)
	{
	    hp = gethostbyname(HostName);
	    sa.sin_addr.S_un.S_addr = *((unsigned long*)hp->h_addr);
	}
	else
	{
		sa.sin_addr.S_un.S_addr = inet_addr(HostName);
	}

    if(connect(ClientSock, (struct sockaddr*)&sa, sizeof(sa)))
	{
		closesocket(ClientSock);
		return -1;
	}

	if(setsockopt(ClientSock,SOL_SOCKET,SO_KEEPALIVE,(char*)&keepalive,sizeof(keepalive)))
	{
		closesocket(ClientSock);
		return -1;
	}

	tcp_keepalive alive_in;
	tcp_keepalive alive_out;
	alive_in.keepalivetime = 10000;
	alive_in.keepaliveinterval = 1000;
	alive_in.onoff = true;
	unsigned long ulBytesReturn = 0;

	if(WSAIoctl(ClientSock,SIO_KEEPALIVE_VALS, &alive_in, sizeof(alive_in), &alive_out, sizeof(alive_out), &ulBytesReturn, NULL, NULL))
	{
		closesocket(ClientSock);
		return -1;
	}
	
	ssl = SSL_new(ctx);
	if(ssl == NULL)
	{
		closesocket(ClientSock);
		return -1;
	}

	int ret = SSL_set_fd(ssl, ClientSock);
	if(ret == 0)
	{
		SSL_free(ssl);
		ssl = NULL;
		closesocket(ClientSock);
		return -1;
	}

	ret = SSL_connect(ssl);
	if(ret != 1)
	{
		SSL_shutdown(ssl);
		SSL_free(ssl);
		ssl = NULL;
		closesocket(ClientSock);
		return -1;
	}

	return (int)ClientSock;
}

MT4_EXPORT int __stdcall ConnectToAuthHost(char *HostName)
{
	struct hostent *hp;
	struct sockaddr_in sa;
    SOCKET ClientSock;

	ClientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if( ClientSock == INVALID_SOCKET)
		return -1;

	sa.sin_family = AF_INET;
	sa.sin_port = htons(AUTH_SERVER_PORT);
    if(inet_addr(HostName) == INADDR_NONE)
	{
	    hp = gethostbyname(HostName);
	    sa.sin_addr.S_un.S_addr = *((unsigned long*)hp->h_addr);
	}
	else
	{
		sa.sin_addr.S_un.S_addr = inet_addr(HostName);
	}

    if(connect(ClientSock, (struct sockaddr*)&sa, sizeof(sa)))
	{
		closesocket(ClientSock);
		return -1;
	}

	bool keepalive = true;
	if(setsockopt(ClientSock,SOL_SOCKET,SO_KEEPALIVE,(char*)&keepalive,sizeof(keepalive)))
	{
		closesocket(ClientSock);
		return -1;
	}

	tcp_keepalive alive_in;
	tcp_keepalive alive_out;
	alive_in.keepalivetime = 10000;
	alive_in.keepaliveinterval = 1000;
	alive_in.onoff = true;
	unsigned long ulBytesReturn = 0;

	if(WSAIoctl(ClientSock,SIO_KEEPALIVE_VALS, &alive_in, sizeof(alive_in), &alive_out, sizeof(alive_out), &ulBytesReturn, NULL, NULL))
	{
		closesocket(ClientSock);
		return -1;
	}

	ssl = SSL_new(ctx);
	if(ssl == NULL)
	{
		closesocket(ClientSock);
		return -1;
	}

	int ret = SSL_set_fd(ssl, ClientSock);
	if(ret == 0)
	{
		SSL_free(ssl);
		ssl = NULL;
		closesocket(ClientSock);
		return -1;
	}

	ret = SSL_connect(ssl);
	if(ret != 1)
	{
		SSL_shutdown(ssl);
		SSL_free(ssl);
		ssl = NULL;
		closesocket(ClientSock);
		return -1;
	}

	return (int)ClientSock;
}

MT4_EXPORT int __stdcall CloseConnection(int ClientSock)
{
	if(ssl != NULL)
	{
		SSL_shutdown(ssl);
		SSL_free(ssl);
		ssl = NULL;
	}
	return closesocket(ClientSock);
}

MT4_EXPORT int __stdcall SendAuthMessage(int ClientSock, char *Username, char* Password)
{
	struct AuthPair Ap;
	int sendout;
	int result = 0;

	if(Username == NULL || Password == NULL)
		return 1;

	memset(&Ap, 0, sizeof(struct AuthPair));
	strcpy_s(Ap.Uesrname, Username);
	strcpy_s(Ap.Password, Password);
	
	if(ssl != NULL)
	{
		sendout = SSL_write(ssl, (const void*)&Ap, sizeof(struct AuthPair));
		if(sendout == sizeof(struct AuthPair))
		{
			SSL_read(ssl, (void *)&result, sizeof(result));
			if(result == 0xffff)
				return 0;
			else
				return 1;
		}
		else
		{
			return 1;
		}
	}
	return 1;
}

MT4_EXPORT int __stdcall SendAuthEAIDMessage(int ClientSock, char *Username, char* Password, long EAID)
{
	struct AuthPairEAID Ap;
	int sendout;
	int result = 0;

	if(Username == NULL || Password == NULL)
		return 1;

	memset(&Ap, 0, sizeof(struct AuthPairEAID));
	strcpy_s(Ap.Username, Username);
	strcpy_s(Ap.Password, Password);
	Ap.EAID = EAID;
	if(ssl != NULL)
	{
		sendout = SSL_write(ssl, (void *)&Ap, sizeof(struct AuthPairEAID));
		if(sendout == sizeof(struct AuthPairEAID))
		{
			SSL_read(ssl, (void *)&result, sizeof(result));
	        if(result == 0xffff)
			{
				return 0;
			}
			else
				return 1;
		}
		else
			return 1;
	}

	return 1;
}

MT4_EXPORT int __stdcall SendKeepAlive(int ClientSock, int magic)
{
	int sendout;
	int recvlen;
	char KA[16];
	char RA[16];
	char *SA;
	int magic_r;
	memset(KA, 0 , sizeof(KA));
	memset(RA, 0 , sizeof(RA));
	SA = itoa(magic, KA, 10);
	
	if(ssl != NULL)
	{
		sendout = SSL_write(ssl, (void *)SA, sizeof(KA));
		if(sendout == sizeof(KA))
		{
			recvlen = SSL_read(ssl, (void*)RA, sizeof(RA));
			if(recvlen == sizeof(RA))
			{
				magic_r = atoi(RA);
				if(magic_r == magic)
					return 0;
			} 
			else
				return 1;
		}
		return 1;
	}
	return 1;
}

MT4_EXPORT int __stdcall ReceiveFromHost(int ClientSock, char *symbol, int *cmd, 
										 int *slippage, double *volume, double *percent, double *price, 
										 double *stoploss, double *takeprofit, int *operate, int *ticket)
{
    struct Order order;
    SOCKET sock = (SOCKET)ClientSock;
	int length;

    memset(&order, 0, sizeof(struct Order));
	if(ssl != NULL)
	{
		length = SSL_read(ssl, (void*)&order, sizeof(order));

		if (length == sizeof(struct Order))
		{
			strcpy(symbol, order.symbol);
		    *cmd = order.cmd;
		    *slippage = order.slippage;
		    *volume = order.volume;
			*percent = order.percent;
		    *price = order.price;
		    *stoploss = order.stoploss;
		    *takeprofit = order.takeprofit;
			*operate = order.operate;
			*ticket = order.ticket;
		    return 0;
		}
		else if(length < 0)
		{
			return 1;
		}
		else
		{
			return -1;
		}
	}
	return -1;
}

MT4_EXPORT int __stdcall SendOrder(int ClientSock, char *symbol, int cmd, 
								   int slippage, double volume, double percent, double price, 
								   double stoploss, double takeprofit, int operate, int ticket)
{
	int length;
	struct Order order;
	SOCKET sock = (SOCKET)ClientSock;

    memset(&order, 0, sizeof(struct Order));
	strcpy_s(order.symbol, symbol);
	order.cmd = cmd;
	order.slippage = slippage;
	order.volume = volume;
	order.percent = percent;
	order.price = price;
	order.stoploss = stoploss;
	order.takeprofit = takeprofit;
	order.operate = operate;
	order.ticket = ticket;

	if(ssl != NULL)
	{
		length = SSL_write(ssl, (void *)&order, sizeof(struct Order));
    
		if(length == sizeof(struct Order))
		{
		    return 0;
		}
		else
		    return -1;

	}
	return -1;
}

MT4_EXPORT int __stdcall SetRecvTimeOut(int ClientSock, int RecvTimeOut)
{
	SOCKET sock;
	sock = ClientSock;

    if(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&RecvTimeOut, sizeof(RecvTimeOut)))
	{
	    return -1;
	}

	return 0;
}

MT4_EXPORT int __stdcall SetTCPNoDelay(int ClientSock, int enable)
{
	SOCKET sock;
	sock = ClientSock;

	if(setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char*)&enable, sizeof(enable)))
	{
		return -1;
	}

	return 0;
}