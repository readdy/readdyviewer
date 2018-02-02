set(shader_files
        "framing/fragment.glsl" "framing/vertex.glsl"
        "light/light.glsl"
        "particles/fragment.glsl" "particles/vertex.glsl"
        "edge/fragment.glsl" "edge/vertex.glsl")

foreach (item IN ITEMS ${shader_files})
    list(APPEND shaders_out "${CMAKE_CURRENT_BINARY_DIR}/${item}")
    add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${item}"
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${item}" "${CMAKE_CURRENT_BINARY_DIR}/${item}"
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${item}"
    )
endforeach ()

message(STATUS "Shaders: ${shaders_out}")

add_custom_target(shaders-target DEPENDS "${shaders_out}")
