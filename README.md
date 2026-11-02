# TIVIDY — Workflow Management System
 
COS 214 Final Project (2026). A generic workflow management system written in **C++11**, with a React UI,
demonstrated with two industries that run on the **same engine**:
 
- **Education** — university assignment marking (submission, marking, moderation, release)
- **Healthcare** — hospital admission (registration, treatment, severity approval, ward release)
The C++ backend is a REST server; the React frontend talks to it. Everything below runs for both industries.
 
---
 
## Team
 
| Member                          | Folder                 | Area                                                                                      |
| ------------------------------- | ---------------------- | ----------------------------------------------------------------------------------------- |
| _Ethan Van Eyden_ (u25260244)   | `member1_core/`        | Core workflow model (Composite, instances, registries)                                    |
| _Jayden Dean_ (u24647897)       | `member2_execution/`   | Item and container lifecycle (State)                                                      |
| _Stefan van Staden_ (u25031784) | `member3_routing/`     | Assignment & traversal (Strategy, Iterator)                                               |
| _Christian Nunes_ (u25138309)   | `member4_services/`    | Commands, events, history (Command, Mediator, Observer, Chain of Responsibility)          |
| _Kyle Pillay_ (u23545756)       | `member5_education/`   | Education scenario (concrete Mediator, Leaf, Chain, Builder)                              |
| _Rochaan Verster_ (u25045785)   | `member6_hospital/`    | Hospital scenario (concrete Mediator, Leaf, Chain, Builder)                               |
| _Lesego Tebeile_ (u25143230)    | `member7_integration/` | Abstract Factory, Builder, Facade, Visitors, REST API, JSON, CI/CD, Docker                |
 
---
 
## Quick start
 
Pick one way to run it.
 
### A. Docker (everything, one command)
 
```bash
docker compose up --build
```
 
| URL                            | What                                                                       |
| ------------------------------ | -------------------------------------------------------------------------- |
| http://localhost:3000          | the UI (nginx serves the React build and forwards `/api/*` to the backend) |
| http://localhost:8080/health   | the backend directly (optional)                                            |
 
Stop with `docker compose down`.
 
### B. Without Docker (two terminals)
 
```bash
# terminal 1 — backend (REST API on :8080)
cmake -S . -B build
cmake --build build
./build/tividy 8080
 
# terminal 2 — frontend (UI on http://localhost:5173, proxies /api to :8080)
cd frontend
npm install
npm run dev
```
 
`./build/tividy [port] [host]` — default port `8080`, default host `0.0.0.0`.
 
---
 
## Requirements
 
| Tool                | Version                            | Needed for                         |
| ------------------- | ---------------------------------- | ---------------------------------- |
| CMake               | >= 3.14                            | backend build                      |
| C++11 compiler      | `g++` >= 7, `clang++` >= 6, MSVC 2019+ | backend build                  |
| Git                 | any                                | source control                     |
| Node.js             | 22 (npm included)                  | frontend                           |
| Docker + Compose v2 | optional                           | `docker compose up`                |
 
The first CMake configure needs internet: GoogleTest is downloaded with `FetchContent`.
Tested on Ubuntu 22.04 (CI) and WSL/Linux. Windows should work with MinGW-w64 or MSVC but is unverified.
 
---
 
## Build and test
 
### Backend (C++)
 
```bash
rm -rf build                      # only if you want a clean start; ALWAYS do this after pulling big changes
cmake -S . -B build               # re-run after changing CMakeLists.txt
cmake --build build               # builds the server (tividy) and the tests (tividy_tests)
ctest --test-dir build --output-on-failure
```
 
> **Link errors like "undefined reference to ..." after a pull or file copy?** Delete the `build/` folder and
> configure again. Stale objects from old files are the usual cause.
 
Run a subset:
 
```bash
ctest --test-dir build -R WiringTest --output-on-failure      # by name
./build/tividy_tests --gtest_filter='WorkItemTest.*'          # directly, richer output
```
 
Build only the server, without GoogleTest (what the Docker image does):
 
```bash
cmake -S . -B build -DTIVIDY_BUILD_TESTS=OFF
cmake --build build --target tividy
```
 
### Frontend (React + TypeScript + Vite)
 
```bash
cd frontend
npm install                       # or `npm ci` for a reproducible install
npm run dev                       # dev server with hot reload
npm run build                     # typecheck + production build into dist/
npm run lint                      # oxlint
npm test                          # unit and component tests (Vitest)
npm run mock-api                  # fake backend on :8080 (no C++ needed)
```
 
Contract tests against the **real** C++ server (start `./build/tividy 8080` first):
 
```bash
VITE_LIVE_API=1 npm test
```
 
`VITE_API_URL` changes the API address (default `http://localhost:8080`; the Docker build uses an empty value so the
browser calls the same origin and nginx proxies to the backend).
 
---
 
## What the system does
 
A workflow **instance** is one independent run of a definition. Each instance owns its own tree:
 
```
Instance (Root)
 └── Assignment / Ward (Container)  — phases: Intake -> Active -> Review -> Publish
      └── Submission / Treatment (Item) — states: Available -> Assigned -> InProgress -> Completed / Rejected
```
 
1. **Intake** — add assignments/wards and submissions/patients.
2. **Open** — the container becomes **Active** and a worker is assigned to every item by the chosen
   assignment **strategy** (Workload, RoundRobin, Random; switchable at runtime).
3. **Work** — the assignee starts and completes each item with a result (mark 0-100 / severity 0-10).
   When every item is complete the container moves to **Review**.
4. **Approve** — batch approval runs every result through the approval chain.
   - Education: 40-49 Moderator, 50-70 TA (never the marker), 71-100 Lecturer. Below 40 is **rejected**.
   - Healthcare: 1-3 Triage Nurse, 4-6 Admitting Doctor, 7-10 Chief of Medicine. **0 is rejected.**
5. **Exception path** — the Lecturer (education, 0-100) or Chief of Medicine (healthcare, 1-10) settles each rejected item.
6. **Publish** — when everything is settled the container publishes and an observer records the release notice
   (marks released / billing generated).
Everything that happens (commands, events, and refused actions as `Denied:` lines) is written to the
instance's audit history, and a progress report is available per instance.
 
---
 
## REST API
 
Base: `http://localhost:8080`. Every route exists for both `/api/education` and `/api/healthcare`.
Full contract: [`FRONTEND_API.md`](FRONTEND_API.md).
 
| Method + path (after `/api/<scenario>`)                      | Purpose                                              |
| ------------------------------------------------------------ | ---------------------------------------------------- |
| `POST /start {name}`                                         | create an instance -> `{instanceId}`                 |
| `GET /instances`                                             | list instance ids                                    |
| `DELETE /:id`                                                | delete an instance                                   |
| `GET /:id/state` (`?state=Rejected` to filter)               | the tree: containers (with `phase`) and items        |
| `GET /:id/history`                                           | audit history                                        |
| `GET /:id/progress`                                          | monitoring figures                                   |
| `POST /:id/submit` (education) / `/admit` (healthcare)       | add an assignment / ward `{containerName}`           |
| `POST /:id/containers/:cid/open`                             | Intake -> Active, assign workers                     |
| `POST /:id/containers/:cid/submissions` / `/patients`        | add a submission / patient                           |
| `POST /:id/containers/:cid/approve`                          | batch approval                                       |
| `POST /:id/items/:itemId/start`                              | start an item                                        |
| `POST /:id/items/:itemId/complete {value}`                   | record the result                                    |
| `POST /:id/items/:itemId/override {value, accept}`           | settle a rejected item                               |
| `GET` / `PUT /:id/strategy {strategy}`                       | read / change the assignment strategy                |
 
Replies are `{"result": "Success|InvalidTransition|Rejected|NotFound", "message": "why"}` with HTTP 200 / 409 / 409 / 404.
 
---
 
## Design patterns
 
Twelve GoF patterns, all executed by the running system.
Participant mapping and justification: [`docs/PATTERNS.md`](docs/PATTERNS.md).
 
| Category     | Patterns                                                                        |
| ------------ | ------------------------------------------------------------------------------- |
| Creational   | Abstract Factory, Builder (+ Director)                                          |
| Structural   | Composite, Facade                                                               |
| Behavioural  | State, Strategy, Iterator, Command, Mediator, Observer, Chain of Responsibility, Visitor |
 
---
 
## Repository layout
 
```
tividy/
├── CMakeLists.txt                  # explicit source and test lists (a missing file is a build error)
├── Dockerfile                      # backend image
├── docker-compose.yml              # backend + frontend
├── README.md
├── FRONTEND_API.md                 # API contract
├── .github/workflows/ci.yml        # CI: backend, frontend, live contract test, docker
├── app/main_web.cpp                # server entry point
├── member1_core/ ... member7_integration/    # each member owns one folder (headers in include/)
├── tests/                          # GoogleTest suites
├── frontend/                       # React UI (Vite), Dockerfile, nginx.conf, tests
└── docs/                           # UML exports, PATTERNS.md, design documents, research
```
 
**All member folders are on the compiler's include path**, so `#include "Item.h"` works from any folder.
**Header names must be globally unique** (use `EducationPolicies.h`, `HospitalPolicies.h`, never `Policies.h` twice).
 
---
 
## Git workflow (team rules)
 
- Never push directly to `dev` or `main`. Work on a `feat/...` or `fix/...` branch and open a **pull request**.
- Before opening a PR, bring your branch up to date and test the result:
```bash
  git fetch origin
  git merge origin/dev
  rm -rf build && cmake -S . -B build && cmake --build build && ctest --test-dir build
```
- CI must be green. A green PR is safe to merge, a red PR is not.
- Keep commits small and meaningful; every member must contribute commits, tests and at least one reviewed PR.
### CI (GitHub Actions)
 
Every push and pull request to `main` or `dev` runs `.github/workflows/ci.yml`:
 
1. **backend** — configure, build the server and tests, run `ctest`, upload the `tividy` binary.
2. **frontend** — `npm ci`, lint, build, unit tests, then the live contract test against the built server.
3. **docker** — `docker compose up --wait` and a smoke test through nginx.
### Audit before pushing
 
```bash
# (a) every .cpp on disk must be listed in CMakeLists.txt — empty output = OK
comm -23 <(ls member*_*/*.cpp app/*.cpp | sort) \
         <(grep -oE '(member[0-9]+_[a-z]+|app)/[A-Za-z_]+\.cpp' CMakeLists.txt | sort -u)
 
# (b) no file listed twice — empty output = OK
grep -oE 'member[0-9]+_[a-z]+/[A-Za-z_]+\.cpp' CMakeLists.txt | sort | uniq -d
 
# (c) every test file on disk must be listed in CMakeLists.txt — empty output = OK
comm -23 <(ls tests/test_*.cpp | sort) <(grep -oE 'tests/test_[a-z_]+\.cpp' CMakeLists.txt | sort -u)
```
 
---