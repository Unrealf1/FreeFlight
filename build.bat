@echo off

echo Building the project...

if not exist ".\build" (
	mkdir ".\build"
)

cd build

cmake -G "NMake Makefiles" %* ../

cmake --build .

cd ..

