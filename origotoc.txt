#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "keyarray.h"

/*
 *  Global variables
 */
  int argCount;
  char** argVar;

/*
 *  Token declarations
 */

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

/*
 *  Keyword table declarations
 */

  typedef struct Keyword {
    char*    name;
    unsigned token;
  } Keyword;

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

  unsigned FindKeyword( char* identifier );

/*
 *  System helper declarations
 */

  bool RunProgram( const char* programName, char* programArgs[], int* exitCode );

/*
 *  Error reporting declarations
 */

  // Logic warnings
  enum WarningCodes {
    noWarnings = 0
  };

  void Warning( unsigned line, unsigned column, unsigned warningCode );

  // Syntax errors
  enum ExpectedCodes {
    nothingExpected = 0,
    expectedProgram,
    expectedIdentifier,
    expectedTopLevelOrRun,
    expectedTopLevel,
    expectedRun,
    expectedFuncStatement,
    expectedMethodStatement,
    expectedRunStatement
  };

  void Expected( unsigned line, unsigned column, unsigned expectedCode );

  // Program errors
  enum ErrorCodes {
    noError = 0,
    setExitHandlerFailed,
    unableToOpen,
    unableToCreate,
    unableToRun,
    unableToBuild,
    unableToLink,
    stringOperationFailed
  };

  void Error( unsigned errorCode, char* errorText );

/*
 *  String declarations
 */

  typedef struct rstring {
    size_t length;
    size_t rsvdLength;
  } rstring;

  size_t rstrlen( rstring* source );
  size_t rstrrsvdlen( rstring* source );
  char* rstrtext( rstring* source );

  rstring* rstralloc( size_t reserveLength );
  rstring* rstrzalloc( size_t reserveLength );
  void rstrfree( rstring** rstrPtr );

  void rstrclear( rstring* source );

  rstring* rstrcopy( rstring* source );
  rstring* rstrcopyc( char* source, size_t sourceLength );

  rstring* rstrappendch( rstring* dest, char ch );

  rstring* rstrappend( rstring* dest, rstring* source );
  rstring* rstrappendc( rstring* dest, char* source, size_t sourceLength );

  size_t rrevscan( rstring* source, char matchCh );
  size_t rrevscanc( char* source, char matchCh );

  size_t rscan( rstring* source, char matchCh );
  size_t rscanc( char* source, char matchCh );

  rstring* rsubstr( rstring* source, size_t startPos, size_t endPos );
  rstring* rsubstrc( char* source, size_t sourceLength, size_t startPos, size_t endPos );

/*
 *  Token value declarations
 */

  typedef struct TokenVal {
    unsigned valType;
    union {
      unsigned uVal;
      int iVal;
      char chVal;
      rstring* strVal;
      struct {
        uint8_t* dataVal;
        size_t dataSize;
      };
    };
  } TokenVal;

  void ReleaseTokenVal( TokenVal* tokenVal );

/*
 *  Type specifier declarations
 */

  typedef struct ArrayDimension {
    int minRange;
    int maxRange;
    size_t count;
    size_t offset;
  } ArrayDimension;

  typedef struct TypeSpec {
    // <ptrRef '#' | ptrData '@'>
    unsigned pointerType;

    // <typeStruct, typeUint, etc>
    unsigned baseType;
    union {
      size_t baseSize;
      size_t baseBits;
    };
    rstring* baseTypeName;

    // '[' <[min..max[,...]] | [count[,...]]> ']'
    size_t dimensionCount;
    ArrayDimension* dimension;

    // = defaultValue
    TokenVal initVal;
  } TypeSpec;

  void ReleaseTypeSpec( TypeSpec* typeSpec );

/*
 *  Symbol table declarations
 */

  typedef struct TypeAliasSymbol {
    rstring* typeName;
  } TypeAliasSymbol;

  typedef struct TypeSymbol {
    TypeSpec typeSpec;
    uint8_t* defaultVal;
    unsigned valType;
    size_t valSize;
  } TypeSymbol;

  typedef struct ImportSymbol {
  } ImportSymbol;

  enum SymType {
    symNone = 0,
    symBaseType,
    symType,
    symFuncImport
  };

  typedef struct SymItem {
    unsigned token;

    union {
      TypeAliasSymbol typeAlias;
      TypeSymbol typeSym;
      ImportSymbol importSym;
    };
  } SymItem;

  DECLARE_STRING_KEYARRAY_TYPES( SymTable, SymItem )

  void CopySymItem( SymItem* dest, SymItem* source );
  void FreeSymItem( SymItem* symItem );

  DECLARE_STRING_KEYARRAY_CREATE( CreateSymTab, SymTable )
  DECLARE_STRING_KEYARRAY_FREE( ReleaseSymTab, SymTable, FreeSymItem )

  DECLARE_STRING_KEYARRAY_INSERT( InsertSymbol, SymTable, SymItem )
  DECLARE_STRING_KEYARRAY_REMOVE( RemoveSymbol, SymTable, FreeSymItem )

  DECLARE_STRING_KEYARRAY_RETRIEVE( RetrieveSymbol, SymTable, SymItem )

  bool DeclareType( SymTable* symTab, char* definedType, char* typeName, TokenVal* initVal );

/*
 *  Lexer declarations
 */

  typedef struct RetFile {
    FILE* handle;
    rstring* fileName;
    unsigned line;
    unsigned column;
    char curCh;
    char nextCh;
  } RetFile;

  RetFile* OpenRet( char* fileName, size_t nameLength );
  void CloseRet( RetFile** source );

  bool ReadChar( RetFile* source );

  bool SkipSpace( RetFile* source );
  bool SkipComments( RetFile* source );

  bool ReadIdent( RetFile* source, rstring** ident );

  bool ReadBinNum( RetFile* source, unsigned* num );
  bool ReadOctalNum( RetFile* source, unsigned* num );
  bool ReadHexNum( RetFile* source, unsigned* num );
  bool ReadNum( RetFile* source, unsigned* num );

  bool ReadString( RetFile* source, rstring** string );

  unsigned ReadOperator( RetFile* source );

  unsigned GetToken();
  unsigned TranslateKeyword( unsigned originalToken );

/*
 *  Expression parser declarations
 */

/*
 *  C generation declarations
 */

  typedef struct CGen {
    FILE* cHandle;
    FILE* cIncludeHandle;
    rstring* cFileName;
    rstring* cIncludeName;
    bool runDeclared;
  } CGen;

  CGen* CreateC( char* fileName, size_t nameLength );
  void CloseC( CGen** source );

  bool GenTypeSpec( CGen* csource, TypeSpec* typeSpec );

/*
 *  Parser declarations
 */

  typedef struct IfStack {
    unsigned block;
    unsigned currentIf;
    unsigned nextIf;
    unsigned endIf;
  } IfStack;

  typedef struct LoopStack {
  } LoopStack;

  void ParseProgramHeader();
  void EndParse();

  void ParseType();
  void ParseImport();

  void ParseLocalVar();
  void ParseLocalVarEnd();

  void ParseIf();
  void ParseFor();

  void ParseStatement();

  void ParseRun();
  void ParseRunEnd();

  void ParseTopLevel();

/*
 *  Main program
 */

  rstring* programName = NULL;

  rstring* retFileName = NULL;
  rstring* cFileName = NULL;
  rstring* exeFileName = NULL;

  RetFile* retSource = NULL;
  CGen* cSourceGen = NULL;

  SymTable* symTab = NULL;
  SymTable* localTab = NULL;

  rstring* curIdent = NULL;
  TokenVal curVal = {};
  unsigned curLine = 0;
  unsigned curColumn = 0;
  unsigned curToken = 0;

  rstring* nextIdent = NULL;
  TokenVal nextVal = {};
  unsigned nextLine = 0;
  unsigned nextColumn = 0;
  unsigned nextToken = 0;

  void PrintBanner() {
    printf( "\nOrigo to C Alpha - Copyright 2020 Orlando Llanes\n" );
  }

  void PrintUsage() {
    printf( "\nusage: origotoc source[.ret] [binary[.ext]]\n" );
  }

  void Cleanup() {
    rstrfree( &programName );

    rstrfree( &curIdent );
    rstrfree( &nextIdent );

    ReleaseTokenVal( &curVal );
    ReleaseTokenVal( &nextVal );

    rstrfree( &retFileName );
    rstrfree( &cFileName );
    rstrfree( &exeFileName );

    CloseRet( &retSource );
    CloseC( &cSourceGen );

    ReleaseSymTab( &symTab );
    ReleaseSymTab( &localTab );
  }

  rstring* RemoveFileExtension( rstring* fileName ) {
    rstring* newStr = NULL;
    size_t extIndex = 0;

    if( fileName && rstrlen(fileName) ) {
      extIndex = rrevscan(fileName, '.');
      if( extIndex == -1 ) {
        // Case 1: Extension not specified
        newStr = rstrcopy(fileName);
      } else {
        // Default: Extension specified
        newStr = rsubstr(fileName, 0, extIndex - 1);
      }
    }
    return newStr;
  };

int main( int argc, char* argv[] ) {
  rstring* arg1 = NULL;
  rstring* arg2 = NULL;
  rstring* tempStr = NULL;
  rstring* tmpFileName = NULL;
  size_t scanIndex = 0;

  argCount = argc;
  argVar = argv;

  if( atexit(Cleanup) ) {
    Error( setExitHandlerFailed, "" );
  }

  PrintBanner();

  /* Parse command line */
  if( (argc < 2) || (argc > 3) ) {
    PrintUsage();
    return 1;
  }

  // Default Retineo source is argv[1]
  arg1 = rstrcopyc(argv[1], strlen(argv[1]));

  // Default Executable dest is argv[1] without a file extension
  if( argc > 2 ) {
    arg2 = rstrcopyc(argv[2], strlen(argv[2]));
  } else {
    arg2 = RemoveFileExtension(arg1);
  }

  /* Initialize file names */
  // Determine Retineo source name
  scanIndex = rrevscan(arg1, '.');
  if( scanIndex == -1 ) {
    // Case 1: Extension not specified
    retFileName = rstrappendc(arg1, ".ret", 4);
  } else if( (scanIndex + 1) == rstrlen(arg1) ) {
    // Case 2: Extension only has the dot
    retFileName = rsubstr(arg1, 0, scanIndex - 1);
  } else {
    // Default: Set Retineo source name to argv[1]
    retFileName = rstrcopy(arg1);
  }

  // Derive C file name
  cFileName = RemoveFileExtension(arg1);
  tempStr = rstrappendc(cFileName, ".rgc", 4);
  if( tempStr == NULL ) {
    rstrfree( &arg1 );
    rstrfree( &arg2 );
    Error( stringOperationFailed, "C file name" );
  }
  cFileName = tempStr;

  // Determine Executable name
  scanIndex = rrevscan(arg2, '.');
  if( scanIndex == -1 ) {
    // Case 1: Extension not specified
    exeFileName = rstrappendc(arg2, ".exe", 4);
  } else if( (scanIndex + 1) == rstrlen(arg2) ) {
    // Case 2: Extension only has the dot
    exeFileName = rsubstr(arg2, 0, scanIndex - 1);
  } else {
    // Default: Set Executable name to argv[2]
    exeFileName = rstrcopy(arg2);
  }

  // Release temporary argv strings
  rstrfree( &arg1 );
  rstrfree( &arg2 );

  /* Open and create files */
  retSource = OpenRet(rstrtext(retFileName), rstrlen(retFileName));
  if( retSource == NULL ) {
    Error( unableToOpen, rstrtext(retFileName) );
  }

  cSourceGen = CreateC(rstrtext(cFileName), rstrlen(cFileName));
  if( cSourceGen == NULL ) {
    Error( unableToCreate, rstrtext(cFileName) );
  }

  /* Parse Retineo source */
  printf( "\nParsing '%s'...", rstrtext(retFileName) );

  GetToken();
  GetToken();

  ParseProgramHeader();
  ParseTopLevel();
  EndParse();

  CloseRet( &retSource );
  CloseC( &cSourceGen );

  printf( "\nDone parsing.\n" );

  /* Compile generated C source */
  printf( "\nBuilding '%s' from '%s'... ",
    rstrtext(exeFileName), rstrtext(cFileName) );

  char* tccArgs[] = {
    " -xc ",
    rstrtext(cFileName),
    " -o ",
    rstrtext(exeFileName),
    NULL
  };
  int tccResult = 0;
  RunProgram( ".\\tcc\\tcc.exe ", tccArgs, &tccResult );
  if( tccResult != 0 ) {
    printf( "\nError building.\n" );
  } else {
    printf( "\nDone building.\n" );
  }

  /* Release resources */
  Cleanup();

  return 0;
}

/*
 *  Token implementation
 */

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

/*
 *  System helper implementation
 */

  #include <process.h>

  bool RunProgram( const char* programName, char* programArgs[], int* exitCode ) {
    int runResult = 0;

    runResult = _spawnv(_P_WAIT, programName, programArgs);

    if( runResult != -1 ) {
      *exitCode = runResult;
    }

    return (runResult != -1);
  }

/*
 *  Error reporting implementation
 */

  // Logic warning reporting
  const char* warningString[] = {
    ""
  };

  const size_t warningCount = sizeof(warningString) / sizeof(warningString[0]);

  void Warning( unsigned line, unsigned column, unsigned warningCode ) {
    if( warningCode && (warningCode < warningCount) ) {
      printf( "  Warning(L%u, C%u): %s\n", line, column, warningString[warningCode] );
    } else if( warningCode ) {
      printf( "  Warning: code %u\n", warningCode );
    }
  }

  // Syntax error reporting
  const char* expectedString[] = {
    "",
    "program",
    "undeclared identifier",
    "type, import, or run",
    "type, or import",
    "run",
    "statement, return, return EXPR, or result expression",
    "statement, inherited statement, bind, return, return EXPR, or result expression",
    "statement or return"
  };

  const size_t expectedCount = sizeof(expectedString) / sizeof(expectedString[0]);

  void Expected( unsigned line, unsigned column, unsigned expectedCode ) {
    if( expectedCode && (expectedCode < expectedCount) ) {
      printf( "  Expected(L%u, C%u): %s\n", line, column, expectedString[expectedCode] );
    } else if( expectedCode ) {
      printf( "  Expected: code %u\n", expectedCode );
    }

    Cleanup();
    exit( expectedCode );
  }

  // Program error reporting
  const char* errorString[] = {
    "",
    "Set exit handler failed",
    "Unable to open",
    "Unable to create",
    "Unable to run",
    "Unable to build",
    "Unable to link",
    "String operation failed"
  };

  const size_t errorCount = sizeof(errorString) / sizeof(errorString[0]);

  void Error( unsigned errorCode, char* errorText ) {
    if( errorCode && (errorCode < errorCount) && errorText ) {
      printf( "  ERROR(%u) %s: %s\n", errorCode, errorString[errorCode], errorText );
    } else if( errorCode ) {
      printf( "  ERROR: code %u\n", errorCode );
    }

    Cleanup();
    exit( errorCode );
  }

/*
 *  String implementation
 */

  inline size_t rstrlen( rstring* source ) {
    return (source ? source->length : 0);
  }

  inline size_t rstrrsvdlen( rstring* source ) {
    return (source ? source->rsvdLength : 0);
  }

  inline char* rstrtext( rstring* source ) {
    return (source ? (((char*)source) + sizeof(rstring)) : NULL);
  }

  rstring* rstralloc( size_t reserveLength ) {
    rstring* newString = NULL;
    size_t   textSize;

    if( reserveLength == 0 ) {
      reserveLength = 8;
    }

    textSize = (reserveLength + 7) & (~7);
    if( reserveLength > textSize ) {
      return NULL;
    }

    newString = malloc(sizeof(rstring) + textSize);
    if( newString ) {
      newString->length = 0;
      newString->rsvdLength = textSize;

      *(((char*)newString) + sizeof(rstring)) = '\0';
    }

    return newString;
  }

  void rstrfree( rstring** rstrPtr ) {
    if( rstrPtr ) {
      if( (*rstrPtr) ) {
        free( (*rstrPtr) );
        (*rstrPtr) = NULL;
      }
    }
  }

  rstring* rstrzalloc( size_t reserveLength ) {
    rstring* newString = NULL;
    size_t   textSize;

    if( reserveLength == 0 ) {
      reserveLength = 8;
    }

    textSize = (reserveLength + 7) & (~7);
    if( reserveLength > textSize ) {
      return NULL;
    }

    newString = calloc(1, sizeof(rstring) + textSize);
    if( newString ) {
      newString->rsvdLength = textSize;
    }

    return newString;
  }

  void rstrclear( rstring* dest ) {
    char* destText = NULL;
    if( dest ) {
      destText = rstrtext(dest);

      dest->length = 0;
      memset( destText, 0, rstrrsvdlen(dest) );
    }
  }

  rstring* rstrcopy( rstring* source ) {
    rstring* newString = NULL;
    size_t sourceLength;
    size_t textSize;

    sourceLength = rstrlen(source);
    textSize = (sourceLength + 8) & (~7);
    if( sourceLength > textSize ) {
      return NULL;
    }

    newString = calloc(1, sizeof(rstring) + textSize);
    if( newString ) {
      newString->length = sourceLength;
      if( source ) {
        strcpy( ((char*)newString) + sizeof(rstring),
              ((char*)source) + sizeof(rstring) );
      }
    }

    if( newString ) {
      newString->rsvdLength = textSize;
    }

    return newString;
  }

  rstring* rstrcopyc( char* source, size_t sourceLength ) {
    rstring* newString = NULL;
    size_t textSize;

    if( source == NULL ) {
      source = "";
    }

    if( sourceLength == 0 ) {
      sourceLength = strlen(source);
    }

    textSize = (sourceLength + 8) & (~7);
    if( sourceLength > textSize ) {
      return NULL;
    }

    if( source ) {
      newString = calloc(1, sizeof(rstring) + textSize);
      if( newString ) {
        newString->length = sourceLength;
        strcpy( ((char*)newString) + sizeof(rstring), source );
      }
    }

    if( newString ) {
      newString->rsvdLength = textSize;
    }

    return newString;
  }

  rstring* rstrappendch( rstring* dest, char ch ) {
    rstring* destTemp;
    char* destText;
    size_t destLength;
    size_t destRsvd;
    size_t destSize;

    destLength = rstrlen(dest);
    destRsvd = rstrrsvdlen(dest);

    if( (destRsvd - destLength) < 2 ) {
      destSize = (destRsvd + 8) & (~7);

      destTemp = realloc(dest, sizeof(rstring) + destSize);
      if( destTemp == NULL ) {
        return NULL;
      }
      dest = destTemp;

      dest->length = destLength;
      dest->rsvdLength = destSize;
    }

    destText = rstrtext(dest);
    if( destText ) {
      destText[destLength + 0] = ch;
      destText[destLength + 1] = '\0';
      dest->length++;
    }

    return dest;
  }

  rstring* rstrappend( rstring* dest, rstring* source ) {
    rstring* destTemp;
    size_t destLength;
    size_t destRsvd;
    size_t destSize;
    size_t sourceLength;
    size_t totalLength;

    if( source == NULL ) {
      return dest;
    }

    destLength = rstrlen(dest);
    destRsvd = rstrrsvdlen(dest);

    sourceLength = rstrlen(source);

    totalLength = destLength + sourceLength;
    destSize = (totalLength + 8) & (~7);
    if( totalLength > destSize ) {
      return NULL;
    }

    if( destSize > destRsvd ) {
      destTemp = realloc(dest, sizeof(rstring) + destSize);
      if( destTemp == NULL ) {
        return NULL;
      }
      dest = destTemp;
      dest->rsvdLength = destSize;
    }

    strcpy( ((char*)dest) + sizeof(rstring) + destLength,
        ((char*)source) + sizeof(rstring) );

    dest->length = totalLength;

    return dest;
  }

  rstring* rstrappendc( rstring* dest, char* source, size_t sourceLength ) {
    rstring* destTemp;
    size_t destLength;
    size_t destRsvd;
    size_t destSize;
    size_t totalLength;

    if( source == NULL ) {
      return dest;
    }

    destLength = rstrlen(dest);
    destRsvd = rstrrsvdlen(dest);

    if( source == NULL ) {
      source = "";
    }

    if( sourceLength == 0 ) {
      sourceLength = strlen(source);
    }

    totalLength = destLength + sourceLength;
    destSize = (totalLength + 8) & (~7);
    if( totalLength > destSize ) {
      return NULL;
    }

    if( destSize > destRsvd ) {
      destTemp = realloc(dest, sizeof(rstring) + destSize);
      if( destTemp == NULL ) {
        return NULL;
      }
      dest = destTemp;
      dest->rsvdLength = destSize;
    }

    strcpy( ((char*)dest) + sizeof(rstring) + destLength, source );

    dest->length = totalLength;

    return dest;
  }

  size_t rscan( rstring* source, char matchCh ) {
    char* sourceCh;
    size_t sourceLength;
    size_t matchIndex;

    sourceCh = rstrtext(source);
    sourceLength = rstrlen(source);

    if( !(sourceCh && sourceLength) ) {
      return (-1);
    }

    for( matchIndex = 0; matchIndex < sourceLength; matchIndex++ ) {
      if( *sourceCh == '\0' ) {
        break;
      }

      if( *sourceCh == matchCh ) {
        return matchIndex;
      }
      sourceCh++;
    }

    return (-1);
  }

  size_t rscanc( char* source, char matchCh ) {
    char* sourceCh;
    size_t matchIndex;

    sourceCh = source;

    if( sourceCh == NULL ) {
      return (-1);
    }

    matchIndex = 0;
    while( *sourceCh ) {
      if( *sourceCh == matchCh ) {
        return matchIndex;
      }
      sourceCh++;
      matchIndex++;
    }

    return (-1);
  }

  size_t rrevscan( rstring* source, char matchCh ) {
    char* sourceCh;
    size_t sourceLength;
    size_t matchIndex;

    sourceLength = rstrlen(source);
    if( sourceLength == 0 ) {
      return (-1);
    }
    matchIndex = sourceLength - 1;

    sourceCh = rstrtext(source) + matchIndex;

    if( source == NULL ) {
      return (-1);
    }

    while( sourceLength && (*sourceCh) ) {
      if( (*sourceCh) == matchCh ) {
        return matchIndex;
      }

      sourceCh--;
      sourceLength--;
      matchIndex--;
    }

    return (-1);
  }

  size_t rrevscanc( char* source, char matchCh ) {
    char* sourceCh;
    size_t sourceIndex;
    size_t matchIndex = (-1);

    if( source == NULL ) {
      return (-1);
    }

    sourceCh = source;

    sourceIndex = 0;
    while( (*sourceCh) ) {
      if( (*sourceCh) == matchCh ) {
        matchIndex = sourceIndex;
      }

      sourceIndex++;
      sourceCh++;
    }

    return matchIndex;
  }

  rstring* rsubstr( rstring* source, size_t startPos, size_t endPos ) {
    rstring* newStr;
    char* destText;
    char* sourceText;
    size_t sourceLength;
    size_t subLength;

    if( source == NULL ) {
      return NULL;
    }

    sourceLength = rstrlen(source);
    sourceText = rstrtext(source);

    subLength = (endPos - startPos) + 1;
    if( subLength >= sourceLength ) {
      return NULL;
    }

    newStr = rstralloc(subLength);
    if( newStr == NULL ) {
      return NULL;
    }
    newStr->length = subLength;

    sourceText = rstrtext(source);
    destText = rstrtext(newStr);
    memcpy( destText, (sourceText + startPos), subLength );
    destText[subLength] = '\0';

    return newStr;
  }

  rstring* rsubstrc( char* source, size_t sourceLength,
      size_t startPos, size_t endPos ) {
    rstring* newStr;
    char* destText;
    size_t subLength;

    if( source == NULL ) {
      return NULL;
    }

    if( sourceLength == 0 ) {
      sourceLength = strlen(source);
    }

    subLength = (endPos - startPos) + 1;
    if( subLength >= sourceLength ) {
      return NULL;
    }

    newStr = rstralloc(subLength);
    if( newStr == NULL ) {
      return NULL;
    }
    newStr->length = subLength;

    destText = rstrtext(newStr);
    memcpy( destText, (source + startPos), subLength );
    destText[subLength] = '\0';

    return newStr;
  }

/*
 *  Token value implementation
 */

  void ReleaseTokenVal( TokenVal* tokenVal ) {
    if( tokenVal ) {
      switch( tokenVal->valType ) {
      case typeString:
        rstrfree( &(tokenVal->strVal) );
        break;

      case typeStruct:
      case typeUnion:
        free( &(tokenVal->dataVal) );
        tokenVal->dataVal = NULL;
        break;
      }

      memset( tokenVal, 0, sizeof(TokenVal) );
    }
  }

/*
 *  Type specifier implementation
 */

  void ReleaseTypeSpec( TypeSpec* typeSpec ) {
    if( typeSpec ) {
      rstrfree( &(typeSpec->baseTypeName) );

      if( typeSpec->dimension ) {
        free( typeSpec->dimension );
        typeSpec->dimension = NULL;
      }

      memset( typeSpec, 0, sizeof(TypeSpec) );
    }
  }

/*
 *  Symbol table implementation
 */

  void CopySymItem( SymItem* dest, SymItem* source ) {
    ///TODO: Allocate and copy dynamic data
  }

  void FreeSymItem( SymItem* symItem ) {
    if( symItem ) {
      switch( symItem->token ) {
      case symNone:
        return;

      case symBaseType:
        return;

      case symType:
        if( symItem->typeSym.typeSpec.dimension ) {
          free( symItem->typeSym.typeSpec.dimension );
          symItem->typeSym.typeSpec.dimension = NULL;
        }
        return;

      case symFuncImport:
        return;
      }
    }
  }

  bool DeclareType( SymTable* symTab, char* definedType, char* typeName, TokenVal* initVal ) {
    SymItem typeItem = {};

    return false;
  }

/*
 *  Lexer implementation
 */

  RetFile* OpenRet( char* fileName, size_t nameLength ) {
    RetFile* newSource = NULL;

    if( !(fileName && (*fileName)) ) {
      goto ReturnError;
    }

    newSource = calloc(1, sizeof(RetFile));
    if( newSource == NULL ) {
      goto ReturnError;
    }

    newSource->fileName = rstrcopyc(fileName, nameLength);
    if( newSource->fileName == NULL ) {
      goto ReturnError;
    }

    newSource->handle = fopen(fileName, "rb");
    if( newSource->handle == NULL ) {
      goto ReturnError;
    }

    ReadChar( newSource );
    ReadChar( newSource );

    newSource->line = 1;
    newSource->column = 1;

    return newSource;

  ReturnError:
    CloseRet( &newSource );

    return NULL;
  }

  void CloseRet( RetFile** source ) {
    if( source ) {
      if( (*source) ) {
        if( (*source)->handle ) {
          fclose( (*source)->handle );
          (*source)->handle = NULL;
        }

        if( (*source)->fileName ) {
          free( (*source)->fileName );
          (*source)->fileName = NULL;
        }

        free( (*source) );
        (*source) = NULL;
      }
    }
  }

  bool ReadChar( RetFile* source ) {
    unsigned columnInc = 1;
    char tmpCh = 0;

    if( !(source && source->handle) ) {
      return false;
    }

    if( source->curCh == 10 ) {
      source->line++;
      source->column = 1;
      columnInc = 0;
    }

    source->curCh = source->nextCh;
    source->nextCh = 0;

    if( fread(&(source->nextCh), 1, sizeof(char),
        source->handle) != sizeof(char) ) {
      return false;
    }

    if( source->nextCh == 13 ) {
      if( fread(&tmpCh, 1, sizeof(char),
          source->handle) != sizeof(char) ) {
        return false;
      }

      if( tmpCh != 10 ) {
        if( fseek(source->handle, -1, SEEK_CUR) == 0 ) {
          return false;
        }
      }

      source->nextCh = 10;
    }

    source->column += columnInc;

    return true;
  }

  bool SkipSpace( RetFile* source ) {
    if( source == NULL ) {
      return false;
    }

    if( isspace(source->curCh) ) {
      while( isspace(source->curCh) ) {
        ReadChar( source );
      }
      return true;
    }

    return false;
  }

  bool SkipComments( RetFile* source ) {
    unsigned level;
    unsigned loopFlags;

    if( source == NULL ) {
      return false;
    }

    do {
      loopFlags = 0;

      // Skip space characters
      if( isspace(source->curCh) ) {
        while( isspace(source->curCh) ) {
          ReadChar( source );
        }
        loopFlags |= 1;
      }

      // Skip single-line comments
      if( (source->curCh == '/') && (source->nextCh == '/') ) {
        do {
          if( !ReadChar(source) ) {
            return false;
          }
        } while( source->curCh != 10 );
        loopFlags |= (1 << 1);
      }

      // Skip multi-line comments
      if( (source->curCh == '/') && (source->nextCh == '*') ) {
        level = 0;
        do {
          if( (source->curCh == '/') && (source->nextCh == '*') ) {
            if( level == (-1) ) {
              return false;
            }
            level++;
            if( !(ReadChar(source) && ReadChar(source)) ) {
              return false;
            }
            continue;
          }

          if( (source->curCh == '*') && (source->nextCh == '/') ) {
            if( level == 0 ) {
              return false;
            }
            level--;
            if( !(ReadChar(source) && ReadChar(source)) ) {
              return false;
            }
            continue;
          }

          if( !ReadChar(source) ) {
            return false;
          }
        } while( level );

        loopFlags |= (1 << 2);
      }
    } while( loopFlags );

    return true;
  }

  bool ReadIdent( RetFile* source, rstring** ident ) {
    rstring* dest;
    rstring* tmpDest;

    if( !(source && ident) ) {
      return false;
    }

    dest = (*ident);

    if( !((source->curCh == '_') || isalnum(source->curCh)) ) {
      return false;
    }

    while( (source->curCh == '_') || isalnum(source->curCh) ) {
      tmpDest = rstrappendch(dest, source->curCh);
      if( tmpDest == NULL ) {
        (*ident) = dest;
        return false;
      }
      dest = tmpDest;

      ReadChar( source );
    }

    (*ident) = dest;
    return true;
  }

  bool ReadBinNum( RetFile* source, unsigned* num ) {
    unsigned result = 0;

    if( !(source && num) ) {
      return false;
    }

    if( (source->curCh != '0') && (source->nextCh != 'b') ) {
      return false;
    }

    if( !(ReadChar(source) && ReadChar(source)) ) {
      return false;
    }

    while( source->curCh ) {
      if( (source->curCh >= '0') && (source->curCh <= '1') ) {
        result = (result << 1) + (source->curCh - '0');
      } else if( source->curCh == '_' ) {
      } else {
        break;
      }

      ReadChar( source );
    }

    *num = result;
    return true;
  }

  bool ReadOctalNum( RetFile* source, unsigned* num ) {
    unsigned result = 0;

    if( !(source && num) ) {
      return false;
    }

    if( (source->curCh != '0') && (source->nextCh != 'o') ) {
      return false;
    }

    if( !(ReadChar(source) && ReadChar(source)) ) {
      return false;
    }

    while( source->curCh ) {
      if( (source->curCh >= '0') && (source->curCh <= '7') ) {
        result = (result << 3) + (source->curCh - '0');
      } else if( source->curCh == '_' ) {
      } else {
        break;
      }

      ReadChar( source );
    }

    *num = result;
    return true;
  }

  bool ReadHexNum( RetFile* source, unsigned* num ) {
    unsigned result = 0;

    if( !(source && num) ) {
      return false;
    }

    if( (source->curCh != '0') && (source->nextCh != 'x') ) {
      return false;
    }

    if( !(ReadChar(source) && ReadChar(source)) ) {
      return false;
    }

    while( source->curCh ) {
      if( (source->curCh >= '0') && (source->curCh <= '9') ) {
        result = (result << 4) + (source->curCh - '0');
      } else if( (source->curCh >= 'a') && (source->curCh <= 'f') ) {
        result = (result << 4) + (source->curCh - 'a') + 10;
      } else if( (source->curCh >= 'A') && (source->curCh <= 'F') ) {
        result = (result << 4) + (source->curCh - 'A') + 10;
      } else if( source->curCh == '_' ) {
      } else {
        break;
      }

      ReadChar( source );
    }

    *num = result;
    return true;
  }

  bool ReadNum( RetFile* source, unsigned* num ) {
    unsigned result = 0;

    if( !(source && num) ) {
      return false;
    }

    // Check for other types of integers
    if( source->curCh == '0' ) {
      switch( source->nextCh ) {
      case 'b':
        return ReadBinNum(source, num);

      case 'o':
        return ReadOctalNum(source, num);

      case 'x':
        return ReadHexNum(source, num);
      }
    }

    // Skip leading zero's
    while( source->curCh == '0' ) {
      ReadChar( source );
    }

    // Tokenize decimal
    while( (source->curCh == '_') || isdigit(source->curCh) ) {
      if( source->curCh != '_' ) {
        result = (result * 10) + ((source->curCh) - '0');
      }
      ReadChar( source );
    }

    *num = result;
    return true;
  }

  bool ReadString( RetFile* source, rstring** destString ) {
    rstring* dest = NULL;
    rstring* tmpDest = NULL;
    char quoteCh;

    if( !(source && destString) ) {
      return false;
    }

    dest = (*destString);

    quoteCh = source->curCh;
    if( !((quoteCh == '\"') || (quoteCh == '\'')) ) {
      return false;
    }

    if( !ReadChar(source) ) {
      return false;
    }

    while( source->curCh != quoteCh ) {
      tmpDest = rstrappendch(dest, source->curCh);
      if( tmpDest == NULL ) {
        (*destString) = dest;
        return false;
      }
      dest = tmpDest;

      if( !ReadChar(source) ) {
        (*destString) = dest;
        return false;
      }
    }

    // Skip closing quote character
    if( !ReadChar(source) ) {
      (*destString) = dest;
      return false;
    }

    (*destString) = dest;
    return true;
  }

  typedef struct Operator {
    char* text;
    unsigned token;
  } Operator;

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

  unsigned ReadOperator( RetFile* source ) {
    char operator[8] = {};
    size_t leftIndex;
    size_t operIndex;
    size_t rightIndex;
    size_t operLength = 0;
    int    compareCode;
    unsigned token = 0;

    if( !(source && ispunct(source->curCh)) ) {
      return 0;
    }

    while( ispunct(source->curCh) ) {
      if( operLength > 7 ) {
        break;
      }

      operator[operLength++] = source->curCh;

      leftIndex = 0;
      rightIndex = operCount;
      operIndex = operCount / 2;

      while( leftIndex < rightIndex ) {
        compareCode = strcmp(operTable[operIndex].text, operator);
        if( compareCode == 0 ) {
          token = operTable[operIndex].token;
          break;
        }

        if( compareCode > 0 ) {
          rightIndex = operIndex;
        } else {
          leftIndex = operIndex + 1;
        }

        operIndex = (leftIndex + rightIndex) / 2;
      }

      if( leftIndex > rightIndex ) {
        return token;
      }

      if( !ReadChar(source) ) {
        return token;
      }
    }

    return token;
  }

  unsigned GetToken() {
    rstring* tmpIdent = NULL;
    char nextCh = '\0';

    if( retSource == NULL ) {
      return 0;
    }

    /* Swap current and next tokens/values/etc */
    // Identifiers
    rstrclear( curIdent );
    tmpIdent = curIdent;
    curIdent = nextIdent;
    nextIdent = tmpIdent;

    // Token values
    ReleaseTokenVal( &curVal );
    curVal = nextVal;
    ReleaseTokenVal( &nextVal );

    // Character position
    curLine = nextLine;
    curColumn = nextColumn;

    // Token codes
    curToken = nextToken;
    nextToken = 0;

    /* Skip comments/space and capture line/column */
    SkipComments( retSource );

    nextLine = retSource->line;
    nextColumn = retSource->column;

    /* Determine token type to read */
    nextCh = retSource->nextCh;

    if( (nextCh == '_') || isalnum(nextCh) ) {
      if( ReadIdent(retSource, &nextIdent) ) {
        nextToken = tkIdent;
      }
      return curToken;
    }

    if( isdigit(nextCh) ) {
      if( ReadNum(retSource, &nextVal.uVal) ) {
        nextToken = valUint;
      }
      return curToken;
    }

    if( (nextCh == '"') || (nextCh == '\'') ) {
      if( ReadString(retSource, &nextVal.strVal) ) {
        nextToken = valString;
      }
      return curToken;
    }

    if( ispunct(nextCh) ) {
      nextToken = ReadOperator(retSource);
    }

    return curToken;
  }

  unsigned TranslateKeyword( unsigned originalToken ) {
    unsigned keywordToken;

    if( originalToken == tkIdent ) {
      keywordToken = FindKeyword(rstrtext(curIdent));
      if( keywordToken ) {
        return keywordToken;
      }
    }

    return originalToken;
  }

/*
 *  Expression parser implementation
 */

/*
 *  C generation declarations
 */

  CGen* CreateC( char* fileName, size_t nameLength ) {
    CGen* newGen = NULL;
    rstring* tempStr = NULL;
    char* tempStrText;
    size_t tempStrLen;
    size_t strIndex;

    if( !(fileName && (*fileName)) ) {
      goto ReturnError;
    }

    newGen = calloc(1, sizeof(CGen));
    if( newGen == NULL ) {
      goto ReturnError;
    }

    // Set the C file name
    newGen->cFileName = rstrcopyc(fileName, nameLength);
    if( newGen->cFileName == NULL ) {
      goto ReturnError;
    }

    // Derive C include name
    newGen->cIncludeName = RemoveFileExtension(newGen->cFileName);
    tempStr = rstrappendc(newGen->cIncludeName, ".rgh", 2);
    if( tempStr == NULL ) {
      goto ReturnError;
    }
    newGen->cIncludeName = tempStr;

    // Create then initialize C source file
    newGen->cHandle = fopen(rstrtext(newGen->cFileName), "wb");
    if( newGen->cHandle == NULL ) {
      goto ReturnError;
    }

    // Create then initialize C header file
    newGen->cIncludeHandle = fopen(rstrtext(newGen->cIncludeName), "wb");
    if( newGen->cIncludeHandle == NULL ) {
      goto ReturnError;
    }

    tempStr = RemoveFileExtension(newGen->cFileName);
    tempStrText = rstrtext(tempStr);
    tempStrLen = rstrlen(tempStr);
    for( strIndex = 0; strIndex < tempStrLen; strIndex++ ) {
      tempStrText[strIndex] = toupper(tempStrText[strIndex]);
    }
    fprintf( newGen->cIncludeHandle, "#ifndef %s_H\n", rstrtext(tempStr) );
    fprintf( newGen->cIncludeHandle, "#define %s_H\n\n", rstrtext(tempStr) );
    rstrfree( &tempStr );

    return newGen;

  ReturnError:
    CloseC( &newGen );
    return NULL;
  }

  void CloseC( CGen** source ) {
    if( source ) {
      if( (*source) ) {
        if( (*source)->cHandle ) {
          fclose( (*source)->cHandle );
          (*source)->cHandle = NULL;
        }

        if( (*source)->cIncludeHandle ) {
          fclose( (*source)->cIncludeHandle );
          (*source)->cIncludeHandle = NULL;
        }

        if( (*source)->cFileName ) {
          rstrfree( &((*source)->cFileName) );
        }

        if( (*source)->cIncludeName ) {
          rstrfree( &((*source)->cIncludeName) );
        }

        free( (*source) );
        (*source) = NULL;
      }
    }
  }

  bool GenTypeSpec( CGen* csource, TypeSpec* typeSpec ) {
    return false;
  }

/*
 *  Parser implementation
 */

  void ParseProgramHeader() {
    if( !(retSource && cSourceGen) ) {
      return; ///TODO: Change return to Error
    }

    /* Validate program header */
    curToken = TranslateKeyword(curToken);
    if( curToken != rsvdProgram ) {
      Expected( curLine, curColumn, expectedProgram );
    }
    curToken = TranslateKeyword(GetToken()); // Skip program

    if( curToken != tkIdent ) {
      Expected( curLine, curColumn, expectedIdentifier );
    }
    programName = rstrcopy(curIdent);
    GetToken(); // Skip IDENT

    fprintf( cSourceGen->cHandle, "\n/* program: %s */\n\n",
      rstrtext(programName) );
    fprintf( cSourceGen->cHandle, "#include \"%s\"\n",
      rstrtext(cSourceGen->cIncludeName) );

    symTab = CreateSymTab(0);
    if( symTab == NULL ) {
      Error( unableToCreate, "Symbol Table" );
    }
  }

  void EndParse() {
    if( cSourceGen && cSourceGen->cIncludeHandle ) {
      fprintf( cSourceGen->cIncludeHandle, "\n#endif\n" );
    }
  }

  void ParseTypeSpec( TypeSpec* destSpec ) {
  }

  void ParseType() {
  }

  void ParseImport() {
  }

  void ParseLocalVar() {
    TypeSpec typeSpec = {};
    SymItem symItem = {};
    unsigned typeToken;
    unsigned rsvdToken;

    do {
      rsvdToken = TranslateKeyword(GetToken()); // Skip var
      if( rsvdToken == rsvdEnd ) {
        ParseLocalVarEnd();
        break;
      }

      typeToken = RetrieveSymbol(symTab, rstrtext(curIdent), &symItem);

      ReleaseTypeSpec( &typeSpec );
      ParseTypeSpec( &typeSpec );
    } while( curToken );
  }

  void ParseLocalVarEnd() {
  }

  void ParseIf() {
    ///TODO: IfStack usage [draft]

    //  if COND
    //    currentIf = nextIf++
    //  then STATEMENT
    //  [".nextif", hex(block), hex(nextIf), ":"]
    //  [".endif", hex(block), hex(endIf), ":"]

    //  if COND
    //    currentIf = nextIf++
    //    ...
    //    jmp endIf
    //
    //  [".nextif", hex(block), hex(currentIf), ":"]
    //  elseif COND
    //    currentIf = nextIf++
    //    ...
    //    jmp endIf
    //
    //  [".nextif", hex(block), hex(currentIf), ":"]
    //  else
    //    ...
    //  [".endif", hex(block), hex(endIf), ":"]
    //  endif
  }

  void ParseFor() {
  }

  void ParseStatement() {
  }

  void ParseRun() {
    unsigned varToken;

    if( !(retSource && cSourceGen) ) {
      return; ///TODO: Replace with error
    }

    if( localTab == NULL ) {
      localTab = CreateSymTab(0);
      if( localTab == NULL ) {
        ///TODO: Error creating localTab
      }
    }

    fprintf( cSourceGen->cHandle,
      "\nint main( int argc, char* argv[] ) {\n" );

    do {
      varToken = TranslateKeyword(curToken);
      if( varToken == rsvdVar ) {
        curToken = varToken;
        ParseLocalVar();
        continue;
      }
      break;
    } while( varToken );

    // Parse run statement
    do {
      curToken = TranslateKeyword(curToken);

      if( curToken == rsvdEnd ) {
        ///TODO: Show error if nested statement is still open

        GetToken(); // Skip end

        ParseRunEnd();
        break;
      }
    } while( curToken );
  }

  void ParseRunEnd() {
    if( cSourceGen && cSourceGen->cHandle ) {
      fprintf( cSourceGen->cHandle, "\n  return 0;\n}\n" );

      ReleaseSymTab( &localTab );
    }
  }

  void ParseTopLevel() {
    unsigned expectedCode = expectedTopLevelOrRun;

    if( retSource && cSourceGen ) {
      while( curToken ) {
        curToken = TranslateKeyword(curToken);

        /* type */
        if( curToken == rsvdType ) {
          ParseType();
          continue;
        }

        /* import */
        if( curToken == rsvdImport ) {
          ParseImport();
          continue;
        }

        /* run */
        if( curToken == rsvdRun ) {
          // Check if run was previously declared
          if( cSourceGen->runDeclared ) {
            Expected( curLine, curColumn, expectedCode );
          }
          cSourceGen->runDeclared = true;
          expectedCode = expectedTopLevel;

          GetToken(); // Skip run

          ParseRun();
          continue;
        }

        if( curToken ) {
          Expected( curLine, curColumn, expectedCode );
        }
      };
    }

    if( cSourceGen->runDeclared == false ) {
      Expected( curLine, curColumn, expectedRun );
    }
  }
