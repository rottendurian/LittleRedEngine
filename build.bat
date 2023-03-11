cmake -S . -B build
cd build
msbuild vulkan.sln -m /p:configuration=debug
cd ..