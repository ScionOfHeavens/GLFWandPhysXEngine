cmake_minimum_required(VERSION 4.4)

project(GLFWandPhysXEngine)
add_executable(${PROJECT_NAME})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)

add_subdirectory("dependencies/glfw")
add_subdirectory("dependencies/glm")
add_subdirectory("dependencies/assimp")

add_library(GLAD STATIC)

target_sources(GLAD
  PRIVATE
    "dependencies/glad/src/glad.c"

  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS "dependencies/glad/include"
    FILES
      "dependencies/glad/include/KHR/khrplatform.h"
      "dependencies/glad/include/glad/glad.h"
)

add_library(STB STATIC)

target_sources(STB
  PRIVATE
    "dependencies/stb/src/stb_image.cpp"

  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS "dependencies/stb/include"
    FILES
      "dependencies/stb/include/stb_image.h"
)

add_library(physx INTERFACE)
option(PHYSX_BUILD_TYPE "The build type of PhysX, i.e., one of {debug, checked, profile, release}" "debug")
target_compile_options(physx INTERFACE /W3)

if(NOT CMAKE_BUILD_TYPE)
    if(PHYSX_BUILD_TYPE STREQUAL "debug" OR PHYSX_BUILD_TYPE STREQUAL "checked")
        set(CMAKE_BUILD_TYPE "Debug")
    endif()
endif()

target_include_directories(
  physx 
  INTERFACE
  "dependencies/physx/physx/include"
  )
target_link_directories(physx 
  INTERFACE
  "dependencies/physx/physx/bin/win.x86_64.vc143.md/${PHYSX_BUILD_TYPE}"
  )

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Building snippet in debug with PhysX ${PHYSX_BUILD_TYPE} configuration")
    target_compile_definitions(physx INTERFACE _DEBUG)
    # add_compile_definitions(_DEBUG)
else()
    message("Building snippet in release configuration with PhysX ${PHYSX_BUILD_TYPE} configuration")
    target_compile_definitions(physx INTERFACE _NDEBUG)
    # add_compile_definitions(NDEBUG)
endif()

target_link_libraries(physx
  INTERFACE
    PhysXExtensions_static_64 
    PhysX_64
    PhysXPvdSDK_static_64
    PhysXCommon_64
    PhysXFoundation_64
    )
    
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/dependencies/physx/physx/bin/win.x86_64.vc143.md/${PHYSX_BUILD_TYPE}"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    COMMENT "Copy PhysX runtime DLLs"
)

foreach(dll PhysX_64 PhysXCommon_64 PhysXFoundation_64)
    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/dependencies/physx/physx/bin/win.x86_64.vc143.md/${PHYSX_BUILD_TYPE}/${dll}.dll")
        message(FATAL_ERROR "Missing ${dll}.dll in PhysX ${PHYSX_BUILD_TYPE} folder")
    endif()
endforeach()

target_link_libraries(${PROJECT_NAME}
    PRIVATE

    physx
    glfw
    assimp
    GLAD
    STB
    glm::glm
)

file(GLOB PROJECT_HEADERS "include/*.hpp")
file(GLOB PROJECT_SOURCES "src/*.cpp")

target_sources(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCES}

    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS "include"
        FILES
            ${PROJECT_HEADERS}
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
    COMMENT "Copy assets directory"
)