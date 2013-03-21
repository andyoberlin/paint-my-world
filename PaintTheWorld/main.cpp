//
//  main.cpp
//  PaintTheWorld
//
//  Created by Andy Oberlin on 3/13/13.
//  Copyright (c) 2013 RememerMe. All rights reserved.
//

// Example showing how to read and write images
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cvaux.hpp>

int main(int argc, char** argv) {
    CvCapture* capture = cvCaptureFromCAM( CV_CAP_ANY );
    if ( !capture ) {
        fprintf( stderr, "ERROR: capture is NULL \n" );
        return -1;
    }
    
    // Create a window in which the captured images will be presented
    cvNamedWindow( "OpenCVCamera", CV_WINDOW_AUTOSIZE );
    
    // Show the image captured from the camera in the window and repeat
    while ( 1 ) {
        // Get one frame
        IplImage* frame = cvQueryFrame( capture );
        if ( !frame ) {
            fprintf( stderr, "ERROR: frame is null...\n" );
            break;
        }
        cvShowImage( "OpenCVCamera", frame );
        // Do not release the frame!
        //If ESC key pressed, Key=0x10001B under OpenCV 0.9.7(linux version),
        //remove higher bits using AND operator
        if ( (cvWaitKey(10) & 255) == 27 ) break;
    }
    
    // Release the capture device housekeeping
    cvReleaseCapture( &capture );
    cvDestroyWindow( "OpenCVCamera" );
    
    return 0;
}
