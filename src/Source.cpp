//opencv libraries
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//C++ standard libraries
#include <iostream>
#include <vector>
#include <Windows.h>

using namespace cv;
using namespace std;

Mat result;

//function declarations

//double to string
string double2string(double do_b);
uchar toZero(uchar a);

/**
Function that returns the maximum of 3 integers
@param a first integer
@param b second integer
@param c third integer
*/
int myMax(int a, int b, int c);

/**
Function that returns the minimum of 3 integers
@param a first integer
@param b second integer
@param c third integer
*/
int myMin(int a, int b, int c);

/**
Function that detects whether a pixel belongs to the skin based on RGB values
@param src The source color image
@param dst The destination grayscale image where skin pixels are colored white and the rest are colored black
*/
void mySkinDetect(Mat& src, Mat& dst);

int main()
{

	// read templates
	Mat tp[5], tp_skin[5];
	tp[0] = imread("templates/1.jpg", 1);
	tp[1] = imread("templates/2.jpg", 1);
	tp[2] = imread("templates/3.jpg", 1);
	tp[3] = imread("templates/4.jpg", 1);
	tp[4] = imread("templates/5.jpg", 1);

	cv::cvtColor(tp[0], tp[0], CV_BGR2GRAY);
	cv::cvtColor(tp[1], tp[1], CV_BGR2GRAY);
	cv::cvtColor(tp[2], tp[2], CV_BGR2GRAY);
	cv::cvtColor(tp[3], tp[3], CV_BGR2GRAY);
	cv::cvtColor(tp[4], tp[4], CV_BGR2GRAY);

	cv::threshold(tp[0], tp_skin[0], 66, 255, THRESH_BINARY);
	cv::threshold(tp[1], tp_skin[1], 20, 255, THRESH_BINARY);
	cv::threshold(tp[2], tp_skin[2], 20, 255, THRESH_BINARY);
	cv::threshold(tp[3], tp_skin[3], 20, 255, THRESH_BINARY);
	cv::threshold(tp[4], tp_skin[4], 20, 255, THRESH_BINARY);

	//----------------
	//a) Reading a stream of images from a webcamera, and displaying the video
	//----------------
	// For more information on reading and writing video: http://docs.opencv.org/modules/highgui/doc/reading_and_writing_images_and_video.html
	// open the video camera no. 0
	VideoCapture cap(0);

	// if not successful, exit program
	if (!cap.isOpened())
	{
		cout << "Cannot open the video cam" << endl;
		return -1;
	}

	//create a window called "MyVideoFrame0"
	namedWindow("MyVideo0", WINDOW_AUTOSIZE);
	Mat frame0;

	// read a new frame from video
	bool bSuccess0 = cap.read(frame0);

	//if not successful, break loop
	if (!bSuccess0)
	{
		cout << "Cannot read a frame from video stream" << endl;
	}

	//show the frame in "MyVideo" window
	imshow("MyVideo0", frame0);
	namedWindow("display_window", CV_WINDOW_AUTOSIZE);

	while (1)
	{
		// read a new frame from video
		Mat frame;
		bool bSuccess = cap.read(frame);

		//if not successful, break loop
		if (!bSuccess)
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}
		//show the frame in "MyVideo" window
		imshow("MyVideo0", frame0);

		Mat frame0c, framec;
		Mat frameDest;
		frameDest = Mat::zeros(frame.rows, frame.cols, CV_8UC1); //Returns a zero array of same size as src mat, and of type CV_8UC1
		//----------------
		//	b) Skin color detection
		//----------------
		mySkinDetect(frame, frameDest);
		medianBlur(frameDest, frameDest, 11);

		cv::Mat  img_edge, labels, img_color, stats, centroids, bounding_box;
		cv::threshold(frameDest, img_edge, 128, 255, cv::THRESH_BINARY_INV);
		bitwise_not(img_edge, img_edge);

		int i, nccomps = cv::connectedComponentsWithStats(
			img_edge, labels,
			stats, centroids
		);
		cout << "Total Connected Components Detected: " << nccomps << endl;
		if (nccomps <= 2) {
			frame0 = frame;
			continue;
		}
		vector<double> left(3);
		vector<double> top(3);
		vector<double> width(3);
		vector<double> height(3);
		int max1 = 1, max2 = 2;
		if (stats.at<int>(max2, cv::CC_STAT_AREA) > stats.at<int>(max1, cv::CC_STAT_AREA)) {
			max1 = 2;
			max2 = 1;
		}
		for (i = 3; i < nccomps; i++) {
			if (stats.at<int>(i, cv::CC_STAT_AREA) > stats.at<int>(max1, cv::CC_STAT_AREA)) {
				if (stats.at<int>(max1, cv::CC_STAT_AREA) > stats.at<int>(max2, cv::CC_STAT_AREA))
					max2 = max1;
				max1 = i;
			}
			else if ((stats.at<int>(i, cv::CC_STAT_AREA) <= stats.at<int>(max1, cv::CC_STAT_AREA)) && (stats.at<int>(i, cv::CC_STAT_AREA) > stats.at<int>(max2, cv::CC_STAT_AREA))) {
				max2 = i;
			}
		}


		left[0] = stats.at<int>(max1, CC_STAT_LEFT); //连通域的boundingbox的最左边
		top[0] = stats.at<int>(max1, CC_STAT_TOP);//连通域的boundingbox的最上边
		width[0] = stats.at<int>(max1, CC_STAT_WIDTH);//连通域的宽
		height[0] = stats.at<int>(max1, CC_STAT_HEIGHT);//连通域的高


		left[1] = stats.at<int>(max2, CC_STAT_LEFT); //连通域的boundingbox的最左边
		top[1] = stats.at<int>(max2, CC_STAT_TOP);//连通域的boundingbox的最上边
		width[1] = stats.at<int>(max2, CC_STAT_WIDTH);//连通域的宽
		height[1] = stats.at<int>(max2, CC_STAT_HEIGHT);//连通域的高

		Mat hand1, hand2;

		Rect rect1(left[0], top[0], width[0], height[0]);
		Rect rect2(left[1], top[1], width[1], height[1]);

		hand1 = frameDest(rect1);
		hand2 = frameDest(rect2);

		imshow("hand1", hand1);
		imshow("hand2", hand2);
		imshow("Skin", frameDest);

		int resize_value = 200;
		Mat hand1_resize, hand2_resize;
		hand1_resize = Mat::zeros(resize_value, resize_value, CV_8UC1); //Returns a zero array of same size as src mat, and of type CV_8UC1
		resize(hand1, hand1_resize, hand1_resize.size(), 0, 0, INTER_NEAREST);
		hand2_resize = Mat::zeros(resize_value, resize_value, CV_8UC1); //Returns a zero array of same size as src mat, and of type CV_8UC1
		resize(hand2, hand2_resize, hand2_resize.size(), 0, 0, INTER_NEAREST);

		Mat tp_resize[5];
		Mat tp_resizeleft[5];

		for (int i = 0; i < 5; i++) {
			tp_resize[i] = Mat::zeros(resize_value, resize_value, CV_8UC1); //Returns a zero array of same size as src mat, and of type CV_8UC1
			resize(tp_skin[i], tp_resize[i], tp_resize[i].size(), 0, 0, INTER_NEAREST);
			flip(tp_resize[i], tp_resizeleft[i], 1);
		}

		//Templete matching
		Mat result0;

		// Create the result matrix
		int result_cols = hand1_resize.cols - tp_resize[0].cols + 1;
		int result_rows = hand1_resize.rows - tp_resize[0].rows + 1;

		result0.create(result_rows, result_cols, CV_8UC1);

		matchTemplate(hand1_resize, tp_resize[0], result0, CV_TM_CCOEFF_NORMED);
		//normalize(result0, result0, 0, 1, NORM_MINMAX, -1, Mat());

		// Localizing the best match with minMaxLoc
		double minVal0; double maxVal0; Point minLoc0; Point maxLoc0;

		minMaxLoc(result0, &minVal0, &maxVal0, &minLoc0, &maxLoc0, Mat());

		Point matchLoc = maxLoc0;
		double matchvalue = maxVal0;
		int max_i = 0, max_j = 0;
		Mat match_result;


		// Create the result matrix
		int match_result_cols = hand1_resize.cols - tp_resize[0].cols + 1;
		int match_result_rows = hand1_resize.rows - tp_resize[0].rows + 1;

		match_result.create(match_result_rows, match_result_cols, CV_8UC1);

		for (int i = 0; i < 5; i++) {
			//Templete matching
			Mat result;

			// Create the result matrix
			int result_cols = hand1_resize.cols - tp_resize[i].cols + 1;
			int result_rows = hand1_resize.rows - tp_resize[i].rows + 1;

			result.create(result_rows, result_cols, CV_8UC1);

			matchTemplate(hand1_resize, tp_resize[i], result, CV_TM_CCOEFF_NORMED);
			//normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

			// Localizing the best match with minMaxLoc
			double minVal; double maxVal; Point minLoc; Point maxLoc;

			minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

			if (maxVal > matchvalue)
			{
				matchvalue = maxVal;
				matchLoc = maxLoc;
				max_i = i;
				match_result = result;
			}
		}

		for (int i = 0; i < 5; i++) {
			//Templete matching
			Mat result;

			// Create the result matrix
			int result_cols = hand1_resize.cols - tp_resizeleft[i].cols + 1;
			int result_rows = hand1_resize.rows - tp_resizeleft[i].rows + 1;

			result.create(result_rows, result_cols, CV_8UC1);

			matchTemplate(hand1_resize, tp_resizeleft[i], result, CV_TM_CCOEFF_NORMED);
			//normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

			// Localizing the best match with minMaxLoc
			double minVal; double maxVal; Point minLoc; Point maxLoc;

			minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

			if (maxVal > matchvalue)
			{
				matchvalue = maxVal;
				matchLoc = maxLoc;
				max_i = i;
				match_result = result;
			}
		}

		//Templete matching
		result0.create(result_rows, result_cols, CV_8UC1);

		matchTemplate(hand2_resize, tp_resize[0], result0, CV_TM_CCOEFF_NORMED);
		//normalize(result0, result0, 0, 1, NORM_MINMAX, -1, Mat());
		minMaxLoc(result0, &minVal0, &maxVal0, &minLoc0, &maxLoc0, Mat());

		Point matchLoc1 = maxLoc0;
		double matchvalue1 = maxVal0;
		int max_i1 = 0, max_j1 = 0;
		Mat match_result1;
		match_result.create(match_result_rows, match_result_cols, CV_8UC1);

		for (int i = 0; i < 5; i++) {
			//Templete matching
			Mat result;

			// Create the result matrix
			int result_cols = hand2_resize.cols - tp_resize[i].cols + 1;
			int result_rows = hand2_resize.rows - tp_resize[i].rows + 1;

			result.create(result_rows, result_cols, CV_8UC1);

			matchTemplate(hand2_resize, tp_resize[i], result, CV_TM_CCOEFF_NORMED);
			//normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

			// Localizing the best match with minMaxLoc
			double minVal; double maxVal; Point minLoc; Point maxLoc;

			minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

			if (maxVal > matchvalue1)
			{
				matchvalue1 = maxVal;
				matchLoc1 = maxLoc;
				max_i1 = i;
				match_result1 = result;
			}
		}

		for (int i = 0; i < 5; i++) {
			//Templete matching
			Mat result;

			// Create the result matrix
			int result_cols = hand2_resize.cols - tp_resizeleft[i].cols + 1;
			int result_rows = hand2_resize.rows - tp_resizeleft[i].rows + 1;

			result.create(result_rows, result_cols, CV_8UC1);

			matchTemplate(hand2_resize, tp_resizeleft[i], result, CV_TM_CCOEFF_NORMED);
			//normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

			// Localizing the best match with minMaxLoc
			double minVal; double maxVal; Point minLoc; Point maxLoc;

			minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

			if (maxVal > matchvalue1)
			{
				matchvalue1 = maxVal;
				matchLoc1 = maxLoc;
				max_i1 = i;
				match_result1 = result;
			}
		}

		// Show me what you got
		Mat img_display;
		frame0.copyTo(img_display);
		//blue
		if (matchvalue > 0.4)
		{
			Point loc1(left[0], top[0]);
			rectangle(img_display, loc1, Point(loc1.x + width[0], loc1.y + height[0]), Scalar(255, 0, 0), 2, 8, 0);
		}
		//red
		if (matchvalue1 > 0.4)
		{
			Point loc2(left[1], top[1]);
			rectangle(img_display, loc2, Point(loc2.x + width[1], loc2.y + height[1]), Scalar(0, 0, 255), 2, 8, 0);
			//rectangle(match_result, matchLoc1, Point(matchLoc1.x + tp_resizeleft[max_i1][max_j1].cols, matchLoc1.y + tp_resizeleft[max_i1][max_j1].rows), Scalar(0, 0, 255), 2, 8, 0);
		}
		string text; Point origin(0, 100);
		int font_face = cv::FONT_HERSHEY_COMPLEX;
		double font_scale = 0.5;
		int thickness = 1;

		text = "Blue ID:" + std::to_string(max_i + 1) + " maxvalue=" + double2string(matchvalue);
		text = text + " Red ID:" + std::to_string(max_i1 + 1) + " maxvalue=" + double2string(matchvalue1);

		putText(img_display, text, origin, font_face, font_scale, cv::Scalar(0, 255, 255), thickness, 8, 0);

		string add; Point origin1(100, 300);
		int font_face1 = cv::FONT_HERSHEY_COMPLEX;
		double font_scale1 = 2;
		int thickness1 = 1;

		add = std::to_string(max_i + 1) + " + " + std::to_string(max_i1 + 1) + " = " + std::to_string(max_i1 + 1 + max_i + 1);
		putText(img_display, add, origin1, font_face1, font_scale1, cv::Scalar(0, 0, 255), thickness1, 8, 0);

		//imshow("result_window", match_result);
		imshow("display_window", img_display);

		frame0 = frame;
		//wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		if (waitKey(30) == 27)
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}
	cap.release();
	return 0;
}

//Function that returns the maximum of 3 integers
int myMax(int a, int b, int c) {
	int m = a;
	(void)((m < b) && (m = b)); //short-circuit evaluation
	(void)((m < c) && (m = c));
	return m;
}

//Function that returns the minimum of 3 integers
int myMin(int a, int b, int c) {
	int m = a;
	(void)((m > b) && (m = b));
	(void)((m > c) && (m = c));
	return m;
}

//Function that detects whether a pixel belongs to the skin based on RGB values
void mySkinDetect(Mat& src, Mat& dst) {
	//Surveys of skin color modeling and detection techniques:
	//Vezhnevets, Vladimir, Vassili Sazonov, and Alla Andreeva. "A survey on pixel-based skin color detection techniques." Proc. Graphicon. Vol. 3. 2003.
	//Kakumanu, Praveen, Sokratis Makrogiannis, and Nikolaos Bourbakis. "A survey of skin-color modeling and detection methods." Pattern recognition 40.3 (2007): 1106-1122.
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			//For each pixel, compute the average intensity of the 3 color channels
			Vec3b intensity = src.at<Vec3b>(i, j); //Vec3b is a vector of 3 uchar (unsigned character)
			int B = intensity[0]; int G = intensity[1]; int R = intensity[2];
			if ((R > 95 && G > 40 && B > 20) && (myMax(R, G, B) - myMin(R, G, B) > 15) && (abs(R - G) > 15) && (R > G) && (R > B)) {
				dst.at<uchar>(i, j) = 255;
			}
		}
	}
}

// double to string
string double2string(double do_b)
{
	string str1;
	stringstream ss;
	ss << do_b;
	ss >> str1;
	return str1;
}

// Set negative to 0
uchar toZero(uchar a)
{
	if (a < 0)
		return 0;
	else
		return a;
}