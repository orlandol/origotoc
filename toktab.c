
#include "keyarray.h"

#include <stdint.h>
#include <stdio.h>

/* Placeholder declarations */

  typedef enum Token {
    tkConst = 1,
    tkEnum,
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

    valUint = 100,
    valInt,
    valStruct,
    valUnion,
    valData,

    ptrData = 200,
    ptrRef,

    typeUint = 300,
    typeInt,
    typeEnum,
    typeStruct,
    typeUnion,
    typeObject
  } Token;

  typedef enum MemberAccessType {
    accessImmutable = 0,
      defaultAccess = accessImmutable,
    accessMutable = 1,
    accessInternal = 2
  } MemberAccessType;

  typedef struct TokenVal {
    unsigned valType;
    union {
      unsigned uval;
      int ival;
    };
  } TokenVal;

/* Type table declarations */

  typedef struct TypeSpec {
    char typeName[64];
    unsigned ptrType;
    unsigned baseType;
    size_t indexCount;
  } TypeSpec;

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

  void FreeTokenVal( TokenVal* tokenVal );
  int CopyTokenVal( TokenVal* dest, TokenVal* source );

  int MangleName( TypeSpec* typeSpec, char* destBuffer, size_t destSize );

/* Enum table declarations */

  typedef struct Enum {
    unsigned typeID;
    TokenVal value;
  } Enum;

  void FreeEnum( Enum* data );
  int CopyEnum( Enum* dest, Enum* source );

  DECLARE_STRING_KEYARRAY_TYPES( EnumTable, Enum )

  DECLARE_STRING_KEYARRAY_CREATE( CreateEnumTable, EnumTable )
  DECLARE_STRING_KEYARRAY_FREE( FreeEnumTable, EnumTable, FreeEnum )

  DECLARE_STRING_KEYARRAY_INSERT( InsertEnum, EnumTable, Enum )
  DECLARE_STRING_KEYARRAY_REMOVE( RemoveEnum, EnumTable, FreeEnum )
  DECLARE_STRING_KEYARRAY_MODIFY( ModifyEnum, EnumTable, Enum )
  DECLARE_STRING_KEYARRAY_RETRIEVE( RetrieveEnum, EnumTable, Enum )
  DECLARE_STRING_KEYARRAY_FINDINDEX( IndexOfEnum, EnumTable )

  DECLARE_STRING_KEYARRAY_RELEASEUNUSED( CompactEnumTable, EnumTable )
  DECLARE_STRING_KEYARRAY_COPY( CopyEnumTable, EnumTable, Enum,
      CopyEnum, FreeEnum )

/* Field table declarations */

  typedef struct Field {
    unsigned typeID;
    TokenVal value;
  } Field;

  void FreeField( Field* data );
  int CopyField( Field* dest, Field* source );

  DECLARE_STRING_KEYARRAY_TYPES( FieldTable, Field )

  DECLARE_STRING_KEYARRAY_CREATE( CreateFieldTable, FieldTable )
  DECLARE_STRING_KEYARRAY_FREE( FreeFieldTable, FieldTable, FreeField )

  DECLARE_STRING_KEYARRAY_INSERT( InsertField, FieldTable, Field )
  DECLARE_STRING_KEYARRAY_REMOVE( RemoveField, FieldTable, FreeField )
  DECLARE_STRING_KEYARRAY_MODIFY( ModifyField, FieldTable, Field )
  DECLARE_STRING_KEYARRAY_RETRIEVE( RetrieveField, FieldTable, Field )
  DECLARE_STRING_KEYARRAY_FINDINDEX( IndexOfField, FieldTable )

  DECLARE_STRING_KEYARRAY_RELEASEUNUSED( CompactFieldTable, FieldTable )
  DECLARE_STRING_KEYARRAY_COPY( CopyFieldTable, FieldTable, Field,
      CopyField, FreeField )

/* Parameter table declarations */

  typedef struct Param {
    unsigned typeID;
    unsigned index;
  } Param;

  void FreeParam( Param* data );
  int CopyParam( Param* dest, Param* source );

  DECLARE_STRING_KEYARRAY_TYPES( ParamTable, Param )

  DECLARE_STRING_KEYARRAY_CREATE( CreateParamTable, ParamTable )
  DECLARE_STRING_KEYARRAY_FREE( FreeParamTable, ParamTable, FreeParam )

  DECLARE_STRING_KEYARRAY_INSERT( InsertParam, ParamTable, Param )
  DECLARE_STRING_KEYARRAY_REMOVE( RemoveParam, ParamTable, FreeParam )
  DECLARE_STRING_KEYARRAY_MODIFY( ModifyParam, ParamTable, Param )
  DECLARE_STRING_KEYARRAY_RETRIEVE( RetrieveParam, ParamTable, Param )
  DECLARE_STRING_KEYARRAY_FINDINDEX( IndexOfParam, ParamTable )

  DECLARE_STRING_KEYARRAY_RELEASEUNUSED( CompactParamTable, ParamTable )
  DECLARE_STRING_KEYARRAY_COPY( CopyParamTable, ParamTable, Param,
      CopyParam, FreeParam )

/* Identifier table declarations */

  typedef struct Ident {
    unsigned index;
  } Ident;

  void FreeIdent( Ident* data );
  int CopyIdent( Ident* dest, Ident* source );

  DECLARE_STRING_KEYARRAY_TYPES( IdentTable, Ident )

  DECLARE_STRING_KEYARRAY_CREATE( CreateIdentTable, IdentTable )
  DECLARE_STRING_KEYARRAY_FREE( FreeIdentTable, IdentTable, FreeIdent )

  DECLARE_STRING_KEYARRAY_INSERT( InsertIdentName, IdentTable, Ident )
  DECLARE_STRING_KEYARRAY_REMOVE( RemoveIdentName, IdentTable, FreeIdent )
  DECLARE_STRING_KEYARRAY_MODIFY( ModifyIdentName, IdentTable, Ident )
  DECLARE_STRING_KEYARRAY_RETRIEVE( RetrieveIdentName, IdentTable, Ident )
  DECLARE_STRING_KEYARRAY_FINDINDEX( IndexOfIdentName, IdentTable )

  DECLARE_STRING_KEYARRAY_RELEASEUNUSED( CompactIdentTable, IdentTable )
  DECLARE_STRING_KEYARRAY_COPY( CopyIdentTable, IdentTable, Ident,
      CopyIdent, FreeIdent )

/* Object member variable table declarations */

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

  DECLARE_STRING_KEYARRAY_INSERT( InsertMember, MemberTable, Member )
  DECLARE_STRING_KEYARRAY_REMOVE( RemoveMember, MemberTable, FreeMember )
  DECLARE_STRING_KEYARRAY_MODIFY( ModifyMember, MemberTable, Member )
  DECLARE_STRING_KEYARRAY_RETRIEVE( RetrieveMember, MemberTable, Member )
  DECLARE_STRING_KEYARRAY_FINDINDEX( IndexOfMember, MemberTable )

  DECLARE_STRING_KEYARRAY_RELEASEUNUSED( CompactMemberTable, MemberTable )
  DECLARE_STRING_KEYARRAY_COPY( CopyMemberTable, MemberTable, Member,
      CopyMember, FreeMember )

/* Method table declarations */

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

  DECLARE_STRING_KEYARRAY_INSERT( InsertMethod, MethodTable, Method )
  DECLARE_STRING_KEYARRAY_REMOVE( RemoveMethod, MethodTable, FreeMethod )
  DECLARE_STRING_KEYARRAY_MODIFY( ModifyMethod, MethodTable, Method )
  DECLARE_STRING_KEYARRAY_RETRIEVE( RetrieveMethod, MethodTable, Method )
  DECLARE_STRING_KEYARRAY_FINDINDEX( IndexOfMethod, MethodTable )

  DECLARE_STRING_KEYARRAY_RELEASEUNUSED( CompactMethodTable, MethodTable )
  DECLARE_STRING_KEYARRAY_COPY( CopyMethodTable, MethodTable, Method,
      CopyMethod, FreeMethod )

/* Token table declarations */

  typedef struct ConstSym { // const
     unsigned typeID;
   } ConstSym;
 
   typedef struct EnumSym { // enum
     unsigned typeID;
     FieldTable* fields;
   } EnumSym;
 
   typedef struct StructSym { // struct
     FieldTable* fields;
   } StructSym;
 
   typedef struct UnionSym { // union
     FieldTable* fields;
   } UnionSym;
 
  typedef struct TypeSym { // type
    unsigned typeID;
  } TypeSym;

  typedef struct VarSym { // var
    unsigned typeID;
  } VarSym;

  typedef struct { // import
    unsigned typeID;
    ParamTable* funcParams;
  } ImportSym;

  typedef struct { // func
    unsigned typeID;
    ParamTable* funcParams;
  } FuncSym;

  typedef struct { // object
    char baseObject[128];
    MemberTable* members;
  } ObjectSym;

  typedef struct { // abstract
    char objectName[128];
    IdentTable* baseInterfaces;
    MethodTable* methods;
  } AbstractSym;

  typedef struct { // interface
    char objectName[128];
    IdentTable* baseInterfaces;
    MethodTable* methods;
  } InterfaceSym;

  typedef struct { // operator
    ParamTable* operParams;
  } OperatorSym;

  typedef struct TokenSym {
    unsigned tokenCode;

    union {
      ConstSym constSym;
      EnumSym enumSym;
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
  int LookupEnumType( TokenTable* tokenTable, char* enumTypeName, EnumSym* destSym );

  int DeclareStruct( TokenTable* tokenTable, char* structName, FieldTable* fieldTable );
  int LookupStruct( TokenTable* tokenTable, char* structName, StructSym* destSym );

  int DeclareUnion( TokenTable* tokenTable, char* unionName, FieldTable* fieldTable );
  int LookupUnion( TokenTable* tokenTable, char* unionName, UnionSym* destSym );

  int DeclareType( TokenTable* tokenTable, unsigned typeID, char* testName );
  int LookupType( TokenTable* tokenTable, char* testName, TypeSym* destSym );

  int DeclareVar( TokenTable* tokenTable, unsigned typeID, char* varName );
  int LookupVar( TokenTable* tokenTable, char* varName, VarSym* destSym );

  int DeclareFunc( TokenTable* tokenTable, unsigned typeID, char* funcName, ParamTable* paramTable );
  int LookupFunc( TokenTable* tokenTable, char* funcName, FuncSym* destSym );

  int DeclareObject( TokenTable* tokenTable, char* objectName, char* inheritsName, MemberTable* memberTable );
  int LookupObject( TokenTable* tokenTable, char* objectName, ObjectSym* objectSym );

  int DeclareAbstract( TokenTable* tokenTable, char* abstractName, char* implementsName, MethodTable* methodTable );
  int LookupAbstract( TokenTable* tokenTable, char* abstractName, AbstractSym* destSym );

  int DeclareInterface( TokenTable* tokenTable, char* interfaceName, char* implementsName, MethodTable* methodTable );
  int LookupInterface( TokenTable* tokenTable, char* interfaceName, InterfaceSym* destSym );

  int DeclareOperator( TokenTable* tokenTable, unsigned leftTypeID, unsigned operatorToken, unsigned rightTypeID, ParamTable* paramTable );
  int LookupOperator( TokenTable* tokenTable, unsigned leftTypeID, unsigned operatorToken, unsigned rightTypeID, OperatorSym* destSym );

/* Global variables */

  TypeTable* typeTable = NULL;
  TokenTable* tokenTable = NULL;

  unsigned newTypeID = 1111;

int main( int argc, char* argv[] ) {
  typeTable = CreateTypeTable(0);
  tokenTable = CreateTokenTable(0);

  TypeSpec typeSpec = {"Foo", ptrData, typeStruct, 1000};
  char fooMangled[2048] = {};
  MangleName( &typeSpec, fooMangled, sizeof(fooMangled) - 1 );
  printf( "%s\n", fooMangled );

  if( InsertTypeSpec(typeTable, newTypeID, &typeSpec) == 0 ) {
    printf( "Unable to add typeIP [%u]\n", newTypeID );
    exit(1);
  }

  if( DeclareVar(tokenTable, newTypeID, "foo") == 0 ) {
    printf( "Unable to add var 'foo'\n" );
    exit(1);
  }

  VarSym varSym = {};
  if( LookupVar(tokenTable, "foo", &varSym) == 0 ) {
    printf( "Unable to look up symbol 'foo'\n" );
    exit(2);
  }
  printf( "'foo'.typeID == %u\n", varSym.typeID );

  FreeTypeTable( &typeTable );
  FreeTokenTable( &tokenTable );

  return 0;
}

/* Type table implementation */

  void FreeTypeSpec( TypeSpec* data ) {
  }

  int CopyTypeSpec( TypeSpec* dest, TypeSpec* source ) {
    return 0;
  }

  void FreeTokenVal( TokenVal* tokenVal ) {
  }

  int CopyTokenVal( TokenVal* dest, TokenVal* source ) {
    return 0;
  }

  int MangleName( TypeSpec* typeSpec, char* destBuffer, size_t destSize ) {
    static const char* typeString[] = { "", "u", "i", "e", "s", "u", "o" };
    char indexHexStr[16] = {};
    char mangledResult[256] = {};
    int mangledLength;

    if( !(typeSpec && destBuffer && destSize) ) {
      return 0;
    }

    snprintf( indexHexStr, sizeof(indexHexStr) - 1, "a%0.8x", typeSpec->indexCount );

    mangledLength = snprintf( mangledResult, sizeof(mangledResult) - 1, "%s%s%s%s",
      typeSpec->ptrType == ptrData ? "p" : typeSpec->ptrType == ptrRef ? "r" : "",
      (typeSpec->baseType - typeUint + 1) <= (typeObject - typeUint + 1) ? typeString[typeSpec->baseType - typeUint + 1] : "",
      (typeSpec->baseType - typeEnum + 1) <= (typeObject - typeEnum + 1) ? typeSpec->typeName : "",
      typeSpec->indexCount ? indexHexStr : ""
    );

    if( (mangledLength > 0) && (mangledLength < destSize) ) {
      memcpy( destBuffer, mangledResult, mangledLength + 1 );
      return -1;
    }

    return 0;
  }

/* Enum table implementation */

  void FreeEnum( Enum* data ) {
  }

  int CopyEnum( Enum* dest, Enum* source ) {
    return 0;
  }

/* Field table implementation */

  void FreeField( Field* data ) {
  }

  int CopyField( Field* dest, Field* source ) {
    return 0;
  }

/* Parameter table implementation */

  void FreeParam( Param* data ) {
  }

  int CopyParam( Param* dest, Param* source ) {
    return 0;
  }

/* Object member variable table declarations */

  void FreeMember( Member* data ) {
  }

  int CopyMember( Member* dest, Member* source ) {
    return 0;
  }

/* Identifier table implementation */

  void FreeIdent( Ident* data ) {
  }

  int CopyIdent( Ident* dest, Ident* source ) {
    return 0;
  }

/* Method table implementation */

  void FreeMethod( Method* data ) {
    if( data ) {
      FreeParamTable( &data->params );
    }
  }

  int CopyMethod( Method* dest, Method* source ) {
    return 0;
  }

/* Token Table implementation */

  void FreeTokenSym( TokenSym* data ) {
    if( data == NULL ) {
      return;
    }

    switch( data->tokenCode ) {
    case tkEnum:
      FreeEnumTable( &data->enumSym.fields );
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
      FreeParamTable( &data->funcSym.funcParams );
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
      FreeParamTable( &data->operatorSym.operParams );
      break;
    }
  }

  int CopyTokenSym( TokenSym* dest, TokenSym* source ) {
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
    return 0;
  }

  int LookupEnumType( TokenTable* tokenTable, char* enumTypeName, EnumSym* destSym ) {
    return 0;
  }

  int DeclareStruct( TokenTable* tokenTable, char* structName, FieldTable* fieldTable ) {
    return 0;
  }

  int LookupStruct( TokenTable* tokenTable, char* structName, StructSym* destSym ) {
    return 0;
  }

  int DeclareUnion( TokenTable* tokenTable, char* unionName, FieldTable* fieldTable ) {
    return 0;
  }

  int LookupUnion( TokenTable* tokenTable, char* unionName, UnionSym* destSym ) {
    return 0;
  }

  int DeclareType( TokenTable* tokenTable, unsigned typeID, char* testName ) {
    return 0;
  }

  int LookupType( TokenTable* tokenTable, char* testName, TypeSym* destSym ) {
    return 0;
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
    return 0;
  }

  int LookupFunc( TokenTable* tokenTable, char* funcName, FuncSym* destSym ) {
    return 0;
  }

  int DeclareObject( TokenTable* tokenTable, char* objectName, char* inheritsName, MemberTable* memberTable ) {
    return 0;
  }

  int LookupObject( TokenTable* tokenTable, char* objectName, ObjectSym* objectSym ) {
    return 0;
  }

  int DeclareAbstract( TokenTable* tokenTable, char* abstractName, char* implementsName, MethodTable* methodTable )  {
    return 0;
  }

  int LookupAbstract( TokenTable* tokenTable, char* abstractName, AbstractSym* destSym ) {
    return 0;
  }

  int DeclareInterface( TokenTable* tokenTable, char* interfaceName, char* implementsName, MethodTable* methodTable ) {
    return 0;
  }

  int LookupInterface( TokenTable* tokenTable, char* interfaceName, InterfaceSym* destSym ) {
    return 0;
  }

  int DeclareOperator( TokenTable* tokenTable, unsigned leftTypeID, unsigned operatorToken, unsigned rightTypeID, ParamTable* paramTable ) {
    return 0;
  }

  int LookupOperator( TokenTable* tokenTable, unsigned leftTypeID, unsigned operatorToken, unsigned rightTypeID, OperatorSym* destSym ) {
    return 0;
  }
