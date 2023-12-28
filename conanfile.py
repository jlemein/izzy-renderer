from conan import ConanFile, tools
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps
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
    generators = "CMakeDeps"
    default_options = {
        "shared": False,
        "fPIC": True,
        "gtest/*:build_gmock": True
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

    def requirements(self):
        self.requires("assimp/5.0.1")
        self.requires("glm/cci.20230113")
        self.requires("entt/3.12.2")
        self.requires("glfw/3.3.2")
        self.requires("spdlog/1.12.0")
        self.requires("imgui/cci.20230105+1.89.2.docking")
        self.requires("nlohmann_json/3.9.1")
        self.requires("stb/cci.20230920")
        self.requires("boost/1.83.0")
        self.requires("cxxopts/2.2.1")
        self.requires("openexr/2.5.7")
        self.requires("gtest/1.14.0", test=True)

