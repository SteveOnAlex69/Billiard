cmake_minimum_required(VERSION 3.16)
project(GoGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# --- FORCE OUTPUT TO "build" FOLDER ---

# 1. Define the output path
set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/build")

# 2. Force ALL configurations (Debug, Release) to go to the same place
# If you don't do this, VS creates "bin/Debug/GoGame.exe"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR})

# 3. (Optional) Also put DLLs (.lib files) there if needed
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${OUTPUT_DIR})

#Main execcutable file
add_executable(GoGame "src/main.cpp")

target_include_directories(GoGame PRIVATE
	${CMAKE_SOURCE_DIR}/include
)
target_sources(GoGame PRIVATE 
	"src/GameLogic.cpp" "src/Helper.cpp" "src/World.cpp")


set(SFML_DIR "${CMAKE_SOURCE_DIR}/SFML/lib/cmake/SFML")

#Inputing the folder for accessing assets
target_compile_definitions(GoGame PRIVATE PROJECT_DIR="${CMAKE_SOURCE_DIR}/")

#SFML package linking
find_package(SFML 3 REQUIRED COMPONENTS Audio Graphics Network Window System)
target_link_libraries(GoGame PRIVATE SFML::Audio SFML::Graphics SFML::Network SFML::Window SFML::System)

#Copying SFML files into the build files
file(GLOB SFML_DLLS "${CMAKE_SOURCE_DIR}/SFML/bin/*.dll")

# Create a permanent rule for the Builder -> No need to manually copy dll files
add_custom_command(TARGET GoGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${SFML_DLLS}
    $<TARGET_FILE_DIR:GoGame>
    COMMENT "Copying SFML DLLs to the build folder..."
)