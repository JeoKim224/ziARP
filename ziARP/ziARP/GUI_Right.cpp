///////////////////////////////////////////////////////////////////////////////////////
//
//	* 'ziArp' - a ARP poisoning attack detector
//	* 2012 Summer Handong Global University GHOST U3 Team Open Source Project
//	* File Name		: GUI_Right.cpp
//	* Author		: ���̻�-(Isaac Park), ������-(Hyunjeong)
//	* Date			: 2012. 7. 16 - 2012. 8. 4
//	* Description
//		 GUI������â���� �����������쿡 ���� ������ �Լ����� �����ϰ��ִ�. �� ���α׷��� 
//		 �� ��ɵ��� ����� ��µǴ� List View�� ���ԵǾ��ִ�. 
//		 List View�� IP, MAC, Vendor, Time, Host�� �÷��� �������ְ� IP���� ��IP�� ������
//		 �¸� �ʷ�(����),����(����),����(Static Mac)���� ��������.
//		 List View�ؿ� �� Į���� ������ Editâ�� �پ �����ϰ� IP�˻��� �� �ֵ��� �Ͽ���. 
///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
//
//	'#include's
//
///////////////////////////////////////////////////////////////////////////////////////
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif

#include "resource.h"
#include <windows.h>
#include <commctrl.h>
#include "header/datas.h"
#include "header/get_info.h"

#define MAX_STR 1024
#define WM_SIGNAL WM_USER
#define TRAY_NOTIFY (WM_APP + 100)

///////////////////////////////////////////////////////////////////////////////////////
//
//	Global Variables
//
///////////////////////////////////////////////////////////////////////////////////////

extern INT STATE;

//��Ÿ�Լ�
void ChangeStatus(HWND,INT);
extern void CharToTCHAR(char* char_str, TCHAR* TCHAR_str);
extern void TCHARToChar(char* char_str, TCHAR* TCHAR_str);
void AddList(HWND hWnd, PSPOOF_L ARP_LIST);
void ChangeList(INT index);
void ChangeState(void);


INT ChatchChange(HWND hWnd, PSPOOF_L ARP_LIST, PSPOOF_L HEAD);
extern DWORD WINAPI engin(LPVOID arg);
extern HANDLE hThread;
INT GetWndFocus(void);
extern HINSTANCE g_hInst;
extern HIMAGELIST hImgSm, hImgLa;
extern NOTIFYICONDATA nid;
WNDPROC g_OldProc;

//�޽���ó���Լ��� �ڵ�
HWND hIp1,hIp2, hMac1,hMac2, hVendor1,hVendor2, hTime1, hTime2, hHost1, hHost2;
HWND hMenu1,hMenu2,hMenu3,hMenu4;

// ������ �ڵ�
extern HWND hWndMain, hC1, hC3;
HWND hC2, hList;

// ������ ���ϵ��� �޽��� ���ν���
enum { IDC_IP=1,IDC_MAC,IDC_VENDOR,IDC_TIME, IDC_HOST, IDC_DEF, IDC_FIND};

///////////////////////////////////////////////////////////////////////////////////////
//
//	Prototypes
//
///////////////////////////////////////////////////////////////////////////////////////

// ������ ���ν���
LRESULT CALLBACK ChildRightProc(HWND,UINT,WPARAM,LPARAM);
//�޽��� ó���Լ�
LRESULT RightCreate(HWND,WPARAM,LPARAM);
LRESULT RightNotify(HWND,WPARAM,LPARAM);
LRESULT RightCommand(HWND,WPARAM,LPARAM);
LRESULT RightSpoof(HWND,WPARAM,LPARAM);
LRESULT RightDrawItem(HWND,WPARAM,LPARAM);
//��ȭ���� 
BOOL CALLBACK DefenceDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);

LRESULT CALLBACK MyEditWindowProc( HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam );



///////////////////////////////////////////////////////////////////////////////////////
//
//	Body
//
///////////////////////////////////////////////////////////////////////////////////////

//=====================================================================================
//
//	* Function : ChildRightProc()
//	* Description 
//		ChildRightProc�Լ��� WndProc���� ������� "ChildRight"�������� �޽��� ���ν����̴�.
//		�ڵ��� hC1�̴�. �� �ݹ��Լ����� ���̴� ����ڸ޽����� WM_SPOOF�� �ִ�.
//
//=====================================================================================
LRESULT CALLBACK ChildRightProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	RECT crt;
	int x_center;
	
	switch (iMessage) {
	case WM_CREATE:
		return RightCreate(hWnd,wParam,lParam);

	case WM_NOTIFY:
		return RightNotify(hWnd,wParam,lParam);

	case WM_SIZE:
		GetClientRect(hWnd,&crt);
		x_center = (crt.right-crt.left)/2;

		MoveWindow(hList,10,10,crt.right-20,HIWORD(lParam)-140,TRUE);

		MoveWindow(hMenu3,x_center-110,HIWORD(lParam)-40,90,25,TRUE);
		MoveWindow(hMenu4,x_center+20,HIWORD(lParam)-40,90,25,TRUE);

		MoveWindow(hIp1,x_center-225,HIWORD(lParam)-110,20,20,TRUE);
		MoveWindow(hIp2,x_center-205,HIWORD(lParam)-110,115,20,TRUE);
		MoveWindow(hMac1,x_center-80,HIWORD(lParam)-110,35,20,TRUE);
		MoveWindow(hMac2,x_center-45,HIWORD(lParam)-110,130,20,TRUE);
		MoveWindow(hHost1,x_center+95,HIWORD(lParam)-110,40,20,TRUE);
		MoveWindow(hHost2,x_center+135,HIWORD(lParam)-110,100,20,TRUE);
		MoveWindow(hVendor1,x_center-225,HIWORD(lParam)-80,60,20,TRUE);
		MoveWindow(hVendor2,x_center-165,HIWORD(lParam)-80,185,20,TRUE);
		MoveWindow(hTime1,x_center+30,HIWORD(lParam)-80,40,20,TRUE);
		MoveWindow(hTime2,x_center+70,HIWORD(lParam)-80,165,20,TRUE);
		
		return 0;

	case WM_COMMAND:
		return RightCommand(hWnd,wParam,lParam);
	
	case WM_SPOOF:
		return RightSpoof(hList,wParam,lParam);

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}


//=====================================================================================
//
//	* Function : MyEditWindowProc()
//	* Description 
//		�� �Լ��� �Ʒ��� IP, MAC, Vendor, Time, Host Editâ����̿� ��Ű�� ������
//		���ʴ�� focus�� ���ϵ��� SubClassing�Ͽ���. �� �Լ��� SubClassing��
//		���Ǵ� ���ν����̴�.
//
//=====================================================================================
LRESULT CALLBACK MyEditWindowProc( HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam )
{
	static UINT index = 0;
	HWND hArray[5]={hIp2,hMac2,hVendor2,hTime2,hHost2};

	if(iMessage == WM_CHAR)
	{
		if(wParam == VK_TAB)
			if((index=GetWndFocus())!=-1)
				SetFocus(hArray[(++index)%5]);
	}

    return g_OldProc( hWnd, iMessage, wParam, lParam );
}


//=====================================================================================
//
//	* Function : RightCreate()
//	* Description 
//		�� �Լ��� ���� ChildRightProc�ݹ��Լ��� WM_CREATE�޽����� �����ϴ� �޽��� ó�� �Լ�
//		�̴�. ChildRight �����찡 ����� �� ó���� �ʱ�ȭ�Ǿ�� �� ������ ����Ǿ�� �� ��
//		������ ȣ���Ѵ�.
//
//=====================================================================================
LRESULT RightCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	//List View�� �����.
	hList=CreateWindow(WC_LISTVIEW,NULL,WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_NOSORTHEADER,// | LVS_OWNERDRAWFIXED,
		0,0,0,0,hWnd,NULL,g_hInst,NULL);
	ListView_SetExtendedListViewStyle(hList,LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);


	//�÷������
	LVCOLUMN col;
	col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_SUBITEM | LVCF_TEXT;
	col.fmt = LVCFMT_LEFT;
	
	col.cx = 120;
	col.iSubItem = 0;
	col.pszText = TEXT("IP");
	ListView_InsertColumn(hList,0,&col);

	col.cx = 120;
	col.iSubItem = 1;
	col.pszText = TEXT("Mac");
	ListView_InsertColumn(hList,1,&col);

	col.cx = 100;
	col.iSubItem = 2;
	col.pszText = TEXT("Vendor");
	ListView_InsertColumn(hList,2,&col);

	col.cx = 160;
	col.iSubItem = 3;
	col.pszText = TEXT("Time");
	ListView_InsertColumn(hList,3,&col);

	col.cx = 130;
	col.iSubItem = 4;
	col.pszText = TEXT("Host");
	ListView_InsertColumn(hList,4,&col);


	//������ �Է¹ޱ� ���� ��Ʈ�ѵ��� �����.
	hIp1=CreateWindow(TEXT("static"),TEXT("IP"),WS_CHILD | WS_VISIBLE,
		0,0,0,0,hWnd,(HMENU)-1,g_hInst,NULL);
	hIp2=CreateWindow(TEXT("edit"),NULL,WS_CHILD | WS_VISIBLE | WS_BORDER |
		ES_AUTOHSCROLL,0,0,0,0,hWnd,(HMENU)IDC_IP,g_hInst,NULL);
	hMac1=CreateWindow(TEXT("static"),TEXT("MAC"),WS_CHILD | WS_VISIBLE,
		0,0,0,0,hWnd,(HMENU)-1,g_hInst,NULL);
	hMac2=CreateWindow(TEXT("edit"),NULL,WS_CHILD | WS_VISIBLE | WS_BORDER |  
		ES_AUTOHSCROLL,0,0,0,0,hWnd,(HMENU)IDC_MAC,g_hInst,NULL);

	hVendor1=CreateWindow(TEXT("static"),TEXT("VENDOR"),WS_CHILD | WS_VISIBLE,
		0,0,0,0,hWnd,(HMENU)-1,g_hInst,NULL);
	hVendor2=CreateWindow(TEXT("edit"),NULL,WS_CHILD | WS_VISIBLE | WS_BORDER |
		ES_AUTOHSCROLL,0,0,0,0,hWnd,(HMENU)IDC_VENDOR,g_hInst,NULL);

	hTime1=CreateWindow(TEXT("static"),TEXT("TIME"),WS_CHILD | WS_VISIBLE,
		0,0,0,0,hWnd,(HMENU)-1,g_hInst,NULL);
	hTime2=CreateWindow(TEXT("edit"),NULL,WS_CHILD | WS_VISIBLE | WS_BORDER |
		ES_AUTOHSCROLL,0,0,0,0,hWnd,(HMENU)IDC_TIME,g_hInst,NULL);

	hHost1=CreateWindow(TEXT("static"),TEXT("HOST"),WS_CHILD | WS_VISIBLE,
		0,0,0,0,hWnd,(HMENU)-1,g_hInst,NULL);
	hHost2=CreateWindow(TEXT("edit"),NULL,WS_CHILD | WS_VISIBLE | WS_BORDER |
		ES_AUTOHSCROLL,0,0,0,0,hWnd,(HMENU)IDC_HOST,g_hInst,NULL);

	hMenu3=CreateWindow(TEXT("button"),TEXT("IP�˻�"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		0,0,0,0,hWnd,(HMENU)IDC_FIND,g_hInst,NULL);
	hMenu4=CreateWindow(TEXT("button"),TEXT("���"),WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		0,0,0,0,hWnd,(HMENU)IDC_DEF,g_hInst,NULL);
	
	//tabŰ ���ϱ� ���� ����Ŭ���
	g_OldProc = (WNDPROC)GetWindowLongPtr( hIp2, GWLP_WNDPROC );
	SetWindowLongPtr( hIp2, GWLP_WNDPROC, (LONG_PTR)MyEditWindowProc );
	SetWindowLongPtr( hMac2, GWLP_WNDPROC, (LONG_PTR)MyEditWindowProc );
	SetWindowLongPtr( hVendor2, GWLP_WNDPROC, (LONG_PTR)MyEditWindowProc );
	SetWindowLongPtr( hTime2, GWLP_WNDPROC, (LONG_PTR)MyEditWindowProc );
	SetWindowLongPtr( hHost2, GWLP_WNDPROC, (LONG_PTR)MyEditWindowProc );
	return 0;
}


//=====================================================================================
//
//	* Function : RightNotify()
//	* Description 
//		�� �Լ��� ���� ChildRightProc�ݹ��Լ��� WM_NOTIFY�޽����� �����ϴ� �޽��� ó�� �Լ�
//		�̴�.
//
//=====================================================================================
LRESULT RightNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	TCHAR szIP[255], szMAC[255], szVENDOR[255], szTIME[255], szHOST[255];
	LVITEM LI;
	LPNMHDR hdr;
	LPNMLISTVIEW nlv;
	hdr=(LPNMHDR)lParam;
	nlv=(LPNMLISTVIEW)lParam;

	if (hdr->hwndFrom == hList) {
		switch (hdr->code) {
		// ���õ� �׸�(List View���� Focus�� �׸�)�� ������Ʈ�� ����Ѵ�.
		case LVN_ITEMCHANGED:
			if (nlv->uChanged == LVIF_STATE && nlv->uNewState == 
				(LVIS_SELECTED | LVIS_FOCUSED)) {
				LI.mask=LVIF_IMAGE;
				LI.iItem=nlv->iItem;
				LI.iSubItem=0;
				ListView_GetItem(hList, &LI);
				
				ListView_GetItemText(hList,nlv->iItem,0,szIP,MAX_STR);
				SetDlgItemText(hWnd,IDC_IP,szIP);
				ListView_GetItemText(hList,nlv->iItem,1,szMAC,MAX_STR);
				SetDlgItemText(hWnd,IDC_MAC,szMAC);
				ListView_GetItemText(hList,nlv->iItem,2,szVENDOR,MAX_STR);
				SetDlgItemText(hWnd,IDC_VENDOR,szVENDOR);
				ListView_GetItemText(hList,nlv->iItem,3,szTIME,MAX_STR);
				SetDlgItemText(hWnd,IDC_TIME,szTIME);
				ListView_GetItemText(hList,nlv->iItem,4,szHOST,MAX_STR);
				SetDlgItemText(hWnd,IDC_HOST,szHOST);
			}
			return TRUE;
		}
	}
	return 0;
}
//=====================================================================================
//
//	* Function : RightCommand()
//	* Description 
//		�� �Լ��� ���� ChildRightProc�ݹ��Լ��� WM_COMMAND�޽����� �����ϴ� �޽��� ó�� �Լ�
//		�̴�. 
//
//=====================================================================================
LRESULT RightCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	struct tag_ip {
	TCHAR ip[20];
	TCHAR mac[20];
	TCHAR vendor[50];
	TCHAR time[50];
	TCHAR host[50];
	} IpAdd[]={

		{NULL,NULL,NULL,NULL,NULL},

	};

	TCHAR szIP[255];
	
	int idx;
	
	/*hImgSm = ImageList_LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BIT_VIEW_16), 16,2,RGB(255,255,255));
	SendMessage(hList, LVM_SETIMAGELIST,(WPARAM)LVSIL_SMALL, (LPARAM)hImgSm);*/


	switch (LOWORD(wParam)) {
		// ������ �׸��� ����Ѵ�.
		case IDC_DEF:
			DialogBox(g_hInst,MAKEINTRESOURCE(IDD_DIALOG_DEFENCE),hWnd,DefenceDlgProc);
			return TRUE;
			
		// IP�� �׸��� �˻��Ѵ�.
		case IDC_FIND:
			LVFINDINFO fi;
			GetDlgItemText(hWnd,IDC_IP,szIP,255);
			fi.flags=LVFI_STRING;
			fi.psz=szIP;
			fi.vkDirection=VK_DOWN;
			idx=ListView_FindItem(hList,-1,&fi);
			if (idx==-1) {
				MessageBox(hWnd,TEXT("������ ip �����ϴ�"),TEXT("�˸�"),MB_OK);
			} else {
				ListView_SetItemState(hList,-1,0,LVIS_FOCUSED | LVIS_SELECTED);
				ListView_SetItemState(hList,idx,LVIS_FOCUSED | LVIS_SELECTED,
					LVIS_FOCUSED | LVIS_SELECTED);
				ListView_EnsureVisible(hList,idx,FALSE);
			}
			return TRUE;
		}
		return 0;
}

//=====================================================================================
//
//	* Function : RightSpoof()
//	* Description 
//		�� �Լ��� ���� ChildRightProc�ݹ��Լ��� WM_SPOOF�޽����� �����ϴ� �޽��� ó�� �Լ�
//		�̴�. wParam�� SM_INIT, SM_CHANGE, SM_ADD�� ������ �޽����� ������ Ž�������
//		�����͸� �ֵ����� �� �ִ�.
//
//=====================================================================================
LRESULT RightSpoof(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	static PSPOOF_L HEAD=NULL;
	PSPOOF_L __ARP_LIST=(PSPOOF_L)lParam;
	INT LIST_index;
	ARPDATA* arpnode;
	char hostStr[17];
	
	switch(wParam)
		{
		//List View�� �ʱ�ȭ�� �� ���� �޽����̴�.
		case SM_INIT:
			HEAD=__ARP_LIST;
			ListView_DeleteAllItems(hWnd);
			while(__ARP_LIST!=NULL){
				AddList(hWnd,__ARP_LIST);
				__ARP_LIST=__ARP_LIST->next;
			}
			ChangeState();
			SendMessage(hC2,WM_PAINT,0,0);
			break;
		//List View�� �ش� �ε����� ������ �� ���� �޽����̴�.
		case SM_CHANGE:
			LIST_index=ChatchChange(hWnd,__ARP_LIST,HEAD);
			ChangeList(LIST_index);
			/* �ǽ��̳� ���ݻ�Ȳ�� Ʈ���� �߻�*/
			if(__ARP_LIST->flag==SUSPICIOUS || __ARP_LIST->flag==ATTACK){
				nid.cbSize = sizeof(NOTIFYICONDATA);
				nid.hWnd = hWndMain;
				nid.uID = 0;
				nid.uFlags = NIF_ICON|NIF_TIP|NIF_MESSAGE|NIF_INFO;
				nid.dwInfoFlags = 0x0006;
				nid.uTimeout = 500;
				nid.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON1));
				lstrcpy(nid.szInfoTitle, TEXT("���ǻ�Ȳ"));
				wsprintf(nid.szTip, TEXT("�ǽɽ����� %d.%d.%d.%d"),
					__ARP_LIST->ipAddr[0],__ARP_LIST->ipAddr[1],__ARP_LIST->ipAddr[2],__ARP_LIST->ipAddr[3]);
				wsprintf(nid.szInfo, TEXT("�ǽɽ����� %d.%d.%d.%d�� ���α׷��� �����Ǿ����ϴ�."),
					__ARP_LIST->ipAddr[0],__ARP_LIST->ipAddr[1],__ARP_LIST->ipAddr[2],__ARP_LIST->ipAddr[3]);
				Shell_NotifyIcon(NIM_MODIFY, &nid);
			}
			break;
		//List View�� ���� �� ���� �޽����̴�.
		case SM_ADD:
			get_hostname(__ARP_LIST->ipAddr, hostStr);
			add_hostname(ARP_LIST,__ARP_LIST->ipAddr,hostStr);
			AddList(hWnd,__ARP_LIST);
			break;
		case SM_NODEVICE:
			MessageBox(hWnd,TEXT("�������̽��� ã���� �����ϴ�.\nWinpcap�� ��ġ �Ǿ����� Ȯ���� �ֽñ� �ٶ��ϴ�."),TEXT("�˸�"),MB_OK);
			break;
		case SM_STOP_THREAD:
			SuspendThread(hThread);
			break;
		case SM_RESUME_THREAD:
			ResumeThread(hThread);
			break;
		case SM_UPDATE_HOSTNAME:
			LIST_index=ChatchChange(hWnd,__ARP_LIST,HEAD);
			ChangeList(LIST_index);
			break;
		case SM_UPDATE_LIST_NOMAL:
			arpnode = (ARPDATA*)lParam;
			updateState(ARP_LIST,NOMAL,arpnode->ipAddr,arpnode->macAddr, arpnode->vendor,arpnode->timestr," ");
			break;
		case SM_UPDATE_LIST_SUSP:
			arpnode = (ARPDATA*)lParam;
			updateState(ARP_LIST,SUSPICIOUS,arpnode->ipAddr,arpnode->macAddr, arpnode->vendor,arpnode->timestr," ");
			break;
		case SM_UPDATE_LIST_ATTACK:
			arpnode = (ARPDATA*)lParam;
			updateState(ARP_LIST,ATTACK,arpnode->ipAddr,arpnode->macAddr, arpnode->vendor,arpnode->timestr," ");
			break;
		case SM_UPDATE_LIST_STATIC:
			arpnode = (ARPDATA*)lParam;
			updateState(ARP_LIST,STATIC,arpnode->ipAddr,arpnode->macAddr, arpnode->vendor,arpnode->timestr," ");
	}
		return 0;
}

//=====================================================================================
//
//	* Function : AddList()
//	* Description 
//		�� �Լ��� WM_SPOOF���� List View�� ���ڿ����� �Է��ϴ� �Լ��̴�.
//		�ι�° ���ڷ� �޴� �����Ϳ� �ִ� �ڷᱸ���� ������ ����ϵ��� ��ӵǾ��ִ�.
//
//=====================================================================================
void AddList(HWND hWnd, PSPOOF_L ARP_LIST)
{
	TCHAR szIP[256], szMAC[256], szVENDOR[256], szTIME[256],szHOST[256];
	int idx;
	LVITEM LI;

	wsprintf(szIP,TEXT("%d.%d.%d.%d"),
		ARP_LIST->ipAddr[0],ARP_LIST->ipAddr[1],ARP_LIST->ipAddr[2],ARP_LIST->ipAddr[3]);
	wsprintf(szMAC,TEXT("%02X:%02X:%02X:%02X:%02X:%02X"),
		ARP_LIST->macAddr[0],ARP_LIST->macAddr[1],ARP_LIST->macAddr[2],
		ARP_LIST->macAddr[3],ARP_LIST->macAddr[4],ARP_LIST->macAddr[5]);
	CharToTCHAR(ARP_LIST->vendor,szVENDOR);
	CharToTCHAR(ARP_LIST->timestr,szTIME);
	CharToTCHAR(ARP_LIST->hostName,szHOST);

	hImgSm = ImageList_LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BIT_VIEW_16), 16,2,RGB(255,255,255));
	SendMessage(hList, LVM_SETIMAGELIST,(WPARAM)LVSIL_SMALL, (LPARAM)hImgSm);

	LI.mask=LVIF_TEXT | LVIF_IMAGE;
	LI.iImage=(ARP_LIST->flag==0? 0:(ARP_LIST->flag!=3? 1:2));
	LI.iSubItem=0;
	idx=ListView_GetItemCount(hList);
	LI.iItem=idx;
	LI.pszText=szIP;
	ListView_InsertItem(hList,&LI);

	ListView_SetItemText(hList,idx,1,szMAC);
	ListView_SetItemText(hList,idx,2,szVENDOR);
	ListView_SetItemText(hList,idx,3,szTIME);
	ListView_SetItemText(hList,idx,4,szHOST);
}

//=====================================================================================
//
//	* Function : ChatchChange()
//	* Description 
//		�� �Լ��� WM_SPOOF���� List View�� ���ڿ����� �ش籸��ü�� ��� �ε����� ��������
//		����ϴ� �Լ��̴�.
//		�ι�° ���ڷ� �޴� �����Ϳ� �ִ� �ڷᱸ���� ������ ����ϰ� ����° ���ڷ� 
//		PSPOOF_L�ڷᱸ�� ��ũ�ý�Ʈ�� ����� �޵��� ��ӵǾ��ִ�.
//
//=====================================================================================
INT ChatchChange(HWND hWnd, PSPOOF_L ARP_LIST, PSPOOF_L HEAD)
{
	INT index=-1;
	INT flag = 0;
	PSPOOF_L tmp=HEAD;
	while(tmp!=NULL)
	{
		index++;
		if(memcmp(ARP_LIST->ipAddr,tmp->ipAddr,4)==0)
			break;
		tmp=tmp->next;
	}

	//scan flag changed
	
	ChangeState();
	InvalidateRect(hC2,NULL,TRUE);
	SendMessage(hC2,WM_PAINT,0,0);
	
	InvalidateRect(hC3,NULL,TRUE);
	SendMessage(hC3,WM_PAINT,0,0);

	return index;
}


//=====================================================================================
//
//	* Function : ChangeList()
//	* Description 
//		�� �Լ��� WM_SPOOF���� List View�� �ش�Ǵ� �ε����� ��ȯ�ϴ� �Լ��̴�.
//		�Է¹��� ����ü�� ���������� ARP_LIST�� ����Ѵ�.
//
//=====================================================================================
void ChangeList(INT index)
{
	TCHAR szIP[256], szMAC[256], szVENDOR[256], szTIME[256],szHOST[256];
	int i;
	LVITEM LI;
	PSPOOF_L ARP_LIST_TMP=ARP_LIST;

	for(i=0;i<index;i++)
		 ARP_LIST_TMP= ARP_LIST_TMP->next;


	wsprintf(szIP,TEXT("%d.%d.%d.%d"),
		ARP_LIST_TMP->ipAddr[0],ARP_LIST_TMP->ipAddr[1],ARP_LIST_TMP->ipAddr[2],ARP_LIST_TMP->ipAddr[3]);
	wsprintf(szMAC,TEXT("%02X:%02X:%02X:%02X:%02X:%02X"),
		ARP_LIST_TMP->macAddr[0],ARP_LIST_TMP->macAddr[1],ARP_LIST_TMP->macAddr[2],
		ARP_LIST_TMP->macAddr[3],ARP_LIST_TMP->macAddr[4],ARP_LIST_TMP->macAddr[5]);
	CharToTCHAR(ARP_LIST_TMP->vendor,szVENDOR);
	CharToTCHAR(ARP_LIST_TMP->timestr,szTIME);
	CharToTCHAR(ARP_LIST_TMP->hostName,szHOST);

	hImgSm = ImageList_LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BIT_VIEW_16), 16,2,RGB(255,255,255));
	SendMessage(hList, LVM_SETIMAGELIST,(WPARAM)LVSIL_SMALL, (LPARAM)hImgSm);

	LI.mask=LVIF_TEXT | LVIF_IMAGE;
	LI.iImage=(ARP_LIST_TMP->flag==0? 0:((ARP_LIST_TMP->flag!=3)?1:2));
	LI.iItem=index;

	LI.iSubItem=0;
	LI.pszText=szIP;
	ListView_SetItem(hList,&LI);

	LI.mask=LVIF_TEXT;
	LI.iSubItem=1;
	LI.pszText=szMAC;
	ListView_SetItem(hList,&LI);

	LI.iSubItem=2;
	LI.pszText=szVENDOR;
	ListView_SetItem(hList,&LI);

	LI.iSubItem=3;
	LI.pszText=szTIME;
	ListView_SetItem(hList,&LI);

	LI.iSubItem=4;
	LI.pszText=szHOST;
	ListView_SetItem(hList,&LI);
}


//=====================================================================================
//
//	* Function : ChangeState()
//	* Description 
//		�� �Լ��� ��ü �������ǻ���� ���¸� �ٲٴ� �Լ��̴�.
//
//=====================================================================================
void ChangeState(void)
{
	INT YELLO=0;
	INT RED=0;
	INT flag=0;
	PSPOOF_L tmp = ARP_LIST;
	
	while(tmp!=NULL)
	{
		flag=tmp->flag;
		switch(flag)
		{
		case 1:
			YELLO++;
			break;
		case 2:
			RED++;
			break;
		}
		tmp=tmp->next;
	}

	if(RED>0)
		STATE=2;

	else if(YELLO>0)
		STATE=1;

	else
		STATE=0;
}


//=====================================================================================
//
//	* Function : GetWndFocus()
//	* Description 
//		�� �Լ��� ���� ����� �� �ֵ��� subclassing�� �̿��� MyEditWindowProc()���ν���
//		�� ����ϴ� �Լ��̴�. ���� ����� �� 5���� editâ�� ������� focus��ų �� �ֵ��� 
//		�ش�� �ε����� �����ϴ� �Լ��̴�.
//
//=====================================================================================
INT GetWndFocus(void)
{
	HWND hWnd;

	hWnd=GetFocus();

	if(hWnd == hIp2)
		return 0;
	else if(hWnd == hMac2)
		return 1;
	else if(hWnd == hVendor2)
		return 2;
	else if(hWnd == hTime2)
		return 3;
	else if(hWnd == hHost2)
		return 4;
	else
		return -1;
}