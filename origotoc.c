/* 
 * MIT License
 * 
 * OrigoToC 0.1.7 Alpha
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
void FreePtr( void** ptrVar ) {
  if( ptrVar ) {
    if( *ptrVar ) {
      free( *ptrVar );
      *ptrVar = NULL;
    }
  }
}

/*
 *  Path functions
 */
int SplitPath( const char* fromFullPath, char** toDir,
  char** toBaseName, char** toExt ) {

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
  if( (fromFullPath == NULL) || (*fromFullPath == '\0') ) { return 1; }
  if( (toDir == NULL) || *toDir ) { return 2; }
  if( (toBaseName == NULL) || *toBaseName ) { return 3; }
  if( (toExt == NULL) || *toExt ) { return 4; }

  pathDir = fromFullPath;
  pathBaseName = fromFullPath;
  pathExt = fromFullPath;

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
    *toDir = newDir;
  }

  if( basenameLen ) {
    newBaseName = malloc((basenameLen + 1) * sizeof(char));
    if( newBaseName == NULL ) { goto ExitError; }
    memcpy( newBaseName, pathBaseName, basenameLen * sizeof(char) );
    newBaseName[basenameLen] = '\0';
    *toBaseName = newBaseName;
  }

  if( extLen ) {
    newExt = malloc((extLen + 1) * sizeof(char));
    if( newExt == NULL ) { goto ExitError; }
    memcpy( newExt, pathExt, extLen * sizeof(char) );
    newExt[extLen] = '\0';
    *toExt = newExt;
  }

  return 0;

ExitError:
  FreePtr( &newDir );
  FreePtr( &newBaseName );
  FreePtr( &newExt );
  return 6;
}

int JoinPath( const char* fromDir, const char* fromBaseName,
  const char* fromExt, char** toFullPath ) {

  char* newPath = NULL;
  size_t totalLen;
  const char* tmpCh;
  char pathSeparator = '\0';
  char extDot = '\0';

  // Validate parameters
  if( (toFullPath == NULL) || *toFullPath ) { return 4; }
  if( (fromDir == NULL) && (fromBaseName == NULL)
    && (fromExt == NULL) ) { return 5; }

  // Calculate the total string length
  totalLen = 0;
  if( fromDir ) {
    totalLen = strlen(fromDir);
    if( totalLen ) {
      // Check for trailing path separator
      if( (fromDir[totalLen - 1] != '\\') && (fromDir[totalLen - 1] != '/') ) {
        totalLen++;
        // If path separator isn't at end, detect which slash to use
        tmpCh = fromDir;
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

  if( fromBaseName ) {
    totalLen += strlen(fromBaseName);
    // Skip slash at start of BaseName
    if( (*fromBaseName == '\\') || (*fromBaseName == '/') ) {
      fromBaseName++;
      totalLen--;
    }
    // If a slash remains, the base name is [probably?] invalid
    if( (*fromBaseName == '\\') || (*fromBaseName == '/') ) { return 6; }
  }

  if( fromExt ) {
    totalLen += strlen(fromExt);
    // Add a dot if missing from file extension
    if( *fromExt != '.' ) {
      extDot = '.';
      totalLen++;
    }
  }

  // Allocate string buffer
  newPath = malloc((totalLen + 1) * sizeof(char));
  if( newPath == NULL ) { return 7; }
  *newPath = '\0';

  // Copy in each parameter
  if( fromDir ) {
    strcat( newPath, fromDir );
    if( pathSeparator ) {
      strncat( newPath, &pathSeparator, 1 );
    }
  }

  if( fromBaseName ) {
    strcat( newPath, fromBaseName );
  }

  if( fromExt ) {
    if( extDot ) {
      strncat( newPath, &extDot, 1 );
    }
    strcat( newPath, fromExt );
  }

  *toFullPath = newPath;
  return 0;
}

/*
 *  OrigoToC program declarations
 */
OrigoOptions options = {};

void PrintBanner() {
  printf( "OrigoToC %s (0x%.8X)\n%s\n\n",
    ORIGOTOC_VERSTRING, ORIGOTOC_VERSION, ORIGOTOC_COPYRIGHT );
}

void Usage() {
  printf( "usage: origotoc source[.ret] [binary.exe]\n" );
}

int ParseOptions( OrigoOptions* toOptionsVar ) {
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

  if( toOptionsVar == NULL ) { return 1; }

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
  toOptionsVar->sourceFileName = sourceFileName;

  result = JoinPath(binaryDir, binaryBaseName, ".rtc", &cFileName);
  if( result || (cFileName == NULL) ) {
    errorResult = 9;
    goto ExitError;
  }
  toOptionsVar->cFileName = cFileName;

  result = JoinPath(binaryDir, binaryBaseName, ".rth", &hFileName);
  if( result || (hFileName == NULL) ) {
    errorResult = 10;
    goto ExitError;
  }
  toOptionsVar->hFileName = hFileName;

  result = JoinPath(binaryDir, binaryBaseName, binaryExt, &binaryFileName);
  if( result || (binaryFileName == NULL) ) {
    errorResult = 11;
    goto ExitError;
  }
  toOptionsVar->binaryFileName = binaryFileName;

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

void Error( unsigned ofCode, const char* withMessage ) {
  printf( "Error[%u]: %s\n", ofCode, withMessage );
  exit(1);
}

void SyntaxError( unsigned onLine, unsigned onColumn,
  const char* prefix, const char* message ) {

  printf( "%s[L%u,C%u]: %s\n", prefix, onLine, onColumn, message );
  exit(1);
}

/*
 *  Symbol table declarations
 */

#include "keyarray.h"

SymTable* symTable = NULL;
SymTable* localTable = NULL;

void FreeSymbol( Symbol* data ) {
}

int CopySymbol( Symbol* dest, Symbol* source ) {
  return 0;
}

DECLARE_STRING_KEYARRAY_CREATE( CreateSymTable, SymTable )
DECLARE_STRING_KEYARRAY_FREE( FreeSymTable, SymTable, FreeSymbol )

DECLARE_STRING_KEYARRAY_INSERT( InsertSymbol, SymTable, Symbol )
DECLARE_STRING_KEYARRAY_REMOVE( RemoveSymbol, SymTable, FreeSymbol )

DECLARE_STRING_KEYARRAY_RETRIEVE( LookupSymbol, SymTable, Symbol )
DECLARE_STRING_KEYARRAY_MODIFY( ModifySymbol, SymTable, Symbol )

DECLARE_STRING_KEYARRAY_FINDINDEX( SymbolIndex, SymTable )

DECLARE_STRING_KEYARRAY_RELEASEUNUSED( ReleaseUnusedSymbols, SymTable )

DECLARE_STRING_KEYARRAY_COPY( CopySymTable, SymTable, Symbol,
  CopySymbol, FreeSymbol )

/*
 *  Lexer declarations
 */

const KeywordItem baseTypeName[] = {
  "bool", baseBool,
  "char", baseChar,
  "fsize", baseFsize,
  "int", baseInt,
  "int16", baseInt16,
  "int32", baseInt32,
  "int64", baseInt64,
  "int8", baseInt8,
  "size", baseSize,
  "uint", baseUint,
  "uint16", baseUint16,
  "uint32", baseUint32,
  "uint64", baseUint64,
  "uint8", baseUint8
};
const size_t baseTypeCount = sizeof(baseTypeName)
  / sizeof(baseTypeName[0]);

const KeywordItem topLevelKeyword[] = {
  "const", tlConst,
  "enum", tlEnum,
  "func", tlFunc,
  "funcdecl", tlFuncDecl,
  "import", tlImport,
  "interface", tlInterface,
  "method", tlMethod,
  "object", tlObject,
  "run", tlRun,
  "struct", tlStruct,
  "type", tlType,
  "union", tlUnion,
  "var", tlVar
};
const size_t topLevelCount = sizeof(topLevelKeyword)
  / sizeof(topLevelKeyword[0]);

const KeywordItem statementKeyword[] = {
  "bind", stmtBind,
  "break", stmtBreak,
  "else", stmtElse,
  "elseif", stmtElseIf,
  "endfor", stmtEndFor,
  "endif", stmtEndIf,
  "endwhile", stmtEndWhile,
  "for", stmtFor,
  "goto", stmtGoto,
  "if", stmtIf,
  "next", stmtNext,
  "repeat", stmtRepeat,
  "when", stmtWhen,
  "while", stmtWhile
};
const size_t statementCount = sizeof(statementKeyword)
  / sizeof(statementKeyword[0]);

const KeywordItem reservedWord[] = {
  "bind", stmtBind,
  "bool", baseBool,
  "break", stmtBreak,
  "char", baseChar,
  "const", tlConst,
  "else", stmtElse,
  "elseif", stmtElseIf,
  "endfor", stmtEndFor,
  "endif", stmtEndIf,
  "endwhile", stmtEndWhile,
  "enum", tlEnum,
  "for", stmtFor,
  "fsize", baseFsize,
  "func", tlFunc,
  "funcdecl", tlFuncDecl,
  "goto", stmtGoto,
  "if", stmtIf,
  "import", tlImport,
  "int", baseInt,
  "int16", baseInt16,
  "int32", baseInt32,
  "int64", baseInt64,
  "int8", baseInt8,
  "interface", tlInterface,
  "method", tlMethod,
  "next", stmtNext,
  "object", tlObject,
  "repeat", stmtRepeat,
  "run", tlRun,
  "size", baseSize,
  "struct", tlStruct,
  "type", tlType,
  "uint", baseUint,
  "uint16", baseUint16,
  "uint32", baseUint32,
  "uint64", baseUint64,
  "uint8", baseUint8,
  "union", tlUnion,
  "var", tlVar,
  "when", stmtWhen,
  "while", stmtWhile
};
const size_t reservedCount = sizeof(reservedWord)
  / sizeof(reservedWord[0]);

RetFile retFile = {};

void MarkToken( RetFile* fromSource ) {
  if( fromSource == NULL ) { Error( 1, "MarkToken" ); }

  fromSource->markedPos = ftell(fromSource->handle);
  fromSource->markedLine = fromSource->line;
  fromSource->markedColumn = fromSource->column;
}

void ReturnToken( RetFile* fromSource ) {
  if( fromSource == NULL ) { Error( 1, "ReturnToken" ); }

  if( fseek(fromSource->handle, fromSource->markedPos, SEEK_SET) ) {
    Error( 2, "ReturnToken" );
  }

  fromSource->line = fromSource->markedLine;
  fromSource->column = fromSource->markedColumn;

  fromSource->markedPos = 0;
  fromSource->markedLine = 0;
  fromSource->markedColumn = 0;
}

int ReadChar( RetFile* fromSource ) {
  int columnInc = 1;
  char tmpCh;

  if( fromSource && fromSource->handle ) {

    if( fromSource->curCh == '\n' ) {
      fromSource->line++;
      fromSource->column = 1;
      columnInc = 0;
    }

    fromSource->curCh = fromSource->nextCh;
    fromSource->nextCh = fgetc(fromSource->handle);

    if( fromSource->nextCh == '\r' ) {
      tmpCh = fgetc(fromSource->handle);
      if( tmpCh != '\n' ) {
        if( fseek( fromSource->handle, -sizeof(char), SEEK_CUR ) != 0 ) {
          Error( 2, "ReadChar" );
        }
      }
      fromSource->nextCh = '\r';
    }

    fromSource->column += columnInc;
  }

  return fromSource->curCh;
}

int ReadIdentChar( RetFile* fromSource ) {
  int identCh;

  if( fromSource == NULL ) { return EOF; }

  identCh = ReadChar(fromSource);

  // Only read _azAZ
  if( identCh == '_' ) { return identCh; }
  if( isalnum(identCh) ) { return identCh; }

  return EOF;
}

int ReadIdent( RetFile* fromSource, char* toIdent ) {
  char tmpIdent[IDENT_MAXLEN] = {};
  size_t identLen = 0;

  if( fromSource == NULL ) { return 1; }
  if( toIdent == NULL ) { return 2; }

  MarkToken( fromSource );

  if( (fromSource->curCh != '_')
    && (isalpha(fromSource->curCh) == 0) ) { return 3; }

  do {
    if( identLen < IDENT_MAXINDEX ) {
      tmpIdent[identLen] = fromSource->curCh;
      tmpIdent[identLen + 1] = '\0';
    }
    identLen++;
  } while( ReadIdentChar(fromSource) != EOF );

  if( identLen == 0 ) { return 4; }

  strcpy( toIdent, tmpIdent );

  return 0;
}

unsigned FindTopLevelKeyword( const char* identName ) {
  size_t leftIndex = 0;
  size_t rightIndex = topLevelCount;
  size_t keywordIndex = topLevelCount / 2;
  int    compareCode = 0;

  if( (identName == NULL) || (*identName == '\0') ) { return 0; }

  while( leftIndex < rightIndex ) {
    compareCode = strcmp(topLevelKeyword[keywordIndex].name, identName);
    if( compareCode == 0 ) {
      return topLevelKeyword[keywordIndex].tokenCode;
    }

    if( compareCode > 0 ) {
      rightIndex = keywordIndex;
    } else {
      leftIndex = keywordIndex + 1;
    }

    keywordIndex = (leftIndex + rightIndex) / 2;
  }

  return 0;
}

int ReadTopLevelKeyword( RetFile* fromSource, unsigned* toTokenCode ) {
  char ident[IDENT_MAXLEN] = {};
  unsigned token = 0;
  int result = 0;

  if( fromSource == NULL ) { return 1; }
  if( toTokenCode == NULL ) { return 2; }

  result = ReadIdent(fromSource, ident);
  if( result ) { return 3; }

  token = FindTopLevelKeyword(ident);
  if( token == 0 ) { return 4; }
  *toTokenCode = token;

  return 0;
}

int ReadBinaryDigit( RetFile* fromSource ) {
  int binDigit;

  binDigit = ReadChar(fromSource);

  // Skip separators
  while( binDigit == '_' ) {
    binDigit = ReadChar(fromSource);
  }

  // Only read 01
  if( (binDigit == '0') || (binDigit == '1') ) { return binDigit; }

  return EOF;
}

int ReadBinaryNumber( RetFile* fromSource, unsigned* toUint ) {
  unsigned checkNumber;
  unsigned binaryNumber = 0;

  if( fromSource == NULL ) { return 1; }
  if( toUint == NULL ) { return 2; }

  while( ReadBinaryDigit(fromSource) != EOF ) {
    checkNumber = binaryNumber;
    binaryNumber = (binaryNumber << 1) + (fromSource->curCh - '0');

    if( checkNumber > binaryNumber ) { SyntaxError(
      fromSource->tokenLine, fromSource->tokenColumn,
      "Overflow", "Too many digits in number" );
    }
  }

  *toUint = binaryNumber;

  return 0;
}

int ReadOctalDigit( RetFile* fromSource ) {
  int octalDigit;

  octalDigit = ReadChar(fromSource);

  // Skip separators
  while( octalDigit == '_' ) {
    octalDigit = ReadChar(fromSource);
  }

  // Only read 07
  if( (octalDigit >= '0') && (octalDigit <= '7') ) { return octalDigit; }

  return EOF;
}

int ReadOctalNumber( RetFile* fromSource, unsigned* toUint ) {
  unsigned checkNumber;
  unsigned octalNumber = 0;

  if( fromSource == NULL ) { return 1; }
  if( toUint == NULL ) { return 2; }

  while( ReadOctalDigit(fromSource) != EOF ) {
    checkNumber = octalNumber;
    octalNumber = (octalNumber << 3) + (fromSource->curCh - '0');

    if( checkNumber > octalNumber ) { SyntaxError(
      fromSource->tokenLine, fromSource->tokenColumn,
      "Overflow", "Too many digits in number" );
    }
  }

  *toUint = octalNumber;

  return 0;
}

int ReadHexDigit( RetFile* fromSource ) {
  int hexDigit;

  hexDigit = ReadChar(fromSource);

  // Skip separators
  while( hexDigit == '_' ) {
    hexDigit = ReadChar(fromSource);
  }

  // Only read 09afAF
  if( isxdigit(hexDigit) ) { return tolower(hexDigit); }

  return EOF;
}

int ReadHexNumber( RetFile* fromSource, unsigned* toUint ) {
  unsigned checkNumber;
  unsigned hexNumber = 0;

  if( fromSource == NULL ) { return 1; }
  if( toUint == NULL ) { return 2; }

  while( ReadHexDigit(fromSource) != EOF ) {
    checkNumber = hexNumber;

    if( (fromSource->curCh >= 'a') && (fromSource->curCh <= 'f') ) {
      hexNumber = (hexNumber << 3) + (fromSource->curCh - 'a') + 10;
    } else {
      hexNumber = (hexNumber << 3) + (fromSource->curCh - '0');
    }

    if( checkNumber > hexNumber ) { SyntaxError(
      fromSource->tokenLine, fromSource->tokenColumn,
      "Overflow", "Too many digits in number" );
    }
  }

  *toUint = hexNumber;

  return 0;
}

int ReadDecimalDigit( RetFile* fromSource ) {
  int digit;

  digit = ReadChar(fromSource);

  // Skip separators
  while( digit == '_' ) {
    digit = ReadChar(fromSource);
  }

  // Only read 09
  if( (digit >= '0') && (digit <= '9') ) { return digit; }

  return EOF;
}

int ReadDecimalNumber( RetFile* fromSource, unsigned* toUint ) {
  unsigned checkNumber;
  unsigned decimalNumber = 0;

  if( fromSource == NULL ) { return 1; }
  if( toUint == NULL ) { return 2; }

  if( isdigit(fromSource->curCh) == 0 ) { return 3; }

  do {
    checkNumber = decimalNumber;
    decimalNumber = (decimalNumber * 10) + (fromSource->curCh - '0');

    if( checkNumber > decimalNumber ) { SyntaxError(
      fromSource->tokenLine, fromSource->tokenColumn,
      "Overflow", "Too many digits in number" );
    }
  } while( ReadDecimalDigit(fromSource) != EOF );

  *toUint = decimalNumber;

  return 0;
}

int ReadNumber( RetFile* fromSource, unsigned* toUint ) {
  unsigned number = 0;
  int result = 0;

  if( fromSource == NULL ) { return 1; }
  if( toUint == NULL ) { return 2; }

  MarkToken( fromSource );

  // Parse binary, octal, or hex
  if( fromSource->curCh == '0' ) {
    ReadChar( fromSource ); // Skip 0

    switch( fromSource->curCh ) {
    case 'b': result = ReadBinaryNumber( fromSource, &number ); break;
    case 'o': result = ReadOctalNumber( fromSource, &number ); break;
    case 'x': result = ReadHexNumber( fromSource, &number ); break;

    default:
      if( isdigit(fromSource->curCh) == 0 ) {
        SyntaxError( fromSource->tokenLine, fromSource->tokenColumn,
          "Expected", "0b, 0o, or 0x" );
      }
      result = ReadDecimalNumber( fromSource, &number );
    }
  } else {
    result = ReadDecimalNumber( fromSource, &number );
  }

  if( result ) { Error( result, "ReadNumber" ); }

  *toUint = number;

  return 0;
}

int ReadString( RetFile* fromSource, char** toString ) {
  if( fromSource == NULL ) { return 1; }
  if( toString == NULL ) { return 2; }

  MarkToken( fromSource );

  return 3;
}

int OpenRet( const char* fileName, RetFile* toSourceVar ) {
  if( fileName == NULL ) { return 1; }
  if( toSourceVar == NULL ) { return 2; }

  CloseRet( toSourceVar );

  toSourceVar->handle = fopen(fileName, "r");
  if( toSourceVar->handle == NULL ) { return 3; }

  toSourceVar->line = 1;
  toSourceVar->column = 1;
  toSourceVar->tokenLine = 1;
  toSourceVar->tokenColumn = 1;

  ReadChar( toSourceVar );
  ReadChar( toSourceVar );

  return 0;
}

void CloseRet( RetFile* sourceVar ) {
  if( sourceVar && sourceVar->handle ) {
    fclose( sourceVar->handle );
    sourceVar->handle = NULL;

    memset( sourceVar, 0, sizeof(RetFile) );
    sourceVar->line = 1;
    sourceVar->column = 1;
    sourceVar->tokenLine = 1;
    sourceVar->tokenColumn = 1;
  }
}

/*
 *  C code generator declarations
 */

CFile cGen = {};

/*
 *  Parser declarations
 */

int SkipSpace( RetFile* fromSource ) {
  if( fromSource ) {
    if( isspace(fromSource->curCh) ) {
      while( isspace(fromSource->curCh) ) {
        ReadChar( fromSource );
      }
    }
    return 0;
  }
  return 2;
}

int SkipComment( RetFile* fromSource ) {
  unsigned commentLevel = 0;
  if( fromSource == NULL ) { return 1; }

  if( (fromSource->curCh == '/') && (fromSource->nextCh == '*') ) {
    ReadChar( fromSource );
    ReadChar( fromSource );
    commentLevel++;

    while( commentLevel ) {
      if( fromSource->curCh == EOF ) {
        return 2;
      }

      if( (fromSource->curCh == '/') && (fromSource->nextCh == '*') ) {
        ReadChar( fromSource );
        ReadChar( fromSource );
        if( commentLevel == ((unsigned)-1) ) {
          return 3;
        }
        commentLevel++;
        continue;
      }

      if( (fromSource->curCh == '*') && (fromSource->nextCh == '/') ) {
        ReadChar( fromSource );
        ReadChar( fromSource );
        if( commentLevel == 0 ) {
          return 4;
        }
        commentLevel--;
        continue;
      }
      ReadChar( fromSource );
    }
    return 0;
  }

  if( (fromSource->curCh == '/') && (fromSource->nextCh == '/') ) {
    ReadChar( fromSource );
    ReadChar( fromSource );
    while( fromSource->curCh != '\n' ) {
      if( fromSource->curCh == EOF ) { return 5; }
      ReadChar( fromSource );
    }
    return 0;
  }

  return 6;
}

void SkipNonterminals( RetFile* fromSource ) {
  while( (SkipSpace(fromSource) | SkipComment(fromSource)) == 0 ) {
    if( fromSource->curCh == EOF ) {
      break;
    }
  }

  fromSource->tokenLine = fromSource->line;
  fromSource->tokenColumn = fromSource->column;
}

int Match( RetFile* fromSource, const char* withText ) {
  const char* textCh = withText;
  unsigned startLine = 0;
  unsigned startColumn = 0;
  int result = 0;

  if( withText == NULL ) { return 1; }
  if( fromSource == NULL ) { return 2; }

  MarkToken( fromSource );

  startLine = fromSource->line;
  startColumn = fromSource->column;

  while( *textCh ) {
    if( *textCh != fromSource->curCh ) { return 3; }
    ReadChar( fromSource );
    textCh++;
  }

  return 0;
}

// program IDENT
int ParseProgram( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable ) {
  char programName[IDENT_MAXLEN] = {};
  int result = 0;

  if( fromSource == NULL ) { return 1; }
  if( toCgen == NULL ) { return 2; }
  if( usingSymTable == NULL ) { return 3; }

  SkipNonterminals( fromSource );
  result = Match(fromSource, "program");
  if( result ) { SyntaxError( fromSource->tokenLine,
    fromSource->tokenColumn, "Expected", "program" ); }

  SkipNonterminals( fromSource );
  result = ReadIdent(fromSource, programName);
  if( result ) { SyntaxError( fromSource->tokenLine,
    fromSource->tokenColumn, "Expected", "Identifier" ); }
}

unsigned FindBaseType( const char* identName ) {
  size_t leftIndex = 0;
  size_t rightIndex = baseTypeCount;
  size_t baseTypeIndex = baseTypeCount / 2;
  int    compareCode = 0;

  if( (identName == NULL) || (*identName == '\0') ) { return 0; }

  while( leftIndex < rightIndex ) {
    compareCode = strcmp(baseTypeName[baseTypeIndex].name, identName);
    if( compareCode == 0 ) {
      return baseTypeName[baseTypeIndex].tokenCode;
    }

    if( compareCode > 0 ) {
      rightIndex = baseTypeIndex;
    } else {
      leftIndex = baseTypeIndex + 1;
    }

    baseTypeIndex = (leftIndex + rightIndex) / 2;
  }

  return 0;
}

// [@][SIMPLETYPE] ['[' CONSTEXPR [','...] ']']
// @[SIMPLETYPE] ['[' [CONSTEXPR [','...]] ']']
void ParseTypeSpec( RetFile* fromSource, SymTable* usingSymTable,
  TypeSpec* toTypeSpec ) {

  TypeSpec typeSpec = {};
  int result = 0;
  unsigned uintNum = 0;

  if( fromSource == NULL ) { Error( 1, "ParseTypeSpec" ); }
  if( toTypeSpec == NULL ) { Error( 2, "ParseTypeSpec" ); }

  // Parse pointer and allow array to not have a dimension
  if( fromSource->curCh == '@' ) {
    ReadChar( fromSource ); // Skip @
    typeSpec.pointerType = ptrData;
  }

  // Read simple type [name], if present. Undeclared ident is an error
  // if followed by array dimension. Otherwise, rewind ident. Defer
  // error until array is parsed, if present.
  SkipNonterminals( fromSource );
  result = ReadIdent(fromSource, typeSpec.simpleTypeName);

  typeSpec.simpleType = FindBaseType(typeSpec.simpleTypeName);
  if( typeSpec.simpleType == 0 ) {
  }

  // Read array dimension
  if( fromSource->curCh == '[' ) {
    ReadChar( fromSource ); // Skip [

    // Parse array dimension. Require unless type spec is a pointer.
    SkipNonterminals( fromSource );

    result = ReadNumber(fromSource, &uintNum);
    if( result && (typeSpec.pointerType == 0) ) {
      SyntaxError( fromSource->tokenLine, fromSource->tokenColumn,
        "Expected", "Constant expression" );
    }
    typeSpec.dimCount = uintNum;

    // Parse closing brace
    SkipNonterminals( fromSource );
    if( fromSource->curCh != ']' ) {
      SyntaxError( fromSource->tokenLine, fromSource->tokenColumn,
        "Expected", "]" );
    }
    ReadChar( fromSource );
  }

  *toTypeSpec = typeSpec;
}

int ParseEnum( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable ) {
  if( fromSource == NULL ) { return 1; }
  if( toCgen == NULL ) { return 2; }
  if( usingSymTable == NULL ) { return 3; }

  printf( "ParseEnum\n" );

  return 4;
}

int ParseUnion( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable ) {
  if( fromSource == NULL ) { return 1; }
  if( toCgen == NULL ) { return 2; }
  if( usingSymTable == NULL ) { return 3; }
  return 4;
}

int ParseStruct( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable ) {
  if( fromSource == NULL ) { return 1; }
  if( toCgen == NULL ) { return 2; }
  if( usingSymTable == NULL ) { return 3; }
  return 4;
}

int ParseType( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable ) {
  if( fromSource == NULL ) { return 1; }
  if( toCgen == NULL ) { return 2; }
  if( usingSymTable == NULL ) { return 3; }
  return 4;
}

int ParseConst( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable ) {
  if( fromSource == NULL ) { return 1; }
  if( toCgen == NULL ) { return 2; }
  if( usingSymTable == NULL ) { return 3; }
  return 4;
}

int ParseVar( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable ) {
  if( fromSource == NULL ) { return 1; }
  if( toCgen == NULL ) { return 2; }
  if( usingSymTable == NULL ) { return 3; }
  return 4;
}

int ParseFuncDecl( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable ) {
  if( fromSource == NULL ) { return 1; }
  if( toCgen == NULL ) { return 2; }
  if( usingSymTable == NULL ) { return 3; }
  return 4;
}

int ParseImport( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable ) {
  if( fromSource == NULL ) { return 1; }
  if( toCgen == NULL ) { return 2; }
  if( usingSymTable == NULL ) { return 3; }
  return 4;
}

void ParseLocalVar( RetFile* fromSource, CFile* toCgen,
  SymTable* usingSymTable, SymTable* usingLocalTable ) {

  char ident[IDENT_MAXLEN] = {};
  int result = 0;
  TypeSpec varType;

  if( fromSource == NULL ) { Error( 1, "ParseLocalVar" ); }
  if( toCgen == NULL ) { Error( 2, "ParseLocalVar" ); }
  if( usingSymTable == NULL ) { Error( 3, "ParseLocalVar" ); }
  if( usingLocalTable == NULL ) { Error( 4, "ParseLocalVar" ); }

  do {
    SkipNonterminals( fromSource );
    result = ReadIdent(fromSource, ident);

    if( strcmp(ident, "end") == 0 ) { return; }

    SkipNonterminals( fromSource );
    memset( &varType, 0, sizeof(varType) );
    ParseTypeSpec( fromSource, usingSymTable, &varType );

    do {
putchar( '.' );
      SkipNonterminals( fromSource );
      memset( ident, 0, sizeof(ident) );
      result = ReadIdent(fromSource, ident);
      if( result ) { 
        if( fromSource->curCh == ',' ) {
          ReadChar( fromSource );
          continue;
        }
        SyntaxError( fromSource->tokenLine, fromSource->tokenColumn,
        "Expected", "local variable declaration" );
      }
    } while( fromSource->curCh != EOF );
  } while( fromSource->curCh != EOF );
}

// Parse if
int ParseStatement( RetFile* fromSource, CFile* toCgen,
  SymTable* usingSymTable, SymTable* usingLocalTable ) {

  if( fromSource == NULL ) { return 1; }
  if( toCgen == NULL ) { return 2; }
  if( usingSymTable == NULL ) { return 3; }

  return 5;
}

// Parse return
int ParseFuncStatement( RetFile* fromSource, CFile* toCgen,
  SymTable* usingSymTable, SymTable* usingLocalTable ) {

  if( fromSource == NULL ) { return 1; }
  if( toCgen == NULL ) { return 2; }
  if( usingSymTable == NULL ) { return 3; }

  return 5;
}

int ParseFunc( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable ) {
  if( fromSource == NULL ) { return 1; }
  if( toCgen == NULL ) { return 2; }
  if( usingSymTable == NULL ) { return 3; }
  return 4;
}

int ParseObject( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable ) {
  if( fromSource == NULL ) { return 1; }
  if( toCgen == NULL ) { return 2; }
  if( usingSymTable == NULL ) { return 3; }
  return 4;
}

int ParseInterface( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable ) {
  if( fromSource == NULL ) { return 1; }
  if( toCgen == NULL ) { return 2; }
  if( usingSymTable == NULL ) { return 3; }
  return 4;
}

int ParseMethod( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable ) {
  if( fromSource == NULL ) { return 1; }
  if( toCgen == NULL ) { return 2; }
  if( usingSymTable == NULL ) { return 3; }
  return 4;
}

// run var...end statement... end
void ParseRun( RetFile* fromSource, CFile* toCgen,
  SymTable* usingSymTable ) {

  char ident[IDENT_MAXLEN] = {};
  int result = 0;

  if( fromSource == NULL ) { Error( 1, "ParseRun" ); }
  if( toCgen == NULL ) { Error( 2, "ParseRun" ); }
  if( usingSymTable == NULL ) { Error( 3, "ParseRun" ); }

  // Create local symbol table
  if( localTable ) { Error( 4, "ParseRun" ); }
  localTable = CreateSymTable(0);
  if( localTable == NULL ) { Error( 5, "ParseRun" ); }

  if( fromSource->runDeclared ) {
    SyntaxError(fromSource->tokenLine, fromSource->tokenColumn,
      "Duplicate", "run already declared.");
  }
  fromSource->runDeclared = -1;

  // Skip run
  SkipNonterminals( fromSource );
  result = ReadIdent(fromSource, ident);
  if( result ) { SyntaxError(fromSource->tokenLine, fromSource->tokenColumn,
    "Expected", "var block or statement"); }

  // Parse local var declarations
  while( strcmp(ident, "var") == 0 ) {
    ParseLocalVar(fromSource, toCgen, usingSymTable, localTable);

    // Skip end
    SkipNonterminals( fromSource );
    result = ReadIdent(fromSource, ident);
    if( result ) { SyntaxError(fromSource->tokenLine,
      fromSource->tokenColumn, "Expected", "var block or statement"); }
  }

  // Parse statements
  do {
    if( strcmp(ident, "end") == 0 ) { break; }

    SkipNonterminals( fromSource );
    result = ReadIdent(fromSource, ident);
    if( result ) { SyntaxError(fromSource->tokenLine,
      fromSource->tokenColumn, "Expected", "var block or statement"); }
  } while( fromSource->curCh != EOF );

  FreeSymTable( &localTable );
}

void Parse( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable ) {
  char keyword[IDENT_MAXLEN] = {};
  unsigned keywordToken = 0;
  int declResult = 0;
  int result = 0;

  ParseProgram( fromSource, toCgen, usingSymTable );

  while( fromSource->curCh != EOF ) {
    SkipNonterminals( fromSource );
    keywordToken = 0;
    result = ReadTopLevelKeyword(fromSource, &keywordToken);
    if( result ) {
      if( fromSource->curCh == EOF ) { return; }
      Error(result, "Parse > ReadTopLevelKeyword");
    }

    declResult = 0;
    result = 0;
    switch( keywordToken ) {
    case tlEnum:
      declResult = ParseEnum(fromSource, toCgen, usingSymTable);
      result = 4;
      break;

    case tlUnion:
      declResult = ParseUnion(fromSource, toCgen, usingSymTable);
      result = 5;
      break;

    case tlStruct:
      declResult = ParseStruct(fromSource, toCgen, usingSymTable);
      result = 6;
      break;

    case tlType:
      declResult = ParseType(fromSource, toCgen, usingSymTable);
      result = 7;
      break;

    case tlConst:
      declResult = ParseConst(fromSource, toCgen, usingSymTable);
      result = 8;
      break;

    case tlVar:
      declResult = ParseVar(fromSource, toCgen, usingSymTable);
      result = 9;
      break;

    case tlFuncDecl:
      declResult = ParseFuncDecl(fromSource, toCgen, usingSymTable);
      result = 10;
      break;

    case tlImport:
      declResult = ParseImport(fromSource, toCgen, usingSymTable);
      result = 11;
      break;

    case tlFunc:
      declResult = ParseFunc(fromSource, toCgen, usingSymTable);
      result = 12;
      break;

    case tlObject:
      declResult = ParseObject(fromSource, toCgen, usingSymTable);
      result = 13;
      break;

    case tlInterface:
      declResult = ParseInterface(fromSource, toCgen, usingSymTable);
      result = 14;
      break;

    case tlMethod:
      declResult = ParseMethod(fromSource, toCgen, usingSymTable);
      result = 15;
      break;

    case tlRun: 
      ParseRun(fromSource, toCgen, usingSymTable);
      break;

    default:
      SyntaxError( fromSource->tokenLine, fromSource->tokenColumn,
        "Expected", "top level declaration" );
    }

    if( declResult ) {
      Error( result, "Parse > switch" );
    }
  }
}

/*
 *  Main program declarations
 */

void Cleanup() {
  // Release memory used by source file
  CloseRet( &retFile );

  // Release memory used by symbol table
  FreeSymTable( &symTable );
  FreeSymTable( &localTable );

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

  symTable = CreateSymTable(0);
  Parse( &retFile, &cGen, symTable );

  return 0;
}
