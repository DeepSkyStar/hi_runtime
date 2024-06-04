from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.scm import Git

class hi_runtimeRecipe(ConanFile):
    name = "hi_runtime"
    version = "1.0"
    package_type = "library"
    requires = ("sdk/1.0")

    # Optional metadata
    license = "<Put the package license here>"
    author = "<Put your name here> <And your email here>"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "<Description of hi_runtime package here>"
    topics = ("<Put some tag here>", "<here>", "<and here>")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    exports_sources = ("CMakeLists.txt",
                       "hi_runtime/*",
                       "include/*",
                       "src/*",
                       )
    # Sources are located in the same place as this recipe, copy them to the recipe
    # def source(self):
    #     git = Git(self)
    #     git.clone(url="https://github.com/DeepSkyStar/hi_runtime.git", target=".")

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self, generator="Ninja")
        tc.variables["STM_PLATFORM"] = "stm32"
        tc.variables["CMAKE_C_COMPILER"] = "arm-none-eabi-gcc"
        tc.variables["CMAKE_CXX_COMPILER"] = "arm-none-eabi-g++"
        tc.variables["CMAKE_ASM_COMPILER"] = "arm-none-eabi-gcc"
        tc.variables["CMAKE_AR"] = "arm-none-eabi-ar"
        tc.variables["CMAKE_OBJCOPY"] = "arm-none-eabi-objcopy"
        tc.variables["CMAKE_OBJDUMP"] = "arm-none-eabi-objdump"
        tc.variables["SIZE"] = "arm-none-eabi-size"
        tc.variables["CMAKE_TRY_COMPILE_TARGET_TYPE"] = "STATIC_LIBRARY"
        tc.variables["BUILD_TOOLCHAIN"] = "armgcc"
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["hi_runtime"]

    

    

