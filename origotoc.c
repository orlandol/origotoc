
#include <stdio.h>
#include <stdlib.h>

/*
 * Global variables
 */

  char baseSourceName[FILENAME_MAX + 1] = {};
  char retName[FILENAME_MAX + 1] = {};
  char exeName[FILENAME_MAX + 1] = {};
  char cName[FILENAME_MAX + 1] = {};
  char headerName[FILENAME_MAX + 1] = {};

  FILE* retFile = NULL;

  char curCh = 0;
  char nextCh = 0;

/*
 * Declarations
 */
  void Cleanup();

  void ParseOptions( int argc, char* argv[] );

  FILE* OpenRet( char* sourceName );

  char ReadChar();

  int RunProgram( char* commandLine );


/*
 * Implementations
 */
  void Cleanup() {
    if( retFile ) {
      fclose( retFile );
      retFile = NULL;
    }
  }

  void ParseOptions( int argc, char* argv[] ) {
    if( argc < 2 ) {
      printf( "usage: origotoc source[.ret] [binary[.exe]]\n" );
      exit(1);
    }
  }

  FILE* OpenRet( char* sourceName ) {
    FILE* fileResult = NULL;

    if( sourceName && (*sourceName) ) {
      fileResult = fopen(sourceName, "r");
      if( fileResult ) {
        ReadChar();
        ReadChar();
      }
    }

    return fileResult;
  }

  char ReadChar() {
    int result = 0;

    curCh = nextCh;
    nextCh = 0;

    if( retFile ) {
      result = fgetc(retFile);
      if( result == EOF ) {
        return 0;
      }
      nextCh = (char)result;
    }

    return curCh;
  }

  int RunProgram( char* commandLine ) {
    int errorLevel = 0;

    if( commandLine && (*commandLine) ) {
      errorLevel = system(commandLine);
    }

    return errorLevel;
  }

int main( int argc, char* argv[] ) {
  char ch;

  ParseOptions( argc, argv );

  do {
    ch = ReadChar();
  } while( ch );

  return 0;
}
