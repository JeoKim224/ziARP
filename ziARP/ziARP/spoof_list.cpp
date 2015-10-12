///////////////////////////////////////////////////////////////////////////////////////
//
//	* 'ziArp' - a ARP poisoning attack detector
//	* 2012 Summer Handong Global University GHOST U3 Team Open Source Project
//	* File Name		: spoof_list.cpp
//	* Author		: �̽���-
//	* Date			: 2012. 7. 16 - 2012. 8. 4
//	* Description
//		 PSPOOF_L ����ü�� GUI ����â�� ��µǴ� ������ ������ �ִ� ��ũ�� ����Ʈ�̴�.
//		�ش� ����� PSPOOF_L ����Ʈ�� �ʿ��� ���� �Լ����� ������ �ִ�.
//		(�߰�, ����, ����, ������Ʈ, ����)
//		PSPOOF_L ����ü�� ����ϴ� ����Ʈ�� HEADER_ATTACK, HEADER_SUSPICIUS, HEADER_STATIC
//		3������ �ִ�.
//		HEADER_ATTACK �������� �����Ǵ� ��Ŷ ������ �����Ѵ�.
//		HEADER_SUSPICIUS �������� �ǽɵǴ� ��Ŷ ������ �����Ѵ�.
//		HEADER_STATIC	�������� ������ ����Ʈ�� �����Ѵ�.
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
#include <Windows.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include "header/datas.h"
#include "header/spoof_list.h"
#include "header/log.h"
#include "header/policy.h"
#include "header/get_info.h"

///////////////////////////////////////////////////////////////////////////////////////
//
//	Global Variables
//
///////////////////////////////////////////////////////////////////////////////////////
PSPOOF_L HEADER_ATTACK;
PSPOOF_L HEADER_SUSPICIUS;
PSPOOF_L HEADER_STATIC;

///////////////////////////////////////////////////////////////////////////////////////
//
//	Body
//
///////////////////////////////////////////////////////////////////////////////////////
//=====================================================================================
//
//	* Function : createSpoof
//	* Description 
//		����Ʈ�� ���� ����� �Ѱ� ���� ���ڷ� ������ ä���.
//		�� �� �ش� ����� �ּҸ� �����Ѵ�.
//
//=====================================================================================
PSPOOF_L createSpoof(PSPOOF_L h, char flag, unsigned char* ipAddr, unsigned char* macAddr, 
					char* vendor, char* timestr, char* hostName)
{
	PSPOOF_L node = (SPOOF_L*)malloc(sizeof(SPOOF_L));
	node->next = NULL;
	node->flag=flag;
	memcpy(node->ipAddr, ipAddr, IP_ALEN);
	memcpy(node->macAddr, macAddr, ETH_ALEN);
	strcpy(node->vendor, vendor);
	strcpy(node->timestr, timestr);
	strcpy(node->hostName, hostName);
	return node;
}

//=====================================================================================
//
//	* Function : isSpoof
//	* Description 
//		�Ѱ� ���� ����� ��ũ�� ����Ʈ���� Ư�� �����ǰ��� ������ ��尡 �����ϴ��� �����
//		�����Ѵ�. �����ϸ� '1'�� �׷��� ������ '0'�� �����Ѵ�.
//
//=====================================================================================
int isSpoof(PSPOOF_L h, unsigned char* ipAddr)
{
	PSPOOF_L p;
	int i=0;
	if(h==NULL) return 0;
	for( p=h; p !=NULL ; p=p->next){
		if(memcmp(p->ipAddr, ipAddr, IP_ALEN)==0) //exist
			return 1;
	}
	return 0;
}

//=====================================================================================
//
//	* Function : addSpoof
//	* Description 
//		�Ѱܹ��� flag(ATTACK, SUSPICIUS, NOMAL, STATIC)�� �ش��ϴ� ����Ʈ�� ���ο� ��带
//		�߰��Ѵ�. ���� ����Ʈ�� �������� ���� ��� ���� �����Ѵ�. �׷��� ���� ���� ����Ʈ
//		���� ���ο� ��带 �ٿ��ִ´�.
//
//=====================================================================================
void addSpoof(PSPOOF_L h, char flag, unsigned char* ipAddr, unsigned char* macAddr, 
			char* vendor, char* timestr, char* hostName)
{
	// ���� ����
	if(h==NULL){
		if( flag == ATTACK){
			PSPOOF_L temp = createSpoof(h,ATTACK,ipAddr,macAddr,vendor,timestr,hostName);
			HEADER_ATTACK = temp;
		}
		else if( flag == SUSPICIOUS){
			PSPOOF_L temp = createSpoof(h,SUSPICIOUS,ipAddr,macAddr,vendor,timestr,hostName);
			HEADER_SUSPICIUS = temp;
		}
		else if( flag == NOMAL) {
			PSPOOF_L temp = createSpoof(h,NOMAL,ipAddr,macAddr,vendor,timestr,hostName);
			ARP_LIST = temp;
		}
		else if( flag == STATIC) {
			PSPOOF_L temp = createSpoof(h,STATIC,ipAddr,macAddr,vendor,timestr,hostName);
			HEADER_STATIC = temp;
		}
		return;
	}

	// ���� ����Ʈ �ڿ� ���δ�.
	PSPOOF_L temp = (SPOOF_L*)malloc(sizeof(SPOOF_L));
	temp->next = NULL;
	temp->flag=flag;
	memcpy(temp->ipAddr, ipAddr, IP_ALEN);
	memcpy(temp->macAddr, macAddr, ETH_ALEN);
	strcpy(temp->vendor, vendor);
	strcpy(temp->timestr, timestr);
	strcpy(temp->hostName, hostName);

	// ��ũ�� ����Ʈ�� ������ �̵�
	PSPOOF_L p = h;
	while(p->next != NULL){
		p = p->next;
	}
	p->next = temp;
}

//=====================================================================================
//
//	* Function : updateSpoof
//	* Description 
//		�Ѱܹ��� ip �ּҿ� �ش��ϴ� ����� �ð��� ������Ʈ�Ѵ�.
//
//=====================================================================================
void updateSpoof(PSPOOF_L h, unsigned char* ipAddr, char* timestr)
{
	PSPOOF_L p = h;
	while(p!=NULL && memcmp(p->ipAddr, ipAddr, IP_ALEN)!=0){
		p = p->next;
	}
	if(p!=NULL){ // match
		strcpy(p->timestr, timestr);
	//	log("[Debugging] ���ݽð� Update ");
	}
}

//=====================================================================================
//
//	* Function : deleteSpoof
//	* Description 
//		�ش� ip �ּҸ� ����Ʈ���� �����Ѵ�.
//
//=====================================================================================
void deleteSpoof(PSPOOF_L h, unsigned char* ipAddr, char flag)
{
	PSPOOF_L p = h;
	PSPOOF_L temp;
	// ù��° ���(���)�� ��ġ�� ���
	if(memcmp(p->ipAddr,ipAddr,IP_ALEN) == 0){
		if(flag == ATTACK){
			temp = HEADER_ATTACK;
			HEADER_ATTACK = HEADER_ATTACK->next;
		}
		else if(flag == SUSPICIOUS){
			temp = HEADER_SUSPICIUS;
			HEADER_SUSPICIUS = HEADER_SUSPICIUS->next;
		}
		else if(flag == NOMAL){
			temp = ARP_LIST;
			ARP_LIST = ARP_LIST->next;
		}
		free(temp);
		return;
	}
	
	while(p->next !=NULL && memcmp(p->next->ipAddr,ipAddr,IP_ALEN)!=0 ) {
		p = p->next;
	}

	if(p->next != NULL){
		temp = p->next;
		p->next = p->next->next;
		free(temp);
	}
}

//=====================================================================================
//
//	* Function : isEmptySpoof
//	* Description 
//		�ش� ����Ʈ�� ����ִ��� üũ
//
//=====================================================================================
int isEmptySpoof(PSPOOF_L h)
{
	if(h==NULL)
		return 1;
	else return 0;
}

//=====================================================================================
//
//	* Function : refreshSpoofAttack, refreshSpoofSuspicius
//	* Description 
//		arpProc�Լ��� ���� ȣ��Ǵ� �Լ���. �� �Լ��� ������ ���� ���θ� üũ�ϰ� �� �����
//		�ݿ��Ѵ�. ����Ʈ�� �� ��忡�� �ð��� ����Ǿ� �ִµ� �̽ð��� ���ڷ� ���޵� �ð���
//		���Ͽ� ������ ���߾������� �Ǵ��Ѵ�. ���� ������ ���߾��ٰ� �ǴܵǾ��� �ÿ���
//		deleteSpoof�Լ��� ȣ���Ͽ� �ش� ��带 �����ϰ� �����쿡 Message�� �������� ����Ȳ��
//		�ݿ��Ѵ�.
//
//=====================================================================================
void refreshSpoofAttack(char* timestr)
{
	PSPOOF_L p;
	for(p=HEADER_ATTACK; p ; p = p->next){
		if( strcmp( timestr , p->timestr) > 0 ){ // time over
			unsigned char ipAddr[IP_ALEN];
			memcpy(ipAddr,p->ipAddr,IP_ALEN);

			/*����Ʈ���� ����*/
			deleteSpoof(HEADER_ATTACK, ipAddr, ATTACK);
			deleteSpoof(ARP_LIST, ipAddr, NOMAL);

			/*GUI�� �˸�*/
			SendMessage(hC1, WM_SPOOF, SM_INIT, (LPARAM)ARP_LIST);

			/*�α� ���*/
			char message[LOG_MSG_SIZE];
			sprintf(message, "[Info] spoofed packet stoped. chage state from attack to nomal. TargetIP: %d.%d.%d.%d "
				,ipAddr[0],ipAddr[1],ipAddr[2],ipAddr[3]);	
			log(message);
			break;
		}
	}
}

void refreshSpoofSuspicius(char* timestr)
{
	PSPOOF_L p;
	for(p=HEADER_SUSPICIUS; p; p=p->next){
		if( strcmp( timestr , p->timestr) > 0 ){ // time over
			unsigned char ipAddr[IP_ALEN];
			memcpy(ipAddr,p->ipAddr,IP_ALEN);		

			/*����Ʈ���� ����*/
			deleteSpoof(HEADER_SUSPICIUS, ipAddr, SUSPICIOUS);
			deleteSpoof(ARP_LIST,ipAddr, NOMAL);		

			/*GUI�� �˸�*/
			SendMessage(hC1, WM_SPOOF, SM_INIT, (LPARAM)ARP_LIST);

			/*�α� ���*/
			char message[LOG_MSG_SIZE];
			sprintf(message,"[Info] unrequested reply packet stoped. delete from the Suspicious list. TargetIP: %d.%d.%d.%d"
				,ipAddr[0],ipAddr[1],ipAddr[2],ipAddr[3]);
			log(message);
			break;
		}
	}
}

//=====================================================================================
//
//	* Function : updateState
//	* Description 
//		���º�ȭ(ex ����->����, ����->�ǽ�, �ǽ�->����, ����->����)�� �߻��� ��� 
//		�� ������ ��忡 �ݿ��ϰ� �����쿡 ��ȭ�� �߻������� �˸��� �޽����� ������.
//		�޽����� SM_CHANGE�� SM_ADD �ΰ����� �ִ�. SM_CHANGE�� ���� ����Ʈ�� �ش� ������
//		�̹� �����ϰ� ���º�ȭ�� �߻����� ����̰� SM_ADD�� ��������Ʈ�� �ش� ������ ����
//		��� ������ �޽�����. �� �޽��� ��� �����ϰ� �ش� ����� �Ķ���͸� ���ڷ� ������.
//
//=====================================================================================
void updateState(PSPOOF_L h, char flag, unsigned char* ipAddr, unsigned char* macAddr, 
			char* vendor, char* timestr, char* hostName)
{
	PSPOOF_L p = h;
	if( isSpoof(h, ipAddr) ){ //exist
		while(p!=NULL && memcmp(p->ipAddr, ipAddr, IP_ALEN)!=0){
			p = p->next;
		}
		if(p!=NULL){ // match
			p->flag = flag;
			memcpy(p->macAddr,macAddr,ETH_ALEN);
			strcpy(p->timestr,timestr);
			strcpy(p->vendor, vendor);
		}
		/*�޽��� �߻�*/
		if(flag==STATIC) log("[Info] Static update in updateState");
		SendMessage(hC1, WM_SPOOF, SM_CHANGE, (LPARAM)p);
	}
	else { // not exist
		addSpoof(h, flag, ipAddr, macAddr, vendor, timestr, hostName);
		while(p->next != NULL){
			p = p->next;
		}
		
		SendMessage(hC1, WM_SPOOF, SM_ADD, (LPARAM)p);
	}
}

//=====================================================================================
//
//	* Function : add_hostname
//	* Description 
//		add_hostname�� �ش� ������ �ּҿ� ���� ȣ��Ʈ������ ����Ʈ�� �ݿ��ϴ� �Լ��̴�.
//
//=====================================================================================
void add_hostname(PSPOOF_L h, unsigned char* ipAddr, char* hostname){

	PSPOOF_L p ;

	for( p=h; p ; p=p->next){
		if(memcmp(p->ipAddr, ipAddr, IP_ALEN)==0)
			memcpy(p->hostName, hostname, 17);
	}

}
