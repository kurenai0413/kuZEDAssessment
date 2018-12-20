
// kuZEDAssessmentDlg.h : ���Y��
//

#pragma once
#include <iostream>

#include <sl_zed/Camera.hpp>
#include <opencv2/opencv.hpp>

#include "kuVTKVisualization.h"

#define CBWidth 20

// CkuZEDAssessmentDlg ��ܤ��
class CkuZEDAssessmentDlg : public CDialogEx
{
// �غc
public:
	CkuZEDAssessmentDlg(CWnd* pParent = NULL);	// �зǫغc�禡

// ��ܤ�����
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_KUZEDASSESSMENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �䴩

	kuVTKViz3D						m_VTK3DObj;

	sl::Camera						m_ZEDCam;

	sl::InitParameters				m_InitParams;
	sl::RuntimeParameters			m_RuntimeParams;

	int								m_ImgWidth;
	int								m_ImgHeight;

	sl::Mat							m_ImgColorZED;
	sl::Mat							m_ImgDepthZED;

	cv::Mat							m_ImgColorCV;
	cv::Mat							m_ImgDepthCV;

	bool							m_CamFlag;
	bool							m_DetectCBFlag;
	bool							m_DetectFaceFlag;

	std::vector<cv::Point2f>		m_CornerPts;

	sl::Mat							m_PtCloud;

	float							m_TotalCBDistError;

	CvPoint3D32f				*	m_CB3DPoints;

	vtkSmartPointer<vtkActor>		m_CB3DActor;

	cv::CascadeClassifier			m_HaarCascade;
	 
// �{���X��@
protected:
	HICON m_hIcon;

	// ���ͪ��T�������禡
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	cv::Mat slMat2cvMat(sl::Mat &inputImg);
	void SetView3D(kuVTKViz3D &vtkViz3DObj, int viewID, float scale, kuVTKColor vtkColor);

public:
	afx_msg void OnBnClickedLoaddata();
	afx_msg void OnBnClickedOpencam();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedDetectchessboard();
	afx_msg void OnBnClickedDetectface();
	afx_msg void OnBnClickedGetsurface();
};
