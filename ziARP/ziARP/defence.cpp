///////////////////////////////////////////////////////////////////////////////////////
//
//	* 'ziArp' - a ARP poisoning attack detector
//	* 2012 Summer Handong Global University GHOST U3 Team Open Source Project
//	* File Name		: defence.cpp
//	* Author		: ��ο�
//	* Date			: 2012. 7. 16 - 2012. 8. 4
//	* Description
//		ARP Spoofing ���� Ž�� �� ����ϴ� �Լ����� �����̴�. �ŷ��� �� �ִ� IP-MAC �ּҸ�  
//		ARP table�� static �����ϴ� ����� ����Ͽ���.
//
///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
//
//	'#include's
//
///////////////////////////////////////////////////////////////////////////////////////
#include <pcap.h>
#include <string.h>
#include <windows.h>
#include <iprtrmib.h>
#include <tlhelp32.h>
#include <iphlpapi.h>
#include <time.h>
#include "header/log.h"
#include "header/datas.h"
#include "header/get_info.h"
#include "header/oui.h"
#include <ShellAPI.h>
#include <atlstr.h>

///////////////////////////////////////////////////////////////////////////////////////
//
//	Prototypes
//
///////////////////////////////////////////////////////////////////////////////////////
/* Capture ARP Reply */
void packet_handler_D(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);
void capture_arp(u_char *tip);
void addListD(char *ip,char *mac);
int search(char *mac);

DWORD WINAPI listen(LPVOID arg);

/* Send ARP Request */
void arp_request(u_char *ip);
void make_packet(u_char *packet,u_char *ip);
DWORD WINAPI request(LPVOID arg);

void init();
int defence(int index);
int set_static(struct sf_packet);

///////////////////////////////////////////////////////////////////////////////////////
//
//	Global Variables
//
///////////////////////////////////////////////////////////////////////////////////////
/* Ethernet Header */
struct eth_hdr{
	u_char h_dest[6];
	u_char h_src[6];
	u_short h_proto;
};
/* ARP Header */
struct arp_hdr{
	u_short arp_htype;
	u_short arp_ptype;
	u_char arp_hsize;
	u_char arp_psize;
	u_short arp_opcode;
	u_char arp_smac[6];
	u_char arp_sip[4];
	u_char arp_tmac[6];
	u_char arp_tip[4];
};
/* Packet Info */
struct sf_packet{
	char sf_ip[16];
	char sf_mac[18];
	int cnt;
};

char msg_mac[18];
char msg_ip[16];
struct sf_packet packets[5];
u_char target_ip[4];
u_char target_mac[6];
u_char attk_mac[6];

///////////////////////////////////////////////////////////////////////////////////////
//
//	Body
//
///////////////////////////////////////////////////////////////////////////////////////

//=====================================================================================
//
//	* Function : getTargetIP()
//	* Description 
//		HEADER_ATTACK ����Ʈ���� �μ��� ���� �� index�� �ش��ϴ� IP�� target_ip��, �ش��ϴ�
//		MAC�� target_mac�� �����Ѵ�.
//
//=====================================================================================
void getTargetIP(int index)
{
	PSPOOF_L p;
	int i=0;
	for(p=HEADER_ATTACK; p; p=p->next, i++){
		if(index==i){
			memcpy(target_ip, p->ipAddr, IP_ALEN);
			memcpy(attk_mac, p->macAddr, ETH_ALEN);
			break;
		}
	}
}

//=====================================================================================
//
//	* Function : defence()
//	* Description 
//		�� ���� �����带 ������ �ϳ��δ� target_ip�� ARP Request ��Ŷ�� ������ �ٸ� �ϳ��δ�
//		���� Request ��Ŷ�� ���� Reply ��Ŷ�� ���͸��� �������� packets�� �����ϵ��� �Ѵ�. 
//		�������� packets�� ���� �� ARP reply ��Ŷ �� ARP Spoofing �����ڰ� �ƴ� IP�� ������ 
//		��Ŷ�� ã�´�. ã�� ��Ŷ�� IP �ּҿ� MAC �ּҸ� set_static �Լ��� ����� ARP table��
//		static���� �����Ѵ�. 
//=====================================================================================
int defence(int index)
{
	HANDLE handles[2];
	int i,largest=0;
	char att_mac[18];
	char infolog[250];
	getTargetIP(index);
	sprintf_s(att_mac,18,"%02x-%02x-%02x-%02x-%02x-%02x",attk_mac[0],attk_mac[1],attk_mac[2],attk_mac[3],attk_mac[4],attk_mac[5]);
	init();
	handles[0] = CreateThread(NULL,0,listen,(LPVOID)target_ip,0,NULL);
	handles[1] = CreateThread(NULL,0,request,(LPVOID)target_ip,0,NULL);
	WaitForMultipleObjects(2,handles,true,2000);
	for(i=0; i<5; i++)
	{
		if(packets[i].cnt != 0 && memcmp(packets[i].sf_mac,att_mac,18)!=0)
		{
			sprintf(msg_mac,"%s",packets[i].sf_mac);
			sprintf(msg_ip,"%s",packets[i].sf_ip);
			if(set_static(packets[i])){
				sprintf(infolog,"[Info] Static setting complete. TargetIP:%d.%d.%d.%d %02X:%02X:%02X:%02X:%02X:%02X -> %02X:%02X:%02X:%02X:%02X:%02X",
					target_ip[0],target_ip[1],target_ip[2],target_ip[3],att_mac[0],att_mac[1],att_mac[2],att_mac[3],att_mac[4],att_mac[5],
					target_mac[0],target_mac[1],target_mac[2],target_mac[3],target_mac[4],target_mac[5]);
				log(infolog);
				SendMessage(hC1,WM_SPOOF, SM_RESUME_THREAD, NULL);
				return 1;
			}
		}
	}
	log("[Info] Static setting failure");
	SendMessage(hC1,WM_SPOOF, SM_RESUME_THREAD, NULL);
	return 0;	
}

//=====================================================================================
//
//	* Function : init()
//	* Description 
//		�������� packets�� �ʱ�ȭ�Ѵ�.
//		Ž�� ���� �����带 ������Ų��.(SendMessage)
//
//=====================================================================================
void init()
{
	memset(packets,0,sizeof(packets));
	SendMessage(hC1,WM_SPOOF, SM_STOP_THREAD, NULL);
}

//=====================================================================================
//
//	* Function : listen()
//	* Description 
//		ARP Reply packet�� ���͸� �Ͽ� packets�� �����ϴ� �������� �ݹ��Լ� 
//		
//=====================================================================================
DWORD WINAPI listen(LPVOID arg)
{
	capture_arp(target_ip);
	
	return 0;
}

//=====================================================================================
//
//	* Function : request
//	* Description 
//		target_ip�� ARP request packet�� ������ �������� �ݹ��Լ� 
//
//=====================================================================================
DWORD WINAPI request(LPVOID arg)
{
	arp_request(target_ip);

	return 0;
}

//=====================================================================================
//
//	* Function : set_static()
//	* Description 
//		target_ip�� target_mac�� ARP table �� static���� �����Ѵ�. 
//		
//=====================================================================================
int set_static(struct sf_packet packet)
{
	char* dev_name = deviceName;
	int ad_index=0;
	time_t timer;
	struct tm *t;
	char hostname[17];
	char vendor[36];
	char timestr[50];

	ULONG len =0;
	PIP_ADAPTER_INFO p = NULL; // NetWork Adapter list
	DWORD dw = 0;

	SHELLEXECUTEINFO si; // Shell ��ɾ ��� ���� ����ü
	CString path; // static ���� ��ɾ�

	ZeroMemory(&si,sizeof(SHELLEXECUTEINFO));
	si.cbSize = sizeof(SHELLEXECUTEINFO);
	si.lpVerb = __TEXT("open");
	
	OSVERSIONINFO ver; // windows�� ������ ��� ����ü
	ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&ver); // windows�� version�� �������� �Լ�

	/* windows vista �̻� */
	if(ver.dwMajorVersion >= 6)
	{
		len = sizeof(IP_ADAPTER_INFO);
		p = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
		
		dw = GetAdaptersInfo(p,&len);

		/* len�� �������� ���� ������ ���� loop*/
		while(dw == ERROR_BUFFER_OVERFLOW)
		{
			free(p);
			p = (IP_ADAPTER_INFO*)malloc(len);
			dw = GetAdaptersInfo(p,&len);		
		}
		
		/* Adpater List �߿��� dev_name�� ��ġ�ϴ� adater�� ã�� �� �� adapter�� target_ip�� target_mac�� static ������ �Ѵ�. */
		while(p!=NULL)
		{	
			if(strcmp(p->AdapterName,dev_name+20)==0)
			{
				ad_index = p->Index;
				path.Format(L" -c \"interface ipv4\" set neighbors %d \"%d.%d.%d.%d\" \"%02x-%02x-%02x-%02x-%02x-%02x\"",ad_index,
																	target_ip[0],target_ip[1],target_ip[2],target_ip[3],
												target_mac[0],target_mac[1],target_mac[2],target_mac[3],target_mac[4],target_mac[5]);
				si.lpFile = __TEXT("netsh.exe");
				break;
			}
			p=p->Next;
		}
	}
	else
	{
		path.Format(L" -s \"%d.%d.%d.%d\" \"%02x-%02x-%02x-%02x-%02x-%02x\"",
							target_ip[0],target_ip[1],target_ip[2],target_ip[3],
							target_mac[0],target_mac[1],target_mac[3],target_mac[4],target_mac[5]);
		si.lpFile = __TEXT("arp.exe");
	}

	si.lpParameters = path;
	si.nShow = SW_HIDE;

	if(ShellExecuteEx(&si))/* Shell ��ɾ� ����*/{
		/* Static ����Ʈ�� �߰� */
		get_hostname(target_ip, hostname);
		search_vendor(oui_list, target_mac, vendor, 0, OUI_COUNT-1);
		timer = time(NULL);
		t = localtime(&timer);
		strftime(timestr, sizeof(timestr),"%Y-%m-%d %p %I:%M:%S",t);
		addSpoof(HEADER_STATIC,STATIC,target_ip,target_mac,vendor,timestr,hostname);
		deleteSpoof(HEADER_ATTACK, target_ip, ATTACK);

		/* �߰����� UI��º� ���� �޽��� �߻� */
		memcpy(ARP_DATA.ipAddr, target_ip, IP_ALEN);
		memcpy(ARP_DATA.macAddr, target_mac, ETH_ALEN);
		strcpy(ARP_DATA.timestr, timestr);
		strcpy(ARP_DATA.vendor, vendor);
		SendMessage(hC1,WM_SPOOF, SM_UPDATE_LIST_STATIC, (LPARAM)&ARP_DATA);
		return 1;
	}
	else
		return 0;
}

//=====================================================================================
//
//	* Function : capture_arp()
//	* Description 
//		ARP Reply packet�� ���͸� �Ͽ� packets�� �����ϴ� �Լ�. NIC�� ������ ARP Reply 
//		packet �� �μ��� ���� �� tip�� ����� IP �ּҷ� ������ packet���� packets�� ����
//
//=====================================================================================
void capture_arp(u_char *tip)
{
	char packet_filter[] = "arp";
	struct bpf_program fcode;
	pcap_t* fp = adhandle;
    u_int netmask = 0xffffff; 

    if (pcap_compile(fp, &fcode, packet_filter, 1, netmask) <0 )
    {
        log("[Error] Unable to compile the packet filter. Check the syntax.");
        return;
    }
	//set the filter
    if (pcap_setfilter(fp, &fcode)<0)
    {
        log("[Error] Error setting the filter.");
        return;
    }
    
    /* start the capture */
    pcap_loop(fp,0, packet_handler_D, tip);
    
}

//=====================================================================================
//
//	* Function : packet_handler_D()
//	* Description 
//		capture_arp() ���� pcap_loop()�� �ݹ��Լ�
//		
//=====================================================================================
/* Callback function invoked by libpcap for every incoming packet */
void packet_handler_D(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data)
{
	struct eth_hdr *ethp; // Ethernet HEADER
	struct arp_hdr *arpp; // ARP HEADER
	u_char *mac;
	u_char *ip;
	char t_ip[16];
	char p_ip[16];
	char p_mac[18];

	ethp = (eth_hdr*)pkt_data;
	arpp = (arp_hdr*)(pkt_data+sizeof(struct eth_hdr));
	
	if(ntohs(arpp->arp_opcode)==2 && memcmp(arpp->arp_sip,myIPaddr,4)!=0)
	{
		ip = arpp->arp_sip;
		mac = arpp->arp_smac;
		
		
		sprintf_s(t_ip,16,"%d.%d.%d.%d",param[0],param[1],param[2],param[3]);
		sprintf_s(p_ip,16,"%d.%d.%d.%d",ip[0],ip[1],ip[2],ip[3]);
		sprintf_s(p_mac,18,"%02x-%02x-%02x-%02x-%02x-%02x",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
		
		if(memcmp(mac,attk_mac,6) != 0)
		{
			memcpy(target_mac,mac,6);
		}
		/* t_ip�� ������ ����� ip�� ������ packet�� ã�´�.*/
		if(memcmp(t_ip,p_ip,sizeof(t_ip))==0)
		{
			if(search(p_mac) == 0)
			{
				/* ������ reply pakcet������ �˻��ϴ� �κ�. ���ο� reply pakcet�� ��� packets�� ����*/
				addListD(p_ip,p_mac);
			}
		}
	}
}

//=====================================================================================
//
//	* Function : search()
//	* Description 
//		������ reply packet���� �ƴ����� �˻��ϱ� ���� �Լ�.
//		�μ��� ���� �� mac�ּҸ� ������ packet�� �̹� packets�� �̹� ���� �Ǿ� �ִ����� �˻�
//
//=====================================================================================
int search(char *mac)
{
	int i;
	
	for(i=0; i<5; i++)
	{
		if(memcmp(packets[i].sf_mac,mac,18) == 0)
		{
			packets[i].cnt++;
			return 1;
		}
	}
	return 0;

}

//=====================================================================================
//
//	* Function : addList()
//	* Description 
//		���ο� reply packet�� packets�� �����ϴ� �Լ�
//		
//=====================================================================================
void addListD(char *ip, char *mac)
{
	int i;

	for(i=0; i<5; i++)
	{
		if(packets[i].cnt == 0)
		{
			memcpy(packets[i].sf_ip,ip,16);
			memcpy(packets[i].sf_mac,mac,18);
			packets[i].cnt=1;
			return;
		}
	}
}

//=====================================================================================
//
//	* Function : make_packet()
//	* Description 
//		���ڷ� ���޵� ip�� ������ ip�� �ϴ� ARP Request packet�� ����� �Լ� 
//		
//=====================================================================================
void make_packet(u_char *packet, u_char *ip)
{
	//������ �� �ּ�(fixed)
	packet[0]=0xFF;
	packet[1]=0xFF;
	packet[2]=0xFF;
	packet[3]=0xFF;
	packet[4]=0xFF;
	packet[5]=0xFF;
	
	//����� �� �ּ�
	packet[6]=if_mac[0];
	packet[7]=if_mac[1];
	packet[8]=if_mac[2];
	packet[9]=if_mac[3];
	packet[10]=if_mac[4];
	packet[11]=if_mac[5];

	//��Ŷ Ÿ��(ARP)(fixed)
	packet[12]=0x08;
	packet[13]=0x06;

	//�ϵ���� Ÿ��(�̴���)(fixed)
	packet[14]=0x00;
	packet[15]=0x01;

	//�������� Ÿ��(IPv4)(fixed)
	packet[16]=0x08;
	packet[17]=0x00;

	//�ϵ���� ������(fixed)
	packet[18]=0x06;
	//�������� ������
	packet[19]=0x04;

	//�����ڵ�(request)(fixed)
	packet[20]=0x00;
	packet[21]=0x01;

	//Sender MAC address
	packet[22]=if_mac[0];
	packet[23]=if_mac[1];
	packet[24]=if_mac[2];
	packet[25]=if_mac[3];
	packet[26]=if_mac[4];
	packet[27]=if_mac[5];

	//Sender IP address
	packet[28]=myIPaddr[0];
	packet[29]=myIPaddr[1];
	packet[30]=myIPaddr[2];
	packet[31]=myIPaddr[3];
	
	//Target MAC address(fixed)
	packet[32] = 0x00;
	packet[33] = 0x00;
	packet[34] = 0x00;
	packet[35] = 0x00;
	packet[36] = 0x00;
	packet[37] = 0x00;

	//Target IP address
	packet[38]=ip[0];
	packet[39]=ip[1];
	packet[40]=ip[2];
	packet[41]=ip[3];
}


//=====================================================================================
//
//	* Function : arp_request()
//	* Description 
//		make_packet() �Լ��� �̿��� ARP request packet�� ������ �� 10�� ��������. 
//		
//=====================================================================================
void arp_request(u_char *tip)
{	
	u_char packet[42];
	int i=0;
	pcap_t *fp = adhandle;
	make_packet(packet,tip);
	
    /* Send down the packet */
	
	for(i=0; i<10; i++){
		if (pcap_sendpacket(fp, packet,42) != 0)
		{
			log("[Error] Error sending the packet");
			return;
		}
	}
}

