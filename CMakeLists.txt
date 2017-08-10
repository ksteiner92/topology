cmake_minimum_required (VERSION 2.8)

# Project name
project (edge)
project (old)

option(USE_OPENMP "use OpenMP" ON)
option(USE_MPI "use MPI" ON)
option(USE_CLANG "build application with clang" ON)
option(BUILD_DOC "Build documentation" ON)

set(INTEL_ROOT "/opt/intel/compilers_and_libraries/linux" CACHE STRING "Intel root directory")
set(BISON "/usr/bin/bison" CACHE STRING "Path to bison executable")

# Check the C++14 support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++1y" COMPILER_SUPPORTS_CXX14)
if(COMPILER_SUPPORTS_CXX14)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++14 support. Please use a different C++ compiler.")
endif()

#CXX flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -DMKL_LP64")
if(USE_OPENMP AND NOT USE_MPI)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
elseif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -qopenmp")
endif()

# Add include directories
include_directories(include src ${INTEL_ROOT}/mkl/include ${CMAKE_SOURCE_DIR} ${INTEL_ROOT}/mpi/include64)
link_directories(${INTEL_ROOT}/mpi/lib64)

# Clang compiler option
# ON is the default
if(USE_CLANG AND NOT USE_MPI)
    SET (CMAKE_C_COMPILER clang)
    SET (CMAKE_CXX_COMPILER clang++)
elseif(NOT USE_CLANG AND NOT USE_MPI)
    SET (CMAKE_C_COMPILER gcc)
    SET (CMAKE_CXX_COMPILER g++)
else()
    SET (CMAKE_C_COMPILER mpicc)
    SET (CMAKE_CXX_COMPILER mpicxx)
endif()

set(ALGORITHM_SRC src/algorithm/sw.cc src/algorithm/tb.cc)
set(PARSER_SRC src/parser/config/config.cc src/parser/sw/selfenergy.cc src/parser/hr/tbh.cc src/parser/kpath/kpath.cc)
set(MODEL_SRC src/model/aw.cc src/model/bs.cc src/model/dos.cc src/model/chern.cc src/model/z2.cc)
set(SRC src/model/modelfactory.cc src/quan/quantityhandler.cc src/quan/anasw.cc src/edge.cc src/logger.cc src/quan/quantities.cc src/quan/rgrid.cc src/quan/kpathbase.cc)

#generate parsers
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED 3.0.0)
if(BISON_FOUND AND FLEX_FOUND)
    FLEX_TARGET(HrScanner ${CMAKE_SOURCE_DIR}/src/parser/hr/hrscanner.l ${CMAKE_SOURCE_DIR}/src/parser/hr/hrscanner.cc)
    BISON_TARGET(HrParser ${CMAKE_SOURCE_DIR}/src/parser/hr/hrparser.y ${CMAKE_SOURCE_DIR}/src/parser/hr/hrparser.cc)

    FLEX_TARGET(ConfScanner ${CMAKE_SOURCE_DIR}/src/parser/config/confscanner.l ${CMAKE_SOURCE_DIR}/src/parser/config/confscanner.cc)
    BISON_TARGET(ConfParser ${CMAKE_SOURCE_DIR}/src/parser/config/confparser.y ${CMAKE_SOURCE_DIR}/src/parser/config/confparser.cc)

    FLEX_TARGET(KPathScanner ${CMAKE_SOURCE_DIR}/src/parser/kpath/kscanner.l ${CMAKE_SOURCE_DIR}/src/parser/kpath/kscanner.cc)
    BISON_TARGET(KPathParser ${CMAKE_SOURCE_DIR}/src/parser/kpath/kparser.y ${CMAKE_SOURCE_DIR}/src/parser/kpath/kparser.cc)

    FLEX_TARGET(SWScanner ${CMAKE_SOURCE_DIR}/src/parser/sw/swscanner.l ${CMAKE_SOURCE_DIR}/src/parser/sw/swscanner.cc)
    BISON_TARGET(SWParser ${CMAKE_SOURCE_DIR}/src/parser/sw/swparser.y ${CMAKE_SOURCE_DIR}/src/parser/sw/swparser.cc)

    ADD_FLEX_BISON_DEPENDENCY(HrScanner HrParser)
    ADD_FLEX_BISON_DEPENDENCY(ConfScanner ConfParser)
    ADD_FLEX_BISON_DEPENDENCY(KPathScanner KPathParser)
    ADD_FLEX_BISON_DEPENDENCY(SWScanner SWParser)
    set(PARSER_SRC ${PARSER_SRC} ${BISON_SWParser_OUTPUTS} ${FLEX_SWScanner_OUTPUTS}
        ${BISON_KPathParser_OUTPUTS} ${FLEX_KPathScanner_OUTPUTS} ${BISON_HrParser_OUTPUTS}
        ${FLEX_HrScanner_OUTPUTS} ${BISON_ConfParser_OUTPUTS} ${FLEX_ConfScanner_OUTPUTS})
else()
    message("No Bison or Flex could be found")
endif()

add_executable(edge ${ALGORITHM_SRC} ${PARSER_SRC} ${SRC} ${MODEL_SRC})
add_executable(old src/edgeold.cc)

set(LIBRARIES mkl_intel_thread mkl_core mkl_intel_lp64 iomp5 pthread m dl)
if (NOT USE_MPI)
    set(LIBRARIES ${LIBRARIES} mpi)
endif()
target_link_libraries(edge ${LIBRARIES})
target_link_libraries(old ${LIBRARIES})

find_package(Doxygen)
if (DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/doc/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    add_custom_target( doc_doxygen ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM )
else (DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)