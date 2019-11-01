generic_bison(java_signature)
generic_flex(java_signature)

file(GLOB_RECURSE sources "*.cpp" "*.h")
add_library(java_signature
    ${sources}
    ${BISON_parser_OUTPUTS}
    ${FLEX_scanner_OUTPUTS}
)

generic_includes(java_signature)

target_link_libraries(java_signature util)
