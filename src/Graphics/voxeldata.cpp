//#include "voxeldata.h"


//VoxelData::VoxelData()
//{
//
//}
//VoxelData::VoxelData(const int xSize, const int ySize, const int zSize)
//{
//  if (xSize < 1 || ySize <1 || zSize < 1)
//  {
//    Log::getError().log("VoxelData::VoxelData(%,%,%). Arguments must be > 0.",std::to_string(xSize),std::to_string(ySize),std::to_string(zSize));
//    return;
//  }
//    pMaxI = xSize;
//    pMaxJ = ySize;
//    pMaxK = zSize;
////    pIStride = 1;
////    pJStride = xSize;
////    pKStride = xSize;
////    pData[i*pIStride + j*pJStride + k*pKStride];
//
//    // Alustetaan kaikki arvot 1.0f.
//    pData = std::vector<float>(xSize*ySize*zSize,1.0f);
//}
//
//
//VoxelData::~VoxelData()
//{
//
//}

//float VoxelData::getValue(const int i, const int j, const int k) const
//{
//  // Jos viitataan alueen ulkopuolelle, niin palautetaan 1.0f.
//  if (i >= pMaxI) return 1.0f; 
//  if (j >= pMaxJ) return 1.0f;
//  if (k >= pMaxK) return 1.0f;
//  if (i < 0) return 1.0f; 
//  if (j < 0) return 1.0f;
//  if (k < 0) return 1.0f;
//
//  return pData[i + j*pMaxJ + k*pMaxK*pMaxK];
//}
//
//void VoxelData::setValue(const int i, const int j, const int k, float value)
//{
//  if (i >= pMaxI) return; 
//  if (j >= pMaxJ) return;
//  if (k >= pMaxK) return;
//  if (i < 0) return; 
//  if (j < 0) return;
//  if (k < 0) return;
//
//  pData[i + j*pMaxJ + k*pMaxK*pMaxK] = value;
//}

