if(MSVC)
	add_definitions(/W0)
else()
	add_definitions(-w)
endif()

# Check if Catch2 has already been added
if(NOT TARGET Catch2::Catch2)
    add_subdirectory(catch2)
endif()