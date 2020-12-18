
#include <stdio.h>
#include <stdlib.h>

  typedef enum Token {
    tkUnary = 1,
    tkOpenParen,
    tkValue,
    tkCloseParen,
    tkOperator
  } Token;

  unsigned testExpr[] = {
    tkUnary, tkUnary, tkValue,
    tkOperator, tkUnary, tkOpenParen, tkUnary, tkValue, tkCloseParen
  };
  size_t testIndex = 0;
  size_t testIndexMax = sizeof(testExpr) / sizeof(testExpr[0]);

  unsigned token = 0;

  unsigned GetToken() {
    token = 0;
    if( testIndex >= testIndexMax ) {
      return 0;
    }
    token = testExpr[testIndex++];

    return token;
  }

  void ParseCondition() {
    unsigned parenLevel = 0;

    while( (token == tkUnary) || (token == tkOpenParen) ) {
      // Parse first unary and value...
      if( token == tkOpenParen ) {
        if( parenLevel == ((unsigned)-1) ) {
          printf( "Parenthesis nested too deep\n" );
          exit(1);
        }
        parenLevel++;

        putchar('(');
        GetToken(); // Skip Open paren
        continue;
      }

      putchar('u');
      GetToken(); // Skip Unary
    }

    if( token != tkValue ) {
      printf( "Expected value\n" );
      exit(2);
    }
    putchar('V');
    GetToken(); // Skip value

    while( token == tkCloseParen ) {
      if( parenLevel == 0 ) {
        printf( "Parenthesis closed without an opening parenthesis\n" );
        exit(3);
      }
      parenLevel--;
      putchar(')');
      GetToken(); // Skip )
    }

    // ...to make the operator the main main loop's condition
    while( token == tkOperator ) {
      putchar('o');
      GetToken(); // Skip operator

      while( (token == tkUnary) || (token == tkOpenParen) ) {
        // Parse next unary...
        if( token == tkOpenParen ) {
          if( parenLevel == ((unsigned)-1) ) {
            printf( "Parenthesis nested too deep\n" );
            exit(4);
          }
          parenLevel++;

          putchar('(');
          GetToken(); // Skip Open paren
          continue;
        }

        putchar('u');
        GetToken(); // Skip Unary
      }

      // ...and next value
      if( token != tkValue ) {
        printf( "Expected value\n" );
        exit(5);
      }
      putchar('V');
      GetToken(); // Skip value

      while( token == tkCloseParen ) {
        if( parenLevel == 0 ) {
          printf( "Parenthesis closed without an opening parenthesis\n" );
          exit(6);
        }
        parenLevel--;
        putchar(')');
        GetToken(); // Skip )
      }
    }

    if( parenLevel ) {
      printf( "Expression still open because of mismatched parenthesis\n" );
      exit(7);
    }
  }

int main( int argc, char* argv[] ) {
  GetToken();

  ParseCondition();

  return 0;
}
