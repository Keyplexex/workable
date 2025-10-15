file(GLOB_RECURSE SRC_FILES_LIB
#    "${CMAKE_CURRENT_SOURCE_DIR}/lexer/token.cpp"
#    "${CMAKE_CURRENT_SOURCE_DIR}/lexer/lexer.cpp"
#    "${CMAKE_CURRENT_SOURCE_DIR}/parser/ASTNode.cpp"
#    "${CMAKE_CURRENT_SOURCE_DIR}/parser/parser.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/lexer/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/parser/*.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/interpreter/*.cpp"
)

add_library(itmoscript STATIC ${SRC_FILES_LIB})

target_include_directories(itmoscript PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lexer
    ${CMAKE_CURRENT_SOURCE_DIR}/parser
    ${CMAKE_CURRENT_SOURCE_DIR}/interpreter
)