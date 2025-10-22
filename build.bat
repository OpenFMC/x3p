set VCPKG_DEFAULT_TRIPLE=x64-windows
mkdir build
cd build
cmake -DPACK_XSD_RUNTIME=ON ..
cmake --build . --config Debug
cmake --build . --config Release
cpack -C Debug;Release
xcopy /Y openGPS-*-win64*.zip ..
cd ..