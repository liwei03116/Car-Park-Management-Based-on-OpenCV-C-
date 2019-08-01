/*
 *  project.c
 *  finalprojectcolorchange
 *
 *  Created by WEI LI on 10/16/12.
 *  Copyright 2012 Monash University. All rights reserved.
 *
 */

#include "project.h"
#include <stdio.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdbool.h>
#include <opencv/cxcore.h>

/* global variable */
CvRect box;
CvRect boox1[50];

bool Drawing = false;
bool trigger = false;
bool temp = false;
bool checkCarSubtract = false;
bool checkCarPlus = false;
bool checkboolcolor;

IplImage* tempcompareimage[50];

int checkcolr[50];
int i = 0;
int tempcount = 0;
int r = 0;
int numberOfParking = 0; 
int checkcar[50];
int totaldiff;
int temprecordcannyvalue[50];
int triggerl = 0;
/* global variable */

void MouseDraw(int event,int x,int y,int flags,void* param);
void CheckImage(IplImage* imagecheck, CvRect tempboox1[]);
void histogramChack(IplImage* img, IplImage* img1, int ccar, int tecv);
void cannyCheck(IplImage* img, IplImage* img1);
void cannyPixelCheck(IplImage* img, IplImage* img1);

//initial draw rectangle for all parking slot, (color green)
void DrawRect(IplImage*imgd,CvRect rect)
{
	
	cvRectangle(imgd,
				cvPoint(rect.x,rect.y),
				cvPoint(rect.x+rect.width,rect.y+rect.height),
				CV_RGB(0,225,0), 2, 8, 0);
	
}
//checking method for each slot, if it is empty this method running(color green)
void DrawRectEmpty(IplImage*imgd,CvRect rect)
{
	
	cvRectangle(imgd,
				cvPoint(rect.x,rect.y),
				cvPoint(rect.x+rect.width,rect.y+rect.height),
				CV_RGB(0,225,0), 2, 8, 0);
	
}
//checking method for each slot, if it is full this method running(color red) 
void DrawRectFull(IplImage*imgd,CvRect rect)
{
	
	cvRectangle(imgd,
				cvPoint(rect.x,rect.y),
				cvPoint(rect.x+rect.width,rect.y+rect.height),
				CV_RGB(225,0,0), 2, 8, 0);
	
}
int main(int argc,char**argv)
{
	
	CvCapture* capture;
	capture = cvCaptureFromCAM(1);//capture from camera
	//capture = cvCaptureFromFile("/Users/weili/Documents/video/video-2012-10-18-16-32-55.mp4"); 
	int key = 0;
	CvFont font;
	int y;
	for (y=0; y<50; y++) {//initial all of check color number for 0, means red color
		checkcolr[y] = 0;
	}
	
	if (!capture) {
		printf("can not capture, no device found\n");
		return -1; 
	}
	
	cvNamedWindow("camshow",CV_WINDOW_AUTOSIZE);
	
	cvSetMouseCallback("camshow",
					   MouseDraw,
					   NULL);//call mouse event
	
	
	int j;
	bool temppress = false;
	bool temppresstest = false;
	while(key != 'q')// press "q" quit program 
	{
		
		
		IplImage *frame = cvQueryFrame(capture); // capture each of frame from camera
		
		IplImage *reframe = cvCreateImage(cvSize(1024, 768), IPL_DEPTH_8U, 3); // create new image frame to store implement
		
		cvResize(frame, reframe, CV_INTER_LINEAR); //resize initial image to new varibale
		
		if (key != 'l') {
			if (trigger) {
				for (j=0; j<=i; j++) {
					
					DrawRect(reframe,boox1[j]);
				}
			}
			if (temp && tempcount >= 1) {//store all initial slot image 
				if (temp) {
					cvSetImageROI(reframe, box);
					tempcompareimage[r] = cvCreateImage(cvSize(box.width, box.height), IPL_DEPTH_8U, 3);
					cvCopy(reframe, tempcompareimage[r], NULL);
					cvResetImageROI(reframe);
					r++;
					//cvShowImage("initialImage", tempcompareimage[r-1]);
					temp = false;
					
				}
			}
		}
		
		if (key == 'l' || temppress == true) {
			
			CheckImage(reframe, boox1);
			printf("Please waiting 5 seconds and press k\n");
			//key = cvWaitKey(1);
			temppress = true;
		}
		
		if (key == 'k' || temppresstest == true) {//check all of slot and show munbers
			triggerl = 1;
			temppresstest = true;
			
			IplImage* background = cvLoadImage("/users/weili/documents/opencvproject/blank.png", CV_LOAD_IMAGE_COLOR);
			char totalslot[50];
			sprintf(totalslot, "%d", i);
			char numpark[50];
			sprintf(numpark, "%d", numberOfParking);
			char numemptyoark[50];
			sprintf(numemptyoark, "%d", i - numberOfParking);
			
			cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX_SMALL, 1.0, 1.0, 0, 1, 8);
			cvPutText(background, "Total Parking Slot:", cvPoint(10, 30), &font, CV_RGB(0, 0, 225));
			cvPutText(background, totalslot, cvPoint(270, 30), &font, CV_RGB(0, 0, 225));
			cvPutText(background, "Full Place:", cvPoint(10, 60), &font, CV_RGB(0, 0, 225));
			cvPutText(background, numpark, cvPoint(180, 60), &font, CV_RGB(0, 0, 225));
			cvPutText(background, "Empty Place:", cvPoint(10, 90), &font, CV_RGB(0, 0, 225));
			cvPutText(background, numemptyoark, cvPoint(190, 90), &font, CV_RGB(0, 0, 225));
			cvNamedWindow("Display", CV_WINDOW_AUTOSIZE);
			cvShowImage("Display", background);
			printf("System Processing\n");
			//key = cvWaitKey(1);
			cvReleaseImage(&background);
		}
		cvShowImage("camshow",reframe);
		
		cvReleaseImage(&reframe);
		
		if(key == 'p'){
			key = cvWaitKey(0);
		}
		
		key = cvWaitKey(1);
		
	}
	
	cvReleaseCapture(&capture);
	
	cvDestroyWindow("camshow");
	cvDestroyWindow("Display");
	return 0;
}
//mouse event 
void MouseDraw(int event,int x,int y,int flags,void* param)
{
	
	switch(event)
	{
		case CV_EVENT_LBUTTONDOWN://start mouse event draw, when press the left button
		{
			Drawing=true;
			box=cvRect(x,y,0,0);
		}
			break;
		case CV_EVENT_MOUSEMOVE://record mouse move x and y coordinates
		{
			
            if(Drawing)
			{
				box.width=x-box.x;
				box.height=y-box.y;
			}
		}
			break;
		case CV_EVENT_LBUTTONUP://outout coordinates, when release left button
		{
			temp = true;
			tempcount++;
			Drawing=false;
			if (box.width<0)
			{
				box.x+=box.width;
				box.width*=-1;
			}
			if (box.height<0)
			{
				box.y+=box.height;
				box.height*=-1;
			}
			
			if (Drawing == false) {//store all of rectangle coordinates in array
				boox1[i] = box;
				i++;
				trigger = true;
				
			}
		}
			break;
	}
	
}
//checking every frame by the slot 
void CheckImage(IplImage* imagecheck, CvRect tempboox1[])
{
	int k;
	
	IplImage* tempc;
	
	for (k=0; k<i; k++) {
		cvSetImageROI(imagecheck, tempboox1[k]);
		tempc = cvCreateImage(cvSize(tempboox1[k].width, tempboox1[k].height), IPL_DEPTH_8U, 3);
		cvCopy(imagecheck, tempc, NULL);
		cvResetImageROI(imagecheck);
		IplImage* orgimage = cvCreateImage(cvSize(tempboox1[k].width, tempboox1[k].height), IPL_DEPTH_8U, 3);
		IplImage* tempccopy = cvCreateImage(cvSize(tempboox1[k].width, tempboox1[k].height), IPL_DEPTH_8U, 3);
		//histogramChack(tempcompareimage[k], tempc, checkcar[k]);
		cvCopy(tempcompareimage[k], orgimage, NULL);
		cvCopy(tempc, tempccopy, NULL);
		cannyCheck(orgimage, tempccopy);
		if (triggerl == 0) { //store every slot's canny detect pixel values in array
			temprecordcannyvalue[k] = totaldiff;
		}
		else if(triggerl == 1){ //checking every slot using histogram and canny detection
			histogramChack(tempcompareimage[k], tempc, checkcar[k], temprecordcannyvalue[k]);
		}
		//record draw rectangle color, 0 green color, 1 red color
		if (checkCarSubtract == true) {
			checkcolr[k] = 0;
		}
		else if(checkCarPlus == true){
			checkcolr[k] = 1;
		}
		//according to checkcolr value redraw rectangle
		if (checkcolr[k] == 1) {
			DrawRectFull(imagecheck, tempboox1[k]);
		}
		else {
			DrawRectEmpty(imagecheck, tempboox1[k]);
		}
		//calculate whether or not every slot have car 
		if (checkCarSubtract == true) {
			checkcar[k] -= 1;
			checkCarSubtract = false;
			
		}
		else if (checkCarPlus == true) {
			checkcar[k] += 1;
			checkCarPlus = false;
			
		}
		cvReleaseImage(&tempc);
		cvReleaseImage(&orgimage);
		cvReleaseImage(&tempccopy);
	}
	
}
//histogram chacking every slot
void histogramChack(IplImage* img, IplImage* img1, int ccar, int tecv)
{
	bool dfound = false;
	bool cannyfound = false;
	// Compute HSV image and separate into colors
	IplImage* hsv = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 3 );
	IplImage* hsv1 = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 3 );
	cvCvtColor( img, hsv, CV_RGB2BGR );
	cvCvtColor( img1, hsv1, CV_RGB2BGR );
	
	IplImage* h_plane = cvCreateImage( cvGetSize( img ), 8, 1 );
	IplImage* s_plane = cvCreateImage( cvGetSize( img ), 8, 1 );
	IplImage* v_plane = cvCreateImage( cvGetSize( img ), 8, 1 );
	IplImage* planes[] = { h_plane, s_plane };
	cvCvtPixToPlane( hsv, h_plane, s_plane, v_plane, 0 );
	
	IplImage* h_plane1 = cvCreateImage( cvGetSize( img1 ), 8, 1 );
	IplImage* s_plane1 = cvCreateImage( cvGetSize( img1 ), 8, 1 );
	IplImage* v_plane1 = cvCreateImage( cvGetSize( img1 ), 8, 1 );
	IplImage* planes1[] = { h_plane1, s_plane1 };
	cvCvtPixToPlane( hsv1, h_plane1, s_plane1, v_plane1, 0 );
	
	// Build and fill the histogram
	int h_bins = 30, s_bins = 32;
	CvHistogram* hist;
	{
		int hist_size[] = { h_bins, s_bins };
		float h_ranges[] = { 0, 180 };
		float s_ranges[] = { 0, 255 };
		float* ranges[] = { h_ranges, s_ranges };
		hist = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
	}
	CvHistogram* hist1;
	{
		int hist_size[] = { h_bins, s_bins };
		float h_ranges[] = { 0, 180 };
		float s_ranges[] = { 0, 255 };
		float* ranges[] = { h_ranges, s_ranges };
		hist1 = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
	}
	
	cvCalcHist( planes, hist, 0, 0 ); // Compute histogram
	cvCalcHist( planes1, hist1, 0, 0 );// Compute histogram
	cvNormalizeHist( hist, 1 ); // Normalize it
	cvNormalizeHist( hist1, 1 );// Normalize it
	
	double d = cvCompareHist(hist, hist1, CV_COMP_CORREL);
	printf("%f\n", d);
	
	//calculate number of car in parking
	if (d > 0.7) {//prompt empty 
		dfound = false;
	}
	else if(d <= 0.7){//prompt full
		dfound = true;
	}
	if ((totaldiff - tecv) >= 200) {//checking canny value diff
		cannyfound = true;
	}
	if (dfound && cannyfound) { //when all prompted, often checking by day
		if (ccar == 0) {
			numberOfParking ++;
			checkCarPlus = true;
			checkboolcolor = true;
		}
	}
	else if((!dfound) || (!cannyfound)){//when some prompted, condition for shadow or night
		if (dfound && ccar == 1) {
			if (!cannyfound) {
				numberOfParking --;
				checkCarSubtract = true;
				checkboolcolor = false;
			}
		}
		else if((!dfound)&& ccar == 1){
			numberOfParking --;
			checkCarSubtract = true;
			checkboolcolor = false;
		}
		
	}
	
	cvReleaseImage(&hsv);
	cvReleaseImage(&hsv1);
	
	cvReleaseImage(&h_plane);
	cvReleaseImage(&s_plane);
	cvReleaseImage(&v_plane);
	
	cvReleaseImage(&h_plane1);
	cvReleaseImage(&s_plane1);
	cvReleaseImage(&v_plane1);
	
	cvReleaseHist(&hist);
	cvReleaseHist(&hist1);
}
//create canny detection image 
void cannyCheck(IplImage* img, IplImage* img1)
{
	cvSmooth(img, img, CV_GAUSSIAN, 5, 5, 1.25, 1.25);
	
	IplImage *cannyvalue = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 1);
	
	cvCanny(img, cannyvalue, 100, 25, 3);
	
	cvSmooth(img1, img1, CV_GAUSSIAN, 5, 5, 1.25, 1.25);
	
	IplImage *cannyvalue2 = cvCreateImage(cvGetSize(img1), IPL_DEPTH_8U, 1);
	
	cvCanny(img1, cannyvalue2, 100, 25, 3);
	
	//cvShowImage("cannyplusback", cannyvalue2);
	
	cannyPixelCheck(cannyvalue, cannyvalue2);
	cvReleaseImage(&cannyvalue);
	cvReleaseImage(&cannyvalue2);
}
//checking canny detection image pixel
void cannyPixelCheck(IplImage* img, IplImage* img1)
{
	double s[img->height][img->width];
	double s1[img1->height][img1->width];
	int x;
	int y;
	int x1;
	int y1;
	totaldiff = 0;
	for(x=0; x<img -> height; x++)
	{
		for (y=0; y<img -> width; y++) {
			s[x][y] = cvGetReal2D(img, x, y);
		}
	}
	for(x1=0; x1<img1 -> height; x1++)
	{
		for (y1=0; y1<img1 -> width; y1++) {
			s1[x1][y1] = cvGetReal2D(img1, x1, y1);
		}
	}
	for(x=0; x<img -> height; x++)
	{
		for (y=0; y<img -> width; y++) 
		{
			if (s[x][y] == 255.000000) {
				
				if (s[x][y] != s1[x][y]) {
					//printf("%f\n", s[x][y]);
					totaldiff++;
				}
			}
		}
	}
	printf("%d\n", totaldiff);
}
