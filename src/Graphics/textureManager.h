#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

//include <iostream>
#include <vector>
#include <tuple>
#include <stdexcept>
//#include <GL/glew.h>
//#include <SOIL.h>
#include "texture.h"
#include "../Utils/log.h"

class Texture;

class TextureManager
{
  /// Creates and return a reference to the TextureManager object.
  public:
    static TextureManager& getInstance();

    /// Creates a 1D textrure.
    Texture create1D(const std::string& name);

    /// Creates a 2D textrure.
    Texture create2D(const std::string& name);
  
    /// Creates a 3D textrure.
    Texture create3D(const std::string& name);
  
    /// Get texture by its name. throws runtime_expecton if texture is not found.  
    Texture getTextureByName(const std::string& name) const;

    bool deleteTexture(const std::string& name);

  private:
    TextureManager();
    ~TextureManager();

    /// Name:Texture pairs.
    std::vector<std::tuple<std::string, Texture>> pTextures;
};

#endif // TEXTUREMANAGER_H
