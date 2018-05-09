./autogen.sh
##mkdir -p build; cd build
cd src
if [ "$1" == "win" ]; then
	../configure --prefix=/mingw64
else
#	../configure --prefix=/usr --enable-gl3=yes
	../configure --prefix=/usr
fi
make -j6
cd ..

#git submodule foreach git clean -xdf
