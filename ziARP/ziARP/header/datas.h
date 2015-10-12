///////////////////////////////////////////////////////////////////////////////////////
//
//	* 'ziArp' - a ARP poisoning attack detector
//	* 2012 Summer Handong Global University GHOST U3 Team Open Source Project
//	* File Name		: datas.h
//	* Author		: �̽���-(Seungjae Lee)
//	* Date			: 2012. 7. 16 - 2012. 8. 4
//	* Description
//		 Ž�� ���� ��⿡�� �ʿ��� ������ �����ϰ� �ش� ����ü Ÿ�Կ� ���� ���Ǹ� �Ѵ�.
//	�ַ� �ڽ��� ��Ʈ��ũ ȯ�������� Ž���� ���Ǵ� interface�� �������� �����ϰ� �ִ�.
//  datas.cpp�� �ִ� ������ ����ϱ� ���ؼ��� �ش� ��带 include���Ѿ� �Ѵ�.
//
///////////////////////////////////////////////////////////////////////////////////////
#ifndef __GLOBAL__
#define __GLOBAL__
///////////////////////////////////////////////////////////////////////////////////////
//
//	'#include's
//
///////////////////////////////////////////////////////////////////////////////////////
#include <pcap.h>
#include "spoof_list.h"
///////////////////////////////////////////////////////////////////////////////////////
//
//	'#define's and other preprocessing codes
//	 SendMessage�� ���� �޽��� Ÿ���� ����
//
///////////////////////////////////////////////////////////////////////////////////////
#define STRSIZE 100
#define WM_SPOOF (WM_USER+1)
#define SM_INIT 0
#define SM_ADD 1
#define SM_CHANGE 2
#define SM_STOP_THREAD 3
#define SM_RESUME_THREAD 4
#define SM_NODEVICE 40
#define SM_UPDATE_HOSTNAME		50
#define SM_UPDATE_LIST_NOMAL	51
#define SM_UPDATE_LIST_SUSP		52
#define SM_UPDATE_LIST_ATTACK	53
#define SM_UPDATE_LIST_STATIC	54

///////////////////////////////////////////////////////////////////////////////////////
//
//	'typedef's
//
///////////////////////////////////////////////////////////////////////////////////////
// �������̽��� ������ ��� ����Ʈ
typedef struct DEVICE_L {
	DEVICE_L* next;
	char deviceName[STRSIZE];
	unsigned char ipAddr[IP_ALEN];
	char desc[STRSIZE];
} DEVICE_L, *PDEVICE_L;

// ���º����� �˸���(updateState())�� ���� ����ü
typedef struct ARPDATA {
	unsigned char ipAddr[IP_ALEN];
	unsigned char macAddr[ETH_ALEN];
	char vendor[36];
	char timestr[50];
} ARPDATA;

///////////////////////////////////////////////////////////////////////////////////////
//
//	Global Variables
//
///////////////////////////////////////////////////////////////////////////////////////
extern struct sockaddr *addr;
extern struct sockaddr_in *in;

// �ڽ��� ��Ʈ��ũ ȯ�� ����
extern unsigned char myIPaddr[IP_ALEN];
extern unsigned char if_mac[ETH_ALEN];
extern unsigned char netmask[IP_ALEN];
extern unsigned char myNetID[IP_ALEN];
extern char buff[512];

// GUI ���� ����Ʈ�� ��µǴ� ������ ����ִ� ��ũ�� ����Ʈ ���
extern PSPOOF_L ARP_LIST;

// Winpcap�� �̿��Ͽ� ��Ŷ�� �ۼ����� �ڵ�
extern pcap_t *adhandle;

// Winpcap�� �̿��Ͽ� ����� �������̽� ����̽� �̸�
extern char deviceName[];

// Winpcap�� �̿��Ͽ� ������ ��� �������̽� ����Ʈ
extern pcap_if_t *alldevs;
// ������ �������̽� ���
extern pcap_if_t *d;

// ���� ����� ��ǻ���� �������̽� ����� ����ִ� ��ũ�� ����Ʈ
extern PDEVICE_L deviceList;

// ����(����,���,����,����)�� ��ȭ�� �˸� ��(updateState�Լ� <spoof_list.cpp>) �ش� ������ ��� ����
extern ARPDATA ARP_DATA;

// GUI�� ������ �ڵ�
extern HWND hC1;
#endif

