cmake_minimum_required(VERSION 3.10)

project(serial_terminal)

# Usar estandar C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Este flag es para que no mostrar la terminal (CMD) de windows al ejecutar el programa.
# Comentar esta línea si se necesita el CMD para ver los mensajes de debugging.
set(CMAKE_CXX_FLAGS "-mwindows")

# Librería WxWidgets.
# Especificar la ruta a la librería wxWidgets instalada.
set(wxWidgets_ROOT_DIR C:/wxWidgets-3.2.10)
find_package(wxWidgets REQUIRED COMPONENTS core base gl net OPTIONAL_COMPONENTS)
# Incluir directorios de la librería.
include(${wxWidgets_USE_FILE})

add_executable(${PROJECT_NAME} main.cpp
	library/serial_port_windows/serial_port_windows.h
	library/serial_port_windows/serial_port_windows.cpp
	
	library/persistent_data/persistent_data.h
	library/persistent_data/persistent_data.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
	"${PROJECT_SOURCE_DIR}/library"
	"${PROJECT_SOURCE_DIR}/library/serial_port_windows"
    "${PROJECT_SOURCE_DIR}/library/persistent_data"
)

target_link_libraries(${PROJECT_NAME} 
    ${wxWidgets_LIBRARIES}
)
