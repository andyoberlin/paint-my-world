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
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/core/core.hpp>

using namespace cv;

int main(int argc, char** argv) {
    IplImage* original = cvLoadImage( "Original.jpg", CV_LOAD_IMAGE_GRAYSCALE );
    
    if( !original ) {
        std::cout <<  "Could not open or find the image" << std::endl;
        return -1;
    }
    
    float ratio = 500*original->height/original->width;
    
    IplImage* originalResize = cvCreateImage(cvSize(500 , (int)ratio), original->depth, original->nChannels );
    
    cvResize(original, originalResize);
    
    // Create a window in which the captured images will be presented
    cvNamedWindow( "OpenCVCamera", CV_WINDOW_AUTOSIZE );
    
    cvShowImage("OpenCVCamera", originalResize );
    
    waitKey(0);
    
    // Release the capture device housekeeping
    cvDestroyWindow( "OpenCVCamera" );
    
    return 0;
}