#ifndef TESTDATA_H
#define TESTDATA_H
#include <iostream>
#include <vector>
#include <array>
#include <memory>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "cell.h"
#include "voxeldata.h"
#include "../Utils/log.h"

static VoxelData<float> exampleData1()
{
  VoxelData<float> result(3,3,3,1.0f);
  result.setValue(1,1,1,0.0f);
  return result;
}

static VoxelData<float> exampleData2()
{
  VoxelData<float> result(4,4,4,1.0f);
  result.setValue(1,1,1,0.0f);
  result.setValue(2,1,1,0.0f);
  result.setValue(1,2,1,0.0f);
  result.setValue(2,2,1,0.0f);
  result.setValue(1,1,2,0.0f);
  result.setValue(2,1,2,0.0f);
  result.setValue(1,2,2,0.0f);
  result.setValue(2,2,2,0.0f);
  return result;
}

static VoxelData<float> exampleData3()
{
  VoxelData<float> result(128,128,128,1.0f);
  for (int k=0 ; k<128 ; k++) {
  for (int j=0 ; j<128 ; j++) {
  for (int i=0 ; i<128 ; i++) {
      auto iVal = sin((float)i/12.0f);
      auto jVal = cos((float)j/12.0f);
//      auto kVal = cos((float)(j+3)/12.0f);
      result.setValue(i,j,k,(iVal+jVal)/2.0f);   
  }}};

  return result;

}

static VoxelData<float> exampleData4()
{
  int dim = 8;
  VoxelData<float> randomData(dim,dim,dim,1.0f);
  for (int i=0; i<dim ; i++) {
  for (int j=0; j<dim ; j++) {
  for (int k=0; k<dim ; k++) {
    randomData.setValue(i,j,k,static_cast<float>(rand())/static_cast<float>(RAND_MAX)*100.0f);
    //randomData.setValue(i,j,k,static_cast<float>(rand()));
  }}};

  int a = -5; 
  int b = 2;

  VoxelData<float> result(dim,dim,dim,1.0f);
  for (int i=0; i<dim ; i++) {
  for (int j=0; j<dim ; j++) {
  for (int k=0; k<dim ; k++) {
    //float randValue = randomData.getValue(i,j,k); 
    float z = exp(a * static_cast<float>(i)/dim) * exp(a * static_cast<float>(j)/dim); 
    float k1 = exp(b * static_cast<float>(i)/dim) * exp(b * static_cast<float>(j)/dim); 
    
    int sampleCount = 1;
    float initialValue = randomData.getValue(i,j,k);
    float gx = 0.0f;
    float gy = 0.0f;
    float gz = 0.0f;
    for (int a=1 ; a<sampleCount+1 ; a++)
    {
      gx += randomData.getValue(i+a ,j   ,k) - randomData.getValue(i-a, j, k);
      gy += randomData.getValue(i   ,j+a ,k) - randomData.getValue(i, j-a, k);
      gz += randomData.getValue(i   ,j   ,k+a) - randomData.getValue(i, j, k-a);
    }
    float neightbours =  ((gx + gy + gz)/3.0f) / sampleCount;
    float finalValue = neightbours + initialValue / 2.0f;
    finalValue = finalValue > 1.0f ? 1.0f : finalValue;
//    result.setValue(i,j,k,(gx+gy+gz)*z*k1);
//    result.setValue(i,j,k,10000000.0f*z*k1);
      result.setValue(i,j,k,finalValue*z*k1);
//      result.setValue(i,j,k,(gx+gy+gz)*k1 * z);
  }}};

//  u = rand (32, 32);
//  t = linspace (-1, 1, 32);
//  [x, y] = meshgrid (t, t);
//  
//  a = 4;
//  z = exp (- a .* x .^ 2) .* exp (- a .* y .^ 2);
//  b = 128;
//  k = exp (- b .* x .^ 2) .* exp (- b .* y .^ 2);
//  w = conv2 (u, k, 'same') .* z;

  return result;

}
//extern std::vector<glm::vec3> createSomething();
//
//extern std::vector<Cell> exampleVoxelDataset1();

#endif // TESTDATA_H
