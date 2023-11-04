
// tali_ExamDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "tali_Exam.h"
#include "tali_ExamDlg.h"
#include "afxdialogex.h"
#include <codecvt>
#include <string>
//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

//�����˻�
UINT ThreadInspectFolder(LPVOID lParam)
{
	stFolderInsParam *param = (stFolderInsParam *)lParam;
	Ctali_ExamDlg* pView = (Ctali_ExamDlg*)param->m_pView;

	pView->FolderInspect(param->m_strFolderPath);

	return 1;
}


class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
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


// Ctali_ExamDlg ��ȭ ����

// convert UTF-8 string to wstring
std::wstring utf8_to_wstring(const std::string& str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.from_bytes(str);
}



Ctali_ExamDlg::Ctali_ExamDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TALI_EXAM_DIALOG, pParent)
	, m_logger("main", "log/log.txt", true, 1024 * 512, 10)
	//            �ΰ��̸�           �αװ��     rotating    �α׿뷮    �ִ� ���� ��
	, m_dbSet(_INI_FILE_PATH, L"Setting")
	//              ���     �����̸�
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pChain = std::make_shared<CChain>(80, 100000);
}

void Ctali_ExamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_IMAGE, m_imgDisplay);
	DDX_Control(pDX, IDC_STATIC_IMAGE_W, m_imgDisplay_w);
	DDX_Control(pDX, IDC_STATIC_IMAGE_B, m_imgDisplay_b);
	DDX_Text(pDX, IDC_SEARCHED_DEFECT_CNT, m_searchDefectCnt);

	DDX_Control(pDX, IDC_MFCPROPERTYGRID1, m_property);



	DDX_Control(pDX, IDC_ABNORMAL_TAB, m_abnormalTab);
	DDX_Control(pDX, IDC_INK_MARKED, m_InkMarked);
}

BEGIN_MESSAGE_MAP(Ctali_ExamDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_LOAD, &Ctali_ExamDlg::OnBnClickedBtnLoad)
	ON_BN_CLICKED(IDC_BTN_GET_EDGE, &Ctali_ExamDlg::OnBnClickedBtnGetEdge)
	ON_STN_CLICKED(IDC_STATIC_IMAGE, &Ctali_ExamDlg::OnStnClickedStaticImage)
	ON_BN_CLICKED(IDC_BTN_GET_PATTERN, &Ctali_ExamDlg::OnBnClickedBtnGetPattern)
	ON_BN_CLICKED(IDC_BTN_GET_PATTERN2, &Ctali_ExamDlg::OnBnClickedBtnGetPattern2)
	ON_BN_CLICKED(IDC_BTN_SEARCH_PATTERN, &Ctali_ExamDlg::OnBnClickedBtnSearchPattern)
	ON_BN_CLICKED(IDC_BTN_SET_LINE_LENGTH, &Ctali_ExamDlg::OnBnClickedBtnSetLineLength)
	ON_BN_CLICKED(IDC_BTN_SET_THRESHOLD, &Ctali_ExamDlg::OnBnClickedBtnSetThreshold)
	ON_BN_CLICKED(IDC_BTN_SEARCH_COAT_EDGE, &Ctali_ExamDlg::OnBnClickedBtnSearchCoatEdge)
	ON_BN_CLICKED(IDC_BTN_SAVE_INSPECT_PARAM, &Ctali_ExamDlg::OnBnClickedBtnSaveInspectParam)
	ON_BN_CLICKED(IDC_BTN_FOLDER_INSPECT, &Ctali_ExamDlg::OnBnClickedBtnFolderInspect)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// Ctali_ExamDlg �޽��� ó����

BOOL Ctali_ExamDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
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

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	setLogger();
	setDisplay();
	SetTimer(TIMER_ID_MOUSE_INFO, 100, NULL);

	SetDlgItemText(IDC_EDIT_INPUT_RANGE, CString("100"));
	SetDlgItemText(IDC_EDIT_INPUT_THRESHOLD, CString("95"));

	InitProperty();
	CreateParam();
	SyncParamToProperty();
	SetInspectParam(&m_inspect_param, &_param);


	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void Ctali_ExamDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void Ctali_ExamDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR Ctali_ExamDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Ctali_ExamDlg::OnDestroy()
{
	if (m_arrTarget) {
		for (int i = 0; i < 2; i++) delete[] m_arrTarget[i];
		delete m_arrTarget;
	}

	CDialogEx::OnDestroy();
}

void Ctali_ExamDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialogEx::OnTimer(nIDEvent);
	if (nIDEvent == TIMER_ID_MOUSE_INFO) {
		/*int pixel;
		CPoint ptImg, ptMouse;
		string info;
		CString str;
		pixel = m_imgDisplay.gGetPixelInfo(ptMouse, ptImg);
		info = "x: " + to_string(ptImg.x) + ", y: " + to_string(ptImg.y) + ", pixel: " + to_string(pixel);
		str = info.c_str();
		GetDlgItem(IDC_STATIC_INFO)->SetWindowTextW(str);*/
	}
}

void Ctali_ExamDlg::setLogger()
{
	//m_logger.changeLevel(spdlog::level::trace);
	//m_logger.trace("complete to initialize Dialog");
	//m_logger.set_pattern("%E.%e\t%v");
	//m_logger.setConsoleLevel(G_LOGGER_LEVEL_TRACE);
	//m_logger.setFileLevel(G_LOGGER_LEVEL_TRACE);
	//m_logger.changeLevel(spdlog::level::trace);
	//m_logger.set_pattern("%E%e\t%v");
	m_logger.trace("Program Start");
}

void Ctali_ExamDlg::setDisplay()
{
	int nWidth = 15552;//15552; // 1024;
	int nHeight = 3072;//3072;//1200;
	int nBpp = 8;
	m_imgDisplay.gCreate(nWidth, nHeight, nBpp);
	m_imgDisplay.gSetUseRoi();

	m_imgDisplay_w.gCreate(nWidth, nHeight * 0.5, nBpp);
	m_imgDisplay_w.gSetUseRoi();

	m_imgDisplay_b.gCreate(nWidth, nHeight * 0.5, nBpp);
	m_imgDisplay_b.gSetUseRoi();
}


void Ctali_ExamDlg::OnBnClickedBtnLoad()
{
	CFileDialog fDlg(true);
	if (fDlg.DoModal() == IDOK) {
		m_imgDisplay.gLoad(fDlg.GetPathName());
	}

	DivideFullImage(m_imgDisplay.gGetImgPtr(),
		m_imgDisplay_w.gGetImgPtr(),
		m_imgDisplay_b.gGetImgPtr(),
		m_imgDisplay.gGetWidth(),
		m_imgDisplay.gGetHeight()
	);

	bool abnormalTab = false;
	bool inkMarked = false;
	m_searchDefectCnt = Inspect(m_inspect_param, &abnormalTab, &inkMarked);

	if (abnormalTab == true) {
		SetDlgItemTextW(IDC_ABNORMAL_TAB, TEXT("TRUE"));
	}
	else {
		SetDlgItemTextW(IDC_ABNORMAL_TAB, TEXT("FALSE"));
	}

	if (inkMarked == true) {
		SetDlgItemTextW(IDC_INK_MARKED, TEXT("TRUE"));
	}
	else {
		SetDlgItemTextW(IDC_INK_MARKED, TEXT("FALSE"));
	}

	CString strImgDst;
	bool bNG = false;

	if (abnormalTab == false && inkMarked == false && m_searchDefectCnt > 0) {//������
		//strImgDst.Format(TEXT("%s\\%s"), strDestPathNG, vt_file_list[nCnt].m_strFileName);
		bNG = true;
	}
	else if (abnormalTab == true || (abnormalTab == false && inkMarked == true)) {//�Ǻҷ�:skip or ������ and ��ũ��ŷ:skip
		//strImgDst.Format(TEXT("%s\\%s"), strDestPathOK, vt_file_list[nCnt].m_strFileName);
		bNG = false;
	}

	if (bNG == true) {
		SetDlgItemTextW(IDC_STATIC_JUDGE, TEXT("NG"));
	}
	else {
		SetDlgItemTextW(IDC_STATIC_JUDGE, TEXT("OK"));
	}

	
	

	UpdateData(false);

}

CRect Ctali_ExamDlg::funcTabRoi(std::shared_ptr<CChain> chain, unsigned char* pImg, int width, int height,
	int th, int* _cntW, int* _cntB)
{
	unsigned char* pBinary = new unsigned char[width * height];

	bool bWhite = true;
	m_imgProcess.Binarization(pImg, pBinary, width, height, th, bWhite);

	chain.get()->SetChainData(1, pBinary, 2, 2, 4, 100000, width, height);
	int blobCnt = chain.get()->FastChain(0, 0, width, height);

	int maxIdx = 0;
	int maxArea = 0;
	for (int defectIdx = 0; defectIdx < blobCnt; defectIdx++) {
		double area = chain.get()->Chain_Area(defectIdx);

		if (maxArea < area) {
			maxArea = area;
			maxIdx = defectIdx;
		}
	}
	CRect rt;
	rt.SetRect(
		chain.get()->FindMinX(maxIdx),
		chain.get()->FindMinY(maxIdx),
		chain.get()->FindMaxX(maxIdx),
		chain.get()->FindMaxY(maxIdx));

	CRect rtInflateRect;
	rtInflateRect = rt;
	rtInflateRect.InflateRect(-20, -20);
	//ROI������ white ī��Ʈ, black ī��Ʈ�� �̴´�
	int cntW = 0;
	int cntB = 0;

	for (int y = rtInflateRect.top; y < rtInflateRect.bottom; y++) {
		for (int x = rtInflateRect.left; x < rtInflateRect.right; x++) {
			unsigned char val = *(pBinary + y * width + x);

			if (val == 0xff) {
				cntW++;
			}
			else {
				cntB++;
			}
		}
	}

	(*_cntW) = cntW;
	(*_cntB) = cntB;

	delete[] pBinary;

	return rt;
}


void Ctali_ExamDlg::OnBnClickedBtnGetEdge()
{
	int nWidth = m_imgDisplay.gGetWidth();
	int nHeight = m_imgDisplay.gGetHeight();
	unsigned char* fm = m_imgDisplay.gGetImgPtr();
	int *arrX = new int[nWidth] {0, };
	int *arrY = new int[nHeight] {0,};
	for (int j = 0; j < nHeight; j++) {
		for (int i = 0; i < nWidth; i++) {
			if (fm[j*nWidth + i] > 200) {
				arrX[i] = 0;
			}
			else {
				arrX[i] += fm[j*nWidth + i];
			}
		}
	}
	for (int i = 0; i < nWidth - 1; i++) {
		arrX[i] = arrX[i + 1] - arrX[i];
	}
	int nMinX = 0;
	int nIdxX = 0;
	
	for (int i = 0; i < nWidth; i++) {
		if (nMinX > arrX[i]) {
			nMinX = arrX[i];
			nIdxX = i;
		}
	}

	int nSum = 0;
	int nAverage;
	for (int i = 0; i < nWidth; i++) {
		for (int j = 0; j < nHeight; j++) {
			arrY[j] += fm[j * nWidth + i];
			nSum += fm[j * nWidth + i];
		}
	}
	nAverage = nSum / nHeight;

	CPoint edgeStart(nIdxX, 0), edgeEnd(nIdxX, nHeight);
	m_imgDisplay.gDrawLine(edgeStart, edgeEnd);
	CRect rect(nIdxX -120, 0, nIdxX, nHeight);
	m_dbSet.SerGet(false, rect, L"Roi");
	m_dbSet.SerGet(false, nIdxX, L"Edge");
	m_logger.info("get edge = roi[left {}, top {}, right {}, bottom {}", rect.left, rect.top, rect.right, rect.bottom);
	delete[] arrY;
	delete[] arrX;
	m_imgDisplay.UpdateDisplay();
}

double getCorreationData(unsigned char* target, unsigned char* line, int nCount) {
	int sum_X = 0, sum_Y = 0, sum_XY = 0;
	int squareSum_X = 0, squareSum_Y = 0;
	for (int i = 0; i < nCount; i++) {
		sum_X += target[i];
		sum_Y += line[i];
		sum_XY += target[i] * line[i];
		squareSum_X = squareSum_X + target[i] * target[i];
		squareSum_Y = squareSum_Y + line[i] * line[i];
	}
	double a = double(nCount * squareSum_X - sum_X * sum_X) * double(nCount * squareSum_Y - sum_Y * sum_Y);
	a = sqrt(a);
	double corr = (double)(nCount * sum_XY - sum_X * sum_Y) / a;
	return corr;
}


void Ctali_ExamDlg::OnStnClickedStaticImage()
{
	CString sCheck1;
	CString sCheck2;
	if (m_bGetPattern1) {
		m_bGetPattern1 = false;
		GetPattern(m_arrTarget[0]);
		GetDlgItem(IDC_STATIC_GET_CHECK)->SetWindowTextW(_T("True"));
	}
	else if (m_bGetPattern2) {
		m_bGetPattern2 = false;
		GetPattern(m_arrTarget[1]);
		GetDlgItem(IDC_STATIC_GET_CHECK_2)->SetWindowTextW(_T("True"));
	}
	GetDlgItem(IDC_STATIC_GET_CHECK)->GetWindowTextW(sCheck1);
	GetDlgItem(IDC_STATIC_GET_CHECK_2)->GetWindowTextW(sCheck2);
	if (sCheck1 == "True" && sCheck2 == "True") {
		m_bFind = true;
		GetDlgItem(IDC_BTN_SEARCH_PATTERN)->EnableWindow(true);
	}
}


void Ctali_ExamDlg::OnBnClickedBtnGetPattern()
{
	if (!m_bGetPattern1)
	{
		m_bGetPattern1 = true;
		GetDlgItem(IDC_STATIC_GET_CHECK)->SetWindowTextW(_T("False"));
		GetDlgItem(IDC_BTN_SEARCH_PATTERN)->EnableWindow(false);
	}
}


void Ctali_ExamDlg::OnBnClickedBtnGetPattern2()
{
	if (!m_bGetPattern2)
	{
		m_bGetPattern2 = true;
		GetDlgItem(IDC_STATIC_GET_CHECK_2)->SetWindowTextW(_T("False"));
		GetDlgItem(IDC_BTN_SEARCH_PATTERN)->EnableWindow(false);
	}
}

void Ctali_ExamDlg::GetPattern(unsigned char* Target)
{
	int nWidth = m_imgDisplay.gGetWidth();
	int nHeight = m_imgDisplay.gGetHeight();
	unsigned char* fm = m_imgDisplay.gGetImgPtr();
	CPoint ptImg, ptMouse;
	m_imgDisplay.gGetPixelInfo(ptMouse, ptImg);
	int nEdge;
	m_dbSet.SerGet(true, nEdge, L"Edge");
	ptImg.x = nEdge;
	CRect rect(ptImg.x- m_nPatLength, 0, ptImg.x+ m_nPatLength, nHeight);
	for (int i = rect.left; i < rect.right; i++) {
		Target[i - rect.left] = fm[ptImg.y*nWidth + i];
	}
	CPoint pStart(ptImg.x - m_nPatLength, ptImg.y), pEnd(ptImg.x + m_nPatLength, ptImg.y);
	m_imgDisplay.gDrawLine(pStart, pEnd, COLOR_BLUE, 2);
	m_imgDisplay.UpdateDisplay();
}


void Ctali_ExamDlg::OnBnClickedBtnSearchPattern()
{
	if (m_bFind) {
		m_imgDisplay.gDrawClear();
		int nWidth = m_imgDisplay.gGetWidth();
		int nHeight = m_imgDisplay.gGetHeight();
		unsigned char* fm = m_imgDisplay.gGetImgPtr();
		int pixel;
		CPoint ptImg, ptMouse;
		CPoint pScoreLine[2]{ 0, 0 };
		int X[1200], Y[1200];
		int nEdge;
		double dScore[2];
		m_imgDisplay.gGetPixelInfo(ptMouse, ptImg);
		m_dbSet.SerGet(true, nEdge, L"Edge");
		ptImg.x = nEdge;
		CRect rect(ptImg.x - m_nPatLength, 0, ptImg.x + m_nPatLength, nHeight);
		unsigned char* line = new unsigned char[rect.Width()]{ 0, };
		unsigned char* error = new unsigned char[nHeight] { 0, };
		for (int j = 0; j < nHeight; j++) {
			for (int i = rect.left; i < rect.right; i++) {
				line[i - rect.left] = fm[j*nWidth + i];
			}
			dScore[0] = getCorreationData(m_arrTarget[0], line, rect.Width());
			dScore[1] = getCorreationData(m_arrTarget[1], line, rect.Width());
			double dRet = max(dScore[0], dScore[1]);
			X[j] = (int)(dRet * 424 + 500);
			Y[j] = j;
		}
		m_imgDisplay.gDrawLine(CPoint(int(500 + 424 * m_dThScore), 0), CPoint(int(500 + 424 * m_dThScore), nHeight), COLOR_YELLOW, 1);
		
		m_imgDisplay.gPolyline(nHeight, X, Y, COLOR_GREEN);
		m_imgDisplay.gDrawRect(rect);
		m_imgDisplay.UpdateDisplay();
		delete error;
		delete line;
	}
}


void Ctali_ExamDlg::OnBnClickedBtnSetLineLength()
{
	CString str;
	GetDlgItemText(IDC_EDIT_INPUT_RANGE, str);
	m_nPatLength = _ttoi(str);
	m_imgDisplay.gDrawClear();
	m_imgDisplay.UpdateDisplay();
}


void Ctali_ExamDlg::OnBnClickedBtnSetThreshold()
{
	CString str;
	GetDlgItemText(IDC_EDIT_INPUT_THRESHOLD, str);
	m_dThScore = (double)_ttoi(str)/100;
	m_imgDisplay.gDrawClear();
	m_imgDisplay.gDrawLine(CPoint(int(500 + 424 * m_dThScore), 0), CPoint(int(500 + 424 * m_dThScore), m_imgDisplay.gGetHeight()), COLOR_YELLOW, 1);
	m_imgDisplay.UpdateDisplay();
}




//2���� �̹����� �и�
bool Ctali_ExamDlg::DivideFullImage(unsigned char* pSrc, unsigned char* pDstW, unsigned char* pDstD, int width, int height)
{
	bool bRet = false;

	//ī�޶� 2���� ���� �Ǿ��ֱ� ������ ��⿡ �ش��ϴ� 2���� Ȧ¦�迭 ��⸦ üũ
	CRect rtRoi[2];
	int centerX = 7776;
	int widthCopy = 7776;

	for (int moduleIdx = 0; moduleIdx < 2; moduleIdx++)
	{
		if (moduleIdx == 0)
			rtRoi[moduleIdx].SetRect(centerX - 1000, 0, centerX - 500, 0);
		else
			rtRoi[moduleIdx].SetRect(centerX + 500, 0, centerX + 1000, 0);

		// ���� ó��
		if (rtRoi[moduleIdx].left > rtRoi[moduleIdx].right) {
			return false;
		}
		if (rtRoi[moduleIdx].left < 0) {
			return false;
		}
		if (rtRoi[moduleIdx].right > width) {
			return false;
		}

		unsigned char* pImgBuf0 = nullptr;
		unsigned char* pImgBuf1 = nullptr;
		PIXEL_VAL firstPixelVal = GetFirstLinePixelVal(pSrc, width, rtRoi[moduleIdx]);

		if (firstPixelVal == PIXEL_VAL::PIXEL_VAL_WHITE) {
			pImgBuf0 = pDstW;
			pImgBuf1 = pDstD;
		}
		else {
			pImgBuf0 = pDstD;
			pImgBuf1 = pDstW;
		}

		int idxY[2] = { 0, };
		const unsigned char* pFullImg = pSrc;

		for (int y = 0; y < height; y++) {
			if (moduleIdx == 1) {
				if (y % 2 == 0) {
					memcpy(pImgBuf0 + (idxY[0]) * width + (centerX * moduleIdx), pFullImg + y * width + (centerX * moduleIdx), widthCopy);
					idxY[0]++;
				}
				else {
					memcpy(pImgBuf1 + (idxY[1]) * width + (centerX * moduleIdx), pFullImg + y * width + (centerX * moduleIdx), widthCopy);
					idxY[1]++;
				}
			}
			else {
				if (y % 2 == 0) {
					int yy = (y / 2) * width;
					memcpy(pImgBuf0 + (idxY[0]) * width, pFullImg + y * width, widthCopy);
					idxY[0]++;
				}
				else {
					memcpy(pImgBuf1 + (idxY[1]) * width, pFullImg + y * width, widthCopy);
					idxY[1]++;
				}
			}
		}
	}

	bRet = true;

	return bRet;
}

//2���� ������ 1st, 2nd ���� ��⸦ ���� ù��° ������ white, dark ���� ����
//return : enum PIXEL_VAL
PIXEL_VAL Ctali_ExamDlg::GetFirstLinePixelVal(unsigned char* pSrc, int width, const CRect& rtRoi)
{
	const int skipX = 2;
	int nSumPixelValue0 = 0;
	int nSumPixelValue1 = 0;

	for (int x = rtRoi.left; x < rtRoi.right; x += skipX)
	{
		nSumPixelValue0 += *(pSrc + x);
		nSumPixelValue1 += *(pSrc + width + x);
	}

	int aveFirst = nSumPixelValue0 / rtRoi.Width();
	int aveSecond = nSumPixelValue1 / rtRoi.Width();

	return ((aveFirst > aveSecond) ? PIXEL_VAL::PIXEL_VAL_WHITE : PIXEL_VAL::PIXEL_VAL_DARK);
}


int Ctali_ExamDlg::Inspect(INSPECT_PARAM inspect_param, bool* bAbNormalTab, bool* bInkMarked)
{
	int retDefectCnt = 0;

	CRect rtRoiLeft;
	CRect rtRoiRight;
	rtRoiLeft.SetRect(inspect_param.ROI_TRIM_L, 0, inspect_param.ROI_TRIM_R, 1536);
	rtRoiRight.SetRect(inspect_param.ROI_TAB_L, 0, inspect_param.ROI_TAB_R, 1536);

	int th = inspect_param.Tab_TH;

	unsigned char* pImgW = m_imgDisplay_w.gGetImgPtr();
	unsigned char* pImgB = m_imgDisplay_b.gGetImgPtr();
	int width = m_imgDisplay_w.gGetWidth();
	int height = m_imgDisplay_w.gGetHeight();


	int cntW = 0;
	int cntB = 0;
	CRect rtTabROI = funcTabRoi(m_pChain, pImgW, width, height, th, &cntW, &cntB);

	//������ Ȯ��. �����ȿ� ������ ������
	bool abNormalTab = false;
	if (cntW > inspect_param.InkMarkCnt_W_Min && cntB < inspect_param.InkMarkCnt_W_Max) {
		abNormalTab = false;
	}
	else {
		abNormalTab = true;
	}

	bool inkMarked = false;
	if (cntB > inspect_param.InkMarkCnt_B) {
		inkMarked = true;
	}
	else {
		inkMarked = false;
	}

	(*bAbNormalTab) = abNormalTab;
	(*bInkMarked) = inkMarked;

	vector<int> vtEdgeCoatTab;
	vector<int> vtEdgeCoatTrim;

	int edgeThCoat = inspect_param.Edge_TH;

	//1���� ã������ offset �ٿ��� �ٽ� ã�°����� ������ ����
	bool reserch = true;

	vtEdgeCoatTab = FindDetailEdge(pImgW, pImgB, width, height, rtRoiRight, edgeThCoat,
		SEARCH_DIR::SEARCH_DIR_RtL, BRIGHT_DIFF_CONDITION::BRIGHT_DIFF_CONDITION_WtB, reserch);

	vtEdgeCoatTrim = FindDetailEdge(pImgW, pImgB, width, height, rtRoiLeft, edgeThCoat,
		SEARCH_DIR::SEARCH_DIR_LtR, BRIGHT_DIFF_CONDITION::BRIGHT_DIFF_CONDITION_WtB, reserch);

	//�������
	int aveTabEdge = 0;
	int aveTrimEdge = 0;
	int sum = 0;
	for (int i = 0; i < vtEdgeCoatTab.size(); i++) {
		sum += vtEdgeCoatTab[i];
	}
	if (vtEdgeCoatTab.size() > 0) {
		aveTabEdge = sum / vtEdgeCoatTab.size();
	}
	sum = 0;
	for (int i = 0; i < vtEdgeCoatTrim.size(); i++) {
		sum += vtEdgeCoatTrim[i];
	}
	if (vtEdgeCoatTrim.size() > 0) {
		aveTrimEdge = sum / vtEdgeCoatTrim.size();
	}

	int* edgePosTab = &vtEdgeCoatTab[0];
	int* edgePosTrim = &vtEdgeCoatTrim[0];
	int thW = inspect_param.Inspect_TH_W;
	int thB = inspect_param.Inspect_TH_B;
	int binaryRange = inspect_param.Inspect_Range;
	int skipXSttTab = 4;	//edge ��ó �������ؼ� ���̳ʸ� �� skip
	int skipXSttTrim = 10;
	bool reverseBinaryTab = true;
	bool reverseBinaryTrim = false;

	//�ӽ÷� �̹��� Dark ���۸� ���̳ʸ� �̹��� ���۷� ���
	memset(pImgB, 0, sizeof(unsigned char) * width * height);

	//���̳ʸ�
	int skipYTabEdge = 40; //�� ���Ʒ� ���� ���� �κ� ���̳ʸ����� ������ ��ŵ ����
						   //����
	m_imgProcess.BinarizationEdgeRange(pImgW, pImgB,
		width, 0, rtTabROI.top - skipYTabEdge,
		thW, thB, edgePosTab, binaryRange, reverseBinaryTab, skipXSttTab);

	//�ǾƷ�
	m_imgProcess.BinarizationEdgeRange(pImgW, pImgB,
		width, rtTabROI.bottom + skipYTabEdge, height,
		thW, thB, edgePosTab, binaryRange, reverseBinaryTab, skipXSttTab);

	//Ʈ��
	m_imgProcess.BinarizationEdgeRange(pImgW, pImgB,
		width, 0, height,
		thW, thB, edgePosTrim, binaryRange, reverseBinaryTrim, skipXSttTrim);

	//�ҷ�ã��
	m_pChain->SetChainData(1, pImgB, 2, 2, 20, 100000, width, height);

	int blobCnt = m_pChain->FastChain(0, 0, width, height);


	m_imgDisplay_w.gDrawClear();
	m_imgDisplay_b.gDrawClear();

	//�ּ� ���� ����
	int minArea = inspect_param.Defect_Min_Area;

	for (int defectIdx = 0; defectIdx < blobCnt; defectIdx++) {
		double area = m_pChain->Chain_Area(defectIdx);

		if (area < minArea) {
			continue;
		}

		CRect rt;
		rt.SetRect(
			m_pChain->FindMinX(defectIdx),
			m_pChain->FindMinY(defectIdx),
			m_pChain->FindMaxX(defectIdx),
			m_pChain->FindMaxY(defectIdx));

		rt.InflateRect(50, 50);	//�۾Ƽ� �Ѵ��� �ȵ��� ����
		m_imgDisplay_w.gDrawRect(rt);
		m_imgDisplay_b.gDrawRect(rt);

		retDefectCnt++;
	}

	//draw
	m_imgDisplay_w.gDrawRect(rtTabROI);

	m_imgDisplay_w.gDrawRect(rtRoiLeft);
	m_imgDisplay_w.gDrawRect(rtRoiRight);
	m_imgDisplay_w.UpdateDisplay();
	m_imgDisplay_b.UpdateDisplay();

	return retDefectCnt;
}


vector<int> Ctali_ExamDlg::FindDetailEdge(BYTE* fmBright, BYTE* fmDark, int nWidth, int  nHeight,
	CRect rtIns, int th, int dir, int diffCondition, bool research)
{
	int nSearchOffset = 10;

	//����ó��
	if (rtIns.left < 0) {
		rtIns.left = 0;
	}
	if (rtIns.right > nWidth) {
		rtIns.right = nWidth;
	}
	if (rtIns.top < 0) {
		rtIns.top = 0;
	}
	if (rtIns.bottom > nHeight) {
		rtIns.bottom = nHeight;
	}

	std::vector<int> vt_edge;
	vt_edge.clear();
	int before_edge = 0;

	//ã������ diff offset �ٿ��� �� �� ��Ȯ�ϰ� ã��
	int research_range = 10;
	int research_offset = 5;

	for (int y = rtIns.top; y < rtIns.bottom; y++) {
		bool bSearchedEdge_tab = false;
		bool bSearchedEdge_coat = false;

		//y 2�� ��չ�� ������ �� ������ �׳� ���� edge�� ä���
		if (y + 1 >= rtIns.bottom) {
			vt_edge.emplace_back(before_edge);
			continue;
		}

		if (dir == SEARCH_DIR::SEARCH_DIR_LtR) {
			bool searched = false;
			for (int x = rtIns.left; x < rtIns.right - nSearchOffset; x++) {

				int nVal_left = *(fmBright + (y + 0) * nWidth + x) + *(fmBright + (y + 1) * nWidth + x);
				nVal_left /= 2;

				int nVal_right = *(fmBright + (y + 0) * nWidth + (x + nSearchOffset)) + *(fmBright + (y + 1) * nWidth + (x + nSearchOffset));
				nVal_right /= 2;


				int diff = 0;

				if (diffCondition == BRIGHT_DIFF_CONDITION::BRIGHT_DIFF_CONDITION_WtB) {
					diff = nVal_left - nVal_right;
				}
				else {
					diff = nVal_right - nVal_left;
				}

				if (diff > th) {
					if (research == true) {
						//����ġ ��� ����Ҷ��� ���������
					}
					else {
						vt_edge.emplace_back(x + (nSearchOffset * 0.5));
					}

					searched = true;
					before_edge = x + (nSearchOffset * 0.5);
					break;
				}
			}
			if (searched == false) {
				vt_edge.emplace_back(before_edge);
			}
			else if (searched == true) {
				int peak_val = 0;
				int peak_pos = 0;
				for (int x = before_edge - research_range; x < before_edge + research_range; x++) {

					int nVal_left = *(fmBright + (y + 0) * nWidth + x) + *(fmBright + (y + 1) * nWidth + x);
					nVal_left /= 2;

					int nVal_right = *(fmBright + (y + 0) * nWidth + (x + research_offset)) + *(fmBright + (y + 1) * nWidth + (x + research_offset));
					nVal_right /= 2;

					int diff = abs(nVal_left - nVal_right);
					if (peak_val < diff) {
						peak_val = diff;
						peak_pos = x + (research_offset * 0.5);
					}
				}
				vt_edge.emplace_back(peak_pos);
			}
		}
		else if (dir == SEARCH_DIR::SEARCH_DIR_RtL) {		//DIR_RtL
			bool searched = false;
			for (int x = rtIns.right - nSearchOffset - 1; x >= rtIns.left; x--) {

				int nVal_left = *(fmBright + (y + 0) * nWidth + x) + *(fmBright + (y + 1) * nWidth + x);
				nVal_left /= 2;

				int nVal_right = *(fmBright + (y + 0) * nWidth + (x + nSearchOffset)) + *(fmBright + (y + 1) * nWidth + (x + nSearchOffset));
				nVal_right /= 2;


				int diff = 0;

				if (diffCondition == BRIGHT_DIFF_CONDITION::BRIGHT_DIFF_CONDITION_WtB) {
					diff = nVal_right - nVal_left;
				}
				else {
					diff = nVal_left - nVal_right;
				}

				if (diff > th) {
					if (research == true) {
						//����ġ ��� ����Ҷ��� ���������
					}
					else {
						vt_edge.emplace_back(x + (nSearchOffset * 0.5));
					}

					searched = true;
					before_edge = x + (nSearchOffset * 0.5);
					break;
				}
			}
			if (searched == false) {
				vt_edge.emplace_back(before_edge);
			}
			else if (searched == true) {
				int peak_val = 0;
				int peak_pos = 0;
				for (int x = before_edge - research_range; x < before_edge + research_range; x++) {

					int nVal_left = *(fmBright + (y + 0) * nWidth + x) + *(fmBright + (y + 1) * nWidth + x);
					nVal_left /= 2;

					int nVal_right = *(fmBright + (y + 0) * nWidth + (x + research_offset)) + *(fmBright + (y + 1) * nWidth + (x + research_offset));
					nVal_right /= 2;


					int diff = abs(nVal_left - nVal_right);
					if (peak_val < diff) {
						peak_val = diff;
						peak_pos = x + (research_offset * 0.5);
					}
				}
				vt_edge.emplace_back(peak_pos);
			}
		}
	}

	return vt_edge;
}


void Ctali_ExamDlg::OnBnClickedBtnSearchCoatEdge()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	bool abnormalTab = false;
	bool inkMarked = false;
	m_searchDefectCnt = Inspect(m_inspect_param, &abnormalTab, &inkMarked);

	if (abnormalTab == true) {
		SetDlgItemTextW(IDC_ABNORMAL_TAB, TEXT("TRUE"));
	}
	else {
		SetDlgItemTextW(IDC_ABNORMAL_TAB, TEXT("FALSE"));
	}

	if (inkMarked == true) {
		SetDlgItemTextW(IDC_INK_MARKED, TEXT("TRUE"));
	}
	else {
		SetDlgItemTextW(IDC_INK_MARKED, TEXT("FALSE"));
	}

	CString strImgDst;
	bool bNG = false;

	if (abnormalTab == false && inkMarked == false && m_searchDefectCnt > 0) {//������
		//strImgDst.Format(TEXT("%s\\%s"), strDestPathNG, vt_file_list[nCnt].m_strFileName);
		bNG = true;
	}
	else if (abnormalTab == true || (abnormalTab == false && inkMarked == true)) {//�Ǻҷ�:skip or ������ and ��ũ��ŷ:skip
		//strImgDst.Format(TEXT("%s\\%s"), strDestPathOK, vt_file_list[nCnt].m_strFileName);
		bNG = false;
	}

	if (bNG == true) {
		SetDlgItemTextW(IDC_STATIC_JUDGE, TEXT("NG"));
	}
	else {
		SetDlgItemTextW(IDC_STATIC_JUDGE, TEXT("OK"));
	}
	UpdateData(false);

}


//�Ķ���� ���¸� �ʱ�ȭ �Ѵ�.
void Ctali_ExamDlg::CreateParam()
{
	//�Ķ���� ��� ����
	_param.SetParameterPath(fmt::format("C:\\Glim\\inspect_param.ini"));

	//�׷� ����
	
	string strGroupName_ROI = fmt::format("Inspect ROI");
	string strGroupName_Inspect = fmt::format("Inspect");

	//�Ķ���� �ʱ�ȭ
	bool data_bool[2];
	int data_int[2];
	float data_float[2];
	double data_double[2];
	CString data_string[2];

	//�ϴ� �� �۾��� ���α׷� ������ ����� ini ���� �� �ε尡 ������
	shared_ptr<pair<string, PARAM>> ptr_param;

	
	string key;
	key = "ROI_TRIM_L";
	ptr_param = _param.MakeParam(strGroupName_ROI, key.c_str(), 1050);
	_param.SetParam(ptr_param);

	key = "ROI_TRIM_R";
	ptr_param = _param.MakeParam(strGroupName_ROI, key.c_str(), 1500);
	_param.SetParam(ptr_param);

	key = "ROI_TAB_L";
	ptr_param = _param.MakeParam(strGroupName_ROI, key.c_str(), 13700);
	_param.SetParam(ptr_param);

	key = "ROI_TAB_R";
	ptr_param = _param.MakeParam(strGroupName_ROI, key.c_str(), 14270);
	_param.SetParam(ptr_param);

	key = "Tab_TH";
	ptr_param = _param.MakeParam(strGroupName_Inspect, key.c_str(), 200);
	_param.SetParam(ptr_param);

	key = "Edge_TH";
	ptr_param = _param.MakeParam(strGroupName_Inspect, key.c_str(), 50);
	_param.SetParam(ptr_param);

	key = "Inspect_TH_W";
	ptr_param = _param.MakeParam(strGroupName_Inspect, key.c_str(), 220);
	_param.SetParam(ptr_param);

	key = "Inspect_TH_B";
	ptr_param = _param.MakeParam(strGroupName_Inspect, key.c_str(), 50);
	_param.SetParam(ptr_param);

	key = "Inspect_Range";
	ptr_param = _param.MakeParam(strGroupName_Inspect, key.c_str(), 600);
	_param.SetParam(ptr_param);

	key = "Defect_Min_Area";
	ptr_param = _param.MakeParam(strGroupName_Inspect, key.c_str(), 20);
	_param.SetParam(ptr_param);

	key = "InkMarkCnt_W_Min";
	ptr_param = _param.MakeParam(strGroupName_Inspect, key.c_str(), 140000);
	_param.SetParam(ptr_param);

	key = "InkMarkCnt_W_Max";
	ptr_param = _param.MakeParam(strGroupName_Inspect, key.c_str(), 160000);
	_param.SetParam(ptr_param);

	key = "InkMarkCnt_B";
	ptr_param = _param.MakeParam(strGroupName_Inspect, key.c_str(), 100);
	_param.SetParam(ptr_param);


	string param_path = _param.GetParameterPath();
	CFileFind fileFind;
	BOOL bExist = fileFind.FindFile((CString)param_path.c_str());

	if (bExist == true) {
		_param.LoadParam();
	}

	_param.SaveParam();

}

void Ctali_ExamDlg::InitProperty()
{
	HDITEM item;
	item.cxy = 200;
	item.mask = HDI_WIDTH;
	m_property.GetHeaderCtrl().SetItem(0, &HDITEM(item));


}

//���̾�α׻� ��� ������Ƽ�� paramŬ�������� ������ ������ ������Ʈ ���ش�.
void Ctali_ExamDlg::SyncParamToProperty()
{
	//�׷�
	vector<string> group_list = _param.GetListGroup();
	for (int i = 0; i < group_list.size(); i++) {
		string group_name = group_list[i];
		CMFCPropertyGridProperty* main_group = new CMFCPropertyGridProperty((CString)group_name.c_str());
		m_property.AddProperty(main_group);


		//�׷��̸����� ���� ����Ʈ�� �����´�.
		vector<string> vt_param_in_group = _param.GetListParamFromGroupName(group_name);

		for each (string key in vt_param_in_group) {
			//�ϴ� �Ķ���� ��������
			shared_ptr<pair<string, PARAM>> ptr_param;
			ptr_param = _param.GetParam(key);

			PARAM stParam = ptr_param->second;

			CMFCPropertyGridProperty* sub_group = NULL;

			if (stParam.nDataType == TYPE_BOOLEAN) {
				auto val = stParam.bValue;
				sub_group = new CMFCPropertyGridProperty((CString)key.c_str(), (_variant_t)val, TEXT("Select object"));
			}
			else if (stParam.nDataType == TYPE_INT) {
				auto val = stParam.nValue;
				sub_group = new CMFCPropertyGridProperty((CString)key.c_str(), (_variant_t)val, TEXT("Select object"));
			}
			else if (stParam.nDataType == TYPE_DOUBLE) {
				auto val = stParam.dValue;
				sub_group = new CMFCPropertyGridProperty((CString)key.c_str(), (_variant_t)val, TEXT("Select object"));
			}
			else if (stParam.nDataType == TYPE_STRING) {
				CString val = (CString)stParam.strValue.c_str();
				sub_group = new CMFCPropertyGridProperty((CString)key.c_str(), (_variant_t)val, TEXT("Select object"));
			}

			//������ Ÿ�Կ� �°� ����׷쿡 ����Ѵ�
			main_group->AddSubItem(sub_group);

		}
	}
}

//���̾�α׻� ��� ������Ƽ�� ������ paramŬ������ ���� �� update�Ѵ�(with ini save).
void Ctali_ExamDlg::SyncPropertyToParam()
{
	//gParameter���� �׷������� �����ͼ� �ش��ϴ� ������ Ÿ���� ������ ��,
	//�� �����Ϳ� �´� Ÿ������ �ٽ� gParameter�� set �Ѵ�.

	vector<string> group_list = _param.GetListGroup();
	for (int group_idx = 0; group_idx < group_list.size(); group_idx++) {
		string group_name = group_list[group_idx];

		//�׷��� �����´�
		CMFCPropertyGridProperty* get_group = m_property.GetProperty(group_idx);

		//�׷��̸����� ���� ����Ʈ�� �����´�.
		vector<string> vt_param_in_group = _param.GetListParamFromGroupName(group_name);

		for (int sub_group_idx = 0; sub_group_idx < vt_param_in_group.size(); sub_group_idx++) {

			string key = vt_param_in_group[sub_group_idx];

			//���� �׷� ��������
			CMFCPropertyGridProperty* get_sub_group = get_group->GetSubItem(sub_group_idx);

			//�ϴ� �Ķ���� ��������
			shared_ptr<pair<string, PARAM>> ptr_param;
			ptr_param = _param.GetParam(key);

			PARAM* pParam = &ptr_param->second;

			COleVariant vars = get_sub_group->GetValue();

			//�Ķ���Ϳ� �� ������ Ÿ���� �� ��, ��ȯ
			if (pParam->nDataType == TYPE_BOOLEAN) {
				bool bData = vars.boolVal;
				pParam->bValue = bData;
			}
			else if (pParam->nDataType == TYPE_INT) {
				int nData = vars.iVal;
				pParam->nValue = nData;
			}
			else if (pParam->nDataType == TYPE_DOUBLE) {
				double dData = vars.dblVal;
				pParam->dValue = dData;
			}
			else if (pParam->nDataType == TYPE_STRING) {
				CString str = (LPCTSTR)(_bstr_t)vars.bstrVal;
				pParam->strValue = CT2CA(str);
			}

			_param.SetParam(ptr_param);
		}
	}

	_param.SaveParam();
}

//�˻��Ķ���͸� ���� ����ü�� ������ �־���
void Ctali_ExamDlg::SetInspectParam(INSPECT_PARAM* param, gParameter* gParameter)
{
	vector<string> paramList = gParameter->GetListParam();

	for (int idx = 0; idx < paramList.size(); idx++) {
		shared_ptr<pair<string, PARAM>> ptr_param = nullptr;
		ptr_param = gParameter->GetParam(paramList[idx]);
		
		if (ptr_param->first == "ROI_TRIM_L") {
			param->ROI_TRIM_L = ptr_param->second.nValue;
		}
		if (ptr_param->first == "ROI_TRIM_R") {
			param->ROI_TRIM_R = ptr_param->second.nValue;
		}
		if (ptr_param->first == "ROI_TAB_L") {
			param->ROI_TAB_L = ptr_param->second.nValue;
		}
		if (ptr_param->first == "ROI_TAB_R") {
			param->ROI_TAB_R = ptr_param->second.nValue;
		}
		if (ptr_param->first == "Tab_TH") {
			param->Tab_TH = ptr_param->second.nValue;
		}
		if (ptr_param->first == "Edge_TH") {
			param->Edge_TH = ptr_param->second.nValue;
		}
		if (ptr_param->first == "Inspect_TH_W") {
			param->Inspect_TH_W = ptr_param->second.nValue;
		}
		if (ptr_param->first == "Inspect_TH_B") {
			param->Inspect_TH_B = ptr_param->second.nValue;
		}
		if (ptr_param->first == "Inspect_Range") {
			param->Inspect_Range = ptr_param->second.nValue;
		}
		if (ptr_param->first == "Defect_Min_Area") {
			param->Defect_Min_Area = ptr_param->second.nValue;
		}
		if (ptr_param->first == "InkMarkCnt_W_Min") {
			param->InkMarkCnt_W_Min = ptr_param->second.nValue;
		}
		if (ptr_param->first == "InkMarkCnt_W_Max") {
			param->InkMarkCnt_W_Max = ptr_param->second.nValue;
		}
		if (ptr_param->first == "InkMarkCnt_B") {
			param->InkMarkCnt_B = ptr_param->second.nValue;
		}
	}
}




void Ctali_ExamDlg::OnBnClickedBtnSaveInspectParam()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	SyncPropertyToParam();
	SetInspectParam(&m_inspect_param, &_param);

}


void Ctali_ExamDlg::OnBnClickedBtnFolderInspect()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString strPath = GetFolderPath();

	if (strPath.IsEmpty() == false) {
		RunFolderInspect(strPath);
		
	}
}

void Ctali_ExamDlg::RunFolderInspect(CString strPath)
{
	//ũ���̹��� ���� - ������� ó��
	AfxBeginThread(ThreadInspectFolder, new stFolderInsParam(this, strPath), THREAD_PRIORITY_NORMAL, 0);
}

CString Ctali_ExamDlg::GetFolderPath()
{
	// �ʱ� ���� ����
	CString strInitPath = _T("D:\\");

	// ���� ���� ���̾�α�
	CFolderPickerDialog Picker(strInitPath, OFN_FILEMUSTEXIST, NULL, 0);

	CString strPathName;
	CString strFileName;
	CString strFolderPath;

	if (IDOK == Picker.DoModal()) {
		strPathName = Picker.GetPathName();
		strFileName = Picker.GetFileName();
		strFolderPath = Picker.GetFolderPath();
	}

	return strPathName;
}

void Ctali_ExamDlg::FolderInspect(CString strPath)
{
	//���� ���� ����Ʈ�� �����´�.
	std::vector<stFileString> vt_file_list = GetFileListInFolder(strPath);

	CString strDestPathOK;
	strDestPathOK.Format(TEXT("%s\\OK"), strPath);

	CString strDestPathNG;
	strDestPathNG.Format(TEXT("%s\\NG"), strPath);

	if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(strDestPathOK)) {
		CreateDirectory(strDestPathOK, NULL);
	}
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributes(strDestPathNG)) {
		CreateDirectory(strDestPathNG, NULL);
	}

	for (int nCnt = 0; nCnt < vt_file_list.size(); nCnt++){
		CString strFile;
		strFile.Format(TEXT("%s"), vt_file_list[nCnt].m_strFolderPath);
		
		m_imgDisplay.gLoad(strFile);

		DivideFullImage(m_imgDisplay.gGetImgPtr(),
			m_imgDisplay_w.gGetImgPtr(),
			m_imgDisplay_b.gGetImgPtr(),
			m_imgDisplay.gGetWidth(),
			m_imgDisplay.gGetHeight()
		);

		bool abnormalTab = false;
		bool inkMarked = false;
		m_searchDefectCnt = Inspect(m_inspect_param, &abnormalTab, &inkMarked);

		if (abnormalTab == true) {
			SetDlgItemTextW(IDC_ABNORMAL_TAB, TEXT("TRUE"));
		}
		else {
			SetDlgItemTextW(IDC_ABNORMAL_TAB, TEXT("FALSE"));
		}

		if (inkMarked == true) {
			SetDlgItemTextW(IDC_INK_MARKED, TEXT("TRUE"));
		}
		else {
			SetDlgItemTextW(IDC_INK_MARKED, TEXT("FALSE"));
		}

		CString strImgDst;
		bool bNG = false;
		
		if (abnormalTab == false && inkMarked == false && m_searchDefectCnt > 0) {//������
			strImgDst.Format(TEXT("%s\\%s"), strDestPathNG, vt_file_list[nCnt].m_strFileName);
			bNG = true;
		}
		else if(abnormalTab == true || (abnormalTab == false && inkMarked == true)){//�Ǻҷ�:skip or ������ and ��ũ��ŷ:skip
			strImgDst.Format(TEXT("%s\\%s"), strDestPathOK, vt_file_list[nCnt].m_strFileName);
			bNG = false;
		}

		if (bNG == true) {
			SetDlgItemTextW(IDC_STATIC_JUDGE, TEXT("NG"));
		}
		else {
			SetDlgItemTextW(IDC_STATIC_JUDGE, TEXT("OK"));
		}

		CopyFile(strFile, strImgDst, false);

		WriteFile(strPath, strImgDst, bNG, m_searchDefectCnt);
	}
}

std::vector<stFileString> Ctali_ExamDlg::GetFileListInFolder(CString strPath)
{
	std::vector<stFileString> vt_file_list;
	vt_file_list.clear();

	CString strPathAllFile;
	strPathAllFile.Format(TEXT("%s\\*.*"), strPath);

	// �˻� Ŭ����
	CFileFind finder;

	// CFileFind�� ����, ���͸��� �����ϸ� TRUE �� ��ȯ��
	BOOL bWorking = finder.FindFile(strPathAllFile);

	CString strFilePath;
	CString strFileName;

	while (bWorking)
	{
		//���� ���� or ���� �� �����ϸ�ٸ� TRUE ��ȯ
		bWorking = finder.FindNextFile();

		// folder �� ���� continue
		if (finder.IsDirectory() || finder.IsDots())
			continue;

		//������ �̸�
		CString _fileName = finder.GetFileName();

		// �������� �������� ����������� ����
		if (_fileName == _T("Thumbs.db")) continue;

		strFilePath = finder.GetFilePath();
		strFileName = finder.GetFileName();

		stFileString fileString;
		fileString.m_strFolderPath.Format(TEXT("%s"), strFilePath);
		fileString.m_strFileName.Format(TEXT("%s"), strFileName);

		//�о�� ���� �̸��� ����Ʈ�ڽ��� ����
		vt_file_list.emplace_back(fileString);
	}

	return vt_file_list;

}

void Ctali_ExamDlg::WriteFile(CString filePath, CString fileNameWithPath, bool bNG, int defectCnt)
{
	//create folder
	//filePath

	CString strFile;
	strFile.Format(TEXT("%s\\InspectResult.csv"), filePath);

	SYSTEMTIME sysTime;
	::GetLocalTime(&sysTime);

	CString strYMD_Time;
	strYMD_Time.Format(_T("%d%02d%02d_%02d%02d%02d%03d"),
		sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);

	CStdioFile file;

	CString strHeader;

	bool bReadFail = false;

	CFileFind pFind;
	BOOL bRet = pFind.FindFile(strFile);

	if (bRet == false) {
		if (!file.Open(strFile, CFile::modeCreate | CFile::modeWrite | CFile::typeText)) {
			bReadFail = true;
		}
		else {
			//��� ���
			strHeader.Format(TEXT("%s%s%s%s"),
				AddQMark(TEXT("Date")), AddQMark(TEXT("FilePath")),
				AddQMark(TEXT("Judge")), AddQMark(TEXT("DefectCnt"))
			);

			file.WriteString(strHeader + "\n");
		}
	}
	else {
		if (!file.Open(strFile, CFile::modeWrite | CFile::typeText)) {
			bReadFail = true;
		}
	}

	if (bReadFail) {
		return;
	}

	file.SeekToEnd();

	CString strJudge;
	if (bNG == true) {
		strJudge.Format(TEXT("NG"));
	}
	else {
		strJudge.Format(TEXT("OK"));
	}
	
	CString strDefect;
	strDefect.Format(TEXT("%s%s%s%s"),
		AddQMark(strYMD_Time),
		AddQMark(fileNameWithPath),
		AddQMark(strJudge),
		AddQMark(defectCnt)
	);

	file.WriteString(strDefect + TEXT("\n"));

	file.Close();
}

CString Ctali_ExamDlg::AddQMark(CString str) {
	CString retStr = str + ",";
	return retStr;
}

std::string Ctali_ExamDlg::AddQMark_s(std::string  str) {
	std::string  retStr = str + ",";
	return retStr;
}

CString Ctali_ExamDlg::AddQMark(int str) {
	CString retStr;
	retStr.Format(TEXT("%d"), str);
	retStr = retStr + TEXT(",");
	return retStr;
}

CString Ctali_ExamDlg::AddQMark(double str) {
	CString retStr;
	retStr.Format(TEXT("%0.2f"), str);
	retStr = retStr + TEXT(",");
	return retStr;
}

CString Ctali_ExamDlg::AddQMark(float str) {
	CString retStr;
	retStr.Format(TEXT("%0.2f"), str);
	retStr = retStr + TEXT(",");
	return retStr;
}

HBRUSH Ctali_ExamDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  ���⼭ DC�� Ư���� �����մϴ�.
	switch (nCtlColor)
	{
	case CTLCOLOR_STATIC:
	{
		if (pWnd->GetDlgCtrlID() == IDC_STATIC_JUDGE)
		{
			pDC->SetTextColor(RGB(255, 0, 0));
			pDC->SetBkColor(RGB(0, 255, 0));
			CFont font;

			font.CreateFontW(60, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, 0, 0, 0, 0, 0, 0, TEXT("Arial"));
			CFont* oldFont = (CFont*)pDC->SelectObject(&font);
		}
	}
	}

	// TODO:  �⺻���� �������� ������ �ٸ� �귯�ø� ��ȯ�մϴ�.
	return hbr;
}
