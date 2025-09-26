# Nibbles

Nibbles is a cross-platform Qt GUI that analyses byte streams as sequences of
nibbles (4-bit values) and visualises their transition probabilities. The
application was created as a teaching tool for information theory: open a binary
file, and it will compute the joint and conditional distributions of successive
nibbles, show the matrix in a table, and report entropy metrics.

## Features

- 📂 Load any binary file and split it into a stream of nibbles.
- 📈 Compute joint and conditional transition probabilities for all `16 × 16`
nibble pairs.
- 🧮 Display joint entropy, per-nibble entropy, and relative entropy versus the
maximum of 4 bits per nibble.
- 🪟 Responsive Qt Widgets interface with HiDPI-friendly defaults.

## Project layout

```
.
├── CMakeLists.txt          # Top-level CMake project (Qt Widgets application)
├── core/                   # Header-only domain logic (nibbles, entropy, IO)
├── src/                    # Qt GUI application sources
├── conanfile.txt           # Optional Conan recipe for fetching Qt
├── profiles/               # Example Conan profiles
├── pyproject.toml          # Poetry project used to manage Conan locally
└── README.md
```

The domain logic lives entirely in the `core/` headers so that it can be reused
outside of the GUI if desired. The `src/` folder contains the Qt `MainWindow`
implementation, models, and UI description.

## Prerequisites

To build the GUI you need the following tools:

- **CMake ≥ 3.23**
- **A C++17 compiler** (GCC, Clang, MSVC, or Apple Clang)
- **Qt 6 (Widgets module)** – either installed system-wide or provided by Conan
- **Ninja or Make** (or any other CMake generator you prefer)

If you do not already have Qt installed, the repository includes a
`conanfile.txt` that can fetch Qt 6.8 for you. Python 3.12+ and Poetry are only
necessary if you prefer to manage the Conan dependency using Poetry; they are
not required for a manual CMake build.

## Building with CMake (system Qt)

1. Install Qt 6 with the Widgets module. Make sure `qmake` and the Qt CMake
   configuration files (e.g. `Qt6Config.cmake`) are in your PATH or specify
   `-DCMAKE_PREFIX_PATH` when running CMake.
2. Configure and build:

   ```bash
   cmake -S . -B build -G Ninja
   cmake --build build
   ```

   Replace `-G Ninja` with your preferred generator (e.g. `-G "Unix Makefiles"`).

3. Launch the application:

   ```bash
   ./build/src/nibbles
   ```

   On Windows or macOS the executable may live inside a bundle; CMake’s output
   will show the final path.

## Building with Conan + CMake

If you would rather let Conan download Qt for you, use the provided recipe:

```bash
# Optionally create and activate a virtual environment that contains Conan
poetry install          # installs Conan 2.x defined in pyproject.toml
poetry shell            # optional: spawn shell with Conan on PATH

# Configure Conan (only once per machine)
conan profile detect --force

# Install dependencies and generate the CMake toolchain
conan install . -of build/conan --build=missing

# Configure and build with the Conan toolchain
cmake -S . -B build -G Ninja \
      -DCMAKE_TOOLCHAIN_FILE="build/conan/conan_toolchain.cmake"
cmake --build build
```

The executable will be placed in `build/src/` after the build succeeds. Conan’s
profile and cache determine the exact Qt version and build type; check the
`profiles/` directory for examples.

## Running the application

1. Start the built `nibbles` executable.
2. Choose **File → Open…** and select any binary file (log, image, firmware,
   etc.).
3. The application will read the file into a nibble sequence, update the table
   with the `16 × 16` transition probabilities, and display:
   - Total number of observed transitions.
   - Joint entropy `H(S_i, S_{i+1})` (bits per pair).
   - Maximum entropy (always 4 bits per nibble in this model).
   - Relative entropy, normalised per nibble/bit.

If the file cannot be read or parsed, an error dialog explains the failure.

## Development tips

- `core/` contains only headers and is compiled as an `INTERFACE` library. No
  additional build steps are needed when editing these files.
- `SchemeModel` (`src/scheme_model.*`) adapts the probability matrix for
  display in the Qt table view. Update it if you change the presentation.
- The UI layout is stored in `src/mainwindow.ui` and can be edited with Qt
  Designer.

## Testing

The repository currently does not include automated tests. You can manually
verify behaviour by opening known binary inputs (e.g. a file with repeating
bytes versus a random file) and inspecting the resulting entropy values.

## License

This project does not currently include an explicit licence. Add one before
publishing or distributing the application.
