cmake_minimum_required(VERSION 3.14)

project(RuneWarsNA VERSION 20201010.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SWE_WITH_JSON ON CACHE BOOL "enable json" FORCE)
set(SWE_WITH_LUA OFF CACHE BOOL "disable lua" FORCE)
set(SWE_EXAMPLES OFF CACHE BOOL "disable examples" FORCE)
set(SWE_DISABLE_TERMGUI OFF CACHE BOOL "enable termwin_gui" FORCE)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DBUILD_DEBUG")

add_subdirectory(engine)

include(engine/libswe.cmake)

set(RWNA_SOURCE
    src/strings.cpp
    src/gamedata.cpp
    src/runewars.cpp
    src/gametheme.cpp
    src/gameobjects.cpp
    src/settings.cpp
    src/aiturn.cpp
    src/battle.cpp
    src/dialogs.cpp
    src/adventurepart.cpp
    src/battlesummarypart.cpp
    src/mahjongsummarypart.cpp
    src/gamesummarypart.cpp
    src/mahjongpart.cpp
    src/jsongui.cpp
    src/showplayers.cpp
    src/selectperson.cpp)

include_directories(engine src)
add_executable(RuneWarsNA ${RWNA_SOURCE})

target_link_libraries(RuneWarsNA libswe)
set_target_properties(RuneWarsNA PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
