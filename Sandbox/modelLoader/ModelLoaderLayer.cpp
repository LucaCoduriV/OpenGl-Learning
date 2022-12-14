//
// Created by luca on 19/08/22.
//

#include "ModelLoaderLayer.hpp"
#include <VertexArray.hpp>
#include <IndexBuffer.hpp>
#include <Renderer.hpp>
#include <Texture.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vendor/imgui/imgui.h>
#include <Events/EventDispatcher.hpp>
#include <Events/MouseEvent.hpp>
#include <functional>
#include <definitions.hpp>
#include <memory>
#include <Shader/ShaderBuilder.hpp>
#include <iostream>
#include <Scene/Entity.hpp>
#include <Scene/BaseComponents/PointLightComponent.hpp>
#include <Scene/BaseComponents/DirectionalLightComponent.hpp>

ModelLoaderLayer::ModelLoaderLayer() : Layer("LightLayer"),
                                       cam(glm::vec3(0.0f, 0.0f, 0.0f)) {

}

void ModelLoaderLayer::onAttach() {
   GLCall(glEnable(GL_DEPTH_TEST));
   GLCall(glEnable(GL_TEXTURE_2D));
   GLCall(glEnable(GL_BLEND));
   GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

   scene = GE::makeRef<GE::Scene>();
   ourModel = GE::makeRef<ModelLoader>("res/models/backpack/backpack.obj",
                                       &(*scene));

   light = GE::makeRef<GE::Entity>(scene->CreateEntity("Light"));
   auto& pl = light->AddComponent<GE::PointLightComponent>();
   pl.position = glm::vec3(-0.5f,-0.5f,-0.5f);
   pl.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
   pl.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
   pl.specular = glm::vec3(1.0f, 1.0f, 1.0f);
   pl.constant = 1.0f;
   pl.linear = 0.09f;
   pl.quadratic = 0.032f;

   auto& dl = light->AddComponent<GE::DirLightComponent>();
   dl.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
   dl.ambient = glm::vec3(0.05f, 0.05f, 0.05f);
   dl.diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
   dl.specular = glm::vec3(0.5f, 0.5f, 0.5f);

   scene->init();

}

void ModelLoaderLayer::onUpdate(float timeStamp) {

   if (forward) {
      cam.processKeyboard(CameraMovement::FORWARD, timeStamp);
   }
   if (backward) {
      cam.processKeyboard(CameraMovement::BACKWARD, timeStamp);
   }
   if (left) {
      cam.processKeyboard(CameraMovement::LEFT, timeStamp);
   }
   if (right) {
      cam.processKeyboard(CameraMovement::RIGHT, timeStamp);
   }

   renderer->clear();
   scene->shader->bind();
   auto& l = light->GetComponent<GE::DirLightComponent>();
   l.direction.x = lightPos[0];
   l.direction.y = lightPos[1];
   l.direction.z = lightPos[2];

   cam.onUpdate(*scene->shader);

   scene->draw(*scene->shader);

}

void ModelLoaderLayer::onDetach() {
   Layer::onDetach();
}

void ModelLoaderLayer::onEvent(Event &event) {
   EventDispatcher dispatcher(event);
   dispatcher.dispatch<MouseMovedEvent>(
      BIND_EVENT_FN(ModelLoaderLayer::onMouseMove));
   dispatcher.dispatch<KeyPressedEvent>(
      BIND_EVENT_FN(ModelLoaderLayer::onKeyPressed));
   dispatcher.dispatch<KeyReleasedEvent>(
      BIND_EVENT_FN(ModelLoaderLayer::onKeyReleased));
   dispatcher.dispatch<MouseScrolledEvent>(
      BIND_EVENT_FN(ModelLoaderLayer::onScroll));
   dispatcher.dispatch<WindowResizeEvent>(
      BIND_EVENT_FN(ModelLoaderLayer::onWindowResize));
}

void ModelLoaderLayer::onImGuiRender() {
   ImGui::Text("light control");
   ImGui::SliderFloat3("position", lightPos,-5.0f, 5.0f);
   ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
               1000.0f / ImGui::GetIO().Framerate,
               ImGui::GetIO().Framerate);
   ImGui::Render();
}

bool ModelLoaderLayer::onMouseMove(MouseMovedEvent &event) {
   float xpos = static_cast<float>(event.GetX());
   float ypos = static_cast<float>(event.GetY());

   if (firstMouse) {
      lastX = xpos;
      lastY = ypos;
      firstMouse = false;
   }

   float xoffset = xpos - lastX;
   float yoffset =
      lastY - ypos; // reversed since y-coordinates go from bottom to top

   lastX = xpos;
   lastY = ypos;

   cam.processMouseMovement(xoffset, yoffset);
   return true;
}

bool ModelLoaderLayer::onKeyPressed(KeyPressedEvent &event) {
   if (event.GetKeyCode() == (KeyCode) Key::W && !event.IsRepeat())
      forward = true;
   if (event.GetKeyCode() == (KeyCode) Key::S && !event.IsRepeat())
      backward = true;
   if (event.GetKeyCode() == (KeyCode) Key::A && !event.IsRepeat())
      left = true;
   if (event.GetKeyCode() == (KeyCode) Key::D && !event.IsRepeat())
      right = true;

   return true;
}

bool ModelLoaderLayer::onKeyReleased(KeyReleasedEvent &event) {
   if (event.GetKeyCode() == (KeyCode) Key::W)
      forward = false;
   if (event.GetKeyCode() == (KeyCode) Key::S)
      backward = false;
   if (event.GetKeyCode() == (KeyCode) Key::A)
      left = false;
   if (event.GetKeyCode() == (KeyCode) Key::D)
      right = false;

   return true;
}

bool ModelLoaderLayer::onScroll(MouseScrolledEvent &event) {
   cam.processMouseScroll(static_cast<float>(event.GetYOffset()));
   return true;
}

bool ModelLoaderLayer::onWindowResize(WindowResizeEvent &event) {
   cam.setScreenWidth(event.getWidth());
   cam.setScreenHeight(event.getHeight());
   return false;
}