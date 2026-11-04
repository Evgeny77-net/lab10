cmake_minimum_required(VERSION 3.16)
project(CalculatorApp LANGUAGES C CXX)

# --- Стандарт C++17 ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Автоматическое скачивание зависимостей через FetchContent ---
include(FetchContent)

# GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
# Статическая сборка GLFW (без отдельной DLL на Windows)
set(BUILD_SHARED_LIBS   OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# Dear ImGui (дистрибутив с уже готовыми backend'ами для GLFW + OpenGL3)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.90.9-docking
)
FetchContent_GetProperties(imgui)
if(NOT imgui_POPULATED)
    FetchContent_Populate(imgui)
endif()

# На Windows: WIN32 -> gui_app (WinMain), чтобы консольное окно не мелькало.
# На Linux/macOS сборку выполняем как обычно, без этого флага.
if(WIN32)
    add_executable(CalculatorApp WIN32 MACOSX_BUNDLE
        main.cpp
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    )
else()
    add_executable(CalculatorApp MACOSX_BUNDLE
        main.cpp
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    )
endif()

target_include_directories(CalculatorApp PRIVATE
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

# OpenGL: на Windows линкуем opengl32/gdi32, на Linux — через find_package(OpenGL)
find_package(OpenGL REQUIRED)
if(WIN32)
    target_link_libraries(CalculatorApp PRIVATE glfw OpenGL::GL gdi32)
else()
    target_link_libraries(CalculatorApp PRIVATE glfw OpenGL::GL m dl pthread)
endif()

# --- Флаги статической линковки для Windows (MinGW / Code::Blocks) ---
# Готовый .exe запускается автономно, без поиска внешних DLL (libgcc, libstdc++, winpthread)
if(WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_link_options(CalculatorApp PRIVATE
        -static-libgcc
        -static-libstdc++
        -static
        "-Wl,-Bstatic,--whole-archive" -lwinpthread "-Wl,--no-whole-archive"
    )
endif()
