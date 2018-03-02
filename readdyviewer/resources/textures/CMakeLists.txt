set (texture_files framing.png Frank_Noe.png)

foreach(item IN ITEMS ${texture_files})
    list(APPEND textures_out "${CMAKE_CURRENT_BINARY_DIR}/${item}")
    add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${item}"
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${item}" "${CMAKE_CURRENT_BINARY_DIR}/${item}"
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${item}"
    )
endforeach()

message(STATUS "Textures: ${textures_out}")

add_custom_target(textures-target DEPENDS "${textures_out}")
