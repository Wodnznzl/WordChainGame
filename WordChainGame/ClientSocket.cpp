// ClientSocket.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "WordChainGame.h"
#include "ClientSocket.h"
#include "WordChainGameDlg.h"
#include "InGameDlg.h"

// CClientSocket

CClientSocket::CClientSocket()
{
}

CClientSocket::~CClientSocket()
{
}


// CClientSocket ��� �Լ�


void CClientSocket::OnClose(int nErrorCode)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	this->ShutDown();
	this->Close();
	AfxMessageBox(_T("������ ������ �����Ͽ����ϴ�"));
	::PostQuitMessage(0);
	CSocket::OnClose(nErrorCode);
}


void CClientSocket::OnReceive(int nErrorCode)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	TCHAR szBuff[1024];
	int nRead = Receive(szBuff, sizeof(szBuff));
	switch (nRead) {
	case 0: this->Close(); break;
	case SOCKET_ERROR: if (GetLastError() != WSAEWOULDBLOCK) {
		AfxMessageBox(_T("Error occured"));
		this->Close();
	}break;
	default: 
		szBuff[nRead] = _T('\0');
		CWordChainGameDlg* pMain = (CWordChainGameDlg*)AfxGetMainWnd();

		if (szBuff[0] == '0') {	//ȸ������
			CString name;
			CString msg;
			AfxExtractSubString(name, szBuff, 1, ' ');
			msg.Format(_T("%s ���� ȸ������ �ϼ̽��ϴ�\r\n"), name);
			pMain->m_ctrlEdit.ReplaceSel(msg);

			if (pMain->m_strID == name) {	//���� ȸ������
				pMain->m_strID = _T("");
				pMain->m_strPASSWORD = _T("");
				CString text;
				text.Format(_T("%s �� �ȳ��ϼ���!!"), name);
				pMain->SetDlgItemText(IDC_STATIC11, text);
				//��Ȱ��ȭ �� Ȱ��ȭ
				pMain->GetDlgItem(IDC_EDIT4)->EnableWindow(FALSE);
				pMain->GetDlgItem(IDC_EDIT5)->EnableWindow(FALSE);
				pMain->GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
				pMain->GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
				pMain->GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE);
			}
		}
		else if (szBuff[0] == '1') {	//�α���
			CString name;
			CString msg;
			AfxExtractSubString(name, szBuff, 1, ' ');
			msg.Format(_T("%s ���� �α��� �ϼ̽��ϴ�\r\n"), name);
			pMain->m_ctrlEdit.ReplaceSel(msg);

			if (pMain->m_strID == name) {	//���� �α���
				pMain->m_strID = _T("");
				pMain->m_strPASSWORD = _T("");
				CString text;
				text.Format(_T("%s �� �ȳ��ϼ���!!"), name);
				pMain->SetDlgItemText(IDC_STATIC11, text);
				//��Ȱ��ȭ �� Ȱ��ȭ
				pMain->GetDlgItem(IDC_EDIT4)->EnableWindow(FALSE);
				pMain->GetDlgItem(IDC_EDIT5)->EnableWindow(FALSE);
				pMain->GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
				pMain->GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
				pMain->GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE);

			}
		}
		else if(szBuff[0] == '2'){	//�غ� ���� �޽���
			CString name;
			CString ready;
			CString msg;
			AfxExtractSubString(name, szBuff, 1, ' ');
			AfxExtractSubString(ready, szBuff, 2, ' ');
			if (ready == 'y') {
				msg.Format(_T("%s ���� �غ� �Ͽ����ϴ� \r\n"), name);
			}
			else {
				msg.Format(_T("%s ���� �غ���� �Ͽ����ϴ� \r\n"), name);
			}
			pMain->m_ctrlEdit.ReplaceSel(msg);
		}
		else if (szBuff[0] == '3') {	//���� ���� ���
			CString str1;
			CString str2;
			CString str3;
			CString msg;
			AfxExtractSubString(str1, szBuff, 1, ' ');
			AfxExtractSubString(str2, szBuff, 2, ' ');
			AfxExtractSubString(str3, szBuff, 3, ' ');
			msg.Format(_T("%s %s %s \r\n"), str1, str2, str3);
			pMain->m_ctrlEdit.ReplaceSel(msg);
			//EditControl ��Ȱ�� �� Ȱ��ȭ
			pMain->GetDlgItem(IDC_EDIT1)->EnableWindow(FALSE);
			pMain->GetDlgItem(IDC_EDIT2)->EnableWindow(FALSE);
			pMain->GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
			pMain->GetDlgItem(IDC_EDIT4)->EnableWindow(TRUE);
			pMain->GetDlgItem(IDC_EDIT5)->EnableWindow(TRUE);
			pMain->GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
			pMain->GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);

		}
		else if (szBuff[0] == '4') {	//LeaderBoard ���
			CString name, point;
			CString msg;
			for (int i = 0; i < 10; i+=2) {
				AfxExtractSubString(name, szBuff, i+1, ' ');
				AfxExtractSubString(point, szBuff, i+2, ' ');
				msg.Format(_T("%s\t\t%s \r\n"), name, point);
				pMain->m_ctrlLeaderBoard.ReplaceSel(msg);
			}

		}
		else {	//���� ����

			CInGameDlg game;
			game.DoModal();

			game.DestroyWindow();

		}
	}
	CSocket::OnReceive(nErrorCode);
}
