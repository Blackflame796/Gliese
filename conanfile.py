from conan import ConanFile
from conan import tools
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.microsoft import VCVars
from conan.tools.files import apply_conandata_patches, copy, export_conandata_patches, get, replace_in_file, save
from conans.errors import ConanInvalidConfiguration
import os
import textwrap

class GlieseConan(ConanFile):
    name = "gliese"
    description = "Gliese - C++ library for send HTTP-requests"
    version = "0.1"
    settings = "os", "compiler", "build_type", "arch"
    exports_source = "*.patch"
    exports_sources = "CMakeLists.txt", "src/*", "include/*"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": True, "fPIC": True}
    cppstd = "20"
    generators = "CMakeDeps"
    license = "GNU GENERAL PUBLIC LICENSE"
    homepage = "https://github.com/Blackflame796/Gliese.git"
    url = "https://github.com/conan-io/conan-center-index"

    def __init__(self, display_name="Kepler"):
        super().__init__(display_name)
        self.current_dir = os.path.dirname(__file__)
        self.libraries = {}
        with open(os.path.join(self.current_dir,"ConanLibraries.txt"), "r+") as file:
            for string in file.readlines():
                library,type = string.replace("\n","").split(":")[0],string.replace("\n","").split(":")[1]
                self.libraries.update({library:type})

    def configure(self):
        for library,type in self.libraries.items():
            library = library.split("/")[0]
            if type == "shared":
                self.options[library].shared = True
            elif type == "static":
                self.options[library].shared = False
            else:
                raise ConanInvalidConfiguration(f"Invalid build type of {library}")

    def generate(self):
        tc = CMakeToolchain(self,generator="Ninja")
        tc.parallel = False
        tc.generate()
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)
        
    def source(self):
        print("Cloning Gliese")
        src_dir = os.path.join(self.source_folder,"src")
        git = tools.Git(src_dir)
        git.clone("https://github.com/Blackflame796/Gliese")

        print("Checkout Gliese")
        commit = self.conan_data["versions"][self.version]
        git.checkout(commit)

        print("Patching Gliese")
        patches = self.conan_data["patches"][self.version]

        src_patch_files = []
        for patch in patches:
            src_patch_files.append(os.path.join(self.source_folder,patch))

        print("Applied patches")
        for src_patch_file in src_patch_files:
            tools.patch(patch_file=src_patch_file,base_path=src_dir)
            print(src_patch_file)

    def requirements(self):
        for library,type in self.libraries.items():
            self.requires(library)
    
    def build(self):
        print("Building Gliese")
        src_dir = os.path.join(self.source_folder,"src")
        cmake = CMake(self)
        if self.options.shared:
            cmake.definitions["BUILD_SHARED_LIBS"] = "ON"
        else:
            cmake.definitions["BUILD_SHARED_LIBS"] = "OFF"
        cmake.configure(source_folder=src_dir)
        cmake.build(target="Gliese")
    
    def package(self):
        print("Packaging Gliese")
        self.copy("*.hpp",dst="include",keep_path=False)
        self.copy("*.h",dst="include",keep_path=False)
        self.copy("*.lib",dst="lib",keep_path=False)
        self.copy("*.a",dst="lib",keep_path=False)
        self.copy("*.so",dst="lib",keep_path=False)
        self.copy("*.dylib",dst="lib",keep_path=False)
        self.copy("*.dll",dst="lib",keep_path=False)
        # TODO: to remove in conan v2 once legacy generators removed
        self._create_cmake_module_alias_targets(
            os.path.join(self.package_folder, self._module_file_rel_path),
            {
                "Gliese": "Gliese"
            }
        )

    def _create_cmake_module_alias_targets(self, module_file, targets):
        content = ""
        for alias, aliased in targets.items():
            content += textwrap.dedent(f"""\
                if(TARGET {aliased} AND NOT TARGET {alias})
                    add_library({alias} INTERFACE IMPORTED)
                    set_property(TARGET {alias} PROPERTY INTERFACE_LINK_LIBRARIES {aliased})
                endif()
            """)
        save(self, module_file, content)

    @property
    def _module_file_rel_path(self):
        return os.path.join("lib", "cmake", f"conan-official-{self.name}-targets.cmake")
    
    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "Gliese")
        self.cpp_info.set_property("cmake_target_name", "Gliese")
        self.cpp_info.set_property(
            "cmake_target_aliases",
            ["Gliese"],
        )
        self.cpp_info.set_property("pkg_config_name", "Gliese")
        self.cpp_info.includedirs = ["include"]
        self.cpp_info.libs = ["Gliese"]
        self.cpp_info.system_libs.append("zlib")
        self.cpp_info.build_modules["cmake_find_package"] = [self._module_file_rel_path]
        self.cpp_info.build_modules["cmake_find_package_multi"] = [self._module_file_rel_path]
