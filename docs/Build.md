## ⚙️ Build from source

### 🪟Windows

```powershell
mkdir build
cd build
# for x86
cmake ..  --preset  "MSBuild_Release_x32"
cmake --build --preset "MSBuild_Release_x32"
# for x64
cmake ..  --preset  "MSBuild_Release_x64"
cmake --build --preset "MSBuild_Release_x64"
```

### 🐧Linux


```bash
mkdir build
cd build
# for x86
cmake ..  --preset  "GCC_Release_x32"
cmake --build --preset "GCC_Release_x32"
# for x64
cmake ..  --preset  "GCC_Release_x64"
cmake --build --preset "GCC_Release_x64"
```

### 🍏 macOS


```bash
mkdir build
cd build
# for x86
cmake ..  --preset  "Clang_Release_x32"
cmake --build --preset "Clang_Release_x32"
# for x64
cmake ..  --preset  "Clang_Release_x64"
cmake --build --preset "Clang_Release_x64"
```
