# mkdir build
# cd build
# cmake -G "MinGW Makefiles" ..
# cmake -DCMAKE_BUILD_TYPE=Debug -G "MinGW Makefiles" ..
# make -j4
# or,
# mingw32-make -j4

cmake_minimum_required(VERSION 3.0)
project(html2csv)

add_executable(html2csv src/main.c)

# Add compiler flags for GCC and Clang
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(html2csv PRIVATE -Wall -Wextra)
endif()

# Add compiler flags for MSVC
if(MSVC)
  target_compile_options(html2csv PRIVATE /W4)
endif()

# Set linker flags for all builds
if(WIN32)
  # For Windows systems (MinGW-w64 or MSVC)
  target_link_options(html2csv PRIVATE -static-libgcc -static-libstdc++)
  if(MSVC)
    # For MSVC on Windows
    target_compile_options(html2csv PRIVATE /W4 /WX)
  endif()
else()
  # For non-Windows systems (GCC or Clang)
  target_link_options(html2csv PRIVATE -static)
  target_compile_options(html2csv PRIVATE -Werror)
endif()

# Set build type to Release by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Set compiler and linker flags for Debug and Release builds
target_compile_options(html2csv PRIVATE
  $<$<CONFIG:Debug>:-O0 -g>
  $<$<CONFIG:Release>:-O3>
)

if(WIN32 AND MSVC)
  target_link_options(html2csv PRIVATE
    $<$<CONFIG:Debug>:/DEBUG:FULL>
    $<$<CONFIG:Release>:/DEBUG:NONE>
  )
else()
  target_link_options(html2csv PRIVATE
    # $<$<CONFIG:Debug>:-Wl,--export-all-symbols>
  )
endif()

