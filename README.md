# TIVIDY — Workflow Management System

COS 214 Final Project (2026). A generic workflow management system in **C++11**,
demonstrated with two scenarios:

- **Education** — university assignment marking (submission, marking, moderation, release)
- **Healthcare** — hospital admission (registration, assessment, bed allocation, confirmation)

> **Current status:** the shared engine, scenarios and CLI are under construction.
> The test harness and CI are fully set up. Tests are the source of truth right
> now — the app (`build/tividy`) is a stub and will be built out in later phases.

---

## Team

| Member | Folder | Area |
|---|---|---|
| _Ethan Van Eyden_ (u25260244) | `member1_core/` | Core workflow model (Composite, Instances, API) |
| _Jayden Dean_ (u24647897) | `member2_execution/` | Item and container lifecycle (State) |
| _Stefan van Staden_ (u25031784) | `member3_routing/` | Assignment & traversal (Strategy, Iterator) |
| _Christian Nunes_ (u25138309) | `member4_services/` | Commands, events, history (Command, Abstract Mediator, Observer, Chain of Responsibility) |
| _Kyle Pillay_ (u23545756) | `member5_education/` | Education scenario (Concrete Mediator, Concrete Leaf/Composite, Concrete Chain) |
| _Rochaan_ (uXXXXXXXX) | `member6_hospital/` | Hospital scenario (Concrete Mediator, Concrete Leaf/Composite, Concrete Chain) |
| _Lesego Tebeile_ (u25143230) | `member7_integration/` | CI/CD, Scenario Setup factory, Builder, Facade

---

## Requirements

- **CMake** ≥ 3.14
- **C++11** compiler (`g++` ≥ 7, `clang++` ≥ 6, or MSVC 2019+)
- **Git**
- Internet access on the **first** configure (GoogleTest is downloaded automatically via CMake `FetchContent`)

Tested on Ubuntu 22.04 (CI) and Linux/macOS locally. Windows should work with
MinGW-w64 or MSVC, but hasn't been verified.

---

## Repository layout
```
tividy/
├── CMakeLists.txt                              # build configuration
├── README.md
├── .github/workflows/
│ └── ci.yml                                    # GitHub Actions (build + test)
├── app/                                        # CLI entry point
│ └── main_web.cpp
├── member1_core/                               # each member owns one folder
├── member2_execution/                          # headers and .cpp live together here
├── member3_routing/
├── member4_services/
├── member5_education/
├── member6_hospital/
├── member7_integration/
├── tests/                                      # all test files, using GoogleTest
└── docs/                                       # UML exports, design docs, research
```


**All member folders are on the compiler's include path**, so `#include "WorkItem.h"`
works from any folder. **Header names must be globally unique** — use
`EducationPolicies.h`, `HospitalPolicies.h`, not `Policies.h` twice.

---

## Build and run the tests

From the repo root:

```bash
#Optionally if u want to start afresh
rm -rf build

# 1. Configure (once; re-run after changing CMakeLists.txt)
cmake -S . -B build

# 2. Build everything
cmake --build build --target tividy_tests

# 3. Run the full test suite
ctest --test-dir build --output-on-failure
```

## Run a subset

### One suite
```
ctest --test-dir build -R WorkItemTest --output-on-failure
```

### One test directly (richer output)
```
./build/tividy_tests --gtest_filter='WorkItemTest.*'
```

## Run the app (currently a stub)
```
./build/tividy
```

## CI (GitHub Actions)
Every push and pull request to main or dev runs 
.github/workflows/ci.yml:

Check out the repo.

Install cmake, g++, build-essential.

Configure with CMake.

Build the tests 
(cmake --build build --target tividy_tests --parallel - temporary; will switch to full build once the app builds).

Run ctest --test-dir build --output-on-failure.

A green PR = safe to merge. A red PR = do not merge. Fix on your branch and push again.

## Audit before pushing

### (a) Every .cpp on disk must be in CMakeLists.txt - empty output = OK
```
comm -23 <(ls member*_*/*.cpp app/*.cpp 2>/dev/null | sort) \
         <(grep -oE 'member[0-9]+_[a-z]+/[A-Za-z_]+\.cpp|app/[A-Za-z_]+\.cpp' CMakeLists.txt | sort -u)
```

### (b) No file listed twice - empty output = OK
```
grep -oE 'member[0-9]+_[a-z]+/[A-Za-z_]+\.cpp' CMakeLists.txt | sort | uniq -d
```

### (c) Every test file on disk must be in CMakeLists.txt - empty output = OK
```
comm -23 <(ls tests/test_*.cpp | sort) \
         <(grep -oE 'tests/test_[a-z_]+\.cpp' CMakeLists.txt | sort -u)
```
