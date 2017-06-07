/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2008-2013, Itseez Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of Itseez Inc. may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the copyright holders or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/



#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <fstream>
#include <iostream>
#include <opencv2/core.hpp>
#include <string>
#include <bits/stdc++.h>
#include "/home/cooper/gsoc/opencv/modules/objdetect/src/train_shape.hpp"



namespace cv{


class KazemiFaceAlign{

public:
    static bool readStringList( const string& filename, vector<string>& l, string annotation_path_prefix );
    static bool getDataFromTxt(vector<string> filepath, std::map<string, vector<Point2f>>& landmarks, string path_prefix);
    static bool extractMeanShape()std::map<string, vector<Point2f>>& landmarks, string path_prefix,CascadeClassifier& cascade;
    static bool faceDetector();

private:
    int numFaces;
    int numLandmarks;
    vector<Point3f> meanShape;

};



//to read the xml file of the annotation files
static bool KazemiFaceAlign::readStringList( const string& filename, vector<string>& l, string annotation_path_prefix )
{
    l.resize(0);
    FileStorage fs(filename, FileStorage::READ);
    if( !fs.isOpened() )
        return false;
    FileNode n = fs.getFirstTopLevelNode();
    if( n.type() != FileNode::SEQ )
        return false;
    FileNodeIterator it = n.begin(), it_end = n.end();
    for( ; it != it_end; ++it )
        {
            string full_path = annotation_path_prefix + (string)*it;
            l.push_back(full_path);
        }
    return true;
}

//read txt files iteratively opening image and its annotations
static bool KazemiFaceAlign::readtxt(vector<string> filepath, std::map<string, vector<Point2f>>& landmarks, string path_prefix)
{
    //txt file read initiated
    vector<string>::iterator fileiterator = filepath.begin();
    for (; fileiterator != filepath.end() ; fileiterator++)
    {
        ifstream file;
        file.open((string)*fileiterator);
        string key,line;
        getline(file,key);
        key.erase(key.length()-1);
        vector<Point2f> landmarks_temp;
        while(getline(file,line))
        {
            int increment_x=0,increment_y=0;
            char x_coord[10],y_cooord[10];
            while(line[increment_x]!=',')
            {
                x_coord[increment_x]=line[increment_x];
                increment_x++;
            }
            string first = string(x_coord);
            increment_x++;
            while(increment_x < line.length())
            {
                y_cooord[increment_y++]=line[increment_x];
                increment_x++;
            }
            string second = string(y_cooord);
            Point2f new_point;
            new_point.x=std::stof(x_coord);
            new_point.y=std::stof(y_cooord);
            landmarks_temp.push_back(new_point);

        }
        file.close();
        landmarks[key] = landmarks_temp;
        //file reading completed
    }
    return true;
}


static bool KazemiFaceAlign::faceDetector()
{

}

static bool KazemiFaceAlign::extractMeanShape(std::map<string, vector<Point2f>>& landmarks, string path_prefix,CascadeClassifier& cascade)
{
    std::map<string, vector<Point2f>>::iterator db_iterator = landmarks.begin(); // random inititalization as
    db_iterator++;
    string random_initial_image = db_iterator->first;                            //any face file can be taken by imagelist creator
    //find the face size dimmensions which will be used to center and scale each database image
    string imgpath;
    imgpath = path_prefix + random_initial_image + ".jpg";
    Mat image = imread(imgpath);
    //apply face detector on the image
    const static Scalar colors[] =
    {
        Scalar(255,0,0),
        Scalar(255,128,0),
        Scalar(255,255,0),
        Scalar(0,255,0),
        Scalar(0,128,255),
        Scalar(0,255,255),
        Scalar(0,0,255),
        Scalar(255,0,255)
    };
    int scale = 1;
    //remove once testing is complete
    vector<Rect> faces;
    Mat gray, smallImg;
    cvtColor( image, gray, COLOR_BGR2GRAY);
    equalizeHist(gray,gray);
    cascade.detectMultiScale( gray, faces,
        1.1, 4, 0
        //|CASCADE_FIND_BIGGEST_OBJECT,
        //|CASCADE_DO_ROUGH_SEARCH
        |CASCADE_SCALE_IMAGE,
        Size(30, 30) );
    cout<<"here"<<endl;
    for ( size_t i = 0; i < faces.size(); i++ )
    {
        Rect r = faces[i];
        Scalar color = colors[i%8];
        double aspect_ratio = (double)r.width/r.height;
        rectangle( image, cvPoint(cvRound(r.x*scale), cvRound(r.y*scale)),
                       cvPoint(cvRound((r.x + r.width-1)*scale), cvRound((r.y + r.height-1)*scale)),
                       color, 3, 8, 0);

    }
    imshow("result",image);
    waitKey(0);
    return true;
}
}
