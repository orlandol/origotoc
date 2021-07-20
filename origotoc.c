/* 
 * MIT License
 * 
 * Copyright (c) 2014-2021 Orlando Llanes
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "origotoc.h"

/*
 *  Global variables
 */

int argc = 0;
char** argv = NULL;

/*
 *  Helper functions
 */
void FreePtr( void** ptr ) {
  if( ptr ) {
    if( *ptr ) {
      free( *ptr );
      *ptr = NULL;
    }
  }
}

/*
 *  Path split functions
 */
int SplitPath( const char* path, char** outDir,
    char** outBaseName, char** outExt ) {
  const char* pathCh;
  const char* pathDir;
  const char* pathBaseName;
  const char* pathExt;
  size_t curLen = 0;
  size_t dirLen = 0;
  size_t basenameLen = 0;
  size_t extLen = 0;
  char* newDir = NULL;
  char* newBaseName = NULL;
  char* newExt = NULL;

  // Validate parameters
  if( (path == NULL) || (*path == '\0') ) { return 1; }
  if( (outDir == NULL) || *outDir ) { return 2; }
  if( (outBaseName == NULL) || *outBaseName ) { return 3; }
  if( (outExt == NULL) || *outExt ) { return 4; }

  pathDir = path;
  pathBaseName = path;
  pathExt = path;

  // Find last \ or / in file path
  pathCh = pathDir;
  curLen = 0;
  while( *pathCh )  {
    if( *pathCh == '\\' || *pathCh == '/' ) {
      pathBaseName = pathCh + 1;
      dirLen = curLen + 1;
    }
    pathCh++;
    curLen++;
  }

  // Find the first . after the last \ or /
  pathCh = pathBaseName;

  if( (*pathCh == '.') ) {
    // Special case .file as base name, when [dir\ or dir/].file[.ext]
    pathCh++;
    pathExt = pathCh;
    basenameLen++;
  }

  while( *pathCh )  {
    if( *pathCh == '.' ) {
      pathExt = pathCh;
      break;
    }
    pathCh++;
    basenameLen++;
  }

  // Get the length of the file extension
  pathCh = pathExt;
  extLen = 0;
  while( *pathCh )  {
    if( *pathCh == '.' ) {
      break;
    }
    pathCh++;
    pathExt++;
  }
  while( *pathCh ) {
    pathCh++;
    extLen++;
  }

  if( (dirLen | basenameLen | extLen) == 0 ) { return 5; }

  if( dirLen ) {
    newDir = malloc((dirLen + 1) * sizeof(char));
    if( newDir == NULL ) { goto ExitError; }
    memcpy( newDir, pathDir, dirLen * sizeof(char) );
    newDir[dirLen] = '\0';
    *outDir = newDir;
  }

  if( basenameLen ) {
    newBaseName = malloc((basenameLen + 1) * sizeof(char));
    if( newBaseName == NULL ) { goto ExitError; }
    memcpy( newBaseName, pathBaseName, basenameLen * sizeof(char) );
    newBaseName[basenameLen] = '\0';
    *outBaseName = newBaseName;
  }

  if( extLen ) {
    newExt = malloc((extLen + 1) * sizeof(char));
    if( newExt == NULL ) { goto ExitError; }
    memcpy( newExt, pathExt, extLen * sizeof(char) );
    newExt[extLen] = '\0';
    *outExt = newExt;
  }

  return 0;

ExitError:
  FreePtr( &newDir );
  FreePtr( &newBaseName );
  FreePtr( &newExt );
  return 6;
}

int JoinPath( const char* inDir, const char* inBaseName,
    const char* inExt, char** outPath ) {
  char* newPath = NULL;
  size_t totalLen;
  const char* tmpCh;
  char pathSeparator = '\0';
  char extDot = '\0';

  // Validate parameters
  if( (outPath == NULL) || *outPath ) { return 4; }
  if( (inDir == NULL) && (inBaseName == NULL)
    && (inExt == NULL) ) { return 5; }

  // Calculate the total string length
  totalLen = 0;
  if( inDir ) {
    totalLen = strlen(inDir);
    if( totalLen ) {
      // Check for trailing path separator
      if( (inDir[totalLen - 1] != '\\') && (inDir[totalLen - 1] != '/') ) {
        totalLen++;
        // If path separator isn't at end, detect which slash to use
        tmpCh = inDir;
        pathSeparator = '\\'; // default to '\'
        while( *tmpCh ) {
          if( (*tmpCh == '\\') || (*tmpCh == '/') ) {
            pathSeparator = *tmpCh;
            break;
          }
          tmpCh++;
        }
      }
    }
  }

  if( inBaseName ) {
    totalLen += strlen(inBaseName);
    // Skip slash at start of BaseName
    if( (*inBaseName == '\\') || (*inBaseName == '/') ) {
      inBaseName++;
      totalLen--;
    }
    // If a slash remains, the base name is [probably?] invalid
    if( (*inBaseName == '\\') || (*inBaseName == '/') ) { return 6; }
  }

  if( inExt ) {
    totalLen += strlen(inExt);
    // Add a dot if missing from file extension
    if( *inExt != '.' ) {
      extDot = '.';
      totalLen++;
    }
  }

  // Allocate string buffer
  newPath = malloc((totalLen + 1) * sizeof(char));
  if( newPath == NULL ) { return 7; }
  *newPath = '\0';

  // Copy in each parameter
  if( inDir ) {
    strcat( newPath, inDir );
    if( pathSeparator ) {
      strncat( newPath, &pathSeparator, 1 );
    }
  }

  if( inBaseName ) {
    strcat( newPath, inBaseName );
  }

  if( inExt ) {
    if( extDot ) {
      strncat( newPath, &extDot, 1 );
    }
    strcat( newPath, inExt );
  }

  *outPath = newPath;
  return 0;
}

/*
 *  OrigoToC program declarations
 */
OrigoOptions options = {};

void PrintBanner() {
  printf( "OrigoToC %s %s\n\n", ORIGOTOC_VERSTRING, ORIGOTOC_COPYRIGHT );
}

void Usage() {
  printf( "usage: origotoc source[.ret] [binary.exe]\n" );
}

int ParseOptions( OrigoOptions* outOptions ) {
  char* sourceDir = NULL;
  char* sourceBaseName = NULL;
  char* sourceExt = NULL;
  char* binaryDir = NULL;
  char* binaryBaseName = NULL;
  char* binaryExt = NULL;
  char* sourceFileName = NULL;
  char* cFileName = NULL;
  char* hFileName = NULL;
  char* binaryFileName = NULL;
  int result = 0;
  int errorResult = 0;

  // Validate parameters
  if( argc <= 1 ) {
    Usage();
    exit(1);
  }

  if( outOptions == NULL ) { return 1; }

  // Parse Origo file name
  result = SplitPath(argv[1], &sourceDir, &sourceBaseName, &sourceExt);
  if( result != 0 ) { return 2; }

  // Parse .c, .h, and binary file names
  if( argc <= 2 ) {
    if( sourceDir ) {
      binaryDir = strdup(sourceDir);
      if( binaryDir == NULL ) {
        errorResult = 3;
        goto ExitError;
      }
    }

    if( sourceBaseName ) {
      binaryBaseName = strdup(sourceBaseName);
      if( binaryBaseName == NULL ) {
        errorResult = 4;
        goto ExitError;
      }
    }

    if( sourceExt == NULL ) {
      sourceExt = strdup(".ret");
      if( sourceExt == NULL ) {
        errorResult = 5;
        goto ExitError;
      }
    }

    binaryExt = strdup(".exe");
    if( binaryExt == NULL ) {
      errorResult = 6;
      goto ExitError;
    }
  } else {
    result = SplitPath(argv[2], &binaryDir, &binaryBaseName, &binaryExt);
    if( result != 0 ) {
      errorResult = 7;
      goto ExitError;
    }
  }

  // Create file names
  result = JoinPath(sourceDir, sourceBaseName, sourceExt, &sourceFileName);
  if( result || (sourceFileName == NULL) ) {
    errorResult = 8;
    goto ExitError;
  }
  outOptions->sourceFileName = sourceFileName;

  result = JoinPath(binaryDir, binaryBaseName, ".rtc", &cFileName);
  if( result || (cFileName == NULL) ) {
    errorResult = 9;
    goto ExitError;
  }
  outOptions->cFileName = cFileName;

  result = JoinPath(binaryDir, binaryBaseName, ".rth", &hFileName);
  if( result || (hFileName == NULL) ) {
    errorResult = 10;
    goto ExitError;
  }
  outOptions->hFileName = hFileName;

  result = JoinPath(binaryDir, binaryBaseName, binaryExt, &binaryFileName);
  if( result || (binaryFileName == NULL) ) {
    errorResult = 11;
    goto ExitError;
  }
  outOptions->binaryFileName = binaryFileName;

  // Release temporary path components
  FreePtr( &sourceDir );
  FreePtr( &sourceBaseName );
  FreePtr( &sourceExt );
  FreePtr( &binaryDir );
  FreePtr( &binaryBaseName );
  FreePtr( &binaryExt );
  return 0;

ExitError:
  // Release temporary path components
  FreePtr( &sourceDir );
  FreePtr( &sourceBaseName );
  FreePtr( &sourceExt );
  FreePtr( &binaryDir );
  FreePtr( &binaryBaseName );
  FreePtr( &binaryExt );
  // Release the joined file paths that succeeded
  FreePtr( &sourceFileName );
  FreePtr( &cFileName );
  FreePtr( &hFileName );
  FreePtr( &binaryFileName );
  return errorResult;
}

/*
 *  Token declarations
 */

/*
 *  Warning/error functions
 */

void Error( unsigned code, const char* message ) {
  printf( "Error[%u]: %s\n", code, message );
  exit(1);
}

void Expected( unsigned line, unsigned column, const char* message ) {
  printf( "Expected[L%u,C%u]: %s\n", line, column, message );
  exit(1);
}

void Unexpected( unsigned line, unsigned column, const char* message ) {
  printf( "Unexpected[L%u,C%u]: %s\n", line, column, message );
  exit(1);
}

/*
 *  Lexer declarations
 */

RetFile retFile = {};

int ReadChar( RetFile* source ) {
  int columnInc = 1;
  char tmpCh;

  if( source && source->handle ) {

    if( source->curCh == '\n' ) {
      source->line++;
      source->column = 1;
      columnInc = 0;
    }

    source->curCh = source->nextCh;
    source->nextCh = fgetc(source->handle);

    if( source->nextCh == '\r' ) {
      tmpCh = fgetc(source->handle);
      if( tmpCh != '\n' ) {
        ungetc( tmpCh, source->handle );
      }

      source->nextCh = '\r';
    }

    source->column += columnInc;
  }

  return source->curCh;
}

int ReadIdentChar( RetFile* source ) {
  int identCh;

  if( source == NULL ) { return 1; }

  identCh = ReadChar(source);

  // Only read _azAZ
  if( identCh == '_' ) { return identCh; }
  if( isalnum(identCh) ) { return identCh; }

  return EOF;
}

int ReadIdent( RetFile* source, char* ident ) {
  char tmpIdent[IDENT_MAXLEN] = {};
  size_t identLen = 0;

  if( source == NULL ) { return 1; }
  if( ident == NULL ) { return 2; }

  if( (source->curCh != '_')
    && (isalpha(source->curCh) == 0) ) { return 3; }

  do {
    if( identLen < IDENT_MAXINDEX ) {
      tmpIdent[identLen] = source->curCh;
      tmpIdent[identLen + 1] = '\0';
    }
    identLen++;
  } while( ReadIdentChar(source) != EOF );


  if( identLen == 0 ) { return 4; }

  strcpy( ident, tmpIdent );

  return 0;
}

int ReadBinaryDigit( RetFile* source ) {
  int binDigit;

  binDigit = ReadChar(source);

  // Skip separators
  while( (binDigit != EOF) && (binDigit == '_') ) {
    binDigit = ReadChar(source);
  }

  // Only read 01
  if( (binDigit == '0') || (binDigit == '1') ) { return binDigit; }

  return EOF;
}

int ReadOctalDigit( RetFile* source ) {
  int octalDigit;

  octalDigit = ReadChar(source);

  // Skip separators
  while( (octalDigit != EOF) && (octalDigit == '_') ) {
    octalDigit = ReadChar(source);
  }

  // Only read 07
  if( (octalDigit >= '0') && (octalDigit <= '7') ) { return octalDigit; }

  return EOF;
}

int ReadHexDigit( RetFile* source ) {
  int hexDigit;

  hexDigit = ReadChar(source);

  // Skip separators
  while( (hexDigit != EOF) && (hexDigit == '_') ) {
    hexDigit = ReadChar(source);
  }

  // Only read 09afAF
  if( isxdigit(hexDigit) ) { return tolower(hexDigit); }

  return EOF;
}

int ReadDecimalDigit( RetFile* source ) {
  int digit;

  digit = ReadChar(source);

  // Skip separators
  while( (digit != EOF) && (digit == '_') ) {
    digit = ReadChar(source);
  }

  // Only read 09
  if( (digit >= '0') && (digit == '9') ) { return digit; }

  return EOF;
}

int OpenRet( const char* fileName, RetFile* outsource ) {
  if( fileName == NULL ) { return 1; }
  if( outsource == NULL ) { return 2; }

  CloseRet( outsource );

  outsource->handle = fopen(fileName, "r");
  if( outsource->handle == NULL ) { return 3; }

  outsource->line = 1;
  outsource->column = 1;

  ReadChar( outsource );
  ReadChar( outsource );

  return 0;
}

void CloseRet( RetFile* outsource ) {
  if( outsource && outsource->handle ) {
    fclose( outsource->handle );
    outsource->handle = NULL;

    memset( outsource, 0, sizeof(RetFile) );
    outsource->line = 1;
    outsource->column = 1;

    SkipNonterminals( outsource );
  }
}

/*
 *  Symbol table declarations
 */

SymTable symTable = {};

/*
 *  C code generator declarations
 */

CFile cGen = {};

/*
 *  Parser declarations
 */

int SkipSpace( RetFile* source ) {
  if( source ) {
    if( isspace(source->curCh) ) {
      while( isspace(source->curCh) ) {
        ReadChar( source );
      }
    }
    return 0;
  }
  return 2;
}

int SkipComment( RetFile* source ) {
  unsigned commentLevel = 0;
  if( source == NULL ) { return 1; }

  if( (source->curCh == '/') && (source->nextCh == '*') ) {
    ReadChar( source );
    ReadChar( source );
    commentLevel++;

    while( commentLevel ) {
      if( source->curCh == EOF ) {
        return 2;
      }

      if( (source->curCh == '/') && (source->nextCh == '*') ) {
        ReadChar( source );
        ReadChar( source );
        if( commentLevel == ((unsigned)-1) ) {
          return 3;
        }
        commentLevel++;
        continue;
      }

      if( (source->curCh == '*') && (source->nextCh == '/') ) {
        ReadChar( source );
        ReadChar( source );
        if( commentLevel == 0 ) {
          return 4;
        }
        commentLevel--;
        continue;
      }
      ReadChar( source );
    }
    return 0;
  }

  if( (source->curCh == '/') && (source->nextCh == '/') ) {
    ReadChar( source );
    ReadChar( source );
    while( source->curCh != '\n' ) {
      if( source->curCh == EOF ) { return 2; }
      ReadChar( source );
    }
    return 0;
  }

  return 2;
}

void SkipNonterminals( RetFile* source ) {
  while( (SkipSpace(source) | SkipComment(source)) == 0 ) {
    if( source->curCh == EOF ) {
      break;
    }
  }
}

int Match( RetFile* source, const char* text ) {
  const char* textCh = text;
  unsigned startLine = 0;
  unsigned startColumn = 0;
  int result = 0;

  if( text == NULL ) { return 1; }
  if( source == NULL ) { return 2; }

  startLine = source->line;
  startColumn = source->column;

  while( *textCh ) {
    if( *textCh != source->curCh ) { return 3; }
    ReadChar( source );
    textCh++;
  }

  return 0;
}

// program IDENT
int ParseProgram( RetFile* source, CFile* cgen, SymTable* symtab ) {
  char programName[IDENT_MAXLEN] = {};
  unsigned line;
  unsigned column;
  int result = 0;

  if( source == NULL ) { return 1; }
  if( cgen == NULL ) { return 2; }
  if( symtab == NULL ) { return 3; }

  SkipNonterminals( source );
  line = source->line;
  column = source->column;
  result = Match( source, "program" );
  if( result ) { Expected( line, column, "program" ); }

  SkipNonterminals( source );
  line = source->line;
  column = source->column;
  result = ReadIdent(source, programName);
  if( result ) { Expected( line, column, "Identifier" ); }
}

int Parse( RetFile* source, CFile* cgen, SymTable* symtab ) {
  int result = 0;

  result = ParseProgram( source, cgen, symtab );
  if( result ) { Error( result, "Parse > ParseProgram" ); }

  return 0;
}

/*
 *  Main program declarations
 */

void Cleanup() {
  // Release memory used by source file
  CloseRet( &retFile );

  // Release memory used by options
  FreePtr( &options.sourceFileName );
  FreePtr( &options.cFileName );
  FreePtr( &options.hFileName );
  FreePtr( &options.binaryFileName );
}

int main( int paramArgc, char* paramArgv[] ) {
  int result = 0;

  argc = paramArgc;
  argv = paramArgv;

  atexit( Cleanup );

  PrintBanner();

  result = ParseOptions(&options);
  if( result != 0 ) { Error(result, "main > ParseOptions" ); }

  result = OpenRet(options.sourceFileName, &retFile);
  if( result != 0 ) { Error(result, "main > OpenRet" ); }

  result = Parse( &retFile, &cGen, &symTable );
  if( result != 0 ) { Error(result, "main > Parse" ); }

  return 0;
}
