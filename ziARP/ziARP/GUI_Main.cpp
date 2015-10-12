///////////////////////////////////////////////////////////////////////////////////////
//
//	* 'ziArp' - a ARP poisoning attack detector
//	* 2012 Summer Handong Global University GHOST U3 Team Open Source Project
//	* File Name		: GUI_Main.cpp
//	* Author		: ������,���̻�-(HyunJeong, Isaac)
//	* Date			: 2012. 7. 16 - 2012. 8. 4
//	* Description
//		GUI ���� ������ â�� �׷��ְ�, ������ ��ȣ�� �˸�â�� �˸� �޼����� ����ش�.
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
#include "resource.h"
#include <windows.h>
#include <Windowsx.h>
#include <commctrl.h>
#include <atlstr.h>
#include "header/datas.h"
#include "header/log.h"
#include "header/policy.h"
#include "header/spoof_list.h"
#include "header/get_info.h"

#define MAX_STR 1024
#define WM_SIGNAL WM_USER
#define TRAY_NOTIFY (WM_APP + 100)


///////////////////////////////////////////////////////////////////////////////////////
//
//	Global Variables
//
///////////////////////////////////////////////////////////////////////////////////////
INT STATE = 0;
DWORD ThreadID;
HANDLE hThread;
HINSTANCE g_hInst;
HIMAGELIST hImgSm, hImgLa;
INT devIndex;
MSG Message;
NOTIFYICONDATA nid;
//Log �޽����� ����
char logmsg[LOG_MSG_SIZE];

///////////////////////////////////////////////////////////////////////////////////////
//
//	Prototypes
//
///////////////////////////////////////////////////////////////////////////////////////
// ������ ���ν���
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK ChildRightProc(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK ChildTopProc(HWND,UINT,WPARAM,LPARAM);
LRESULT CALLBACK ChildBottomProc(HWND,UINT,WPARAM,LPARAM);
//��ȭ���� 
BOOL CALLBACK AboutDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK InterfaceConfigDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK LogDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK HelpDlgProc(HWND,UINT,WPARAM,LPARAM);
BOOL CALLBACK DefenceDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
//�޽��� ó���Լ�
LRESULT WriteText(HWND,WPARAM,LPARAM);
//��Ÿ�Լ�
LRESULT ParseLog(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);
void ChangeStatus(HWND,INT);
void SetInterface(INT);
void GetInterface(HWND hWnd,TCHAR* lpString);
void GetAttackList(HWND hWnd,TCHAR* lpString);
void CharToTCHAR(char* char_str, TCHAR* TCHAR_str);
void TCHARToChar(char* char_str, TCHAR* TCHAR_str);
int defence(int index);
extern void ChangeState(void);
extern DWORD WINAPI engin(LPVOID arg);
// Ž������ �ܺ��Լ�
extern void getDevice();

// ������ �ڵ�
HWND hWndMain, hC1, hC3;
extern HWND hC2,  hList;
LPCTSTR lpszClass=TEXT("OpenSource");

///////////////////////////////////////////////////////////////////////////////////////
//
//	Body
//
///////////////////////////////////////////////////////////////////////////////////////

//=====================================================================================
//
//	* Function : ���� ������ �� ���� ��,�Ʒ� ������ ����()
//	* Description 
//		GUI ���� ������ â�� �׷��ְ�, ������ ��ȣ�� �˸�â�� �˸� �޼����� ����ش�.
//
//=====================================================================================
int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpszCmdParam,int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst=hInstance;
	
	// ���� ������ Ŭ����
	WndClass.cbClsExtra=0;
	WndClass.cbWndExtra=0;
	WndClass.hbrBackground=NULL;
	WndClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	WndClass.hIcon=LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON1));
	WndClass.hInstance=hInstance;
	WndClass.lpfnWndProc=WndProc;
	WndClass.lpszClassName=lpszClass;
	WndClass.lpszMenuName=MAKEINTRESOURCE(IDR_MENU1);
	WndClass.style=CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	RegisterClass(&WndClass);

	// ������ ���ϵ� Ŭ����
	WndClass.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpfnWndProc=ChildRightProc;
	WndClass.lpszClassName=TEXT("ChildRight");
	RegisterClass(&WndClass);

	// ���� ���ϵ� Ŭ����
	WndClass.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpfnWndProc=ChildTopProc;
	WndClass.lpszClassName=TEXT("ChildTop");
	RegisterClass(&WndClass);

	// �Ʒ��� ���ϵ� Ŭ����
	WndClass.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpfnWndProc=ChildBottomProc;
	WndClass.lpszClassName=TEXT("ChildBottom");
	RegisterClass(&WndClass);
	
	// ���� ������ ����
	hWnd=CreateWindow(lpszClass,lpszClass,WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,CW_USEDEFAULT,1080, 600,
		NULL,(HMENU)NULL,hInstance,NULL);
	ShowWindow(hWnd,nCmdShow);
	hWndMain=hWnd;
	
	while (GetMessage(&Message,NULL,0,0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	
	return (int)Message.wParam;
}

//=====================================================================================
//
//	* Function : CALLBACK WndProc()
//	* Description 
//		���� �������� �޽��� ���ν���
//
//=====================================================================================
LRESULT CALLBACK WndProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	RECT crt;
	HDC hdc;
	PAINTSTRUCT ps;
	HMENU hMenu,hPopupMenu;
	POINT pt;

	switch (iMessage) {
	case WM_CREATE:
		hC1=CreateWindow(TEXT("ChildRight"),NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,
			0,0,0,0,hWnd,(HMENU)0,g_hInst,NULL);
		hC2=CreateWindow(TEXT("ChildTop"),NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_DLGFRAME,
			0,0,0,0,hWnd,(HMENU)0,g_hInst,NULL);
		hC3=CreateWindow(TEXT("ChildBottom"),NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_DLGFRAME,
			0,0,0,0,hWnd,(HMENU)0,g_hInst,NULL);
		file_firstOpen();
		log("Program Execute");
		getDevice();
		//�����
		DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_INTERFACE_CONFIG), HWND_DESKTOP, InterfaceConfigDlgProc);
		hThread = CreateThread(NULL,0,engin,(LPVOID)&devIndex,0,&ThreadID);
		SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);
		return 0;
		 
	case WM_PAINT:
		hdc = BeginPaint(hWnd,&ps);
		EndPaint(hWnd,&ps);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam)){
			//�ش� �޴����� ���� �Ǿ�� �� ������ �Է�
		case ID_MENU_LOG:
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_DIALOG_LOG), HWND_DESKTOP, LogDlgProc);
			break;
		case ID_MENU_EXIT:
			PostQuitMessage(0);
			break;
		case ID_HELP_HELP:
			DialogBox(g_hInst,MAKEINTRESOURCE(IDD_DIALOG_HELP),hWnd,HelpDlgProc);
			break;
		case ID_HELP_INFO:
			DialogBox(g_hInst,MAKEINTRESOURCE(IDD_DIALOG_INFO),hWnd,AboutDlgProc);
			break;
		case IDM_OPEN:
			ShowWindow(hWnd,SW_SHOW);
			nid.cbSize=sizeof(NOTIFYICONDATA);
			nid.hWnd=hWnd;
			nid.uID=0;
			Shell_NotifyIcon(NIM_DELETE, &nid);
			break;
		case IDM_ABOUT:
			DialogBox(g_hInst,MAKEINTRESOURCE(IDD_DIALOG_INFO),hWnd,AboutDlgProc);
			break;
		case IDM_EXIT:
			log("Program Terminate");
			nid.cbSize=sizeof(NOTIFYICONDATA);
			nid.hWnd=hWnd;
			nid.uID=0;
			Shell_NotifyIcon(NIM_DELETE, &nid);
			PostQuitMessage(0);
			return 0;
		}
		return 0;
		//�� �������� ũ�⸦ ��������
	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED) {
			GetClientRect(hWnd,&crt);
			MoveWindow(hC1,400,0,crt.right-400,crt.bottom,TRUE);
			MoveWindow(hC2,0,0,400,200, TRUE);
			MoveWindow(hC3,0,200,400,crt.bottom-200,TRUE);
		}
		return 0;
		//Ʈ���̿��� �����ؾ� �� �۾��� �������ش�.
	case TRAY_NOTIFY:
		switch(lParam){
			//������ ��ư�� ������ �޴��� ����ǵ��� �Ѵ�.
		case WM_RBUTTONDOWN:
			hMenu=LoadMenu(g_hInst,MAKEINTRESOURCE(IDR_MENU2));
			hPopupMenu=GetSubMenu(hMenu,0);
			GetCursorPos(&pt);
			SetForegroundWindow(hWnd);
			TrackPopupMenu(hPopupMenu,TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,pt.x,pt.y,0,hWnd,NULL);
			SetForegroundWindow(hWnd);
			DestroyMenu(hPopupMenu);
			DestroyMenu(hMenu);
			break;
		}

		return 0;

	case WM_CLOSE:
		//������ â�� �ݰ� �Ǹ� Ʈ���̰� �����ǵ��� �Ѵ�.
	case WM_DESTROY:
		//Ʈ���� �����Լ�
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = hWnd;
		nid.uID = 0;
		nid.uFlags = NIF_ICON|NIF_TIP|NIF_MESSAGE|NIF_INFO;
		nid.uCallbackMessage = TRAY_NOTIFY;
		nid.dwInfoFlags= 0x00004;
		nid.uTimeout = 1000;
		nid.hIcon = LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON1));
		lstrcpy(nid.szTip, TEXT("zi ARP by GHOST"));
		lstrcpy(nid.szInfoTitle, TEXT("zi ARP Ž��"));
		lstrcpy(nid.szInfo, TEXT("zi ARP Ž������ ��׶��忡�� ����ǰ� �ֽ��ϴ�. ������ ���Ḧ ���Ͻø� Ʈ���� �޴����� �������ּ���."));
		Shell_NotifyIcon(NIM_ADD, &nid);

		ShowWindow(hWnd, SW_HIDE);
		return 0;
	
	}

	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}

//=====================================================================================
//
//	* Function : CALLBACK ChildTopProc()
//	* Description 
//		���� ���ϵ��� �޽��� ���ν���
//			���¿� ���� ���� �˸��� ��ȣ���� ������ ��Ÿ������ �Ѵ�.
//
//=====================================================================================
LRESULT CALLBACK ChildTopProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	ChangeState();
	
	switch (iMessage) {
	case WM_PAINT:
		switch(STATE){
		case 0:
			ChangeStatus(hWnd, IDB_BIT_GREEN);
			return 0;

		case 1:
			ChangeStatus(hWnd, IDB_BIT_YELLOW);
			return 0;

		case 2:
			ChangeStatus(hWnd, IDB_BIT_RED);
			return 0;
		}

	}
	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}

//=====================================================================================
//
//	* Function : CALLBACK ChildBottomProc()
//	* Description 
//		�Ʒ��� ���ϵ��� �޽��� ���ν���
//			��Ȳ�� �´� �޼����� ����ϴ� �Լ��� �ҷ�����.
//
//=====================================================================================
LRESULT CALLBACK ChildBottomProc(HWND hWnd,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch (iMessage) {

	case WM_PAINT:
		return WriteText(hWnd,wParam,lParam);
	}

	return(DefWindowProc(hWnd,iMessage,wParam,lParam));
}

//=====================================================================================
//
//	* Function : WriteText()
//	* Description 
//		�Ʒ��� ���ϵ��� �޽��� â�� �� �˸� �޼����� ������ �ش�.
//			���¿� ���� �´� ���� �޼����� ���.
//
//=====================================================================================
LRESULT WriteText(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rt;
	LPTSTR str[]={
		// ���� ���� ����
		TEXT("\n���� ����: ����(Safe)\n\n�������� �ǽɵǴ� ��Ŷ�� �������� �ʾҽ��ϴ�."),
		// ��� ���� ����
		TEXT("\n���� ����: ���(Warning)\n\nARP ��Ǫ�� �������� �ǽɵǴ� ��Ŷ�� �����Ǿ����ϴ�.\n\n")
		TEXT(" �� ��û���� ���� ������Ŷ ����\n\n")
		TEXT(" �� ������ ������ ��Ŷ�� �ݺ��� ��� ���ݻ�Ȳ����\n")
		TEXT("     �ݻ�˴ϴ�.\n\n")
		TEXT(" �� ��Ʈ��ũ�� ���� ���޵Ǵ� ��� ������ �����\n")
		TEXT("     ������ �ֽ��ϴ�.\n"),
		// ���� ���� ����
		TEXT("\n���� ����: ����(Danger)\n\nARP ��Ǫ�� �������� ���ϰ� �ǽɵ˴ϴ�.\n\n")
		TEXT(" �� ��û���� ���� ������Ŷ�� ���������� ����\n\n")
		TEXT(" �� ��Ʈ��ũ�� ���� ���޵Ǵ� ��� ������ �����\n")
		TEXT("     ������ �ֽ��ϴ�.\n\n")
		TEXT(" �� Ʈ���� ��û(������), DNS�ּҺ���(DNS ��Ǫ��),\n")
		TEXT("     ������������ ���ݿ� ����� �� �ֽ��ϴ�.\n")
		TEXT("     ���ͳ� ��뿡 �����Ͻñ� �ٶ��ϴ�.\n\n")
		TEXT(" �� ���� �������� Mac���� ���������˴ϴ�.\n\n")
		TEXT(" �� ������ ��Ŷ�� ���̻� �߻����� ������ ������·�\n")
		TEXT("     ���ư��ϴ�.")
	};

	GetClientRect(hWnd,&rt);
	hdc=BeginPaint(hWnd,&ps);
	//������� ��������
	DrawText(hdc,str[STATE],-1,&rt,DT_LEFT|DT_WORDBREAK);
	//�������
	EndPaint(hWnd,&ps);

	return 0;
}

//=====================================================================================
//
//	* Function : CALLBACK InterfaceConfigDlgProc()
//	* Description 
//		ó�� ������ �� �������̽��� ������ ���̾˷α׸� �����Ѵ�. 
//
//=====================================================================================
BOOL CALLBACK InterfaceConfigDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	TCHAR lpszStr[MAX_STR];
	HWND hList;
	int itemIndex;
	switch(iMessage){
	case WM_INITDIALOG:
		GetInterface(hDlg,lpszStr);
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDOK:
			hList=GetDlgItem(hDlg,IDC_LIST1);
			itemIndex=SendMessage(hList,LB_GETCURSEL,0,0);
			if(SendMessage(hList,LB_GETTEXT,(WPARAM)itemIndex,(LPARAM)lpszStr) == -1)
				return TRUE;
			else{
				SetInterface(itemIndex);
				EndDialog(hDlg,IDOK);
				return TRUE;
			}
		case IDCANCEL:
			EndDialog(hDlg,IDCANCEL);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

//=====================================================================================
//
//	* Function : CALLBACK AboutDlgProc()
//	* Description 
//		��� �޴��� about â�� �� ���� ���� ���� ������ �� ���̾˷α׸� �����Ѵ�.
//
//=====================================================================================
BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch(iMessage){
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDOK:
			EndDialog(hDlg,IDOK);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg,IDCANCEL);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

//=====================================================================================
//
//	* Function : CALLBACK LogDlgProc()
//	* Description 
//		��� �޴��� log â�� �� log�� ���� ���̾˷α׸� �����Ѵ�.
//
//=====================================================================================
BOOL CALLBACK LogDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HWND hEdit;
	switch(iMessage){
	case WM_INITDIALOG:
		hEdit=GetDlgItem(hDlg,IDC_EDIT1);
		return ParseLog(hEdit,iMessage,wParam,lParam);
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDOK:
			EndDialog(hDlg,IDOK);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg,IDCANCEL);
			return TRUE;
		}
		break;
	}

	return FALSE;
}

//=====================================================================================
//
//	* Function : CALLBACK HelpDlgProc()
//	* Description 
//		��� �޴��� help â�� �� ������ ���� ���̾˷α׸� �����Ѵ�.
//
//=====================================================================================
BOOL CALLBACK HelpDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch(iMessage){
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDOK:
			EndDialog(hDlg,IDOK);
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg,IDCANCEL);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

//=====================================================================================
//
//	* Function : CALLBACK DefenceDlgProc()
//	* Description 
//		����� ip�� ����� ����ڰ� �ش� �����Ǹ� �������� �����Ͽ�
//			���ݿ� ���� �� �� �� �ֵ��� �ϴ� �κа� �����Ǵ� ���ν���
//
//=====================================================================================
BOOL CALLBACK DefenceDlgProc(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	TCHAR lpszStr[MAX_STR];
	HWND hList;
	int itemIndex;
	switch (iMessage) {
	case WM_INITDIALOG:
		GetAttackList(hDlg,lpszStr);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:			
			hList=GetDlgItem(hDlg,IDC_LIST1);
			itemIndex=SendMessage(hList,LB_GETCURSEL,0,0);
			if(SendMessage(hList,LB_GETTEXT,(WPARAM)itemIndex,(LPARAM)lpszStr) == -1)
				return TRUE;
			else{
				EndDialog(hDlg,IDOK);
				//��� �������� ����	
				if(defence(itemIndex)){
					/* static ���� ���� */
					MessageBox(hDlg, TEXT("���������� �Ϸ��߽��ϴ�."), TEXT("����"),MB_OK);
				}
				else{
					/* static ���� ���� */
					MessageBox(hDlg, TEXT("���������� �����߽��ϴ�.\n �ٽ� �õ��� �ּ���."), TEXT("����"),MB_OK);
				}
				return TRUE;
			}
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg,IDCANCEL);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

//=====================================================================================
//
//	* Function : ChangeStatus()
//	* Description 
//		���� ���ϵ��� ��ȣ���� ��Ȳ�� �°� �׷��ش�.
//
//=====================================================================================
void ChangeStatus(HWND hWnd, INT IDB_BIT )
{
	static TCHAR str[256];
	HDC hdc, hMemDC;
	PAINTSTRUCT ps;
	HBITMAP MyBitmap, OldBitmap;
	BITMAP bit;
	int bx,by;

	hdc=BeginPaint(hWnd,&ps);
	hMemDC=CreateCompatibleDC(hdc);
	MyBitmap=LoadBitmap(g_hInst,MAKEINTRESOURCE(IDB_BIT));
	OldBitmap=(HBITMAP)SelectObject(hMemDC,MyBitmap);

	GetObject(MyBitmap,sizeof(BITMAP),&bit);
	bx=bit.bmWidth;
	by=bit.bmHeight;

	BitBlt(hdc,0,0,bx,by,hMemDC,0,0,SRCCOPY);

	SelectObject(hMemDC,OldBitmap);
	DeleteObject(MyBitmap);
	DeleteDC(hMemDC);
	EndPaint(hWnd,&ps);
}

//=====================================================================================
//
//	* Function : GetInterface()
//	* Description 
//		�������̽��� �������� �Լ�
//
//=====================================================================================
void GetInterface(HWND hWnd,TCHAR* lpString)
{
	TCHAR strDesc[256];
	TCHAR strIP[128];
	int arTab=50;
	PDEVICE_L Link=deviceList;
	while(Link!=NULL){
		wsprintf(strIP,TEXT("%d.%d.%d.%d"),Link->ipAddr[0],Link->ipAddr[1],Link->ipAddr[2],Link->ipAddr[3]);
		CharToTCHAR(Link->desc,strDesc);
		wsprintf(lpString,TEXT("IP�ּ�: %s\t%s"),strIP,strDesc);
		SendDlgItemMessage(hWnd,IDC_LIST1,LB_ADDSTRING,0,(LPARAM)lpString);
		Link=Link->next;
	}
	SendDlgItemMessage(hWnd,IDC_LIST1,LB_SETTABSTOPS,1,(LPARAM)&arTab);
}

//=====================================================================================
//
//	* Function : GetAttackList()
//	* Description 
//		���ݸ���Ʈ�� ������ �� �� �� ����ڿ��� �����ִ� �Լ�
//
//=====================================================================================
void GetAttackList(HWND hWnd,TCHAR* lpString)
{
	TCHAR strMac[18];
	TCHAR strIP[128];
	int arTab=50;
	PSPOOF_L Link=HEADER_ATTACK;
	while(Link!=NULL){
		wsprintf(strIP,TEXT("%d.%d.%d.%d"),
			Link->ipAddr[0],Link->ipAddr[1],Link->ipAddr[2],Link->ipAddr[3]);

		wsprintf(strMac, TEXT("%02X:%02X:%02X:%02X:%02X:%02X"), Link->macAddr[0],Link->macAddr[1],Link->macAddr[2]
		,Link->macAddr[3],Link->macAddr[4],Link->macAddr[5]);
		wsprintf(lpString,TEXT("IP�ּ�: %s\t%s"),strIP,strMac);
		SendDlgItemMessage(hWnd,IDC_LIST1,LB_ADDSTRING,0,(LPARAM)lpString);
		Link=Link->next;
	}
	SendDlgItemMessage(hWnd,IDC_LIST1,LB_SETTABSTOPS,1,(LPARAM)&arTab);
}

//=====================================================================================
//
//	* Function : CharToTCHAR(), TCHARToChar()
//	* Description 
//		char ���� TCHAR���� �ٲ��ִ� �Լ�
//
//=====================================================================================
void CharToTCHAR(char* char_str, TCHAR* TCHAR_str)
{
	INT index = 0;
	for(index=0; char_str[index]!=NULL; index++){
		wsprintf(TCHAR_str+index,TEXT("%c"),char_str[index]);
	}
}

void TCHARToChar(char* char_str, TCHAR* TCHAR_str)
{
	INT index = 0;
	
	for(index=0; TCHAR_str[index]!=NULL; index++){
		sprintf(char_str+index,"%c",TCHAR_str[index]);
	}
}

//=====================================================================================
//
//	* Function : SetInterface()
//	* Description 
//		���� ������ ������ �Ѱ��� ����̽� �ε��� ���ڰ��� �����ϴ� �Լ�
//
//=====================================================================================
void SetInterface(INT deviceIndex)
{
	devIndex=deviceIndex;
}

//=====================================================================================
//
//	* Function : ParseLog()
//	* Description 
//		�α� â�� �α� ���Ͽ��� �о�� �α� ������ ����ִ� �Լ�
//
//=====================================================================================
LRESULT ParseLog(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	HANDLE hLogFile;
	DWORD dwRead;
	char lpBuffer_c[1024]={NULL,};
	static TCHAR* lpBuffer_t=NULL;
	static TCHAR* lpBuffer_tmp[1024]={NULL,};
	INT i = 0; 
	INT index_max=0;
	INT BufSize=0;
	
	hLogFile=CreateFile(filepath,GENERIC_READ,FILE_SHARE_READ,NULL,
	OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	while(1){
		ReadFile(hLogFile,lpBuffer_c,1024,&dwRead,NULL);
		if(dwRead == 0)
			break;
		free(lpBuffer_tmp[i]);
		lpBuffer_tmp[i]=(TCHAR*)malloc((strlen(lpBuffer_c)+1)*sizeof(TCHAR));
		CharToTCHAR(lpBuffer_c,lpBuffer_tmp[i]);
		BufSize+=(strlen(lpBuffer_c)+1)*sizeof(TCHAR);
		
		i++;
	}
	CloseHandle(hLogFile);
	
	index_max=i;
	
	free(lpBuffer_t);
	lpBuffer_t=(TCHAR*)malloc(BufSize);
	wsprintf(lpBuffer_t,TEXT(""));
	for(i=0;i<index_max;i++) {
		wsprintf(lpBuffer_t,TEXT("%s%s"),lpBuffer_t, lpBuffer_tmp[i]);
	}
	SendMessage(hDlg,WM_SETTEXT,0,(LPARAM)lpBuffer_t);
	return TRUE;
}