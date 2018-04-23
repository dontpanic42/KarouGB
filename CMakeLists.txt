cmake_minimum_required(VERSION 3.1)
project(karougb)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH "C:\\Qt\\Qt5.10.1\\5.10.1\\msvc2017_64")

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

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
	src/qt/main.cpp
	src/qt/mainwindow.cpp
)

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
	src/qt/mainwindow.h
)

add_executable(karougb ${SOURCES} ${HEADERS})
target_link_libraries(karougb Qt5::Core Qt5::Widgets)

# Copy Qt5 DLLs
add_custom_command (TARGET karougb POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Widgetsd.dll ${CMAKE_BINARY_DIR}
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Cored.dll ${CMAKE_BINARY_DIR}
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${Qt5Core_DIR}/../../../bin/Qt5Guid.dll ${CMAKE_BINARY_DIR}
	COMMENT "Copying Qt binaries" VERBATIM
)

# Copy Resources
add_custom_command (TARGET karougb POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory  ${CMAKE_CURRENT_SOURCE_DIR}/src/resources ${CMAKE_BINARY_DIR}
	COMMENT "Copying Resources" VERBATIM
)
