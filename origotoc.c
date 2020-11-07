
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Global variables
 */

  typedef struct TokenVal {
    unsigned valType;

    union {
      char* valStr;
      unsigned valUint;
      int valInt;
      char valChar;
    };
  } TokenVal;

  char baseName[FILENAME_MAX + 1] = {};

  char retName[FILENAME_MAX + 1] = {};
  char exeName[FILENAME_MAX + 1] = {};
  char cName[FILENAME_MAX + 1] = {};
  char headerName[FILENAME_MAX + 1] = {};

  FILE* retFile = NULL;
  FILE* cFile = NULL;
  FILE* headerFile = NULL;

  char curCh = 0;
  char nextCh = 0;

/*
 * Declarations
 */

  int RunProgram( char* commandLine );

  void Cleanup();

  void ParseOptions( int argc, char* argv[] );

  FILE* OpenRet( char* inRetName );
  void CloseRet( FILE** fileRet );

  FILE* CreateC( char* outCName );
  void CloseC( FILE** fileC );

  FILE* CreateHeader( char* outHeaderName );
  void CloseHeader( FILE** fileHeader );

  char ReadChar();

  unsigned GetToken();

/*
 * Implementations
 */

  int RunProgram( char* commandLine ) {
    int errorLevel = -1;

    if( commandLine && (*commandLine) ) {
      errorLevel = system(commandLine);
    }

    return errorLevel;
  }

  void Cleanup() {
    CloseRet( &retFile );
    CloseC( &cFile );
    CloseHeader( &headerFile );
  }

  void ParseOptions( int argc, char* argv[] ) {
    char* fileExt = NULL;

    // Validate parameter count
    if( (argc < 2) || (argc > 3) ) {
      printf( "Origo to C Alpha - Copyright 2020 Orlando Llanes\n" );
      printf( "\nusage: origotoc source[.ret] [binary[.exe]]\n" );
      exit(1);
    }

    // Determine the base file name without the file extension
    strncpy( baseName, argv[1], sizeof(baseName) );
    fileExt = strrchr(baseName, '.');
    if( fileExt ) {
      memset( fileExt, 0, strlen(fileExt) );
    }

    // Determine the Retineo source file name
    strncpy( retName, argv[1], sizeof(retName) );
    fileExt = strrchr(retName, '.');
    if( fileExt == NULL ) {
      strncat( retName, ".ret", sizeof(retName) );
    } else if( strlen(fileExt) == 1 ) {
      *fileExt = '\0';
    }

    // Determine the executable file name
    if( argc == 3 ) {
      strncpy( exeName, argv[2], sizeof(exeName) );
    } else {
      strncpy( exeName, baseName, sizeof(exeName) );
    }
    fileExt = strrchr(exeName, '.');
    if( fileExt == NULL ) {
      strncat( exeName, ".exe", sizeof(exeName) );
    } else if( strlen(fileExt) == 1 ) {
      strncat( exeName, "exe", sizeof(exeName) );
    }

    // Set the C file name
    strncpy( cName, baseName, sizeof(cName) );
    strncat( cName, ".rgc", sizeof(cName) );

    // Set the C Header file name
    strncpy( headerName, baseName, sizeof(headerName) );
    strncat( headerName, ".rgh", sizeof(headerName) );
  }

  FILE* OpenRet( char* inRetName ) {
    FILE* fileRet = NULL;

    if( inRetName && (*inRetName) ) {
      fileRet = fopen(inRetName, "r");
    }

    return fileRet;
  }

  void CloseRet( FILE** fileRet ) {
    if( fileRet ) {
      if( (*fileRet) ) {
        fclose( (*fileRet) );
        (*fileRet) = NULL;
      }
    }
  }

  FILE* CreateC( char* outCName ) {
    FILE* fileC = NULL;

    if( outCName && (*outCName) ) {
      fileC = fopen(outCName, "w");
    }

    return fileC;
  }

  void CloseC( FILE** fileC ) {
    if( fileC ) {
      if( (*fileC) ) {
        fclose( (*fileC) );
        (*fileC) = NULL;
      }
    }
  }

  FILE* CreateHeader( char* outHeaderName ) {
    FILE* fileHeader = NULL;

    if( outHeaderName && (*outHeaderName) ) {
      fileHeader = fopen(outHeaderName, "w");
    }

    return fileHeader;
  }

  void CloseHeader( FILE** fileHeader ) {
    if( fileHeader ) {
      if( (*fileHeader) ) {
        fclose( (*fileHeader) );
        (*fileHeader) = NULL;
      }
    }
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

  unsigned GetToken() {
    return 0;
  }

int main( int argc, char* argv[] ) {
  char ch;

  atexit( Cleanup );

  ParseOptions( argc, argv );

  if( OpenRet(retName) == 0 ) {
    printf( "Error opening file %s\n", retName );
  }

  printf( "\nParsing %s...\n", retName );
  printf( "\n" );

  printf( "\nBuilding %s from %s...\n", exeName, headerName );
  printf( "\n" );

  Cleanup();

  return 0;
}
