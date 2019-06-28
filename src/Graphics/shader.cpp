#include <fstream>
#include "shader.h"
#include "../Utils/log.h"


Shader::Shader()
{
}

Shader::~Shader()
{
}

void Shader::dispose() const
{
  //Log::getDebug().log("Deleting shader program: %", std::to_string(pId));
  if (pId != 0) glDeleteProgram(pId);
}

void Shader::init()
{
  pId = glCreateProgram();

  //Log::getDebug().log("Created shader program: %", std::to_string(pId));

  if (pId == 0)
  {
    Log::getError().log("%","Shader::Shader. Failed to create program.");
  }
}

void Shader::setFeedback(const bool feedback, const std::string& feedbackVarying)
{
  pFeedback = feedback;
  pFeedbackVarying = feedbackVarying;
}

void Shader::buildDensity(const std::vector<std::string>& sources)
{
  std::string density;
  std::string geom;
  std::string vert;


  // This is a copy paste function. TODO: move it to the misc.cpp.
  static const auto endsWith = [](const std::string& str, const std::string& postFix)
  {
    if (str.length() >= postFix.length()) {
      return (0 == str.compare(str.length() - postFix.length(), postFix.length(), postFix));
    } else {
      return false;
    } 
  };

  static const auto replace = [](std::string& str, const std::string& from, const std::string& to)
  {
     size_t start_pos = str.find(from);
    if (start_pos == std::string::npos) return false;
    str.replace(start_pos, from.length(), to);
    return true;
  };

  for (const auto& src : sources)
  {
    if (endsWith(src,".geom")) geom = loadSource(src);
    else if (endsWith(src,".df")) density = loadSource(src);
    else if (endsWith(src,".vert")) vert = loadSource(src);
  }

  static const std::string regText = R"([\w|\W]*voxels_per_block:(\d+)[\w|\W]*)";
  static const std::regex reg_vpb(regText);

  static const std::string regDim = R"([\w|\W]*SceneDimension:(\-?\d+),(\-?\d+),(\-?\d+),(\-?\d+),(\-?\d+),(\-?\d+)[\w|\W]*)";
  static const std::regex reg_dim(regDim);

   auto metadata = ProgramState::getInstance().getMetadata(); 


   int voxs_per_b;  

  // Find the voxels_per_block from the density file.
  std::smatch rex_result;
  if (regex_match(density, rex_result, reg_vpb))
  {
    std::string s_num = rex_result[1];
    
    voxs_per_b = atoi(s_num.c_str()); 
  }

  // Find the dimension of the scene.
  std::smatch rex_result_dim;
  if (regex_match(density, rex_result_dim, reg_dim))
  {
    std::string xMin = rex_result_dim[1];
    std::string yMin = rex_result_dim[2];
    std::string zMin = rex_result_dim[3];
    std::string xMax = rex_result_dim[4];
    std::string yMax = rex_result_dim[5];
    std::string zMax = rex_result_dim[6];
//    metadata->dimensionX_min = atoi(xMin.c_str()); 
//    metadata->dimensionY_min = atoi(yMin.c_str()); 
//    metadata->dimensionZ_min = atoi(zMin.c_str()); 
//    metadata->dimensionX_max = atoi(xMax.c_str()); 
//    metadata->dimensionY_max = atoi(yMax.c_str()); 
//    metadata->dimensionZ_max = atoi(zMax.c_str()); 
    Log::getDebug().log("% % % % % %",
                        std::to_string(atoi(xMin.c_str())),
                        std::to_string(atoi(yMin.c_str())),
                        std::to_string(atoi(zMin.c_str())),
                        std::to_string(atoi(xMax.c_str())),
                        std::to_string(atoi(yMax.c_str())),
                        std::to_string(atoi(zMax.c_str()))
        );
    metadata->dimensionsPerDF.push_back(std::make_tuple(atoi(xMin.c_str()),
                                                        atoi(yMin.c_str()),
                                                        atoi(zMin.c_str()),
                                                        atoi(xMax.c_str()),
                                                        atoi(yMax.c_str()),
                                                        atoi(zMax.c_str()),
                                                        voxs_per_b));
                             
  }

  replace(geom, "{{density_function_comes_here_from_another_file}}", density); 
  
  std::vector<std::string> triangulate_src = {vert, geom};
  build(triangulate_src, true);

}

void Shader::build(const std::vector<std::string>& sources, const bool triangulate)
{
    using ShaderObjData = struct{
        GLenum shaderType;
        GLuint shaderObj;
        std::string sourceCode;
    };

    /* A typedef for shader object data: shader object type and source code. */
    using SOD = std::vector<ShaderObjData>;

    /* A container for shader objects type and source code. */
    SOD sod;

    /* We iterate trought all source codes and extract shader object types and sourcecodes. */
    // This is stupid, but I'm running out of time! TODO: Fix this.
    if (!triangulate)
    {
      for (const auto& location : sources)
      {
          //Log::getDebug().log("Creating shader from file: %", location);

          /* Solve the type of shader object from file extension. */
          GLenum shaderType = getShaderType(location);
          if (shaderType == 0)
          {
              Log::getError().log("Shader::build: Could'n solve the shader object type from file '%'.",location);
              Log::getError().log("Shader::build: file extension must be one the following: [%].",".vert .gtcs .gtes .geom .frag");
              // TODO: exception
          }

          ShaderObjData shaderObjectData;
          shaderObjectData.shaderType = shaderType;
          shaderObjectData.shaderObj = glCreateShader(shaderType);
          shaderObjectData.sourceCode = loadSource(location);
          /* Put the source type and source code to sod. */
          sod.push_back(shaderObjectData);
      }
    }
    else 
    {
          ShaderObjData shaderObjectData;
          shaderObjectData.shaderType = GL_VERTEX_SHADER;
          shaderObjectData.shaderObj = glCreateShader(GL_VERTEX_SHADER);
          shaderObjectData.sourceCode = sources[0]; // Really hacky stuff!
          /* Put the source type and source code to sod. */
          sod.push_back(shaderObjectData);
       
          ShaderObjData shaderObjectData2;
          shaderObjectData2.shaderType = GL_GEOMETRY_SHADER;
          shaderObjectData2.shaderObj = glCreateShader(GL_GEOMETRY_SHADER);
          shaderObjectData2.sourceCode = sources[1]; // Really hacky stuff!!!!!
          /* Put the source type and source code to sod. */
          sod.push_back(shaderObjectData2);
    }

    for (const auto& object : sod)
    {
        compileShader(object.shaderObj, object.sourceCode);
        if (!checkShader(object.shaderObj))
            {
            Log::getError().log("Shader::build: couldn't build shader object from source:\n%",object.sourceCode);
            std::runtime_error("Shader::build: An error occurred while compiling a shader object.");
            }
    }

    /* Attach shader objects to the program. */
    for (const auto& object : sod)
    {
        glAttachShader(pId, object.shaderObj);
//        Log::getDebug().log("Shader type is geometry: %",object.shaderType == GL_GEOMETRY_SHADER);
    }
    if (pFeedback)
    {
      const GLchar* feedbackVaryings[] = {"fPosIn","fNormalIn"}; //pFeedbackVarying.c_str()}; 
      //Log::getDebug().log("feedback varying: %",feedbackVaryings[0]);
      //Log::getDebug().log("pId: %",std::to_string(pId));
      glTransformFeedbackVaryings(pId, 2, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);
    }

    GLenum pname = GL_LINK_STATUS;

    //Log::getDebug().log("Linking shader.");

    glLinkProgram(pId);

    //Log::getDebug().log("Linked shader succeed.");

    int status;
    int errorLength;

    //Log::getDebug().log("Checking shader for errors.");

    glGetProgramiv(pId, pname, &status);

    if (status == GL_FALSE)
    {
        glGetProgramiv(pId, GL_INFO_LOG_LENGTH, &errorLength);
        GLchar *errorMessage = new GLchar[errorLength];
        glGetProgramInfoLog(pId, errorLength, NULL, errorMessage);
        Log::getError().log("Shader program creation failed: %", errorMessage);
        delete[] errorMessage;
    }

    //Log::getDebug().log("Shader program created succesfully.");
}

void Shader::bind() const
{
//  Log::getDebug().log("Binding shader %.",std::to_string(pId));
  glUseProgram(pId);
}

void Shader::setUniform(const std::string& name, int value) const
{
    GLint loc = glGetUniformLocation(pId, name.c_str());
    glUniform1i(loc, value);
}

void Shader::setUniform(const std::string& name, float value) const
{
    GLint loc = glGetUniformLocation(pId, name.c_str());
    glUniform1f(loc, value);
}

void Shader::setUniform(const std::string& name, const glm::vec3& value) const
{
    GLint loc = glGetUniformLocation(pId, name.c_str());
    glUniform3fv(loc, 1,  glm::value_ptr(value));
}

void Shader::setUniform(const std::string& name, const glm::mat3& value) const
{
    GLint loc = glGetUniformLocation(pId, name.c_str());
    glUniformMatrix3fv(loc, 1, GL_FALSE,  glm::value_ptr(value));
}

void Shader::setUniform(const std::string& name, const glm::vec4& value) const
{
    GLint loc = glGetUniformLocation(pId, name.c_str());
    glUniform4fv(loc, 1,  glm::value_ptr(value));
}

void Shader::setUniform(const std::string& name, const glm::mat4& value) const
{
    GLint loc = glGetUniformLocation(pId, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE,  glm::value_ptr(value));
}

std::string Shader::loadSource(const std::string& fileLoc)
{
    std::ifstream file(fileLoc);

    if (!file.is_open())
    {
      Log::getError().log("Shader::loadSource(%): Failed to load source.","fileLoc");
      throw std::runtime_error("Shader::loadSource: counld'n open file '" + fileLoc + "'.");
    }

    std::string str;
    file.seekg(0, std::ios::end);
    str.reserve(file.tellg());
    file.seekg(0, std::ios::beg);
    str.assign((std::istreambuf_iterator<char>(file)),
               std::istreambuf_iterator<char>());

    return str;
}


GLuint Shader::getShaderType(const std::string& fileLoc)
{
  static const auto endsWith = [](const std::string& str, const std::string& postFix)
  {
    if (str.length() >= postFix.length()) {
      return (0 == str.compare(str.length() - postFix.length(), postFix.length(), postFix));
    } else {
      return false;
    } 
  };


    /* Supported shader object source code types. */
    static std::array<std::string, 5> fileSuffixes = {".vert",
                                                      ".gtcs",
                                                      ".gtes",
                                                      ".geom",
                                                      ".frag"
//                                                      ".comp"
                                                     };

    /* Corrresponding shader object types in opengl. */
    static std::array<GLuint, 5> GLStageEnums = {GL_VERTEX_SHADER,
                                                 GL_TESS_CONTROL_SHADER,
                                                 GL_TESS_EVALUATION_SHADER,
                                                 GL_GEOMETRY_SHADER, // 0x8dd9,
                                                 GL_FRAGMENT_SHADER
                                                 //GL_COMPUTE_SHADER
                                                };

    for (unsigned int i=0 ; i<fileSuffixes.size() ; i++)
    {
        if (endsWith(fileLoc,fileSuffixes[i])) return GLStageEnums[i];
    }
    Log::getError().log("Shader::getShaderType(%): Failed to determine the shader type.",fileLoc);
    return 0;
}

void Shader::compileShader(const GLuint id, const std::string& source)
{
    //Log::getDebug().log("Shader::compileShaer(): Compiling shader: %", std::to_string(id));
    // Transform c++ string to C string.
    const char* src_C = source.c_str();
    // Push source to shader object managed by opengl.
    glShaderSource(id, 1, &src_C, NULL);
    // return shader object handle.
    glCompileShader(id);
}

bool Shader::checkShader(GLuint obj)
{
    //Log::getDebug().log("Shader::checkShader(): Checking shader: %", std::to_string(obj));
    /* We are checking linking status by default. */
    GLenum pname = GL_COMPILE_STATUS;

    int status;
    int errorLength;
    glGetShaderiv(obj, pname, &status);

    if (status == GL_FALSE)
    {
        glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &errorLength);
        GLchar *errorMessage = new GLchar[errorLength];
        glGetShaderInfoLog(obj, errorLength, NULL, errorMessage);
        Log::getError().log("Shader::getShaderType(%): Shader object compilation failed: %", errorMessage);
        delete[] errorMessage;
        return false;
    }
    //Log::getDebug().log("Shader::checkShader(): Shader checked successfully: %", std::to_string(obj));
    return true;
}
