#include "stdafx.h"
#include "lj.h"
#include "ljDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
//增加的头文件
#include <pcap.h>
#include <remote-ext.h>
//////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
		//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLjDlg dialog

CLjDlg::CLjDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLjDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLjDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLjDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLjDlg)
	DDX_Control(pDX, IDC_IPADDR, m_OneIP);
	DDX_Control(pDX, IDC_COMBO_WANGKA, m_wangka);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLjDlg, CDialog)
	//{{AFX_MSG_MAP(CLjDlg)
	ON_BN_CLICKED(IDC_RADIO_TCP, OnRadioTcp)
	ON_BN_CLICKED(IDC_RADIO_UDP, OnRadioUdp)
	ON_BN_CLICKED(IDC_RADIO_IP, OnRadioIp)
	ON_BN_CLICKED(IDC_BUTTON_START, OnButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, OnButtonExit)
	ON_BN_CLICKED(IDC_BUTTON1, SaveDate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLjDlg message handlers

BOOL CLjDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	//////////////////////////////////////////////////////////////////////////
	//这个是主对话框的初始化
	m_handle = NULL;//句柄变量
	m_iOneNum = 0;
	m_iTCPNum = 0;
	m_iUDPNum = 0;

	pcap_if_t * alldevs;
	pcap_if_t * d;
	char errbuf[PCAP_ERRBUF_SIZE];//保存错误信息

	//找设备
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
	{
		//        err.Format("Error in pcap_findalldevs: %s", errbuf);
		//		MessageBox(NULL,err,"错误",MB_OK);
		return FALSE;
	}

	//界面网卡控件的初始化显示
	for (d = alldevs; d != NULL; d = d->next)
	{
		//有OSes不支持适配器的"description"字段，这种情况的处理
		if (d->description)
			m_wangka.AddString(d->description);
		else
		{
			CString temp;
			temp.Format("%s(No description)", d->name);
			m_wangka.AddString(temp);
		}
	}

	pcap_freealldevs(alldevs);	//释放设备列表资源

	//设置网卡初始化选项
	m_wangka.SetCurSel(0);

	//控件的设置
	((CButton *)GetDlgItem(IDC_RADIO_TCP))->SetCheck(1);
	m_bTCP = TRUE;
	m_bUDP = FALSE;
	m_bOneIP = FALSE;

	GetDlgItem(IDC_IPADDR)->EnableWindow(FALSE);

	/*对界面IP地址控件和端口控件初始化*/

	BYTE IP[4]; //数组装32位IP地址
	struct hostent * shHostent; //主机信息结构体
	char szHostName[128]; //主机名

	gethostname(szHostName, 128); // 获得主机名

	/*if语句由主机名得主机信息结构体,从而对获得32位IP地址*/
	if ((shHostent = gethostbyname(szHostName)) != NULL) //由主机名得主机信息
	{
		//得到主机信息后对IP地址赋值
		IP[0] = ((struct in_addr *)shHostent->h_addr_list[0])->S_un.S_un_b.s_b1;
		IP[1] = ((struct in_addr *)shHostent->h_addr_list[0])->S_un.S_un_b.s_b2;
		IP[2] = ((struct in_addr *)shHostent->h_addr_list[0])->S_un.S_un_b.s_b3;
		IP[3] = ((struct in_addr *)shHostent->h_addr_list[0])->S_un.S_un_b.s_b4;
	}
	else
		//得主机信息失败返回false
		return FALSE;

	//对IP地址控件赋值
	((CIPAddressCtrl *)GetDlgItem(IDC_IPADDR))->SetAddress(IP[0], IP[1], IP[2], IP[3]);
	((CIPAddressCtrl *)GetDlgItem(IDC_IPADDR))->EnableWindow(false);//禁用IP地址控件

	////////////连接数据库//////////////
	CoInitialize(NULL);
	m_pConnection.CreateInstance(__uuidof(Connection));

	try
	{
		m_pConnection->Open("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=lj.mdb", "", "", adModeUnknown);///连接数据库
		///上面一句中连接字串中的Provider是针对ACCESS2000环境的，对于ACCESS97,需要改为:Provider=Microsoft.Jet.OLEDB.3.51;  }
	}
	catch (_com_error e)///捕捉异常
	{
		AfxMessageBox("数据库连接失败，确认数据库lj.mdb是否在当前路径下!");
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////

	return TRUE;  // return TRUE  unless you set the focus to a control
}


//点击TCP单选框的操作
void CLjDlg::OnRadioTcp()
{
	// TODO: Add your control notification handler code here
	m_bTCP = TRUE;
	m_bUDP = FALSE;
	m_bOneIP = FALSE;
	GetDlgItem(IDC_IPADDR)->EnableWindow(FALSE);
}
//点击UDP单选框的操作
void CLjDlg::OnRadioUdp()
{
	// TODO: Add your control notification handler code here
	m_bTCP = FALSE;
	m_bUDP = TRUE;
	m_bOneIP = FALSE;
	GetDlgItem(IDC_IPADDR)->EnableWindow(FALSE);
}
//点击IP单选框的操作
void CLjDlg::OnRadioIp()
{
	// TODO: Add your control notification handler code here
	m_bTCP = FALSE;
	m_bUDP = FALSE;
	m_bOneIP = TRUE;
	GetDlgItem(IDC_IPADDR)->EnableWindow(TRUE);
}

//线程处理函数
DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	CLjDlg * m_dlg = (CLjDlg *)lpParameter;

	struct pcap_pkthdr *header;	//捕获数据包的头
	const u_char *pkt_data; //指向常量的指针
	int res; //pcap_next_ex（捕获数据包函数）的返回值

	m_dlg->SetDlgItemText(IDC_STATIC_TIP, "线程开始捕获数据.");
	while ((res = pcap_next_ex(m_dlg->adhandle, &header, &pkt_data)) >= 0)
	{
		if (res == 0)//超时
			continue;
		//处理捕获的数据包
		m_dlg->SaveDate(header, pkt_data);
	}
	return 1;
}

//点击捕获开始按键
void CLjDlg::OnButtonStart()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);//把界面上的值传到后台
	pcap_if_t * alldevs;
	pcap_if_t * d;
	CString temp;
	char errbuf[PCAP_ERRBUF_SIZE];

	//找设置
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
	{
		MessageBox("查找设备出错", "提示");
		return;
	}
	//得到要捕获的网卡名
	m_wangka.GetLBText(m_wangka.GetCurSel(), temp);

	//转到指定网卡
	for (d = alldevs; d != NULL; d = d->next)
	{
		if (d->description)
		{
			if (temp.Compare(d->description) == 0)
				break;
		}
		else
		{
			CString temp1;
			temp1.Format("%s(No description)", d->name);
			if (temp.Compare(temp1))
				break;
		}
	}

	m_OneIP.GetAddress(ip1, ip2, ip3, ip4);
	sprintf(m_oneIP, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);//把用户输入的IP地址传入后台
	//打开设备
	adhandle = pcap_open(d->name, 65536, PCAP_OPENFLAG_PROMISCUOUS, 1000, NULL, errbuf);
	//参数1 带表找到的网卡名字 参数2代表捕获的最大字节数，参数3是否把网卡设为混杂模式，参数4捕获时间限制，单位毫秒。后面是错误信息缓存
	//这里创建一个线程来处理数据
	LPDWORD ThID = NULL;
	m_handle = CreateThread(NULL, 0, ThreadProc, this, 0, ThID);//把创建线程的句柄附值给句柄变量
														 //参数3为线程处理函数，参数4是处理函数的参数
														//参数6是线程ID
	if (m_handle == NULL)
	{
		return;
	}
}

void CLjDlg::OnButtonStop()
{
	// TODO: Add your control notification handler code here
	if (m_handle == NULL)
	{
		SetDlgItemText(IDC_STATIC_TIP, "线程没有启动.");//把引号里的文字显示在前面的空间中
			  //设置窗口控件文本                                         
		return;
	}
	if (TerminateThread(m_handle, -1) == 0)//关闭线程
	{
		return;
	}
	m_handle = NULL;//释放线程里面的数据信息
	SetDlgItemText(IDC_STATIC_TIP, "捕获线程已经停止.");
	m_pRecordset->Close();//数据库记录集关闭
}

void CLjDlg::SaveDate(pcap_pkthdr *header, const u_char *pkt_data)
{
	//如果没有数据则返回
	if (header == NULL || pkt_data == NULL)
		return;

	//数据包分析和保存
	ipheader * ih;
	tcp* tcph;
	udp* udph;

	CString temp;
	int chang;

	//时间
	struct tm *ltime;
	ltime = localtime(&header->ts.tv_sec);//把捕获打的时间通过localtime函数传给lite变量
	strftime(timestr, sizeof(timestr), "%H:%M:%S", ltime);//把ltime以中间参数形式传给timestr

	//IP
	ih = (ipheader*)(pkt_data + 14);//ip头(把pkt_data指针+14个内存位置强行转换成ipheader*格式)
	int iplen = (ih->ver_ihl & 0xf) * 4;//获取数据包长度
	itoa(iplen, IPLength, 10);//整形向字符型转换 按10进制转换

	sprintf(IPDateLength, "%d", header->len);//IP数据包长度

	sprintf(TTL, "%d", ih->ttl);//生存时间

	sprintf(SIP, "%d.%d.%d.%d", ih->saddr.byte1, ih->saddr.byte2, ih->saddr.byte3, ih->saddr.byte4);
	sprintf(DIP, "%d.%d.%d.%d", ih->daddr.byte1, ih->daddr.byte2, ih->daddr.byte3, ih->daddr.byte4);

	u_short u_sport, u_dport;
	if (ih->proto == 6) //这里是TCP数据包
	{
		strcpy(proto, "TCP"); //协议类型

		tcph = (tcp*)((u_char*)ih + iplen);

		u_sport = ntohs(tcph->sport); //源端口
		u_dport = ntohs(tcph->dport); //目的端口
//		sprintf(Sport,"%d",tcph->sport); //源端口
//		sprintf(Dport,"%d",tcph->dport); //目的端口
		chang = (tcph->other ^ 0x0fff) / 1024;
		sprintf(tcp_hLength, "%d", chang); //TCP首部长度
		sprintf(date, "%X", tcph->content); //TCP数据
	}
	else if (ih->proto == 17) //这里是UDP数据库
	{
		strcpy(proto, "UDP");
		udph = (udp*)((u_char*)ih + iplen);

		u_sport = ntohs(udph->sport); //源端口
		u_dport = ntohs(udph->dport); //目的端口
//		sprintf(Sport,"%d",udph->sport); //源端口
//		sprintf(Dport,"%d",udph->dport); //目的端口
		sprintf(udplength, "%d", udph->len); //TCP首部长度
		sprintf(date, "%X", udph->content); //TCP数据
	}
	sprintf(Sport, "%d", u_sport); //源端口
	sprintf(Dport, "%d", u_dport); //目的端口

	AccessWrite(ih->proto);
}

void CLjDlg::AccessWrite(int Type)
{
	char temp[255];
	CString pktNum;
	BOOL OperType = FALSE;
	if (m_bOneIP)
	{
		if (strcmp(m_oneIP, SIP) != 0 && strcmp(m_oneIP, DIP) != 0)return;

		m_iOneNum++;
		pktNum.Format("OneIP方式包%d个", m_iOneNum);//字符串类函数 以整形显示
		SetDlgItemText(IDC_STATIC_ONEIP, pktNum);

		m_pRecordset.CreateInstance(__uuidof(Recordset));//创建记录集
		m_pRecordset->Open("SELECT * FROM 目标IP", m_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
		try
		{
			m_pRecordset->AddNew();//添加新记录
			m_pRecordset->PutCollect("时间", _variant_t(timestr));//向列中添值
			m_pRecordset->PutCollect("IP首部长度", _variant_t(IPLength));
			m_pRecordset->PutCollect("IP数据包长度", _variant_t(IPDateLength));
			m_pRecordset->PutCollect("生存时间", _variant_t(TTL));
			m_pRecordset->PutCollect("协议类型", _variant_t(proto));
			m_pRecordset->PutCollect("源IP地址", _variant_t(SIP));
			m_pRecordset->PutCollect("目的IP地址", _variant_t(DIP));
			m_pRecordset->PutCollect("源端口", _variant_t(Sport));
			m_pRecordset->PutCollect("目的端口", _variant_t(Dport));

			if (Type == 6)
				m_pRecordset->PutCollect("长度(TCP/UDP)", _variant_t(tcp_hLength));
			else if (Type == 17)
				m_pRecordset->PutCollect("长度(TCP/UDP)", _variant_t(udplength));
			strcpy(temp, date);
			m_pRecordset->PutCollect("数据1", _variant_t(temp));
			if (strlen(date) > 255)
			{
				for (int i = 256; i <= 510; i++)
					temp[i - 256] = date[i];
				m_pRecordset->PutCollect("数据2", _variant_t(temp));
			}
			m_pRecordset->Update();
		}
		catch (_com_error *e)
		{
			AfxMessageBox(e->ErrorMessage());
		}
		return;
	}
	if (Type == 6 && m_bTCP)
	{
		m_iTCPNum++;
		pktNum.Format("捕获TCP包%d个", m_iTCPNum);
		SetDlgItemText(IDC_STATIC_TCP, pktNum);
		m_pRecordset.CreateInstance(__uuidof(Recordset));
		m_pRecordset->Open("SELECT * FROM TCP", m_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
		try
		{
			m_pRecordset->AddNew();
			m_pRecordset->PutCollect("时间", _variant_t(timestr));
			m_pRecordset->PutCollect("IP首部长度", _variant_t(IPLength));
			m_pRecordset->PutCollect("IP数据包长度", _variant_t(IPDateLength));
			m_pRecordset->PutCollect("生存时间", _variant_t(TTL));
			m_pRecordset->PutCollect("协议类型", _variant_t(proto));
			m_pRecordset->PutCollect("源IP地址", _variant_t(SIP));
			m_pRecordset->PutCollect("目的IP地址", _variant_t(DIP));
			m_pRecordset->PutCollect("源端口", _variant_t(Sport));
			m_pRecordset->PutCollect("目的端口", _variant_t(Dport));
			m_pRecordset->PutCollect("TCP首部长度", _variant_t(tcp_hLength));
			strcpy(temp, date);
			m_pRecordset->PutCollect("TCP数据1", _variant_t(temp));
			if (strlen(date) > 255)
			{
				for (int i = 256; i <= 510; i++)
					temp[i - 256] = date[i];
				m_pRecordset->PutCollect("TCP数据2", _variant_t(temp));
			}
			m_pRecordset->Update();
		}
		catch (_com_error *e)
		{
			AfxMessageBox(e->ErrorMessage());
		}
		return;
	}
	if (Type == 17 && m_bUDP)
	{
		m_iUDPNum++;
		pktNum.Format("捕获UDP包%d个", m_iUDPNum);
		SetDlgItemText(IDC_STATIC_UDP, pktNum);
		m_pRecordset.CreateInstance(__uuidof(Recordset));
		m_pRecordset->Open("SELECT * FROM UDP", m_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
		try
		{
			m_pRecordset->AddNew();
			m_pRecordset->PutCollect("时间", _variant_t(timestr));
			m_pRecordset->PutCollect("IP首部长度", _variant_t(IPLength));
			m_pRecordset->PutCollect("IP数据包长度", _variant_t(IPDateLength));
			m_pRecordset->PutCollect("生存时间", _variant_t(TTL));
			m_pRecordset->PutCollect("协议类型", _variant_t(proto));
			m_pRecordset->PutCollect("源IP地址", _variant_t(SIP));
			m_pRecordset->PutCollect("目的IP地址", _variant_t(DIP));
			m_pRecordset->PutCollect("源端口", _variant_t(Sport));
			m_pRecordset->PutCollect("目的端口", _variant_t(Dport));
			m_pRecordset->PutCollect("UDP包长度", _variant_t(udplength));
			strcpy(temp, date);
			m_pRecordset->PutCollect("UDP数据1", _variant_t(temp));
			if (strlen(date) > 255)
			{
				for (int i = 256; i <= 510; i++)
					temp[i - 256] = date[i];
				m_pRecordset->PutCollect("UDP数据2", _variant_t(temp));
			}
			m_pRecordset->Update();
		}
		catch (_com_error *e)
		{
			AfxMessageBox(e->ErrorMessage());
		}
		return;
	}
}

void CLjDlg::OnButtonExit() //电击退出按纽	
{
	// TODO: Add your control notification handler code here
	EndDialog(1);//结束对话框
}

void CLjDlg::SaveDate()
{
	// TODO: Add your control notification handler code here

}