///////////////////////////////////////////////////////////////////////////////////////
//
//	* 'ziArp' - a ARP poisoning attack detector
//	* 2012 Summer Handong Global University GHOST U3 Team Open Source Project
//	* File Name	: get_info.h
//	* Author	: �̽���-(Seunjae Lee)
//				  �ּ���-(Seoyul Choi)
//	* Date		: 2012. 7. 16 - 2012. 8. 4
//	* Description
//		 ���� ����̽��� ���� ����̽��� ������ �д� �Լ��� ������Ÿ���� �����ϰ�
//		���ŵ� NBNS ��Ŷ�� ������ �����ϴ� ����ü�� ������ ��� �����̴�.
//
///////////////////////////////////////////////////////////////////////////////////////

#ifndef __GET_INFO_H__
#define __GET_INFO_H__

///////////////////////////////////////////////////////////////////////////////////////
//
//	'#include's
//
///////////////////////////////////////////////////////////////////////////////////////
#include "datas.h"
#include "policy.h"
#include "oui.h"
#include <pcap.h>

///////////////////////////////////////////////////////////////////////////////////////
//
//	'#define's
//
///////////////////////////////////////////////////////////////////////////////////////
#define BUFSIZE 1500
#define HOST_NAME_SIZE 100
#define NBT_PORT 137

///////////////////////////////////////////////////////////////////////////////////////
//
//	Global Variables
//
///////////////////////////////////////////////////////////////////////////////////////
struct nb_recv { 
	unsigned char dummy[46];
	unsigned char type[2];
	unsigned char start[9]; 
	unsigned char name1[16];
	unsigned char type1[2]; 
	unsigned char name2[16]; 
	unsigned char type2[2]; 
	unsigned char name3[16]; 
	unsigned char type3[2]; 
	unsigned char name4[16]; 
	unsigned char type4[2]; 
	unsigned char name5[16]; 
	unsigned char type5[2]; 
	unsigned char name6[16]; 
	unsigned char type6[2]; 
	unsigned char name7[16]; 
	unsigned char type7[2]; 
};
extern char query[50];
	
///////////////////////////////////////////////////////////////////////////////////////
//
//	Prototypes
//
///////////////////////////////////////////////////////////////////////////////////////
int get_hostname(unsigned char* ipAddr, char* hostname);
int get_info(char *dev_name, u_char *mac_addr, u_char *ip_addr, u_char *netmask);
void addr_aton(char *addr, u_char *netmask);
int get_remote_mac(u_char *targetIPAddr, u_char *targetMACAddress);
int get_hostname(unsigned char* _ipAddr, char* hostname);
int isStatic(u_char *ip);
int isMyLAN(u_char *ip);
void search_vendor(OUI_L* __oui_L, unsigned char* mac, char* vendor, int start, int end);

#endif
