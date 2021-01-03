#include <stdio.h>

  typedef struct File {
    FILE* handle;
  } File;

  typedef struct IFileRead {
    size_t (*read)( File* self, void* inBuf, size_t inSize );
  } IFileRead;

  size_t FileRead_read( File* self, void* readBuf, size_t readSize ) {
    printf( "FileRead_read\n" );

    return 0;
  }

  static const IFileRead FileRead = {
    FileRead_read
  };

int main( int argc, char* argv[] ) {
  File fileObj;
  size_t actualRead = FileRead.read( &fileObj, NULL, 0 );

  printf( "%p %p\n", FileRead.read, FileRead_read );

  return 0;
}
