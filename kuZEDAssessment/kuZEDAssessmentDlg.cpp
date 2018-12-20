
// kuZEDAssessmentDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "kuZEDAssessment.h"
#include "kuZEDAssessmentDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CBWidthPts	6
#define CBHeightPts 9

// 對 App About 使用 CAboutDlg 對話方塊

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

// 程式碼實作
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


// CkuZEDAssessmentDlg 對話方塊



CkuZEDAssessmentDlg::CkuZEDAssessmentDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_KUZEDASSESSMENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CkuZEDAssessmentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TotalDistError, m_TotalCBDistError);
}

BEGIN_MESSAGE_MAP(CkuZEDAssessmentDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OpenCam, &CkuZEDAssessmentDlg::OnBnClickedOpencam)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_DetectChessboard, &CkuZEDAssessmentDlg::OnBnClickedDetectchessboard)
	ON_BN_CLICKED(IDC_DetectFace, &CkuZEDAssessmentDlg::OnBnClickedDetectface)
	ON_BN_CLICKED(IDC_GetSurface, &CkuZEDAssessmentDlg::OnBnClickedGetsurface)
END_MESSAGE_MAP()


// CkuZEDAssessmentDlg 訊息處理常式

BOOL CkuZEDAssessmentDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 將 [關於...] 功能表加入系統功能表。

	// IDM_ABOUTBOX 必須在系統命令範圍之中。
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

	// 設定此對話方塊的圖示。當應用程式的主視窗不是對話方塊時，
	// 框架會自動從事此作業
	SetIcon(m_hIcon, TRUE);			// 設定大圖示
	SetIcon(m_hIcon, FALSE);		// 設定小圖示

	// TODO: 在此加入額外的初始設定
	RECT	rect;
	GetDlgItem(IDC_ColorView)->GetWindowRect(&rect);
	int rectWidth = abs(rect.right - rect.left);
	int rectHeight = abs(rect.bottom - rect.top);

	cvNamedWindow("ColorView", 0);
	cvResizeWindow("ColorView", rectWidth, rectHeight);
	HWND hWnd1 = (HWND)cvGetWindowHandle("ColorView");
	HWND hParent1 = ::GetParent(hWnd1);
	::SetParent(hWnd1, GetDlgItem(IDC_ColorView)->m_hWnd);
	::ShowWindow(hParent1, SW_HIDE);

	cvNamedWindow("DepthView", 0);
	cvResizeWindow("DepthView", rectWidth, rectHeight);
	HWND hWnd2 = (HWND)cvGetWindowHandle("DepthView");
	HWND hParent2 = ::GetParent(hWnd2);
	::SetParent(hWnd2, GetDlgItem(IDC_DepthView)->m_hWnd);
	::ShowWindow(hParent2, SW_HIDE);

	SetView3D(m_VTK3DObj, IDC_VTKView, 100.0f, kuVTKColor(0.2f, 0.3f, 0.3f));

	SetTimer(1, 33, NULL);

	m_CamFlag		 = false;
	m_DetectCBFlag	 = false;
	m_DetectFaceFlag = false;

	m_CB3DPoints = new CvPoint3D32f[CBWidthPts * CBHeightPts];

	m_CB3DActor = vtkSmartPointer<vtkActor>::New();

	m_HaarCascade.load("haarcascade_frontalface_alt.xml");

	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
}

void CkuZEDAssessmentDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果將最小化按鈕加入您的對話方塊，您需要下列的程式碼，
// 以便繪製圖示。對於使用文件/檢視模式的 MFC 應用程式，
// 框架會自動完成此作業。

void CkuZEDAssessmentDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 繪製的裝置內容

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 將圖示置中於用戶端矩形
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 描繪圖示
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 當使用者拖曳最小化視窗時，
// 系統呼叫這個功能取得游標顯示。
HCURSOR CkuZEDAssessmentDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

cv::Mat CkuZEDAssessmentDlg::slMat2cvMat(sl::Mat & inputImg)
{
	// Mapping between MAT_TYPE and CV_TYPE
	int cv_type = -1;
	switch (inputImg.getDataType()) {
	case sl::MAT_TYPE_32F_C1: cv_type = CV_32FC1; break;
	case sl::MAT_TYPE_32F_C2: cv_type = CV_32FC2; break;
	case sl::MAT_TYPE_32F_C3: cv_type = CV_32FC3; break;
	case sl::MAT_TYPE_32F_C4: cv_type = CV_32FC4; break;
	case sl::MAT_TYPE_8U_C1: cv_type = CV_8UC1; break;
	case sl::MAT_TYPE_8U_C2: cv_type = CV_8UC2; break;
	case sl::MAT_TYPE_8U_C3: cv_type = CV_8UC3; break;
	case sl::MAT_TYPE_8U_C4: cv_type = CV_8UC4; break;
	default: break;
	}

	// Since cv::Mat data requires a uchar* pointer, we get the uchar1 pointer from sl::Mat (getPtr<T>())
	// cv::Mat and sl::Mat will share a single memory structure
	return cv::Mat(inputImg.getHeight(), inputImg.getWidth(), cv_type, inputImg.getPtr<sl::uchar1>(sl::MEM_CPU));
}

void CkuZEDAssessmentDlg::SetView3D(kuVTKViz3D & vtkViz3DObj, int viewID, float scale, kuVTKColor vtkColor)
{
	CStatic	* pclStatic;
	LPRECT wndRect = new CRect;
	int wndWidth, wndHeight;

	pclStatic = (CStatic *)GetDlgItem(viewID);
	pclStatic->GetWindowRect(wndRect);
	wndWidth = wndRect->right - wndRect->left;
	wndHeight = wndRect->bottom - wndRect->top;
	vtkViz3DObj.Initialize(pclStatic->m_hWnd, wndWidth, wndHeight, scale, vtkColor);

	delete wndRect;
}

void CkuZEDAssessmentDlg::OnBnClickedOpencam()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	m_InitParams.camera_resolution = sl::RESOLUTION_HD720;
	m_InitParams.depth_mode		   = sl::DEPTH_MODE_ULTRA;
	m_InitParams.coordinate_units  = sl::UNIT_MILLIMETER;

	sl::ERROR_CODE	err = m_ZEDCam.open(m_InitParams);

	m_RuntimeParams.sensing_mode = sl::SENSING_MODE_FILL;
	sl::Resolution image_size = m_ZEDCam.getResolution();

	m_ImgWidth  = image_size.width;
	m_ImgHeight = image_size.height;

	m_ImgColorZED.alloc(m_ImgWidth, m_ImgHeight, sl::MAT_TYPE_8U_C4);
	m_ImgDepthZED.alloc(m_ImgWidth, m_ImgHeight, sl::MAT_TYPE_8U_C4);
	
	//m_ImgColorCV.create(m_ImgHeight, m_ImgWidth, CV_8UC4);
	//m_ImgDepthCV.create(m_ImgHeight, m_ImgWidth, CV_8UC4);

	m_ImgColorCV = slMat2cvMat(m_ImgColorZED);
	m_ImgDepthCV = slMat2cvMat(m_ImgDepthZED);

	m_CamFlag = true;
}


void CkuZEDAssessmentDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	if (m_CamFlag)
	{
		m_ZEDCam.grab(m_RuntimeParams);

		m_ZEDCam.retrieveImage(m_ImgColorZED, sl::VIEW_LEFT, sl::MEM_CPU, m_ImgWidth, m_ImgHeight);
		m_ZEDCam.retrieveImage(m_ImgDepthZED, sl::VIEW_DEPTH, sl::MEM_CPU, m_ImgWidth, m_ImgHeight);

		m_ZEDCam.retrieveMeasure(m_PtCloud, sl::MEASURE_XYZRGBA, sl::MEM_CPU, m_ImgWidth, m_ImgHeight);

		if (m_DetectCBFlag)
		{
			bool found = cv::findChessboardCorners(m_ImgColorCV, cv::Size(CBWidthPts, CBHeightPts), m_CornerPts, CV_CALIB_CB_ADAPTIVE_THRESH);
			cv::drawChessboardCorners(m_ImgColorCV, cv::Size(CBWidthPts, CBHeightPts), m_CornerPts, found);

			sl::float4					pt3DTemp;
			if (found)
			{
				for (int i = 0; i < CBWidthPts * CBHeightPts; i++)
				{
					m_PtCloud.getValue(m_CornerPts[i].x, m_CornerPts[i].y, &pt3DTemp);
					
					m_CB3DPoints[i] = CvPoint3D32f(pt3DTemp.x, pt3DTemp.y, pt3DTemp.z);
				}

				#pragma region // relative error評估 //
				/////////////////////////////////////////////////////////////////////////
				float		PtDist;
				float		vertCBDistError = 0;
				float		horiCBDistError = 0;
				m_TotalCBDistError = 0;
				float	*	vertCBDist = new float[(CBWidthPts - 1) * CBHeightPts];
				float	*	horiCBDist = new float[CBWidthPts * (CBHeightPts - 1)];

				#pragma region // Vertical //
				for (int i = 0; i < CBHeightPts; i++)
				{
					for (int j = 0; j < (CBWidthPts - 1); j++)
					{
						PtDist = sqrt(pow((m_CB3DPoints[CBWidthPts * i + (j + 1)].x - m_CB3DPoints[CBWidthPts * i + j].x), 2)
									+ pow((m_CB3DPoints[CBWidthPts * i + (j + 1)].y - m_CB3DPoints[CBWidthPts * i + j].y), 2)
									+ pow((m_CB3DPoints[CBWidthPts * i + (j + 1)].z - m_CB3DPoints[CBWidthPts * i + j].z), 2));

						vertCBDist[(CBWidthPts - 1) * i + j] = PtDist;

						vertCBDistError += abs(PtDist - CBWidth) / ((CBWidthPts - 1) * CBHeightPts);
					}
				}
				#pragma endregion

				#pragma region // Horizontal //
				for (int i = 0; i < (CBHeightPts - 1); i++)
				{
					for (int j = 0; j < CBWidthPts; j++)
					{
						PtDist = sqrt(pow((m_CB3DPoints[CBWidthPts * (i + 1) + j].x - m_CB3DPoints[CBWidthPts * i + j].x), 2)
									+ pow((m_CB3DPoints[CBWidthPts * (i + 1) + j].y - m_CB3DPoints[CBWidthPts * i + j].y), 2)
									+ pow((m_CB3DPoints[CBWidthPts * (i + 1) + j].z - m_CB3DPoints[CBWidthPts * i + j].z), 2));

						horiCBDist[CBWidthPts * i + j] = PtDist;

						horiCBDistError += abs(PtDist - CBWidth) / (CBWidthPts * (CBHeightPts - 1));
					}
				}
				#pragma endregion

				for (int i = 0; i < (CBWidthPts - 1) * CBHeightPts; i++)
				{
					m_TotalCBDistError += abs(vertCBDist[i] - CBWidth);
				}
				for (int j = 0; j < CBWidthPts * (CBHeightPts - 1); j++)
				{
					m_TotalCBDistError += abs(horiCBDist[j] - CBWidth);
				}

				m_TotalCBDistError /= ((CBWidthPts - 1) * CBHeightPts + CBWidthPts * (CBHeightPts - 1));

				FILE * fp;
				errno_t err = fopen_s(&fp, "CBRelativeError.txt", "w");
				fprintf(fp, "Mean Distance Error: %f\n", m_TotalCBDistError);
				fprintf(fp, "Mean Vertical Distance Error: %f\n", vertCBDistError);
				fprintf(fp, "Mean Horizontal Distance Error: %f\n", horiCBDistError);
				for (int i = 0; i < (CBWidthPts - 1) * CBHeightPts; i++)
				{
					fprintf(fp, "%f\n", abs(vertCBDist[i] - CBWidth));
				}
				for (int i = 0; i < CBWidthPts * (CBHeightPts - 1); i++)
				{
					fprintf(fp, "%f\n", abs(horiCBDist[i] - CBWidth));
				}
				fclose(fp);

				UpdateData(false);
				/////////////////////////////////////////////////////////////////////////
				#pragma endregion	

				std::fstream file;
				file.open("CB3DData.txt", std::ios::out);
				for (int i = 0; i < CBWidthPts * CBHeightPts; i++)
				{
					file << m_CB3DPoints[i].x << " " << m_CB3DPoints[i].y << " " << m_CB3DPoints[i].z << std::endl;
				}
				file.close();

				m_VTK3DObj.DrawSpheres(m_CB3DActor, CBWidthPts * CBHeightPts, m_CB3DPoints, 3, kuVTKColor(1.0, 0.0, 0.0));
			}

			UpdateData(false);
		}

		if (m_DetectFaceFlag)
		{
			cv::Mat grayImg;
			cv::cvtColor(m_ImgColorCV, grayImg, CV_RGBA2GRAY);

			std::vector<cv::Rect_<int>> faces;
			m_HaarCascade.detectMultiScale(grayImg, faces);

			for (int i = 0; i < faces.size(); i++)
			{
				cv::Rect face_i = faces[i];

				cv::rectangle(m_ImgColorCV, face_i, CV_RGB(255, 0, 0), 2, CV_AA);
			}
		}

		cv::imwrite("ImgColorCV.bmp", m_ImgColorCV);
		cv::imwrite("ImgDepthCV.bmp", m_ImgDepthCV);

		cv::imshow("ColorView", m_ImgColorCV);
		cv::imshow("DepthView", m_ImgDepthCV);
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CkuZEDAssessmentDlg::OnBnClickedDetectchessboard()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	m_DetectCBFlag = !m_DetectCBFlag;
}


void CkuZEDAssessmentDlg::OnBnClickedDetectface()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	m_DetectFaceFlag = true;
}


void CkuZEDAssessmentDlg::OnBnClickedGetsurface()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	std::vector<cv::Point3f>	surfacePts;
	sl::float4					pt3DTemp;

	for (int x = 625; x < 853; x++)
	{
		for (int y = 200; y < 573; y++)
		{
			m_PtCloud.getValue(x, y, &pt3DTemp);
			if (!isnan(pt3DTemp.x) && !isnan(pt3DTemp.y) && !isnan(pt3DTemp.z))
			{
				cv::Point3f temp = cv::Point3f(pt3DTemp.x, pt3DTemp.y, pt3DTemp.z);
				surfacePts.push_back(temp);
			}
		}
	}

	int ptsNum = surfacePts.size();

	fstream file;
	file.open("ZEDSurfacePts.txt", std::ios::out);
	for (int i = 0; i < ptsNum; i++)
	{
		file << surfacePts[i].x << " " << surfacePts[i].y << " " << surfacePts[i].z << endl;
	}
	file.close();

	vtkSmartPointer<vtkActor>	ptsActor = vtkSmartPointer<vtkActor>::New();

	CvPoint3D32f * pts = new CvPoint3D32f[ptsNum];
	m_VTK3DObj.DrawPoints(ptsActor, ptsNum, surfacePts, 3, kuVTKColor(0.0, 0.0, 1.0));
}
