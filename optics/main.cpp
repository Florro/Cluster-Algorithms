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

//#include "opencv2/objdetect/objdetect.hpp"
//#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/imgproc/imgproc.hpp"
//#include <opencv2/opencv.hpp>

//using namespace cv;

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
  
  //std::string path = "/home/florian/CXX/ClusterAlgorithms/densityBasedClusterings/testimg.csv";
  //Mat img = imread( (char*)path.c_str(), IMREAD_GRAYSCALE );
  //std::cout << img.rows << " one " << img.cols << std::endl;
  
  //std::vector < float > data;
   //cv::namedWindow( "pic" );
  //cv::resize(img, img, cv::Size(512,512));
  //cv::imshow( "pic", img );
  //cv::waitKey(0);
  /*
  std::cout << img.rows << " two " << img.cols << std::endl;
  unsigned int dim = img.cols;


  for ( int i = 0; i < img.rows; i++ )
  {
    for ( int j = 0; j < img.cols; j++ ) 
    {
      float x = img.at<float>(i,j);
      data.push_back(x/(10.e12));     
      //std::cout << i  << " " << j << " " << x/(10.e12) << std::endl;// img.at(i,j) << std::endl;
    }
  }
  */

  
  
  // Load Data and calculate Distmatrix
  std::string dataPath = "/home/florian/CXX/ClusterAlgorithms/densityBasedClusterings/testimg_trans.csv";//"../../tsne_3d.csv";// "../positions.csv";////;//"/home/florian/Data/data/otto/standard/holdout/holdout_traindata.csv";
  std::vector< float > data;
  unsigned int dim;
  loadData(data, dataPath, dim, true);
  
  std::cout << "Datasize: " << data.size()/dim << " Dim: " << dim << std::endl;
  std::vector< float > distMat = calcDistances(data, dim);
  //real_t* distMat = NULL;

  
  int minPts = 10;
  float epsilon = 0.8;//std::numeric_limits<float>::max();//2;
 
  float sum = 0;
  int it = 0;
  for ( auto & i: distMat )
  {
    sum += i;
    it++;
  }
  std::cout << "mean dist: " << sum/it << std::endl;
 
    
  /*
  //for( int s = 0; s < 2; s++ ){
    // Run DBSCAN
    clusterAlg::DBSCAN* dbscan_test = new clusterAlg::DBSCAN(&data[0], &distMat[0], data.size()/dim, dim, minPts, epsilon);
    
    dbscan_test->showInfo();
    
    dbscan_test->run();
    
    dbscan_test->showClustering();
    
    dbscan_test->showInfo();
    
    //for( auto &xx : dbscan_test->getClusterIDs() )
    //{
    //  std::cout << xx << std::endl;
    //}
    
    delete dbscan_test;
    std::cout << "----------------------------------------------------------" << std::endl;

  //}
*/
  

  
  
  for( int s = 0; s < 1; s++ ){
   
    //clusterAlg::MiniOPTICS* op = new clusterAlg::MiniOPTICS(&data[0], NULL, data.size()/dim, dim, epsilon);
    clusterAlg::OPTICS* op = new clusterAlg::OPTICS(&data[0], NULL, data.size()/dim, dim, minPts, epsilon);

    //op->setStartIdx(101);
    //op->showInfo();
    
    op->run();
       
    op->updateClusterIDs(1.2);
    
    //op->showClustering();

    //std::vector< int > l = op->getClusterIDs();
    //for ( auto & k : l ) {  std::cout << k << std::endl;}
    //op->showInfo();
    //op->showInfo();

    //std::vector< real_t > dists = op->getClusterOrderDistances();
    //for ( auto & k : dists ) {  std::cout << k << std::endl;}
    
    //std::vector< int > dists = op->getClusterIDs();
    //for ( auto & k : dists ) {  std::cout << k << std::endl;}
    
    std::vector< int > ddd = op->getLabels<int>();
    for ( auto & k : ddd ) {  std::cout << k << std::endl;}

    //std::cout << "----------------------------------------------------------" << std::endl;
    delete op; op = NULL;
  }



  //printf("System Success!\n");
  return 0;
  
}