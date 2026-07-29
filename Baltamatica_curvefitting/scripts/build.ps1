dir c:\\mnt\\packages
echo "Unpacking BALTAM_CORE component ..."
mkdir -p deps\core
7z x -y -bsp1 c:\\mnt\packages\BALTAM_CORE-$BALTAM_CORE_VERSION-win64-core.zip -odeps\core\
7z x -y -bsp1 c:\\mnt\packages\BALTAM_CORE-$BALTAM_CORE_VERSION-win64-dev.zip -odeps\core\
echo "Compiling the code..."
mkdir build 
cd build

C:\\mnt\msys2\msys2_shell.cmd -defterm -here -no-start -mingw64 -c "cmake -G 'MinGW Makefiles' -DCMAKE_BUILD_TYPE=Release .. "
C:\\mnt\msys2\msys2_shell.cmd -defterm -here -no-start -mingw64 -c  "mingw32-make -j 4"
C:\\mnt\msys2\msys2_shell.cmd -defterm -here -no-start -mingw64 -c  "mingw32-make package"
echo "Compile complete."

