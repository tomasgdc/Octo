cd ..
mkdir App

::GET Vulkan
copy %VK_SDK_PATH%\\Source\\lib\\vulkan-1.dll App\\vulkan-1.dll

::GET SDL
copy ThirdParty\\sdl\\build\\Debug\\SDL2.dll App\\SDL2.dll

::GET ASSIMP
copy ThirdParty\\assimp\build\code\Debug\\assimp-vc140-mt.dll App\\assimp-vc140-mt.dll

::GET FREETYPE
copy "ThirdParty\\freetype\\builds\\vc2017\\x64\\freetype.dll" App\\freetype.dll

cd Scripts
timeout 2
