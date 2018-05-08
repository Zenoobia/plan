./autogen.sh
##mkdir -p build; cd build
cd src
. ../configure
make -j6
cd ..

#git submodule foreach git clean -xdf
