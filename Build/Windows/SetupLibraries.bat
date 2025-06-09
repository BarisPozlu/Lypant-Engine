cd ../../Lypant/vendor/assimp
cmake CmakeLists.txt -DBUILD_SHARED_LIBS=OFF -DASSIMP_BUILD_ZLIB=ON -DASSIMP_INSTALL=OFF -DUSE_STATIC_CRT=ON
cmake --build . --config Debug
cmake --build . --config Release
echo You can now close this window and generate the solution!
PAUSE