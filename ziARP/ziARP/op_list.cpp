///////////////////////////////////////////////////////////////////////////////////////
//
//	* 'ziArp' - a ARP poisoning attack detector
//	* 2012 Summer Handong Global University GHOST U3 Team Open Source Project
//	* File Name		: op_list.cpp
//	* Author		: �̽���
//	* Date			: 2012. 7. 16 - 2012. 8. 4
//	* Description
//		 ARP REQUEST, ARP REPLY ��Ŷ�� ���� Queue�� �����Ѵ�.
//		 �ش� ����� ARP REQUEST, REPLY Queue�� ����, ����, �����ϴµ� �ʿ��� �Լ����
//		 ������ �ִ�.
//
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//
//	'#include's
//
///////////////////////////////////////////////////////////////////////////////////////
#include "header/op_list.h"
#include "header/policy.h"
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////////////
//
//	Global Variables
//
///////////////////////////////////////////////////////////////////////////////////////
PARPPKT_L HEADER_REPLY;		// ARP Request Queue header
PARPPKT_L HEADER_REQUEST;	// ARP Reply Queue header

///////////////////////////////////////////////////////////////////////////////////////
//
//	Body
//
///////////////////////////////////////////////////////////////////////////////////////
//=====================================================================================
//
//	* Function : CreateRequest(), CreateReply()
//	* Description 
//		ARP Request, Reply Queue ����
//
//=====================================================================================
PARPPKT_L createRequest(PARPPKT_L h, unsigned char* addr)
{
	PARPPKT_L node = (ARPPKT_L*)malloc(sizeof(ARPPKT_L));
	node->next = NULL;
	memcpy( node->arpData.tpa , addr, IP_ALEN);
	return node;
}
PARPPKT_L createReply(PARPPKT_L h, unsigned char* haddr, unsigned char* paddr)
{
	PARPPKT_L node = (ARPPKT_L*)malloc(sizeof(ARPPKT_L));
	node->next = NULL;
	node->ref = 1;
	memcpy( node->arpData.sha , haddr,ETH_ALEN);
	memcpy( node->arpData.spa, paddr,IP_ALEN);
	return node;
}

//=====================================================================================
//
//	* Function : isRequest(), isReply()
//	* Description 
//		ARP Request, Reply Queue�� �ش� �����ǰ� �����ϴ��� ���θ� �����Ѵ�.
//		Input: Header, ip address
//		Output: 1(Exist), 0(or not)
//
//=====================================================================================
int isRequest(PARPPKT_L h, unsigned char* addr)
{
	PARPPKT_L p;
	int i=0;
	if(h==NULL) return 0;
	for( p=h ; p != NULL; p = p->next){
		if( memcmp(p->arpData.tpa, addr, IP_ALEN) == 0 ) // exist
			return 1;
	}
	return 0;
}
int isReplyExist(PARPPKT_L h, unsigned char* addr)
{
	PARPPKT_L p;
	if(h==NULL) return 0;
	for( p=h ; p != NULL; p = p->next){
		if( memcmp(p->arpData.spa, addr, IP_ALEN) == 0 ) // exist
			return 1;
	}
	return 0;
}

//=====================================================================================
//
//	* Function : addRequest, addReply
//	* Description 
//		�ش� �����Ǹ� queue�� �߰��Ѵ�.
//
//=====================================================================================
void addRequest(PARPPKT_L h, unsigned char* addr)
{
	if(h==NULL){ // �ʱ� ����
		PARPPKT_L temp = createRequest(h, addr);
		HEADER_REQUEST = temp;
		return;
	}

	PARPPKT_L temp = (ARPPKT_L*)malloc(sizeof(ARPPKT_L));
	memcpy( temp->arpData.tpa, addr, IP_ALEN);
	temp->next = NULL;
	PARPPKT_L p = h;
	while(p->next != NULL){
		p = p->next;
	}
	p->next = temp;
}
void addReply(PARPPKT_L h, unsigned char* haddr, unsigned char* paddr)
{
	if(h==NULL){ // �ʱ� ����
		PARPPKT_L temp = createReply(h,haddr,paddr);
		HEADER_REPLY = temp;
		return;
	}

	PARPPKT_L temp = (ARPPKT_L*)malloc(sizeof(ARPPKT_L));
	memcpy(temp->arpData.sha, haddr, ETH_ALEN);
	memcpy(temp->arpData.spa, paddr, IP_ALEN);
	temp->ref = 1;
	temp->next = NULL;
	PARPPKT_L p = h;
	while(p->next != NULL){
		p = p->next;
	}
	p->next = temp;
}


//=====================================================================================
//
//	* Function : updateReply()
//	* Description 
//		�������� arp ������ ������ request�� ������ ��� reply�� ����´�.
//		�ش� �Լ��� ������ request�� �������� �ʾ��� ��� ȣ��ȴ�.
//		updateReply ȣ��� �ش� ip�ּҿ� ���� ref���� 1��ŭ ������Ų��.
//		�� �� policy.h�� ���ǵ� �ִ� ��å�� ���� ���ϰ��� ��������.
//		���� ref���� ATTACK_COUNT_POLICY �̻��� ��� 2�� �����ϰ�
//		SUSPICIUS_COUNT_POLICY �̻��� ��� 1�� �����ϰ�.
//		�׷��� ���� ���� 0�� �����Ѵ�. 
//		�� �Լ��� �����ϴ� ���� ���� �ش� Reply packet�� �ǽ�, ����, ���� ��Ŷ ���θ� �Ǵ��Ѵ�.
//
//=====================================================================================
int updateReply(PARPPKT_L h, unsigned char* addr)
{
	PARPPKT_L p = h;

	while(p!=NULL && memcmp(p->arpData.spa, addr, IP_ALEN)!=0 ){
		p = p->next;
	}

	// �ش� ip����
	if(p!=NULL) 
		p->ref++;

	// ref�� ����
	if( p->ref >= ATTACK_COUNT_POLICY )
		return 2;	//ATTACK
	else if( p->ref >= SUSPICIUS_COUNT_POLICY)
		return 1;	//SUSPICIOUS
	return 0;		//NOMAL
}

//=====================================================================================
//
//	* Function : deleteRequest(), deleteReply()
//	* Description 
//		�ش� ������ �ּҿ� �ش��ϴ� node�� queue���� �����Ѵ�.
//
//=====================================================================================
void deleteRequest(PARPPKT_L h, unsigned char* addr)
{
	PARPPKT_L p = h;
	PARPPKT_L temp;
	if(memcmp(p->arpData.tpa, addr, IP_ALEN) == 0){ // ù° node�� �ش� ip�ϰ��
		temp = HEADER_REQUEST;
		HEADER_REQUEST = HEADER_REQUEST->next;
		free(temp);
		return;
	}
	while( p->next != NULL && memcmp( p->next->arpData.tpa, addr, IP_ALEN)!=0){
		p = p->next;
	}
	if(p->next != NULL){
		temp = p->next;
		p->next = p->next->next;
		free(temp);
	}
}
void deleteReply(PARPPKT_L h, unsigned char* addr)
{
	PARPPKT_L p = h;
	PARPPKT_L temp;
	if(memcmp(p->arpData.spa, addr, IP_ALEN) == 0){
		temp = HEADER_REPLY;
		HEADER_REPLY = HEADER_REPLY->next;
		free(temp);
		return;
	}
	while( p->next != NULL && memcmp(p->next->arpData.spa, addr, IP_ALEN)!=0){ // ù° node�� �ش� ip�ϰ��
		p = p->next;
	}
	if(p->next != NULL){
		temp = p->next;
		p->next = p->next->next;
		free(temp);
	}
}
