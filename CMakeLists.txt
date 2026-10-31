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
# All production code except the app entry point.
add_library(tividy_core
    # --- Member 1: core model ---
    member1_core/Colleague.cpp # # uncomment this line & more
    member1_core/Component.cpp
    member1_core/Container.cpp
    member1_core/Item.cpp
    member1_core/Participant.cpp
    member1_core/ParticipantRegistry.cpp
    member1_core/Root.cpp
    member1_core/WorkRegistry.cpp
    member1_core/WorkflowInstance.cpp
    member7_integration/JsonSerializer.cpp

    # --- Member 2: execution & lifecycle ---
    member2_execution/ItemState.cpp
    member2_execution/CreatedState.cpp
    member2_execution/AssignedState.cpp
    member2_execution/InProgressState.cpp
    member2_execution/CompletedState.cpp
    member2_execution/RejectedState.cpp
    member2_execution/IntakePhase.cpp
    member2_execution/ActivePhase.cpp
    member2_execution/ReviewPhase.cpp
    member2_execution/PublishPhase.cpp

    # --- Member 3: assignment & traversal ---
    member3_routing/AssignmentEngine.cpp # # uncomment this line & more
    member3_routing/RandomStrategy.cpp 
    member3_routing/RoundRobinStrategy.cpp 
    member3_routing/WorkloadStrategy.cpp 
    member3_routing/Iterator.cpp
    member3_routing/DepthFirstIterator.cpp
     member3_routing/StateFilterIterator.cpp

    # --- Member 4: commands, events, history ---
    member4_services/WorkflowMediator.cpp

    # --- Member 5: education scenario ---
    member5_education/AssignmentMarkingBuilder.cpp
    member5_education/AssignmentMarkingCoordinator.cpp
    member5_education/AssignmentMarkingDirector.cpp
    member5_education/EducationApprovalChain.cpp
    member5_education/EducationPolicies.cpp
    member5_education/EducationSeed.cpp
    member5_education/Submission.cpp

    # --- Member 6: hospital scenario ---
    member6_hospital/Treatment.cpp
    member6_hospital/HospitalPolicies.cpp
    member6_hospital/HospitalApprovalChain.cpp
    member6_hospital/HospitalAdmissionBuilder.cpp
    member6_hospital/HospitalAdmissionDirector.cpp
    member6_hospital/HospitalAdmissionCoordinator.cpp
    member6_hospital/HospitalSeed.cpp

    # --- Member 7: integration, factories ---
    member7_integration/WorkflowDirector.cpp
    member7_integration/EducationWorkflowFactory.cpp
    member7_integration/HospitalWorkflowFactory.cpp
    member7_integration/TividyWorkflow.cpp
    member7_integration/JsonSerializer.cpp
    member7_integration/RestServer.cpp
    member7_integration/EducationFacade.cpp
    member7_integration/HospitalFacade.cpp    
)

# Every member folder is on the include path, so `#include "Header.h"`
# finds any header regardless of which folder it lives in.
target_include_directories(tividy_core PUBLIC
    member1_core
    member1_core/include
    member2_execution
    member2_execution/include
    member3_routing
    member3_routing/include
    member4_services
    member4_services/include
    member5_education
    member5_education/include
    member6_hospital
    member6_hospital/include
    member7_integration
    member7_integration/include
)
# ----------------- Application -----------------
add_executable(tividy app/main_web.cpp)
target_link_libraries(tividy PRIVATE tividy_core)

# ----------------- Tests -----------------
enable_testing()

add_executable(tividy_tests
    tests/test_assignment.cpp
    tests/test_commands.cpp
    tests/test_education.cpp
    tests/test_events.cpp
    tests/test_history.cpp
    tests/test_hospital.cpp
    # tests/test_integration.cpp
    tests/test_iterators.cpp
    tests/test_json_serializer.cpp
    tests/test_participants.cpp
    tests/test_smoke.cpp
    tests/test_states.cpp
    tests/test_workcontainer.cpp
    tests/test_workflowdefinition.cpp
    tests/test_workflowengine.cpp
    tests/test_workflowinstance.cpp
    tests/test_workitem.cpp
)

target_link_libraries(tividy_tests PRIVATE tividy_core gtest_main)

target_include_directories(tividy_tests PRIVATE
    member1_core
    member1_core/include
    member2_execution
    member2_execution/include
    member3_routing
    member3_routing/include
    member4_services
    member4_services/include
    member5_education
    member5_education/include
    member6_hospital
    member6_hospital/include
    member7_integration
    member7_integration/include
)

include(GoogleTest)
gtest_discover_tests(tividy_tests)