#ifndef MANAGER_H
#define MANAGER_H

#include <vector>
#include <iostream>
//#include <unordered_map>
#include <stdexcept>
#include <memory>
#include "../Utils/log.h"
#include "shader.h"
#include "texture.h"
//#include "../Utils/misc.h"


/* Forward declarations. */
class Shader;
class Texture;

/***************************************************************************************** 
 *                                                                                       *  
 *  Key-value struct.                                                                    *  
 *                                                                                       *  
 ****************************************************************************************/

template<typename T>
class Element {
  public:
    std::string key;
    std::unique_ptr<T> val;

    Element(const std::string& key, std::unique_ptr<T> val) : key{key}, val{std::move(val)} {}
};

/****************************************************************************************
 *                                                                                      *  
 * A base (singleton) class for storing key/value pairs. T is the type of the stored    *
 * values.                                                                              *
 *                                                                                      *  
 ****************************************************************************************/

template<typename T>
class Manager
{

  public:

    /// Copying and moving Manager is forbidden (sigleton class).
    /// Does children derive these???
    Manager(const Manager&) = delete;
    Manager(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;

    /// Creates and return a reference to the singleton Manager object.
    static auto& getInstance();

    /// Create resource (default).
    virtual T* create(const std::string& key);

    /// Get the resource using a string key. Return pointer to the 
    /// created resource. Otherwise returns nullptr.
    virtual T* getByKey(const std::string& key) const;

    /// Deletes an element with a string key. Returns false if deletion fails. 
    /// Otherwise return true.
    virtual bool del(const std::string& key);
  
    /// For some reason the destructor must be public.
    virtual ~Manager() {};

  protected:

    /// Default constructor.
    Manager() {};

  private:

    std::vector<Element<T>> pData;
};

template<typename T>
inline auto& Manager<T>::getInstance()
{
  static Manager instance;
  return instance;
}

template<typename T>
inline T* Manager<T>::create(const std::string& key)
{
  /* Check if the already existst. */
  if (getByKey(key) != nullptr)
  {
    // TODO: log to debug.
    return nullptr; 
  }

  Element<T> e(key,std::unique_ptr<T>(new T()));
  auto ret_val = e.val.get();
  pData.push_back(std::move(e));
  return ret_val;
}

template<typename T>
inline T* Manager<T>::getByKey(const std::string& key) const
{
  for (const auto& e : pData)
  {
    if (e.key == key)
    {
      //Log::getInfo().log("getByKey: %",key);
      //e.val.get()->bind();
      return e.val.get();
    }
  }
  Log::getInfo().log("Ei loytynyt shaderia %", key);
  return nullptr;
}

template<typename T>
inline bool Manager<T>::del(const std::string& key)
{
  for (int i=0; i<pData.size() ; i++)
  {
    if (pData[i].key == key)
    {
      pData.erase(pData.begin() + i);
      return true;
    }
  }
  return false;
}

/****************************************************************************************
 *                                                                                      *  
 * Shader manager                                                                       *
 *                                                                                      *  
 ****************************************************************************************/

class ShaderManager : public Manager<Shader>
{
  public:

    // Get the singleton instance of this class.
    static auto& getInstance() { static ShaderManager instance; return instance; }

};

/****************************************************************************************
 *                                                                                      *  
 * Texture manager                                                                      *
 *                                                                                      *  
 ****************************************************************************************/

class TextureManager : public Manager<Texture>
{

  public:

    // Get the singleton instance of this class.
    static auto& getInstance() { static TextureManager instance; return instance; }

      //Log::getInfo().log("getByKey: %",key);
    Texture* create(const std::string& key) override { Log::getInfo().log("TextureManager::create(): not implemented."); return nullptr; }

    // MUISTA: init(texture:type)
    //T* create(const std::string& key, )

};

#endif // MODELMANAGER_H
