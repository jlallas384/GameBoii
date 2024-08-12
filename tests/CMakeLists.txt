find_package(nlohmann_json)
find_package(GTest)

if(nlohmann_json_FOUND AND GTest_FOUND)
	add_executable(cputest)

	target_sources(cputest PRIVATE
		../src/cpu/cpu.h
		../src/cpu/alu.cpp
		../src/cpu/alu.h
		../src/cpu/flags.h
		../src/address_bus.cpp
		../src/address_bus.h
		cputest.cpp
	)

	target_link_libraries(cputest PRIVATE nlohmann_json::nlohmann_json GTest::GTest)
	file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

endif()


