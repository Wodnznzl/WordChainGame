
// ServerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"
#include "ChildSocket.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CServerDlg 대화 상자



CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_ctrlEdit);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CServerDlg 메시지 처리기

BOOL CServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	//db 연결
	mysql_init(&m_mysql);
	MYSQL *conn = mysql_real_connect(&m_mysql, "localhost", "user1", "pass01", "db1", 3306, (char*)NULL, 0);
	if (conn == NULL) {
		AfxMessageBox((LPCTSTR)mysql_error(&m_mysql));
		::PostQuitMessage(0); 
		return FALSE;
	}
	this->m_ctrlEdit.ReplaceSel(_T("MySQL 서버와 연결되엇습니다\r\n"));
	mysql_set_character_set(&m_mysql, "euckr");


	//서버 실행
	m_pListenSocket = new CListenSocket;
	if (m_pListenSocket->Create(7000, SOCK_STREAM)) {
		if (m_pListenSocket->Listen()) {
			m_ctrlEdit.ReplaceSel(_T("[서버] Port(7000)이 Listen 소켓으로 열렸습니다.\r\n"));
		}
		else AfxMessageBox(_T("ERROR: Failed to create a listen socket"));
	}


	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



BOOL CServerDlg::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	//db
	mysql_close(&m_mysql);

	//서버
	POSITION pos = m_pListenSocket->m_pChildSocketList.GetHeadPosition();
	CChildSocket* pChild = NULL;
	while (pos != NULL) {
		pChild = (CChildSocket*)(m_pListenSocket->m_pChildSocketList.GetNext(pos));
		if (pChild != NULL) {
			pChild->ShutDown(); pChild->Close(); delete pChild;
		}
	}
	m_pListenSocket->ShutDown();
	m_pListenSocket->Close();
	delete m_pListenSocket;
	return CDialogEx::DestroyWindow();
}



void CServerDlg::SignUp(CString id, CString password, UINT port)
{
	//회원가입
	CString query;
	query.Format(_T("insert into member (id,pw) values ('%s',password('%s'))"), id, password);
	int status = mysql_query(&m_mysql, query);
	int UserNo = mysql_insert_id(&m_mysql);
	CString str;
	str.Format(_T("0 %s \r\n"), id);
	CServerDlg* pMain = (CServerDlg*)AfxGetMainWnd();
	pMain->m_ctrlEdit.ReplaceSel(str);
	//map 연결
	m_usermap.insert(pair<UINT, CString>(port, id));
	m_ready.insert(pair<CString, int>(id, 0));

	m_pListenSocket->Broadcast(str);
}


void CServerDlg::Login(CString id, CString password, UINT port)
{
	//로그인
	CString query;
	query.Format(_T("select * from member where id='%s' and pw=password('%s')"), id, password);
	int status = mysql_query(&m_mysql, query);
	MYSQL_RES *result = mysql_store_result(&m_mysql);
	int nRowCount = mysql_num_rows(result);
	if (nRowCount > 0) {
		MYSQL_ROW row = mysql_fetch_row(result);
		int UserNo = atoi(row[0]);
		CString str;
		str.Format(_T("1 %s \r\n"), id);
		CServerDlg* pMain = (CServerDlg*)AfxGetMainWnd();
		pMain->m_ctrlEdit.ReplaceSel(str);
		//map 연결
		m_usermap.insert(pair<UINT, CString>(port, id));
		m_ready.insert(pair<CString, int>(id, 0));

		m_pListenSocket->Broadcast(str);
	}
	else {
		AfxMessageBox("실패");
	}

}


void CServerDlg::Ready(int isready, CString username, CString msg)
{
	if (isready == 1) {
		//준비
		m_pListenSocket->Broadcast(msg);
		m_ready[username] = 1;
	}
	else {
		//준비 해제
		m_pListenSocket->Broadcast(msg);
		m_ready[username] = 0;
	}

	//준비 확인
	int result = 1;
	for (auto it = m_ready.begin(); it != m_ready.end(); it++) {

		if (it->second == 0) result = 0;
	}
	if (result == 1 && m_ready.size()==2) {
		//모두 준비 및 2명 로그인-> 게임 시작
		CString query;
		query.Append(_T("5 \r\n"));
		Sleep(1000);	//딜레이가 없으면 Client 단에서 인게임 다이얼로그가 생성되지 않는다!!
		m_pListenSocket->Broadcast(query);

		CServerDlg* pMain = (CServerDlg*)AfxGetMainWnd();
		pMain->m_ctrlEdit.ReplaceSel(query);
	}
	
}
