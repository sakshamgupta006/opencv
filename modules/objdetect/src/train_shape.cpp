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
#include "opencv2/core.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "train_shape.hpp"
#include <bits/stdc++.h>

using namespace std;
using namespace cv;

namespace cv
{

void KazemiFaceAlignImpl::setCascadeDepth(unsigned int newdepth)
{
    if(newdepth < 0)
    {
        String errmsg = "Invalid Cascade Depth";
        CV_Error(Error::StsBadArg, errmsg);
        return ;
    }
    cascadeDepth = newdepth;
}

//KazemiFaceAlignImpl::~KazemiFaceAlignImpl()

void KazemiFaceAlignImpl::setTreeDepth(unsigned int newdepth)
{
    if(newdepth < 0)
    {
        String errmsg = "Invalid Tree Depth";
        CV_Error(Error::StsBadArg, errmsg);
        return ;
    }
    treeDepth = newdepth;
}

unsigned long KazemiFaceAlignImpl::leftChild(unsigned long idx)
{
    return 2*idx + 1;
}

unsigned long KazemiFaceAlignImpl::rightChild(unsigned long idx)
{
    return 2*idx + 2;
}

//to read the annotation files file of the annotation files
bool KazemiFaceAlignImpl::readAnnotationList(vector<cv::String>& l, string annotation_path_prefix )
{
    string annotationPath = annotation_path_prefix + "*.txt";
    glob(annotationPath,l,false);
    return true;
}

bool KazemiFaceAlignImpl::readnewdataset(vector<cv::String>& l, std::unordered_map<string, vector<Point2f>>& landmarks, string path_prefix)
{
    vector<cv::String> filenames;
    string annotationPath = path_prefix + "*.png";
    string annotationPath2 = path_prefix + "*.pts";
    glob(annotationPath,l,false);
    glob(annotationPath2,filenames,false);
    vector<Point2f> temp;
    string s, tok, randomstring;
    vector<string> coordinates;
    ifstream f;
    for(unsigned long j = 0; j < filenames.size(); j++)
    {
        f.open(filenames[j].c_str(),ios::in);
        if(!f.is_open())
        {
            CV_Error(Error::StsError, "File cannot be opened");
            return false;
        }
        getline(f,randomstring);
        getline(f,randomstring);
        getline(f,randomstring);
        for(int i = 0; i < 68; i++)
        {
            Point2f point;
            getline(f,s);
            stringstream ss(s);
            while(getline(ss, tok,' '))
            {
                coordinates.push_back(tok);
                tok.clear();
            }
            point.x = (float)atof(coordinates[0].c_str());
            point.y = (float)atof(coordinates[1].c_str());
            coordinates.clear();
            temp.push_back(point);
        }
        string v = l[j];
        landmarks[v] = temp;
        temp.clear();
        f.close();
    }
    return true;
}

//read txt files iteratively opening image and its annotations
bool KazemiFaceAlignImpl::readtxt(vector<cv::String>& filepath, std::unordered_map<string, vector<Point2f>>& landmarks, string path_prefix)
{
    //txt file read initiated
    vector<cv::String>::iterator fileiterator = filepath.begin();
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
            stringstream linestream(line);
            string token;
            vector<string> location;
            while(getline(linestream, token,','))
            {
                location.push_back(token);
            }
            landmarks_temp.push_back(Point2f((float)atof(location[0].c_str()),(float)atof(location[1].c_str())));
        }
        file.close();
        landmarks[key] = landmarks_temp;
        //file reading completed
    }
    return true;
}

vector<Rect> KazemiFaceAlignImpl::faceDetector(Mat image,CascadeClassifier& cascade)
{
    vector<Rect> faces;
    int scale = 1;
    Mat gray;
    cvtColor( image, gray, COLOR_BGR2GRAY);
    equalizeHist(gray,gray);
    cascade.detectMultiScale( gray, faces, 1.1, 3, 0 | CASCADE_SCALE_IMAGE, Size(100, 100) );
    numFaces = faces.size();
    return faces;
}

Mat KazemiFaceAlignImpl::getImage(string imgpath, string path_prefix)
{
    return imread(path_prefix + imgpath + ".jpg");
}

bool KazemiFaceAlignImpl::extractPixelValues(trainSample& sample , vector<Point2f>& pixelCoordinates)
{
    vector<Point2f> pixels(pixelCoordinates.size());
    Mat unormmat = unnormalizing_tform(sample.rect[0]);
    for(unsigned long i=0;i<pixelCoordinates.size();i++)
    {
        Mat fiducialPointMat = (Mat_<double>(3,1) << pixelCoordinates[i].x, pixelCoordinates[i].y, 1);
        Mat resultAffineMat = unormmat * fiducialPointMat;
        pixels[i].x = float((resultAffineMat.at<double>(0,0)));
        pixels[i].y = float((resultAffineMat.at<double>(1,0)));
    }
    Mat image = sample.img;
    sample.pixelValues.resize(pixelCoordinates.size());
    if(image.channels() != 1)
        cvtColor(image,image,COLOR_BGR2GRAY);
    for (unsigned int i = 0; i < pixelCoordinates.size(); ++i)
    {
        if(pixels[i].x < image.rows && pixels[i].y < image.cols)
        {
            sample.pixelValues[i] = (int)image.at<uchar>(pixels[i].x, pixels[i].y);
        }
        else
            sample.pixelValues[i] = (int)image.at<uchar>(0,0);
    }
    return true;
}

bool KazemiFaceAlignImpl::calcDiff(vector<Point2f>& input1, vector<Point2f>& input2, vector<Point2f>& output)
{
    output.resize(input1.size());
    for (unsigned long i = 0; i < input1.size(); ++i)
    {
        output[i] = input1[i] - input2[i];
    }
    return true;
}

bool KazemiFaceAlignImpl::calcSum(vector<Point2f>& input1, vector<Point2f>& input2, vector<Point2f>& output)
{
    output.resize(input1.size());
    for (unsigned long i = 0; i < input1.size(); ++i)
    {
        output[i] = input1[i] + input2[i];
    }
    return true;
}

bool KazemiFaceAlignImpl::calcMul(vector<Point2f>& input1, vector<Point2f>& input2, vector<Point2f>& output)
{
    output.resize(input1.size());
    for (unsigned long i = 0; i < input1.size(); ++i)
    {
        output[i].x = input1[i].x * input2[i].x;
        output[i].y = input1[i].y * input2[i].y;
    }
    return true;;
}

bool KazemiFaceAlignImpl::calcMeanShapeBounds()
{
    double meanShapeRectminx, meanShapeRectminy, meanShapeRectmaxx, meanShapeRectmaxy;
    double meanX[meanShape.size()] , meanY[meanShape.size()];
    int pointcount=0;
    for (vector<Point2f>::iterator it = meanShape.begin(); it != meanShape.end(); ++it)
    {
        meanX[pointcount] = (*it).x;
        meanY[pointcount] = (*it).y;
        pointcount++;
    }
    meanShapeRectminx = *min_element(meanX , meanX + meanShape.size());
    meanShapeRectmaxx = *max_element(meanX , meanX + meanShape.size());
    meanShapeRectminy = *min_element(meanY , meanY + meanShape.size());
    meanShapeRectmaxy = *max_element(meanY , meanY + meanShape.size());
    meanShapeBounds.push_back(Point2f(meanShapeRectminx, meanShapeRectminy));
    meanShapeBounds.push_back(Point2f(meanShapeRectmaxx, meanShapeRectmaxy));
    meanShapeReferencePoints[0] = Point2f(meanShapeRectminx, meanShapeRectminy);
    meanShapeReferencePoints[1] = Point2f(meanShapeRectmaxx, meanShapeRectminy);
    meanShapeReferencePoints[2] = Point2f(meanShapeRectminx, meanShapeRectmaxy);
    return true;
}
}