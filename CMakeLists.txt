cmake_minimum_required(VERSION 3.16)
project(Test1 VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =============== 源文件列表 ===============
set(SOURCES
    main.cpp
    GameDestroy.cpp
    GameInit.cpp
    GameRun.cpp
    Resources.cpp
    Render.cpp
)

# =============== 可执行文件 ===============
add_executable(main ${SOURCES})

# =============== 头文件目录（当前目录） ===============
target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR})

# =============== OpenGL ===============
find_package(OpenGL REQUIRED)
target_link_libraries(main OpenGL::GL)

# =============== SFML ===============
set(SFML_ROOT /home/snake/code/cpp/LIBS/SFML/SFML-3.0.2)
target_include_directories(main PRIVATE ${SFML_ROOT}/include)
target_link_directories(main PRIVATE ${SFML_ROOT}/lib)
target_link_libraries(main
    sfml-graphics
    sfml-window
    sfml-system
)
# =============== Box2D ===============
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BOX2D_BUILD_DIR /home/snake/code/cpp/LIBS/box2d/box2d-main3.1.1/buildD)
    set(BOX2D_LIB box2dd)
else()
    set(BOX2D_BUILD_DIR /home/snake/code/cpp/LIBS/box2d/box2d-main3.1.1/buildR)
    set(BOX2D_LIB box2d)
endif()

target_include_directories(main PRIVATE /home/snake/code/cpp/LIBS/box2d/box2d-main3.1.1/include)
target_link_directories(main PRIVATE ${BOX2D_BUILD_DIR}/bin)
target_link_libraries(main ${BOX2D_LIB})
set(CMAKE_BUILD_RPATH "${SFML_ROOT}/lib:${BOX2D_BUILD_DIR}/bin")

# =============== ImGui + ImGui-SFML ===============
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui-SFML.cpp
)
target_sources(main PRIVATE ${IMGUI_SOURCES})
target_include_directories(main PRIVATE ${IMGUI_DIR})

# =============== 复制 Assets 文件夹 ===============
set(ASSETS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/Assets)
set(ASSETS_BINARY_DIR ${CMAKE_BINARY_DIR}/Assets)
if(EXISTS ${ASSETS_SOURCE_DIR})
    add_custom_target(copy_assets ALL
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${ASSETS_BINARY_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${ASSETS_SOURCE_DIR} ${ASSETS_BINARY_DIR}
        COMMENT "Copying Assets directory to build folder"
    )
    add_dependencies(main copy_assets)
else()
    message(WARNING "Assets directory not found at ${ASSETS_SOURCE_DIR}")
endif()