///////////////////////////////////////////////////////////////////////////////////////
//
//	* 'ziArp' - a ARP poisoning attack detector
//	* 2012 Summer Handong Global University GHOST U3 Team Open Source Project
//	* File Name		: get_info.cpp
//	* Author		: �̽���-(Seungjae Lee)
//					  �ּ���-(Seoyul Choi)
//	* Date			: 2012. 7. 16 - 2012. 8. 4
//	* Description
//		 ���� ����̽��� ���� ����̽��� ������ �аų� ��û�ϴ� �Լ��� ���ǵǾ��ִ� �����̴�.
//
///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
//
//	'#include's
//
///////////////////////////////////////////////////////////////////////////////////////
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif
#include <memory.h>
#include <Windows.h>
#include <IPHlpApi.h>
#include <string.h>
#include "header/datas.h"
#include "header/get_info.h"
#include "header/log.h"

#define PACKET_SIZE 42

char query[50] = {
	0x92, 0xd9, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x20, 0x43, 0x4b, 0x41, 
	0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 
	0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 
	0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 
	0x41, 0x41, 0x41, 0x41, 0x41, 0x00, 0x00, 0x21, 
	0x00, 0x01 };

extern ARPPKT_L* ptr;
extern ARPPKT_L* head;

///////////////////////////////////////////////////////////////////////////////////////
//
//	Prototypes
//
///////////////////////////////////////////////////////////////////////////////////////
DWORD WINAPI f(LPVOID arg);
void del_all();
void request_pkt(u_char *packet, u_char *hAddress, u_char *pAddress, u_char *netmask);

///////////////////////////////////////////////////////////////////////////////////////
//
//	Body
//
///////////////////////////////////////////////////////////////////////////////////////

//=====================================================================================
//
//	* Function : get_info()
//	* Description 
//		 ���� �����ִ� ����̽��� �̸��� ���ڷ� �޾� GetAdaptersInfo �Լ��� ����Ͽ�
//		���� ����̽��� MAC �ּҿ� IP �ּҿ� ����� ����ũ�� �д� �Լ��̴�.
//
//=====================================================================================
int get_info(char *dev_name, u_char *mac_addr, u_char *ip_addr, u_char *netmask) {
    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    pAdapterInfo = (IP_ADAPTER_INFO *) malloc(sizeof(IP_ADAPTER_INFO) );
    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);

    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        free(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *) malloc(ulOutBufLen);
        if (pAdapterInfo == NULL)
            return -1;
    }
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == NO_ERROR) {
        pAdapter = pAdapterInfo;
        while (pAdapter) {
            if (pAdapter->AddressLength == ETH_ALEN) {
                if (strstr(dev_name, pAdapter->AdapterName) != NULL) {

					/* get MAC Address */
                    memcpy(mac_addr, pAdapter->Address, ETH_ALEN);

					/* get IP Address */
					addr_aton(pAdapter->IpAddressList.IpAddress.String, ip_addr);

					/* get Netmask */
					addr_aton(pAdapter->IpAddressList.IpMask.String, netmask);

                    break;
                }
            }
            pAdapter = pAdapter->Next;
        }
    }
    else {
        if (pAdapterInfo)
            free(pAdapterInfo);
        return -2;
    }
    if (pAdapterInfo)
        free(pAdapterInfo);
    return 0;
}

//=====================================================================================
//
//	* Function : addr_aton()
//	* Description 
//		 ���ڿ��� �Էµ� �������� �ּҸ� ���������� ��ȯ�ϴ� �Լ��̴�.
//
//=====================================================================================
void addr_aton(char *addr, u_char *netmask){

	char str[4];
	int i, j, k;

	k = 0;
	for(j = 0; j < 4; j++){
		for(i = 0; addr[k] >= '0' && addr[k] <= '9'; i++, k++){
			str[i] = addr[k];
		}
		str[i] = 0;
		netmask[j] = atoi(str);
		k++;
	}
}

//=====================================================================================
//
//	* Function : get_remote_mac()
//	* Description 
//		 ���� ����̽��� MAC �ּҸ� �д� �Լ��̴�. ARP request ��Ŷ�� �����ؼ� �Լ���
//		���ڷ� ���� IP �ּҷ� �߼��� ��, ���ŵ� ARP reply ��Ŷ�� ������ �о� MAC �ּҸ�
//		�˾Ƴ���.
//
//=====================================================================================
int get_remote_mac(u_char *targetIPAddr, u_char *targetMACAddress){

	HANDLE pcap_thread;
	u_char pkt[PACKET_SIZE];
	u_char netmask[4];
	u_char hAddress[6];
	u_char pAddress[4];

	/* Get my hardware/protocol addresses */
	get_info(deviceName, &hAddress[0], &pAddress[0], &netmask[0]);

	/* ��Ŷ ���� */
	request_pkt(pkt, hAddress, pAddress, netmask);

	pkt[38] = targetIPAddr[0];
	pkt[39] = targetIPAddr[1];
	pkt[40] = targetIPAddr[2];
	pkt[41] = targetIPAddr[3];

	head = (ARPPKT_L*)malloc(sizeof(ARPPKT_L));
	ptr = head;

	/* reply ���� �غ� */
	pcap_thread = CreateThread(NULL, 0, f, NULL, 0, NULL);

	/* ARP request ���� */
	if(pcap_sendpacket(adhandle, pkt, PACKET_SIZE) != 0){
		log("Error sending a arp request packet");
		return -1;
	}

	Sleep(WAITING_TIME);

	/* reply ���� ���� */
	TerminateThread(pcap_thread, NULL);
	CloseHandle(pcap_thread);

	/* reply ���� ���� Ȯ�� */
	if(head == ptr){
		del_all();
		return 0;
	}else{
		targetMACAddress[0] = ptr->arpData.sha[0];
		targetMACAddress[1] = ptr->arpData.sha[1];
		targetMACAddress[2] = ptr->arpData.sha[2];
		targetMACAddress[3] = ptr->arpData.sha[3];
		targetMACAddress[4] = ptr->arpData.sha[4];
		targetMACAddress[5] = ptr->arpData.sha[5];
		del_all();
		return 1;
	}
}

//=====================================================================================
//
//	* Function : search_vendor()
//	* Description 
//		 MAC�ּ��� ���� 24bit�� �ش� ������ �� �� �ִ�. �� OUI�� �ش��ϴ� ������ oui.cpp��
//		���ǵ��ִ�. ����Ʈ�� ���ĵ� ���·� �ʱ�ȭ ���ֱ� ������ binary search Algorithm����
//		������ Ž���Ѵ�. ù��° ���ڴ� OUI����Ʈ�� ����ִ� ��� �ּҸ� �����Ѵ�. �ش� �����
//		oui.h�� ���ǵǾ� �ְ� oui.cpp�� �ʱ�ȭ�� �ִ�. �ι�° ���ڴ� Ž���� ���ּҸ� �Ѱ��ְ�
//		����° ���ڿ� �����̸��� ������ ���ڿ� ������ �Ѱ��ش�. 4��° ���ڴ� Ž���� ������
//		���� �����ε� ���� 0���� �����Ѵ�. ������ ���ڴ� Ž���� ������ �ε��� ������ ����ڰ�
//		���� ������ �� ������ ��ü ������ oui.h�� ���ǵ� �ִ�.
//
//=====================================================================================
void search_vendor(OUI_L* __oui_L, unsigned char* mac, char* vendor, int start, int end)
{
	int mid = (start+end)/2;
	int __sub_value = memcmp(__oui_L[mid].oui, mac, 3);
	if(start==mid){
		strcpy(vendor,"unknown");
		return;
	}
	if( __sub_value==0 ) { // match
		strcpy(vendor, __oui_L[mid].vendor);
		return;
	}
	else if( __sub_value < 0 ) // greater than mid
	{
		search_vendor( __oui_L, mac, vendor, mid, end);
	}
	else // smaller than mid
	{
		search_vendor( __oui_L, mac, vendor, start, mid);
	}
}

//=====================================================================================
//
//	* Function : get_hostname()
//	* Description 
//		 ���� ����̽��� ȣ��Ʈ ������ �д� �Լ��̴�. ��� IP�� ���� �������
//		���� ������ ������ ������ NBNS ��Ŷ�� ������ �м��Ͽ� hostname ������ �����Ѵ�.
//
//=====================================================================================
int get_hostname(unsigned char* _ipAddr, char* hostname)
{
	int retval;
	unsigned long ipAddr;
	struct nb_recv nbtbuf;
	unsigned char nbtType[2] = {0x00, 0x21};

	memcpy(&ipAddr, _ipAddr, 4);
	strcpy(hostname," ");

	//���� �ʱ�ȭ
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2,2), &wsa) != 0 )
		return -1;

	//socket()
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(sock == INVALID_SOCKET) return -1;

	int optval = 50; // ���ð� msec

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

	retval = sendto(sock, query, sizeof(query), 0,
		(SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if(retval==SOCKET_ERROR){
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
		strncpy(hostname, (char*)nbtbuf.name1, HOST_STR_SIZE);
		hostname[16] = 0;
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

//=====================================================================================
//
//	* Function : isStatic()
//	* Description 
//		 ���� �ý��ۿ� ���ڷ� ���޵Ǵ� �������ּҰ� �������� ������ �ִ����� Ȯ���ϴ� �Լ���.
//		���������� ������ �� '1'�� �����Ѵ�.
//
//=====================================================================================
int isStatic(u_char *ip)
{
	DWORD i;
	PMIB_IPNETTABLE pIpNetTable = NULL;
	DWORD dwSize = 0;
	DWORD dwRetVal = 0;
	DWORD dwResult;
	struct in_addr cmpIP,entry;
 
	memcpy(&cmpIP,ip,4);

	dwResult = GetIpNetTable(NULL, &dwSize, 0);
	/* Get the size required by GetIpNetTable() */
	if (dwResult == ERROR_INSUFFICIENT_BUFFER) {
		pIpNetTable = (MIB_IPNETTABLE *) malloc (dwSize);
	}
 
	 /* Now that we know the size, lets use GetIpNetTable() */
	if ((dwRetVal = GetIpNetTable(pIpNetTable, &dwSize, 0)) == NO_ERROR){
		if (pIpNetTable->dwNumEntries > 0) {
			for (i=0; i<pIpNetTable->dwNumEntries; i++) {
				entry = *(struct in_addr *)&pIpNetTable->table[i].dwAddr;
				if(memcmp(&entry,&cmpIP,4)==0 && pIpNetTable->table[i].dwType == 4)
					return 1;	
			}
		}
	}

	return 0;
}

//=====================================================================================
//
//	* Function : isMyLAN()
//	* Description 
//		 ���ڷ� ���޹��� ������ �ּҰ� ���� ������ LAN���� ���θ� Ȯ���Ѵ�.
//
//=====================================================================================
int isMyLAN(u_char *ip)
{
	u_char cmpNetID[IP_ALEN];

	cmpNetID[0] = ip[0] & netmask[0];
	cmpNetID[1] = ip[1] & netmask[1];
	cmpNetID[2] = ip[2] & netmask[2];
	cmpNetID[3] = ip[3] & netmask[3];

	if( memcmp(myNetID,cmpNetID,IP_ALEN)==0) // my lan
		return 1;
	else
		return 0;
}