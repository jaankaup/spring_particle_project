#include <stdio.h>
#include <SDL2/SDL.h>
//#include <SDL2/SDL_image.h>
#include <GL/glew.h>

#ifdef EMSCRIPTEN
  #include <emscripten.h>
#endif

#include <math.h>
#include <vector>

#include "Graphics/window.h"
#include "Graphics/shader.h"
#include "Graphics/model.h"
#include "Graphics/vertexbuffer.h"
//#include "Graphics/vertexAttributes.h"
#include "Graphics/renderer.h"
#include "Graphics/texture.h"
//#include "Graphics/testData.h"
//#include "Graphics/marchingCubes.h"
//#include "Graphics/voxeldata.h"
#include "Graphics/camera.h"
#include "Graphics/manager.h"
#include "Graphics/model.h"
#include "Graphics/programstate.h"
//#include "Graphics/timer.h"
//#include "Graphics/vertexBufferManager.h"
#include "Graphics/modelManager.h"
#include "Utils/log.h"
#include "Utils/misc.h"
//#include "Utils/myrandom.h"

/**
 * Context structure that will be passed to the loop handler
 */
struct context
{
    Renderer renderer;
    Camera camera = Camera(glm::vec3(10.0f,10.0f,20.0f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,1.0f,0.0f));
};

void createShaders()
{
    
// WE OMIT THE DEBUG SHADERS FOR NOW. 
  
//    const std::string MARCHING_CUBES_SHADER = "marchingShader_green"; 
//    const std::string MARCHING_CUBES_WIREFRAME = "marchingShaderWire_green"; 
    const std::string MARCHING_CUBES_TRIANGULATION = "triangulationShader"; 
    const std::string SCENE_SHADER = "cubeshader"; 
    const std::string PARTICLE1 = "particle1"; 

//    // The marching cubes shader.
//    Shader* marchingShader = ShaderManager::getInstance().createShader(MARCHING_CUBES_SHADER);
//    std::vector<std::string> marchingShader_src = {"shaders/marching.vert", "shaders/marching_green_thing.geom", "shaders/marching.frag"};
//    marchingShader->build(marchingShader_src);
//
//    ProgramState::getInstance().getMetadata()->cubeMarchShader = MARCHING_CUBES_SHADER;
//
//    // The marching cubes wireframe shader.
//    Shader* marchingShaderLine = ShaderManager::getInstance().createShader(MARCHING_CUBES_WIREFRAME);
//    std::vector<std::string> marchingShaderLine_src = {"shaders/marching.vert", "shaders/marchingWireFrame_green.geom", "shaders/marchingLine2.frag"};
//    marchingShaderLine->build(marchingShaderLine_src);
//
//    ProgramState::getInstance().getMetadata()->cubeMarchWireframe = MARCHING_CUBES_WIREFRAME;

    // The triangulation shader with density function 0-9 .
//////    for (int i=0 ; i < 10 ; i++)
//////    {
//////      Shader* triangulationShader = ShaderManager::getInstance().create(MARCHING_CUBES_TRIANGULATION + std::to_string(i));
//////      std::vector<std::string> triangulate_src = {"shaders/triangulate.vert", "shaders/triangulate.geom", "shaders/densityFunction" + std::to_string(i) + ".df"};
//////      triangulationShader->setFeedback(true,"outputCase");
//////      triangulationShader->buildDensity(triangulate_src);
//////    }
//////
//////    ProgramState::getInstance().getMetadata()->triangulationShader = MARCHING_CUBES_TRIANGULATION + std::to_string(1);
    ProgramState::getInstance().getMetadata()->particle1 = PARTICLE1;
//////
    Shader* sParticle1 = ShaderManager::getInstance().create(PARTICLE1);
    std::vector<std::string> srcKipinat = {"shaders/jouset.comp"};
    sParticle1->build(srcKipinat,false);

    // The shader for drawing the triangulated scene. The name is a bit
    // misleading.
    //Shader* shaderCube = ShaderManager::getInstance().create(SCENE_SHADER);
    //std::vector<std::string> shaderSourcesCube = {"shaders/default_notex.vert", "shaders/default_notex.frag"};
    //shaderCube->build(shaderSourcesCube,false);
    Shader* shaderCube = ShaderManager::getInstance().create(SCENE_SHADER);
    std::vector<std::string> shaderSourcesCube = {"shaders/defaultPoint.vert", "shaders/defaultPoint.frag"};
    shaderCube->build(shaderSourcesCube,false);

    const static auto getLeft = [](const int index, const int width, const int height)
    {
      if (index % width == 0) return -1;  
      return index - 1;
    };

    const static auto getRight = [](const int index, const int width, const int height)
    {
      if (index % width == width-1) return -1;  
      return index + 1;
    };

    const static auto getUp = [](const int index, const int width, const int height)
    {
      if (index - width < 0) return -1;  
      return index - width;
    };
    
    const static auto getBottom = [](const int index, const int width, const int height)
    {
      if (index + width > width*height - 1) return -1;  
      return index + width;
    };

    const static auto getUpLeft = [](const int index, const int width, const int height)
    {
      if (getLeft(index,width,height) == -1 || getUp(index,width,height) == -1) return -1;
      return index - width - 1;
    };

    const static auto getUpRight = [](const int index, const int width, const int height)
    {
      if (getRight(index,width,height) == -1 || getUp(index,width,height) == -1) return -1;
      return index - width + 1;
    };

    const static auto getBottomLeft = [](const int index, const int width, const int height)
    {
      if (getBottom(index,width,height) == -1 || getLeft(index,width,height) == -1) return -1;
      return index + width - 1;
    };

    const static auto getBottomRight = [](const int index, const int width, const int height)
    {
      if (getBottom(index,width,height) == -1 || getRight(index,width,height) == -1) return -1;
      return index + width + 1;
    };

    ProgramState::getInstance().getMetadata()->meshShader = SCENE_SHADER;
    ProgramState::getInstance().getMetadata()->baseState = "BState";
    ProgramState::getInstance().getMetadata()->T1 = "T1";
    ProgramState::getInstance().getMetadata()->T2 = "T2";
    ProgramState::getInstance().getMetadata()->T3 = "T3";
    ProgramState::getInstance().getMetadata()->T4 = "T4";

    const int width = 90;
    const int height = 90;
    int particle_count = width*height;
    float offset = 0.1f;
    glm::vec3 a = glm::vec3(0.0f);
    glm::vec3 b = glm::vec3(offset,offset,0.0f);
    float dist = glm::distance(a,b);
    Log::getInfo().log("Distance = % ", std::to_string(dist));
    //float dist = 0.5f;
    Shader* compute = ShaderManager::getInstance().getByKey("particle1");
    compute->bind();
    compute->setUniform("r",dist);
    compute->setUniform("k",100.0f);

    ProgramState::getInstance().setParticleCount(particle_count);
    ProgramState::getInstance().setParticlesWidth(width);
    ProgramState::getInstance().setParticlesHeight(height);

    //Vertexbuffer* bstate = VertexBufferManager::getInstance().createParticleBuffer("BState");
    ParticleBuffer* bstate = dynamic_cast<ParticleBuffer*>(VertexBufferManager::getInstance().createParticleBuffer("BState"));
    bstate->init();
//    std::vector<std::string> types = {"4f","4f","4f"}; // HUOM, "4f","4f","4f,"4f" ei toimi!!!!
    //GLfloat* feedback = new GLfloat[8];
    //auto array = new GLfloat[8]{0.0f,0.0f,0.0f,0.0f,0.0f,7.2f,0.0f,0.0f};

    // pos-vel
    auto array = new GLfloat[8*particle_count];

    // other-friends-rest
    auto static_data = new GLfloat[12*particle_count];

    // Luodaan data.
    for (int j=0 ; j<width ; j++) {
    for (int i=0 ; i<height ; i++) {
      int position = i + width * j;
      int pah = position*8; 
      int static_pah = position*12; 


      // Positio, tehdaan verho.
      array[pah] = (i%width)*offset; 
      array[pah+1] = (-1.0f)*j*offset; 
      array[pah+2] = 0.0f; 
      array[pah+3] = 1.0f; 

      // Alku velocity
      array[pah+4] = 0.0f; 
      array[pah+5] = 0.0f; 
      array[pah+6] = 0.0f; 
      array[pah+7] = 0.0f; 

      Log::getInfo().log("(%) pos = (%,%,%,%)", std::to_string(position),
                                                std::to_string(array[pah]),
                                                std::to_string(array[pah+1]),
                                                std::to_string(array[pah+2]),
                                                std::to_string(array[pah+3]));
      Log::getInfo().log("(%) vel = (%,%,%,%)", std::to_string(position),
                                                std::to_string(array[pah+4]),
                                                std::to_string(array[pah+5]),
                                                std::to_string(array[pah+6]),
                                                std::to_string(array[pah+7]));

      // Muu data.

      // Jos on ylhaalla, niin asetetaan arvoksi -1.0f, sovitaan etta on
      // staatinen partikkeli. Debuggaus mielessa muuten 6.0f.
      static_data[static_pah] = getUp(position,width,height) == -1 ? -1.0f : 6.0f; 
      static_data[static_pah+1] = 0.0f; // Tanne jotain muuta informaatiota jos keksii.  
      static_data[static_pah+2] = 0.0f; // Tanne jotain muuta informaatiota jos keksii.
      static_data[static_pah+3] = 0.0f; // Tanne jotain muuta informaatiota jos keksii.

      // Kavereiden indeksit. Flex-rakenne.
      float ur = float(getUpRight(position,width,height)); 
      float ul = float(getUpLeft(position,width,height)); 
      float br = float(getBottomRight(position,width,height)); 
      float bl = float(getBottomLeft(position,width,height)); 
      static_data[static_pah+4] = ur; 
      static_data[static_pah+5] = ul;  
      static_data[static_pah+6] = br;
      static_data[static_pah+7] = bl;

      // Tanne lepoarvoja kavereihin. Nyt on kaikilla sama, mutta jos olisi
      // erilainen jousirakenne, niin tanne voisi laittaa muitakin arvoja.
      static_data[static_pah+8]  = dist;
      static_data[static_pah+9] = dist;  
      static_data[static_pah+10] = dist;
      static_data[static_pah+11] = dist;
      Log::getInfo().log("(%) other  = (%,%,%,%)", std::to_string(position),
                                                   std::to_string(static_data[static_pah]),
                                                   std::to_string(static_data[static_pah+1]),
                                                   std::to_string(static_data[static_pah+2]),
                                                   std::to_string(static_data[static_pah+3]));
      Log::getInfo().log("(%) friends = (%,%,%,%)", std::to_string(position),
                                                   std::to_string(static_data[static_pah+4]),
                                                   std::to_string(static_data[static_pah+5]),
                                                   std::to_string(static_data[static_pah+6]),
                                                   std::to_string(static_data[static_pah+7]));
      Log::getInfo().log("(%) rest_length = (%,%,%,%)", std::to_string(position),
                                                   std::to_string(static_data[static_pah+8]),
                                                   std::to_string(static_data[static_pah+9]),
                                                   std::to_string(static_data[static_pah+10]),
                                                   std::to_string(static_data[static_pah+11]));
    }}; 
    std::vector<std::string> types2 = {"4f","4f"};
    //bstate->addData(array, sizeof(float)*particle_count, types2);
    bstate->novoeha(array, static_data, sizeof(float)*particle_count, types2);
    bstate->setCount(particle_count);
    delete[] array;
    delete[] static_data;
/////      int position = i + width * j;
///////      Log::getInfo().log("Position = % ", std::to_string(position));
/////      int pah = position*16; 
///////      Log::getInfo().log("PAH = %", std::to_string(pah));
/////      array[pah] = (i%width)*offset; 
/////      array[pah+1] = (-1.0f)*j*offset; 
/////      array[pah+2] = 0.0f; 
/////      //array[pah+3] = getUp(position,width,height) == -1 ? 2.0f : 0.0f; 
/////      array[pah+3] = position == 0 || position == width -1 ? 2.0f : 0.0f; 
///////      Log::getInfo().log("STATIC (%) = %", std::to_string(position), std::to_string(array[pah+3]));
/////
/////      array[pah+4] = 0.0f; 
/////      array[pah+5] = 0.0f; 
/////      array[pah+6] = 0.0f; 
/////      array[pah+7] = 0.0f; 
/////
///////      if (j == 0 && i == 0) array[i+8] = width + 1.0f;
///////      if (j == 0 && i == width - 1) array[i+8] = 2*width - 2.0f;
///////      if (j == height-1 && i == 0) array[i+8] = (j-1)*width + 1.0f;
///////      if (j == height-1 && i == width - 1) array[i+8] = j*width - 2.0f;
///////
///////      if (j == 0 && i == 0) array[i+8] = width + 1.0f;
///////      if (j == 0 && i == width - 1) array[i+8] = 2*width - 2.0f;
/////      float ur = float(getUpRight(position,width,height)); 
/////      float ul = float(getUpLeft(position,width,height)); 
/////      float br = float(getBottomRight(position,width,height)); 
/////      float bl = float(getBottomLeft(position,width,height)); 
/////      array[pah+8] =  ur; 
/////      array[pah+9] =  ul;  
/////      array[pah+10] = br;
/////      array[pah+11] = bl;
///////      Log::getInfo().log("(%) = (%,%,%,%)", std::to_string(position),
///////                                            std::to_string(ur),std::to_string(ul),
///////                                            std::to_string(br),std::to_string(bl));
///////      Log::getInfo().log("Dist = %", std::to_string(dist));
/////      array[pah+12] = ur == -1.0 ? 0.0f : dist; 
/////      array[pah+13] = ul == -1.0 ? 0.0f : dist; 
/////      array[pah+14] = br == -1.0 ? 0.0f : dist; 
/////      array[pah+15] = bl == -1.0 ? 0.0f : dist; 
/////    }
/////    Log::getInfo().log("Ei koredumppaile1!");
/////    std::vector<std::string> types2 = {"4f","4f","4f","4f"};
/////    Log::getInfo().log("Ei koredumppaile2!");
/////    bstate->addData(array, sizeof(float)*16*particle_count, types2);
/////    Log::getInfo().log("Ei koredumppaile3!");
/////    bstate->setCount(16*particle_count);
/////    Log::getInfo().log("Ei koredumppaile4!");
/////    Log::getInfo().log("Ei koredumppaile5!");
}

// Initialize the marching cubes attributes.
void initializeCubeAttributes()
{
  const int BLOCK_SIZE = 16;
  const int CUBE_COUNT_X = BLOCK_SIZE;
  const int CUBE_COUNT_Y = BLOCK_SIZE;
  const int CUBE_COUNT_Z = BLOCK_SIZE;
  const float ISO_VALUE = 0.0f;

  auto metadata = ProgramState::getInstance().getMetadata();
  metadata->block_size = BLOCK_SIZE; 
  metadata->cube_count_x = CUBE_COUNT_X;
  metadata->cube_count_y = CUBE_COUNT_Y;
  metadata->cube_count_z = CUBE_COUNT_Z;
  metadata->isovalue = ISO_VALUE;
}

void createtextures()
{
  const std::string TEXTURE_NAME = "greenThingTexture"; 
  const std::string TRITABLE_NAME = "tri_table_texture"; 

  auto metadata = ProgramState::getInstance().getMetadata();

  // Create the 3D texture.
  // TODO::: MODIFY ::: TODO
  //Texture tex3D = TextureManager::getInstance().create3D(TEXTURE_NAME);
  Texture* tex3D = TextureManager::getInstance().create(TEXTURE_NAME,TextureType::d3);
  auto tex3D_data = createPerlin3D(256,256,256);
  tex3D->create3D(tex3D_data);
  metadata->texture3Dname = TEXTURE_NAME;

  // Create the tri_table.
  // TODO::: MODIFY ::: TODO
  //Texture tritable = TextureManager::getInstance().create1D(TRITABLE_NAME);
  Texture* tritable = TextureManager::getInstance().create(TRITABLE_NAME,TextureType::d1);
  tritable->create_tritable_texture();
  metadata->tri_table_name = TRITABLE_NAME;
}

void loop_handler2(void *arg)
{
    context* c = static_cast<context*>(arg);
    SDL_Event e;

    ProgramState::getInstance().updateTick();
    while (SDL_PollEvent(&e))
    {
        switch (e.type)
        {
        case SDL_QUIT:
            ProgramState::getInstance().setAppRunning(false);
            break;

        case SDL_KEYDOWN:
            switch (e.key.keysym.sym)
            {
                case SDLK_1:
                    changeScene(1);
                    break;
                case SDLK_2:
                    changeScene(2);
                    break;
                case SDLK_3:
                    changeScene(3);
                    break;
                case SDLK_4:
                    changeScene(4);
                    break;
                case SDLK_5:
                    changeScene(5);
                    break;
                case SDLK_6:
                    changeScene(6);
                    break;
                case SDLK_7:
                    changeScene(7);
                    break;
                case SDLK_8:
                    changeScene(8);
                    break;
                case SDLK_9:
                    changeScene(9);
                    break;
                case SDLK_0:
                    changeScene(0);
                    break;
                case SDLK_KP_PLUS:
                    {
                      auto blockSize = ProgramState::getInstance().getMetadata()->block_size;
                      if (blockSize != 66)
                      {
                        ProgramState::getInstance().getMetadata()->block_size = blockSize + 2;
                        Log::getInfo().log("Block size: %", std::to_string(blockSize + 2));
                      }
                    break;
                    }
                case SDLK_KP_MINUS:
                    {
                      auto blockSize = ProgramState::getInstance().getMetadata()->block_size;
                      if (blockSize != 2)
                      {
                        ProgramState::getInstance().getMetadata()->block_size = blockSize - 2;
                        Log::getInfo().log("Block size: %", std::to_string(blockSize - 2));
                      }
                      break;
                    }
//                case SDLK_SPACE:
//                    camera_.toggleMode();
//                    break;

                case SDLK_q: 
                    ProgramState::getInstance().setAppRunning(false);
                    break;
                case SDLK_ESCAPE: 
                    ProgramState::getInstance().setAppRunning(false);
                    break;
                case SDLK_t:
                    {
                    auto metadata = ProgramState::getInstance().getMetadata();
                    auto name = metadata->texture3Dname;
                    // TODO::: MODIFY ::: TODO
                    //TextureManager::getInstance().deleteTexture(name); 
                    TextureManager::getInstance().del(name); 
                    // TODO::: MODIFY ::: TODO
                    //Texture tex3D = TextureManager::getInstance().create3D(name);
                    Texture* tex3D = TextureManager::getInstance().create(name,TextureType::d3);
                    auto tex3D_data = createPerlin3D(256,256,256);
                    tex3D->create3D(tex3D_data);
                    metadata->texture3Dname = name;
                    Log::getInfo().log("Creating a new 3D texture...");
                    // Recreate scene model.
                    Log::getInfo().log("Rebuilding scene...");
                    ModelManager::getInstance().createSceneObject();
                    break;
                    }
                case SDLK_y: 
                    {
                    auto metadata = ProgramState::getInstance().getMetadata();
                    auto name = metadata->texture3Dname;
                    // TODO::: MODIFY ::: TODO
                    //TextureManager::getInstance().deleteTexture(name); 
                    TextureManager::getInstance().del(name); 
                    // TODO::: MODIFY ::: TODO
                    //Texture tex3D = TextureManager::getInstance().create3D(name);
                    Texture* tex3D = TextureManager::getInstance().create(name,TextureType::d3);
                    auto tex3D_data = createPerlin3D_rough(256,256,256);
                    tex3D->create3D(tex3D_data);
                    metadata->texture3Dname = name;
                    Log::getInfo().log("Creating a new 3D texture with more rought noise...");
                    // Recreate scene model.
                    Log::getInfo().log("Rebuilding scene...");
                    ModelManager::getInstance().createSceneObject();
                    break;
                    }
            }
        case SDL_WINDOWEVENT:
            switch(e.window.event)
            {
              case SDL_WINDOWEVENT_CLOSE:
                    ProgramState::getInstance().setAppRunning(false);
                  break;
              case SDL_WINDOWEVENT_RESIZED:
              {
                  int w = e.window.data1;
                  int h = e.window.data2;
                  glViewport(0,0,w,h);
                  break;
              }
            }

        /* Camera. */
        case SDL_MOUSEMOTION:
        case SDL_MOUSEBUTTONDOWN:
            c->camera.handleMouseInput(e);
            break;

        }
    }
    c->camera.handleKeyInput();
    //c->renderer.renderModels(c->camera);
    c->renderer.renderKipinat(c->camera);
    Window::getInstance().swapBuffers();
}

//int main()
int main(int argc, char* argv[])
{

  // The program state must be created first.
  ProgramState::getInstance();
  
  // Initialize the base information for the marching cubes.
  initializeCubeAttributes();

  // Create the window. This also creates the opengl context.
  Window window = Window::getInstance();

  // Create all textures.
  //createtextures();

  // Creates a default texture for rendering the cube.
  // TODO::: MODIFY ::: TODO
  //Texture textureCube = TextureManager::getInstance().create2D("cubeTexture");
  //Texture* textureCube = TextureManager::getInstance().create("cubeTexture",TextureType::d2);

  // Context creation. (webassembly).
  context c;

  // We create marching cubes shader only with native opengl.
    
  #ifndef EMSCRIPTEN

  createShaders();

  #endif

  // Initialize the renderer.
  c.renderer.init();


    //ModelManager::getInstance().createSceneObject();
    //Timer::getInstance().reset();

    /**
     * Schedule the main loop handler to get 
     * called on each animation frame
     */

    #ifdef EMSCRIPTEN
    emscripten_set_main_loop_arg(loop_handler2, &c, -1, 1);
    #endif

    #ifndef EMSCRIPTEN
    while (ProgramState::getInstance().getAppRunning())
    {
      loop_handler2(&c);
    }
    #endif

    return 0;
}

