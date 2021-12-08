//
// Created by jlemein on 06-11-20.
//
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <anim_animationsystem.h>
#include <ecs_camerasystem.h>
#include <ecs_transformsystem.h>
#include <ecsg_scenegraph.h>
#include <izz_resourcemanager.h>
#include <glrs_rendersystem.h>
#include <io_inputsystem.h>
#include <vwr_viewer.h>
#include <vwr_viewerextension.h>
#include <vwr_windowinputlistener.h>
#include <gui_guiwindow.h>

#include <iostream>
#include <spdlog/spdlog.h>
#include <vector>

using namespace std;
using namespace lsw;
using namespace lsw::viewer;

namespace {
static void error_callback(int error, const char* description) {
  fprintf(stderr, "Error: %s\n", description);
}
}  // namespace

Viewer::Viewer(std::shared_ptr<ecsg::SceneGraph> sceneGraph, std::shared_ptr<glrs::RenderSystem> renderSystem,
               std::shared_ptr<gui::GuiSystem> guiSystem)
  : m_sceneGraph{sceneGraph}
  , m_guiSystem(guiSystem)
  , m_registry(sceneGraph->getRegistry())
  , m_animationSystem{make_shared<anim::AnimationSystem>(sceneGraph)}
  , m_renderSystem{renderSystem}
  , m_cameraSystem{make_shared<ecs::CameraSystem>(m_registry)}
  , m_debugSystem{make_shared<ecs::DebugSystem>(m_registry)}
  , m_transformSystem{make_shared<ecs::TransformSystem>(m_registry)} {}

Viewer::~Viewer() {}

void Viewer::setWindowSize(unsigned int width, unsigned int height) {
  m_displayDetails.windowWidth = static_cast<int>(width);
  m_displayDetails.windowHeight = static_cast<int>(height);
  m_cameraSystem->setFramebufferSize(width, height);
}

void Viewer::setTitle(const std::string& title) {
  m_title = title;
}

void Viewer::initialize() {
  glfwSetErrorCallback(error_callback);

  if (!glfwInit()) exit(EXIT_FAILURE);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_SAMPLES, 8);

  GLFWwindow* window =
      glfwCreateWindow(m_displayDetails.windowWidth, m_displayDetails.windowHeight, m_title.c_str(), NULL, NULL);

  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetWindowUserPointer(window, this);
  glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
    static_cast<Viewer*>(glfwGetWindowUserPointer(window))->onWindowResize(width, height);
  });

  m_displayDetails.window = reinterpret_cast<WindowHandle*>(window);
  m_displayDetails.shadingLanguage = "glsl";
  m_displayDetails.shadingLanguageVersion = "#version 130";

  m_inputSystem = std::make_shared<io::InputSystem>(window, 640, 640);
  m_genericInputListener = std::make_shared<WindowInputListener>(window);
  m_inputSystem->registerInputListener(m_genericInputListener);
  m_firstPersonSystem = std::make_shared<ecs::FirstPersonMovementSystem>(m_registry, m_inputSystem.get());

  glfwMakeContextCurrent(window);
  if (glewInit() != GLEW_OK) {
    throw std::runtime_error("Failed initializing GLEW");
  }

  glfwSwapInterval(1);

//  glEnable(GL_DEPTH_TEST);
//  glDisable(GL_CULL_FACE);
//  glCullFace(GL_BACK);
//  glFrontFace(GL_CCW);
//  glClearColor(0.15F, 0.15F, 0.25F, 0.0F);

  m_cameraSystem->init();
  m_debugSystem->init();      // for debug visualizations
  m_animationSystem->init();  // possible bone initialization

  // should be called before render system.

  m_renderSystem->init();
  m_inputSystem->init();
  m_firstPersonSystem->init();
  m_transformSystem->init();

  m_guiSystem->initialize(this);
}

int Viewer::run() {
  GLFWwindow* window = reinterpret_cast<GLFWwindow*>(m_displayDetails.window);

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
    m_cameraSystem->setFramebufferSize(width, height);
    //    m_renderSystem->setPerspective(
    //        glm::perspective(45.0F, ratio, 0.01F, 1000.0F));

    glViewport(0, 0, width, height);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // update systems
    m_animationSystem->update(time, dt);
    m_inputSystem->update();

    // if no gui system (then process),
    // OR (only executed if there is gui system), check if not processing input (then process)
    if (!m_guiSystem || !m_guiSystem->isProcessingInput()) {
      m_firstPersonSystem->update(dt);
    }
    m_transformSystem->update(time, dt);
    m_cameraSystem->update(dt);
    m_guiSystem->update(time, dt);
    m_renderSystem->update(time, dt);

    m_guiSystem->beforeRender();

    m_renderSystem->render();

    m_guiSystem->afterRender();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);

  glfwTerminate();
  return 0;
}

ecsg::SceneGraph& Viewer::getSceneGraph() {
  return *m_sceneGraph;
}

DisplayDetails Viewer::getDisplayDetails() {
  return m_displayDetails;
}

void Viewer::setActiveCamera(ecsg::SceneGraphEntity cameraEntity) {
  m_renderSystem->setActiveCamera(cameraEntity.handle());
}

void Viewer::onWindowResize(int width, int height) {
  spdlog::info("Window is resized to: {} x {}", width, height);
  m_renderSystem->getFramebuffer().resize(width, height);
  glViewport(0, 0, width, height);
}