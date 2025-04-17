# Seam Carving

This repository implements an object‑oriented seam carving utility in C++ for resizing grayscale (PGM) and color (PPM) images. It uses a dynamic programming algorithm to remove low‑energy seams.

---

## Table of Contents
1. [Prerequisites](#prerequisites)
2. [Premake Setup](#premake-setup)
3. [Building on All Platforms](#building-on-all-platforms)
   - [Windows (Visual Studio)](#windows-visual-studio)
   - [Linux (Makefiles)](#linux-makefiles)
   - [macOS (Xcode)](#macos-xcode)
4. [GitHub Codespaces](#github-codespaces)
5. [Usage](#usage)
6. [License](#license)

---

## Prerequisites

- **C++17** or later compatible compiler.
- **Premake5** (version 5.0 or newer) installed on your PATH.
- **Git** (for cloning the repository).


## Premake Setup

1. Clone this repository:
   ```bash
   git clone https://github.com/<your‑org>/seam-carving.git
   cd seam‑carving
   ```

2. Generate platform‑specific project files:
   ```bash
   # For Visual Studio (Windows):
   premake5 vs2022

   # For GNU Make (Linux):
   premake5 gmake2

   # For Xcode (macOS):
   premake5 xcode4
   ```

3. The generated files will appear in the root:
   - `seam_carving.sln` (VS solution)
   - `Makefile` (GNU Make)
   - `SeamCarving.xcodeproj` (Xcode project)


## Building on All Platforms

### Windows (Visual Studio)

1. Open `seam_carving.sln` in Visual Studio 2022.
2. Select **Release** or **Debug** configuration.
3. Build the `SeamCarving` project.
4. Executable will be found in `bin/Release` or `bin/Debug`.

### Linux (GNU Make)

1. Ensure `make` and a GCC/Clang compiler are installed.
2. Run:
   ```bash
   make config=release         # or config=debug
   ```
3. Executable is in `bin/release` or `bin/debug`.

### macOS (Xcode)

1. Open `SeamCarving.xcodeproj` with Xcode.
2. Choose **My Mac** target and **Release**/ **Debug**.
3. Build the project. Executable is under `Products` in the project navigator.


## GitHub Codespaces

This project supports GitHub Codespaces for a fully configured cloud dev environment:

1. Click **Code** → **Create codespace on main** in your GitHub repo.
2. Codespaces will install:
   - GCC/Clang toolchain
   - `premake5`
3. Inside the Codespace terminal, run:
   ```bash
   premake5 gmake2
   make config=release
   ```
4. The compiled binary is located at `bin/release/seam_carving`.
5. You can mount and test with sample PGM/PPM files directly in the container.


## Usage

```bash
./seam_carving <input_file> <num_vertical> [<num_horizontal>]
```
- **`<input_file>`**: Path to `.pgm` or `.ppm` image.
- **`<num_vertical>`**: Number of vertical seams to remove.
- **`<num_horizontal>`**: (Optional) Number of horizontal seams.

Example:
```bash
./seam_carving sample.ppm 50 20
# Produces sample_processed_50_20.ppm
```


## License

MIT License © 2025
