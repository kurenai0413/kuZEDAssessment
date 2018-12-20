#include <sl_zed/Camera.hpp>
#include <opencv2/opencv.hpp>

cv::Mat slMat2cvMat(sl::Mat& input) {
	// Mapping between MAT_TYPE and CV_TYPE
	int cv_type = -1;
	switch (input.getDataType()) {
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
	return cv::Mat(input.getHeight(), input.getWidth(), cv_type, 
				   input.getPtr<sl::uchar1>(sl::MEM_CPU));
}


void main()
{
	sl::Camera			zed;

	sl::InitParameters	init_params;
	init_params.camera_resolution = sl::RESOLUTION_HD720;
	init_params.depth_mode = sl::DEPTH_MODE_QUALITY;
	init_params.coordinate_units = sl::UNIT_MILLIMETER;

	

	sl::ERROR_CODE	err = zed.open(init_params);
	
	sl::RuntimeParameters	runtime_params;
	runtime_params.sensing_mode = sl::SENSING_MODE_STANDARD;
	sl::Resolution image_size = zed.getResolution();

	int imgWidth = image_size.width;
	int imgHeight = image_size.height;

	sl::Mat	imgColorZED(imgWidth, imgHeight, sl::MAT_TYPE_8U_C4);
	cv::Mat imgColorCV = slMat2cvMat(imgColorZED);
	sl::Mat imgDepthZED(imgWidth, imgHeight, sl::MAT_TYPE_8U_C4);
	cv::Mat imgDepthCV = slMat2cvMat(imgDepthZED);

	while (zed.grab(runtime_params) == sl::SUCCESS)
	{
		zed.retrieveImage(imgColorZED, sl::VIEW_LEFT, sl::MEM_CPU, imgWidth, imgHeight);
		zed.retrieveImage(imgDepthZED, sl::VIEW_DEPTH, sl::MEM_CPU, imgWidth, imgHeight);

		cv::imshow("ColorImage", imgColorCV);
		cv::imshow("DepthImage", imgDepthCV);

		cv::waitKey(1);
	}

	system("pause");
}