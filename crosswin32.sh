PATH=/opt/xmingw/bin/:/opt/xmingw/i386-mingw32msvc/bin/:$PATH
PKG_CONFIG_PATH=/mnt/c/MinGW/Gtk/lib/pkgconfig
CC=i386-mingw32msvc-gcc
CXX=i386-mingw32msvc-g++
AR=i386-mingw32msvc-ar
AS=i386-mingw32msvc-as
LD=i386-mingw32msvc-ld
CPP=i386-mingw32msvc-cpp
export PATH PKG_CONFIG_PATH CC CXX AR AS LD CPP
./configure --host=i386-mingw32msvc
grep -v '#define malloc' config.h|grep -v '#define realloc' > config.h.new
cp config.h.new config.h
PKG_CONFIG='pkg-config --define-variable=prefix=/mnt/c/MinGW/Gtk'
make CXXFLAGS=-mms-bitfields CFLAGS=-mms-bitfields LDFLAGS=-mms-bitfields LIBS="`$PKG_CONFIG --libs gtk+-2.0`" PKG_CONFIG="$PKG_CONFIG"
