cmake_minimum_required(VERSION 3.14)
project(TIVIDY CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ----------------- GoogleTest -----------------
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# ----------------- Core library -----------------
# All production code except app/main.cpp.
add_library(tividy_core
    # --- Member 1: core model ---
    # member1_core/Colleague.cpp # # uncomment this line & more
    # member1_core/Component.cpp
    # member1_core/Container.cpp
    # member1_core/Item.cpp
    member1_core/Participant.cpp
    member1_core/ParticipantRegistry.cpp
    # member1_core/Root.cpp
    # member1_core/WorkRegistry.cpp

    # --- Member 2: execution & lifecycle ---
    # member2_execution/WorkflowInstance.cpp # # uncomment this line & more

    # --- Member 3: assignment & traversal ---
    # member3_routing/AssignmentEngine.cpp # # uncomment this line & more

    # --- Member 4: commands, events, history ---
    # member4_services/WorkCommand.cpp # # uncomment this line & more

    # --- Member 5: education scenario ---
    # member5_education/AssignmentMarkingBuilder.cpp # # uncomment this line & more

    # --- Member 6: hospital scenario ---
    # member6_hospital/HospitalAdmissionBuilder.cpp # # uncomment this line & more

    # --- Member 7: integration, factories, decorators ---
    # member7_integration/WorkflowDirector.cpp # # uncomment this line & more
)

# Every member folder is on the include path, so `#include "Header.h"`
# finds any header regardless of which folder it lives in.
target_include_directories(tividy_core PUBLIC
    member1_core
    member1_core/include
    member2_execution
    member3_routing
    member4_services
    member5_education
    member6_hospital
    member7_integration
)

# ----------------- Application -----------------
add_executable(tividy app/main_web.cpp)
target_link_libraries(tividy PRIVATE tividy_core)

# ----------------- Tests -----------------
enable_testing()

add_executable(tividy_tests
    tests/test_workitem.cpp
    tests/test_workcontainer.cpp
    tests/test_workflowdefinition.cpp
    tests/test_workflowinstance.cpp
    tests/test_states.cpp
    tests/test_workflowengine.cpp
    tests/test_assignment.cpp
    tests/test_iterators.cpp
    tests/test_commands.cpp
    tests/test_events.cpp
    tests/test_history.cpp
    tests/test_education.cpp
    tests/test_hospital.cpp
    tests/test_integration.cpp

    tests/test_participants.cpp

    tests/test_smoke.cpp        # one trivial test so ctest has something to run
)

target_link_libraries(tividy_tests PRIVATE tividy_core gtest_main)

target_include_directories(tividy_tests PRIVATE
    member1_core
    member1_core/include
    member2_execution
    member3_routing
    member4_services
    member5_education
    member6_hospital
    member7_integration
)

include(GoogleTest)
gtest_discover_tests(tividy_tests)