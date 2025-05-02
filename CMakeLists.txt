cmake_minimum_required(VERSION 3.2)

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
set(wxWidgets_ROOT_DIR C:/wxWidgets-3.2.8)
find_package(wxWidgets REQUIRED COMPONENTS core base gl net OPTIONAL_COMPONENTS)
# Incluir directorios de la librería.
include(${wxWidgets_USE_FILE})

add_executable(${PROJECT_NAME} main.cpp
	library/SerialPortWindows/SerialPortWindows.h
	library/SerialPortWindows/SerialPortWindows.cpp
	
	library/Persistent_data/Persistent_data.h
	library/Persistent_data/Persistent_data.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
	"${PROJECT_SOURCE_DIR}/library"
	"${PROJECT_SOURCE_DIR}/library/SerialPortWindows"
    "${PROJECT_SOURCE_DIR}/library/Persistent_data"
)

target_link_libraries(${PROJECT_NAME} 
    ${wxWidgets_LIBRARIES}
)