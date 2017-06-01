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

using namespace std;
using namespace cv;

string cascadename;

//to read the xml file of the annotation files
static bool readStringList( const string& filename, vector<string>& l, string annotation_path_prefix )
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
static bool readtxt(vector<string> filepath, vector<Point2f>& landmarks, string path_prefix)
{
    //txt file read initiated
    vector<string>::iterator it2 = filepath.begin();
    for (; it2 != filepath.end() ; it2++)
    {
        ifstream file;
        file.open((string)*it2);
        string imgpath,line;
        getline(file,imgpath);
        imgpath.erase(imgpath.length()-1);
        imgpath = path_prefix + imgpath + ".jpg";
        Mat image = imread(imgpath);
        imshow("res",image);
        waitKey(0);
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
            landmarks.push_back(new_point);
        }
        file.close();
        //file reading completed
    }
    return true;
}

int main()
{
        string filename = "list.xml";       // need to be passed as arguments
        vector<string> names;
        vector<Point2f> landmarks;
        string path_prefix = "annotation/";    // need to be passed as arguments
        readStringList(filename, names, path_prefix);
        readtxt(names, landmarks,path_prefix);
return 0;
}