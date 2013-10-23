APPNAME = "freetype"
if [ -z "${NDK_ROOT+aaa}" ];then
echo "please define NDK_ROOT"
exit 1
fi
if [ -z "${AGE_ROOT+aaa}" ];then
echo "please define AGE_ROOT"
exit 1
fi
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJROOT="$DIR"
APP_ANDROID_ROOT="$DIR"

echo "NDK_ROOT = $NDK_ROOT"
echo "AGE_ROOT = $AGE_ROOT"
echo "PROJROOT = $PROJROOT"
echo "APP_ANDROID_ROOT = $APP_ANDROID_ROOT"

echo "start build project $APPNAME"
"$NDK_ROOT"/ndk-build NDK_NO_WARNINGS=1 NDK_LOG=0 NDK_DEBUG=1 -j8 -C "$APP_ANDROID_ROOT" $* "NDK_MODULE_PATH=${PROJROOT}:${AGE_ROOT}"
echo "complete build project $APPNAME"

mkdir ../../lib/android/armeabi
mkdir ../../lib/android/armeabi-v7a
echo cp ./obj/local/armeabi/libfreetype.so    ../../lib/android/armeabi/
cp ./obj/local/armeabi/libfreetype.so    ../../lib/android/armeabi/
echo cp ./obj/local/armeabi-v7a/libfreetype.so    ../../lib/android/armeabi-v7a/
cp ./obj/local/armeabi-v7a/libfreetype.so    ../../lib/android/armeabi-v7a/
echo cp ./obj/local/armeabi/libfreetype.a    ../../lib/android/armeabi/
cp ./obj/local/armeabi/libfreetype.a    ../../lib/android/armeabi/
echo cp ./obj/local/armeabi-v7a/libfreetype.a    ../../lib/android/armeabi-v7a/
cp ./obj/local/armeabi-v7a/libfreetype.a    ../../lib/android/armeabi-v7a/
echo cp ./obj/local/armeabi/libfreetype_static.a    ../../lib/android/armeabi/
cp ./obj/local/armeabi/libfreetype_static.a    ../../lib/android/armeabi/
echo cp ./obj/local/armeabi-v7a/libfreetype_static.a    ../../lib/android/armeabi-v7a/
cp ./obj/local/armeabi-v7a/libfreetype_static.a    ../../lib/android/armeabi-v7a/
cp ./obj/local/armeabi/libcpufeatures.a    ../../lib/android/armeabi/
cp ./obj/local/armeabi-v7a/libcpufeatures.a    ../../lib/android/armeabi-v7a/
mkdir ../../../bin/android/armeabi
mkdir ../../../bin/android/armeabi-v7a
echo cp ./obj/local/armeabi/libfreetype.so    ../../../bin/android/armeabi/
cp ./obj/local/armeabi/libfreetype.so    ../../../bin/android/armeabi/
echo cp ./obj/local/armeabi-v7a/libfreetype.so    ../../../bin/android/armeabi-v7a/
cp ./obj/local/armeabi-v7a/libfreetype.so    ../../../bin/android/armeabi-v7a/
echo cp ./obj/local/armeabi/libfreetype.a    ../../../bin/android/armeabi/
cp ./obj/local/armeabi/libfreetype.a    ../../../bin/android/armeabi/
echo cp ./obj/local/armeabi-v7a/libfreetype.a    ../../../bin/android/armeabi-v7a/
cp ./obj/local/armeabi-v7a/libfreetype.a    ../../../bin/android/armeabi-v7a/
echo cp ./obj/local/armeabi/libfreetype_static.a    ../../../bin/android/armeabi/
cp ./obj/local/armeabi/libfreetype_static.a    ../../../bin/android/armeabi/
echo cp ./obj/local/armeabi-v7a/libfreetype_static.a    ../../../bin/android/armeabi-v7a/
cp ./obj/local/armeabi-v7a/libfreetype_static.a    ../../../bin/android/armeabi-v7a/
cp ./obj/local/armeabi/libcpufeatures.a    ../../../bin/android/armeabi/
cp ./obj/local/armeabi-v7a/libcpufeatures.a    ../../../bin/android/armeabi-v7a/
