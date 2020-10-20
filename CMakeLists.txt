cmake_minimum_required(VERSION 3.14)

project(RuneWarsNA VERSION 20201010.1)

set(SWE_WITH_JSON ON CACHE BOOL "enable json" FORCE)
set(SWE_WITH_LUA OFF CACHE BOOL "disable lua" FORCE)
set(SWE_EXAMPLES OFF CACHE BOOL "disable examples" FORCE)
set(SWE_DISABLE_TERMGUI OFF CACHE BOOL "disable build termwin_gui" FORCE)

add_subdirectory(engine)

include(engine/libswe.cmake)

set(RWNA_SOURCE
    src/game/strings.cpp
    src/game/gamedata.cpp
    src/game/runewars.cpp
    src/game/gametheme.cpp
    src/game/gameobjects.cpp
    src/game/settings.cpp
    src/game/aiturn.cpp
    src/game/battle.cpp
    src/game/dialogs.cpp
    src/screen/adventurepart.cpp
    src/screen/battlesummarypart.cpp
    src/screen/mahjongsummarypart.cpp
    src/screen/gamesummarypart.cpp
    src/screen/mahjongpart.cpp
    src/screen/jsongui.cpp
    src/screen/showplayers.cpp
    src/screen/selectperson.cpp)

include_directories(engine src/game src/screen)
add_executable(RuneWarsNA ${RWNA_SOURCE})

target_link_libraries(RuneWarsNA libswe)
set_target_properties(RuneWarsNA PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
