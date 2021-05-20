@echo off

echo Rebuilding the whole project from scratch...

if exist ".\build" (
	rd /S /Q ".\build"
)

mkdir build

cd build

cmake -G "NMake Makefiles" %* ../

cmake --build .

cd ..

