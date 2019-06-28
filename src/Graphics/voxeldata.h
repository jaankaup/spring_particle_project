#ifndef VOXELDATA_H
#define VOXELDATA_H

#include <vector>
#include <string>
#include <tuple>
#include "../Utils/log.h"

//typedef struct Slice {
//  int x;  
//}
   

template <typename T>
class VoxelData
{
	public:
		VoxelData();
		VoxelData(const int xSize, const int ySize, const int zSize, const T maxvalue); 
		~VoxelData();

    T getValue(const int i, const int j, const int k) const;
    void setValue(const int i, const int j, const int k, T value) ;

    std::tuple<int,int,int> getDimensions() const;

	private:
    int pMaxI = 0;
    int pMaxJ = 0;
    int pMaxK = 0;
    std::vector<T> pData;
    T pMaxvalue;
          
};

template<typename T>
//inline VoxelData<T>() {}
inline VoxelData<T>::VoxelData() {}

template<typename T>
//inline VoxelData<T>(const int xSize, const int ySize, const int zSize) {
inline VoxelData<T>::VoxelData(const int xSize, const int ySize, const int zSize, const T maxvalue) {
      pMaxvalue = maxvalue;
      if (xSize < 1 || ySize <1 || zSize < 1)
      {
        Log::getError().log("VoxelData::VoxelData(%,%,%). Arguments must be > 0.",std::to_string(xSize),std::to_string(ySize),std::to_string(zSize));
        return;
      }
        pMaxI = xSize;
        pMaxJ = ySize;
        pMaxK = zSize;
    
        pData = std::vector<T>(xSize*ySize*zSize,pMaxvalue);
}

template<typename T>
//inline ~VoxelData<T>() {}
inline VoxelData<T>::~VoxelData() {}

// TODO: erikoista.
template<typename T>
inline T VoxelData<T>::getValue(const int i, const int j, const int k) const
{
  // Jos viitataan alueen ulkopuolelle, niin palautetaan 1.0f.
  if (i >= pMaxI) return pMaxvalue; 
  if (j >= pMaxJ) return pMaxvalue;
  if (k >= pMaxK) return pMaxvalue;
  if (i < 0) return pMaxvalue; 
  if (j < 0) return pMaxvalue;
  if (k < 0) return pMaxvalue;

  return pData[i + j*pMaxJ + k*pMaxK*pMaxK];
}

template<typename T>
inline void VoxelData<T>::setValue(const int i, const int j, const int k, T value)
{
  if (i >= pMaxI) return; 
  if (j >= pMaxJ) return;
  if (k >= pMaxK) return;
  if (i < 0) return; 
  if (j < 0) return;
  if (k < 0) return;

  pData[i + j*pMaxJ + k*pMaxK*pMaxK] = value;
}

template<typename T>
//inline std::tuple<int,int,int> VoxelData<T>::getDimensions() const
inline std::tuple<int,int,int> VoxelData<T>::getDimensions() const
{
   return std::make_tuple(pMaxI,pMaxJ,pMaxK);
}

#endif // VOXELDATA_H
