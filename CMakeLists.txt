cmake_minimum_required(VERSION 3.1)
project(karougb)

# Make building tests optional
option(test "Build all tests." ON) # Makes boolean 'test' available.
enable_testing()

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH "C:\\Qt\\Qt5.10.1\\5.10.1\\msvc2017_64")

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)

include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Gui_INCLUDE_DIRS})

# Application source files
# Do *not* add main.cpp, it is added below. Otherwise we would get issues with the testsuit
set(SOURCES
	src/buttons.cpp                          
	src/cpu/cpu.cpp                          
	src/debug.cpp                         
	src/emulator.cpp                            
	src/gpu.cpp                                        
	src/mem/cartridge.cpp                    
	src/mem/cart_loader.cpp                  
	src/mem/mbc/cart_mbc0.cpp                
	src/mem/mbc/cart_mbc1.cpp                
	src/mem/mbc/cart_mbc3.cpp                
	src/mem/mbc/cart_mbc3_rtc.cpp            
	src/mem/mem.cpp                          
	src/os/osx/osx.cpp                                    
	src/timer.cpp                            
	src/timewarp.cpp 
	src/qt/keyeventmapper.cpp
	src/qt/mainwindow.cpp
	src/qt/qtioprovider.cpp
	src/qt/screenwidget.cpp
)

# Application headers
set(HEADERS
	src/buttons.h
	src/cpu/cpu.h
	src/cpu/cpu_inline.h
	src/cpu/cpu_mapping.h
	src/cpu/cpu_ops.h
	src/cpu/cpu_timing.h
	src/debug.h
	src/debug_translation.h
	src/emulator.h
	src/gpu.h
	src/io/io_provider.h
	src/log.h
	src/mem/cartridge.h
	src/mem/cart_loader.h
	src/mem/cart_mbc.h
	src/mem/mbc/cart_mbc0.h
	src/mem/mbc/cart_mbc1.h
	src/mem/mbc/cart_mbc3.h
	src/mem/mbc/cart_mbc3_rtc.h
	src/mem/mem.h
	src/os/os.h
	src/os/osx/osx.h
	src/rom_bios.h
	src/timer.h
	src/timewarp.h
	src/types.h
	src/qt/keyeventmapper.h
	src/qt/mainwindow.h
	src/qt/qtioprovider.h
	src/qt/screenwidget.h
)

# Test sources (addin main is required here)
set(TEST_SOURCES
	test/main.cpp
)

# Test headers
set(TEST_HEADERS
	test/bogus_ioprovider.h 
	test/gputest.h 
	test/instrtest.h 
	test/vram_dma_transfer_test.h
	test/gbctest_doublespeedmode.h
)

add_executable(karougb ${SOURCES} src/qt/main.cpp ${HEADERS})
target_link_libraries(karougb Qt5::Widgets)

# Copy Qt5 DLLs
# Qt uses different dlls for debug and release
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	# Debug mode
	add_custom_command (TARGET karougb POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Widgetsd.dll ${CMAKE_BINARY_DIR}
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Cored.dll ${CMAKE_BINARY_DIR}
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Guid.dll ${CMAKE_BINARY_DIR}
		COMMENT "Copying Qt binaries (debug)" VERBATIM
	)
else()
	# Release mode
	add_custom_command (TARGET karougb POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Widgets.dll ${CMAKE_BINARY_DIR}
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Core.dll ${CMAKE_BINARY_DIR}
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Gui.dll ${CMAKE_BINARY_DIR}
		COMMENT "Copying Qt binaries (release)" VERBATIM
	)
endif()

# Copy Resources
add_custom_command (TARGET karougb POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory  ${CMAKE_CURRENT_SOURCE_DIR}/src/resources ${CMAKE_BINARY_DIR}
	COMMENT "Copying Resources" VERBATIM
)

# Test setup and execution
if (test)
	# gtest download magic
	configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
	execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
	  RESULT_VARIABLE result
	  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
	if(result)
	  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
	endif()
	execute_process(COMMAND ${CMAKE_COMMAND} --build .
	  RESULT_VARIABLE result
	  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
	if(result)
	  message(FATAL_ERROR "Build step for googletest failed: ${result}")
	endif()

	# Prevent overriding the parent project's compiler/linker settings on Windows
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

	# This defines the gtest and gtest_main targets.
	add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
					 ${CMAKE_BINARY_DIR}/googletest-build
					 EXCLUDE_FROM_ALL)
	# gtest download magic end
	 
	# Include gtest headers
	include_directories(${gtest_SOURCE_DIR}/include)
	# Define test project
	project(karougb_unit_tests)
	# Add our main app as library so we can link the tests against it
	add_library(karougb_lib ${SOURCES} ${HEADERS})
	target_link_libraries(karougb_lib Qt5::Widgets)
	# Create the test executable
	add_executable(karougb_unit_tests ${TEST_SOURCES} ${TEST_HEADERS})
	# Link the test executable agains gtest and our app (lib)
	target_link_libraries(karougb_unit_tests gtest gtest_main karougb_lib)
	# Declare the test executable as test
	add_test(NAME karougb_tests COMMAND karougb_unit_tests)

	# Add test resources
	add_custom_command (TARGET karougb_unit_tests POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory  ${CMAKE_CURRENT_SOURCE_DIR}/test/resources ${CMAKE_BINARY_DIR}
		COMMENT "Copying test resources (debug)" VERBATIM
	)
endif(test)
