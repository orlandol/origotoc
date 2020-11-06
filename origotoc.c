
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

  unsigned GetToken();

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
    if( (argc < 2) || (argc > 3) ) {
      printf( "Origo to C Alpha - Copyright 2020 Orlando Llanes\n" );
      printf( "\nusage: origotoc source[.ret] [binary[.exe]]\n" );
      exit(1);
    }

    strncpy( baseSourceName, argv[1], sizeof(baseSourceName) );

    strncpy( retName, argv[1], sizeof(retName) );

    if( argc == 3 ) {
      strncpy( exeName, argv[2], sizeof(exeName) );
    } else {
      strncpy( exeName, argv[1], sizeof(exeName) );
    }

    strncpy( cName, baseSourceName, sizeof(cName) );
    strncat( cName, ".rgc", sizeof(cName) );

    strncpy( headerName, baseSourceName, sizeof(headerName) );
    strncat( headerName, ".rgh", sizeof(headerName) );
  }

  FILE* OpenRet( char* sourceName ) {
    FILE* fileResult = NULL;

    if( sourceName && (*sourceName) ) {
      fileResult = fopen(sourceName, "r");
      if( fileResult ) {
        ReadChar();
        ReadChar();

        GetToken();
        GetToken();
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
    int errorLevel = -1;

    if( commandLine && (*commandLine) ) {
      errorLevel = system(commandLine);
    }

    return errorLevel;
  }

  unsigned GetToken() {
    return 0;
  }

int main( int argc, char* argv[] ) {
  char ch;

  ParseOptions( argc, argv );

  printf( "baseSourceName == %s\n", baseSourceName );
  printf( "retName == %s\n", retName );
  printf( "exeName == %s\n", exeName );
  printf( "cName == %s\n", cName );
  printf( "headerName == %s\n", headerName );

  do {
    ch = ReadChar();
  } while( ch );

  return 0;
}
