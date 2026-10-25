# Corona Hospital Management System (C++ OOP)

A **console-based patient-record manager** originally developed for the **Object-Oriented Programming (OOP)** course at **Taiz University**, College of Engineering, Information Technology department. This repository provides a **maintained educational version** of the original 2020 C++ project, plus an archived copy of its source for historical reference.

> **Educational demonstration only.** This is **not** a production-ready healthcare system. Patient records are stored as unencrypted plain-text files. **Do not enter real patient data** or deploy it for medical use.

## Features

- Register patients with name, gender, age, phone number, address, medical problem, and assigned doctor.
- List records and search by patient ID or part of a full name.
- Modify and delete existing records by unique patient ID.
- Keep records between runs using one text file per patient.
- Validate required inputs, age, and phone-number format.
- Use a two-step file replacement process to reduce accidental record loss during updates.

## Technologies

- **C++17** and its standard library (`std::filesystem`, streams, containers).
- **OOP:** `Patient` (encapsulated patient data), `PatientRepository` (file persistence), and `Operations` (menu actions).
- **CMake** for platform-independent builds; legacy Visual Studio solution and project files are also included.
- No external libraries, databases, online services, or paid tools required to build this application.

## Build and run

### Option A: Visual Studio Community on Windows

1. Install a supported **Visual Studio Community** edition with **Desktop development with C++** and the Windows SDK. This edition is free for eligible individual use (subject to Microsoft's license terms).
2. Open `Hospital.sln`. The original solution targets the **Visual Studio 2019 v142** toolset. If you use a newer Visual Studio version, allow it to **retarget** the project to your installed C++ toolset and Windows SDK.
3. Select **Debug | x64**, then press **Ctrl+F5** (Start Without Debugging).
4. Patient files are stored in `data/` under the process's current working directory, typically the project directory when launched by Visual Studio.

Alternatively, open the repository **as a CMake project** in Visual Studio (supported editions) and run the `Hospital` target.

### Option B: CMake and a free C++ compiler (Windows, Linux, macOS)

Requirements: **CMake 3.16+** and any C++17-capable compiler (for example, GCC, Clang, or Microsoft C++ Build Tools).

```sh
cmake -S . -B build
cmake --build build --config Release
```

Run on Windows (single-configuration build using MinGW):

```powershell
.\build\Hospital.exe
```

Run on Windows (Visual Studio multi-configuration generator):

```powershell
.\build\Release\Hospital.exe
```

Run on Linux/macOS:

```sh
./build/Hospital
```

For a minimal GCC/MinGW build without CMake:

```sh
g++ -std=c++17 -Wall -Wextra -pedantic Hospital.cpp -o Hospital
```

**Important:** Running the program requires no Visual Studio license, but you still need a compatible compiler to build a Windows `.exe`. A Linux build is not a Windows executable.

## Console menu

```text
1. Add a new patient
2. Modify patient information
3. Delete patient information
4. Search for a patient
5. List all patients
0. Exit
```

Adding a patient creates a stable ID such as `P000001`. Two patients with the same name receive different IDs. Editing and deleting use the ID displayed in the patient list; searching supports IDs or full/partial names. The app does not have an authentication mechanism; the old hard-coded demo password was removed because it provided no real security.

### Data storage

The program creates the following runtime files, **which are excluded from Git**:

```text
data/
├── next_id.txt
└── patients/
    ├── P000001.txt
    └── P000002.txt
```

Patient files contain readable text, with **no encryption, user permissions, or audit logging**. Use **fictional test data only**. The `HOSPITAL_DATA_DIR` environment variable may optionally override the default `data` directory, which is useful for isolated tests.

**Old data is not migrated automatically.** In the original project, a patient's first name was used as the filename. The new version uses unique IDs and a different record format to avoid name collisions. Existing real data should **not** be copied or uploaded to GitHub.

## Original project and attribution

**Original academic project (2020):**

- **University:** Taiz University, College of Engineering, Information Technology.
- **Course:** Object-Oriented Programming (OOP).
- **Course lecturer (as credited in the original program):** Eng. Ezzaddin Ahmed.
- **Contributors (as credited in the original program):** Omar Yahya Saeed; Hazim Saeed Alqady; Shehab Mohammed Al-Ward.

The initial code is preserved in [`legacy/Hospital_2020.cpp`](legacy/Hospital_2020.cpp) to document its university-project origins. **This archived file is not built** and contains the original Windows-specific commands and insecure hard-coded demonstration password. Do not use it in production. The maintained `Hospital.cpp` file was refactored in 2026 for more reliable operation and portability, without claiming that these later improvements were part of the original coursework.

### Changes in the maintained version

- Retained the five original console operations: add, edit, delete, search, list.
- Introduced unique IDs to prevent overwriting patients with the same first name.
- Kept OOP and text-file storage; separated interactive operations from persistence.
- Improved input validation and allowed names, addresses, and problems with spaces.
- Avoided deleting old records before replacement data is fully written.
- Removed `goto`, nonportable `system("cls")`/`system("pause")`, and the hard-coded demo password.
- Added CMake, a `.gitignore`, and automated console regression tests.

## Tests

Build the program, then run:

```sh
python3 tests/test_cli.py ./build/Hospital
```

Windows PowerShell (adjust the executable path to your build type):

```powershell
py -3 tests\test_cli.py .\build\Release\Hospital.exe
```

The tests use a temporary directory and **fictional records**. They check add/list/search/edit/delete, duplicate-name safety, invalid inputs, and delete cancellation.

## Known limitations

- No login/authentication, encryption, access controls, audit log, or database.
- No concurrency protection: only one program instance should modify the same data directory at a time.
- Basic input validation is not a substitute for clinical or legal data handling standards.
- Plain-text storage can be damaged by manual edits or external interruption; file backups and transaction recovery are limited.
- Interface is text-only and primarily in English.

## License

No open-source license is included because the original project lists multiple contributors. Select a license only after obtaining agreement from the relevant rights holders. Publishing the source on GitHub without a license does **not** automatically grant reuse rights to others.
