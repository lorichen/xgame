echo Press '1' clean project
echo Press '2' build project
echo Press '3' exit

set /p ans=
if %ans%==1 goto cmda
if %ans%==2 goto cmdb
if %ans%==3 goto cmdc

:cmda
echo =============== clean project ==================
%NDK8%/ndk-build clean
goto cmdc

:cmdb
echo =============== build project ==================
%NDK8%/ndk-build -B V=0 NDK_LOG=0 NDK_DEBUG=1 -j4
goto cmdc

:cmdc
echo =============== exit ==================
PAUSE
