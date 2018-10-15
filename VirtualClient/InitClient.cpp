#include "stdafx.h"

#include <stdio.h>

void InsertLogMsg(LPTSTR lpszMsg);

extern HINSTANCE		g_hInst;
extern HWND				g_hMainWnd;

BOOL ConnectServer(SOCKET &s, SOCKADDR_IN* addr, UINT nMsgID, LPCTSTR lpServerIP, DWORD dwIP, int nPort, long lEvent)
{
	if (s != INVALID_SOCKET)
	{
		closesocket(s);
		s = INVALID_SOCKET;
	}

	s = socket(AF_INET, SOCK_STREAM, 0);

	addr->sin_family	= AF_INET;
	addr->sin_port		= htons(nPort);
	
	if (lpServerIP)
		addr->sin_addr.s_addr	= inet_addr(lpServerIP);
	else
	{
		DWORD dwReverseIP = 0;

		dwReverseIP = (LOBYTE(LOWORD(dwIP)) << 24) | (HIBYTE(LOWORD(dwIP)) << 16) | (LOBYTE(HIWORD(dwIP)) << 8) | (HIBYTE(HIWORD(dwIP)));

		addr->sin_addr.s_addr	= dwReverseIP;
	}

	if (WSAAsyncSelect(s, g_hMainWnd, nMsgID, lEvent) == SOCKET_ERROR)
		return FALSE;

	if (connect(s, (const struct sockaddr FAR*)addr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
			return TRUE;
	}

	return FALSE;
}

LRESULT OnSockMsg(WPARAM wParam, LPARAM lParam)
{
	switch (WSAGETSELECTEVENT(lParam))
	{
		case FD_CONNECT:
		{
			//InsertLogMsg(_T("has connected to server"));
			char str[32];
			InsertLogMsg(itoa(WSAGETSELECTERROR(lParam),str,10));

			break;
		}
		case FD_CLOSE:
		{
			break;
		}
		case FD_READ:
		{
			char	szMsg[4096] = "nwz";

			int nLen = recv((SOCKET)wParam, szMsg, sizeof(szMsg), 0);

			szMsg[nLen] = '\0';
			
			InsertLogMsg(szMsg);
			
			char log[200];
			
			_TDEFAULTMESSAGE	DefaultMsg;
			
			char* pszBegin; 
			char* pszEnd;

			if((pszBegin = (char *)memchr(szMsg, '#', memlen(szMsg))) && (pszEnd = (char *)memchr(szMsg, '!', memlen(szMsg))))
			{
			InsertLogMsg(pszBegin);
			InsertLogMsg(pszEnd);

			*pszEnd='\0';


			fnDecodeMessageA(&DefaultMsg, (pszBegin + 2));


			/*
			char				szIDPassword[32];
			
			  int nDecodeLen = fnDecode6BitBufA(pszBegin+18, szIDPassword, sizeof(szIDPassword));
			  szIDPassword[nDecodeLen] = '\0';
			  
				
				
				  sprintf(log,"Msg is %d,data is %s",DefaultMsg.wIdent,szIDPassword);
			*/

			sprintf(log,"Msg is %d",DefaultMsg.wIdent);
			
			InsertLogMsg(log);

			}
			
			


			break;
		}
	}

	return 0;
}
