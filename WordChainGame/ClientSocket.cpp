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
	m_turn = _T("");
	m_ID = _T("");
	m_MyScore = _T("");
	m_OtherScore = _T("");
	m_itime = 0;
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
				m_ID = pMain->m_strID;
				pMain->m_strID = _T("");
				pMain->m_strPASSWORD = _T("");
				pMain->UpdateData(FALSE);
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
				m_ID = pMain->m_strID;
				pMain->m_strID = _T("");
				pMain->m_strPASSWORD = _T("");
				pMain->UpdateData(FALSE);
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
			CString name, score;
			CString msg;
			for (int i = 0; i < 10; i+=2) {
				AfxExtractSubString(name, szBuff, i+1, ' ');
				AfxExtractSubString(score, szBuff, i+2, ' ');
				msg.Format(_T("%s\t\t%s \r\n"), name, score);
				pMain->m_ctrlLeaderBoard.ReplaceSel(msg);
			}

		}
		else if (szBuff[0] == '5') {	//���� or �����
			CString turn, time;
			CString name1, name2;
			
			AfxExtractSubString(turn, szBuff, 1, ' ');
			AfxExtractSubString(time, szBuff, 2, ' ');
			AfxExtractSubString(name1, szBuff, 3, ' ');
			AfxExtractSubString(name2, szBuff, 5, ' ');

			m_itime = _ttoi(time);
			pMain->m_cnt = m_itime / 1000;

			if (name1 == m_ID) {
				AfxExtractSubString(m_MyScore, szBuff, 4, ' ');
				pMain->SetDlgItemText(IDC_STATIC15, m_MyScore);
				AfxExtractSubString(m_OtherScore, szBuff, 6, ' ');
				pMain->SetDlgItemText(IDC_STATIC14, m_OtherScore);
			}
			else {
				AfxExtractSubString(m_MyScore, szBuff, 6, ' ');
				pMain->SetDlgItemText(IDC_STATIC15, m_MyScore);
				AfxExtractSubString(m_OtherScore, szBuff, 4, ' ');
				pMain->SetDlgItemText(IDC_STATIC14, m_OtherScore);

			}

			m_turn = turn;
			CString text;

			if (m_turn == m_ID) {	//����
				text.Format(_T("���� �Դϴ�!!"));
				pMain->SetDlgItemText(IDC_STATIC18, text);
				pMain->KillTimer(2);

				pMain->SetTimer(1, 1000, NULL);	//1�ʸ���

			}
			else {	//�����
				text.Format(_T("����� �Դϴ�!!"));
				pMain->SetDlgItemText(IDC_STATIC18, text);
				pMain->KillTimer(1);

				pMain->SetTimer(2, 1000, NULL);	//1�ʸ���

			}
		}
		else if (szBuff[0] == '6') {	//�ܾ� ����
			CString word, correct, msg;
			AfxExtractSubString(word, szBuff, 1, ' ');
			AfxExtractSubString(correct, szBuff, 2, ' ');

			if (m_turn == m_ID) {	//����
				if (correct == '1') {	//�ùٸ� �ܾ�
					msg.Format(_T("%s \r\n"), word);

				}
				else {	//Ʋ�� �ܾ�

				}
			}
			else {	//�����

			}
		}
		else if (szBuff[0] == '7') {	//���� Ȯ��
			CString mypoint, otherpoint;
			AfxExtractSubString(mypoint, szBuff, 1, ' ');
			AfxExtractSubString(otherpoint, szBuff, 2, ' ');
		}

		else {	//���� ����


			//â Ȱ��ȭ
			pMain->GetDlgItem(IDC_EDIT7)->EnableWindow(TRUE);
			pMain->GetDlgItem(IDC_EDIT8)->EnableWindow(TRUE);
			pMain->GetDlgItem(IDC_EDIT9)->EnableWindow(TRUE);
			pMain->GetDlgItem(IDOK)->EnableWindow(TRUE);

			//�غ� ��Ȱ��ȭ
			pMain->SetDlgItemText(IDC_BUTTON4, _T("�غ�"));
			pMain->GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);

			
		}
	}
	CSocket::OnReceive(nErrorCode);
}
