-Para compilar, abrir un terminal y:

	1- Crear una carpeta "build" y abrir un cmd en ella.

	2- Configurar el proyecto: 

		cmake -G 'MinGW Makefiles' ..

		-Nota: En Windows se usa el flag -G "MinGW Makefiles" para indicarle a CMake que utilice el compilador GCC, porque sino usa el compilador visual C.

	2- Compilar el proyecto:
	
		C:/TDM-GCC-64/bin/mingw32-make.exe

	o mediante un solo compando (útil para comando build en Geany): cmake -G "MinGW Makefiles" .. && C:/TDM-GCC-64/bin/mingw32-make.exe