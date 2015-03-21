
// PCServerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PCServer.h"
#include "PCServerDlg.h"
#include "afxdialogex.h"
#include "tables.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ID_MENU 1010
#define WM_SHOWTASK (WM_USER + 755)
#define SOCKET_PORT 8888 //Լ����8888�˿�ͨѶ
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~globle~~~~~~~~~~~~~~~~~~~~~~~~~*/
const char* WINDOWNHANDLERNAME = "PCServer";
SOCKET ServerSock;
SOCKET connectSock=-1;
struct sockaddr_in dstclient_addr;
char recv_message_server[256] = {0};
char      send_message[256] = {0};

volatile BOOL ThreadRun = false;
volatile BOOL conStatus = false;
volatile BOOL isCmdExist = false;
HANDLE hThread;
DWORD ThreadID;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~globle~~~~~~~~~~~~~~~~~~~~~~~~~*/
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
public:
	//	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	//	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CPCServerDlg �Ի���



CPCServerDlg::CPCServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPCServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
}

void CPCServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPCServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_STN_CLICKED(IDC_STATIC_CMD, &CPCServerDlg::OnStnClickedStaticCmd)
	ON_STN_CLICKED(IDC_rights, &CPCServerDlg::OnStnClickedrights)
	ON_BN_CLICKED(ID_BTN_START, &CPCServerDlg::OnBnClickedBtnStart)
	ON_WM_CTLCOLOR()
	ON_MESSAGE(WM_SHOWTASK,OnShowTask)
	ON_WM_CLOSE()
	ON_COMMAND(ID_MENU, OnChangmenuitem)
	ON_BN_CLICKED(IDC_BUTTON1, &CPCServerDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CPCServerDlg ��Ϣ��������

BOOL CPCServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵������ӵ�ϵͳ�˵��С�
	
	SetWindowLong(m_hWnd,GWL_STYLE,GetStyle() | WS_MINIMIZEBOX );
	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ����Ӷ���ĳ�ʼ������
	m_nid.cbSize  = (DWORD)sizeof(NOTIFYICONDATA);
	m_nid.hWnd    = this->m_hWnd;
	m_nid.uID     = IDI_ICON1;
	m_nid.uFlags  = NIF_ICON | NIF_MESSAGE | NIF_TIP ;
	m_nid.uCallbackMessage = WM_SHOWTASK;             // �Զ������Ϣ����
	m_nid.hIcon   = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON1));
	strcpy_s(m_nid.szTip, "����������");                // ��Ϣ��ʾ��Ϊ"����������"��VS2008 UNICODE������wcscpy_s()����
	Shell_NotifyIcon(NIM_ADD, &m_nid);                // ������������ͼ��
	
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CPCServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if (nID == SC_MINIMIZE) 
    { 
        ShowWindow(SW_HIDE); 
    }  
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի���������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CPCServerDlg::OnPaint()
{
	
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CPCServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CPCServerDlg::OnStnClickedStaticCmd()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
}


void CPCServerDlg::OnStnClickedrights()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
}

BOOL InItServerSock()
{
	//Variable Define
	int Status;
	WORD wMajorVersion,wMinorVersion;
	WORD wVersionReqd;
	WSADATA lpmyWSAData;

	//InI Winsock
	wMajorVersion = 1;
	wMinorVersion = 2;
	wVersionReqd = MAKEWORD(wMajorVersion,wMinorVersion);

	//Startup WinSock
	Status = WSAStartup(wVersionReqd,&lpmyWSAData);	
	if (Status != 0)
		return FALSE;	
	//Socket
	ServerSock = socket(AF_INET,SOCK_STREAM,0);
	if (ServerSock == INVALID_SOCKET)
		return FALSE;

	dstclient_addr.sin_family = PF_INET;
	dstclient_addr.sin_port = htons(SOCKET_PORT);
	dstclient_addr.sin_addr.s_addr = INADDR_ANY;

	//BIND
	Status = bind(ServerSock,(struct sockaddr far *)&dstclient_addr,sizeof(dstclient_addr));
	if (Status != 0)
	{
		SetDlgItemText(FindWindow(NULL, WINDOWNHANDLERNAME), IDC_STATIC_INFO, "�׽��ְ�ʧ��");
		SetDlgItemText(FindWindow(NULL, WINDOWNHANDLERNAME), ID_BTN_START, "����");
		return FALSE;
	}
	//LISTEN
	Status = listen(ServerSock,1);
	if (Status != 0)
		return FALSE;

	char name[255]; 
	PHOSTENT hostinfo; 
	if( gethostname (name, sizeof(name)) == 0) 
	{ 
		if((hostinfo = gethostbyname(name)) != NULL) 
		{ 
			CString local_IP_address; 
			local_IP_address = inet_ntoa (*(struct in_addr *)*hostinfo->h_addr_list); 
			u_short port = ((dstclient_addr.sin_port & 0xFF00)>>8) | ((dstclient_addr.sin_port & 0x00FF)<<8);

			CString temp;
			temp.Format("���ӣ�%s",local_IP_address);
			SetDlgItemText(FindWindow(NULL, WINDOWNHANDLERNAME), IDC_STATIC_INFO, temp);
		} 
	}

	return TRUE;
}
BOOL waiteAccept()
{
	//ACCEPT
	int len = sizeof(dstclient_addr);
	connectSock = accept(ServerSock,(struct sockaddr far *)&dstclient_addr,&len); //ʧ�ܷ���-1
	if (connectSock==-1)  
	{
		closesocket(connectSock);
		connectSock=-1;
		return FALSE;
	}
	conStatus=true;
	CString temp;
	temp.Format("%s:%d ������!", inet_ntoa(dstclient_addr.sin_addr), dstclient_addr.sin_port);//remoteSockAddr->sa_data
	SetDlgItemText(FindWindow(NULL, WINDOWNHANDLERNAME), IDC_STATIC_CMD, temp);
	return TRUE;
}
void analysisCMD(CString temp,CPCServerDlg *pp)
{
	CPoint point;
	CString cmd;
	float dx,dy;
	char timec[100];
	
	int index = temp.Find("|");

	if(temp.Left(index).Compare("shutdown")==0)
	{
		cmd = "shutdown -s -t ";
		temp = temp.Right(temp.GetLength()-index-1);
		int time = atoi(temp.GetString());
		time *= 60;
		_itoa_s(time,timec,10);
		cmd+=timec;
		//system(cmd);
		isCmdExist=true;
	}
	else if(temp.Left(index).Compare("cancel")==0)
	{
		isCmdExist=false;
		cmd = "shutdown -a";
		//system(cmd);
	}
	else if(temp.Left(index).Compare("mouse")==0)
	{
	
		temp = temp.Right(temp.GetLength()-index-2);//ȥ������ͷ�Լ�x
		int index = temp.Find("y");
		dx = atof(temp.Left(index).GetString());
		temp = temp.Right(temp.GetLength()-index-1);

		index=temp.Find("|");
		if(index!=-1)  //���������쳣�ź�
		{
			dy = atof(temp.Left(index-5).GetString());
			GetCursorPos(&point);
			mouse_event( MOUSEEVENTF_MOVE,dx,dy,0,NULL);
			temp = temp.Right(temp.GetLength()-index-2);
			index = temp.Find("y");
			dx = atof(temp.Left(index).GetString());
			temp = temp.Right(temp.GetLength()-index-1);
		}
		dy = atof(temp.GetString());
	/*	
		CString editStr;
		CString strTime;
		SYSTEMTIME st; 
		
			pp->GetDlgItemTextA(IDC_EDIT_TEST,editStr);
			editStr+="\r\n";
			GetLocalTime(&st); 
			strTime.Format("%02d:%02d:%03d run:",st.wMinute,st.wSecond,st.wMilliseconds);
			editStr+=strTime;
			editStr.AppendFormat("%f",dx);
			editStr+=" ";
			editStr.AppendFormat("%f",dy);
			pp->SetDlgItemText(IDC_EDIT_TEST, editStr);
			
			CEdit *pe;
			pe = (CEdit*) pp->GetDlgItem(IDC_EDIT_TEST);
			index=pe->GetLineCount();
			pe->LineScroll(index,0);
	*/
			
			GetCursorPos(&point);
			mouse_event( MOUSEEVENTF_MOVE,dx,dy,0,NULL);
	}
	else if(temp.Left(index).Compare("lc")==0)
	{
		mouse_event( MOUSEEVENTF_LEFTDOWN,0,0,0,NULL);
		mouse_event( MOUSEEVENTF_LEFTUP,0,0,0,NULL);
	}
	else if(temp.Left(index).Compare("rc")==0)
	{
		mouse_event( MOUSEEVENTF_RIGHTDOWN,0,0,0,NULL);
		mouse_event( MOUSEEVENTF_RIGHTUP,0,0,0,NULL);
	}
	else if(temp.Left(index).Compare("dc")==0)
	{
		mouse_event( MOUSEEVENTF_LEFTDOWN,0,0,0,NULL);
		mouse_event( MOUSEEVENTF_LEFTUP,0,0,0,NULL);
		mouse_event( MOUSEEVENTF_LEFTDOWN,0,0,0,NULL);
		mouse_event( MOUSEEVENTF_LEFTUP,0,0,0,NULL);
	}
	else if(temp.Left(index).Compare("dcm")==0) 
	{
		temp = temp.Right(temp.GetLength()-index-2);//ȥ������ͷ�Լ�x
		int index = temp.Find("y");
		dx = atof(temp.Left(index).GetString());
		temp = temp.Right(temp.GetLength()-index-1);

		index=temp.Find("|");
		if(index!=-1)  //���������쳣�ź�
		{
			dy = atof(temp.Left(index-5).GetString());
			GetCursorPos(&point);
			mouse_event( MOUSEEVENTF_MOVE,dx,dy,0,NULL);
			temp = temp.Right(temp.GetLength()-index-2);
			index = temp.Find("y");
			dx = atof(temp.Left(index).GetString());
			temp = temp.Right(temp.GetLength()-index-1);
		}
		dy = atof(temp.GetString());

		mouse_event( MOUSEEVENTF_LEFTDOWN,0,0,0,NULL);
		mouse_event( MOUSEEVENTF_LEFTDOWN,0,0,0,NULL);
		mouse_event( MOUSEEVENTF_MOVE,dx*5,dy*5,0,NULL);
	}
	else if(temp.Left(index).Compare("keyboard")==0)
	{
		temp = temp.Right(temp.GetLength()-index-1); //ȥ�� keybord|
		LPCSTR value = temp.GetString();
#if 1
		CString editStr;
		CString strTime;
		SYSTEMTIME st; 
		
			pp->GetDlgItemTextA(IDC_EDIT_TEST,editStr);
			editStr+="\r\n";
			GetLocalTime(&st); 
			strTime.Format("%02d:%02d:%03d run:",st.wMinute,st.wSecond,st.wMilliseconds);
			editStr+=strTime;
			editStr.AppendFormat("%c = %d[%d]", *value, *value, strlen(value));
			pp->SetDlgItemText(IDC_EDIT_TEST, editStr);
			
			CEdit *pe;
			pe = (CEdit*) pp->GetDlgItem(IDC_EDIT_TEST);
			index=pe->GetLineCount();
			pe->LineScroll(index,0);
#endif		
		if ( strlen(value) != 1 )
			return;
		
		if( *value >= 'a' && *value <= 'z' )
		{
			keybd_event(letterTable[*value - 'a'], 0, 0, 0); //���¼�
			keybd_event(letterTable[*value - 'a'],0,KEYEVENTF_KEYUP,0);
		}

		if ( (*value >= '0' && *value <= '9') || *value == ' ' )
		{	
			keybd_event(*value, 0, 0, 0); //���¼�
			keybd_event(*value,0,KEYEVENTF_KEYUP,0);
		}

		if ( *value >= 'A' && *value <= 'Z' )
		{	
			keybd_event(VK_SHIFT, 0, 0, 0);
			keybd_event(*value, 0, 0, 0);
			keybd_event(*value, 0, KEYEVENTF_KEYUP, 0);
			keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
		}

	}
}

DWORD WINAPI CPCServerDlg::threadFunc(LPVOID threadNum)
{
	SYSTEMTIME st; 
	CString strTime;
	CPCServerDlg *pp = (CPCServerDlg*)threadNum;
	int length;	
	CString temp,editStr;
	if(!InItServerSock())
		return 0;
	if(!waiteAccept())
		return 0;
	//�ȴ����ӵ�ʱ�����ӿ��ܱ�ȡ����
	if(!ThreadRun)
	{
		pp->SetDlgItemText(IDC_STATIC_INFO, "����������");
		return 0;
		
	}
	while(ThreadRun)
	{
		length = recv(connectSock,(char*)recv_message_server,sizeof(recv_message_server),0);
		if( length>0)
		{	
			
			temp.Format("%s",recv_message_server);
#if 1	
			pp->GetDlgItemTextA(IDC_EDIT_TEST,editStr);
			if(!editStr.IsEmpty())
				editStr+="\r\n";
			GetLocalTime(&st); 
			strTime.Format("%02d:%02d:%03d   ",st.wMinute,st.wSecond,st.wMilliseconds);
			editStr+=strTime;
			editStr+=recv_message_server;
			pp->SetDlgItemText(IDC_EDIT_TEST, editStr);
		
			CEdit *pe;
			pe = (CEdit*) pp->GetDlgItem(IDC_EDIT_TEST);
			int index=pe->GetLineCount();
			pe->LineScroll(index,0);
#endif	
			analysisCMD(temp,pp); //��������н���
			memset(recv_message_server, 0, sizeof(recv_message_server));
			send(connectSock,"0",strlen("0"), 0);
		}
		else
		{
			conStatus=false;
			pp->SetDlgItemText(IDC_STATIC_CMD, "ͨѶ�ж�");
			waiteAccept();
		}
	}
	return 0;
}
void CPCServerDlg::OnBnClickedBtnStart()
{
	// TODO: 
	if(ThreadRun == false)
	{
		ThreadRun = true; // �߳�״̬ ��Ϊ ��accept��ɺ��޸�
		hThread=CreateThread(NULL,
			0,
			threadFunc,
			this,//���ݵĲ���
			0,
			&ThreadID);
		SetDlgItemText( ID_BTN_START,"ֹͣ����");
		this->Invalidate();
	}
	else
	{	
		if(conStatus)
		{
			if(MessageBox("��ܰ��ʾ��������,ȷ���Ͽ�?","��ʾ",MB_YESNO)==IDNO)
			   return ;
		}
		if(isCmdExist)   //ȡ������
		{
			system("shutdown -a");
			isCmdExist=false;
		}
		ThreadRun = false;
		closesocket(connectSock);
		closesocket(ServerSock);	
		SetDlgItemText(IDC_STATIC_INFO, "> �ȿ������� <");
		SetDlgItemText(IDC_STATIC_CMD, "δ����");
		SetDlgItemText(ID_BTN_START,"��������");
		SetDlgItemText(IDC_EDIT_TEST, "");
		this->Invalidate();
	}

}



HBRUSH CPCServerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	if(pWnd->GetDlgCtrlID()==IDC_rights && ThreadRun){

		pDC->SetTextColor(RGB(255,0,0)); //��Ϊ��ɫ
	}
	if(pWnd->GetDlgCtrlID()==IDC_STATIC_INFO && ThreadRun){
		pDC->SetTextColor(RGB(0,0,255)); //��Ϊ��ɫ
	}

	return hbr;


}


LRESULT CPCServerDlg::OnShowTask(WPARAM wParam, LPARAM lParam)
{
	if(wParam != IDI_ICON1)
		return 1;
	switch(lParam)
	{
	case WM_RBUTTONUP:                                        // �Ҽ�����ʱ�����˵�
		{
			LPPOINT lpoint = new tagPOINT;
			::GetCursorPos(lpoint);                    // �õ����λ��
			CMenu menu;
			menu.CreatePopupMenu();                    // ����һ������ʽ�˵�
			menu.AppendMenu(MF_STRING,ID_MENU,"�ر�");
			menu.TrackPopupMenu(TPM_LEFTALIGN, lpoint->x ,lpoint->y, this);
			HMENU hmenu = menu.Detach();
			menu.DestroyMenu();
			delete lpoint;
		}
		break;
	case WM_LBUTTONDOWN:                                 // ˫������Ĵ���
		{
			this->ShowWindow(SW_SHOWNORMAL);         // ��ʾ������
		}
		break;
	}
	return 0;

}

void CPCServerDlg::OnClose()
{
	// TODO: �ڴ�������Ϣ������������/�����Ĭ��ֵ
	if(conStatus)
	{
		if(MessageBox("��ܰ��ʾ��������,ȷ���ر�?","��ʾ",MB_YESNO)==IDNO)
			return ;
	}
	else
	{
		if(MessageBox("��ܰ��ʾ��ȷ���ر�?","��ʾ",MB_YESNO)==IDNO)
			return ;
	}
	CDialogEx::OnClose();
}


void CPCServerDlg::OnChangmenuitem()

{
	if(conStatus)
	{
		if(MessageBox("��ܰ��ʾ��������,ȷ���ر�?","��ʾ",MB_YESNO)==IDNO)
			return ;
	}
	else
	{
		if(MessageBox("��ܰ��ʾ��ȷ���ر�?","��ʾ",MB_YESNO)==IDNO)
			return ;
	}
	this->DestroyWindow();
       // ����д��Ҫ��δ����Ĵ���     

}



void CPCServerDlg::OnBnClickedButton1()
{
	// TODO: �ڴ����ӿؼ�֪ͨ�����������
	CPoint point;
	GetCursorPos(&point);
	mouse_event( MOUSEEVENTF_MOVE,100,0,0,NULL);
	mouse_event( MOUSEEVENTF_RIGHTDOWN,0,0,0,NULL);
	mouse_event( MOUSEEVENTF_RIGHTUP,0,0,0,NULL);
}
