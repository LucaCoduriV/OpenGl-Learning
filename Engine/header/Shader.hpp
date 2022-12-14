/*
-----------------------------------------------------------------------------------
Nom du fichier  : Shader.hpp
Auteur(s)       : lucac
Date creation   : 10.08.2022
Description     : <à compléter>
Remarque(s)     : <à compléter>
Compilateur     : Mingw-w64 g++ 8.1.0
-----------------------------------------------------------------------------------

*/

#ifndef GAMEENGINE_SHADER_HPP
#define GAMEENGINE_SHADER_HPP

#include <string>
#include <unordered_map>
#include "GL/glew.h"
#include <glm/glm.hpp>
#include <filesystem>


class Shader {
public:
   // the program ID
   unsigned int ID;

   // le constructeur lit et construit le shader
   explicit Shader(const std::filesystem::path& vertexPath, const
   std::filesystem::path&
   fragmentPath);

   explicit Shader(const std::string& vertexProgram, const std::string&
   fragmentProgram);

   // Activation du shader
   void bind();

   static void unbind();

   // fonctions utiles pour l'uniform

   void setBool(const std::string &name, bool value);

   void setInt(const std::string &name, int value);

   void setFloat(const std::string &name, float value);

   void setFloat(const std::string &name, float value, float value2);

   void setFloat(const std::string &name, float value, float value2,
                 float value3);

   void setFloat(const std::string &name, float value, float value2, float value3,
                 float value4);

   void setMat4f(const std::string &name, const glm::mat4& value);

private:
   [[nodiscard]] int getUniformLocation(const std::string &name);
   static void checkCompileErrors(unsigned int shader, const std::string &type);
   void compileShader(const std::string& vertexProgram, const std::string& fragmentProgram);
   std::unordered_map<std::string, int> uniformLocationCache;

};


#endif //GAMEENGINE_SHADER_HPP
