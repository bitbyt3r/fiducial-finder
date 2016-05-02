#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

Mat src; Mat src_gray;
int thresh = 231;
int angle_thresh = 90;
int max_thresh = 255;
RNG rng(12345);

/// Function header
void thresh_callback(int, void* );

/** @function main */
int main( int argc, char** argv )
{
  /// Load source image and convert it to gray
  src = imread( argv[1], 1 );

  /// Convert image to gray and blur it
  cvtColor( src, src_gray, CV_BGR2GRAY );
  blur( src_gray, src_gray, Size(3,3) );

  /// Create Window
  char* source_window = "Source";
  namedWindow( source_window, CV_WINDOW_NORMAL );
  imshow( source_window, src );

//  createTrackbar( " Threshold:", "Source", &angle_thresh, max_thresh, thresh_callback );
  thresh_callback( 0, 0 );
  int key = 0;
  while (key != 27) {
    key = waitKey(0);
  }
  return(0);
}

/** @function thresh_callback */
void thresh_callback(int, void* )
{
  Mat threshold_output;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;

  /// Detect edges using Threshold
  threshold( src_gray, threshold_output, thresh, 255, THRESH_BINARY );
  /// Find contours
  findContours( threshold_output, contours, hierarchy, CV_RETR_TREE, 
CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

  /// Find the rotated rectangles and ellipses for each contour
  vector<RotatedRect> minRect( contours.size() );
  vector<RotatedRect> minEllipse( contours.size() );

  for( int i = 0; i < contours.size(); i++ )
     { minRect[i] = minAreaRect( Mat(contours[i]) );
       if( contours[i].size() > 4 )
         { minEllipse[i] = fitEllipse( Mat(contours[i]) ); }
     }

  /// Draw contours + rotated rects + ellipses
  Mat drawing = Mat::zeros( threshold_output.size(), CV_8UC3 );
  vector<vector<Point> > contours_poly(contours.size());
  int correct = 0;
  int toomany = 0;
  int toofew = 0;
  for( int i = 0; i< contours.size(); i++ )
     {
       approxPolyDP(Mat(contours[i]), contours_poly[i], 2, true);
       vector<double> diff(contours_poly[i].size());
       vector<double> angles(contours_poly[i].size());
       for ( int j = 0; j < contours_poly[i].size(); j = j + 1) {
         Point leg1 = contours_poly[i][j];
         Point vertex = contours_poly[i][(j+1)%contours_poly[i].size()];
         Point leg2 = contours_poly[i][(j+2)%contours_poly[i].size()];
         double numerator = leg1.y*(vertex.x-leg2.x) + vertex.y*(leg2.x-leg1.x) + leg2.y*(leg1.x-vertex.x);
         double denominator = 1 + (leg1.y-vertex.y)*(vertex.y-leg2.y);
         double angle = atan2(numerator, denominator) * 180.0 / CV_PI;
         angle = 180 - abs(angle);
         angles[j] = angle;
       }
       for (int j=0; j<contours_poly[i].size(); j++) {
         diff[j] = angles[(j+1)%contours_poly[i].size()] - angles[j];
       }
       int corners = 0;
       for (int j=1; j<contours_poly[i].size(); j++) {
         if (diff[j] > 0 && diff[j-1] < 0) {
           corners++;
         }
       }
       Scalar color = Scalar( 0, 0, 0);
       if (corners > 4) {
//         color = Scalar( 100, 255, 100 );
         toomany++;
       }
       else if (corners < 2) {
//         color = Scalar( 100, 100, 255);
         toofew++;
       }
       else {
         correct++;
         color = Scalar( 255, 200, 200 );
       }
       if (corners == 0) {
         color = Scalar( 0, 0, 0 );
       }
       if ((minRect[i].size.width / minRect[i].size.height) > 3) {
         color = Scalar( 0, 0, 0 );
       } else if ((minRect[i].size.height / minRect[i].size.width) > 3) {
         color = Scalar( 0, 0, 0 );
       }
       if ((minRect[i].size.height + minRect[i].size.width) < 75) {
         color = Scalar( 0, 0, 0 );
       }
       drawContours( drawing, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
       //ellipse( drawing, minEllipse[i], color, 2, 8 );
     }

  /// Show in a window
  printf("Correct: %d, Too Many: %d, Too Few: %d\n", correct, toomany, toofew);
  namedWindow( "Contours", CV_WINDOW_NORMAL );
  imshow( "Contours", drawing );
}
