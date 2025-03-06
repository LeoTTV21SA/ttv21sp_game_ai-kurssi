# Specify minimum supported cmake version
cmake_minimum_required(VERSION 3.10)

# Project TTV21SPPelienTekoaly
project(TTV21SPPelienTekoaly)

## Use C++20:
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ulkoiset kirjastot:
set(MIKROPLOT_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory("ext/mikroplot-main")
set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory("ext/SFML-2.6.0")

# Include polut + filet:
include_directories("include")
file(GLOB_RECURSE GAME_AI_INC_FILES "./include/*.h")

# suoritattavat sovellukset:
add_executable(1_blind_search notebooks/1_blind_search.cpp ${GAME_AI_INC_FILES})
target_link_libraries(1_blind_search sfml-graphics)

add_executable(Sudoku_Solver notebooks/Sudoku_Solver.cpp ${GAME_AI_INC_FILES})
target_link_libraries(Sudoku_Solver sfml-graphics)

add_executable(main_exercise_fsm submissions/main_exercise_fsm.cpp)
target_link_libraries(main_exercise_fsm sfml-graphics)

add_executable(main_exercise_astar submissions/main_exercise_astar.cpp)
target_link_libraries(main_exercise_astar mikroplot)

add_executable(exer_astar submissions/exer_astar.cpp  ${GAME_AI_INC_FILES} )
target_link_libraries(exer_astar mikroplot)

add_executable(practical_work submissions/practical_work.cpp  ${GAME_AI_INC_FILES} )
target_link_libraries(practical_work mikroplot)

add_executable(2_grid_search notebooks/2_grid_search.cpp  ${GAME_AI_INC_FILES} )
target_link_libraries(2_grid_search sfml-graphics)

add_executable(paths notebooks/paths.cpp include/agents.h)
# Copy assets from "assets" folder to output binary directory on post build step:
add_custom_command(TARGET 1_blind_search POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${CMAKE_CURRENT_SOURCE_DIR}/assets"
	"${PROJECT_BINARY_DIR}/assets"
	COMMENT "Copying asset files to binary directory")
