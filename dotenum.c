
#include <stdio.h>

  typedef struct DottedEnumType {
    int field1;
    int field2;
  } DottedEnumType;

  static const DottedEnumType DottedEnum = {
    1,
    2
  };

int main( int argc, char* argv[] ) {
  int foo = DottedEnum.field1;

  printf( "foo == %i; DottedEnum.field1 == %i; DottedEnum.field2 == %i\n",
    foo, DottedEnum.field1, DottedEnum.field2 );

  return 0;
}
