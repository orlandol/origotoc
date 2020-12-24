
#include "keyarray.h"

#include <stdio.h>

/* Placeholder declarations */

  typedef enum Token {
    tkVar = 1
  } Token;

  typedef struct TokenVal {
    unsigned valType;
    union {
      unsigned uval;
      int ival;
    };
  } TokenVal;

/* Type table declarations */
  typedef struct TypeSpec {
    unsigned ptrType;
    unsigned baseType;
    char baseName[1024];
    size_t indexCount;
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
      } objectSym;

      struct { // abstract
      } abstractSym;

      struct { // interface
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

  int DeclareVar( TokenTable* destTable, unsigned typeID, char* varName );

/* Global variables */
  TypeTable* typeTable = NULL;
  TokenTable* tokenTable = NULL;

  unsigned newTypeID = 0;

int main( int argc, char* argv[] ) {
  typeTable = CreateTypeTable(0);
  tokenTable = CreateTokenTable(0);

  if( DeclareVar(tokenTable, newTypeID, "foo") == 0 ) {
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

/* Token Table implementation */

  void FreeTokenSym( TokenSym* data ) {
  }

  int DeclareVar( TokenTable* destTable, unsigned typeID, char* varName ) {
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
