
// tali_ExamDlg.h : 헤더 파일
//
#define COLOR_YELLOW RGB(255, 255, 51)

#pragma once
#include "gImage.h"
#include "gLogger.h"
#include "gCfg.h"
#include "Fchain.h"
#include "ImageProcess.h"
#include "gParameter.h"
#include "afxwin.h"


#define _INI_FILE_PATH L"../../Setting/tali.ini"
#define TIMER_ID_MOUSE_INFO 100

enum PIXEL_VAL
{
	PIXEL_VAL_WHITE = 0,
	PIXEL_VAL_DARK,
};

enum SEARCH_DIR
{
	SEARCH_DIR_LtR,
	SEARCH_DIR_RtL,
};

enum BRIGHT_DIFF_CONDITION
{
	BRIGHT_DIFF_CONDITION_WtB,
	BRIGHT_DIFF_CONDITION_BtW,
	BRIGHT_DIFF_CONDITION_ABS,
};

struct INSPECT_PARAM
{
	int ROI_TRIM_L;
	int ROI_TRIM_R;
	int ROI_TAB_L;
	int ROI_TAB_R;
	int Tab_TH;
	int Edge_TH;
	int Inspect_TH_W;
	int Inspect_TH_B;
	int Inspect_Range;
	int Defect_Min_Area;
	int InkMarkCnt_W_Min;
	int InkMarkCnt_W_Max;
	int InkMarkCnt_B;
	int SearchTabRoi_L;
	int SearchTabRoi_R;
	int SearchTabRoi_T;
	int SearchTabRoi_B;
	
	INSPECT_PARAM()
	{
		ROI_TRIM_L = 0;
		ROI_TRIM_R = 0;
		ROI_TAB_L = 0;
		ROI_TAB_R = 0;
		Tab_TH = 0;
		Edge_TH = 0;
		Inspect_TH_W = 0;
		Inspect_TH_B = 0;
		Inspect_Range = 0;
		Defect_Min_Area = 0;
		InkMarkCnt_W_Min = 0;
		InkMarkCnt_W_Max = 0;
		InkMarkCnt_B = 0;
		SearchTabRoi_L = 0;
		SearchTabRoi_R = 0;
		SearchTabRoi_T = 0;
		SearchTabRoi_B = 0;
	}
};

class Ctali_ExamDlg;

struct stFolderInsParam
{
	Ctali_ExamDlg* m_pView;
	CString m_strFolderPath;
	stFolderInsParam(Ctali_ExamDlg* pView, CString strFolderPath)
		:m_pView(pView), m_strFolderPath(strFolderPath) {}

};

struct stFileString
{
	CString m_strFolderPath;
	CString m_strFileName;
};

// Ctali_ExamDlg 대화 상자
class Ctali_ExamDlg : public CDialogEx
{
// 생성입니다.
public:
	Ctali_ExamDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TALI_EXAM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	//property_inspect
	gParameter _param;
	//property_inspect
	CMFCPropertyGridCtrl m_property;
	//property_inspect
	void CreateParam();
	//property_inspect
	void SyncParamToProperty();
	//property_inspect
	void SyncPropertyToParam();
	//property_inspect
	void InitProperty();
	//property_inspect
	void SetInspectParam(INSPECT_PARAM* param, gParameter* gParameter);


	//검사파라미터
	INSPECT_PARAM m_inspect_param;
  

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	bool DivideFullImage(unsigned char* pSrc, unsigned char* pDstW, unsigned char* pDstD, int width, int height);
	PIXEL_VAL GetFirstLinePixelVal(unsigned char* pSrc, int width, const CRect& rtRoi);
	vector<int> FindDetailEdge(BYTE* fmBright, BYTE* fmDark, int nWidth, int  nHeight,
		CRect rtIns, int th, int dir, int diffCondition, bool research);

private:
	void setLogger();
	void setDisplay();
	void GetPattern(unsigned char* Target);
	unsigned char** m_arrTarget = new unsigned char*[2]{ new unsigned char[1024], new unsigned char[1024] };
	bool m_bGetPattern1 = false;
	bool m_bGetPattern2 = false;
	bool m_bFind = false;
	int m_nPatLength = 100;
	double m_dThScore = 0.95;

	CRect funcTabRoi(std::shared_ptr<CChain> chain, unsigned char* pImg, int width, int height, int th, int* cntW, int* cntB, CRect rtROI);

	std::shared_ptr<CChain> m_pChain;
	ImageProcess m_imgProcess;

	int Inspect(INSPECT_PARAM inspect_param, bool* bAbNormalTab, bool* bInkMarked, int* _cntW, int* _cntB);
	int m_searchDefectCnt = 0;

	CString GetFolderPath();
	void RunFolderInspect(CString strPath);
	
	std::vector<stFileString> GetFileListInFolder(CString strPath);
	void WriteFile(CString filePath, CString fileNameWithPath, bool judge, int defectCnt);

	CString AddQMark(CString str);
	CString AddQMark(double str);
	std::string AddQMark_s(std::string  str);
	CString AddQMark(int str);
	CString AddQMark(float str);
	CRect InvalidRect(CRect rtArea, int width, int height);

public:
	gImage m_imgDisplay;
	gImage m_imgDisplay_w;
	gImage m_imgDisplay_b;
	gLogger m_logger;
	gCfg m_dbSet;

	void FolderInspect(CString strPath);

public:
	afx_msg void OnBnClickedBtnLoad();
	afx_msg void OnBnClickedBtnGetEdge();
	afx_msg void OnStnClickedStaticImage();
	afx_msg void OnBnClickedBtnGetPattern();
	afx_msg void OnBnClickedBtnGetPattern2();
	afx_msg void OnBnClickedBtnSearchPattern();
	afx_msg void OnBnClickedBtnSetLineLength();
	afx_msg void OnBnClickedBtnSetThreshold();
	afx_msg void OnBnClickedBtnGetTabroi();
	afx_msg void OnBnClickedBtnSearchCoatEdge();
	afx_msg void OnBnClickedBtnSaveInspectParam();
	afx_msg void OnBnClickedBtnFolderInspect();
	CEdit m_abnormalTab;
	CEdit m_InkMarked;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
