cmake_minimum_required(VERSION 3.10)

project(serial_terminal)

# Use the C++11 standard.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# wxWidgets library.
# Set the path to the installed wxWidgets library.
set(wxWidgets_ROOT_DIR "C:/wxWidgets-3.2.10" CACHE PATH "Path to the wxWidgets installation.")
find_package(wxWidgets REQUIRED COMPONENTS core base gl net OPTIONAL_COMPONENTS)
# Include library directories.
include(${wxWidgets_USE_FILE})

add_executable(${PROJECT_NAME} main.cpp
	library/serial_port_windows/serial_port_windows.h
	library/serial_port_windows/serial_port_windows.cpp
	
	library/persistent_data/persistent_data.h
	library/persistent_data/persistent_data.cpp
)

if(WIN32)
	# Build as a Windows GUI application, without opening a console window.
	set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC
	"${PROJECT_SOURCE_DIR}/library"
	"${PROJECT_SOURCE_DIR}/library/serial_port_windows"
	"${PROJECT_SOURCE_DIR}/library/persistent_data"
)

target_link_libraries(${PROJECT_NAME} 
    ${wxWidgets_LIBRARIES}
)
