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
#include <queue>

#define NUM_CLOSEST 5

using namespace cv;

struct KeyPointDistPair {
    KeyPoint pt;
    double distance;
} typedef KeyPointDistPair;

static bool operator <(const KeyPointDistPair& a, const KeyPointDistPair& b) {
    return a.distance < b.distance;
}

static double distance(KeyPoint pt1, KeyPoint pt2) {
    double x = pt1.pt.x - pt2.pt.x;
    double y = pt1.pt.y - pt2.pt.y;
    return sqrt(x*x + y*y);
}

static double findAngle(double a, double b, double c) {
    return acos((b*b + c*c - a*a) / (2*b*c));
}

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
    
    for (int i = 0; i < originalKeypoints.size(); i++) {
        KeyPoint point = originalKeypoints[i];
        // find closest keypoints
        std::priority_queue<KeyPointDistPair> closest;
        
        for (int j = 0; j < originalKeypoints.size(); j++) {
            if (i != j) {
                KeyPointDistPair pair = {
                    originalKeypoints[j],
                    distance(point, originalKeypoints[j])
                };
                
                if (closest.size() < NUM_CLOSEST) {
                    closest.push(pair);
                }
                else if (pair < closest.top()) {
                    closest.pop();
                    closest.push(pair);
                }
            }
        }
        
        std::cout << "-------------- For keypoint " << i << " --------------" << std::endl; 
        
        KeyPointDistPair prev = closest.top();
        closest.pop();
        
        while(!closest.empty()) {
            KeyPointDistPair next = closest.top();
            double a = distance(prev.pt, next.pt);
            double angle = findAngle(a, prev.distance, next.distance);
            std::cout << angle << std::endl;
            closest.pop();
            prev = next;
        }
    }
    
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