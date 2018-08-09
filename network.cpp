
#include "Network.h"
#include <process.h>


#pragma comment(lib ,"ws2_32.lib")


using namespace network;


UINT __stdcall CNetwork::CompletionThread(LPVOID pComPort)
{
	HANDLE		  hCompletionPort = (HANDLE)pComPort;
	LPHANDLE_DATA pHandleData = NULL;
	LPIO_DATA	  pIoData = NULL;

	while (TRUE) {
		DWORD dwFlag = 0;
		DWORD dwTranLen = 0;

		GetQueuedCompletionStatus(
			hCompletionPort,
			&dwTranLen,
			(PULONG_PTR)&pHandleData,
			(LPOVERLAPPED*)&pIoData,
			INFINITE
		);

		pIoData->czTmpBuf[dwTranLen] = NULL;
		strcat_s(pIoData->czRetBuf, pIoData->czTmpBuf);
		
		if (dwTranLen == 0) {
            // TODO:
            // use pIoData
			continue;
		}

		memset(&(pIoData->overlapped), 0, sizeof(OVERLAPPED));
		pIoData->wsaBuf.len = TRAN_BUF_SIZE;
		pIoData->wsaBuf.buf = pIoData->czTmpBuf;

		WSARecv(pHandleData->hClntSock,
			&(pIoData->wsaBuf),
			1,
			NULL,
			&dwFlag,
			&(pIoData->overlapped),
			NULL
		);
	}

	return 0;
}


void network::CNetwork::Run(void)
{
	WSADATA		  wsaData;
	HANDLE		  hCompletionPort = NULL;
	SYSTEM_INFO	  si;
	SOCKADDR_IN	  addrServer;
	LPIO_DATA	  pIoData = NULL;
	LPHANDLE_DATA pHandleData = NULL;
	SOCKET		  hServer = NULL;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		return;
	}

	hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	GetSystemInfo(&si);

	for (UINT i = 0; i < (UINT)si.dwNumberOfProcessors; ++i) {
		_beginthreadex(NULL, 0,
			(_beginthreadex_proc_type)CNetwork::CompletionThread,
			(LPVOID)hCompletionPort, 0, NULL);
	}

	hServer = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(atoi("31220"));
	addrServer.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(hServer, (SOCKADDR*)&addrServer, sizeof(addrServer));
	listen(hServer, SOMAXCONN);

	while (TRUE) {
		DWORD		dwFlag = 0;
		DWORD		dwRecvLen = 0;
		SOCKET		hClient;
		SOCKADDR_IN addrClient;
		int			nAddrLen = sizeof(addrClient);
		
		hClient = accept(hServer, (SOCKADDR*)&addrClient, &nAddrLen);
		
		pHandleData = new HANDLE_DATA;
		pHandleData->hClntSock = hClient;

		memcpy(&(pHandleData->clntAddr), &addrClient, nAddrLen);

		CreateIoCompletionPort((HANDLE)hClient, hCompletionPort, (ULONG_PTR)pHandleData, 0);
		
		pIoData = new IO_DATA;
		memset(&(pIoData->overlapped), 0, sizeof(OVERLAPPED));

		pIoData->wsaBuf.len = TRAN_BUF_SIZE;
		pIoData->wsaBuf.buf = pIoData->czTmpBuf;

		WSARecv(pHandleData->hClntSock,
			&(pIoData->wsaBuf),
			1,
			&dwRecvLen,
			&dwFlag,
			&(pIoData->overlapped),
			NULL
		);
	}
}
