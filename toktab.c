
#include "keyarray.h"

#include <stdint.h>
#include <stdio.h>

/* Placeholder declarations */

  typedef enum Token {
    tkVar = 1,
    tkConst,
    tkEnum,
    tkStruct,
    tkUnion,
    tkFunc,

    valUint = 100,
    valInt,
    valStruct,
    valUnion,

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
      struct {
        uint8_t* contents;
        size_t size;
      } dataval;
    };
  } TokenVal;

/* Type table declarations */
  typedef struct TypeSpec {
    unsigned ptrType;
    unsigned baseType;
    unsigned baseTypeID;
    size_t indexCount;
    size_t typeSize;
  } TypeSpec;

  void FreeTypeSpec( TypeSpec* data );

  DECLARE_UINT_KEYARRAY_TYPES( TypeTable, TypeSpec )

  DECLARE_UINT_KEYARRAY_CREATE( CreateTypeTable, TypeTable )
  DECLARE_UINT_KEYARRAY_FREE( FreeTypeTable, TypeTable, FreeTypeSpec )

  DECLARE_UINT_KEYARRAY_INSERT( InsertTypeSpec, TypeTable, TypeSpec )
  DECLARE_UINT_KEYARRAY_REMOVE( RemoveTypeSpec, TypeTable, FreeTypeSpec )
  DECLARE_UINT_KEYARRAY_MODIFY( ModifyTypeSpec, TypeTable, TypeSpec )
  DECLARE_UINT_KEYARRAY_RETRIEVE( RetrieveTypeSpec, TypeTable, TypeSpec )
  DECLARE_UINT_KEYARRAY_FINDINDEX( IndexOfTypeSpec, TypeTable )

/* Enum field table declarations */

  typedef struct EnumField {
    unsigned typeID;
    TokenVal value;
  } EnumField;

  void FreeEnumField( EnumField* data );

  DECLARE_STRING_KEYARRAY_TYPES( EnumFieldTable, EnumField )

  DECLARE_STRING_KEYARRAY_CREATE( CreateEnumTable, EnumFieldTable )
  DECLARE_STRING_KEYARRAY_FREE( FreeEnumTable, EnumFieldTable, FreeEnumField )

  DECLARE_STRING_KEYARRAY_INSERT( InsertEnumField, EnumFieldTable, EnumField )
  DECLARE_STRING_KEYARRAY_REMOVE( RemoveEnumField, EnumFieldTable, FreeEnumField )
  DECLARE_STRING_KEYARRAY_MODIFY( ModifyEnumField, EnumFieldTable, EnumField )
  DECLARE_STRING_KEYARRAY_RETRIEVE( RetrieveEnumField, EnumFieldTable, EnumField )
  DECLARE_STRING_KEYARRAY_FINDINDEX( IndexOfEnumField, EnumFieldTable )

/* Struct field table declarations */

  typedef struct StructField {
    unsigned typeID;
    TokenVal value;
  } StructField;

  void FreeStructField( StructField* data );

  DECLARE_STRING_KEYARRAY_TYPES( StructFieldTable, StructField )

  DECLARE_STRING_KEYARRAY_CREATE( CreateStructTable, StructFieldTable )
  DECLARE_STRING_KEYARRAY_FREE( FreeStructTable, StructFieldTable, FreeStructField )

  DECLARE_STRING_KEYARRAY_INSERT( InsertStructField, StructFieldTable, StructField )
  DECLARE_STRING_KEYARRAY_REMOVE( RemoveStructField, StructFieldTable, FreeStructField )
  DECLARE_STRING_KEYARRAY_MODIFY( ModifyStructField, StructFieldTable, StructField )
  DECLARE_STRING_KEYARRAY_RETRIEVE( RetrieveStructField, StructFieldTable, StructField )
  DECLARE_STRING_KEYARRAY_FINDINDEX( IndexOfStructField, StructFieldTable )

/* Function parameter table declarations */

  typedef struct FuncParam {
    unsigned typeID;
    unsigned paramIndex;
  } FuncParam;

  void FreeFuncParam( FuncParam* data );

  DECLARE_STRING_KEYARRAY_TYPES( FuncParamTable, FuncParam )

  DECLARE_STRING_KEYARRAY_CREATE( CreateParamTable, FuncParamTable )
  DECLARE_STRING_KEYARRAY_FREE( FreeFuncParamTable, FuncParamTable, FreeFuncParam )

  DECLARE_STRING_KEYARRAY_INSERT( InsertFuncParam, FuncParamTable, FuncParam )
  DECLARE_STRING_KEYARRAY_REMOVE( RemoveFuncParam, FuncParamTable, FreeFuncParam )
  DECLARE_STRING_KEYARRAY_MODIFY( ModifyFuncParam, FuncParamTable, FuncParam )
  DECLARE_STRING_KEYARRAY_RETRIEVE( RetrieveFuncParam, FuncParamTable, FuncParam )
  DECLARE_STRING_KEYARRAY_FINDINDEX( IndexOfFuncParam, FuncParamTable )

/* Identifier name table declarations */

  typedef struct IdentName {
    unsigned nameIndex;
  } IdentName;

  void FreeIdentName( IdentName* data );

  DECLARE_STRING_KEYARRAY_TYPES( IdentTable, IdentName )

  DECLARE_STRING_KEYARRAY_CREATE( CreateIdentTable, IdentTable )
  DECLARE_STRING_KEYARRAY_FREE( FreeIdentTable, IdentTable, FreeIdentName )

  DECLARE_STRING_KEYARRAY_INSERT( InsertIdentName, IdentTable, IdentName )
  DECLARE_STRING_KEYARRAY_REMOVE( RemoveIdentName, IdentTable, FreeIdentName )
  DECLARE_STRING_KEYARRAY_MODIFY( ModifyIdentName, IdentTable, IdentName )
  DECLARE_STRING_KEYARRAY_RETRIEVE( RetrieveIdentName, IdentTable, IdentName )
  DECLARE_STRING_KEYARRAY_FINDINDEX( IndexOfIdentName, IdentTable )

/* Object member variable table declarations */

  typedef struct MemberVar {
    unsigned typeID;
    unsigned accessType;
    unsigned memberIndex;
    TokenVal value;
  } MemberVar;

  void FreeMemberVar( MemberVar* data );

  DECLARE_STRING_KEYARRAY_TYPES( MemberTable, MemberVar )

  DECLARE_STRING_KEYARRAY_CREATE( CreateMemberTable, MemberTable )
  DECLARE_STRING_KEYARRAY_FREE( FreeMemberTable, MemberTable, FreeMemberVar )

  DECLARE_STRING_KEYARRAY_INSERT( InsertMemberVar, MemberTable, MemberVar )
  DECLARE_STRING_KEYARRAY_REMOVE( RemoveMemberVar, MemberTable, FreeMemberVar )
  DECLARE_STRING_KEYARRAY_MODIFY( ModifyMemberVar, MemberTable, MemberVar )
  DECLARE_STRING_KEYARRAY_RETRIEVE( RetrieveMemberVar, MemberTable, MemberVar )
  DECLARE_STRING_KEYARRAY_FINDINDEX( IndexOfMemberVar, MemberTable )

/* Method table declarations */

  typedef struct InterfaceMethod {
    unsigned typeID;
    unsigned methodIndex;
    FuncParamTable* params;
  } InterfaceMethod;

  void FreeInterfaceMethod( InterfaceMethod* data );

  DECLARE_STRING_KEYARRAY_TYPES( MethodTable, InterfaceMethod )

  DECLARE_STRING_KEYARRAY_CREATE( CreateMethodTable, MethodTable )
  DECLARE_STRING_KEYARRAY_FREE( FreeMethodTable, MethodTable, FreeInterfaceMethod )

  DECLARE_STRING_KEYARRAY_INSERT( InsertInterfaceMethod, MethodTable, InterfaceMethod )
  DECLARE_STRING_KEYARRAY_REMOVE( RemoveInterfaceMethod, MethodTable, FreeInterfaceMethod )
  DECLARE_STRING_KEYARRAY_MODIFY( ModifyInterfaceMethod, MethodTable, InterfaceMethod )
  DECLARE_STRING_KEYARRAY_RETRIEVE( RetrieveInterfaceMethod, MethodTable, InterfaceMethod )
  DECLARE_STRING_KEYARRAY_FINDINDEX( IndexOfInterfaceMethod, MethodTable )

/* Token table declarations */

  typedef struct TokenSym {
    unsigned tokenCode;

    union {
      struct { // const
        unsigned typeID;
        TokenVal value;
      } constSym;

      struct { // enum
        unsigned typeID;
        EnumFieldTable* fields;
      } enumSym;

      struct { // struct
        StructFieldTable* fields;
      } structSym;

      struct { // union
        StructFieldTable* fields;
      } unionSym;

      struct { // type
        unsigned typeID;
        TokenVal defaultValue;
      } typeSym;

      struct { // var
        unsigned typeID;
        TokenVal initialValue;
      } varSym;

      struct { // import
        unsigned typeID;
        FuncParamTable* funcParams;
      } importSym;

      struct { // func
        unsigned typeID;
        FuncParamTable* funcParams;
      } funcSym;

      struct { // object
        char baseObject[128];
        MemberTable* members;
      } objectSym;

      struct { // abstract
        char objectName[128];
        IdentTable* baseInterfaces;
        MethodTable* methods;
      } abstractSym;

      struct { // interface
        char objectName[128];
        IdentTable* baseInterfaces;
        MethodTable* methods;
      } interfaceSym;

      struct { // operator
      } operatorSym;
    };
  } TokenSym;

  void FreeTokenSym( TokenSym* data );

  DECLARE_STRING_KEYARRAY_TYPES( TokenTable, TokenSym )

  DECLARE_STRING_KEYARRAY_CREATE( CreateTokenTable, TokenTable )
  DECLARE_STRING_KEYARRAY_FREE( FreeTokenTable, TokenTable, FreeTokenSym )

  DECLARE_STRING_KEYARRAY_INSERT( InsertTokenSym, TokenTable, TokenSym )
  DECLARE_STRING_KEYARRAY_REMOVE( RemoveTokenSym, TokenTable, FreeTokenSym )

  DECLARE_STRING_KEYARRAY_MODIFY( ModifyTokenSym, TokenTable, TokenSym )
  DECLARE_STRING_KEYARRAY_RETRIEVE( RetrieveTokenSym, TokenTable, TokenSym )
  DECLARE_STRING_KEYARRAY_FINDINDEX( IndexOfTokenSym, TokenTable )

  int DeclareVar( TokenTable* destTable,
      unsigned typeID, char* varName, TokenVal* initialVal );

/* Global variables */
  TypeTable* typeTable = NULL;
  TokenTable* tokenTable = NULL;

  unsigned newTypeID = 0;

  void PrintMangledName( TypeSpec* typeSpec, char* baseTypeName ) {
    const char* typeString[] = {
      "",
      "u",
      "i",
      "e",
      "s",
      "u",
      "o"
    };
    char indexHexStr[8] = {};

    if( typeSpec == NULL ) {
      printf( "typeSpec parameter is NULL\n" );
      exit(100);
    }

    sprintf( indexHexStr, "%0.8x", typeSpec->indexCount );

    printf( "%s%s%s%s",
      typeSpec->ptrType == ptrData ? "p" : typeSpec->ptrType == ptrRef ? "r" : "",
      (typeSpec->baseType - typeUint + 1) <= (typeObject - typeUint + 1) ? typeString[typeSpec->baseType - typeUint + 1] : "",
      ((typeSpec->baseType - typeEnum + 1) <= (typeObject - typeEnum + 1)) && baseTypeName ? baseTypeName : "",
      typeSpec->indexCount ? indexHexStr : ""
    );
  }

int main( int argc, char* argv[] ) {
  typeTable = CreateTypeTable(0);
  tokenTable = CreateTokenTable(0);

  TypeSpec typeSpec = {ptrData, typeObject, 0, 0, 16};
  PrintMangledName( &typeSpec, "Foo" );
  printf( "_Assign_" );
  PrintMangledName( &typeSpec, "Foo" );
  printf( "\n" );

  if( DeclareVar(tokenTable, newTypeID, "foo", NULL) == 0 ) {
    printf( "Unable to add var 'foo'\n" );
    exit(1);
  }

  FreeTypeTable( &typeTable );
  FreeTokenTable( &tokenTable );

  return 0;
}

/* Type table implementation */

  void FreeTypeSpec( TypeSpec* data ) {
  }

/* Enum field table implementation */

  void FreeEnumField( EnumField* data ) {
  }

/* Struct field table implementation */

  void FreeStructField( StructField* data ) {
  }

/* Function parameter table implementation */

  void FreeFuncParam( FuncParam* data ) {
  }

/* Object member variable table declarations */

  void FreeMemberVar( MemberVar* data ) {
  }

/* Identifier name table implementation */

  void FreeIdentName( IdentName* data ) {
  }

/* Method table implementation */

  void FreeInterfaceMethod( InterfaceMethod* data ) {
  }

/* Token Table implementation */

  void FreeTokenSym( TokenSym* data ) {
  }

  int DeclareVar( TokenTable* destTable,
      unsigned typeID, char* varName, TokenVal* initialVal ) {
    TokenSym sym = {};

    if( !(destTable && typeID && varName) ) {
      return 0;
    }

    sym.tokenCode = tkVar;
    sym.varSym.typeID = typeID;

    if( InsertTokenSym(destTable, varName, &sym) == 0 ) {
      return 0;
    }

    return -1;
  }
