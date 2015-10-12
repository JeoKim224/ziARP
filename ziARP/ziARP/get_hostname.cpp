/* 
*  Handong Global University
*  [UUU Project]
*  Author:	Seungjae Lee
*  Date:	2012.7.27
*/

#include <WinSock2.h>
#include "header/get_hostname.h"

#pragma comment(lib, "ws2_32")

int get_hostname(unsigned char* _ipAddr, char* hostname)
{
	int retval;
	unsigned long ipAddr;
	struct nb_recv nbtbuf;
	unsigned char nbtType[2] = {0x00, 0x21};

	memcpy(&ipAddr, _ipAddr, 4);

	//���� �ʱ�ȭ
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0 )
		return -1;

	//socket()
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == INVALID_SOCKET) return -1;

	int optval = 1000; // ���ð� msec

	retval = setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&optval, sizeof(optval));
	if(retval==SOCKET_ERROR)
		return -1;

	retval = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&optval, sizeof(optval));
	if(retval==SOCKET_ERROR)
		return -1;

	//���� �ּ� ����ü �ʱ�ȭ
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_port=htons(NBT_PORT);
	serveraddr.sin_addr.s_addr = ipAddr;

	//������ ��ſ� ����� ����
	SOCKADDR_IN peeraddr;
	int addrlen;
	char buf[BUFSIZE+1];
	int len;

	retval = sendto(sock, query, sizeof(query), 0,
		(SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if(retval==SOCKET_ERROR){
			strcpy(hostname,"unknown");
		return -1;
	}

	ZeroMemory(buf,sizeof(buf));
	//������ �ޱ�
	addrlen = sizeof(peeraddr);
	retval = recvfrom(sock, buf, BUFSIZE, 0,
		(SOCKADDR*)&peeraddr, &addrlen);
	if(retval == SOCKET_ERROR)
		return -1;
	memcpy(&nbtbuf, buf, sizeof(nbtbuf));

	if( memcmp( nbtbuf.type , nbtType, 2)== 0 ){
		strcpy(hostname, (char*)nbtbuf.name1);
	}
	else{
		strcpy(hostname,"");
	}
	
	// closesocket()
	closesocket(sock);

	//���� ����
	WSACleanup();
	return 0;
}