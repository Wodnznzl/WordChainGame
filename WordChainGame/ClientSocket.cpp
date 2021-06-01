// ClientSocket.cpp : 구현 파일입니다.
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


// CClientSocket 멤버 함수


void CClientSocket::OnClose(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	this->ShutDown();
	this->Close();
	AfxMessageBox(_T("서버가 연결을 종료하엿습니다"));
	::PostQuitMessage(0);
	CSocket::OnClose(nErrorCode);
}


void CClientSocket::OnReceive(int nErrorCode)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
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

		if (szBuff[0] == '0') {	//회원가입
			CString name;
			CString msg;
			AfxExtractSubString(name, szBuff, 1, ' ');
			msg.Format(_T("%s 님이 회원가입 하셨습니다\r\n"), name);
			pMain->m_ctrlEdit.ReplaceSel(msg);

			if (pMain->m_strID == name) {	//내가 회원가입
				m_ID = pMain->m_strID;
				pMain->m_strID = _T("");
				pMain->m_strPASSWORD = _T("");
				pMain->UpdateData(FALSE);
				CString text;
				text.Format(_T("%s 님 안녕하세요!!"), name);
				pMain->SetDlgItemText(IDC_STATIC11, text);
				//비활성화 및 활성화
				pMain->GetDlgItem(IDC_EDIT4)->EnableWindow(FALSE);
				pMain->GetDlgItem(IDC_EDIT5)->EnableWindow(FALSE);
				pMain->GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
				pMain->GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
				pMain->GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE);
			}
		}
		else if (szBuff[0] == '1') {	//로그인
			CString name;
			CString msg;
			AfxExtractSubString(name, szBuff, 1, ' ');
			msg.Format(_T("%s 님이 로그인 하셨습니다\r\n"), name);
			pMain->m_ctrlEdit.ReplaceSel(msg);

			if (pMain->m_strID == name) {	//내가 로그인
				m_ID = pMain->m_strID;
				pMain->m_strID = _T("");
				pMain->m_strPASSWORD = _T("");
				pMain->UpdateData(FALSE);
				CString text;
				text.Format(_T("%s 님 안녕하세요!!"), name);
				pMain->SetDlgItemText(IDC_STATIC11, text);
				//비활성화 및 활성화
				pMain->GetDlgItem(IDC_EDIT4)->EnableWindow(FALSE);
				pMain->GetDlgItem(IDC_EDIT5)->EnableWindow(FALSE);
				pMain->GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
				pMain->GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
				pMain->GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE);

			}
		}
		else if(szBuff[0] == '2'){	//준비 관련 메시지
			CString name;
			CString ready;
			CString msg;
			AfxExtractSubString(name, szBuff, 1, ' ');
			AfxExtractSubString(ready, szBuff, 2, ' ');
			if (ready == 'y') {
				msg.Format(_T("%s 님이 준비 하였습니다 \r\n"), name);
			}
			else {
				msg.Format(_T("%s 님이 준비취소 하였습니다 \r\n"), name);
			}
			pMain->m_ctrlEdit.ReplaceSel(msg);
		}
		else if (szBuff[0] == '3') {	//서버 연결 결과
			CString str1;
			CString str2;
			CString str3;
			CString msg;
			AfxExtractSubString(str1, szBuff, 1, ' ');
			AfxExtractSubString(str2, szBuff, 2, ' ');
			AfxExtractSubString(str3, szBuff, 3, ' ');
			msg.Format(_T("%s %s %s \r\n"), str1, str2, str3);
			pMain->m_ctrlEdit.ReplaceSel(msg);
			//EditControl 비활성 및 활성화
			pMain->GetDlgItem(IDC_EDIT1)->EnableWindow(FALSE);
			pMain->GetDlgItem(IDC_EDIT2)->EnableWindow(FALSE);
			pMain->GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
			pMain->GetDlgItem(IDC_EDIT4)->EnableWindow(TRUE);
			pMain->GetDlgItem(IDC_EDIT5)->EnableWindow(TRUE);
			pMain->GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
			pMain->GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);

		}
		else if (szBuff[0] == '4') {	//LeaderBoard 결과
			CString name, score;
			CString msg;
			for (int i = 0; i < 10; i+=2) {
				AfxExtractSubString(name, szBuff, i+1, ' ');
				AfxExtractSubString(score, szBuff, i+2, ' ');
				msg.Format(_T("%s\t\t%s \r\n"), name, score);
				pMain->m_ctrlLeaderBoard.ReplaceSel(msg);
			}

		}
		else if (szBuff[0] == '5') {	//내턴 or 상대턴
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

			if (m_turn == m_ID) {	//내턴
				text.Format(_T("내턴 입니다!!"));
				pMain->SetDlgItemText(IDC_STATIC18, text);
				pMain->KillTimer(2);

				pMain->SetTimer(1, 1000, NULL);	//1초마다

			}
			else {	//상대턴
				text.Format(_T("상대턴 입니다!!"));
				pMain->SetDlgItemText(IDC_STATIC18, text);
				pMain->KillTimer(1);

				pMain->SetTimer(2, 1000, NULL);	//1초마다

			}
		}
		else if (szBuff[0] == '6') {	//단어 받음
			CString word, correct, msg;
			AfxExtractSubString(word, szBuff, 1, ' ');
			AfxExtractSubString(correct, szBuff, 2, ' ');

			if (m_turn == m_ID) {	//내턴
				if (correct == '1') {	//올바른 단어
					msg.Format(_T("%s \r\n"), word);

				}
				else {	//틀린 단어

				}
			}
			else {	//상대턴

			}
		}
		else if (szBuff[0] == '7') {	//생명 확인
			CString mypoint, otherpoint;
			AfxExtractSubString(mypoint, szBuff, 1, ' ');
			AfxExtractSubString(otherpoint, szBuff, 2, ' ');
		}

		else {	//게임 시작


			//창 활성화
			pMain->GetDlgItem(IDC_EDIT7)->EnableWindow(TRUE);
			pMain->GetDlgItem(IDC_EDIT8)->EnableWindow(TRUE);
			pMain->GetDlgItem(IDC_EDIT9)->EnableWindow(TRUE);
			pMain->GetDlgItem(IDOK)->EnableWindow(TRUE);

			//준비 비활성화
			pMain->SetDlgItemText(IDC_BUTTON4, _T("준비"));
			pMain->GetDlgItem(IDC_BUTTON4)->EnableWindow(FALSE);

			
		}
	}
	CSocket::OnReceive(nErrorCode);
}
