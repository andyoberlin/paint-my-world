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
    // Instantiate the Feature Detector
    OrbFeatureDetector detector;
    
    std::vector<KeyPoint> originalKeypoints;
    Mat originalKeypointsImage;
    
    // load the image to display
    Mat original = imread( "Original.jpg", CV_LOAD_IMAGE_GRAYSCALE );
    
    if( !original.data ) {
        std::cout <<  "Could not open or find the image" << std::endl;
        return -1;
    }
    
    // detect and draw the keypoints
    detector.detect(original, originalKeypoints);
    
    drawKeypoints(original, originalKeypoints, originalKeypointsImage, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
    
    // resize image to display in window
    Size size = original.size();
    float ratio = 1000*size.height/size.width;
    
    Mat originalResize;
    
    resize(originalKeypointsImage, originalResize, Size(1000, ratio));
    
    // Create a window in which the captured images will be presented
    cvNamedWindow( "OpenCVCamera", CV_WINDOW_AUTOSIZE );
    
    imshow("OpenCVCamera", originalResize );
    
    waitKey(0);
    
    // Release the capture device housekeeping
    cvDestroyWindow( "OpenCVCamera" );
    
    return 0;
}