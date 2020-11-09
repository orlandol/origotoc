
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Declarations
 */

  typedef enum ErrorCodes {
    errorInvalidCommandline = 1,
    errorSLCommentClosedWithEOF,
    errorMLCommentNestedTooDeep,
    errorMLCommentClosedWithEOF,
    errorMLCommentClosedBeforeOpening,
    errorOpeningRetFile,
    errorCreatingCFile,
    errorCreatingHeaderFile,
    errorReadingIdentifier,
    expectedKeywordProgram,
    expectedUndeclaredIdentifier,
    expectedRun,
    expectedRunOrTopLevel,
    expectedTopLevel,
    runAlreadyDeclared,
    errorCFileNotOpen,
    errorHeaderFileNotOpen,
    expectedEndOrStatement,
  } ErrorCodes;

  typedef struct TokenVal {
    unsigned valType;

    union {
      unsigned valUint;
      int valInt;
      char valChar;
    };
  } TokenVal;

  typedef struct Keyword {
    char*    name;
    unsigned token;
  } Keyword;

  typedef struct Operator {
    char* text;
    unsigned token;
  } Operator;

  typedef struct ArrayDimension {
    int minValue;
    int maxValue;
  } ArrayDimension;

  typedef struct TypeSpec {
    unsigned ptrType;
    unsigned baseType;
    char baseName[1024];
    unsigned dimensionCount;
    ArrayDimension* dimension;
  } TypeSpec;

  int RunProgram( char* commandLine );

  void Cleanup();

  void ParseOptions( int argc, char* argv[] );

  unsigned FindKeyword( char* identifier );

  FILE* OpenRet( char* inRetName );
  void CloseRet( FILE** fileRet );

  FILE* CreateC( char* outCName );
  void CloseC( FILE** fileC );

  FILE* CreateHeader( char* outHeaderName );
  void CloseHeader( FILE** fileHeader );

  char ReadChar();

  void SkipSpace();
  void SkipSpaceAndComments();

  int ReadIdent( char* destIdent, size_t destMaxLen );

  int ReadBinNum( unsigned* destNum );
  int ReadOctalNum( unsigned* destNum );
  int ReadHexNum( unsigned* destNum );
  int ReadNum( unsigned* destNum );
  int ReadString( char* destString, size_t destMaxLen );
  unsigned ReadOperator();

  unsigned GetToken();
  unsigned FindKeyword( char* identifier );
  unsigned TranslateKeyword( unsigned originalToken, char* originalTokenStr );

  int ReadTypeSpec( TypeSpec* destSpec );
  void ReleaseTypeSpec( TypeSpec* typeSpec );

  void BeginParse();
  void ParseProgramHeader();
  void ParseRun();
  void ParseEndRun();
  void ParseTopLevel();
  void EndParse();

/*
 * Global variables
 */

  char baseName[FILENAME_MAX + 1] = {};

  char retName[FILENAME_MAX + 1] = {};
  char exeName[FILENAME_MAX + 1] = {};
  char cName[FILENAME_MAX + 1] = {};
  char headerName[FILENAME_MAX + 1] = {};

  FILE* retFile = NULL;
  FILE* cFile = NULL;
  FILE* headerFile = NULL;

  typedef enum Token {
    // General tokens
    tkGeneral = (0 << 9),
      tkEOF,
      tkEOL,
      tkIdent,
      tkDot,
      tkDotDot,
      tkLParen,
      tkRParen,
      tkLBrace,
      tkRBrace,
      tkLDoubleBrace,
      tkRDoubleBrace,
      tkComma,
      tkColon,
      tkAt,
      tkHash,

    // Reserved high level keyword tokens
    rsvdIdent = (1 << 9),
      rsvdProgram,
        firstRsvd = rsvdProgram,
      rsvdEnum,
      rsvdType,
      rsvdStruct,
      rsvdObject,
      rsvdImport,
      rsvdPublic,
      rsvdMutable,
      rsvdExtends,
      rsvdImplements,
      rsvdMethod,
      rsvdSelf,
      rsvdFunc,
      rsvdAsm,
      rsvdNoRet,
      rsvdNoFrame,
      rsvdReturn,
      rsvdVar,
      rsvdRun,
      rsvdEnd,
      rsvdIf,
      rsvdThen,
      rsvdThenIf,
      rsvdElseIf,
      rsvdElse,
      rsvdEndIf,
      rsvdWhile,
      rsvdNext,
      rsvdStop,
      rsvdEndWhile,
      rsvdIn,
      rsvdExit,
        lastRsvd = rsvdExit,

    // Type tokens
    typeDecl = (2 << 9),
      typeInt,
        firstType = typeInt,
      typeInt8,
      typeInt16,
      typeInt32,
      typeUint,
      typeUint8,
      typeUint16,
      typeUint32,
      typeChar,
      typeString,
      typeBool,
      typeStruct,
      typeUnion,
        lastType = typeUnion,

    // Pointer tokens
    ptrDecl = (3 << 9),
      ptrData,
        firstPtr = ptrData,
      ptrRef,
        lastPtr = ptrRef,

    // Literal value tokens
    valImmediate = (4 << 9),
      valInt = (valImmediate + (0 << 5)),
          firstValInt = valInt,
        valInt8,
        valInt16,
        valInt32,
          lastValInt = valInt32,

      valUint = (valImmediate + (1 << 5)),
          firstValUint = valUint,
        valUint8,
        valUint16,
        valUint32,
          lastValUint = valUint32,

      valChar = (valImmediate + (2 << 5)),
          firstValChar = valChar,
          lastValChar = valChar,

      valString = (valImmediate + (3 << 5)),
          firstValString = valString,
          lastValString = valString,

      valBool = (valImmediate + (4 << 5)),
          firstValBool = valBool,
          lastValBool = valBool,

    // Operator tokens
    operSymbol = (5 << 9),
      operPrec00 = (operSymbol + (0 << 5)),
      operPrec01 = (operSymbol + (1 << 5)),
        opPostInc,
        opPostDec,
      operPrec02 = (operSymbol + (2 << 5)),
        opPreInc,
        opPreDec,
        unaryNeg,
        unaryIsNot,
        unaryNot,
      operPrec03 = (operSymbol + (3 << 5)),
      operPrec04 = (operSymbol + (4 << 5)),
        opMul,
        opDiv,
        opMod,
      operPrec05 = (operSymbol + (5 << 5)),
        opAdd,
        opSub,
      operPrec06 = (operSymbol + (6 << 5)),
        opShl,
        opShr,
        opSShr,
        opRol,
        opSRol,
        opRor,
        opSRor,
      operPrec07 = (operSymbol + (7 << 5)),
      operPrec08 = (operSymbol + (8 << 5)),
        opLT,
        opLTEq,
        opGT,
        opGTEq,
      operPrec09 = (operSymbol + (9 << 5)),
        opEq,
        opNotEq,
      operPrec10 = (operSymbol + (10 << 5)),
        opAnd,
      operPrec11 = (operSymbol + (11 << 5)),
        opXor,
      operPrec12 = (operSymbol + (12 << 5)),
        opOr,
      operPrec13 = (operSymbol + (13 << 5)),
        opAndIs,
      operPrec14 = (operSymbol + (14 << 5)),
        opOrIs,
      operPrec15 = (operSymbol + (15 << 5)),

    // Assignment operators
    assignSymbol  = (6 << 9),
      assignTo,
      assignNot,
      assignAdd,
      assignSub,
      assignMul,
      assignDiv,
      assignMod,
      assignShl,
      assignShr,
      assignSShr,
      assignRol,
      assignSRol,
      assignRor,
      assignSRor,
      assignAnd,
      assignXor,
      assignOr
  } Token;

  const Keyword keywordTable[] = {
    "bool",    typeBool,
    "char",    typeChar,
    "end",     rsvdEnd,
    "import",  rsvdImport,
    "int",     typeInt,
    "int16",   typeInt16,
    "int32",   typeInt32,
    "int8",    typeInt8,
    "program", rsvdProgram,
    "run",     rsvdRun,
    "string",  typeString,
    "type",    rsvdType,
    "uint",    typeUint,
    "uint16",  typeUint16,
    "uint32",  typeUint32,
    "uint8",   typeUint8
  };
  const size_t keywordCount = sizeof(keywordTable) / sizeof(keywordTable[0]);

  const Operator operTable[] = {
    "!",    unaryIsNot,
    "!=",   opNotEq,
    "#",    tkHash,
    "%",    opMod,
    "%=",   assignMod,
    "&",    opAnd,
    "&&",   opAndIs,
    "&=",   assignAnd,
    "(",    tkLParen,
    ")",    tkRParen,
    "*",    opMul,
    "*=",   assignMul,
    "+",    opAdd,
    "++",   opPostInc,
    "+=",   assignAdd,
    ",",    tkComma,
    "-",    opSub,
    "--",   opPostDec,
    "-=",   assignSub,
    ".",    tkDot,
    "..",   tkDotDot,
    "/",    opDiv,
    "/=",   assignDiv,
    ":",    tkColon,
    "<",    opLT,
//    "<-<",  opSRol,
//    "<-<=", assignSRol,
    "<<",   opShl,
//    "<<<",  opRol,
//    "<<<=", assignRol,
    "<<=",  assignShl,
    "<=",   opLTEq,
    "=",    assignTo,
    "==",   opEq,
    ">",    opGT,
//    ">->",  opSRor,
//    ">->=", assignSRor,
    ">=",   opGTEq,
    ">>",   opShr,
    ">>=",  assignShr,
//    ">>>",  opRor,
//    ">>>=", assignRor,
    "@",    tkAt,
    "[",    tkLBrace,
    "[[",   tkLDoubleBrace,
    "]",    tkRBrace,
    "]]",   tkRDoubleBrace,
    "^",    opXor,
    "^=",   assignXor,
    "|",    opOr,
    "|=",   assignOr,
//    "|>>",  opSShr,
//    "|>>=", assignSShr,
    "||",   opOrIs,
    "~",    unaryNot,
    "~=",   assignNot
  };
  const size_t operCount = sizeof(operTable) / sizeof(operTable[0]);

  char programName[1024] = {};
  int runDeclared = 0;

  unsigned curToken = 0;
  char curCh = 0;
  char curTokenStr[1024] = {};
  TokenVal curTokenVal = {};

  unsigned nextToken = 0;
  char nextCh = 0;
  char nextTokenStr[1024] = {};
  TokenVal nextTokenVal = {};

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
      exit(errorInvalidCommandline);
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
      strncat( retName, ".ret", sizeof(retName) - 1 );
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
      strncat( exeName, ".exe", sizeof(exeName) - 1 );
    } else if( strlen(fileExt) == 1 ) {
      strncat( exeName, "exe", sizeof(exeName) - 1 );
    }

    // Set the C file name
    strncpy( cName, baseName, sizeof(cName) );
    strncat( cName, ".rgc", sizeof(cName) - 1 );

    // Set the C Header file name
    strncpy( headerName, baseName, sizeof(headerName) );
    strncat( headerName, ".rgh", sizeof(headerName) - 1 );
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

  void SkipSpace() {
    while( isspace(curCh) ) {
      ReadChar();
    }
  }

  void SkipSpaceAndComments() {
    unsigned commentLevel;
    unsigned loopFlags;

    do {
      loopFlags = 0;

      // Skip space characters
      if( isspace(curCh) ) {
        while( isspace(curCh) ) {
          ReadChar();
        }
        loopFlags |= 1;
      }

      // Skip single-line comments
      if( (curCh == '/') && (nextCh == '/') ) {
        do {
          if( ReadChar() == 0 ) {
            printf( "Single-line comments must end with CR and/or LF\n" );
            exit(errorSLCommentClosedWithEOF);
          }
        } while( (curCh != '\r') && (curCh != '\n') );
        loopFlags |= 2;
      }

      // Skip multi-line comments
      if( (curCh == '/') && (nextCh == '*') ) {
          commentLevel = 0;
          do {
            if( (curCh == '/') && (nextCh == '*') ) {
              if( commentLevel == ((unsigned)-1) ) {
                printf( "Multi-line comment is nested too many levels deep\n" );
                exit(errorMLCommentNestedTooDeep);
              }
              commentLevel++;
              if( !(ReadChar() && ReadChar()) ) {
                printf( "Unexpected EOF in multi-line comment opened with /*\n" );
                exit(errorMLCommentClosedWithEOF);
              }
              continue;
            }

            if( (curCh == '*') && (nextCh == '/') ) {
              if( commentLevel == 0 ) {
                printf( "Each multi-line comment must be opened with /* before closing with */\n" );
                exit(errorMLCommentClosedBeforeOpening);
              }
              commentLevel--;
              if( !(ReadChar() && ReadChar()) ) {
                return; // Valid within this function's scope
              }
              continue;
            }

            if( ReadChar() == 0 ) {
              return; // Valid within this function's scope
            }
        } while( commentLevel );

        loopFlags |= 4;
      }
    } while( curCh && loopFlags );
  }

  int ReadIdent( char* destIdent, size_t destMaxLen ) {
    size_t destIndex = 0;

    if( !(retFile && destIdent && destMaxLen) ) {
      return 0;
    }

    if( (curCh != '_') && (isalnum(curCh) == 0) ) {
      return 0;
    }

    while( (curCh == '_') || (isalnum(curCh)) ) {
      if( (destIndex + 1) < destMaxLen ) {
        destIdent[destIndex++] = curCh;
      }
      ReadChar();
    }
    destIdent[destIndex] = '\0';

    return -1;
  }

  int ReadBinNum( unsigned* destNum ) {
    unsigned result = 0;

    if( !(retFile && destNum) ) {
      return 0;
    }

    if( (curCh != '0') || (nextCh != 'b') ) {
      return 0;
    }

    ReadChar();
    ReadChar();

    if( !((curCh == '_') || ((curCh >= '0') && (curCh <= '1'))) ) {
      return 0;
    }

    while( curCh ) {
      if( (curCh >= '0') && (curCh <= '1')  ) {
        result = (result << 1) + (curCh - '0');
      } else if( curCh == '_' ) {
      } else {
        break;
      }
      ReadChar();
    }

    *destNum = result;
    return -1;
  }

  int ReadOctalNum( unsigned* destNum ) {
    unsigned result = 0;

    if( !(retFile && destNum) ) {
      return 0;
    }

    if( (curCh != '0') || (nextCh != 'o') ) {
      return 0;
    }

    ReadChar();
    ReadChar();

    if( !((curCh == '_') || ((curCh >= '0') && (curCh <= '7'))) ) {
      return 0;
    }

    while( curCh ) {
      if( (curCh >= '0') && (curCh <= '7')  ) {
        result = (result << 3) + (curCh - '0');
      } else if( curCh == '_' ) {
      } else {
        break;
      }

      ReadChar();
    }

    *destNum = result;
    return -1;
  }

  int ReadHexNum( unsigned* destNum ) {
    unsigned result = 0;

    if( !(retFile && destNum) ) {
      return 0;
    }

    if( (curCh != '0') || (nextCh != 'x') ) {
      return 0;
    }

    ReadChar();
    ReadChar();

    if( !((curCh == '_') || isxdigit(curCh)) ) {
      return 0;
    }

    while( curCh ) {
      if( (curCh >= '0') && (curCh <= '9')  ) {
        result = (result << 4) + (curCh - '0');
      } else if( (curCh >= 'a') && (curCh <= 'f')  ) {
        result = (result << 4) + (curCh - 'a') + 10;
      } else if( (curCh >= 'A') && (curCh <= 'F')  ) {
        result = (result << 4) + (curCh - 'A') + 10;
      } else if( curCh == '_' ) {
      } else {
        break;
      }

      ReadChar();
    }

    *destNum = result;
    return -1;
  }

  int ReadNum( unsigned* destNum ) {
    unsigned result = 0;

    if( !(retFile && destNum) ) {
      return 0;
    }

    if( isdigit(curCh) == 0 ) {
      return 0;
    }

    // Check for other types of integers
    if( curCh == '0' ) {
      switch( nextCh ) {
      case 'b': return ReadBinNum(destNum);
      case 'o': return ReadOctalNum(destNum);
      case 'x': return ReadHexNum(destNum);
      }
    }

    // Skip leading zeroes
    while( curCh == '0' ) {
      ReadChar();
    }

    // Tokenize decimal
    while( (curCh == '_') || isdigit(curCh) ) {
      if( curCh != '_' ) {
        result = (result * 10) + (curCh - '0');
      }
      ReadChar();
    }

    *destNum = result;
    return -1;
  }

  int ReadString( char* destString, size_t destMaxLen ) {
    size_t destIndex = 0;
    char quoteCh;

    if( !(retFile && destString && destMaxLen) ) {
      return 0;
    }

    if( (curCh != '"') && (curCh != '\'') ) {
      return 0;
    }
    quoteCh = curCh;
    ReadChar(); // Skip opening quote character

    while( curCh ) {
      if( curCh == quoteCh ) {
        break;
      }

      if( (destIndex + 1) < destMaxLen ) {
        destString[destIndex++] = curCh;
      }
      ReadChar();
    }
    destString[destIndex] = '\0';
    ReadChar(); // Skip closing quote character

    return -1;
  }

  unsigned ReadOperator() {
    return 0;
  }

  unsigned GetToken() {
    // Set current variables
    curToken = nextToken;
    memcpy( curTokenStr, nextTokenStr, sizeof(nextTokenStr) );
    memcpy( &curTokenVal, &nextTokenVal, sizeof(nextTokenVal) );

    // Initialize next variables
    nextToken = 0;
    memset( nextTokenStr, 0, sizeof(nextTokenStr) );
    memset( &nextTokenVal, 0, sizeof(nextTokenVal) );

    // Prepare to read next token
    SkipSpaceAndComments();

    // Determine next token type, then read it
    if( isdigit(curCh) ) {
      if( ReadNum(&nextTokenVal.valUint) ) {
        nextTokenVal.valType = valUint;
        nextToken = valUint;
      }
      return curToken;
    }

    if( (curCh == '_') || isalnum(curCh) ) {
      if( ReadIdent(nextTokenStr, sizeof(nextTokenStr)) ) {
        nextToken = tkIdent;
      }
      return curToken;
    }

    if( (curCh == '"') || (curCh == '\'') ) {
      if( ReadString(nextTokenStr, sizeof(nextTokenStr)) ) {
        nextTokenVal.valType = valString;
        nextToken = valString;
      }
      return curToken;
    }

    if( ispunct(nextCh) ) {
      nextToken = ReadOperator();
    }

    return curToken;
  }

  unsigned FindKeyword( char* identifier ) {
    size_t leftIndex = 0;
    size_t rightIndex = keywordCount;
    size_t keywordIndex = keywordCount / 2;
    int    compareCode = 0;

    if( !(identifier && (*identifier)) ) {
      return 0;
    }

    while( leftIndex < rightIndex ) {
      compareCode = strcmp(keywordTable[keywordIndex].name, identifier);
      if( compareCode == 0 ) {
        return keywordTable[keywordIndex].token;
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

  unsigned TranslateKeyword( unsigned originalToken, char* originalTokenStr ) {
    unsigned keywordToken;

    if( originalToken == tkIdent ) {
      keywordToken = FindKeyword(originalTokenStr);
      if( keywordToken ) {
        return keywordToken;
      }
    }

    return originalToken;
  }

  int ReadTypeSpec( TypeSpec* destSpec ) {
    TypeSpec tempSpec = {};
    int minIndex;
    int maxIndex;

    if( !(retFile && destSpec) ) {
      return 0;
    }

    if( (curToken == ptrData) || (curToken == ptrRef) ) {
      tempSpec.ptrType = curToken;
      GetToken(); // Skip # or *
    }
    if( (curToken == ptrData) || (curToken == ptrRef) ) {
      return 0; // Double pointers or references not directly supported
    }

    if( (curToken >= firstType) && (curToken <= lastType) ) {
      tempSpec.baseType = curToken;
      memcpy( tempSpec.baseName, curTokenStr, sizeof(curTokenStr) );
      GetToken(); // Skip base type
    }

    if( curToken == tkLBrace ) {
      GetToken(); // Skip array dimension [
;;;
      do {
        if( ReadNum(&minIndex) == 0 ) {
          return 0;
        }
        if( curToken == opSub ) {
          minIndex = -minIndex;
        }
        maxIndex = minIndex;
        GetToken(); // Skip number

        if( curToken == tkComma ) {
          GetToken(); // Skip , and repeat
          continue;
        }
      } while( curToken );
    }

    return 0;

  Cleanup:
    ReleaseTypeSpec( &tempSpec );
    return 0;
  }

  void ReleaseTypeSpec( TypeSpec* typeSpec ) {
    if( typeSpec && typeSpec->dimension ) {
      free( (typeSpec->dimension) );
      (typeSpec->dimension) = NULL;
    }
  }

  void BeginParse() {
    // Pre-read first two characters
    ReadChar();
    ReadChar();

    // Pre-process first two tokens
    GetToken();
    GetToken();

    // Initialize C file
    if( cFile ) {
      fprintf( cFile, "\n#include \"%s\"\n", headerName );
    } else {
      printf( "C file not open\n" );
      exit( errorCFileNotOpen );
    }

    // Initialize Header file
    if( headerFile ) {
      fprintf( headerFile, "#ifndef %s_H\n#define %s_H\n", baseName, baseName );
    } else {
      printf( "Header file not open\n" );
      exit( errorHeaderFileNotOpen );
    }
  }

  void ParseProgramHeader() {
    unsigned programToken;
    unsigned programNameToken;

    // program PROGRAMNAME
    programToken = TranslateKeyword(curToken, curTokenStr);
    if( programToken != rsvdProgram ) {
      printf( "Expected keyword program\n" );
      exit( expectedKeywordProgram );
    }
    GetToken(); // Skip program

    programNameToken = TranslateKeyword(curToken, curTokenStr);
    if( programNameToken != tkIdent ) {
      printf( "Expected undeclared identifier for program name\n" );
      exit( expectedUndeclaredIdentifier );
    }
    memcpy( programName, curTokenStr, sizeof(curTokenStr) );
    GetToken(); // Skip PROGRAMNAME
  }

  void ParseRun() {
    unsigned keywordToken;

    if( runDeclared ) {
      printf( "run is already declared\n" );
      exit( runAlreadyDeclared );
    }
    runDeclared = -1;
    GetToken(); // Skip run

    if( cFile ) {
      fprintf( cFile, "\nint main( int argc, char* argv[] ) {\n" );
    } else {
      printf( "C file not open\n" );
      exit( errorCFileNotOpen );
    }

    // Parse one or more local variable declaration blocks
    do {
      keywordToken = TranslateKeyword(curToken, curTokenStr);
      if( keywordToken != rsvdVar ) {
        break;
      }
      ///TODO: ParseLocalVarBlock
    } while( curToken );

    // Parse run block statements until end of run block
    do {
      keywordToken = TranslateKeyword(curToken, curTokenStr);
      if( (keywordToken >= firstRsvd) && (keywordToken <= lastRsvd) ) {
        switch( keywordToken ) {
        case rsvdEnd:
          ParseEndRun();
          return;

        default:
          printf( "Expected end or statement\n" );
          exit( expectedEndOrStatement );
        }
      }
    } while( curToken );
  }

  void ParseEndRun() {
    GetToken(); // Skip end

    if( cFile ) {
      fprintf( cFile, "  return 0;\n}\n" );
    } else {
      printf( "C file not open\n" );
      exit( errorCFileNotOpen );
    }
  }

  void ParseTopLevel() {
    unsigned topLevelToken;
    unsigned topLevelExitCode = expectedRunOrTopLevel;
    char* topLevelExitStr = "Expected run, or top level keyword\n";

    do {
      topLevelToken = TranslateKeyword(curToken, curTokenStr);

      switch( topLevelToken ) {
      case rsvdRun:
        ParseRun();
        topLevelExitStr = "Expected top level keyword\n";
        topLevelExitCode = expectedTopLevel;
        break;

      default:
        if( topLevelToken ) {
          printf( topLevelExitStr );
          exit( topLevelExitCode );
        }
      }
    } while( curToken );
  }

  void EndParse() {
    GetToken(); // Skip end

    // Finalize C file
    if( runDeclared == 0 ) {
      printf( "Expected run\n" );
      exit( expectedRun );
    }

    // Finalize Header file
    fprintf( headerFile, "\n#endif %s_H\n", baseName );
  }

int main( int argc, char* argv[] ) {
  char ch;

  // Initialize program
  atexit( Cleanup );

  // Process command line
  ParseOptions( argc, argv );

  // Open source file
  retFile = OpenRet(retName);
  if( retFile == 0 ) {
    printf( "Error opening file %s\n", retName );
    exit( errorOpeningRetFile );
  }

  // Create intermediate files
  cFile = CreateC(cName);
  if( cFile == 0 ) {
    printf( "Error creating file %s\n", cName );
    exit( errorCreatingCFile);
  }

  headerFile = CreateHeader(headerName);
  if( headerFile == 0 ) {
    printf( "Error creating file %s\n", headerName );
    exit( errorCreatingHeaderFile );
  }

  // Parse Retineo source into C intermediate files
  printf( "\nParsing %s...\n", retName );

  BeginParse();
  ParseProgramHeader();
  ParseTopLevel();
  EndParse();

  CloseRet( &retFile );
  CloseC( &cFile );
  CloseHeader( &headerFile );

  printf( "Done.\n" );

  // Build executable from C intermediate files
  printf( "\nBuilding %s from %s...\n", exeName, headerName );
  printf( "\n" );

  // Release program resources
  Cleanup();

  return 0;
}
