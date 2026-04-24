cmake_minimum_required(VERSION 3.10)

project(photocomm C)

# Set Directory to FFMPEG libs
# set(FFMPEG_DIR "/opt/homebrew/Cellar/ffmpeg/8.1_1")

# set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES "src/*.c")
file(GLOB_RECURSE HEADERS "include/*.h")

find_package(SDL2 REQUIRED COMPONENTS SDL2)
find_package(SDL2_ttf REQUIRED)

add_executable(transmitter src/main.c src/transmitter.c src/shared.c)

target_compile_definitions(transmitter PRIVATE TRANSMITTER)

target_include_directories(transmitter PRIVATE 
    #   "${FFMPEG_DIR}/include"
    "${CMAKE_SOURCE_DIR}/include"
)

target_link_libraries(transmitter PRIVATE
    #"${FFMPEG_DIR}/lib/libavcodec.dylib"
    #"${FFMPEG_DIR}/lib/libavformat.dylib"
    #"${FFMPEG_DIR}/lib/libavutil.dylib"
    #"${FFMPEG_DIR}/lib/libswscale.dylib"
    #"${FFMPEG_DIR}/lib/libswresample.dylib"
    SDL2::SDL2
    SDL2_ttf::SDL2_ttf
)

add_executable(receiver src/main.c src/receiver.c src/shared.c)

target_compile_definitions(receiver PRIVATE RECEIVER)

target_include_directories(receiver PRIVATE 
    #"${FFMPEG_DIR}/include"
    "${CMAKE_SOURCE_DIR}/include"
)

target_link_libraries(receiver PRIVATE
    #"${FFMPEG_DIR}/lib/libavcodec.dylib"
    #"${FFMPEG_DIR}/lib/libavformat.dylib"
    #"${FFMPEG_DIR}/lib/libavutil.dylib"
    #"${FFMPEG_DIR}/lib/libswscale.dylib"
    #"${FFMPEG_DIR}/lib/libswresample.dylib"
    SDL2::SDL2
    SDL2_ttf::SDL2_ttf
    lgpio
)
