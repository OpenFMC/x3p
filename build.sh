# !/bin/sh
mkdir build
cd build
cmake -DPACK_XSD_RUNTIME=ON ..
cmake --build . --config Release
cpack -C Release
mv -f openGPS-*-Linux*.tar.gz ..
cd ..