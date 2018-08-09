#pragma once


#include <WinSock2.h>
#include <windows.h>


#define TRAN_BUF_SIZE 10240


#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")


namespace network
{
	class CNetwork
	{
	private:
        
		typedef struct tagHANDLE_DATA
		{
			SOCKET		hClntSock;
			SOCKADDR_IN clntAddr;
		} HANDLE_DATA, *LPHANDLE_DATA;

        
		typedef struct tagPER_IO_DATA
		{
			OVERLAPPED overlapped;
			char	   czTmpBuf[TRAN_BUF_SIZE];
			WSABUF	   wsaBuf;
			char	   czRetBuf[TRAN_BUF_SIZE];
		} IO_DATA, *LPIO_DATA;

        
		static UINT __stdcall CompletionThread(LPVOID pComPort);

	public:
		CNetwork(void)
		{
		}

		void Run(void);
	};
}
