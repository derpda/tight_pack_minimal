cmake_minimum_required(VERSION 3.9)
project(tight_packing_minimal)

#################### Configure options                      ####################
set(PLATFORM CACHE STRING "FPGA platform for which to build")
option(TIGHT_PACKING "Whether to use tight packing (new HLSLIB branch)" OFF)
if(${TIGHT_PACKING})
  set(GIT_BRANCH "tight-packing")
else()
  set(GIT_BRANCH "master")
endif()


#################### Vitis setup                            ####################
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(find_or_download)

find_or_download(hlslib XILINX_VERSION_DEPENDENT)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR}/dependencies/hlslib/cmake)

get_filename_component(ENV_VITIS_VERSION $ENV{XILINX_HLS} NAME)
find_package(Vitis REQUIRED VERSION EXACT ${ENV_VITIS_VERSION})


#################### Software/host build                    ####################
add_executable(sw_emu sw_emu.cpp kernel.cpp)
add_executable(host host.cpp)
foreach(HOST_EXEC sw_emu host)
  target_compile_features(${HOST_EXEC} PRIVATE cxx_std_17)
  target_compile_options(${HOST_EXEC} PRIVATE -O3)
  target_link_libraries(${HOST_EXEC} PRIVATE hlslib)
endforeach()


#################### Vitis compiler options                 ####################
set(KERNEL_NAME test_kernel)
list(APPEND VITIS_FLAGS
  --kernel ${KERNEL_NAME}
  --target hw
  --platform ${PLATFORM}
  --connectivity.nk ${KERNEL_NAME}:1:${KERNEL_NAME}_1
  --advanced.prop kernel.${KERNEL_NAME}.kernel_flags=-std=c++14
  -DHLSLIB_SYNTHESIS
  --optimize 3
  --include ${CMAKE_BINARY_DIR}/dependencies/hlslib/include
  --log_dir ${CMAKE_BINARY_DIR}/logs
  --report_dir ${CMAKE_BINARY_DIR}/reports
)


#################### Hardware build                         ####################
file(GLOB HEADERS "${CMAKE_SOURCE_DIR}/*.h")
set(XO_FILE hw.xo)
add_custom_command(
  COMMENT   "Compiling for ${PLATFORM}."
  OUTPUT    ${XO_FILE}
  COMMAND   ${Vitis_COMPILER} ${VITIS_FLAGS} -c ${CMAKE_SOURCE_DIR}/kernel.cpp -o ${XO_FILE}
  DEPENDS   ${CMAKE_SOURCE_DIR}/kernel.cpp ${HEADERS}
)
add_custom_target(compile_hw DEPENDS ${XO_FILE})
set(XCLBIN_FILE hw.xclbin)
add_custom_command(
  COMMENT   "Linking against ${PLATFORM}."
  OUTPUT    ${XCLBIN_FILE}
  COMMAND   ${Vitis_COMPILER} ${VITIS_FLAGS} -l ${XO_FILE} -o ${XCLBIN_FILE}
  DEPENDS   ${XO_FILE}
)
add_custom_target(link_hw DEPENDS ${XCLBIN_FILE})
add_custom_target(hw DEPENDS host link_hw)
