///////////////////////////////////////////////////////////////////////////////////////
//
//	* 'ziArp' - a ARP poisoning attack detector
//	* 2012 Summer Handong Global University GHOST U3 Team Open Source Project
//	* File Name		: arp_scanning.c
//	* Author		: �ּ���-(Seoyul Choi)
//	* Date			: 2012. 7. 16 - 2012. 8. 4
//	* Description
//		 ���� ����� �� �ִ� ��� IP �ּҿ� ARP request ��Ŷ�� ����
//		reply�� ���� IP�� �����ϰ� ����� IP ����Ʈ�� NBNS ��Ŷ�� ����
//		ȣ��Ʈ ������ �߰��ϴ� �����̴�. ���� �Լ��� arp_scanning()�̴�.
//
///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
//
//	'#include's
//
///////////////////////////////////////////////////////////////////////////////////////

#ifndef HAVE_REMOTE
#define HAVE_REMOTE
#endif
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif
#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <IPHlpApi.h>
#include <iostream>
#include <windows.h>
#include <WinSock2.h>
#include "header/policy.h"
#include "header/get_info.h"
#include "header/spoof_list.h"
#include "header/log.h"

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "Iphlpapi.lib")

#define PACKET_SIZE 42

///////////////////////////////////////////////////////////////////////////////////////
//
//	Global Variables
//
///////////////////////////////////////////////////////////////////////////////////////

ARPPKT_L* ptr;
ARPPKT_L* head;

u_char hAddress[6];
u_char pAddress[4];

///////////////////////////////////////////////////////////////////////////////////////
//
//	Prototypes
//
///////////////////////////////////////////////////////////////////////////////////////

void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);
void add_1(const u_char *pkt, char* timestr);
void del_all();
void request_pkt(u_char *packet, u_char *hAddress, u_char *pAddress, u_char *netmask);
DWORD WINAPI f(LPVOID arg);
int get_remote_mac(u_char *targetIPAddr, u_char *targetMACAddress);
int get_hostname(unsigned char* _ipAddr, char* hostname);
void addStaticList();
DWORD WINAPI g(LPVOID arg);

///////////////////////////////////////////////////////////////////////////////////////
//
//	Body
//
///////////////////////////////////////////////////////////////////////////////////////

//=====================================================================================
//
//	* Function : arp_scanning()
//	* Description 
//		 ARP ���̺��� �ϼ��ϱ� ���� ���� �Լ�. ���� �����ִ� ����̽��� MAC �ּҿ� IP �ּҿ�
//		����� ����ũ�� �о�� request ��Ŷ�� �����Ѵ�. reply ��Ŷ�� ���� �� �ִ� �Լ���
//		ȣ���ϴ� �����带 �����ϰ� ���� ����� ���� ��� IP�� request ��Ŷ�� �߼��Ѵ�.
//		reply ���� �����尡 ����Ǹ� ȣ��Ʈ ������ �������� �����带 �����Ѵ�.
//
//=====================================================================================
void arp_scanning(){
	u_int i=0;
	u_char packet[PACKET_SIZE];
	u_char netmask[4];
	u_char netmask2[4];
	u_char ip_addr2[4];
	u_int range[2];
	HANDLE pcap_thread;

	head = (ARPPKT_L*)malloc(sizeof(ARPPKT_L));
	ptr = head;
	
	/* Get my hardware/protocol addresses */
	get_info(deviceName, &hAddress[0], &pAddress[0], &netmask[0]);

	/* request ��Ŷ ���� */
	request_pkt(packet, hAddress, pAddress, netmask);

	/* Convert big endian to little endian */
	netmask2[0] = netmask[3];
	netmask2[1] = netmask[2];
	netmask2[2] = netmask[1];
	netmask2[3] = netmask[0];
	ip_addr2[0] = pAddress[3];
	ip_addr2[1] = pAddress[2];
	ip_addr2[2] = pAddress[1];
	ip_addr2[3] = pAddress[0];

	u_int netmask3;
	u_int ip_addr3;
	memcpy(&netmask3, netmask2, 4);
	memcpy(&ip_addr3, ip_addr2, 4);
	
	/* ������Ʈ �߽� ����� ���� */
	range[0] = (ip_addr3 & netmask3) + 1;
	range[1] = (ip_addr3 & netmask3) + ~(netmask3) - 1;

	/* reply ���� �غ� */
	pcap_thread = CreateThread(NULL, 0, f, NULL, 0, NULL);

	/* ARP request ���� */
	for(i = range[0]; i <= range[1]; i++){
		packet[41] = i;
		if(pcap_sendpacket(adhandle, packet, PACKET_SIZE) != 0){
			return ;
		}
	}
	Sleep(WAITING_TIME);

	/* reply ���� ���� */
	TerminateThread(pcap_thread,NULL);
	CloseHandle(pcap_thread);

	/* ���� �ý����� ������������ �ݿ� */
	addStaticList();

	/* ��Ŷ ����Ʈ ���� */
	SendMessage(hC1, WM_SPOOF, SM_INIT, (LPARAM)ARP_LIST);
	del_all();	

	/* ȣ��Ʈ ���� �������� ������ ���� */
	CreateThread(NULL, 0, g, NULL, 0, NULL);	
	
	
	return;

}

//=====================================================================================
//
//	* Function : request_pkt()
//	* Description 
//		 ARP request ��Ŷ�� �����ϴ� �Լ�.
//
//=====================================================================================
void request_pkt(u_char *packet, u_char *hAddress, u_char *pAddress, u_char *netmask){
	
	/* ��ε�ĳ���� (fixed) */
	packet[0] = 0xFF;
	packet[1] = 0xFF;
	packet[2] = 0xFF;
	packet[3] = 0xFF;
	packet[4] = 0xFF;
	packet[5] = 0xFF;

	/* Get Source MAC Address */
	packet[6] = hAddress[0];
	packet[7] = hAddress[1];
	packet[8] = hAddress[2];
	packet[9] = hAddress[3];
	packet[10] = hAddress[4];
	packet[11] = hAddress[5];

	/* ��Ÿ��: arp (fixed) */
	packet[12] = 0x08;
	packet[13] = 0x06;

	/* �ϵ���� Ÿ��: Ethernet (fixed) */
	packet[14] = 0x00;
	packet[15] = 0x01;

	/* �������� Ÿ��: IPv4 (fixed) */
	packet[16] = 0x08;
	packet[17] = 0x00;

	/* �ϵ���� ������: 6 (fixed) */
	packet[18] = 0x06;

	/* �������� ������: arp (fixed) */
	packet[19] = 0x04;

	/* opcode: arp request (fixed) */
	packet[20] = 0x00;
	packet[21] = 0x01;

	/* Sender MAC Address */
	packet[22] = packet[6];
	packet[23] = packet[7];
	packet[24] = packet[8];
	packet[25] = packet[9];
	packet[26] = packet[10];
	packet[27] = packet[11];

	/* Sender IP Address */
	packet[28] = pAddress[0];
	packet[29] = pAddress[1];
	packet[30] = pAddress[2];
	packet[31] = pAddress[3];

	/* Target MAC Address (��ε�ĳ����) (fixed) */
	packet[32] = 0x00;
	packet[33] = 0x00;
	packet[34] = 0x00;
	packet[35] = 0x00;
	packet[36] = 0x00;
	packet[37] = 0x00;

	/* Target IP Address */
	packet[38] = packet[28] & netmask[0];
	packet[39] = packet[29] & netmask[1];
	packet[40] = packet[30] & netmask[2];
	packet[41] = packet[31] & netmask[3];

}

//=====================================================================================
//
//	* Function : packet_handler()
//	* Description 
//		 ����̽��� ��Ŷ�� ������ ������ ȣ��Ǵ� �ݹ� �Լ�.
//
//=====================================================================================
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data){

	struct tm *ltime;
	char timestr[50];
	time_t local_tv_sec;

	/* ��Ŷ ����: ARP reply (02) */
	if(pkt_data[20] == 0x00 && pkt_data[21] == 0x02){
		/* ��Ŷ ����: ������ IP �ּҰ� �� ��ǻ���� IP �ּҿ� ��ġ�ϴ� ��Ŷ */
		if(pkt_data[38] == pAddress[0] && pkt_data[39] == pAddress[1] && pkt_data[40] == pAddress[2] && pkt_data[41] == pAddress[3]){
			local_tv_sec = header->ts.tv_sec;
			ltime = localtime(&local_tv_sec);
			strftime(timestr, sizeof timestr, "%Y-%m-%d %p %I:%M:%S", ltime);

			/* ��Ŷ ���� ���� */
			add_1(pkt_data, timestr);
		}
	}
}

//=====================================================================================
//
//	* Function : add_1()
//	* Description 
//		 ��ũ�� ����Ʈ�� ARP reply ��Ŷ�� ������ �����Ͽ� ARP ���̺��� �����ϴ� �Լ�.
//
//=====================================================================================
void add_1(const u_char *pkt, char* timestr){

	ARPPKT_L *newNode = (ARPPKT_L*)malloc(sizeof(ARPPKT_L));
	unsigned char ipAddr[IP_ALEN];
	unsigned char macAddr[ETH_ALEN];
	char vendor[36];
	char hostname[16] = " ";

	/* sender hardware address */
	newNode->arpData.sha[0] = pkt[22];
	newNode->arpData.sha[1] = pkt[23];
	newNode->arpData.sha[2] = pkt[24];
	newNode->arpData.sha[3] = pkt[25];
	newNode->arpData.sha[4] = pkt[26];
	newNode->arpData.sha[5] = pkt[27];

	macAddr[0]=pkt[22];
	macAddr[1]=pkt[23];
	macAddr[2]=pkt[24];
	macAddr[3]=pkt[25];
	macAddr[4]=pkt[26];
	macAddr[5]=pkt[27];

	/* sender protocol address */
	newNode->arpData.spa[0] = pkt[28];
	newNode->arpData.spa[1] = pkt[29];
	newNode->arpData.spa[2] = pkt[30];
	newNode->arpData.spa[3] = pkt[31];

	ipAddr[0] = pkt[28];
	ipAddr[1] = pkt[29];
	ipAddr[2] = pkt[30];
	ipAddr[3] = pkt[31];

	newNode->next = NULL;

	ptr->next = newNode;
	ptr = newNode;
	
	if( memcmp(ipAddr, pAddress, IP_ALEN) != 0){
		search_vendor(oui_list, macAddr, vendor,0 ,OUI_COUNT-1);
		if(!isSpoof(ARP_LIST,ipAddr)){
			addSpoof(ARP_LIST,NOMAL,ipAddr,macAddr, vendor, timestr, hostname);
		}
	}
}

//=====================================================================================
//
//	* Function : del_all()
//	* Description 
//		 ARP ���̺� �Ҵ�� �޸𸮸� ���� �����ϴ� �Լ�.
//
//=====================================================================================
void del_all(){

	ARPPKT_L *delNode = (ARPPKT_L*)malloc(sizeof(ARPPKT_L));
	delNode = head;
	do{
		head = head->next;
		free(delNode);
		delNode = NULL;
	}while(head != ptr);

}

//=====================================================================================
//
//	* Function : f()
//	* Description 
//		 packet_handler() �Լ��� ȣ���ϴ� �����带 �����ϱ� ���� �Լ�.
//
//=====================================================================================
DWORD WINAPI f(LPVOID arg){

	/* �ݹ� �Լ� ȣ�� */
	pcap_loop(adhandle, 0, packet_handler, NULL);
	return 0;
}

//=====================================================================================
//
//	* Function : g()
//	* Description 
//		 ARP ���̺� ���� ��� IP �ּҿ� ���� ������ ��û�ϴ� get_hostname() �Լ���
//		ȣ���ϴ� �����带 �����ϱ� ���� �Լ�.
//
//=====================================================================================
DWORD WINAPI g(LPVOID arg){
	
	PSPOOF_L ptr = ARP_LIST;

	do{
		/* get hostname */
		get_hostname(ptr->ipAddr, ptr->hostName);

		if( strcmp(ptr->hostName," ") != 0){
			SendMessage(hC1, WM_SPOOF, SM_UPDATE_HOSTNAME, (LPARAM)ptr);
		}

		ptr = ptr->next;
	}while(ptr != NULL);

	return 0;
}

//=====================================================================================
//
//	* Function : addStaticList()
//	* Description 
//		 ARP ���̺��� �������� �����Ǿ��ִ� ��ġ�� ����Ʈ�� �����ϴ� �Լ�.
//
//=====================================================================================
void addStaticList()
{
	DWORD i;
	PMIB_IPNETTABLE pIpNetTable = NULL;
	DWORD dwSize = 0;
	DWORD dwRetVal = 0;
	DWORD dwResult;
	struct in_addr cmpIP,entry;

	PSPOOF_L p;
	unsigned char macAddr[ETH_ALEN];
	char timestr[50];
	char vendor[36];
	time_t timer;
	struct tm *t;

	dwResult = GetIpNetTable(NULL, &dwSize, 0);
	/* Get the size required by GetIpNetTable() */
	if (dwResult == ERROR_INSUFFICIENT_BUFFER) {
		pIpNetTable = (MIB_IPNETTABLE *) malloc (dwSize);
	}
	if ((dwRetVal = GetIpNetTable(pIpNetTable, &dwSize, 0)) == NO_ERROR){
		if (pIpNetTable->dwNumEntries > 0) 
		{
			for(p=ARP_LIST; p; p=p->next){
				memcpy(&cmpIP, p->ipAddr, IP_ALEN);
				for (i=0; i<pIpNetTable->dwNumEntries; i++) 
				{
					entry = *(struct in_addr *)&pIpNetTable->table[i].dwAddr;
					/* Static match */
					if(memcmp(&entry,&cmpIP,4)==0 && pIpNetTable->table[i].dwType == 4)
					{
						memcpy(macAddr,pIpNetTable->table[i].bPhysAddr,6);
						search_vendor(oui_list, macAddr, vendor, 0, OUI_COUNT-1);
						timer = time(NULL);
						t = localtime(&timer);
						strftime(timestr, sizeof(timestr),"%Y-%m-%d %p %I:%M:%S",t);
						/* Static linked list�� �߰� */
						addSpoof(HEADER_STATIC,STATIC,p->ipAddr,macAddr,vendor,timestr," ");

						/* �߰����� UI��º� ���� �޽��� �߻� */
						memcpy(ARP_DATA.ipAddr, p->ipAddr, IP_ALEN);
						memcpy(ARP_DATA.macAddr, macAddr, ETH_ALEN);
						strcpy(ARP_DATA.timestr, timestr);
						strcpy(ARP_DATA.vendor, vendor);
						SendMessage(hC1,WM_SPOOF, SM_UPDATE_LIST_STATIC, (LPARAM)&ARP_DATA);
					}
				}
			}
		}
	}
	else{
		log("[Error] Get static list failure");
		exit(1);
	}
	free(pIpNetTable);
}
