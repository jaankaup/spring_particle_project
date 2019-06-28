#include "testData.h"

//VoxelData exampleData1()
//{
//  auto result = VoxelData(3,3,3);
//  result.setValue(1,1,1,0.0f);
//  return result;
//}
//
//VoxelData exampleData2()
//{
//  auto result = VoxelData(4,4,4);
//  result.setValue(1,1,1,0.0f);
//  result.setValue(2,1,1,0.0f);
//  result.setValue(1,2,1,0.0f);
//  result.setValue(2,2,1,0.0f);
//  result.setValue(1,1,2,0.0f);
//  result.setValue(2,1,2,0.0f);
//  result.setValue(1,2,2,0.0f);
//  result.setValue(2,2,2,0.0f);
//  return result;
//}
//
//VoxelData exampleData3()
//{
//  auto result = VoxelData(128,128,128);
//  for (int k=0 ; k<128 ; k++) {
//  for (int j=0 ; j<128 ; j++) {
//  for (int i=0 ; i<128 ; i++) {
//      auto iVal = sin((float)i/12.0f);
//      auto jVal = cos((float)j/12.0f);
////      auto kVal = cos((float)(j+3)/12.0f);
//      result.setValue(i,j,k,(iVal+jVal)/2.0f);   
//  }}};
//
//  return result;
//}
//
///* A simple 16*16*16 data set. */
////std::array<Cell,4096> exampleVoxelDataset1()
//std::vector<Cell> exampleVoxelDataset1()
//{
//  Log::getDebug().log("exampleVoxelDataset1()");
////  Cell result[4096];
//  std::vector<Cell> result;
//  for (int i=0 ; i<3 ; i++) {
//    for (int j=0 ; j<3 ; j++) {
//      for (int k=0 ; k<3 ; k++) {
//        std::array<glm::vec3,8> positions = {{
//          {{glm::vec3((float)i    ,(float)j     ,(float)k)}},       
//          {{glm::vec3((float)(i+1),(float)j     ,(float)k)}},       
//          {{glm::vec3((float)(i+1),(float)j     ,(float)(k+1))}},       
//          {{glm::vec3((float)i    ,(float)j     ,(float)(k+1))}},       
//          {{glm::vec3((float)i    ,(float)(j+1) ,(float)k)}},       
//          {{glm::vec3((float)(i+1),(float)(j+1) ,(float)k)}},       
//          {{glm::vec3((float)(i+1),(float)(j+1) ,(float)(k+1))}},       
//          {{glm::vec3((float)i    ,(float)(j+1) ,(float)(k+1))}}
//        }};
////        glm::ivec3 pos(i,j,k);
//        std::array<float,8> isovalues = {0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,0.0f}; 
//        Cell c(positions,isovalues); // Huonoa koodia. Ehka kopiointia.
//        result.push_back(c);
//        result[i+3*j+3*3*k] = c;
//        Log::getDebug().log("%+%+%=%", std::to_string(i),std::to_string(j),std::to_string(k),std::to_string((i+3*j+3*3*k)));
//      }
//    }
//  }
//  for (int i=0; i<result.size() ; i++)
//  {
//    for (int j=0; j<8 ; j++)
//    {
//      if (result[i].pPositions[j] == glm::vec3(1.0f,1.0f,1.0f))
//      {
//        Log::getDebug().log("result[%].pPositions[%].", std::to_string(i),std::to_string(j));
//        result[i].pIsovalues[j] = 1.0f;
//      }
//      if (result[i].pPositions[j] == glm::vec3(2.0f,1.0f,1.0f))
//      {
//        Log::getDebug().log("result[%].pPositions[%].", std::to_string(i),std::to_string(j));
//        result[i].pIsovalues[j] = 1.0f;
//      }
//      if (result[i].pPositions[j] == glm::vec3(1.0f,1.0f,2.0f))
//      {
//        Log::getDebug().log("result[%].pPositions[%].", std::to_string(i),std::to_string(j));
//        result[i].pIsovalues[j] = 1.0f;
//      }
//      if (result[i].pPositions[j] == glm::vec3(2.0f,1.0f,2.0f))
//      {
//        Log::getDebug().log("result[%].pPositions[%].", std::to_string(i),std::to_string(j));
//        result[i].pIsovalues[j] = 1.0f;
//      }
//      if (result[i].pPositions[j] == glm::vec3(1.0f,2.0f,1.0f))
//      {
//        Log::getDebug().log("result[%].pPositions[%].", std::to_string(i),std::to_string(j));
//        result[i].pIsovalues[j] = 1.0f;
//      }
//      if (result[i].pPositions[j] == glm::vec3(2.0f,2.0f,1.0f))
//      {
//        Log::getDebug().log("result[%].pPositions[%].", std::to_string(i),std::to_string(j));
//        result[i].pIsovalues[j] = 1.0f;
//      }
//      if (result[i].pPositions[j] == glm::vec3(1.0f,2.0f,2.0f))
//      {
//        Log::getDebug().log("result[%].pPositions[%].", std::to_string(i),std::to_string(j));
//        result[i].pIsovalues[j] = 1.0f;
//      }
//      if (result[i].pPositions[j] == glm::vec3(2.0f,2.0f,2.0f))
//      {
//        Log::getDebug().log("result[%].pPositions[%].", std::to_string(i),std::to_string(j));
//        result[i].pIsovalues[j] = 1.0f;
//      }
//    }
//  }
//  result[14].pIsovalues = {1.0f,1.0f,1.0f,1.0f,1.0f,1.0f, 1.0f, 1.0f};
//  return result;
//}
