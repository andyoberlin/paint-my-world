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
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/core/core.hpp>
#include <queue>

#define NUM_KEYPOINTS 500
#define NUM_CLOSEST 5

using namespace cv;

int mouseDown = 0;

struct Signature {
    KeyPoint pt;
    double* descriptor;
} typedef Signature;

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

static void getSignatures(vector<KeyPoint> keypoints, Signature signatures[NUM_KEYPOINTS]) {
    for (int i = 0; i < keypoints.size(); i++) {
        KeyPoint point = keypoints[i];
        // find closest keypoints
        std::priority_queue<KeyPointDistPair> closest;
        
        for (int j = 0; j < keypoints.size(); j++) {
            if (i != j) {
                KeyPointDistPair pair = {
                    keypoints[j],
                    distance(point, keypoints[j])
                };
                
                if (closest.size() <= NUM_CLOSEST) {
                    closest.push(pair);
                }
                else if (pair < closest.top()) {
                    closest.pop();
                    closest.push(pair);
                }
            }
        }
        
        KeyPointDistPair prev = closest.top();
        closest.pop();
        
        double* descriptor = new double[NUM_CLOSEST];
        
        int k = 0;
        
        while(!closest.empty()) {
            KeyPointDistPair next = closest.top();
            double a = distance(prev.pt, next.pt);
            double angle = findAngle(a, prev.distance, next.distance);
            descriptor[k] = angle;
            closest.pop();
            prev = next;
            k++;
        }
        
        Signature sig = {
            point,
            descriptor
        };
        
        signatures[i] = sig;
    }
}

static void drawCircle( Mat* img, Point center ) {
    int thickness = -1;
    int lineType = 8;
    
    circle(*img,
        center,
        5.0,
        Scalar( 0, 0, 255 ),
        thickness,
        lineType
    );
}

static void onClick(int event, int x, int y, int flags, void* param) {
    Mat* image = (Mat*) param;
    switch(event) {
        case CV_EVENT_LBUTTONDOWN:
            drawCircle(image, Point(x, y));
            imshow("OpenCVCamera", *image);
            mouseDown = 1;
            break;
            
        case CV_EVENT_LBUTTONUP:
            mouseDown = 0;
            break;
            
        case CV_EVENT_MOUSEMOVE:
            if (mouseDown) {
                drawCircle(image, Point(x, y));
                imshow("OpenCVCamera", *image);
            }
            break;
    }
}

int main(int argc, char** argv) {
    // Instantiate the Feature Detector
    SiftFeatureDetector detector(NUM_KEYPOINTS);
    
    /*
    ---------------------------------------------------------------------
     
        Load original image and detect keypoints and get signatures
     
    ---------------------------------------------------------------------
    */
    
    vector<KeyPoint> originalKeypoints;
    Mat originalKeypointsImage;
    
    // load the image to display
    Mat original = imread( "Original.jpg", CV_LOAD_IMAGE_GRAYSCALE );
    
    if( !original.data ) {
        std::cout <<  "Could not open or find the image" << std::endl;
        return -1;
    }
    
    // resize image to display in window
    Size size = original.size();
    float ratio = 1000*size.height/size.width;
    
    Mat originalResize;
    
    //resize(original, originalResize, Size(1000, ratio));
    
    // Create a window in which the captured images will be presented
    cvNamedWindow( "OpenCVCamera", CV_WINDOW_AUTOSIZE );
    
    //imshow("OpenCVCamera", originalResize );
    
    // detect and draw the keypoints
    detector.detect(original, originalKeypoints);
    
    //Signature originalSignatures[NUM_KEYPOINTS];
    
    //getSignatures(originalKeypoints, originalSignatures);
    
    SiftDescriptorExtractor extractor;
    Mat originalDescriptors;
    
    extractor.compute(original, originalKeypoints, originalDescriptors);
    
    if (originalDescriptors.type() != CV_32F) {
        originalDescriptors.convertTo(originalDescriptors, CV_32F);
    }
    
    /*
     --------------------------------------------------------------------------------------
     
     Enter drawing phase: The user draws on the image and we map the drawing to keypoints
     
     --------------------------------------------------------------------------------------
    
    cvSetMouseCallback("OpenCVCamera", onClick, (void*) &originalResize);
    
    waitKey(0);
    */
     
    /*
     ---------------------------------------------------------------------
     
     Load translated image and detect keypoints and get signatures
     
     ---------------------------------------------------------------------
     */
    
    vector<KeyPoint> translateKeypoints;
    Mat translateKeypointsImage;
    
    // load the image to display
    Mat translate = imread( "Translate.jpg", CV_LOAD_IMAGE_GRAYSCALE );
    
    if( !translate.data ) {
        std::cout <<  "Could not open or find the image" << std::endl;
        return -1;
    }
    
    // detect and draw the keypoints
    detector.detect(translate, translateKeypoints);
    
    //Signature translateSignatures[NUM_KEYPOINTS];
    
    //getSignatures(translateKeypoints, translateSignatures);
    
    Mat translateDescriptors;
    
    extractor.compute(translate, translateKeypoints, translateDescriptors);
    
    if (translateDescriptors.type() != CV_32F) {
        translateDescriptors.convertTo(translateDescriptors, CV_32F);
    }
    
    FlannBasedMatcher matcher;
    std::vector< DMatch > matches;
    matcher.match( originalDescriptors, translateDescriptors, matches);
    
    double max_dist = 0; double min_dist = 200;
    
    //-- Quick calculation of max and min distances between keypoints
    for( int i = 0; i < originalDescriptors.rows; i++ ) {
        double dist = matches[i].distance;
        if( dist < min_dist ) min_dist = dist;
        if( dist > max_dist ) max_dist = dist;
    }
    
    std::vector<DMatch> goodMatches;
    
    for (int i = 0; i < originalDescriptors.rows; i++ ) { 
        if (matches[i].distance < 2*min_dist) {
            goodMatches.push_back( matches[i]); 
        }
    }

    //-- Draw only "good" matches
    Mat imgMatches;
    drawMatches( original, originalKeypoints, translate, translateKeypoints,
                goodMatches, imgMatches, Scalar::all(-1), Scalar::all(-1),
                vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
    
    //-- Show detected matches
    resize(imgMatches, originalResize, Size(1000, ratio));
    
    imshow( "OpenCVCamera", originalResize );
    
    /*
     ---------------------------------------------------------------------------
     
     Compare signatures and figure out if we are looking at something similar
     
     ---------------------------------------------------------------------------
     */
    waitKey(0);
    
    
    /*
    for (int i = 0; i < NUM_KEYPOINTS; i++) {
        delete[] originalSignatures[i].descriptor;
        delete[] translateSignatures[i].descriptor;
    }
    */
    //drawKeypoints(original, originalKeypoints, originalKeypointsImage, Scalar::all(-1), DrawMatchesFlags::DEFAULT);
    
    // Release the capture device housekeeping
    cvDestroyWindow( "OpenCVCamera" );
    
    return 0;
}