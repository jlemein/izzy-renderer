//
// Created by jlemein on 06-11-20.
//
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <ecs_camerasystem.h>
#include <ecs_rendersystem.h>
#include <ecs_transformsystem.h>
#include <ecsg_scenegraph.h>
#include <io_inputsystem.h>
#include <res_resourcemanager.h>
#include <vwr_viewer.h>
#include <vwr_viewerextension.h>
#include <vwr_windowinputlistener.h>

#include <iostream>
#include <vector>

using namespace std;
using namespace affx;
using namespace affx::viewer;

namespace {
static void error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}
} // namespace

Viewer::Viewer(ecs::SceneGraph &sceneGraph,
               res::ResourceManager &resourceManager)
    : m_sceneGraph{sceneGraph}, m_resourceManager{resourceManager},
      m_registry(sceneGraph.getRegistry()),
      m_renderSystem{make_shared<ecs::RenderSystem>(m_registry)},
      m_cameraSystem{make_shared<ecs::CameraSystem>(m_registry)},
      m_debugSystem{make_shared<ecs::DebugSystem>(m_registry)},
      m_transformSystem{make_shared<ecs::TransformSystem>(m_registry)} {}

Viewer::~Viewer() {}

void Viewer::setWindowSize(unsigned int width, unsigned int height) {
  m_displayDetails.windowWidth = static_cast<int>(width);
  m_displayDetails.windowHeight = static_cast<int>(height);
}

void Viewer::initialize() {

  glfwSetErrorCallback(error_callback);

  if (!glfwInit())
    exit(EXIT_FAILURE);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  GLFWwindow *window = glfwCreateWindow(m_displayDetails.windowWidth,
                                        m_displayDetails.windowHeight,
                                        "Simple Viewer", NULL, NULL);

  m_displayDetails.window = reinterpret_cast<WindowHandle *>(window);
  m_displayDetails.shadingLanguage = "glsl";
  m_displayDetails.shadingLanguageVersion = "#version 130";

  m_inputSystem = std::make_shared<io::InputSystem>(window, 640, 640);
  m_genericInputListener = std::make_shared<WindowInputListener>(window);
  m_inputSystem->registerInputListener(m_genericInputListener);
  m_firstPersonSystem = std::make_shared<ecs::FirstPersonMovementSystem>(
      m_registry, m_inputSystem.get());

  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);
  if (glewInit() != GLEW_OK) {
    std::cout << "Failed initializing GLEW\n";
  }

  glfwSwapInterval(1);

  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);
  glClearColor(0.25F, 0.25F, 0.35F, 0.0F);

  m_cameraSystem->init();
  m_debugSystem->init(); // for debug visualizations
  // should be called before render system.
  m_renderSystem->init();
  m_inputSystem->init();
  m_firstPersonSystem->init();
  m_transformSystem->init();

  for (auto &extensions : m_viewerExtensions) {
    extensions->initialize();
  }
}

int Viewer::run() {
  GLFWwindow *window = reinterpret_cast<GLFWwindow *>(m_displayDetails.window);

  float prevTime = 0.0F;
  while (!glfwWindowShouldClose(window)) {
    float ratio;
    int width, height;
    float time = static_cast<float>(glfwGetTime());
    float dt = time - prevTime;
    prevTime = time;

    glfwGetFramebufferSize(window, &width, &height);
    //    ratio = width / (float)height;
    //    m_renderSystem->setMovementVector(movementVector);
    //    m_renderSystem->setPerspective(
    //        glm::perspective(45.0F, ratio, 0.01F, 1000.0F));

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // update systems
    m_inputSystem->update();
    m_firstPersonSystem->update(dt);
    m_transformSystem->update(time, dt);
    m_cameraSystem->update(dt);

    for (auto &extensions : m_viewerExtensions) {
      extensions->update(time, dt);
    }

    m_renderSystem->update(time, dt);

    for (auto &extensions : m_viewerExtensions) {
      extensions->beforeRender();
    }
    m_renderSystem->render();
    for (auto &extensions : m_viewerExtensions) {
      extensions->afterRender();
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);

  glfwTerminate();
  return 0;
}

ecs::SceneGraph &Viewer::getSceneGraph() { return m_sceneGraph; }

DisplayDetails Viewer::getDisplayDetails() { return m_displayDetails; }

void Viewer::setActiveCamera(ecs::SceneGraphEntity cameraEntity) {
  m_renderSystem->setActiveCamera(cameraEntity.handle());
}

void Viewer::registerExtension(
    std::shared_ptr<ecs::IViewerExtension> interactable) {
  m_viewerExtensions.emplace_back(move(interactable));
}

void Viewer::registerRenderSubsystem(
    std::shared_ptr<ecs::IRenderSubsystem> renderSubsystem) {
  m_renderSystem->addSubsystem(renderSubsystem);
}
