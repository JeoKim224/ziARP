///////////////////////////////////////////////////////////////////////////////////////
//
//	* 'ziArp' - a ARP poisoning attack detector
//	* 2012 Summer Handong Global University GHOST U3 Team Open Source Project
//	* File Name		: openDevice.cpp
//	* Author		: �̽���-(Seungjae Lee)
//					  ��ο�-(Minwoo Kim)
//					  �ּ���-(Seoyul Choi)
//	* Date			: 2012. 7. 16 - 2012. 8. 4
//
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//
//	'#include's
//
///////////////////////////////////////////////////////////////////////////////////////
#define HAVE_REMOTE
#include "header/datas.h"
#include "header/policy.h"
#include "header/log.h"
#include <pcap.h>
#include <memory.h>

#define PACKET_SIZE 42
void CharToTCHAR(char* char_str, TCHAR* TCHAR_str);
///////////////////////////////////////////////////////////////////////////////////////
//
//	Body
//
///////////////////////////////////////////////////////////////////////////////////////
//=====================================================================================
//
//	* Function : openDevice()
//	* Description
//		 �ش� ����� ����ڰ� ������ ����̽��� ���� �ش� �ڵ��� datas.cpp�� �����
//		adhandle�� �����Ѵ�.
//
//=====================================================================================
int openDevice(int index){

	char errbuf[PCAP_ERRBUF_SIZE];
	int counter;
	char logmsg[LOG_MSG_SIZE];
	unsigned int lNetMask = 0;
	struct bpf_program lFCode;

	for(d=alldevs, counter=0; d; d=d->next, counter++){
		if(counter==index){
			strncpy(deviceName,d->name,1024);
			break;
		}
	}

	/* ����̽� ���� */
	if((adhandle = pcap_open(deviceName, 65536, PCAP_OPENFLAG_PROMISCUOUS, 1000, NULL, errbuf)) == NULL){
		sprintf(logmsg,"[Error] Unable to open the adapter. not supported by WinPcap\n");
		log(logmsg);
		SendMessage(hC1, WM_SPOOF, SM_NODEVICE, NULL);
		pcap_freealldevs(alldevs);
		return -1;
	}

	if (d->addresses != NULL)
		lNetMask = ((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
	else
		lNetMask = 0xffffff;

	ZeroMemory(&lFCode, sizeof(lFCode));
	if(pcap_compile(adhandle, &lFCode, "", 1, lNetMask) < 0 ) {
		log("[Error] Unable to compile the packet filter. \n");
		exit(1);
	}

	if(pcap_setfilter(adhandle, &lFCode) < 0 ) {
		log("[Error] Error setting the filter.\n");
	}

	/* �αױ�� */
	sprintf(logmsg, "[Info] %s Device Opened\n\r", d->description);
	log(logmsg);
	/* ��Ʈ��ũ ����̽� ��� ���� */
	pcap_freealldevs(alldevs);
	return 0;
}