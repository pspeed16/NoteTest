// OpenCVNoteTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "opencv2\core.hpp"
#include "opencv2\highgui.hpp"
#include "opencv2\opencv.hpp"
#include "opencv2\imgproc.hpp"
#include "opencv2\imgcodecs.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;
using namespace cv;

RNG rng(12345);
Mat whole = imread("wholenote.PNG", 0);
Mat half = imread("halfNote.PNG", 0);
Mat quarter = imread("quarterNote.PNG", 0);
Mat eighth = imread("eighthNote.PNG", 0);
Mat allNotes = imread("Note_1234.PNG", 0);
Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create();
vector<vector<Point>> contours;
vector<Vec4i> hierarchy;
int thresh = 175;
int maxThresh = 255;
bool notesDetected = false;
void printNotes(bool children[]);
void position();
void threshCallback(int, void*);

int main()
{
	
	
	/*bitwise_not(whole, whole);
	bitwise_not(half, half);
	bitwise_not(quarter, quarter);
	bitwise_not(eighth, eighth);*/
	blur(allNotes, allNotes, Size(13, 13));
	VideoCapture cap;
	if (!cap.open(0)) return 0;
	Mat bg;
	cap >> bg;
	cvtColor(bg, bg, CV_BGR2GRAY);
	threshold(bg, bg, 220, 255, CV_THRESH_BINARY);
	for (;;) {
		cap >> allNotes;
		medianBlur(allNotes, allNotes, 3);
		cvtColor(allNotes, allNotes, CV_BGR2GRAY);
		threshold(allNotes, allNotes, 220, 255, CV_THRESH_BINARY);
		erode(allNotes, allNotes, getStructuringElement(cv::MORPH_CROSS,
			cv::Size(2 * 1 + 1, 2 * 1 + 1),
			cv::Point(1, 1)));
		dilate(allNotes, allNotes, getStructuringElement(cv::MORPH_CROSS,
			cv::Size(2 * 1 + 1, 2 * 1 + 1),
			cv::Point(1, 1)));
		//subtract(allNotes, bg, allNotes);
		
		
		
		/// Create Window
		char* source_window = "Source";
		namedWindow(source_window, CV_WINDOW_AUTOSIZE);
		imshow(source_window, allNotes);
		Mat cannyOutput;
		threshold(allNotes, cannyOutput, thresh, 255, THRESH_BINARY);
		//Canny(allNotes, cannyOutput, thresh, thresh * 2, 3);
		findContours(cannyOutput, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
		vector<vector<Point>> contoursPoly(contours.size());
		vector<Rect> boundRect(contours.size());
		vector<Point2f> center(contours.size());
		vector<float> radius(contours.size());
		bool* hasChild = new bool[contours.size()];
		for (int i = 0; i < contours.size(); i++)
		{
			approxPolyDP(Mat(contours[i]), contoursPoly[i], 3, true);
			//boundRect[i] = boundingRect(Mat(contoursPoly[i]));
			//minEnclosingCircle((Mat)contoursPoly[i], center[i], radius[i]);
			if (hierarchy[i][3] == i - 1 && hierarchy[i - 1][2] == i&&arcLength(contours[i - 1], true) < 2000) hasChild[i - 1] = true;
			else hasChild[i - 1] = false;
		}
		Mat drawing = Mat::zeros(cannyOutput.size(), CV_8UC3);
		for (int i = 0; i< contours.size(); i++)
		{
			/*printf(" * Contour[%d] - Area OpenCV: %.2f - Length: %.2f \n", i, contourArea(contours[i]), arcLength(contours[i], true));
			if (arcLength(contours[i], true) > 800 && arcLength(contours[i], true) < 2000) cout << "Eighth!" << endl;
			else if (!hasChild[i] && arcLength(contours[i], true) < 800 && arcLength(contours[i], true)>400) cout << "Quarter!" << endl;
			if (hasChild[i]) {
				if (contourArea(contours[i])>5700 && contourArea(contours[i])<7500)
					cout << "is whole!" << endl;
				else cout << "is half!" << endl;
			}
			cout << hierarchy[i] << endl;*/
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
			rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
			namedWindow("Contours", CV_WINDOW_AUTOSIZE);
			imshow("Contours", drawing);
		}
		/*createTrackbar(" Canny thresh:", "Source", &thresh, maxThresh, threshCallback);
		threshCallback(0, 0);*/
		if (waitKey(10) == 13) {
			printNotes(hasChild);
			position();
		}
		if (waitKey(10)==27) break;
	}
	/*threshold(whole, whole, 220, 255, THRESH_BINARY);
	threshold(half, half, 220, 255, THRESH_BINARY);
	threshold(quarter, quarter, 220, 255, THRESH_BINARY);
	threshold(eighth, eighth, 220, 255, THRESH_BINARY);*/
	/*SimpleBlobDetector::Params params;
	params.minThreshold = 10;
	params.maxThreshold = 230;
	params.filterByArea = 1;
	params.minArea = 100;
	params.maxArea = 50000;
	params.filterByCircularity = 0;
	params.maxCircularity = 1.0;
	params.minCircularity = 0;
	params.filterByConvexity = 0;
	params.maxConvexity = 1.0;
	params.minConvexity = 0;
	params.filterByInertia = 0;
	params.maxInertiaRatio = 1;
	params.minInertiaRatio = 0;
	Ptr<SimpleBlobDetector> detector=SimpleBlobDetector::create(params);
	for (;;) {
		vector<KeyPoint> keypoints;
		detector->detect(whole, keypoints);
		Mat imKey;
		drawKeypoints(whole, keypoints, imKey, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
		imshow("whole", imKey);
		imshow("half", half);
		imshow("quarter", quarter);
		imshow("eighth", eighth);
		if (waitKey(10) == 27) break;
	}*/
		waitKey(0);
    return 0;
}

void printNotes(bool children[])
{
	for (int i = 0; i < contours.size(); i++) {
		if (arcLength(contours[i], true) > 800 && arcLength(contours[i], true) < 2000) cout << "Eighth!" << endl;
		else if (!children[i] && arcLength(contours[i], true) < 800 && arcLength(contours[i], true) > 400) cout << "Quarter!" << endl;
		if (children[i]) {
			if (contourArea(contours[i]) > 5700 && contourArea(contours[i]) < 7500)
				cout << "is whole!" << endl;
			else cout << "is half!" << endl;
		}
		cout << hierarchy[i] << endl;
	}
}
void position()
{
	int firstX = 112;
	int firstY = 198;
	vector<KeyPoint> keypoints;
	erode(allNotes, allNotes, getStructuringElement(cv::MORPH_CROSS,
		cv::Size(2 * 3 + 1, 2 * 3 + 1),
		cv::Point(3, 3)));
	dilate(allNotes, allNotes, getStructuringElement(cv::MORPH_CROSS,
		cv::Size(2 * 3 + 1, 2 * 3 + 1),
		cv::Point(3, 3)));
	detector->detect(allNotes, keypoints);
	for (int i = 0; i < keypoints.size(); i++) {
		float x = keypoints[i].pt.x;
		float y = keypoints[i].pt.y;
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 5; k++) {
				if (x > firstX + j * 86 && x < firstX + 83 + j * 86 && y>firstY + k * 46 && y < firstY + 34 + k * 46) {
					cout << "SPACE " << j << "," << k << " ";
				}
			}
		}
	}
	Mat im_with_keypoints;
	drawKeypoints(allNotes, keypoints, im_with_keypoints, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	imshow("keypoints", im_with_keypoints);
}

void threshCallback(int, void*)
{
	Mat cannyOutput;
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	threshold(allNotes, cannyOutput, thresh, 255, THRESH_BINARY);
	//Canny(allNotes, cannyOutput, thresh, thresh * 2, 3);
	findContours(cannyOutput,contours,hierarchy,CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	vector<vector<Point>> contoursPoly(contours.size());
	vector<Rect> boundRect(contours.size());
	vector<Point2f> center(contours.size());
	vector<float> radius(contours.size());
	bool* hasChild = new bool[contours.size()];
	for (int i = 0; i < contours.size(); i++) 
	{
		approxPolyDP(Mat(contours[i]), contoursPoly[i], 3, true);
		//boundRect[i] = boundingRect(Mat(contoursPoly[i]));
		//minEnclosingCircle((Mat)contoursPoly[i], center[i], radius[i]);
		if (hierarchy[i][3] == i - 1 && hierarchy[i - 1][2] == i&&arcLength(contours[i - 1], true) < 2000) hasChild[i-1] = true;
		else hasChild[i - 1] = false;
	}
	Mat drawing = Mat::zeros(cannyOutput.size(), CV_8UC3);
	for (int i = 0; i< contours.size(); i++)
	{
		printf(" * Contour[%d] - Area OpenCV: %.2f - Length: %.2f \n", i, contourArea(contours[i]), arcLength(contours[i], true));
		if (arcLength(contours[i], true) > 800 && arcLength(contours[i], true) < 2000) cout << "Eighth!"<< endl;
		else if (!hasChild[i] && arcLength(contours[i], true) < 800 && arcLength(contours[i], true)>400) cout << "Quarter!" << endl;
		if (hasChild[i]) {
			if(contourArea(contours[i])>5700 && contourArea(contours[i])<7500)
			cout << "is whole!" << endl;
			else cout << "is half!" << endl;
		}
		cout << hierarchy[i] << endl;
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
		rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
		namedWindow("Contours", CV_WINDOW_AUTOSIZE);
		imshow("Contours",drawing);
	}
}

