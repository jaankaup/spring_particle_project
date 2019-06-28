#include "textureData.h"
// UBSan

TextureData::TextureData(const int dataSize, const int width, const int height, const int depth)
{
  if (dataSize < 0)
  {
    Log::getError().log("TextureData::TextureData(data,%,%,%,%).",std::to_string(dataSize),std::to_string(width),std::to_string(height),std::to_string(depth));
    Log::getError().log("TextureData::TextureData: the dataSize < 0.");
    return;
  }

  if (! (setWidth(width) && setHeight(height) && setDepth(depth)) )
  {
    Log::getError().log("TextureData::TextureData(data,%,%,%,%).",std::to_string(dataSize),std::to_string(width),std::to_string(height),std::to_string(depth));
    Log::getError().log("TextureData::TextureData: the state of TextureData object is undefined. ",std::to_string(dataSize),std::to_string(width),std::to_string(height),std::to_string(depth));
  } 
  else
  {
    pSize = dataSize;
    pData = std::make_unique<uint8_t[]>(dataSize);   
  }
}

TextureData::~TextureData()
{

}

//const std::vector<unsigned char>& TextureData::getData() const
uint8_t* TextureData::getData() const
{
  //return *pData;
  return pData.get();
}


bool TextureData::setWidth(const int w)
{
  int result;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &result); 
//  Log::getDebug().log("TextureData::setWidth: result = %.", std::to_string(result));
  if (w > result)
  {
    Log::getError().log("TextureData::setWidth(%): GL_MAX_TEXTURE_SIZE == %.", std::to_string(w),std::to_string(result));
    return false;
  }
  pWidth = w;
  return true;
}

bool TextureData::setHeight(const int h)
{

  int result;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &result);
//  Log::getDebug().log("TextureData::setHeight: result = %.", std::to_string(result));
  if (h > result)
  {
    Log::getError().log("TextureData::setHeight(%): GL_MAX_TEXTURE_SIZE == %.", std::to_string(h),std::to_string(result));
    return false;
  }
  pHeight = h;
  return true;
}

bool TextureData::setDepth(const int d)
{
  int result;
  glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &result); 
//  Log::getDebug().log("TextureData::setDepth: result = %.", std::to_string(result));
  if (d > result)
  {
    Log::getError().log("TextureData::setDepth(%): GL_MAX_TEXTURE_SIZE == %.", std::to_string(d),std::to_string(result));
    return false;
  }
  pDepth = d;
  return true;
}

int TextureData::getWidth() const
{
  return pWidth;
}

int TextureData::getHeight() const
{
  return pHeight;
}

int TextureData::getDepth() const
{
  return pDepth;
}

int TextureData::getDataSize() const
{
  return pSize;
}
