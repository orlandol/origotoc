
#include "keyarray.h"

#include <stdint.h>
#include <stdio.h>

/* Placeholder declarations */

  typedef enum Token {
    tkConst = 1,
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

/* Token value declarations */

  typedef struct TokenVal {
    unsigned valType;

    union {
      unsigned valUint;
      int valInt;
      char valChar;
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

  DECLARE_STRING_KEYARRAY_INSERT( DeclareEnum, EnumTable, Enum )
  DECLARE_STRING_KEYARRAY_REMOVE( RemoveEnum, EnumTable, FreeEnum )
  DECLARE_STRING_KEYARRAY_MODIFY( ModifyEnum, EnumTable, Enum )
  DECLARE_STRING_KEYARRAY_RETRIEVE( LookupEnum, EnumTable, Enum )
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

  DECLARE_STRING_KEYARRAY_INSERT( DeclareField, FieldTable, Field )
  DECLARE_STRING_KEYARRAY_REMOVE( RemoveField, FieldTable, FreeField )
  DECLARE_STRING_KEYARRAY_MODIFY( ModifyField, FieldTable, Field )
  DECLARE_STRING_KEYARRAY_RETRIEVE( LookupField, FieldTable, Field )
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

  DECLARE_STRING_KEYARRAY_INSERT( DeclareParam, ParamTable, Param )
  DECLARE_STRING_KEYARRAY_REMOVE( RemoveParam, ParamTable, FreeParam )
  DECLARE_STRING_KEYARRAY_MODIFY( ModifyParam, ParamTable, Param )
  DECLARE_STRING_KEYARRAY_RETRIEVE( LookupParam, ParamTable, Param )
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

  DECLARE_STRING_KEYARRAY_INSERT( DeclareIdent, IdentTable, Ident )
  DECLARE_STRING_KEYARRAY_REMOVE( RemoveIdent, IdentTable, FreeIdent )
  DECLARE_STRING_KEYARRAY_MODIFY( ModifyIdent, IdentTable, Ident )
  DECLARE_STRING_KEYARRAY_RETRIEVE( LookupIdent, IdentTable, Ident )
  DECLARE_STRING_KEYARRAY_FINDINDEX( IndexOfIdent, IdentTable )

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

  DECLARE_STRING_KEYARRAY_INSERT( DeclareMember, MemberTable, Member )
  DECLARE_STRING_KEYARRAY_REMOVE( RemoveMember, MemberTable, FreeMember )
  DECLARE_STRING_KEYARRAY_MODIFY( ModifyMember, MemberTable, Member )
  DECLARE_STRING_KEYARRAY_RETRIEVE( LookupMember, MemberTable, Member )
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

  DECLARE_STRING_KEYARRAY_INSERT( DeclareMethod, MethodTable, Method )
  DECLARE_STRING_KEYARRAY_REMOVE( RemoveMethod, MethodTable, FreeMethod )
  DECLARE_STRING_KEYARRAY_MODIFY( ModifyMethod, MethodTable, Method )
  DECLARE_STRING_KEYARRAY_RETRIEVE( LookupMethod, MethodTable, Method )
  DECLARE_STRING_KEYARRAY_FINDINDEX( IndexOfMethod, MethodTable )

  DECLARE_STRING_KEYARRAY_RELEASEUNUSED( CompactMethodTable, MethodTable )
  DECLARE_STRING_KEYARRAY_COPY( CopyMethodTable, MethodTable, Method,
      CopyMethod, FreeMethod )

/* Token table declarations */

  typedef struct ConstSym { // const
     unsigned typeID;
   } ConstSym;
 
   typedef struct EnumTypeSym { // enum
     unsigned typeID;
     EnumTable* enums;
   } EnumTypeSym;
 
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

  typedef struct ImportSym { // import
    unsigned typeID;
    ParamTable* funcParams;
  } ImportSym;

  typedef struct FuncSym { // func
    unsigned typeID;
    ParamTable* params;
  } FuncSym;

  typedef struct ObjectSym { // object
    char baseObject[128];
    MemberTable* members;
  } ObjectSym;

  typedef struct AbstractSym { // abstract
    char objectName[128];
    IdentTable* baseInterfaces;
    MethodTable* methods;
  } AbstractSym;

  typedef struct InterfaceSym { // interface
    char objectName[128];
    IdentTable* baseInterfaces;
    MethodTable* methods;
  } InterfaceSym;

  typedef struct OperatorSym { // operator
    ParamTable* params;
  } OperatorSym;

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

/* Token value implementation */

/* Type table implementation */

  void FreeTypeSpec( TypeSpec* data ) {
  }

  int CopyTypeSpec( TypeSpec* dest, TypeSpec* source ) {
    return -1;
  }

  int MangleName( TypeSpec* typeSpec, char* destBuffer, size_t destSize ) {
    static const char* typeString[] = { "", "u", "i", "e", "s", "u", "o" };
    char mangledResult[256] = {};
    int mangledLength;

    if( !(typeSpec && destBuffer && destSize) ) {
      return 0;
    }

    mangledLength = snprintf( mangledResult, sizeof(mangledResult) - 1, "%s%s%s%s",
      typeSpec->ptrType == ptrData ? "p" : typeSpec->ptrType == ptrRef ? "r" : "",
      (typeSpec->baseType - typeUint + 1) <= (typeObject - typeUint + 1) ? typeString[typeSpec->baseType - typeUint + 1] : "",
      (typeSpec->baseType - typeEnum + 1) <= (typeObject - typeEnum + 1) ? typeSpec->typeName : "",
      typeSpec->indexCount ? "a" : ""
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
    return -1;
  }

/* Field table implementation */

  void FreeField( Field* data ) {
  }

  int CopyField( Field* dest, Field* source ) {
    return -1;
  }

/* Parameter table implementation */

  void FreeParam( Param* data ) {
  }

  int CopyParam( Param* dest, Param* source ) {
    return -1;
  }

/* Object member variable table declarations */

  void FreeMember( Member* data ) {
  }

  int CopyMember( Member* dest, Member* source ) {
    return -1;
  }

/* Identifier table implementation */

  void FreeIdent( Ident* data ) {
  }

  int CopyIdent( Ident* dest, Ident* source ) {
    return -1;
  }

/* Method table implementation */

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

/* Token Table implementation */

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
