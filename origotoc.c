	
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Declarations
 */

  // Enum names with redundant values for easy program exit code cross-reference
  typedef enum ErrorCodes {
    errorInvalidCommandline = 1,
    errorSLCommentClosedWithEOF = 2,
    errorMLCommentNestedTooDeep = 3,
    errorMLCommentClosedWithEOF = 4,
    errorMLCommentClosedBeforeOpening = 5,
    errorOpeningRetFile = 6,
    errorCreatingCFile = 7,
    errorCreatingHeaderFile = 8,
    errorReadingIdentifier = 9,
    expectedKeyword = 10,
    expectedUndeclaredIdentifier = 11,
    expectedRun = 12,
    expectedRunOrTopLevel = 13,
    expectedTopLevel = 14,
    runAlreadyDeclared = 15,
    errorCFileNotOpen = 16,
    errorHeaderFileNotOpen = 17,
    expectedEndOrStatement = 18,
    errorRetFileNotOpen = 19,
    expectedLocalVarDecl = 20,
    errorBuildingIntermediateSource = 21,
    errorInTypeSpecification = 22,
    errorPendingImplementation = 23,
    expectedLeftParenthesis = 24,
    expectedRightParenthesis = 25,
    expectedSymbol = 26,
    errorSimpleTypeNotSpecified = 27,
    expectedArrayDimension = 28,
    errorUnsupportedSyntax = 29,
    expectedLeftBrace = 30,
    expectedRightBrace = 31,
    expectedDeclaredIdentifier = 32,
    expectedStatement = 33,
    errorParenNestedTooDeep = 34,
    expectedValidOperator = 35,
    expectedOperand = 36,
    errorUnsupportedArrayIndex = 37,
    expectedUnaryOperatorOrOperand = 38,
    errorUnsupportedOperator = 39,
    expectedVariable = 40,
  } ErrorCodes;

  typedef struct Keyword {
    char*    name;
    unsigned token;
  } Keyword;

  typedef struct Operator {
    char* text;
    unsigned token;
  } Operator;

  typedef struct TypeSpec {
    unsigned ptrType;
    unsigned baseType;
    char baseName[1024];
    size_t indexCount;
  } TypeSpec;

  typedef struct TokenVal {
    unsigned valType;

    union {
      unsigned valUint;
      int valInt;
      char valChar;
    };
  } TokenVal;

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

  void ParseTypeSpec( TypeSpec* destSpec );

  void BeginParse();
  void ParseProgramHeader();
  void ParseConstDecl();
  void ParseEnumDecl();
  void ParseStructDecl();
  void ParseUnionDecl();
  void ParseTypeDecl();
  void ParseGlobalVarBlock();
  void ParseImportDecl();
  void ParseFuncDecl();
  void ParseFuncCall();
  void ParseMethodCall();
  void ParseBind();
  void ParseVarRef();
  void ParseCondition();
  void ParseIf();
  void ParseFuncThen();
  void ParseMethodThen();
  void ParseThen();
  void ParseElseIf();
  void ParseElse();
  void ParseEndIf();
  void ParseForIn();
  void ParseForToDownTo();
  void ParseFor();
  void ParseEndFor();
  void ParseEcho();
  void ParseEchoLn();
  void ParseRepeat();
  void ParseWhen();
  void ParseWhile();
  void ParseEndWhile();
  void ParseLabelDecl();
  void ParseLocalVarBlock();
  void ParseGoto();
  void ParseExit();
  int ParseFuncStatement();
  int ParseMethodStatement();
  int ParseStatement();
  void ParseFunc();
  void ParseObjectDecl();
  void ParseAbstractDecl();
  void ParseInterfaceDecl();
  void ParseMethod();
  void ParseOperatorFunc();
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

    // Reserved high level keyword tokens
    rsvdIdent = (1 << 9),
      rsvdProgram,
        firstRsvd = rsvdProgram,
      rsvdConst,
      rsvdEnum,
      rsvdType,
      rsvdVar,
      rsvdFrom,
      rsvdImport,
      rsvdFuncDecl,
      rsvdResult,
      rsvdReturn,
      rsvdFunc,
      rsvdObject,
      rsvdInherits,
      rsvdMutable,
      rsvdImmutable,
      rsvdAbstract,
      rsvdImplements,
      rsvdInterface,
      rsvdSelf,
      rsvdMethod,
      rsvdOperator,
      rsvdRun,
      rsvdEnd,
      rsvdBind,
      rsvdIf,
      rsvdThen,
      rsvdElseIf,
      rsvdElse,
      rsvdEndIf,
      rsvdIn,
      rsvdTo,
      rsvdDownTo,
      rsvdNext,
      rsvdBreak,
      rsvdFor,
      rsvdEndFor,
      rsvdRepeat,
      rsvdWhen,
      rsvdWhile,
      rsvdEndWhile,
      rsvdEcho,
      rsvdEchoLn,
      rsvdGoto,
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
      typeFunc,
      typeObject,
      typeMethod,
        lastType = typeMethod,

    // Variable tokens
    varDecl = (3 << 9),
      varInt,
        firstVar = varInt,
      varInt8,
      varInt16,
      varInt32,
      varUint,
      varUint8,
      varUint16,
      varUint32,
      varChar,
      varString,
      varBool,
      varStruct,
      varUnion,
      varFunc,
      varObject,
      varMethod,
        lastVar = varMethod,

    // Pointer tokens
    ptrDecl = (4 << 9),
      ptrData,
        firstPtr = ptrData,
      ptrRef,
        lastPtr = ptrRef,

    // Literal value tokens
    valImmediate = (5 << 9),
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
    operSymbol = (6 << 9),
      operPrec00 = (operSymbol + (0 << 5)),
        firstOper = operPrec00,
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
        lastOper = operPrec15,

    // Assignment operators
    assignSymbol  = (7 << 9),
      assignTo,
        firstAssign = assignTo,
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
      assignOr,
        lastAssign = assignOr
  } Token;

  const Keyword keywordTable[] = {
    "abstract", rsvdAbstract,
    "bind",    rsvdBind,
    "bool",    typeBool,
    "break",   rsvdBreak,
    "char",    typeChar,
    "const",   rsvdConst,
    "downto",  rsvdDownTo,
    "echo",    rsvdEcho,
    "echoln",  rsvdEchoLn,
    "else",    rsvdElse,
    "elseif",  rsvdElseIf,
    "end",     rsvdEnd,
    "endfor",  rsvdEndFor,
    "endif",   rsvdEndIf,
    "endwhile", rsvdEndWhile,
    "enum",    rsvdEnum,
    "exit",    rsvdExit,
    "for",     rsvdFor,
    "from",    rsvdFrom,
    "func",    rsvdFunc,
    "funcdecl", rsvdFuncDecl,
    "goto",    rsvdGoto,
    "if",      rsvdIf,
    "immutable", rsvdImmutable,
    "implements", rsvdImplements,
    "import",  rsvdImport,
    "in",      rsvdIn,
    "inherits", rsvdInherits,
    "int",     typeInt,
    "int16",   typeInt16,
    "int32",   typeInt32,
    "int8",    typeInt8,
    "interface", rsvdInterface,
    "method",  rsvdMethod,
    "mutable", rsvdMutable,
    "next",    rsvdNext,
    "object",  rsvdObject,
    "operator", rsvdOperator,
    "program", rsvdProgram,
    "repeat",  rsvdRepeat,
    "result",  rsvdResult,
    "return",  rsvdReturn,
    "run",     rsvdRun,
    "self",    rsvdSelf,
    "string",  typeString,
    "struct",  typeStruct,
    "then",    rsvdThen,
    "to",      rsvdTo,
    "type",    rsvdType,
    "uint",    typeUint,
    "uint16",  typeUint16,
    "uint32",  typeUint32,
    "uint8",   typeUint8,
    "union",   typeUnion,
    "var",     rsvdVar,
    "when",    rsvdWhen,
    "while",   rsvdWhile
  };
  const size_t keywordCount = sizeof(keywordTable) / sizeof(keywordTable[0]);

  const Operator operTable[] = {
    "!",    unaryIsNot,
    "!=",   opNotEq,
    "#",    ptrRef,
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
    "<-<",  opSRol,
    "<-<=", assignSRol,
    "<<",   opShl,
    "<<<",  opRol,
    "<<<=", assignRol,
    "<<=",  assignShl,
    "<=",   opLTEq,
    "=",    assignTo,
    "==",   opEq,
    ">",    opGT,
    ">->",  opSRor,
    ">->=", assignSRor,
    ">=",   opGTEq,
    ">>",   opShr,
    ">>=",  assignShr,
    ">>>",  opRor,
    ">>>=", assignRor,
    "@",    ptrData,
    "[",    tkLBrace,
    "[[",   tkLDoubleBrace,
    "]",    tkRBrace,
    "]]",   tkRDoubleBrace,
    "^",    opXor,
    "^=",   assignXor,
    "|",    opOr,
    "|=",   assignOr,
    "|>>",  opSShr,
    "|>>=", assignSShr,
    "||",   opOrIs,
    "~",    unaryNot,
    "~=",   assignNot
  };
  const size_t operCount = sizeof(operTable) / sizeof(operTable[0]);

  char programName[1024] = {};
  int runDeclared = 0;

  unsigned line = 1;
  unsigned column = 1;

  unsigned curLine = 1;
  unsigned curColumn = 1;
  unsigned curToken = 0;
  char curTokenStr[1024] = {};
  TokenVal curTokenVal = {};

  unsigned nextLine = 1;
  unsigned nextColumn = 1;
  unsigned nextToken = 0;
  char curCh = 0;
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

      column++;
      if( curCh == '\n' ) {
        line++;

        column = 1;
        if( nextCh ) {
          column--;
        }
      }
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
            printf( "[L%u,C%u] Single-line comments must end with CR and/or LF\n", line, column );
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
                printf( "[L%u,C%u] Multi-line comment is nested too many levels deep\n", line, column );
                exit(errorMLCommentNestedTooDeep);
              }
              commentLevel++;
              if( !(ReadChar() && ReadChar()) ) {
                printf( "[L%u,C%u] Unexpected EOF in multi-line comment opened with /*\n", line, column );
                exit(errorMLCommentClosedWithEOF);
              }
              continue;
            }

            if( (curCh == '*') && (nextCh == '/') ) {
              if( commentLevel == 0 ) {
                printf( "[L%u,C%u] Each multi-line comment must be opened with /* before closing with */\n", line, column );
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
    char operator[8] = {};
    size_t leftIndex;
    size_t operIndex;
    size_t rightIndex;
    size_t operLength = 0;
    int    compareCode;
    unsigned token = 0;

    if( !(retFile && ispunct(curCh)) ) {
      return 0;
    }

    do {
      if( operLength > 5 ) {
        break;
      }

      operator[operLength++] = curCh;

      leftIndex = 0;
      rightIndex = operCount;
      operIndex = operCount / 2;

      while( leftIndex < rightIndex ) {
        compareCode = strcmp(operTable[operIndex].text, operator);
        if( compareCode == 0 ) {
          token = operTable[operIndex].token;
          if( ReadChar() == 0 ) {
            return token;
          }
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
    } while( ispunct(curCh) && (compareCode == 0) );

    return token;
  }

  unsigned GetToken() {
    // Set current variables
    curLine = nextLine;
    curColumn = nextColumn;
    curToken = nextToken;
    memcpy( curTokenStr, nextTokenStr, sizeof(nextTokenStr) );
    memcpy( &curTokenVal, &nextTokenVal, sizeof(nextTokenVal) );

    // Initialize next variables
    nextToken = 0;
    memset( nextTokenStr, 0, sizeof(nextTokenStr) );
    memset( &nextTokenVal, 0, sizeof(nextTokenVal) );

    // Prepare to read next token
    SkipSpaceAndComments();

    nextLine = line;
    nextColumn = column;

    // Determine next token type, then read it
    if( (curCh == '_') || isalpha(curCh) ) {
      if( ReadIdent(nextTokenStr, sizeof(nextTokenStr)) ) {
        nextToken = tkIdent;
      }
      return curToken;
    }

    if( isdigit(curCh) ) {
      if( ReadNum(&nextTokenVal.valUint) ) {
        nextTokenVal.valType = valUint;
        nextToken = valUint;
      }
      return curToken;
    }

    if( ispunct(curCh) ) {
      nextToken = ReadOperator();
    }

    if( (curCh == '"') || (curCh == '\'') ) {
      if( ReadString(nextTokenStr, sizeof(nextTokenStr)) ) {
        nextTokenVal.valType = valString;
        nextToken = valString;
      }
      return curToken;
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

  void ParseTypeSpec( TypeSpec* destSpec ) {
    TypeSpec tempSpec = {};
    unsigned keywordToken;

    if( retFile == NULL ) {
      printf( "Error source file not open\n" );
      exit( errorRetFileNotOpen );
    }

    if( destSpec == NULL ) {
      return;
    }

    // Set pointer type if specified
    if( (curToken == ptrData) || (curToken == ptrRef) ) {
      tempSpec.ptrType = curToken;
      GetToken(); // Skip # or *
    }

    // Set base type if specified
    keywordToken = FindKeyword(curTokenStr);
    if( (keywordToken >= firstType) && (keywordToken <= lastType) ) {
      tempSpec.baseType = curToken;
      memcpy( tempSpec.baseName, curTokenStr, sizeof(curTokenStr) );
      GetToken(); // Skip base type
    }

if( curToken != tkIdent ) { // Temporary line until token table is implemented
    // Pointer type and/or base type are required
    if( (tempSpec.ptrType | tempSpec.baseType) == 0 ) {
      printf( "[L%u,C%u] Pointer type and/or base type must be specified.\n", curLine, curColumn );
      exit( errorSimpleTypeNotSpecified );
    }
} // Temporary line until token table is implemented

    // Process array dimension
    if( curToken == tkLBrace ) {
      GetToken(); // Skip opening array bracket [

      switch( curToken ) {
      case valUint:
        tempSpec.indexCount = curTokenVal.valUint;
        GetToken(); // Skip uint value
        break;

      case tkRBrace:
        // Array dimension required for non-pointers
        if( tempSpec.ptrType == 0 ) {
          printf( "[L%u,C%u] Array dimension must be specified for non-pointers\n", curLine, curColumn );
          exit( expectedArrayDimension );
        }
        break;

      default:
        printf( "[L%u,C%u] Unsupported array dimension specified\n", curLine, curColumn );
        exit( errorUnsupportedSyntax );
      }

      if( curToken != tkRBrace ) {
        printf( "[L%u,C%u] Array dimension must be closed with a right brace ]\n", curLine, curColumn );
        exit( expectedRightBrace );
      }
      GetToken(); // Skip closing array bracket ]
    }

    memcpy( destSpec, &tempSpec, sizeof(tempSpec) );
  }

  void BeginParse() {
    // Pre-read first two characters
    ReadChar();
    ReadChar();

    line = 1;
    column = 1;

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
      // Write include guard begin
      fprintf( headerFile, "#ifndef %s_H\n#define %s_H\n", baseName, baseName );

      // Generate built-in type declarations for unsigned integers
      fprintf( headerFile, "\n  typedef unsigned uint;\n" );
      fprintf( headerFile, "  typedef unsigned char uint8;\n" );
      fprintf( headerFile, "  typedef unsigned short uint16;\n" );
      fprintf( headerFile, "  typedef unsigned long uint32;\n" );

      // Generate built-in type declarations for signed integers
      fprintf( headerFile, "\n  typedef signed char int8;\n" );
      fprintf( headerFile, "  typedef signed short int16;\n" );
      fprintf( headerFile, "  typedef signed long int32;\n" );
    } else {
      printf( "Header file not open\n" );
      exit( errorHeaderFileNotOpen );
    }
  }

  /*
   *  program IDENT
   */
  void ParseProgramHeader() {
    unsigned programToken;

    // program IDENT
    programToken = FindKeyword(curTokenStr);
    if( programToken != rsvdProgram ) {
      printf( "[L%u,C%u] Expected keyword program\n", curLine, curColumn );
      exit( expectedKeyword );
    }
    GetToken(); // Skip keyword program

    if( curToken != tkIdent ) {
      printf( "[L%u,C%u] Expected undeclared identifier for program name\n", curLine, curColumn );
      exit( expectedUndeclaredIdentifier );
    }
    memcpy( programName, curTokenStr, sizeof(curTokenStr) );
    GetToken(); // Skip IDENT
  }

  /*
   *  const TYPESPEC IDENT = CEXPR
   */
  void ParseConstDecl() {
    GetToken(); // Skip keyword const

    printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
    exit( errorPendingImplementation );
  }

  /*
   *  enum BASETYPE IDENT
   *    IDENT, ... = CEXPR
   *    ...
   *  end
   */
  void ParseEnumDecl() {
    GetToken(); // Skip keyword enum

    printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
    exit( errorPendingImplementation );
  }

  /*
   *  struct IDENT
   *    TYPESPEC IDENT, ...
   *    union (TYPESPEC, ...) IDENT, ...
   *    ...
   *  end
   */
  void ParseStructDecl() {
    GetToken(); // Skip keyword struct

    printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
    exit( errorPendingImplementation );
  }

  /*
   *  union IDENT
   *    TYPESPEC IDENT, ...
   *    struct
   *      TYPESPEC IDENT, ...
   *      ...
   *    end
   *    ...
   *  end
   */
  void ParseUnionDecl() {
    GetToken(); // Skip keyword union

    printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
    exit( errorPendingImplementation );
  }

  /*
   *  type TYPESPEC IDENT
   */
  void ParseTypeDecl() {
    TypeSpec typeSpec = {};
    unsigned keywordToken;
    GetToken(); // Skip keyword type

    memset( &typeSpec, 0, sizeof(typeSpec) );
    ParseTypeSpec( &typeSpec );

    keywordToken = FindKeyword(curTokenStr);
    if( (curToken != tkIdent) || keywordToken ) {
      printf( "[L%u,C%u] Expected undeclared identifier\n", curLine, curColumn );
      exit( expectedUndeclaredIdentifier );
    }
    GetToken(); // Skip identifier
  }

  /*
   *  var
   *    TYPESPEC IDENT, ... = CEXPR
   *    ...
   *  end
   */
  void ParseGlobalVarBlock() {
    GetToken(); // Skip keyword var

    printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
    exit( errorPendingImplementation );
  }

  /*
   *  import TYPESPEC IDENT( TYPESPEC IDENT, ... )
   */
  void ParseImportDecl() {
    TypeSpec typeSpec;
    unsigned keywordToken;

    GetToken(); // Skip keyword import
    GetToken(); // Skip IDENT

    if( curToken != tkLParen ) {
      printf( "[L%u,C%u] Expected left parenthesis\n", curLine, curColumn );
      exit( expectedLeftParenthesis );
    }
    GetToken(); // Skip left parenthesis (

    do {
      memset( &typeSpec, 0, sizeof(typeSpec) );
      ParseTypeSpec( &typeSpec );

      do {
        GetToken(); // Skip ident
        if( curToken != tkComma ) {
          break;
        }
        GetToken(); // Skip comma (,)
      } while( curToken && (curToken == tkIdent) );
    } while( curToken && (curToken != tkRParen) );
    GetToken(); // Skip right parenthesis }

    keywordToken = FindKeyword(curTokenStr);
    if( keywordToken != rsvdFrom ) {
      printf( "[L%u,C%u] Expected keyword from\n", curLine, curColumn );
      exit( expectedKeyword );
    }
    GetToken(); // Skip keyword from

    GetToken(); // Skip DLL file name string
  }

  /*
   *  funcdecl TYPESPEC IDENT( TYPESPEC IDENT, ... )
   */
  void ParseFuncDecl() {
    GetToken(); // Skip keyword funcdecl

    printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
    exit( errorPendingImplementation );
  }

  /*
   *  FUNCCALL( EXPR, ... )
   */
  void ParseFuncCall() {
    printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
    exit( errorPendingImplementation );
  }

  /*
   *  METHODCALL( EXPR, ... )
   */
  void ParseMethodCall() {
    printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
    exit( errorPendingImplementation );
  }

  /*
   *  bind( OBJVAR, IFNAME, IFNAME.METHODNAME, ... )
   */
  void ParseBind() {
    GetToken(); // Skip keyword bind

    printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
    exit( errorPendingImplementation );
  }

  /*
   *  VARREF ASSIGNOP EXPR
   */
  void ParseVarRef() {
    printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
    exit( errorPendingImplementation );
  }

  /*
   *  CONDITION
   */
  void ParseCondition() {
    unsigned keywordToken;
    unsigned parenLevel = 0;

printf( "ParseCondition()\n" );
    do {
      // Parse left unary operator, or open parenthesis
      do {
        switch( curToken ) {
        case tkLParen:
printf( "ParseCondition() main loop: left open parenthesis (\n" );
          if( parenLevel == ((unsigned)-1) ) {
            printf( "[L%u,C%u] Parenthesis nested too deep\n", curLine, curColumn );
            exit( errorParenNestedTooDeep );
          }
          parenLevel++;
          GetToken(); // Skip open parenthesis (
          continue;

        case opSub:
printf( "ParseCondition() main loop: left unary -\n" );
          GetToken(); // Skip unary negate operator (-)
          continue;

        case opAdd:
printf( "ParseCondition() main loop: left unary +\n" );
          GetToken(); // Skip unary positive operator (+)
          continue;

        case unaryNot:
printf( "ParseCondition() main loop: left unary ~\n" );
          GetToken(); // Skip bitwise unary not operator (~)
          continue;

        case unaryIsNot:
printf( "ParseCondition() main loop: left unary !\n" );
          GetToken(); // Skip boolean unary not operator (!)
          continue;
        }
        break;
      } while( curToken );

      // Parse left operand
      switch( curToken ) {
      case opPostInc:
      case opPostDec:
printf( "ParseCondition() main loop: ++/-- left operand\n" );
        // Parse left pre-increment/decrement operator
        if( curToken != tkIdent ) { // Change when token table is implemented
          printf( "[L%u,C%u] Variable expected\n", curLine, curColumn );
          exit( expectedVariable );
        }
        GetToken(); // Skip pre-increment/decrement operator

      case tkIdent: // Change when token table is implemented
printf( "ParseCondition() main loop: left operand curTokenStr == %s\n", curTokenStr );
        GetToken(); // Skip identifier

        // Temporary - begin parse array dimension
        if( curToken == tkLBrace ) {
printf( "ParseCondition() main loop: left operand [\n" );
          GetToken(); // Skip open brace ([)

          switch( curToken ) { // Parse array index
          case tkIdent: // Change when token table is implemented
printf( "ParseCondition() main loop: left operand array index curTokenStr == %s\n", curTokenStr );
            GetToken(); // Skip array index identifier
            break;

          case valUint:
printf( "ParseCondition() main loop: left operand array index curTokenVal.valUint == %u\n", curTokenVal.valUint );
            GetToken(); // Skip array index value
            break;

          default:
printf( "ParseCondition() main loop: left operand array index ??? curTokenStr == %s; curToken == %u\n", curTokenStr, curToken );
            printf( "[L%u,C%u] Unsupported array index\n", curLine, curColumn );
            exit( errorUnsupportedArrayIndex );
          }

          if( curToken != tkRBrace ) {
            printf( "[L%u,C%u] Expected right brace\n", curLine, curColumn );
            exit( errorUnsupportedArrayIndex );
          }
          GetToken(); // Skip closing brace (])
        }
        // Temporary - end parse array dimension

        // Parse left post-increment/decrement operator
        switch( curToken ) {
        case opPostInc:
        case opPostDec:
printf( "ParseCondition() main loop: left operand --/++\n" );
          GetToken(); // Skip post-increment/decrement operator
          break;
        }
        break;

      case valUint:
printf( "ParseCondition() main loop: left operand curTokenVal.valUint == %u\n", curTokenVal.valUint );
        GetToken(); // Skip uint value
        break;

      default:
printf( "ParseCondition() main loop: left operand ??? curTokenStr == %s; curToken == %u\n", curTokenStr, curToken );
        printf( "[L%u,C%u] Expected left operand variable or value\n", curLine, curColumn );
        exit( expectedOperand );
      }

      // Parse right subexpression
      if( (curToken >= firstOper) && (curToken <= lastOper) ) {
        // Parse operator
        switch( curToken ) {
        case opLT:
        case opLTEq:
        case opGT:
        case opGTEq:
        case opEq:
        case opNotEq:
        case opAndIs:
        case opOrIs:
        case opMul:
        case opDiv:
        case opMod:
        case opAdd:
        case opSub:
        case opShl:
        case opShr:
        case opAnd:
        case opXor:
        case opOr:
printf( "ParseCondition() main loop: operator curToken == %u; firstOper == %u; lastOper == %u\n", curToken, firstOper, lastOper );
          break;

        default:
printf( "ParseCondition() main loop: operator ??? curTokenStr == %s; curToken == %u; firstOper == %u; lastOper == %u\n", curTokenStr, curToken, firstOper, lastOper );
          printf( "[L%u,C%u] Unsupported operator\n", curLine, curColumn );
          exit( errorUnsupportedOperator );
        }
        GetToken(); // Skip operator

        // Parse right unary
        do {
          switch( curToken ) {
          case tkLParen:
            if( parenLevel == ((unsigned)-1) ) {
              printf( "[L%u,C%u] Parenthesis nested too deep\n", curLine, curColumn );
              exit( errorParenNestedTooDeep );
            }
            parenLevel++;
            GetToken(); // Skip open parenthesis (
            continue;

          case opSub:
            GetToken(); // Skip unary negate operator (-)
            continue;

          case opAdd:
            GetToken(); // Skip unary positive operator (+)
            continue;

          case unaryNot:
            GetToken(); // Skip bitwise unary not operator (~)
            continue;

          case unaryIsNot:
            GetToken(); // Skip boolean unary not operator (!)
            continue;
          }
          break;
        } while( curToken );

        // Parse right operand
        switch( curToken ) {
          case opPostInc:
          case opPostDec:
            // Parse right pre-increment/decrement operator
            if( curToken != tkIdent ) { // Change when token table is implemented
              printf( "[L%u,C%u] Variable expected\n", curLine, curColumn );
              exit( expectedVariable );
            }
            GetToken(); // Skip pre-increment/decrement operator

        case tkIdent: // Change when token table is implemented
          GetToken(); // Skip identifier

          // Temporary - begin parse array dimension
          if( curToken == tkLBrace ) {
            GetToken(); // Skip open brace ([)

            switch( curToken ) { // Parse array index
            case tkIdent: // Change when token table is implemented
              GetToken(); // Skip array index identifier
              break;

            case valUint:
              GetToken(); // Skip array index value
              break;

            default:
              printf( "[L%u,C%u] Unsupported array index\n", curLine, curColumn );
              exit( errorUnsupportedArrayIndex );
            }

            if( curToken != tkRBrace ) {
              printf( "[L%u,C%u] Expected right brace\n", curLine, curColumn );
              exit( errorUnsupportedArrayIndex );
            }
            GetToken(); // Skip closing brace (])
          }
          // Temporary - end parse array dimension

          // Parse right post-increment/decrement operator
          switch( curToken ) {
          case opPostInc:
          case opPostDec:
            GetToken(); // Skip post-increment/decrement operator
            break;
          }
          break;

        case valUint:
          GetToken(); // Skip uint value
          break;

        default:
          printf( "[L%u,C%u] Expected left operand variable or value\n", curLine, curColumn );
          exit( expectedOperand );
        }
      }

      // Parse closing parenthesis
      while( curToken == tkRParen ) {
        if( parenLevel == 0 ) {
          printf( "[L%u,C%u] Parenthesis must be open with ( before closed with )\n", curLine, curColumn );
          exit( expectedRightParenthesis );
        }
        parenLevel--;
        GetToken(); // Skip close parenthesis )
      }

//      printf( "ParseCondition() breakpoint: curTokenStr == %s; curToken == %u; nextTokenStr == %s; nextToken == %u\n", curTokenStr, curToken, nextTokenStr, nextToken );
//      exit( 255 );
    } while( curToken );
  }

  /*
   *  if CONDITION
   */
  void ParseIf() {
    GetToken(); // Skip keyword if

    ParseCondition();
  }

  /*
   *  then FUNCSTATEMENT
   */
  void ParseFuncThen() {
    GetToken(); // Skip keyword then

    if( ParseFuncStatement() == 0 ) {
      printf( "[L%u,C%u] Expected function statement\n", curLine, curColumn );
      exit( expectedStatement );
    }
  }

  /*
   *  then METHODSTATEMENT
   */
  void ParseMethodThen() {
    GetToken(); // Skip keyword then

    if( ParseMethodStatement() == 0 ) {
      printf( "[L%u,C%u] Expected method statement\n", curLine, curColumn );
    }
      exit( expectedStatement );
  }

  /*
   *  then STATEMENT
   */
  void ParseThen() {
    GetToken(); // Skip keyword then

    if( ParseStatement() == 0 ) {
      printf( "[L%u,C%u] Expected statement\n", curLine, curColumn );
      exit( expectedStatement );
    }
  }

  /*
   *  elseif CONDITION
   */
  void ParseElseIf() {
    GetToken(); // Skip keyword elseif

    printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
    exit( errorPendingImplementation );
  }

  /*
   *  else // Last elseXYZ section before endif
   */
  void ParseElse() {
    GetToken(); // Skip keyword else

    printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
    exit( errorPendingImplementation );
  }

  /*
   *  endif
   */
  void ParseEndIf() {
    GetToken(); // Skip keyword endif

    printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
    exit( errorPendingImplementation );
  }

  /*
   *  for VARREF in [CEXPR, ...]
   */
  void ParseForIn() {
    GetToken(); // Skip keyword in

    printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
    exit( errorPendingImplementation );
  }

  /*
   *  for VARREF = EXPR to EXPR
   *  for VARREF = EXPR downto EXPR
   */
  void ParseForToDownTo() {
    GetToken(); // Skip keyword to, or keyword downto

    printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
    exit( errorPendingImplementation );
  }

  /*
   *  for VARREF in [CEXPR, ...]
   *  for VARREF = EXPR to EXPR
   *  for VARREF = EXPR downto EXPR
   */
  void ParseFor() {
    GetToken(); // Skip keyword for

    printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
    exit( errorPendingImplementation );
  }

  /*
   *  endfor
   */
  void ParseEndFor() {
    GetToken(); // Skip keyword endfor

    printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
    exit( errorPendingImplementation );
  }

  /*
   *  echo( EXPR, ... )
   */
  void ParseEcho() {
    GetToken(); // Skip keyword echo

    printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
    exit( errorPendingImplementation );
  }

  /*
   *  echo( EXPR, ... )
   */
  void ParseEchoLn() {
    GetToken(); // Skip keyword echoln

    printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
    exit( errorPendingImplementation );
  }

  /*
   *  repeat
   */
  void ParseRepeat() {
    GetToken(); // Skip keyword repeat

    printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
    exit( errorPendingImplementation );
  }

  /*
   *  when CONDITION
   */
  void ParseWhen() {
    GetToken(); // Skip keyword when

    printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
    exit( errorPendingImplementation );
  }

  /*
   *  while CONDITION
   */
  void ParseWhile() {
    GetToken(); // Skip keyword while

    printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
    exit( errorPendingImplementation );
  }

  /*
   *  endwhile
   */
  void ParseEndWhile() {
    GetToken(); // Skip keyword endwhile

    printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
    exit( errorPendingImplementation );
  }

  /*
   *  IDENT:
   */
  void ParseLabelDecl() {
    GetToken(); // Skip label identifier

    if( curToken != tkColon ) {
      printf( "[L%u,C%u] Expected colon\n" );
      exit( expectedSymbol );
    }
    GetToken(); // Skip colon (:)
  }

  /*
   *  var
   *    TYPESPEC IDENT, ... = CEXPR
   *    ...
   *  end
   */
  void ParseLocalVarBlock() {
    TypeSpec typeSpec;
    unsigned keywordToken;

    GetToken(); // Skip keyword var

    while( curToken ) {
      // Get type specification
      memset( &typeSpec, 0, sizeof(typeSpec) );
      ParseTypeSpec( &typeSpec );

      // Declare local variables
      do {
        keywordToken = FindKeyword(curTokenStr);
        if( (curToken != tkIdent) || keywordToken ) {
          printf( "[L%u,C%u] Expected undeclared identifier\n", curLine, curColumn );
          exit( expectedUndeclaredIdentifier );
        }

        GetToken(); // Skip ident

        if( curToken != tkComma ) {
          break;
        }
        GetToken(); // Skip comma (,)
      } while( curToken && (curToken == tkIdent) );

      // Check for keyword end
      keywordToken = FindKeyword(curTokenStr);
      if( keywordToken == rsvdEnd ) {
        GetToken(); // Skip keyword end
        return;
      }
    }
  }

  /*
   *  goto LABEL
   */
  void ParseGoto() {
    GetToken(); // Skip goto

    if( curToken != tkIdent ) { ///TODO: Replace when token table is implemented
      printf( "[L%u,C%u] Expected label\n", curLine, curColumn );
      exit( expectedDeclaredIdentifier );
    }
    GetToken(); // Skip LABEL
  }

  /*
   *  exit( EXPR )
   */
  void ParseExit() {
    GetToken(); // Skip keyword exit

    printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
    exit( errorPendingImplementation );
  }

  int ParseFuncStatement() {
    unsigned keywordToken;

    if( strcmp(curTokenStr, "result") == 0 ) {
      GetToken(); // Skip keyword result

      printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
      exit( errorPendingImplementation );
//      return -1;
    } else if( strcmp(curTokenStr, "return") == 0 ) {
      GetToken(); // Skip keyword return

      printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
      exit( errorPendingImplementation );
//      return -1;
    } else if( strcmp(curTokenStr, "then") == 0 ) {
      ParseFuncThen();
      return -1;
    }

    return ParseStatement();
  }

  int ParseMethodStatement() {
    unsigned keywordToken;

    if( strcmp(curTokenStr, "self") == 0 ) {
      printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
      exit( errorPendingImplementation );
//      return -1;
    } else if( strcmp(curTokenStr, "result") == 0 ) {
      printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
      exit( errorPendingImplementation );
//      return -1;
    } else if( strcmp(curTokenStr, "return") == 0 ) {
      printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
      exit( errorPendingImplementation );
//      return -1;
    } else if( strcmp(curTokenStr, "then") == 0 ) {
      ParseMethodThen();
      return -1;
    }

    return ParseStatement();
  }

  /*
   *  FUNCCALL, METHODCALL, bind, VAREXPR, if/then, if...endif, for...endfor,
   *  repeat...when, while...endwhile, echo, echoln, LABELDECL, goto, exit
   */
  int ParseStatement() {
    unsigned keywordToken;

    keywordToken = FindKeyword(curTokenStr);
    switch( keywordToken ) {
    case rsvdBind:
      ParseBind();
      return -1;

    case rsvdIf:
      ParseIf();
      return -1;

    case rsvdElseIf:
      ParseElseIf();
      return -1;

    case rsvdElse:
      ParseElse();
      return -1;

    case rsvdEndIf:
      ParseEndIf();
      return -1;

    case rsvdFor:
      ParseFor();
      return -1;

    case rsvdEndFor:
      ParseEndFor();
      return -1;

    case rsvdRepeat:
      ParseRepeat();
      return -1;

    case rsvdWhen:
      ParseWhen();
      return -1;

    case rsvdWhile:
      ParseWhile();
      return -1;

    case rsvdEndWhile:
      ParseEndWhile();
      return -1;

    case rsvdEcho:
      ParseEcho();
      return -1;

    case rsvdEchoLn:
      ParseEchoLn();
      return -1;

    case rsvdGoto:
      ParseGoto();
      return -1;

    case rsvdExit:
      ParseExit();
      return -1;
    }

    if( (curToken == tkIdent) && (nextToken == tkColon) ) {
      ParseLabelDecl();
      return -1;
    }

    return 0;
  }

  /*
   *  func TYPESPEC IDENT( TYPESPEC IDENT, ... )
   *    var
   *      TYPESPEC IDENT, ... = CEXPR
   *      ...
   *    end
   *
   *    STATEMENTS
   *    result ASSIGNOP EXPR
   *    return
   *    ...
   *  end
   */
  void ParseFunc() {
    GetToken(); // Skip keyword func

    printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
    exit( errorPendingImplementation );
  }

  /*
   *  object IDENT
   *    inherits OBJNAME, ...
   *  mutable
   *  immutable // Default
   *    TYPESPEC MEMBERIDENT, ...
   *    ...
   *  end
   */
  void ParseObjectDecl() {
    GetToken(); // Skip keyword object

    printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
    exit( errorPendingImplementation );
  }

  /*
   *  abstract IDENT
   *    implements OBJNAME
   *    inherits IFNAME, ...
   *
   *    method METHODNAME( TYPESPEC IDENT, ... )
   *    ...
   *  end
   */
  void ParseAbstractDecl() {
    GetToken(); // Skip keyword abstract

    printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
    exit( errorPendingImplementation );
  }

  /*
   *  interface IDENT
   *    implements OBJNAME
   *    inherits IFNAME, ...
   *
   *    method METHODNAME( TYPESPEC IDENT, ... )
   *    ...
   *  end
   */
  void ParseInterfaceDecl() {
    GetToken(); // Skip keyword interface

    printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
    exit( errorPendingImplementation );
  }

  /*
   *  method IFNAME.METHODNAME( TYPESPEC IDENT, ... )
   *    var
   *      TYPESPEC IDENT = CEXPR, ... = CEXPR
   *      ...
   *    end
   *
   *    STATEMENTS
   *    OBJSTATEMENTS
   *    result ASSIGNOP EXPR
   *    return EXPR
   *  end
   */
  void ParseMethod() {
    GetToken(); // Skip keyword method

    printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
    exit( errorPendingImplementation );
  }

  /*
   *  operator TYPESPEC OPER( TYPESPEC VALUE, ... )
   *    var
   *      TYPESPEC IDENT, ... = CEXPR
   *      ...
   *    end
   *
   *    STATEMENTS
   *    result ASSIGNOP EXPR
   *    return EXPR
   *  end
   */
  void ParseOperatorFunc() {
    GetToken(); // Skip keyword operator

    printf( "[L%u,C%u] Keyword pending implementation\n", curLine, curColumn );
    exit( errorPendingImplementation );
  }

  /*
   *  run
   *    var
   *      TYPESPEC IDENT, ... = CEXPR
   *    end
   *
   *    STATEMENTS
   *  end
   */
  void ParseRun() {
    unsigned keywordToken;

    if( runDeclared ) {
      printf( "[L%u,C%u] run is already declared\n", curLine, curColumn );
      exit( runAlreadyDeclared );
    }
    runDeclared = -1;
    GetToken(); // Skip keyword run

    if( cFile ) {
      fprintf( cFile, "\nint main( int argc, char* argv[] ) {\n" );
    } else {
      printf( "C file not open\n" );
      exit( errorCFileNotOpen );
    }

    // Parse local variable declaration block
    keywordToken = FindKeyword(curTokenStr);
    if( keywordToken == rsvdVar ) {
      ParseLocalVarBlock();
    }

    // Parse run block statements until end of run block
    do {
      keywordToken = FindKeyword(curTokenStr);
      switch( keywordToken ) {
      case rsvdEnd:
        ParseEndRun();
        return;

      default:
        if( ParseStatement() == 0 ) {
          printf( "[L%u,C%u] Expected end or statement\n", curLine, curColumn );
          exit( expectedEndOrStatement );
        }
      }
    } while( curToken );
  }

  /*
   *  end
   */
  void ParseEndRun() {
    GetToken(); // Skip keyword end

    if( cFile ) {
      fprintf( cFile, "  return 0;\n}\n" );
    } else {
      printf( "C file not open\n" );
      exit( errorCFileNotOpen );
    }
  }

  /*
   *  const, enum, struct, union, type, var, import, funcdecl, func,
   *  object, abstract, interface, method, operator, run
   */
  void ParseTopLevel() {
    char* topLevelExitStr = "Expected run, or top level keyword";
    unsigned topLevelExitCode = expectedRunOrTopLevel;
    unsigned keywordToken;

    do {
      keywordToken = FindKeyword(curTokenStr);

      if( keywordToken == rsvdConst ) {
        ParseConstDecl();
        continue;
      }

      if( keywordToken == rsvdEnum ) {
        ParseEnumDecl();
        continue;
      }

      if( keywordToken == typeStruct ) {
        ParseStructDecl();
        continue;
      }

      if( keywordToken == typeUnion ) {
        ParseUnionDecl();
        continue;
      }

      if( keywordToken == rsvdType ) {
        ParseTypeDecl();
        continue;
      }

      if( keywordToken == rsvdVar ) {
        ParseGlobalVarBlock();
        continue;
      }

      if( keywordToken == rsvdImport ) {
        ParseImportDecl();
        continue;
      }

      if( keywordToken == rsvdFuncDecl ) {
        ParseFuncDecl();
        continue;
      }

      if( keywordToken == rsvdFunc ) {
        ParseFunc();
        continue;
      }

      if( keywordToken == rsvdObject ) {
        ParseObjectDecl();
        continue;
      }

      if( keywordToken == rsvdAbstract ) {
        ParseAbstractDecl();
        continue;
      }

      if( keywordToken == rsvdInterface ) {
        ParseInterfaceDecl();
        continue;
      }

      if( keywordToken == rsvdMethod ) {
        ParseMethod();
        continue;
      }

      if( keywordToken == rsvdOperator ) {
        ParseOperatorFunc();
        continue;
      }

      if( keywordToken == rsvdRun ) {
        ParseRun();
        topLevelExitStr = "Expected top level keyword";
        topLevelExitCode = expectedTopLevel;
        continue;
      }

      if( curToken ) {
        printf( "[L%u,C%u] %s\n", curLine, curColumn, topLevelExitStr );
        exit( topLevelExitCode );
      }
    } while( curToken );
  }

  void EndParse() {
    GetToken(); // Skip end

    // Finalize C file
    if( runDeclared == 0 ) {
      printf( "[L%u,C%u] Expected run\n", curLine, curColumn );
      exit( expectedRun );
    }

    // Write include guard end
    fprintf( headerFile, "\n#endif %s_H\n", baseName );
  }

int main( int argc, char* argv[] ) {
  char commandLine[4096] = {};
  int result;

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
  snprintf( commandLine, sizeof(commandLine) - 1,
    ".\\tcc\\tcc.exe -xc %s -o %s", cName, exeName );
  result = RunProgram( commandLine );
  if( result ) {
    printf( "Error building intermediate source %s\n", cName );
    exit( errorBuildingIntermediateSource );
  }
  printf( "Done.\n" );

  // Release program resources
  Cleanup();

  return 0;
}
