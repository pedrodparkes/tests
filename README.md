# tests
test projects for glash team

# Cygwin ffmpeg files instruction
0) https://ffmpeg.zeranoe.com/builds/ - downloading shared & dev builds x86/x64 depending on cygwin version
1) dev package: copy everything from **include** folder to cygwin's **/usr/include**
2) dev package: copy everything from **lib** folder to cygwin's **/lib/**
3) shared package: copy **bin/\*.dll** to the folder with exe file of your program
4) To compile **libavtest** (e.g.) use `gcc libavtest.c -lavformat -lavutil -o libavtest.exe`
