add_executable(unit unit.c mock_i2c.c)
target_link_libraries(unit PRIVATE pcf8563)
target_compile_options(unit PRIVATE
    -Wmissing-declarations
    -Wmissing-prototypes
    -Wstrict-prototypes
)

add_test(NAME unit COMMAND unit)
