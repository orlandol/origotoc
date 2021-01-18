
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "keyarray.h"

/*
 *  Origo configuration options
 */

  #define MAXLEN_TOKENSTR 2047

  #define MAXLEN_IDENT 63
  #define MAXLEN_PAIRED_IDENT (((MAXLEN_IDENT) * 2) + 1)
  #define MAXLEN_QUALIFIED_IDENT (MAXLEN_TOKENSTR)

  #define MAXLEN_MANGLED_TYPESPEC (((MAXLEN_IDENT) * 2) + 1)
  #define MAXLEN_MANGLED_OPERATOR ((((MAXLEN_IDENT) * 3) + 2)

  #define MAXLEN_OPERATOR 4

/*
 *  Origo system exit codes
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
    errorInternal = 41,
  } ErrorCodes;

/*
 *  System helper declarations
 */

  int RunProgram( char* commandLine );

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
      tkConst,
      tkEnumType,
      tkStruct,
      tkUnion,
      tkType,
      tkVar,
      tkImport,
      tkFunc,
      tkObject,
      tkAbstract,
      tkInterface,
      tkOperator,

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
      typeEnum,
      typeStruct,
      typeUnion,
      typeFunc,
      typeObject,
      typeMethod,
        lastType = typeMethod,

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

      valStruct = (valImmediate + (5 << 5)),
          firstValStruct = valStruct,
          lastValStruct = valStruct,

      valUnion = (valImmediate + (6 << 5)),
          firstValUnion = valUnion,
          lastValUnion = valUnion,

      valData = (valImmediate + (7 << 5)),
          firstValData = valData,
          lastValData = valData,

    // Operator tokens
    operSymbol = (5 << 9),
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
    assignSymbol  = (6 << 9),
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

/*
 *  Type specifier table declarations
 */

  typedef struct TypeSpec {
    char typeName[MAXLEN_MANGLED_TYPESPEC + 1];
    unsigned ptrType;
    unsigned baseType;
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

  void FreeTypeSpec( TypeSpec* data );
  int CopyTypeSpec( TypeSpec* dest, TypeSpec* source );

  DECLARE_UINT_KEYARRAY_TYPES( TypeTable, TypeSpec )

  DECLARE_UINT_KEYARRAY_CREATE( CreateTypeTable, TypeTable )
  DECLARE_UINT_KEYARRAY_FREE( FreeTypeTable, TypeTable, FreeTypeSpec )

  DECLARE_UINT_KEYARRAY_INSERT( InsertTypeSpec, TypeTable, TypeSpec )
  DECLARE_UINT_KEYARRAY_REMOVE( RemoveTypeSpec, TypeTable, FreeTypeSpec )
  DECLARE_UINT_KEYARRAY_MODIFY( ModifyTypeSpec, TypeTable, TypeSpec )
  DECLARE_UINT_KEYARRAY_RETRIEVE( RetrieveTypeSpec, TypeTable, TypeSpec )
  DECLARE_UINT_KEYARRAY_FINDINDEX( IndexOfTypeSpec, TypeTable )

  DECLARE_UINT_KEYARRAY_RELEASEUNUSED( CompactTypeTable, TypeTable )
  DECLARE_UINT_KEYARRAY_COPY( CopyTypeTable, TypeTable, TypeSpec,
      CopyTypeSpec, FreeTypeSpec )

  int MangleTypeSpec( TypeSpec* typeSpec, char* destBuffer, size_t destSize );

/*
 *  Enum table declarations
 */

  typedef struct Enum {
    unsigned typeID;
    TokenVal value;
  } Enum;

  void FreeEnum( Enum* data );
  int CopyEnum( Enum* dest, Enum* source );

  DECLARE_STRING_KEYARRAY_TYPES( EnumTable, Enum )

  DECLARE_STRING_KEYARRAY_CREATE( CreateEnumTable, EnumTable )
  DECLARE_STRING_KEYARRAY_FREE( FreeEnumTable, EnumTable, FreeEnum )

  DECLARE_STRING_KEYARRAY_INSERT( DeclareEnum, EnumTable, Enum )
  DECLARE_STRING_KEYARRAY_REMOVE( RemoveEnum, EnumTable, FreeEnum )
  DECLARE_STRING_KEYARRAY_MODIFY( ModifyEnum, EnumTable, Enum )
  DECLARE_STRING_KEYARRAY_RETRIEVE( LookupEnum, EnumTable, Enum )
  DECLARE_STRING_KEYARRAY_FINDINDEX( IndexOfEnum, EnumTable )

  DECLARE_STRING_KEYARRAY_RELEASEUNUSED( CompactEnumTable, EnumTable )
  DECLARE_STRING_KEYARRAY_COPY( CopyEnumTable, EnumTable, Enum,
      CopyEnum, FreeEnum )

/*
 *  Field table declarations
 */

  typedef struct Field {
    unsigned typeID;
    TokenVal value;
  } Field;

  void FreeField( Field* data );
  int CopyField( Field* dest, Field* source );

  DECLARE_STRING_KEYARRAY_TYPES( FieldTable, Field )

  DECLARE_STRING_KEYARRAY_CREATE( CreateFieldTable, FieldTable )
  DECLARE_STRING_KEYARRAY_FREE( FreeFieldTable, FieldTable, FreeField )

  DECLARE_STRING_KEYARRAY_INSERT( DeclareField, FieldTable, Field )
  DECLARE_STRING_KEYARRAY_REMOVE( RemoveField, FieldTable, FreeField )
  DECLARE_STRING_KEYARRAY_MODIFY( ModifyField, FieldTable, Field )
  DECLARE_STRING_KEYARRAY_RETRIEVE( LookupField, FieldTable, Field )
  DECLARE_STRING_KEYARRAY_FINDINDEX( IndexOfField, FieldTable )

  DECLARE_STRING_KEYARRAY_RELEASEUNUSED( CompactFieldTable, FieldTable )
  DECLARE_STRING_KEYARRAY_COPY( CopyFieldTable, FieldTable, Field,
      CopyField, FreeField )

/*
 *  Parameter table declarations
 */

  typedef struct Param {
    unsigned typeID;
    unsigned index;
  } Param;

  void FreeParam( Param* data );
  int CopyParam( Param* dest, Param* source );

  DECLARE_STRING_KEYARRAY_TYPES( ParamTable, Param )

  DECLARE_STRING_KEYARRAY_CREATE( CreateParamTable, ParamTable )
  DECLARE_STRING_KEYARRAY_FREE( FreeParamTable, ParamTable, FreeParam )

  DECLARE_STRING_KEYARRAY_INSERT( DeclareParam, ParamTable, Param )
  DECLARE_STRING_KEYARRAY_REMOVE( RemoveParam, ParamTable, FreeParam )
  DECLARE_STRING_KEYARRAY_MODIFY( ModifyParam, ParamTable, Param )
  DECLARE_STRING_KEYARRAY_RETRIEVE( LookupParam, ParamTable, Param )
  DECLARE_STRING_KEYARRAY_FINDINDEX( IndexOfParam, ParamTable )

  DECLARE_STRING_KEYARRAY_RELEASEUNUSED( CompactParamTable, ParamTable )
  DECLARE_STRING_KEYARRAY_COPY( CopyParamTable, ParamTable, Param,
      CopyParam, FreeParam )

/*
 *  Identifier table declarations
 */

  typedef struct Ident {
    unsigned index;
  } Ident;

  void FreeIdent( Ident* data );
  int CopyIdent( Ident* dest, Ident* source );

  DECLARE_STRING_KEYARRAY_TYPES( IdentTable, Ident )

  DECLARE_STRING_KEYARRAY_CREATE( CreateIdentTable, IdentTable )
  DECLARE_STRING_KEYARRAY_FREE( FreeIdentTable, IdentTable, FreeIdent )

  DECLARE_STRING_KEYARRAY_INSERT( DeclareIdent, IdentTable, Ident )
  DECLARE_STRING_KEYARRAY_REMOVE( RemoveIdent, IdentTable, FreeIdent )
  DECLARE_STRING_KEYARRAY_MODIFY( ModifyIdent, IdentTable, Ident )
  DECLARE_STRING_KEYARRAY_RETRIEVE( LookupIdent, IdentTable, Ident )
  DECLARE_STRING_KEYARRAY_FINDINDEX( IndexOfIdent, IdentTable )

  DECLARE_STRING_KEYARRAY_RELEASEUNUSED( CompactIdentTable, IdentTable )
  DECLARE_STRING_KEYARRAY_COPY( CopyIdentTable, IdentTable, Ident,
      CopyIdent, FreeIdent )

/*
 *  Object member variable table declarations
 */

  typedef enum MemberAccessType {
    accessImmutable = 0,
      defaultAccess = accessImmutable,
    accessMutable = 1,
    accessInternal = 2
  } MemberAccessType;

  typedef struct Member {
    unsigned typeID;
    unsigned accessType;
    unsigned memberIndex;
    TokenVal value;
  } Member;

  void FreeMember( Member* data );
  int CopyMember( Member* dest, Member* source );

  DECLARE_STRING_KEYARRAY_TYPES( MemberTable, Member )

  DECLARE_STRING_KEYARRAY_CREATE( CreateMemberTable, MemberTable )
  DECLARE_STRING_KEYARRAY_FREE( FreeMemberTable, MemberTable, FreeMember )

  DECLARE_STRING_KEYARRAY_INSERT( DeclareMember, MemberTable, Member )
  DECLARE_STRING_KEYARRAY_REMOVE( RemoveMember, MemberTable, FreeMember )
  DECLARE_STRING_KEYARRAY_MODIFY( ModifyMember, MemberTable, Member )
  DECLARE_STRING_KEYARRAY_RETRIEVE( LookupMember, MemberTable, Member )
  DECLARE_STRING_KEYARRAY_FINDINDEX( IndexOfMember, MemberTable )

  DECLARE_STRING_KEYARRAY_RELEASEUNUSED( CompactMemberTable, MemberTable )
  DECLARE_STRING_KEYARRAY_COPY( CopyMemberTable, MemberTable, Member,
      CopyMember, FreeMember )

/*
 *  Method table declarations
 */

  typedef struct Method {
    unsigned typeID;
    unsigned methodIndex;
    ParamTable* params;
  } Method;

  void FreeMethod( Method* data );
  int CopyMethod( Method* dest, Method* source );

  DECLARE_STRING_KEYARRAY_TYPES( MethodTable, Method )

  DECLARE_STRING_KEYARRAY_CREATE( CreateMethodTable, MethodTable )
  DECLARE_STRING_KEYARRAY_FREE( FreeMethodTable, MethodTable, FreeMethod )

  DECLARE_STRING_KEYARRAY_INSERT( DeclareMethod, MethodTable, Method )
  DECLARE_STRING_KEYARRAY_REMOVE( RemoveMethod, MethodTable, FreeMethod )
  DECLARE_STRING_KEYARRAY_MODIFY( ModifyMethod, MethodTable, Method )
  DECLARE_STRING_KEYARRAY_RETRIEVE( LookupMethod, MethodTable, Method )
  DECLARE_STRING_KEYARRAY_FINDINDEX( IndexOfMethod, MethodTable )

  DECLARE_STRING_KEYARRAY_RELEASEUNUSED( CompactMethodTable, MethodTable )
  DECLARE_STRING_KEYARRAY_COPY( CopyMethodTable, MethodTable, Method,
      CopyMethod, FreeMethod )

/*
 *  Token table declarations
 */

  // const
  typedef struct ConstSym {
     unsigned typeID;
   } ConstSym;
 
  // enum
  typedef struct EnumTypeSym {
    unsigned typeID;
    EnumTable* enums;
  } EnumTypeSym;
 
  // struct
  typedef struct StructSym {
    FieldTable* fields;
  } StructSym;
 
  // union
  typedef struct UnionSym {
    FieldTable* fields;
  } UnionSym;
 
  // type
  typedef struct TypeSym {
    unsigned typeID;
  } TypeSym;

  // var
  typedef struct VarSym {
    unsigned typeID;
  } VarSym;

  // import
  typedef struct ImportSym {
    unsigned typeID;
    ParamTable* funcParams;
  } ImportSym;

  // func/funcdecl
  typedef struct FuncSym {
    unsigned typeID;
    ParamTable* params;
  } FuncSym;

  // object
  typedef struct ObjectSym {
    char baseObject[MAXLEN_IDENT + 1];
    MemberTable* members;
  } ObjectSym;

  // abstract
  typedef struct AbstractSym {
    char objectName[MAXLEN_IDENT + 1];
    IdentTable* baseInterfaces;
    MethodTable* methods;
  } AbstractSym;

  // interface
  typedef struct InterfaceSym {
    char objectName[MAXLEN_IDENT + 1];
    IdentTable* baseInterfaces;
    MethodTable* methods;
  } InterfaceSym;

  // operator
  typedef struct OperatorSym {
    ParamTable* params;
  } OperatorSym;

  // Token specific fields
  typedef struct TokenSym {
    unsigned tokenCode;
    union {
      ConstSym constSym;
      EnumTypeSym enumTypeSym;
      StructSym structSym;
      UnionSym unionSym;
      TypeSym typeSym;
      VarSym varSym;
      ImportSym importSym;
      FuncSym funcSym;
      ObjectSym objectSym;
      AbstractSym abstractSym;
      InterfaceSym interfaceSym;
      OperatorSym operatorSym;
    };
  } TokenSym;

  void FreeTokenSym( TokenSym* data );
  int CopyTokenSym( TokenSym* dest, TokenSym* source );

  DECLARE_STRING_KEYARRAY_TYPES( TokenTable, TokenSym )

  DECLARE_STRING_KEYARRAY_CREATE( CreateTokenTable, TokenTable )
  DECLARE_STRING_KEYARRAY_FREE( FreeTokenTable, TokenTable, FreeTokenSym )

  DECLARE_STRING_KEYARRAY_INSERT( InsertTokenSym, TokenTable, TokenSym )
  DECLARE_STRING_KEYARRAY_REMOVE( RemoveTokenSym, TokenTable, FreeTokenSym )

  DECLARE_STRING_KEYARRAY_MODIFY( ModifyTokenSym, TokenTable, TokenSym )
  DECLARE_STRING_KEYARRAY_RETRIEVE( RetrieveTokenSym, TokenTable, TokenSym )
  DECLARE_STRING_KEYARRAY_FINDINDEX( IndexOfTokenSym, TokenTable )

  DECLARE_STRING_KEYARRAY_RELEASEUNUSED( CompactTokenTable, TokenTable )
  DECLARE_STRING_KEYARRAY_COPY( CopyTokenTable, TokenTable, TokenSym,
      CopyTokenSym, FreeTokenSym )

  int DeclareConst( TokenTable* tokenTable, unsigned typeID, char* constName );
  int LookupConst( TokenTable* tokenTable, char* constName, ConstSym* destSym );

  int DeclareEnumType( TokenTable* tokenTable, unsigned typeID, char* enumTypeName, EnumTable* enumTable );
  int LookupEnumType( TokenTable* tokenTable, char* enumTypeName, EnumTypeSym* destSym );

  int DeclareStruct( TokenTable* tokenTable, char* structName, FieldTable* fieldTable );
  int LookupStruct( TokenTable* tokenTable, char* structName, StructSym* destSym );

  int DeclareUnion( TokenTable* tokenTable, char* unionName, FieldTable* fieldTable );
  int LookupUnion( TokenTable* tokenTable, char* unionName, UnionSym* destSym );

  int DeclareType( TokenTable* tokenTable, unsigned typeID, char* testName );
  int LookupType( TokenTable* tokenTable, char* typeName, TypeSym* destSym );

  int DeclareVar( TokenTable* tokenTable, unsigned typeID, char* varName );
  int LookupVar( TokenTable* tokenTable, char* varName, VarSym* destSym );

  int DeclareFunc( TokenTable* tokenTable, unsigned typeID, char* funcName, ParamTable* paramTable );
  int LookupFunc( TokenTable* tokenTable, char* funcName, FuncSym* destSym );

  int DeclareObject( TokenTable* tokenTable, char* objectName, char* inheritsName, MemberTable* memberTable );
  int LookupObject( TokenTable* tokenTable, char* objectName, ObjectSym* destSym );

  int DeclareAbstract( TokenTable* tokenTable, char* abstractName, char* implementsName, MethodTable* methodTable );
  int LookupAbstract( TokenTable* tokenTable, char* abstractName, AbstractSym* destSym );

  int DeclareInterface( TokenTable* tokenTable, char* interfaceName, char* implementsName, MethodTable* methodTable );
  int LookupInterface( TokenTable* tokenTable, char* interfaceName, InterfaceSym* destSym );

  int DeclareOperator( TokenTable* tokenTable, char* mangledName, ParamTable* paramTable );
  int LookupOperator( TokenTable* tokenTable, char* mangledName, OperatorSym* destSym );

/*
 *  Parser declarations
 */

  char programName[MAXLEN_IDENT + 1] = {};
  int runDeclared = 0;

  unsigned line = 1;
  unsigned column = 1;

  unsigned curLine = 1;
  unsigned curColumn = 1;
  unsigned curToken = 0;
  char curTokenStr[MAXLEN_TOKENSTR + 1] = {};
  TokenVal curTokenVal = {};

  unsigned nextLine = 1;
  unsigned nextColumn = 1;
  unsigned nextToken = 0;
  char curCh = 0;
  char nextCh = 0;
  char nextTokenStr[MAXLEN_TOKENSTR + 1] = {};
  TokenVal nextTokenVal = {};

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
 *  Main program
 */

  char baseName[FILENAME_MAX + 1] = {};

  char retName[FILENAME_MAX + 1] = {};
  char exeName[FILENAME_MAX + 1] = {};
  char cName[FILENAME_MAX + 1] = {};
  char headerName[FILENAME_MAX + 1] = {};

  FILE* retFile = NULL;
  FILE* cFile = NULL;
  FILE* headerFile = NULL;

  TokenTable* tokenTable = NULL;
  TypeTable* typeTable = NULL;

  unsigned newTypeID = 0;

  void ParseOptions( int argc, char* argv[] ) {
    char* fileExt = NULL;

    // Validate parameter count
    if( (argc < 2) || (argc > 3) ) {
      printf( "Origo to C Alpha - Copyright 2020 Orlando Llanes\n" );
      printf( "\nusage: origotoc source[.ret] [binary[.exe]]\n" );
      exit(errorInvalidCommandline);
    }

    // Determine the base file name without the file extension
    strncpy( baseName, argv[1], FILENAME_MAX + 1 );
    fileExt = strrchr(baseName, '.');
    if( fileExt ) {
      memset( fileExt, 0, strlen(fileExt) );
    }

    // Determine the Retineo source file name
    strncpy( retName, argv[1], FILENAME_MAX + 1 );
    fileExt = strrchr(retName, '.');
    if( fileExt == NULL ) {
      strncat( retName, ".ret", FILENAME_MAX + 1 );
    } else if( strlen(fileExt) == 1 ) {
      *fileExt = '\0';
    }

    // Determine the executable file name
    if( argc == 3 ) {
      strncpy( exeName, argv[2], FILENAME_MAX + 1 );
    } else {
      strncpy( exeName, baseName, FILENAME_MAX + 1 );
    }
    fileExt = strrchr(exeName, '.');
    if( fileExt == NULL ) {
      strncat( exeName, ".exe", FILENAME_MAX );
    } else if( strlen(fileExt) == 1 ) {
      strncat( exeName, "exe", FILENAME_MAX );
    }

    // Set the C file name
    strncpy( cName, baseName, FILENAME_MAX + 1 );
    strncat( cName, ".rgc", FILENAME_MAX );

    // Set the C Header file name
    strncpy( headerName, baseName, FILENAME_MAX + 1 );
    strncat( headerName, ".rgh", FILENAME_MAX );
  }

  void Cleanup() {
    FreeTokenTable( &tokenTable );
    FreeTypeTable( &typeTable );

    CloseRet( &retFile );
    CloseC( &cFile );
    CloseHeader( &headerFile );
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
  printf( "\nBuilding %s from %s/%s...\n", exeName, cName, headerName );
  snprintf( commandLine, FILENAME_MAX,
    ".\\tcc\\tcc.exe -xc %s -o %s -Wall", cName, exeName );
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

/*
 *  System helper implementation
 */

  int RunProgram( char* commandLine ) {
    int errorLevel = -1;

    if( commandLine && (*commandLine) ) {
      errorLevel = system(commandLine);
    }

    return errorLevel;
  }


/*
 *  Type specifier table implementation
 */

  void FreeTypeSpec( TypeSpec* data ) {
  }

  int CopyTypeSpec( TypeSpec* dest, TypeSpec* source ) {
    return -1;
  }

  int MangleTypeSpec( TypeSpec* typeSpec, char* destBuffer, size_t destSize ) {
    static const char* typeString[] = { "", "u", "i", "e", "s", "u", "o" };
    char mangledResult[MAXLEN_MANGLED_TYPESPEC + 1] = {};
    int mangledLength;

    if( !(typeSpec && destBuffer && destSize) ) {
      return 0;
    }

    mangledLength = snprintf( mangledResult, MAXLEN_MANGLED_TYPESPEC, "%s%s%s%s",
      typeSpec->ptrType == ptrData ? "p" : typeSpec->ptrType == ptrRef ? "r" : "",
      (typeSpec->baseType - typeUint + 1) <= (typeObject - typeUint + 1) ? typeString[typeSpec->baseType - typeUint + 1] : "",
      (typeSpec->baseType - typeEnum + 1) <= (typeObject - typeEnum + 1) ? typeSpec->typeName : "",
      typeSpec->indexCount ? "a" : ""
    );
printf( "MangleTypeSpec(): mangledResult == %s; mangledLength == %u\n", mangledResult, mangledLength );

    if( (mangledLength > 0) && (mangledLength < destSize) ) {
      memcpy( destBuffer, mangledResult, mangledLength + 1 );
      return -1;
    }

    return 0;
  }

/*
 *  Enum table implementation
 */

  void FreeEnum( Enum* data ) {
  }

  int CopyEnum( Enum* dest, Enum* source ) {
    return -1;
  }

/*
 *  Field table implementation
 */

  void FreeField( Field* data ) {
  }

  int CopyField( Field* dest, Field* source ) {
    return -1;
  }

/*
 *  Parameter table implementation
 */

  void FreeParam( Param* data ) {
  }

  int CopyParam( Param* dest, Param* source ) {
    return -1;
  }

/*
 *  Identifier table implementation
 */

  void FreeIdent( Ident* data ) {
  }

  int CopyIdent( Ident* dest, Ident* source ) {
    return -1;
  }

/*
 *  Object member variable table declarations
 */

  void FreeMember( Member* data ) {
  }

  int CopyMember( Member* dest, Member* source ) {
    return -1;
  }

/*
 *  Method table implementation
 */

  void FreeMethod( Method* data ) {
    if( data ) {
      FreeParamTable( &data->params );
    }
  }

  int CopyMethod( Method* dest, Method* source ) {
    if( dest && source ) {
      dest->params = CopyParamTable(source->params);
      if( dest->params ) {
        return -1;
      }
    }

    return 0;
  }

/*
 *  Token Table implementation
 */

  void FreeTokenSym( TokenSym* data ) {
    if( data == NULL ) {
      return;
    }

    switch( data->tokenCode ) {
    case tkEnumType:
      FreeEnumTable( &data->enumTypeSym.enums );
      break;

    case tkStruct:
      FreeFieldTable( &data->structSym.fields );
      break;

    case tkUnion:
      FreeFieldTable( &data->unionSym.fields );
      break;

    case tkImport:
      FreeParamTable( &data->importSym.funcParams );
      break;

    case tkFunc:
      FreeParamTable( &data->funcSym.params );
      break;

    case tkObject:
      FreeMemberTable( &data->objectSym.members );
      break;

    case tkAbstract:
      FreeIdentTable( &data->abstractSym.baseInterfaces );
      FreeMethodTable( &data->abstractSym.methods );
      break;

    case tkInterface:
      FreeIdentTable( &data->interfaceSym.baseInterfaces );
      FreeMethodTable( &data->interfaceSym.methods );
      break;

    case tkOperator:
      FreeParamTable( &data->operatorSym.params );
      break;
    }
  }

  int CopyTokenSym( TokenSym* dest, TokenSym* source ) {
    if( dest && source ) {
      switch( source->tokenCode ) {
      case tkEnumType:
        dest->enumTypeSym.enums =
            CopyEnumTable(source->enumTypeSym.enums);
        if( dest->enumTypeSym.enums ) {
          return -1;
        }
        break;

      case tkStruct:
        dest->structSym.fields =
            CopyFieldTable(source->structSym.fields);
        if( dest->structSym.fields ) {
          return -1;
        }
        break;

      case tkUnion:
        dest->unionSym.fields =
            CopyFieldTable(source->unionSym.fields);
        if( dest->unionSym.fields ) {
          return -1;
        }
        break;

      case tkImport:
        dest->importSym.funcParams =
            CopyParamTable(source->importSym.funcParams);
        if( dest->importSym.funcParams ) {
          return -1;
        }
        break;

      case tkFunc:
        dest->funcSym.params =
            CopyParamTable(source->funcSym.params);
        if( dest->funcSym.params ) {
          return -1;
        }
        break;

      case tkObject:
        dest->objectSym.members =
            CopyMemberTable(source->objectSym.members);
        if( dest->objectSym.members ) {
          return -1;
        }
        break;

      case tkAbstract:
        dest->abstractSym.baseInterfaces =
            CopyIdentTable(source->abstractSym.baseInterfaces);
        dest->abstractSym.methods =
            CopyMethodTable(source->abstractSym.methods);
        if( dest->abstractSym.baseInterfaces &&
            dest->abstractSym.methods ) {
          return -1;
        }
        FreeIdentTable( &dest->abstractSym.baseInterfaces );
        FreeMethodTable( &dest->abstractSym.methods );
        break;

      case tkInterface:
        dest->interfaceSym.baseInterfaces =
          CopyIdentTable(source->interfaceSym.baseInterfaces);
        dest->interfaceSym.methods =
          CopyMethodTable(source->interfaceSym.methods);
        if( dest->interfaceSym.baseInterfaces &&
            dest->interfaceSym.methods) {
          return -1;
        }
        FreeIdentTable( &dest->interfaceSym.baseInterfaces );
        FreeMethodTable( &dest->interfaceSym.methods );
        break;

      case tkOperator:
        dest->operatorSym.params =
          CopyParamTable(source->operatorSym.params);
        if( dest->operatorSym.params ) {
          return -1;
        }
        break;
      }
    }

    return 0;
  }

  int DeclareConst( TokenTable* tokenTable, unsigned typeID, char* constName ) {
    TokenSym sym = {};

    if( !(tokenTable && typeID && constName) ) {
      return 0;
    }

    sym.tokenCode = tkConst;
    sym.constSym.typeID = typeID;

    return InsertTokenSym(tokenTable, constName, &sym);
  }

  int LookupConst( TokenTable* tokenTable, char* constName, ConstSym* destSym ) {
    TokenSym sym = {};

    if( !(tokenTable && constName && destSym) ) {
      return 0;
    }

    if( RetrieveTokenSym(tokenTable, constName, &sym) == 0 ) {
      return 0;
    }

    if( sym.tokenCode != tkConst ) {
      return 0;
    }

    *destSym = sym.constSym;
    return -1;
  }

  int DeclareEnumType( TokenTable* tokenTable, unsigned typeID, char* enumTypeName, EnumTable* enumTable ) {
    TokenSym sym = {};

    if( !(tokenTable && typeID && enumTypeName && enumTable) ) {
      return 0;
    }

    sym.tokenCode = tkEnumType;
    sym.enumTypeSym.typeID = typeID;
    sym.enumTypeSym.enums = enumTable;

    return InsertTokenSym(tokenTable, enumTypeName, &sym);
  }

  int LookupEnumType( TokenTable* tokenTable, char* enumTypeName, EnumTypeSym* destSym ) {
    TokenSym sym = {};

    if( !(tokenTable && enumTypeName && destSym) ) {
      return 0;
    }

    if( RetrieveTokenSym(tokenTable, enumTypeName, &sym) == 0 ) {
      return 0;
    }

    if( sym.tokenCode != tkEnumType ) {
      return 0;
    }

    *destSym = sym.enumTypeSym;
    return -1;
  }

  int DeclareStruct( TokenTable* tokenTable, char* structName, FieldTable* fieldTable ) {
    TokenSym sym = {};

    if( !(tokenTable && structName && fieldTable) ) {
      return 0;
    }

    sym.tokenCode = tkStruct;
    sym.structSym.fields = fieldTable;

    return InsertTokenSym(tokenTable, structName, &sym);
  }

  int LookupStruct( TokenTable* tokenTable, char* structName, StructSym* destSym ) {
    TokenSym sym = {};

    if( !(tokenTable && structName && destSym) ) {
      return 0;
    }

    if( RetrieveTokenSym(tokenTable, structName, &sym) == 0 ) {
      return 0;
    }

    if( sym.tokenCode != tkStruct ) {
      return 0;
    }

    *destSym = sym.structSym;
    return -1;
  }

  int DeclareUnion( TokenTable* tokenTable, char* unionName, FieldTable* fieldTable ) {
    TokenSym sym = {};

    if( !(tokenTable && unionName && fieldTable) ) {
      return 0;
    }

    sym.tokenCode = tkUnion;
    sym.unionSym.fields = fieldTable;

    return InsertTokenSym(tokenTable, unionName, &sym);
  }

  int LookupUnion( TokenTable* tokenTable, char* unionName, UnionSym* destSym ) {
    TokenSym sym = {};

    if( !(tokenTable && unionName && destSym) ) {
      return 0;
    }

    if( RetrieveTokenSym(tokenTable, unionName, &sym) == 0 ) {
      return 0;
    }

    if( sym.tokenCode != tkUnion ) {
      return 0;
    }

    *destSym = sym.unionSym;
    return -1;
  }

  int DeclareType( TokenTable* tokenTable, unsigned typeID, char* typeName ) {
    TokenSym sym = {};

    if( !(tokenTable && typeID && typeName) ) {
      return 0;
    }

    sym.tokenCode = tkType;
    sym.typeSym.typeID = typeID;

    return InsertTokenSym(tokenTable, typeName, &sym);
  }

  int LookupType( TokenTable* tokenTable, char* typeName, TypeSym* destSym ) {
    TokenSym sym = {};

    if( !(tokenTable && typeName && destSym) ) {
      return 0;
    }

    if( RetrieveTokenSym(tokenTable, typeName, &sym) == 0 ) {
      return 0;
    }

    if( sym.tokenCode != tkType ) {
      return 0;
    }

    *destSym = sym.typeSym;
    return -1;
  }

  int DeclareVar( TokenTable* tokenTable, unsigned typeID, char* varName ) {
    TokenSym sym = {};

    if( !(tokenTable && typeID && varName) ) {
      return 0;
    }

    sym.tokenCode = tkVar;
    sym.varSym.typeID = typeID;

    return InsertTokenSym(tokenTable, varName, &sym);
  }

  int LookupVar( TokenTable* tokenTable, char* varName, VarSym* destSym ) {
    TokenSym sym = {};

    if( !(tokenTable && varName && destSym) ) {
      return 0;
    }

    if( RetrieveTokenSym(tokenTable, varName, &sym) == 0 ) {
      return 0;
    }

    if( sym.tokenCode != tkVar ) {
      return 0;
    }

    *destSym = sym.varSym;
    return -1;
  }

  int DeclareFunc( TokenTable* tokenTable, unsigned typeID, char* funcName, ParamTable* paramTable ) {
    TokenSym sym = {};

    if( !(tokenTable && typeID && funcName) ) {
      return 0;
    }

    sym.tokenCode = tkFunc;
    sym.funcSym.typeID = typeID;
    sym.funcSym.params = paramTable;

    return InsertTokenSym(tokenTable, funcName, &sym);
  }

  int LookupFunc( TokenTable* tokenTable, char* funcName, FuncSym* destSym ) {
    TokenSym sym = {};

    if( !(tokenTable && funcName && destSym) ) {
      return 0;
    }

    if( RetrieveTokenSym(tokenTable, funcName, &sym) == 0 ) {
      return 0;
    }

    if( sym.tokenCode != tkFunc ) {
      return 0;
    }

    *destSym = sym.funcSym;
    return -1;
  }

  int DeclareObject( TokenTable* tokenTable, char* objectName, char* inheritsName, MemberTable* memberTable ) {
    TokenSym sym = {};

    if( !(tokenTable && objectName && inheritsName && memberTable) ) {
      return 0;
    }

    sym.tokenCode = tkObject;
    strncpy( sym.objectSym.baseObject, inheritsName, sizeof(sym.objectSym.baseObject) - 1 );
    sym.objectSym.members = memberTable;

    return InsertTokenSym(tokenTable, objectName, &sym);
  }

  int LookupObject( TokenTable* tokenTable, char* objectName, ObjectSym* destSym ) {
    TokenSym sym = {};

    if( !(tokenTable && objectName && destSym) ) {
      return 0;
    }

    if( RetrieveTokenSym(tokenTable, objectName, &sym) == 0 ) {
      return 0;
    }

    if( sym.tokenCode != tkObject ) {
      return 0;
    }

    *destSym = sym.objectSym;
    return -1;
  }

  int DeclareAbstract( TokenTable* tokenTable, char* abstractName, char* implementsName, MethodTable* methodTable )  {
    TokenSym sym = {};

    if( !(tokenTable && abstractName && implementsName && methodTable) ) {
      return 0;
    }

    sym.tokenCode = tkAbstract;
    strncpy( sym.abstractSym.objectName, implementsName, sizeof(sym.objectSym.baseObject) - 1 );
    sym.abstractSym.methods = methodTable;

    return InsertTokenSym(tokenTable, abstractName, &sym);
  }

  int LookupAbstract( TokenTable* tokenTable, char* abstractName, AbstractSym* destSym ) {
    TokenSym sym = {};

    if( !(tokenTable && abstractName && destSym) ) {
      return 0;
    }

    if( RetrieveTokenSym(tokenTable, abstractName, &sym) == 0 ) {
      return 0;
    }

    if( sym.tokenCode != tkAbstract ) {
      return 0;
    }

    *destSym = sym.abstractSym;
    return -1;
  }

  int DeclareInterface( TokenTable* tokenTable, char* interfaceName, char* implementsName, MethodTable* methodTable ) {
    TokenSym sym = {};

    if( !(tokenTable && interfaceName && implementsName && methodTable) ) {
      return 0;
    }

    sym.tokenCode = tkInterface;
    strncpy( sym.interfaceSym.objectName, implementsName, sizeof(sym.objectSym.baseObject) - 1 );
    sym.interfaceSym.methods = methodTable;

    return InsertTokenSym(tokenTable, interfaceName, &sym);
  }

  int LookupInterface( TokenTable* tokenTable, char* interfaceName, InterfaceSym* destSym ) {
    TokenSym sym = {};

    if( !(tokenTable && interfaceName && destSym) ) {
      return 0;
    }

    if( RetrieveTokenSym(tokenTable, interfaceName, &sym) == 0 ) {
      return 0;
    }

    if( sym.tokenCode != tkInterface ) {
      return 0;
    }

    *destSym = sym.interfaceSym;
    return -1;
  }

  int DeclareOperator( TokenTable* tokenTable, char* mangledName, ParamTable* paramTable ) {
    TokenSym sym = {};

    if( !(tokenTable && mangledName && paramTable) ) {
      return 0;
    }

    sym.tokenCode = tkOperator;
    sym.operatorSym.params = paramTable;

    return InsertTokenSym(tokenTable, mangledName, &sym);
  }

  int LookupOperator( TokenTable* tokenTable, char* mangledName, OperatorSym* destSym ) {
    TokenSym sym = {};

    if( !(tokenTable && mangledName && destSym) ) {
      return 0;
    }

    if( RetrieveTokenSym(tokenTable, mangledName, &sym) == 0 ) {
      return 0;
    }

    if( sym.tokenCode != tkOperator ) {
      return 0;
    }

    *destSym = sym.operatorSym;
    return -1;
  }

/* Parser implementation */

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

  /* Operator parser implementation */

  typedef struct Operator {
    char* text;
    unsigned token;
  } Operator;

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

  unsigned ReadOperator() {
    char operator[MAXLEN_OPERATOR + 1] = {};
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
      if( operLength > MAXLEN_OPERATOR ) {
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

  /* Keyword table implementation */
  typedef struct Keyword {
    char*    name;
    unsigned token;
  } Keyword;

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
printf( "ParseTypeSpec()\n" );

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
      strncpy( tempSpec.typeName, curTokenStr, sizeof(tempSpec.typeName) );
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
    tokenTable = CreateTokenTable(0);
    if( tokenTable == NULL ) {
      printf( "Unable to create token table\n" );
      exit( errorInternal );
    }

    typeTable = CreateTypeTable(0);
    if( typeTable == NULL ) {
      printf( "Unable to create type table\n" );
      exit( errorInternal );
    }

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
    char mangledName[MAXLEN_MANGLED_TYPESPEC] = {};
    TypeSpec typeSpec = {};
    unsigned keywordToken;
    unsigned curTypeID = 0;

    GetToken(); // Skip keyword type

    memset( &typeSpec, 0, sizeof(typeSpec) );
    ParseTypeSpec( &typeSpec );

    if( newTypeID == ((unsigned)-1) ) {
      printf( "[L%u,C%u] Too many types declared\n", curLine, curColumn );
      exit( errorInternal );
    }
    curTypeID = ++newTypeID;

    if( InsertTypeSpec(typeTable, curTypeID, &typeSpec) == 0 ) {
      printf( "[L%u,C%u] Internal error adding typespec to type table\n", curLine, curColumn );
      exit( errorInternal );
    }

//  int MangleTypeSpec( TypeSpec* typeSpec, char* destBuffer, size_t destSize )
    if( MangleTypeSpec(&typeSpec, mangledName, MAXLEN_MANGLED_TYPESPEC) == 0 ) {
      printf( "[L%u,C%u] Error mangling type specification\n", curLine, curColumn );
      exit( errorInternal );
    }
printf( "ParseTypeDecl(): mangledName == %s\n", mangledName );

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
    // Parse first unary operators, and/or open parentheses
    do {
      switch( curToken ) {
      case tkLParen:
printf( "ParseCondition() main loop: first open parenthesis (\n" );
        if( parenLevel == ((unsigned)-1) ) {
          printf( "[L%u,C%u] Parenthesis nested too deep\n", curLine, curColumn );
          exit( errorParenNestedTooDeep );
        }
        parenLevel++;
        GetToken(); // Skip open parenthesis (
        continue;

      case opSub:
printf( "ParseCondition() main loop: first unary -\n" );
        GetToken(); // Skip unary negate operator (-)
        continue;

      case opAdd:
printf( "ParseCondition() main loop: first unary +\n" );
        GetToken(); // Skip unary positive operator (+)
        continue;

      case unaryNot:
printf( "ParseCondition() main loop: first unary ~\n" );
        GetToken(); // Skip bitwise unary not operator (~)
        continue;

      case unaryIsNot:
printf( "ParseCondition() main loop: first unary !\n" );
        GetToken(); // Skip boolean unary not operator (!)
        continue;
      }
      break;
    } while( curToken );

      // Parse first operand
    switch( curToken ) {
    case opPostInc:
    case opPostDec:
printf( "[L%u,C%u] Operator pending implementation\n", curLine, curColumn );
exit( errorPendingImplementation );
printf( "ParseCondition() main loop: ++/-- first operand\n" );
      // Parse first pre-increment/decrement operator
      if( curToken != tkIdent ) { // Change when token table is implemented
        printf( "[L%u,C%u] Variable expected\n", curLine, curColumn );
        exit( expectedVariable );
      }
      GetToken(); // Skip pre-increment/decrement operator

    case tkIdent: // Change when token table is implemented
printf( "ParseCondition() main loop: first operand curTokenStr == %s\n", curTokenStr );
      GetToken(); // Skip identifier

      // Temporary - begin parse array dimension
      if( curToken == tkLBrace ) {
printf( "ParseCondition() main loop: first operand [\n" );
        GetToken(); // Skip open brace ([)

        switch( curToken ) { // Parse array index
        case tkIdent: // Change when token table is implemented
printf( "ParseCondition() main loop: first operand array index curTokenStr == %s\n", curTokenStr );
          GetToken(); // Skip array index identifier
          break;

        case valUint:
printf( "ParseCondition() main loop: first operand array index curTokenVal.valUint == %u\n", curTokenVal.valUint );
          GetToken(); // Skip array index value
          break;

        default:
printf( "ParseCondition() main loop: first operand array index ??? curTokenStr == %s; curToken == %u\n", curTokenStr, curToken );
          printf( "[L%u,C%u] Unsupported array index\n", curLine, curColumn );
          exit( errorUnsupportedArrayIndex );
        }

        if( curToken != tkRBrace ) {
          printf( "[L%u,C%u] Expected right brace\n", curLine, curColumn );
          exit( errorUnsupportedArrayIndex );
        }
printf( "ParseCondition() main loop: first operand ]\n" );
        GetToken(); // Skip closing brace (])
      }
      // Temporary - end parse array dimension

      // Parse first post-increment/decrement operator
      switch( curToken ) {
      case opPostInc:
      case opPostDec:
printf( "ParseCondition() main loop: first operand --/++\n" );
printf( "[L%u,C%u] Operator pending implementation\n", curLine, curColumn );
exit( errorPendingImplementation );
        GetToken(); // Skip post-increment/decrement operator
        break;
      }
      break;

    case valUint:
printf( "ParseCondition() main loop: first operand curTokenVal.valUint == %u\n", curTokenVal.valUint );
      GetToken(); // Skip uint value
      break;

    default:
printf( "ParseCondition() main loop: first operand ??? curTokenStr == %s; curToken == %u\n", curTokenStr, curToken );
      printf( "[L%u,C%u] Expected first operand variable or value\n", curLine, curColumn );
      exit( expectedOperand );
    }

    // Parse closing parenthesis
    while( curToken == tkRParen ) {
printf( "ParseCondition() main loop: first closing parenthesis (\n" );
      if( parenLevel == 0 ) {
        printf( "[L%u,C%u] Parenthesis closed ) without an opening parenthesis (\n", curLine, curColumn );
        exit( expectedRightParenthesis );
      }
      parenLevel--;
      GetToken(); // Skip close parenthesis )
    }

    do {
      // Parse next subexpression
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

        // Parse next unary
        do {
          switch( curToken ) {
          case tkLParen:
printf( "ParseCondition() main loop: next open parenthesis (\n" );
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

        // Parse next operand
        switch( curToken ) {
          case opPostInc:
          case opPostDec:
printf( "[L%u,C%u] Operator pending implementation\n", curLine, curColumn );
exit( errorPendingImplementation );
            // Parse next pre-increment/decrement operator
            if( curToken != tkIdent ) { // Change when token table is implemented
              printf( "[L%u,C%u] Variable expected\n", curLine, curColumn );
              exit( expectedVariable );
            }
            GetToken(); // Skip pre-increment/decrement operator

        case tkIdent: // Change when token table is implemented
printf( "ParseCondition() main loop: next operand curTokenStr == %s\n", curTokenStr );
          GetToken(); // Skip identifier

          // Temporary - begin parse array dimension
          if( curToken == tkLBrace ) {
printf( "ParseCondition() main loop: next operand [\n" );
            GetToken(); // Skip open brace ([)

            switch( curToken ) { // Parse array index
            case tkIdent: // Change when token table is implemented
printf( "ParseCondition() main loop: next operand array index curTokenStr == %s\n", curTokenStr );
              GetToken(); // Skip array index identifier
              break;

            case valUint:
printf( "ParseCondition() main loop: next operand array index curTokenVal.valUint == %u\n", curTokenVal.valUint );
              GetToken(); // Skip array index value
              break;

            default:
printf( "ParseCondition() main loop: next operand array index ??? curTokenStr == %s; curToken == %u\n", curTokenStr, curToken );
              printf( "[L%u,C%u] Unsupported array index\n", curLine, curColumn );
              exit( errorUnsupportedArrayIndex );
            }

            if( curToken != tkRBrace ) {
              printf( "[L%u,C%u] Expected next brace\n", curLine, curColumn );
              exit( errorUnsupportedArrayIndex );
            }
printf( "ParseCondition() main loop: next operand ]\n" );
            GetToken(); // Skip closing brace (])
          }
          // Temporary - end parse array dimension

          // Parse next post-increment/decrement operator
          switch( curToken ) {
          case opPostInc:
          case opPostDec:
printf( "ParseCondition() main loop: next operand --/++\n" );
printf( "[L%u,C%u] Operator pending implementation\n", curLine, curColumn );
exit( errorPendingImplementation );
            GetToken(); // Skip post-increment/decrement operator
            break;
          }
          if( curToken == tkRParen ) {
            // Delegate continue to RParen case
            break;
          }
          // Otherwise skip default loop break
          continue;

        case valUint:
printf( "ParseCondition() main loop: next operand curTokenVal.valUint == %u\n", curTokenVal.valUint );
          GetToken(); // Skip uint value

          if( curToken == tkRParen ) {
            // Delegate continue to RParen case
            break;
          }
          // Otherwise skip default loop break
          continue;

        default:
printf( "ParseCondition() main loop: next operand ??? curTokenStr == %s; curToken == %u\n", curTokenStr, curToken );
          printf( "[L%u,C%u] Expected next operand variable or value\n", curLine, curColumn );
          exit( expectedOperand );
        }
      }

      // Parse closing parenthesis
      if( curToken == tkRParen ) {
        while( curToken == tkRParen ) {
printf( "ParseCondition() main loop: next closing parenthesis (\n" );
          if( parenLevel == 0 ) {
            printf( "[L%u,C%u] Parenthesis closed ) without an opening parenthesis (\n", curLine, curColumn );
            exit( expectedRightParenthesis );
          }
          parenLevel--;
          GetToken(); // Skip close parenthesis )
        }
        continue;
      }

      // Default loop break indicating end of condition
    } while( curToken && ((curToken >= firstOper) && (curToken <= lastOper)) );
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
  }

  /*
   *  endif
   */
  void ParseEndIf() {
    GetToken(); // Skip keyword endif
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
        if( ParseFuncStatement() == 0 ) {
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
printf( "ParseTopLevel(): rsvdType; line == %u; column == %u\n", curLine, curColumn );
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
    fprintf( headerFile, "\n#endif\n" );
  }
