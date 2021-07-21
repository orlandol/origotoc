/* 
 * MIT License
 * 
 * Copyright (c) 2014-2021 Orlando Llanes
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef H_ORIGOTOC
#define H_ORIGOTOC

/*
 *  Version declarations
 */
#define ALPHA_RELEASE 0
#define RSVD1_RELEASE 1
#define BETA_RELEASE 2
#define RELEASE_CANDIDATE 3
#define STABLE_RELEASE 4
#define HOTFIX_RELEASE 5
#define LTS_RELEASE 6
#define FINAL_RELEASE 7

#define MAKEVER(major, minor, patch, release)\
  ((((major) & 0xFF) << 24) | (((minor) & 0xFF) << 16) |\
   (((patch) & 0x1FFF) << 3) | ((release) & 0x07))

#define MAJORVER(version) (((version) >> 24) & 0xFF)
#define MINORVER(version) (((version) >> 16) & 0xFF)
#define PATCHVER(version) (((version) >> 3) & 0x1FFF)
#define RELEASETYPE(version) ((version) & 0x7)

/*
 *  Global variables
 */

extern int argc;
extern char** argv;

/*
 *  Helper functions
 */
void FreePtr( void** ptrVar );

/*
 *  Path split functions
 */

int SplitPath( const char* fromFullPath, char** toDir,
    char** toBaseName, char** toExt );
int JoinPath( const char* fromDir, const char* fromBaseName,
    const char* fromExt, char** toFullPath );

/*
 *  OrigoToC program declarations
 */

#define ORIGOTOC_VERSION MAKEVER(0, 1, 2, ALPHA_RELEASE)
#define ORIGOTOC_VERSTRING "0.1.2 Alpha"
#define ORIGOTOC_COPYRIGHT "Copyright (C) 2014-2021 Orlando Llanes"

typedef struct OrigoOptions {
  char* sourceFileName;
  char* cFileName;
  char* hFileName;
  char* binaryFileName;
} OrigoOptions;

extern OrigoOptions options;

void PrintBanner();
void Usage();

int ParseOptions( OrigoOptions* toOptionsVar );

/*
 *  Token declarations
 */

#define IDENT_MAXLEN 32
#define IDENT_MAXINDEX (IDENT_MAXLEN - 1)

enum TokenCode {
  GeneralToken = 0,
  tkEOF,
  tkEOL,

  TopLevelKeyword = 1024,
  tlEnum,
  tlUnion,
  tlStruct,
  tlType,
  tlConst,
  tlVar,
  tlFuncDecl,
  tlImport,
  tlFunc,
  tlObject,
  tlInterface,
  tlMethod,
  tlRun,

  StatementKeyword = 2048,
  stmtBind,
  stmtElse,
  stmtElseIf,
  stmtEndFor,
  stmtEndIf,
  stmtEndWhile,
  stmtBreak,
  stmtNext,
  stmtFor,
  stmtGoto,
  stmtIf,
  stmtRepeat,
  stmtWhen,
  stmtWhile
};

/*
 *  Warning/error functions
 */

void Error( unsigned ofCode, const char* message );
void Expected( unsigned onLine, unsigned onColumn,
  const char* message );
void Unexpected( unsigned onLine, unsigned onColumn,
  const char* message );

/*
 *  Symbol table declarations
 */

typedef struct SymTable {
} SymTable;

extern SymTable symtab;

/*
 *  Lexer declarations
 */

typedef struct RetFile {
  FILE* handle;

  unsigned line;
  unsigned column;

  int runDeclared;

  char curCh;
  char nextCh;
} RetFile;

typedef struct KeywordItem {
  const char* name;
  unsigned tokenCode;
} KeywordItem;

extern const KeywordItem topLevelKeyword[];
extern const KeywordItem statementKeyword[];
extern const KeywordItem reservedWord[];

extern RetFile retFile;

int ReadChar( RetFile* fromSource );

int ReadIdentChar( RetFile* fromSource );
int ReadIdent( RetFile* fromSource, char* toIdent );
unsigned FindTopLevelKeyword( const char* identName );
int ReadTopLevelKeyword( RetFile* fromSource, unsigned* toTokenCode );

int ReadBinaryDigit( RetFile* fromSource );
int ReadOctalDigit( RetFile* fromSource );
int ReadHexDigit( RetFile* fromSource );
int ReadDecimalDigit( RetFile* fromSource );

int ReadString( RetFile* fromSource, char** toString );

int OpenRet( const char* fileName, RetFile* toSourceVar );
void CloseRet( RetFile* sourceVar );

/*
 *  C code generator declarations
 */

typedef struct CFile {
  FILE* cHandle;
  FILE* hHandle;
} CFile;

extern CFile cGen;

/*
 *  Parser declarations
 */

int SkipSpace( RetFile* fromSource );
int SkipComment( RetFile* fromSource );
void SkipNonterminals( RetFile* fromSource );

int Match( RetFile* fromSource, const char* withText );

int ParseProgram( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable );

int ParseEnum( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable );
int ParseUnion( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable );
int ParseStruct( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable );
int ParseType( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable );
int ParseConst( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable );
int ParseVar( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable );
int ParseFuncDecl( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable );
int ParseImport( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable );
int ParseFunc( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable );
int ParseObject( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable );
int ParseInterface( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable );
int ParseMethod( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable );
int ParseRun( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable );

int Parse( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable );

/*
 *  Main program declarations
 */

void Cleanup();

#endif // H_ORIGOTOC
