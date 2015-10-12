///////////////////////////////////////////////////////////////////////////////////////
//
//	* 'ziArp' - a ARP poisoning attack detector
//	* 2012 Summer Handong Global University GHOST U3 Team Open Source Project
//	* File Name		: getDevice.cpp
//	* Author		: �̽���-(Seungjae Lee)
//	* Date			: 2012. 7. 16 - 2012. 8. 4
//	* Description
//		winpcap�� �̿��Ͽ� �ش� ��ǻ���� ����̽� ����Ʈ�� ������ ���� 
//		��ũ�� ����Ʈ�� �߰��ϴ� ����� ����ִ�.
//
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//
//	'#include's
//
///////////////////////////////////////////////////////////////////////////////////////
#define HAVE_REMOTE
#include "header/datas.h"
#include "header/get_info.h"
#include "header/log.h"

///////////////////////////////////////////////////////////////////////////////////////
//
//	Prototypes
//
///////////////////////////////////////////////////////////////////////////////////////
void addDevice(PDEVICE_L h, char* deviceName, char* desc);
void getDevice();


///////////////////////////////////////////////////////////////////////////////////////
//
//	Body
//
///////////////////////////////////////////////////////////////////////////////////////
//=====================================================================================
//
//	* Function : getDevice()
//	* Description 
//		����� �ý��ۿ� Ȱ��ȭ�� �������̽��� ������ ���� �ش� ������ ��ũ�� ����Ʈ�� �����Ѵ�.
//
//=====================================================================================
void getDevice() {
	int i=0;
	char errbuf[PCAP_ERRBUF_SIZE];

	/* ���� ��ǻ���� ��Ʈ��ũ ����̽� ����Ʈ ������ */
	if(pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1){
		log("[Error] Error in pcap_findalldevs");
		exit(1);
	}
	
	/* Device ���� ���� */
	for(d = alldevs, i=0; d; d = d->next, i++){
		addDevice(deviceList,d->name, d->description);
	}
	/* ����̽� ����Ʈ�� �������� ����. winpcap�� ��ġ���� �ʾ��� ��쵵 �ִ�. */
	if(i == 0){
		log("[Error] No interfaces found! Make sure WinPcap is installed.");
		SendMessage(hC1, WM_SPOOF, SM_NODEVICE, NULL);
		return;
	}
}
//=====================================================================================
//
//	* Function : addDevice
//	* Description 
//		GUI�� ������ �������̽� ����Ʈ�� deviceList����� ����ȴ�.
//		addDevice�� getDevice�Լ����� ���Ǹ� deviceList����� �������̽��� �߰��ϴ� �Լ���.
//
//=====================================================================================
void addDevice(PDEVICE_L h, char* deviceName, char* desc)
{
	PDEVICE_L p;
	unsigned char ipAddr[IP_ALEN];
	unsigned char macAddr[ETH_ALEN];
	unsigned char netmask[IP_ALEN];

	get_info(deviceName,macAddr,ipAddr,netmask);

	if(h==NULL){
		deviceList = (PDEVICE_L)malloc(sizeof(DEVICE_L));
		deviceList->next = NULL;
		strcpy(deviceList->deviceName, deviceName);
		memcpy(deviceList->ipAddr, ipAddr,IP_ALEN);
		strcpy(deviceList->desc, desc);
	}
	else{
		p=h;
		while(p->next!=NULL){
			p = p->next;
		}
		PDEVICE_L temp = (PDEVICE_L)malloc(sizeof(DEVICE_L));
		temp->next=NULL;
		strcpy(temp->deviceName, deviceName);
		memcpy(temp->ipAddr, ipAddr,IP_ALEN);
		strcpy(temp->desc, desc);
		p->next = temp;
	}
}