option(PL_GENERATE_HRE_TASK "PL_VHDL_PARSER" ON)
if(PL_GENERATE_HRE_TASK OR BUILD_ALL_PLUGINS)
    file(GLOB_RECURSE GENERATE_HRE_TASK_INC ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
    file(GLOB_RECURSE GENERATE_HRE_TASK_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

    hal_add_plugin(generate_hre_task
                   SHARED
                   HEADER ${GENERATE_HRE_TASK_INC}
                   SOURCES ${GENERATE_HRE_TASK_SRC}
                   LINK_LIBRARIES PUBLIC gui netlist_simulator_controller
                   )
endif()
