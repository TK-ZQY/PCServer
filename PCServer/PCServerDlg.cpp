
// PCServerDlg.cpp : 实现文件
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
#define ID_MENU_CANCEL 1011
#define WM_SHOWTASK (WM_USER + 755)
#define SOCKET_PORT 8888 //约定用8888端口通讯
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~globle~~~~~~~~~~~~~~~~~~~~~~~~~*/
const char* WINDOWNHANDLERNAME = "PCServer";
SOCKET ServerSock;
SOCKET connectSock = -1;
struct sockaddr_in dstclient_addr;
char recv_message_server[256] = { 0 };
char      send_message[256] = { 0 };

volatile BOOL ThreadRun = false;
volatile BOOL conStatus = false;
volatile BOOL isCmdExist = false;
HANDLE hThread;
DWORD ThreadID;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~globle~~~~~~~~~~~~~~~~~~~~~~~~~*/
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
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


// CPCServerDlg 对话框



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
	ON_MESSAGE(WM_SHOWTASK, OnShowTask)
	ON_WM_CLOSE()
	ON_COMMAND(ID_MENU, OnChangmenuitem)
	ON_BN_CLICKED(IDC_BUTTON1, &CPCServerDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CPCServerDlg 消息处理程序

BOOL CPCServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	SetWindowLong(m_hWnd, GWL_STYLE, GetStyle() | WS_MINIMIZEBOX);
	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	m_nid.hWnd = this->m_hWnd;
	m_nid.uID = IDI_ICON1;
	m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	m_nid.uCallbackMessage = WM_SHOWTASK;             // 自定义的消息名称
	m_nid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON1));
	char *strtp = "服务器程序";
	CString temp(strtp);
	wcscpy(m_nid.szTip, temp);
	//strcpy_s(m_nid.szTip, "服务器程序");                // 信息提示条为"服务器程序"，VS2008 UNICODE编码用wcscpy_s()函数
	Shell_NotifyIcon(NIM_ADD, &m_nid);                // 在托盘区添加图标

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CPCServerDlg::OnPaint()
{

	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CPCServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CPCServerDlg::OnStnClickedStaticCmd()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CPCServerDlg::OnStnClickedrights()
{
	// TODO: 在此添加控件通知处理程序代码
}

BOOL InItServerSock(CPCServerDlg* pp)
{
	//Variable Define
	int Status;
	WORD wMajorVersion, wMinorVersion;
	WORD wVersionReqd;
	WSADATA lpmyWSAData;

	//InI Winsock
	wMajorVersion = 1;
	wMinorVersion = 2;
	wVersionReqd = MAKEWORD(wMajorVersion, wMinorVersion);

	//Startup WinSock
	Status = WSAStartup(wVersionReqd, &lpmyWSAData);
	if (Status != 0)
		return FALSE;
	//Socket
	ServerSock = socket(AF_INET, SOCK_STREAM, 0);
	if (ServerSock == INVALID_SOCKET)
		return FALSE;

	dstclient_addr.sin_family = PF_INET;
	dstclient_addr.sin_port = htons(SOCKET_PORT);
	dstclient_addr.sin_addr.s_addr = INADDR_ANY;

	//BIND
	Status = bind(ServerSock, (struct sockaddr far *)&dstclient_addr, sizeof(dstclient_addr));
	if (Status != 0)
	{
		pp->SetDlgItemText(IDC_STATIC_INFO, (LPCWSTR)"套接字绑定失败");
		pp->SetDlgItemText(ID_BTN_START, (LPCWSTR)"重试");
		return FALSE;
	}
	//LISTEN
	Status = listen(ServerSock, 1);
	if (Status != 0)
		return FALSE;

	char name[255];
	PHOSTENT hostinfo;
	if (gethostname(name, sizeof(name)) == 0)
	{
		if ((hostinfo = gethostbyname(name)) != NULL)
		{
			CString local_IP_address = NULL;
			local_IP_address = inet_ntoa(*(struct in_addr *)*hostinfo->h_addr_list);
			u_short port = ((dstclient_addr.sin_port & 0xFF00) >> 8) | ((dstclient_addr.sin_port & 0x00FF) << 8);
			CString info("请连接：");
			info.Append(local_IP_address);
			pp->SetDlgItemTextW(IDC_STATIC_INFO, info);
		}
	}

	return TRUE;
}
BOOL waitAccept(CPCServerDlg *pp)
{
	//ACCEPT
	int len = sizeof(dstclient_addr);
	connectSock = accept(ServerSock, (struct sockaddr far *)&dstclient_addr, &len); //失败返回-1
	if (connectSock<0)
	{
		closesocket(connectSock);
		connectSock = -1;
		return false;
	}
	conStatus = true;
	CString tp = NULL;
	/*可能多线程上下文切换原因，台式机上一段时间不连接Server,
	*即使accept失败仍,会有一个127.0.0.1连接到它。调试效果和运行不同
	*下面是一个临时的解决方法
	*/
	if (CString(inet_ntoa(dstclient_addr.sin_addr)).Compare((CString)"127.0.0.1") == 0)
		return false;
	if (connectSock == -1)
		return false;
	tp.Format(L"%d %s:%d 已连接!", connectSock,CString(inet_ntoa(dstclient_addr.sin_addr)), dstclient_addr.sin_port);//remoteSockAddr->sa_data

	pp->SetDlgItemText(IDC_STATIC_CMD, tp);
	return true;
}
char *stringToChar(CString temp)
{
	int len = WideCharToMultiByte(CP_ACP, 0, temp, -1, NULL, 0, NULL, NULL);
	char *fuck = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, temp, -1, fuck, len, NULL, NULL);
	return fuck;
}
void analysisCMD(CString temp, CPCServerDlg *pp)
{
	CPoint point;
	CString cmd;
	float dx, dy;
	char timec[100];

	int index = temp.Find('|');
	if (temp.Left(index).Compare((CString)"shutdown") == 0)
	{
		cmd = "shutdown -s -t ";
		temp = temp.Right(temp.GetLength() - index - 1);
		char *fuck = stringToChar(temp);
		int time = atoi(fuck);
		time *= 60;
		_itoa_s(time, timec, 10);
		cmd += timec;
		//system(cmd);
		isCmdExist = true;
	}
	else if (temp.Left(index).Compare((CString)"cancel") == 0)
	{
		isCmdExist = false;
		cmd = "shutdown -a";
		//system(cmd);
	}
	else if (temp.Left(index).Compare((CString)"mouse") == 0)
	{
		temp = temp.Right(temp.GetLength() - index - 2);//去掉命令头以及x
		int index = temp.Find('y');
		dx = atof(stringToChar(temp.Left(index).GetString()));
		temp = temp.Right(temp.GetLength() - index - 1);

		index = temp.Find('|');
		if (index != -1)  //处理两条异常信号
		{
			dy = atof(stringToChar(temp.Left(index - 5).GetString()));
			GetCursorPos(&point);
			mouse_event(MOUSEEVENTF_MOVE, dx, dy, 0, NULL);
			temp = temp.Right(temp.GetLength() - index - 2);
			index = temp.Find('y');
			dx = atof(stringToChar(temp.Left(index).GetString()));
			temp = temp.Right(temp.GetLength() - index - 1);
		}
		dy = atof(stringToChar(temp.GetString()));
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
		mouse_event(MOUSEEVENTF_MOVE, dx, dy, 0, NULL);
	}
	else if (temp.Left(index).Compare((CString)"lc") == 0)
	{
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, NULL);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, NULL);
	}
	else if (temp.Left(index).Compare((CString)"rc") == 0)
	{
		mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, NULL);
		mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, NULL);
	}
	else if (temp.Left(index).Compare((CString)"dc") == 0)
	{
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, NULL);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, NULL);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, NULL);
		mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, NULL);
	}
	else if (temp.Left(index).Compare((CString)"dcm") == 0)
	{
		temp = temp.Right(temp.GetLength() - index - 2);//去掉命令头以及x
		int index = temp.Find('y');
		dx = atof(stringToChar((temp.Left(index).GetString())));
		temp = temp.Right(temp.GetLength() - index - 1);

		index = temp.Find('|');
		if (index != -1)  //处理两条异常信号
		{
			dy = atof((char*)temp.Left(index - 5).GetString());
			GetCursorPos(&point);
			mouse_event(MOUSEEVENTF_MOVE, dx, dy, 0, NULL);
			temp = temp.Right(temp.GetLength() - index - 2);
			index = temp.Find('y');
			dx = atof((char*)temp.Left(index).GetString());
			temp = temp.Right(temp.GetLength() - index - 1);
		}
		dy = atof((char*)(temp.GetString()));

		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, NULL);
		mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, NULL);
		mouse_event(MOUSEEVENTF_MOVE, dx * 5, dy * 5, 0, NULL);
	}
	else if (temp.Left(index).Compare((CString)"keyboard") == 0)
	{
		temp = temp.Right(temp.GetLength() - index - 1); //去掉 keybord|
		LPCSTR value = (LPCSTR)temp.GetString();
#if 1
		CString editStr;
		SYSTEMTIME st;

		pp->GetDlgItemTextW(IDC_EDIT_TEST, editStr);
		editStr += "\r\n";
		GetLocalTime(&st);
		CString tempTime = NULL;
		tempTime.Format(_T("%02d:%02d:%03d run:"), st.wMinute, st.wSecond, st.wMilliseconds);
		editStr += tempTime;
		CString tempCMD = NULL;
		tempCMD.Format(_T("%c = %d[%d]"), *value, *value, strlen(value));
		editStr += tempCMD;
		pp->SetDlgItemText(IDC_EDIT_TEST, editStr);

		CEdit *pe;
		pe = (CEdit*)pp->GetDlgItem(IDC_EDIT_TEST);
		index = pe->GetLineCount();
		pe->LineScroll(index, 0);
#endif		
		if (strlen(value) != 1)
			return;

		if (*value >= 'a' && *value <= 'z')
		{
			keybd_event(letterTable[*value - 'a'], 0, 0, 0); //按下键
			keybd_event(letterTable[*value - 'a'], 0, KEYEVENTF_KEYUP, 0);
		}

		if ((*value >= '0' && *value <= '9') || *value == ' ')
		{
			keybd_event(*value, 0, 0, 0); //按下键
			keybd_event(*value, 0, KEYEVENTF_KEYUP, 0);
		}

		if (*value >= 'A' && *value <= 'Z')
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
	CPCServerDlg *pp = (CPCServerDlg*)threadNum;
	
	CString editStr;
	if (!InItServerSock(pp))
		return 0;
	while (!waitAccept(pp));
	//等待连接的时候，连接可能被取消。
	if (!ThreadRun)
	{
		pp->SetDlgItemText(IDC_STATIC_INFO, (CString)"请启动服务！");
		return 0;

	}
	while (ThreadRun)
	{
		int length = -1;
		length = recv(connectSock, (char*)recv_message_server, sizeof(recv_message_server), 0);
		if (length>0)
		{
			CString tempStr = NULL;
			tempStr.Format(_T("%s"), (CString)recv_message_server);
#if 1	
			pp->GetDlgItemTextW(IDC_EDIT_TEST, editStr);
			if (!editStr.IsEmpty())
				editStr += "\r\n";
			GetLocalTime(&st);
			CString strTime = NULL;
			strTime.Format(_T("%02d:%02d:%03d   "), st.wMinute, st.wSecond, st.wMilliseconds);
			editStr += strTime;
			editStr += recv_message_server;
			pp->SetDlgItemText(IDC_EDIT_TEST, editStr);

			CEdit *pe;
			pe = (CEdit*)pp->GetDlgItem(IDC_EDIT_TEST);
			int index = pe->GetLineCount();
			pe->LineScroll(index, 0);
#endif	
			analysisCMD(tempStr, pp); //对命令进行解析
			memset(recv_message_server, 0, sizeof(recv_message_server));
			send(connectSock, "0", strlen("0"), 0);
		}
		else
		{
			conStatus = false;
			pp->SetDlgItemText(IDC_STATIC_CMD, (CString)"通讯中断");
			while (!waitAccept(pp));
		}
	}
	return 0;
}
void CPCServerDlg::OnBnClickedBtnStart()
{
	// TODO: 
	if (ThreadRun == false)
	{
		ThreadRun = true; // 线程状态 改为 在accept完成后修改
		hThread = CreateThread(NULL,
			0,
			threadFunc,
			this,//传递的参数
			0,
			&ThreadID);
		CString tip("停止服务");
		SetDlgItemTextW(ID_BTN_START, tip);
		this->Invalidate();
	}
	else
	{
		if (conStatus)
		{
			if (MessageBox((CString)"温馨提示！已连接,确定断开?", (CString)"提示", MB_YESNO) == IDNO)
				return;
		}
		if (isCmdExist)   //取消命令
		{
			system("shutdown -a");
			isCmdExist = false;
		}
		ThreadRun = false;
		closesocket(connectSock);
		closesocket(ServerSock);
		SetDlgItemText(IDC_STATIC_INFO, (CString)"> 先开启服务 <");
		SetDlgItemText(IDC_STATIC_CMD, (CString)"未连接");
		SetDlgItemText(ID_BTN_START, (CString)"开启服务");
		SetDlgItemText(IDC_EDIT_TEST, (CString)"");
		conStatus = false;
		this->Invalidate();
	}

}



HBRUSH CPCServerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	if (pWnd->GetDlgCtrlID() == IDC_rights && ThreadRun){

		pDC->SetTextColor(RGB(255, 0, 0)); //设为红色
	}
	if (pWnd->GetDlgCtrlID() == IDC_STATIC_INFO && ThreadRun){
		pDC->SetTextColor(RGB(0, 0, 255)); //设为绿色
	}

	return hbr;


}


LRESULT CPCServerDlg::OnShowTask(WPARAM wParam, LPARAM lParam)
{
	if (wParam != IDI_ICON1)
		return 1;
	switch (lParam)
	{
	case WM_RBUTTONUP:                                        // 右键起来时弹出菜单
	{
		LPPOINT lpoint = new tagPOINT;
		::GetCursorPos(lpoint);                    // 得到鼠标位置
		CMenu menu;
		menu.CreatePopupMenu();
		// 声明一个弹出式菜单
		CString s("关闭");
		menu.AppendMenu(MF_STRING, ID_MENU, s);
		menu.TrackPopupMenu(TPM_LEFTALIGN, lpoint->x, lpoint->y, this);
		//HMENU hmenu = menu.Detach();
	//	menu.DestroyMenu();
		//delete lpoint;
	}
	break;
	case WM_LBUTTONDOWN:                                 // 双击左键的处理
	{
		this->ShowWindow(SW_SHOWNORMAL);         // 显示主窗口
	}
	break;
	}
	return 0;

}

void CPCServerDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (conStatus)
	{
		if (MessageBox((CString)"温馨提示！已连接,确定关闭?", (CString)"提示", MB_YESNO) == IDNO)
			return;
	}
	else
	{
		if (MessageBox((CString)"温馨提示！确定关闭?", (CString)"提示", MB_YESNO) == IDNO)
			return;
	}
	Shell_NotifyIcon(NIM_DELETE, &m_nid);
	CDialogEx::OnClose();
}


void CPCServerDlg::OnChangmenuitem()

{
	if (conStatus)
	{
		if (MessageBox((CString)"温馨提示！已连接,确定关闭?", (CString)"提示", MB_YESNO) == IDNO)
			return;
	}
	else
	{
		if (MessageBox((CString)"温馨提示！确定关闭?", (CString)"提示", MB_YESNO) == IDNO)
			return;
	}
	Shell_NotifyIcon(NIM_DELETE, &m_nid);
	this->DestroyWindow();
	// 这里写你要如何处理的代码     

}


void CPCServerDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CPoint point;
	GetCursorPos(&point);
	mouse_event(MOUSEEVENTF_MOVE, 100, 0, 0, NULL);
	mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, NULL);
	mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, NULL);
}

