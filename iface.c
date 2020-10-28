#include <stdio.h>

  size_t FileRead_read( void* readBuf, size_t readSize ) {
    printf( "FileRead_read\n" );

    return 0;
  }

typedef struct IFileRead {
  size_t (*read)( void* inBuf, size_t inSize );
} IFileRead;

static const IFileRead FileRead = {
  &FileRead_read
};

int main( int argc, char* argv[] ) {
  size_t actualRead = FileRead.read( NULL, 0 );
  printf( "%p %p\n", FileRead.read, FileRead_read );

  return 0;
}
