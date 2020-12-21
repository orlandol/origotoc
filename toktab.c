
#include "keyarray.h"

#include <stdio.h>

/* Placeholder declarations */

  typedef enum Token {
    tkVar = 1
  } Token;

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
  DECLARE_UINT_KEYARRAY_FINDINDEX( TypeSpecIndex, TypeTable )

/* Token table declarations */
  typedef struct TokenSym {
    unsigned tokenCode;
    unsigned typeID;
  } TokenSym;

  void FreeTokenSym( TokenSym* data );

  DECLARE_STRING_KEYARRAY_TYPES( TokenTable, TokenSym )
  DECLARE_STRING_KEYARRAY_CREATE( CreateTokenTable, TokenTable )
  DECLARE_STRING_KEYARRAY_FREE( FreeTokenTable, TokenTable, FreeTokenSym )
  DECLARE_STRING_KEYARRAY_INSERT( InsertTokenSym, TokenTable, TokenSym )
  DECLARE_STRING_KEYARRAY_REMOVE( RemoveTokenSym, TokenTable, FreeTokenSym )
  DECLARE_STRING_KEYARRAY_MODIFY( ModifyTokenSym, TokenTable, TokenSym )
  DECLARE_STRING_KEYARRAY_RETRIEVE( RetrieveTokenSym, TokenTable, TokenSym )
  DECLARE_STRING_KEYARRAY_FINDINDEX( TokenSymIndex, TokenTable )

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


/* Token Table implementation */

  void FreeTokenSym( TokenSym* data ) {
  }

  int DeclareVar( TokenTable* destTable, unsigned typeID, char* varName ) {
    TokenSym varSym = {};

    if( !(destTable && typeID && varName) ) {
      return 0;
    }

    varSym.tokenCode = tkVar;
    varSym.typeID = typeID;

    if( InsertTokenSym(destTable, varName, &varSym) == 0 ) {
      return 0;
    }

    return -1;
  }
