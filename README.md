# Cross-Platform C++ CI Pipeline with GitHub Actions

This project demonstrates a cross-platform Continuous Integration (CI) setup using GitHub Actions for building and testing a C++ project on **Windows** and **Linux** using **MSVC**, **GCC**, and **Clang**.

## 🚀 Features

- ✅ Builds and tests the project on:
  - Windows (MSVC)
  - Linux (GCC & Clang)
- 🧪 Automatically runs tests using `ctest`.
- 🔧 Runs compiled executables and performs runtime checks.
- 💥 Fails early if any executable is missing or crashes.

## 🏗️ Build Matrix

The GitHub Actions matrix includes:

| OS             | Compiler | Build Type |
|----------------|----------|------------|
| Windows        | MSVC     | Release    |
| Ubuntu (Linux) | GCC      | Release    |
| Ubuntu (Linux) | Clang    | Release    |

## 🧩 GitHub Actions Workflow (`.github/workflows/ci.yml`)

This workflow does the following:

1. **Checks out** the source code.
2. **Sets reusable variables** (like build output path).
3. **Configures CMake** with appropriate compilers.
4. **Builds** the project with `cmake --build`.
5. **Runs tests** with `ctest`.
6. **Runs executables** based on platform:
   - On Windows: `w_priority_inheritence.exe`, `w_priority_fault.exe`
   - On Linux: `l_priority_inheritence`, `l_priority_fault`

## 🧪 Test Output

Executables are expected to be placed under:
- Windows: `build/Release/*.exe`
- Linux: `build/*`

The CI will:
- Check if each expected executable exists.
- Run it directly.
- Fail if any executable is missing or errors out.

## 📁 Example Project Structure

```

project-root/
│
├── src/                     # C++ source files
├── CMakeLists.txt
├── .github/
│   └── workflows/
│       └── ci.yml           # This GitHub Actions workflow
├── README.md

````

## ⚙️ Prerequisites for Local Builds

To build locally (optional):

```bash
# Linux:
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
cd build && ctest

# Windows (with Visual Studio):
cmake -B build -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
cd build && ctest -C Release
````

## Example Output
```
[Medium 0] Working...
[Medium 0] Working...
[Medium 0] Working...
[Medium 0] Working...
[Medium 0] Working...
[Medium 0] Working...
[Medium 0] Working...
[Medium 0] Working...
[Medium 0] Working...
[Medium 0] Working...
[Medium 0] Working...
[Medium 0] Working...
[Medium 0] Working...
[Medium 0] Working...
[Medium 0] Working...
[Medium 0] Working...
[Medium 0] Working...
[Medium 0] Working...
[Medium 0] Working...
[Medium 0] Working...
[Medium 0] Working...
[Medium 0] Working...
[Medium 0] Working...
[Medium 0] Working...
[Medium 0] Working...
[Medium 0] Working...
[Medium 0] Working...
[Medium 0] Working...
[Medium 0] Working...
[Medium [High] Trying to lock mutex
] Working...
[Medium [Low] Locked mutex
```


## Build Instructions

1. Clone the repository:
   ```
   git clone https://github.com/marybadalyan/Priority_Inversion_-Mars_Pathfinder-
   ```
2. Navigate to the repository:
   ```
   cd Thread-Safe_Counter
   ```
3. Generate build files:
   ```
   cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
   ```
4. Build the project:
   ```
   cmake --build build --config Release
   ```
5. Run the executable from the build directory:
   ```
   ./build/l_priority_fault

   ```

## Usage

Run the program with optional arguments to customize the experiment parameters:

```
./l_priority_fault 
```


## 📝 Notes

* Real-time priorities (e.g., for `SCHED_FIFO` in Linux) might require elevated privileges (`sudo`).
* The actual executables must be produced by your CMake project and match the expected names.

