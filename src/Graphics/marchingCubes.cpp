#include "marchingCubes.h"

extern glm::vec3 interPolateV(float isolevel,const glm::vec3& p1, const glm::vec3& p2,float valp1, float valp2)
{
   float mu;
   glm::vec3 p;

   if (std::abs(isolevel-valp1) < 0.00001)
      return p1;
   if (std::abs(isolevel-valp2) < 0.00001)
      return p2;
   if (std::abs(valp1-valp2) < 0.00001)
      return p1;
   mu = (isolevel - valp1) / (valp2 - valp1);
   p.x = p1.x + mu * (p2.x - p1.x);
   p.y = p1.y + mu * (p2.y - p1.y);
   p.z = p1.z + mu * (p2.z - p1.z);

//   Log::getDebug().log("isolevel = %", std::to_string(isolevel));
//   Log::getDebug().log("p1 = (%,%,%)", std::to_string(vertices[q].x),std::to_string(vertices[q].y),std::to_string(vertices[q].z));
   return p;
}

extern glm::vec3 interPolateN(float isolevel,const glm::vec3& n1, const glm::vec3& n2)
{
   float u;
   glm::vec3 p;

//   if (std::abs(isolevel-valp1) < 0.00001)
//      return p1;
//   if (std::abs(isolevel-valp2) < 0.00001)
//      return p2;
//   if (std::abs(valp1-valp2) < 0.00001)
//      return p1;
//   u = 1.0f - isolevel;
   return isolevel * n1 + (1-isolevel) * n2; 

//   Log::getDebug().log("isolevel = %", std::to_string(isolevel));
//   Log::getDebug().log("p1 = (%,%,%)", std::to_string(vertices[q].x),std::to_string(vertices[q].y),std::to_string(vertices[q].z));
}

//extern glm::vec3 calculateNormal(const int i, const int j, const int k, const VoxelData& data)
//{
//    float gx = data.getValue(i+1  ,j  ,k) - data.getValue(i-1, j, k);
//    float gy = data.getValue(i  ,j+1  ,k) - data.getValue(i, j-1, k);
//    float gz = data.getValue(i  ,j  ,k+1) - data.getValue(i, j, k-1);
////    if (gx == 0.0f && gy == 0.0f && gz == 0.0f)
////    {
////    Log::getDebug().log("ZERO VECTOR!");
////    return glm::vec3(1.0f,1.0f,1.0f);
////    }
//    glm::vec3 v(gx,gy,gz);
////    Log::getDebug().log("v = (%,%,%)", std::to_string(v.x),std::to_string(v.y),std::to_string(v.z));
////    auto pah = glm::vec3(4.0f,3.0f,0.5f);
////    auto n = glm::normalize(glm::vec3(0.5f,0.4f,0.3f));
////    glm::vec3 n = glm::normalize(v);
////    Log::getDebug().log("n = (%,%,%)", std::to_string(n.x),std::to_string(n.y),std::to_string(n.z));
//    return glm::normalize(v);
////    return glm::vec3(1.0f,0.0f,0.0f);
//}
