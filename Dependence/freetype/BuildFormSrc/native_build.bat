set APPNAME="freetype"

echo "======================================="
echo "build project %APPNAME%"
CALL "private_build.bat"

echo xcopy .\libs\armeabi\*.so    ..\..\lib\android\armeabi /s/y
echo xcopy .\libs\armeabi-v7a\*.so    ..\..\lib\android\armeabi-v7a /s/y
echo xcopy .\obj\local\armeabi\*.a    ..\..\lib\android\armeabi /s/y
echo xcopy .\obj\local\armeabi-v7a\*.a    ..\..\lib\android\armeabi-v7a /s/y
xcopy .\libs\armeabi\*.so    ..\..\lib\android\armeabi /s/y
xcopy .\libs\armeabi-v7a\*.so    ..\..\lib\android\armeabi-v7a /s/y
xcopy .\obj\local\armeabi\*.a    ..\..\lib\android\armeabi /s/y
xcopy .\obj\local\armeabi-v7a\*.a    ..\..\lib\android\armeabi-v7a /s/y
echo xcopy .\libs\armeabi\*.so    ..\..\..\bin\android\armeabi /s/y
echo xcopy .\libs\armeabi-v7a\*.so    ..\..\..\bin\android\armeabi-v7a /s/y
echo xcopy .\obj\local\armeabi\*.a    ..\..\..\bin\android\armeabi /s/y
echo xcopy .\obj\local\armeabi-v7a\*.a    ..\..\..\bin\android\armeabi-v7a /s/y
xcopy .\libs\armeabi\*.so    ..\..\..\bin\android\armeabi /s/y
xcopy .\libs\armeabi-v7a\*.so    ..\..\..\bin\android\armeabi-v7a /s/y
xcopy .\obj\local\armeabi\*.a    ..\..\..\bin\android\armeabi /s/y
xcopy .\obj\local\armeabi-v7a\*.a    ..\..\..\bin\android\armeabi-v7a /s/y
