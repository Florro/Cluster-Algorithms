#include <iostream>
#include <cstdio>

#include "algorithm/optics.h"
#include "algorithm/minioptics.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <numeric>
#include <cassert>

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/opencv.hpp>

using namespace cv;

void loadData(std::vector< float > & data, std::string dataPath, unsigned int &dim, bool header)
{
    std::string line;
    std::ifstream dataStream ((char*)dataPath.c_str());
    assert(dataStream);

    if(header) dataStream >> line;

    while( dataStream >> line)
    {

        std::string x;
        std::istringstream lineStream (line);

        dim = 0;
        while(lineStream){
            if (!getline( lineStream, x, ',' )) break;
            data.push_back(atof(x.c_str()));
            dim++;
        }
    }

    dataStream.close();
}

float calcDist(const real_t* x1, const real_t* x2, unsigned int size)
{
    if ( x1 == x2 ){
        throw std::range_error("x1 == x2 in DBSCAN::calcDist(x1, x2)!");
    }
    real_t dist = 0.0;
    for ( unsigned int i = 0; i < size; i++ )
    {
        dist += (x1[i]- x2[i]) * (x1[i] - x2[i]);
    }
    return sqrt( dist );
}

std::vector< float > calcDistances(std::vector< float > &data, unsigned int dim)
{
    int dataSize = data.size()/dim;

    //std::cout << "Datasize222: " << data.size() << " dataSize: " << dataSize << std::endl;

    std::vector< float > distMat(dataSize*dataSize);

    for(int i = 0; i < dataSize; i++)
    {
        for(int j = i + 1; j < dataSize; j++)
        {
            try {
                distMat[i * dataSize + j] = calcDist(&data[i * dim], &data[j * dim], dim);
            }
            catch ( std::invalid_argument & e ){ std::cout << e.what() << std::endl; }
        } 
    }
    return distMat;
}

int main(void){

    std::string path = "/home/florian/Data/data/noaa-whales/w_7489/w_1.jpg";
    Mat img = imread( (char*)path.c_str(), IMREAD_GRAYSCALE );
    //std::cout << img.rows << " one " << img.cols << std::endl;

    std::vector < float > data;
    //cv::namedWindow( "pic" );
    cv::resize(img, img, cv::Size(100,100));
    //cv::imshow( "pic", img );
    //cv::waitKey(0);

    std::cout << img.rows << " two " << img.cols << std::endl;
    unsigned int dim = 3;//img.cols;

    for ( int i = 0; i < img.rows; i++ )
    {
        for ( int j = 0; j < img.cols; j++ ) 
        {
//            Point3_<uchar>* p = img.ptr< Point3_<uchar> >(j,i);
          //  std::cout << p->x << " " << p->y << " "<< p->z << std::endl;
            //float x = img.at<Vec3b>(i,j)[0];
            data.push_back(float(img.at<Vec3b>(i,j)[2]));     
            data.push_back(float(img.at<Vec3b>(i,j)[1]));     
            data.push_back(float(img.at<Vec3b>(i,j)[0]));
	        
            //std::cout << i  << " " << j << " " << x/(10.e12) << std::endl;// img.at(i,j) << std::endl;
        }
    }
    std::cout << data.size()/3 << std::endl;
    //std::cin.get();
/*
    // Load Data and calculate Distmatrix
    std::string dataPath = "/home/florian/CXX/ClusterAlgorithms/densityBasedClusterings/testimg_trans.csv";//"../../tsne_3d.csv";// "../positions.csv";////;//"/home/florian/Data/data/otto/standard/holdout/holdout_traindata.csv";
    std::vector< float > data;
    unsigned int dim;
    loadData(data, dataPath, dim, true);

    std::cout << "Datasize: " << data.size()/dim << " Dim: " << dim << std::endl;
    std::vector< float > distMat = calcDistances(data, dim);
    //real_t* distMat = NULL;
*/
    int minPts = 10;
    float epsilon = 5;//std::numeric_limits<float>::max();//2;
/*
    std::vector< float > distMat = calcDistances(data, dim);
    
    float sum = 0.0;
    int it = 0;
    for (auto & i : distMat ) {
      sum += i;
      it++;
    }
    std::cout << "mean " << sum/it << std::endl;
    
*/
    for( int s = 0; s < 1; s++ ){

        //clusterAlg::MiniOPTICS* op = new clusterAlg::MiniOPTICS(&data[0], NULL, data.size()/dim, dim, epsilon);
        clusterAlg::OPTICS* op = new clusterAlg::OPTICS(&data[0], NULL, data.size()/dim, dim, minPts, epsilon);

        //op->setStartIdx(101);
        op->showInfo();

        op->run();

        op->updateClusterIDs(5);

        op->showClustering();

        //std::vector< int > l = op->getClusterIDs();
        //for ( auto & k : l ) {  std::cout << k << std::endl;}
        //op->showInfo();
        op->showInfo();

        //std::vector< real_t > dists = op->getClusterOrderDistances();
        //for ( auto & k : dists ) {  std::cout << k << std::endl;}

        //std::vector< int > dists = op->getClusterIDs();
        //for ( auto & k : dists ) {  std::cout << k << std::endl;}

        std::vector< int > ddd = op->getLabels<int>();
        //for ( auto & k : ddd ) {  std::cout << k << std::endl;} 
        
        unsigned char bits[data.size()];
        for ( unsigned int k = 0; k < data.size(); k++ ) {
	    if ( data[k] == -999 ) {
	      bits[k] = char(0);
	    }
	    else {
	      bits[k] = char(data[k] + 100);
	    }
	    
	}
	Mat m(img.rows, img.cols, CV_8UC1, bits);
	cv::namedWindow( "pic" );
	cv::resize(m, m, cv::Size(300,300));
	cv::imshow( "pic", m );
	cv::waitKey(0);
        
        //std::cout << "----------------------------------------------------------" << std::endl;
        delete op; op = NULL;
    }



    printf("System Success!\n");
    return 0;

}
