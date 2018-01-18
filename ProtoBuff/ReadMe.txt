[commands to make "protoc"]
sudo yum install autoconf automake libtool unzip gcc-c++ git -y
git clone https://github.com/google/protobuf.git
cd protobuf
./autogen.sh
./configure
make

[use to generate addressbook.pb.h && addressbook.pb.cc]
protoc -I=$SRC_DIR --cpp_out=$DST_DIR $SRC_DIR/addressbook.proto

export LD_LIBRARY_PATH=/usr/local/lib
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig

[compile]
g++ add_person.cpp addressbook.pb.cc `pkg-config --cflags --libs protobuf`
