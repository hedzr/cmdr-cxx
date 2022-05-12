
# just a sample


find_package(BISON)
find_package(FLEX)

FLEX_TARGET(MyScanner
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scanner/lex.l
    ${CMAKE_CURRENT_BINARY_DIR}/my.lex.cpp COMPILE_FLAGS -w)
BISON_TARGET(MyParser
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scanner/parser.y
    ${CMAKE_CURRENT_BINARY_DIR}/my.parser.cpp)
ADD_FLEX_BISON_DEPENDENCY(MyScanner MyParser)
