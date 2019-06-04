message( STATUS "${Gn}Generating runner.cpp for the cxxtest framework...${Na}")
if(EXISTS "${CXXTEST_OUTPUT}")
  file(REMOVE "${CXXTEST_OUTPUT}")
  message( STATUS "${Ye}Removed old runner.cpp${Na}")
endif()
execute_process(
  COMMAND python --version
)
execute_process(
  COMMAND python cxxtestgen --version
  WORKING_DIRECTORY "${CXXTEST_PATH}/bin"
)
execute_process(
  COMMAND python cxxtestgen --runner=ParenPrinter --no-static-init --no-eh --no-std --output=${CXXTEST_OUTPUT} --world=${PROJECT_NAME} ${TEST_HEADER_FILES}
  WORKING_DIRECTORY "${CXXTEST_PATH}/bin"
)
if(NOT EXISTS "${CXXTEST_OUTPUT}")
  message( STATUS "${Ye}Could not generate runner.cpp. Tests will not be available! Do you miss the required PYTHON installation?${Na}")
endif()
