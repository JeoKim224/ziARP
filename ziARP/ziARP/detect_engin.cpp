///////////////////////////////////////////////////////////////////////////////////////
//
//	* 'ziArp' - a ARP poisoning attack detector
//	* 2012 Summer Handong Global University GHOST U3 Team Open Source Project
//	* File Name		: detect_engin.cpp
//	* Author		: �̽���-(Seungjae Lee)
//	* Date			: 2012. 7. 16 - 2012. 8. 4
//	* Description
//	  ARP packet�� ���� ���� ��Ŷ Ȥ�� ������ ��Ŷ ���θ� �Ǵ��ϴ� ��������̴�.
//	  ���������� ��Ŷ�� ��û�ϸ鼭 ���������� arp��Ŷ�� �Ǵ��ϰ� �� ����� Message�� �˷��ش�.
//	  �Ϲ������� �������� arp ��Ŷ�� request�� ���� reply�� �����Ѵ�.
//	  �ش� ������ ������ request��Ŷ�� ����Ʈ�� �����ϰ� ������ reply��Ŷ�� ������ ���θ� �Ǵ��Ѵ�.
//	  ��, request���� ���� reply ��Ŷ�� ������ ��Ŷ���� �����ϰ� ����Ʈ�� ���� �����Ѵ�.
//	  ������ ��Ŷ�� ���� ����Ʈ�� ũ�� SUSPICIUS, ATTACK �ΰ����� �ִ�.
//	  �ش� IP �ּҿ� ���� ������ ��Ŷ�� ���� �������� SUSPICIUS�� ATTACK ���°� �ǴܵǴµ�
//	  �� ������ ���� ��å�� policy.h�� define�Ǿ� �ִ�. 
//
///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
//
//	'#define's and other preprocessing codes
//
///////////////////////////////////////////////////////////////////////////////////////
#define HAVE_REMOTE
#define MAX_BUF_SIZE 1024
#define snprintf _snprintf

#pragma comment(lib, "wpcap.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Iphlpapi.lib")

///////////////////////////////////////////////////////////////////////////////////////
//
//	'#include's
//
///////////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>
#include <pcap.h>
#include <IPHlpApi.h>
#include "header/op_list.h"
#include "header/spoof_list.h"
#include "header/policy.h"
#include "header/datas.h"
#include "header/get_info.h"
#include "header/log.h"

///////////////////////////////////////////////////////////////////////////////////////
//
//	Prototypes
//
///////////////////////////////////////////////////////////////////////////////////////
void packet_handler_reply(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);
int get_if_mac(const char *dev_name, u_char *mac_addr);	// �ش� ����̽��� ���ּ� �������� �Լ�
void arpProc(PARPHDR pARPData, time_t local_tv_sec);
///////////////////////////////////////////////////////////////////////////////////////
//
//	Body
//
///////////////////////////////////////////////////////////////////////////////////////
//=====================================================================================
//
//	* Function : detect_engin
//	* Description 
//		LAN���θ� üũ�Ҷ� ����ϱ� ���� �ڽ��� Network ID�� ����Ͽ� �����Ѵ�.
//		Winpcap�� ����Ͽ� �ش� �ڵ鿡�� ��Ŷ ������ ��� packet_handle_reply�� ȣ���Ѵ�.
//
//=====================================================================================
int detect_engin(char *lAdapter)
{
	HEADER_REQUEST = NULL;
	HEADER_REPLY = NULL;
	
	/* ��Ʈ��ũ ������ ������������ �����Ѵ� */
	// if_mac: �������̽��� ���ּ� 
	// myIPaddr: �������̽��� IPv4 �ּ�
	// netmask : �������̽��� �ݸ���ũ �ּ�
	// myNetID : ���� �ڽ��� ��Ʈ��ũ ID�� (���� ����� ����üũ�� ���)
	get_info(lAdapter, if_mac, myIPaddr, netmask);
	myNetID[0] = myIPaddr[0] & netmask[0];
	myNetID[1] = myIPaddr[1] & netmask[1];
	myNetID[2] = myIPaddr[2] & netmask[2];
	myNetID[3] = myIPaddr[3] & netmask[3];

	// Start intercepting data packets.
	pcap_loop(adhandle, 0, packet_handler_reply, NULL);

	return 0;
}

//=====================================================================================
//
//	* Function : detect_engin
//	* Description
//		Callback function invoked by winpcap for every incoming packet 
//		Ether header�� �˻��Ͽ� arp��Ŷ�� �����ϰ� �ش� ��Ŷ�� arpProc���� ó���ϵ��� �Ѵ�.
//
//=====================================================================================
void packet_handler_reply(u_char* param, const struct pcap_pkthdr* header,
					const u_char *pkt_data)
{
	time_t local_tv_sec; // �ð�
	
	PETHDR lEHdr = (PETHDR) pkt_data;
	local_tv_sec = header->ts.tv_sec; // winpcap ���� �Ѱ��� ������� �ð� ����

	switch(htons(lEHdr->ether_type)) {
	case 0x0806 : // ARP Packet
		arpProc((PARPHDR)(pkt_data+14), local_tv_sec);
	}
}

//=====================================================================================
//
//	* Function : arpProc
//	* Description 
//		�Ѿ�� arp��Ŷ�� ������ ���θ� �Ǵ��ϴ� �Լ��̴�. arp��Ŷ �ش��� �ش� ��Ŷ�� ������ 
//		�ð��� ���ڷ� �Ѿ�´�. �ش� ��Ŷ�� request ��Ŷ�̸� request queue�� �߰��ϰ�
//		reply ��Ŷ�̸� request queue�� �����ϴ��� Ȯ���Ѵ�. �������� �ʴ´ٸ� ������ ��Ŷ�̴�.
//		������ ��Ŷ�� ��� reply queue�� �ش� ��带 ������Ʈ �����ְ� ���Ϲ޴� ������ state��
//		�Ǵ��Ѵ�.
//		�׸��� ATTACK, SUSPICIUS LIST�� �ð��� �ش� ������ ��Ŷ�� ������ ������ ������Ʈ �����ش�.
//		policy.h�� ���ǵ� �ð���ŭ ���ݽð��� ������Ʈ ���� �ʾ��� ��� ������ ��������� �����ϰ�
//		����Ʈ���� �����ϰ� ������·� �����Ѵ�.
//
//=====================================================================================
void arpProc(PARPHDR pARPData, time_t local_tv_sec) {

	int isSusp=0, isAttack=0; // ������ ��Ŷ�� ��� �ش� ��Ŷ�� ���¸� ���� ����
	char vendor[36];

	/*�ð� ���� ����*/
	char timestr[50];		  // ���� �ð�
	char cmptime[50];		  // �� �ð�: �������ð�(Policy.h�� ���ǵ�)��ŭ �ð��� update���� ���� ��� ������ ��������� �����Ѵ�.
	struct tm *ltime;
	struct tm *lptime;
	struct tm stTempTime;

	//Log �޽����� ����
	char logmsg[LOG_MSG_SIZE];

	// ��Ŷ ���� �ð� ����
	ltime = localtime(&local_tv_sec);
	memcpy(&stTempTime, ltime, sizeof(struct tm));

	/* �ð� ���� �����Ͽ� ���� */
	local_tv_sec = local_tv_sec - ATTACK_TIME_OVER_SEC;
	lptime = localtime(&local_tv_sec);

	// ������ �ð� ������ ���ڿ� ���·� ����
	strftime( timestr, sizeof(timestr), "%Y-%m-%d %p %I:%M:%S", &stTempTime);
	strftime( cmptime, sizeof(cmptime), "%Y-%m-%d %p %I:%M:%S", lptime);

	// ARP Request Packet from myInterface
	if( ntohs(pARPData->opcode) == ARP_REQUEST 
		&& memcmp(pARPData->spa , myIPaddr, IP_ALEN)==0 ) {

		// request queue�� �������� ���� ��� �߰�
		if( !isRequest(HEADER_REQUEST, pARPData->tpa) ){
			addRequest(HEADER_REQUEST, pARPData->tpa);
		}
	}

	//ARP Reply Packet
	else if( ntohs(pARPData->opcode) == ARP_REPLY 
		&& memcmp(pARPData->spa, myIPaddr, IP_ALEN)!=0 ) { 

		if(!isMyLAN(pARPData->spa)) // �ٸ� LAN�̸� ����
			return;

		/* Normal Packet Receive */
		if( isRequest(HEADER_REQUEST, pARPData->spa) ) {
			deleteRequest(HEADER_REQUEST, pARPData->spa);	// Delete request from the list

			/* Add as Normal Pakcet */
			search_vendor(oui_list, pARPData->sha, vendor,0,OUI_COUNT-1);

			/* ARP_LIST�� �ݿ� */
			memcpy(ARP_DATA.ipAddr, pARPData->spa, IP_ALEN);
			memcpy(ARP_DATA.macAddr, pARPData->sha, ETH_ALEN);
			strcpy(ARP_DATA.timestr, timestr);
			strcpy(ARP_DATA.vendor, vendor);
			SendMessage(hC1, WM_SPOOF, SM_UPDATE_LIST_NOMAL, (LPARAM)&ARP_DATA);

			/* reply packet delete from the list */
			if( isReplyExist(HEADER_REPLY, pARPData->spa) ) {
				deleteReply(HEADER_REPLY, pARPData->spa);
			}

			/* ���� ��Ŷ�� ���� ���� ������ �ǽ� Ȥ�� ���ݻ�Ȳ�̾��� ��� ����Ʈ���� �����Ѵ�. */
			if( isSpoof(HEADER_SUSPICIUS, pARPData->spa)) {
				deleteSpoof(HEADER_SUSPICIUS, pARPData->spa, SUSPICIOUS);
			}
			if( isSpoof(HEADER_ATTACK, pARPData->spa)) {
				deleteSpoof(HEADER_ATTACK, pARPData->spa, ATTACK);
			}
		}

		/* Abnormal Packet Receive */
		else { 
			int state; // ����(����,�ǽ�,����) ����
			if(isReplyExist(HEADER_REPLY, pARPData->spa)){ 
				// �������� ���ŵ� �������� reply ��Ŷ�� ��� ������Ʈ
				state = updateReply(HEADER_REPLY, pARPData->spa);	
			}
			else {
				// ó�� ���ŵ� ��� �ǽɻ��·� �����ϰ� reply queue�� �߰�
				state=SUSPICIOUS;
				addReply(HEADER_REPLY, pARPData->sha, pARPData->spa);
			}
			switch(state){ // ���¿� ���� switch
				//Suspicius packet
				case SUSPICIOUS: 
					// vendor ����
					search_vendor(oui_list, pARPData->sha, vendor,0,OUI_COUNT-1);
					isSusp = isSpoof(HEADER_SUSPICIUS, pARPData->spa);

					// �ǽ� ����Ʈ�� ������ �߰�
					// static���� ��� ��ġ�� ���� ���ܷ� �Ѵ�.
					if(!isSusp && !isSpoof(HEADER_STATIC, pARPData->spa) ) { 

						// �߰�
						addSpoof(HEADER_SUSPICIUS, SUSPICIOUS, pARPData->spa, pARPData->sha, vendor, timestr, " ");
						
						/* �ǽ���Ŷ ����->arplist�� �ݿ� */
						memcpy(ARP_DATA.ipAddr, pARPData->spa, IP_ALEN);
						memcpy(ARP_DATA.macAddr, pARPData->sha, ETH_ALEN);
						strcpy(ARP_DATA.timestr, timestr);
						strcpy(ARP_DATA.vendor, vendor);						
						SendMessage(hC1, WM_SPOOF, SM_UPDATE_LIST_SUSP, (LPARAM)&ARP_DATA);

						// �αױ��
						sprintf(logmsg,"[Warning] Unrequested arp reply packet is detected. TargetIP: %d.%d.%d.%d"
							,pARPData->spa[0],pARPData->spa[1],pARPData->spa[2],pARPData->spa[3], timestr, vendor);
						log(logmsg);
					}
					else if(isSusp){
						 // ������ ����Ʈ�� ������ ��� �ð� ������Ʈ
						updateSpoof(HEADER_SUSPICIUS, pARPData->spa, timestr);
					}
					break;
				//Attack Packet
				case ATTACK: 
					// vendor ����
					search_vendor(oui_list, pARPData->sha, vendor,0,OUI_COUNT-1);
					isAttack = isSpoof(HEADER_ATTACK, pARPData->spa);

					// �ǽɻ��� ����Ʈ�� ������ ����
					if(isSpoof(HEADER_SUSPICIUS, pARPData->spa)){
						deleteSpoof(HEADER_SUSPICIUS, pARPData->spa, SUSPICIOUS);
					}

					// ���� ����Ʈ�� ������ �߰�
					// static���� ��� ��ġ�� ���� ���ܷ� �Ѵ�.
					if(!isAttack && !isSpoof(HEADER_STATIC, pARPData->spa)){ 
						addSpoof(HEADER_ATTACK, ATTACK, pARPData->spa, pARPData->sha, vendor, timestr, "");

						/* ������Ŷ ���� -> arplist�� �ݿ�*/
						memcpy(ARP_DATA.ipAddr, pARPData->spa, IP_ALEN);
						memcpy(ARP_DATA.macAddr, pARPData->sha, ETH_ALEN);
						strcpy(ARP_DATA.timestr, timestr);
						strcpy(ARP_DATA.vendor, vendor); 
						SendMessage(hC1, WM_SPOOF, SM_UPDATE_LIST_ATTACK, (LPARAM)&ARP_DATA); 

						// �αױ��
						sprintf(logmsg,"[Danger] arp spoofing attack packet is detected. TargetIP: %d.%d.%d.%d"
							,pARPData->spa[0],pARPData->spa[1],pARPData->spa[2],pARPData->spa[3], timestr, vendor);
						log(logmsg);
					}
					else if(isAttack){
						// ������ ����Ʈ�� ������ ��� �ð� ������Ʈ
						updateSpoof(HEADER_ATTACK, pARPData->spa, timestr);
					}
				};
		}
	}
	// ������ ���߾����� ���θ� üũ
	refreshSpoofAttack(cmptime);
	refreshSpoofSuspicius(cmptime);
}