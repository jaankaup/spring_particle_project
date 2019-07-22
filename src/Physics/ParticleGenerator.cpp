#define GLM_ENABLE_EXPERIMENTAL
#include <math.h>
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include "ParticleGenerator.h"
#include "../Utils/log.h"
#include "../Utils/misc.h"
#include "../Utils/myrandom.h"


int ParticleGenerator::generateGrass(const std::string& name_prefix, const uint32_t grassCount, const uint32_t grassHeight, const uint32_t areaWidth, const uint32_t areaHeight)
{

    std::vector<std::vector<GrassParticle>> all_particles;
    all_particles.reserve(5000);

//    std::vector<GrassParticle> temp;
//    temp.reserve(1000);


    // The dimensions for a single grass.
    static const float rectangle3D_width_min = 0.005;
    static const float rectangle3D_width_max = 0.01;
    static const float rectangle3D_depth = 0.0;
    static const float rectangle3D_height_min = 0.08;
    static const float rectangle3D_height_max = 0.18;
    static const int number_of_points = 1000;
    static const int max_levels = 20;
    static const int max_friends = 10;

    static const int maxHeight = 0.5;
    static const int maxWidth = 0.5;

    /* Large area. */
    MyRandom<float> x_pos;
    x_pos.setDistribution(0.0f,float(areaWidth));

    MyRandom<float> z_pos;
    z_pos.setDistribution(0.0f,float(areaHeight));

    /* A single Grass. */
    
    MyRandom<int> r_level;
    r_level.setDistribution(10,max_levels);

    MyRandom<float> next_x;
    next_x.setDistribution(0.0,areaWidth*0.2f);

    //MyRandom<float> next_y;
    //next_y.setDistribution(0.0,rectangle3D_height*0.5f);

    MyRandom<float> next_z;
    next_z.setDistribution(0.0,areaHeight*0.2f);

    MyRandom<float> next_rotation;;
    next_rotation.setDistribution(0.0f,6.0f);
    /*
     *
     *  level=n                p
     *
     *  ...
     *
     *  ...               p     p   p 
     *
     *  level=1         p    p    p    p
     *
     *  level=0         p    p    p    p
     *
     *
     *
     *         r
     *     p ----> 
     *
     *
     */

    // Counter for particle indices.
    int particle_index = 0;
    int particle_index_sofar = 0;

    int particles_per_level_counter = 0;

    MyRandom<float> next_width;
    next_width.setDistribution(rectangle3D_width_min,rectangle3D_width_max);

    MyRandom<float> next_height;
    next_height.setDistribution(rectangle3D_height_min,rectangle3D_height_max);

    for (int b=0; b<grassCount; b++){

      float rectangle3D_width = next_width();
      //float rectangle3D_depth = 0.0;
      float rectangle3D_height = next_height();
      float rotation_angle = next_rotation();

      int levels = r_level();
      int maximum_vertical_particle_count = ceil(log(levels));
      float radius = float(rectangle3D_width) / float(maximum_vertical_particle_count);

      std::vector<GrassParticle> temp;
      temp.reserve(500);

      std::vector<ParticleLevel> particle_positions;
      particle_positions.reserve(1000);

      for (int i=0; i<levels; i++)
      {
        // how many particles per level. 
        int this_level_count = ceil(log(levels-i)*0.7);
        float start_x = (maximum_vertical_particle_count - this_level_count) * 0.5f * radius;
        float start_y = i * radius;
        int temp_radius = radius;

        // Generate particle positions for this level.
        for (int j=0 ; j<this_level_count ; j++)
        {
          ParticleLevel pl;
          pl.level = i;
          pl.particle_index = particle_index;
          pl.pos = glm::vec3(float(start_x) + radius*j, float(start_y) + radius*j, 0.0f);
          particle_positions.push_back(pl); 
          particle_index++;
        }

        //auto p = std::make_unique<GrassParticle>();
      } // for i=0

//      for (int i=0 ; i<particle_positions.size() ; i++)
//      {
//        //auto ptr = temp_selected[i].get();
//        Log::getInfo().log("PARTICLE:");
//        logGLM("Position: ", particle_positions[i].pos);
//        Log::getInfo().log("Index: %", particle_positions[i].particle_index);
//        Log::getInfo().log("ParticleLevel %", std::to_string(particle_positions[i].level));
//        Log::getInfo().log("");
//      }

      glm::vec4 base_position = glm::vec4(next_x(), 0.0f, next_z(), 0.0f); 
      glm::vec3 base_position_3d = glm::vec3(base_position.x,base_position.y,base_position.z); 
      glm::mat4 unit = glm::mat4(1.0f);
      glm::mat4 translate = glm::translate(unit,base_position_3d);//TODO 
      glm::mat4 rotation = glm::rotate(translate,rotation_angle, glm::vec3(0.0f,1.0f,0.0f));//TODO 
      // Create actual particle.
      for (const auto& p_pos : particle_positions)
      {
        std::vector<ParticleLevel> friends;

        // The actual particle.
        GrassParticle gp;        
        gp.index = p_pos.particle_index;
        gp.pos = glm::vec4(p_pos.pos,1.0f);

        float temp_radius = radius;
        // Friends;
        while (friends.size() < max_friends )
        {
          for (const auto& x : particle_positions)
          {
            // This particle can't be friend with it self.
            if (x.particle_index == p_pos.particle_index) continue;

            bool friend_already_found = false;
            // Check if friend already exists.
            for (const auto& b : friends)
            {
              // Already friends.
              if (x.particle_index == b.particle_index) { friend_already_found = true; break; }
            }
            if (friend_already_found) continue;

            if (glm::distance(x.pos, p_pos.pos) < temp_radius)// (2*radius + 0.002f*levels*radius))
            {
              friends.push_back(x);
            }
          }
          temp_radius = temp_radius + temp_radius * 0.01f;
        } // while

        // No friends. Ignore this particle.
        if (friends.size() == 0) continue;

        // Is this particle static. TODOOO
        gp.other.x = p_pos.particle_index < maximum_vertical_particle_count + particle_index_sofar? -1.0f : 6.0f;
        //gp.other.x = -1.0f;
        gp.vel = glm::vec4(0.0f);

        if (friends.size() > 1)
        {
          auto fr = friends[0];
          gp.friends1.x = float(fr.particle_index);
          gp.rest_lengths.x = glm::distance(p_pos.pos,fr.pos);
        }

        if (friends.size() > 2)
        {
          auto fr = friends[1];
          gp.friends1.y = float(fr.particle_index);
          gp.rest_lengths.y = glm::distance(p_pos.pos,fr.pos);
        }

        if (friends.size() > 3)
        {
          auto fr = friends[2];
          gp.friends1.z = float(fr.particle_index);
          gp.rest_lengths.z = glm::distance(p_pos.pos,fr.pos);
        }

        if (friends.size() > 4)
        {
          auto fr = friends[3];
          gp.friends1.w = float(fr.particle_index);
          gp.rest_lengths.w = glm::distance(p_pos.pos,fr.pos);
        }

        if (friends.size() > 5)
        {
          auto fr = friends[4];
          gp.friends2.x = float(fr.particle_index);
          gp.rest_lengths2.x = glm::distance(p_pos.pos,fr.pos);
        }

        if (friends.size() > 6)
        {
          auto fr = friends[5];
          gp.friends2.y = float(fr.particle_index);
          gp.rest_lengths2.y = glm::distance(p_pos.pos,fr.pos);
        }

        if (friends.size() > 7)
        {
          auto fr = friends[6];
          gp.friends2.z = float(fr.particle_index);
          gp.rest_lengths2.z = glm::distance(p_pos.pos,fr.pos);
        }

        if (friends.size() > 8)
        {
          auto fr = friends[7];
          gp.friends2.w = float(fr.particle_index);
          gp.rest_lengths2.w = glm::distance(p_pos.pos,fr.pos);
        }

        if (friends.size() > 9)
        {
          auto fr = friends[8];
          gp.friends3.x = float(fr.particle_index);
          gp.rest_lengths3.x = glm::distance(p_pos.pos,fr.pos);
        }

        if (friends.size() > 10)
        {
          auto fr = friends[9];
          gp.friends3.y = float(fr.particle_index);
          gp.rest_lengths3.y = glm::distance(p_pos.pos,fr.pos);
        }

        if (friends.size() > 11)
        {
          auto fr = friends[10];
          gp.friends3.z = float(fr.particle_index);
          gp.rest_lengths3.z = glm::distance(p_pos.pos,fr.pos);
        }

        if (friends.size() > 12)
        {
          auto fr = friends[11];
          gp.friends3.w = float(fr.particle_index);
          gp.rest_lengths3.w = glm::distance(p_pos.pos,fr.pos);
        }

        if (friends.size() > 13)
        {
          Log::getInfo().log("Friend count ==  %", std::to_string(friends.size()));
        }
        glm::vec4 new_position = gp.pos + base_position;
        //glm::vec3 to_3d = glm::vec3(new_position.x,new_position.y,new_position.z);
        gp.pos = rotation * (new_position);
        //gp.pos = gp.pos + base_position;
        //gp.pos = gp.pos + base_position;
        temp.push_back(gp);
      } // for i=0
      particle_index_sofar += temp.size();
      all_particles.push_back(temp);
    } // for b=0 All Particles created!

//    for (const auto& x : temp)
//    {
//      Log::getInfo().log("Index: %", std::to_string(x.index));
//      logGLM("Position: ", x.pos);
//      logGLM("Velocity: ", x.vel);
//      logGLM("Other: ", x.other);
//      logGLM("Friends1: ", x.friends1);
//      logGLM("Friends2: ", x.friends2);
//      logGLM("Rest1: ", x.rest_lengths);
//      logGLM("Rest2: ", x.rest_lengths2);
//    }
    
    struct PosVel { glm::vec4 pos; glm::vec4 vel; };
    struct StaticData { glm::vec4 other;
                        glm::vec4 friend1;
                        glm::vec4 friend2;
                        glm::vec4 friend3;
                        glm::vec4 rest1;
                        glm::vec4 rest2;
                        glm::vec4 rest3;};

    auto initial = std::unique_ptr<PosVel>(new PosVel[particle_index+1]);
    auto static_data = std::unique_ptr<StaticData>(new StaticData[particle_index+1]);

    auto initial_ptr = initial.get();
    auto sd_ptr = static_data.get();

    // Create data for opengl buffers.
    for (const auto& a : all_particles) { 
    for (const auto& x : a)
    {
      PosVel pv;
      pv.pos = x.pos;
      pv.vel = x.vel;

      StaticData sd;
      sd.other = x.other;
      sd.friend1 = x.friends1;
      sd.friend2 = x.friends2;
      sd.friend3 = x.friends3;
      sd.rest1 = x.rest_lengths;
      sd.rest2 = x.rest_lengths2;
      sd.rest3 = x.rest_lengths3;

//      Log::getInfo().log("x.index ==  %", std::to_string(x.index));
      initial_ptr[x.index] = pv;  
      sd_ptr[x.index] = sd;  
    }}

    std::string initial_buffer = name_prefix + "_initial";
    std::string static_data_buffer = name_prefix + "_static";
    std::string k1_s = name_prefix + "_k1";
    std::string k2_s = name_prefix + "_k2";
    std::string k3_s = name_prefix + "_k3";
    std::string k4_s = name_prefix + "_k4";

    auto idata = VertexBufferManager::getInstance().create(initial_buffer);
    idata->init();

    auto sdata = VertexBufferManager::getInstance().create(static_data_buffer);
    sdata->init_plain_buffer();

    auto k1 = VertexBufferManager::getInstance().create(k1_s);
    k1->init_plain_buffer();

    auto k2 = VertexBufferManager::getInstance().create(k2_s);
    k2->init_plain_buffer();

    auto k3 = VertexBufferManager::getInstance().create(k3_s);
    k3->init_plain_buffer();

    auto k4 = VertexBufferManager::getInstance().create(k4_s);
    k4->init_plain_buffer();

  auto compute_shader = ShaderManager::getInstance().getByKey(name_prefix + "_shader");
  if (compute_shader == nullptr) compute_shader = ShaderManager::getInstance().create(name_prefix + "_shader");
  std::vector<std::string> src = {"shaders/ruohikko.comp"};
  compute_shader->build(src,false);

  // Taytetaan puskurit. Piirtopuskuri, on samalla partikkeleiden tila.
  std::vector<std::string> types = {"4f","4f"};
  idata->addData(initial_ptr,sizeof(float)*(particle_index+1)*8,types);
  idata->setCount(particle_index+1);

  // Staattisen datan puskuri.
  sdata->populate_data(sd_ptr,sizeof(float)*(particle_index+1)*28);

  // k1-k4 puskurit. Varataan vain tila. Ei tarvitse siirtaa dataa.
  k1->populate_data(nullptr,sizeof(float)*(particle_index+1)*4);
  k2->populate_data(nullptr,sizeof(float)*(particle_index+1)*4);
  k3->populate_data(nullptr,sizeof(float)*(particle_index+1)*4);
  k4->populate_data(nullptr,sizeof(float)*(particle_index+1)*4);

  return particle_index+1;
}
