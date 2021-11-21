from conans import ConanFile, CMake, tools
from conan.tools.cmake import CMakeDeps


class RendererConan(ConanFile):
    name = "renderer"
    version = "0.1"
    license = "<Put the package license here>"
    author = "Jeffrey Lemein"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "<Description of Renderer here>"
    topics = ("<Put some tag here>", "<here>", "<and here>")
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False]
    }
    generators = "cmake", "cmake_find_package"
    default_options = {
        "shared": False,
        "fPIC": True,
        "gtest:build_gmock": True
    }

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_folder=".")
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def build_requirements(self):
        self.build_requires("gtest/1.11.0")

    def requirements(self):
        self.requires("assimp/5.0.1")
        self.requires("glm/0.9.9.8")
        self.requires("entt/3.7.1")
        self.requires("glfw/3.3.2")
        self.requires("spdlog/1.8.2")
        self.requires("imgui/1.80")
        self.requires("nlohmann_json/3.9.1")
        self.requires("stb/20200203")
        self.requires("boost/1.76.0")
        self.requires("cxxopts/2.2.1")
        self.requires("openexr/2.5.7")

