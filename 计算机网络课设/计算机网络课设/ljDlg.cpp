#include "stdafx.h"
#include "lj.h"
#include "ljDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
//���ӵ�ͷ�ļ�
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
	//��������Ի���ĳ�ʼ��
	m_handle = NULL;//�������
	m_iOneNum = 0;
	m_iTCPNum = 0;
	m_iUDPNum = 0;

	pcap_if_t * alldevs;
	pcap_if_t * d;
	char errbuf[PCAP_ERRBUF_SIZE];//���������Ϣ

	//���豸
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
	{
		//        err.Format("Error in pcap_findalldevs: %s", errbuf);
		//		MessageBox(NULL,err,"����",MB_OK);
		return FALSE;
	}

	//���������ؼ��ĳ�ʼ����ʾ
	for (d = alldevs; d != NULL; d = d->next)
	{
		//��OSes��֧����������"description"�ֶΣ���������Ĵ���
		if (d->description)
			m_wangka.AddString(d->description);
		else
		{
			CString temp;
			temp.Format("%s(No description)", d->name);
			m_wangka.AddString(temp);
		}
	}

	pcap_freealldevs(alldevs);	//�ͷ��豸�б���Դ

	//����������ʼ��ѡ��
	m_wangka.SetCurSel(0);

	//�ؼ�������
	((CButton *)GetDlgItem(IDC_RADIO_TCP))->SetCheck(1);
	m_bTCP = TRUE;
	m_bUDP = FALSE;
	m_bOneIP = FALSE;

	GetDlgItem(IDC_IPADDR)->EnableWindow(FALSE);

	/*�Խ���IP��ַ�ؼ��Ͷ˿ڿؼ���ʼ��*/

	BYTE IP[4]; //����װ32λIP��ַ
	struct hostent * shHostent; //������Ϣ�ṹ��
	char szHostName[128]; //������

	gethostname(szHostName, 128); // ���������

	/*if�������������������Ϣ�ṹ��,�Ӷ��Ի��32λIP��ַ*/
	if ((shHostent = gethostbyname(szHostName)) != NULL) //����������������Ϣ
	{
		//�õ�������Ϣ���IP��ַ��ֵ
		IP[0] = ((struct in_addr *)shHostent->h_addr_list[0])->S_un.S_un_b.s_b1;
		IP[1] = ((struct in_addr *)shHostent->h_addr_list[0])->S_un.S_un_b.s_b2;
		IP[2] = ((struct in_addr *)shHostent->h_addr_list[0])->S_un.S_un_b.s_b3;
		IP[3] = ((struct in_addr *)shHostent->h_addr_list[0])->S_un.S_un_b.s_b4;
	}
	else
		//��������Ϣʧ�ܷ���false
		return FALSE;

	//��IP��ַ�ؼ���ֵ
	((CIPAddressCtrl *)GetDlgItem(IDC_IPADDR))->SetAddress(IP[0], IP[1], IP[2], IP[3]);
	((CIPAddressCtrl *)GetDlgItem(IDC_IPADDR))->EnableWindow(false);//����IP��ַ�ؼ�

	////////////�������ݿ�//////////////
	CoInitialize(NULL);
	m_pConnection.CreateInstance(__uuidof(Connection));

	try
	{
		m_pConnection->Open("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=lj.mdb", "", "", adModeUnknown);///�������ݿ�
		///����һ���������ִ��е�Provider�����ACCESS2000�����ģ�����ACCESS97,��Ҫ��Ϊ:Provider=Microsoft.Jet.OLEDB.3.51;  }
	}
	catch (_com_error e)///��׽�쳣
	{
		AfxMessageBox("���ݿ�����ʧ�ܣ�ȷ�����ݿ�lj.mdb�Ƿ��ڵ�ǰ·����!");
		return FALSE;
	}

	//////////////////////////////////////////////////////////////////////////

	return TRUE;  // return TRUE  unless you set the focus to a control
}


//���TCP��ѡ��Ĳ���
void CLjDlg::OnRadioTcp()
{
	// TODO: Add your control notification handler code here
	m_bTCP = TRUE;
	m_bUDP = FALSE;
	m_bOneIP = FALSE;
	GetDlgItem(IDC_IPADDR)->EnableWindow(FALSE);
}
//���UDP��ѡ��Ĳ���
void CLjDlg::OnRadioUdp()
{
	// TODO: Add your control notification handler code here
	m_bTCP = FALSE;
	m_bUDP = TRUE;
	m_bOneIP = FALSE;
	GetDlgItem(IDC_IPADDR)->EnableWindow(FALSE);
}
//���IP��ѡ��Ĳ���
void CLjDlg::OnRadioIp()
{
	// TODO: Add your control notification handler code here
	m_bTCP = FALSE;
	m_bUDP = FALSE;
	m_bOneIP = TRUE;
	GetDlgItem(IDC_IPADDR)->EnableWindow(TRUE);
}

//�̴߳�����
DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	CLjDlg * m_dlg = (CLjDlg *)lpParameter;

	struct pcap_pkthdr *header;	//�������ݰ���ͷ
	const u_char *pkt_data; //ָ������ָ��
	int res; //pcap_next_ex���������ݰ��������ķ���ֵ

	m_dlg->SetDlgItemText(IDC_STATIC_TIP, "�߳̿�ʼ��������.");
	while ((res = pcap_next_ex(m_dlg->adhandle, &header, &pkt_data)) >= 0)
	{
		if (res == 0)//��ʱ
			continue;
		//����������ݰ�
		m_dlg->SaveDate(header, pkt_data);
	}
	return 1;
}

//�������ʼ����
void CLjDlg::OnButtonStart()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);//�ѽ����ϵ�ֵ������̨
	pcap_if_t * alldevs;
	pcap_if_t * d;
	CString temp;
	char errbuf[PCAP_ERRBUF_SIZE];

	//������
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL, &alldevs, errbuf) == -1)
	{
		MessageBox("�����豸����", "��ʾ");
		return;
	}
	//�õ�Ҫ�����������
	m_wangka.GetLBText(m_wangka.GetCurSel(), temp);

	//ת��ָ������
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
	sprintf(m_oneIP, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);//���û������IP��ַ�����̨
	//���豸
	adhandle = pcap_open(d->name, 65536, PCAP_OPENFLAG_PROMISCUOUS, 1000, NULL, errbuf);
	//����1 �����ҵ����������� ����2�����������ֽ���������3�Ƿ��������Ϊ����ģʽ������4����ʱ�����ƣ���λ���롣�����Ǵ�����Ϣ����
	//���ﴴ��һ���߳�����������
	LPDWORD ThID = NULL;
	m_handle = CreateThread(NULL, 0, ThreadProc, this, 0, ThID);//�Ѵ����̵߳ľ����ֵ���������
														 //����3Ϊ�̴߳�����������4�Ǵ������Ĳ���
														//����6���߳�ID
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
		SetDlgItemText(IDC_STATIC_TIP, "�߳�û������.");//���������������ʾ��ǰ��Ŀռ���
			  //���ô��ڿؼ��ı�                                         
		return;
	}
	if (TerminateThread(m_handle, -1) == 0)//�ر��߳�
	{
		return;
	}
	m_handle = NULL;//�ͷ��߳������������Ϣ
	SetDlgItemText(IDC_STATIC_TIP, "�����߳��Ѿ�ֹͣ.");
	m_pRecordset->Close();//���ݿ��¼���ر�
}

void CLjDlg::SaveDate(pcap_pkthdr *header, const u_char *pkt_data)
{
	//���û�������򷵻�
	if (header == NULL || pkt_data == NULL)
		return;

	//���ݰ������ͱ���
	ipheader * ih;
	tcp* tcph;
	udp* udph;

	CString temp;
	int chang;

	//ʱ��
	struct tm *ltime;
	ltime = localtime(&header->ts.tv_sec);//�Ѳ�����ʱ��ͨ��localtime��������lite����
	strftime(timestr, sizeof(timestr), "%H:%M:%S", ltime);//��ltime���м������ʽ����timestr

	//IP
	ih = (ipheader*)(pkt_data + 14);//ipͷ(��pkt_dataָ��+14���ڴ�λ��ǿ��ת����ipheader*��ʽ)
	int iplen = (ih->ver_ihl & 0xf) * 4;//��ȡ���ݰ�����
	itoa(iplen, IPLength, 10);//�������ַ���ת�� ��10����ת��

	sprintf(IPDateLength, "%d", header->len);//IP���ݰ�����

	sprintf(TTL, "%d", ih->ttl);//����ʱ��

	sprintf(SIP, "%d.%d.%d.%d", ih->saddr.byte1, ih->saddr.byte2, ih->saddr.byte3, ih->saddr.byte4);
	sprintf(DIP, "%d.%d.%d.%d", ih->daddr.byte1, ih->daddr.byte2, ih->daddr.byte3, ih->daddr.byte4);

	u_short u_sport, u_dport;
	if (ih->proto == 6) //������TCP���ݰ�
	{
		strcpy(proto, "TCP"); //Э������

		tcph = (tcp*)((u_char*)ih + iplen);

		u_sport = ntohs(tcph->sport); //Դ�˿�
		u_dport = ntohs(tcph->dport); //Ŀ�Ķ˿�
//		sprintf(Sport,"%d",tcph->sport); //Դ�˿�
//		sprintf(Dport,"%d",tcph->dport); //Ŀ�Ķ˿�
		chang = (tcph->other ^ 0x0fff) / 1024;
		sprintf(tcp_hLength, "%d", chang); //TCP�ײ�����
		sprintf(date, "%X", tcph->content); //TCP����
	}
	else if (ih->proto == 17) //������UDP���ݿ�
	{
		strcpy(proto, "UDP");
		udph = (udp*)((u_char*)ih + iplen);

		u_sport = ntohs(udph->sport); //Դ�˿�
		u_dport = ntohs(udph->dport); //Ŀ�Ķ˿�
//		sprintf(Sport,"%d",udph->sport); //Դ�˿�
//		sprintf(Dport,"%d",udph->dport); //Ŀ�Ķ˿�
		sprintf(udplength, "%d", udph->len); //TCP�ײ�����
		sprintf(date, "%X", udph->content); //TCP����
	}
	sprintf(Sport, "%d", u_sport); //Դ�˿�
	sprintf(Dport, "%d", u_dport); //Ŀ�Ķ˿�

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
		pktNum.Format("OneIP��ʽ��%d��", m_iOneNum);//�ַ����ຯ�� ��������ʾ
		SetDlgItemText(IDC_STATIC_ONEIP, pktNum);

		m_pRecordset.CreateInstance(__uuidof(Recordset));//������¼��
		m_pRecordset->Open("SELECT * FROM Ŀ��IP", m_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
		try
		{
			m_pRecordset->AddNew();//����¼�¼
			m_pRecordset->PutCollect("ʱ��", _variant_t(timestr));//��������ֵ
			m_pRecordset->PutCollect("IP�ײ�����", _variant_t(IPLength));
			m_pRecordset->PutCollect("IP���ݰ�����", _variant_t(IPDateLength));
			m_pRecordset->PutCollect("����ʱ��", _variant_t(TTL));
			m_pRecordset->PutCollect("Э������", _variant_t(proto));
			m_pRecordset->PutCollect("ԴIP��ַ", _variant_t(SIP));
			m_pRecordset->PutCollect("Ŀ��IP��ַ", _variant_t(DIP));
			m_pRecordset->PutCollect("Դ�˿�", _variant_t(Sport));
			m_pRecordset->PutCollect("Ŀ�Ķ˿�", _variant_t(Dport));

			if (Type == 6)
				m_pRecordset->PutCollect("����(TCP/UDP)", _variant_t(tcp_hLength));
			else if (Type == 17)
				m_pRecordset->PutCollect("����(TCP/UDP)", _variant_t(udplength));
			strcpy(temp, date);
			m_pRecordset->PutCollect("����1", _variant_t(temp));
			if (strlen(date) > 255)
			{
				for (int i = 256; i <= 510; i++)
					temp[i - 256] = date[i];
				m_pRecordset->PutCollect("����2", _variant_t(temp));
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
		pktNum.Format("����TCP��%d��", m_iTCPNum);
		SetDlgItemText(IDC_STATIC_TCP, pktNum);
		m_pRecordset.CreateInstance(__uuidof(Recordset));
		m_pRecordset->Open("SELECT * FROM TCP", m_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
		try
		{
			m_pRecordset->AddNew();
			m_pRecordset->PutCollect("ʱ��", _variant_t(timestr));
			m_pRecordset->PutCollect("IP�ײ�����", _variant_t(IPLength));
			m_pRecordset->PutCollect("IP���ݰ�����", _variant_t(IPDateLength));
			m_pRecordset->PutCollect("����ʱ��", _variant_t(TTL));
			m_pRecordset->PutCollect("Э������", _variant_t(proto));
			m_pRecordset->PutCollect("ԴIP��ַ", _variant_t(SIP));
			m_pRecordset->PutCollect("Ŀ��IP��ַ", _variant_t(DIP));
			m_pRecordset->PutCollect("Դ�˿�", _variant_t(Sport));
			m_pRecordset->PutCollect("Ŀ�Ķ˿�", _variant_t(Dport));
			m_pRecordset->PutCollect("TCP�ײ�����", _variant_t(tcp_hLength));
			strcpy(temp, date);
			m_pRecordset->PutCollect("TCP����1", _variant_t(temp));
			if (strlen(date) > 255)
			{
				for (int i = 256; i <= 510; i++)
					temp[i - 256] = date[i];
				m_pRecordset->PutCollect("TCP����2", _variant_t(temp));
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
		pktNum.Format("����UDP��%d��", m_iUDPNum);
		SetDlgItemText(IDC_STATIC_UDP, pktNum);
		m_pRecordset.CreateInstance(__uuidof(Recordset));
		m_pRecordset->Open("SELECT * FROM UDP", m_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
		try
		{
			m_pRecordset->AddNew();
			m_pRecordset->PutCollect("ʱ��", _variant_t(timestr));
			m_pRecordset->PutCollect("IP�ײ�����", _variant_t(IPLength));
			m_pRecordset->PutCollect("IP���ݰ�����", _variant_t(IPDateLength));
			m_pRecordset->PutCollect("����ʱ��", _variant_t(TTL));
			m_pRecordset->PutCollect("Э������", _variant_t(proto));
			m_pRecordset->PutCollect("ԴIP��ַ", _variant_t(SIP));
			m_pRecordset->PutCollect("Ŀ��IP��ַ", _variant_t(DIP));
			m_pRecordset->PutCollect("Դ�˿�", _variant_t(Sport));
			m_pRecordset->PutCollect("Ŀ�Ķ˿�", _variant_t(Dport));
			m_pRecordset->PutCollect("UDP������", _variant_t(udplength));
			strcpy(temp, date);
			m_pRecordset->PutCollect("UDP����1", _variant_t(temp));
			if (strlen(date) > 255)
			{
				for (int i = 256; i <= 510; i++)
					temp[i - 256] = date[i];
				m_pRecordset->PutCollect("UDP����2", _variant_t(temp));
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

void CLjDlg::OnButtonExit() //����˳���Ŧ	
{
	// TODO: Add your control notification handler code here
	EndDialog(1);//�����Ի���
}

void CLjDlg::SaveDate()
{
	// TODO: Add your control notification handler code here

}