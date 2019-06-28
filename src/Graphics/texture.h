#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <SOIL.h>
#include "textureManager.h"
#include "textureData.h"
#include "../Utils/log.h"
#include "../Utils/myrandom.h"

//enum textureType : int32_t;
enum class TextureType {d1,d2,d3};

class Texture
{
    /// TextureManager object has access to all Texture member functions.
    /// TextureManager creates and destroys textures objects.
    friend class TextureManager;

    public:

        /// Uses the texture object. Texture object must be initialized 
        /// before the use method. @param unit is the texture unit to use.
        void use(const GLuint unit) const;

        //GLuint getID() const;
        TextureType getTextureType() const;
        void create(const std::string& fileloc);
        void create3D(const TextureData& td);
        void createExample2D();
        void create_tritable_texture();

        /// The destructor doesn't release the texture. Use dispose()
        /// memberfuction to destory texture object.
        ~Texture();

    private:

        /// Handle to the gl texture.
        GLuint pId = 0;

        /// Texture type.
        TextureType pType;

        /// Current texture unit.
        ///GLuint pUnit;

        /// Default constructors.
        Texture();


        /// Initializes the texture object.
        void init(const TextureType t);

        /// Binds the texture. Texture object must be initialized 
        /// before the use method.
        void bind() const;

        /// Releases the texture object.
        void dispose() const;
};

#endif // TEXTURE_H
