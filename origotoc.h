/* 
 * MIT License
 * 
 * OrigoToC 0.1.4 Alpha
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
 *  Version format Maj.Min.Patch.ReleaseType declarations
 */
#define EXPERIMENTAL_RELEASE 7
#define ALPHA_RELEASE 8
#define GAMMA_RELEASE 9
#define BETA_RELEASE 10
#define RELEASE_CANDIDATE 11
#define STABLE_RELEASE 12
#define HOTFIX_RELEASE 13
#define LTS_RELEASE 14
#define FINAL_RELEASE 15

#define MAKEVEREXT(major, minor, patch, release)\
  ((((major) & 0xFF) << 24) | (((minor) & 0xFF) << 16) |\
   (((patch) & 0x0FFF) << 4) | ((release) & 0x000F))

#define MAJORVEREXT(version) (((version) >> 24) & 0xFF)
#define MINORVEREXT(version) (((version) >> 16) & 0xFF)
#define PATCHVEREXT(version) (((version) >> 4) & 0x0FFF)
#define RELEASETYPEEXT(version) ((version) & 0x000F)

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

#define ORIGOTOC_VERSION MAKEVEREXT(0, 1, 4, ALPHA_RELEASE)
#define ORIGOTOC_VERSTRING "0.1.4 Alpha"
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
#define IDENTPAIR_MAXLEN 64
#define IDENTPAIR_MAXINDEX (IDENTPAIR_MAXLEN - 1)

#define TOKENSTR_MAXLEN 2048
#define TOKENSTR_MAXINDEX (TOKENSTR_MAXLEN - 1)

enum TokenCode {
  generalToken = (1024 * 0),
  tkEOF,
  tkEOL,
  tkIdent,

  topLevelToken = (1024 * 1),
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

  typeToken = (1024 * 2),
  ptrData,
  baseInt,
  baseInt8,
  baseInt16,
  baseInt32,
  baseInt64,
  baseUint,
  baseUint8,
  baseUint16,
  baseUint32,
  baseUint64,
  baseSize,
  baseFsize,
  baseChar,
  baseBool,
  typeEnum,
  typeUnion,
  typeStruct,
  typeFunc,
  typeObject,
  typeInterface,
  typeMethod,

  statementToken = (1024 * 3),
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

void ImplementationPending( unsigned onLine, unsigned onColumn,
  const char* message );

void DuplicateIdentifier( unsigned onLine, unsigned onColumn,
  const char* message );
void Expected( unsigned onLine, unsigned onColumn,
  const char* message );
void Unexpected( unsigned onLine, unsigned onColumn,
  const char* message );

/*
 *  Symbol table declarations
 */

typedef struct TypeSpec {
  int pointerType;
  char* name;
  unsigned baseType;
  unsigned dimCount;
} TypeSpec;

typedef struct SymTable {
  unsigned tokenCode;
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

  size_t markedPos;
  unsigned markedLine;
  unsigned markedColumn;
} RetFile;

typedef struct KeywordItem {
  const char* name;
  unsigned tokenCode;
} KeywordItem;

extern const KeywordItem baseTypeName[];
extern const KeywordItem topLevelKeyword[];
extern const KeywordItem statementKeyword[];
extern const KeywordItem reservedWord[];

extern RetFile retFile;

void MarkToken( RetFile* fromSource );
void ReturnToken( RetFile* fromSource );

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

void ParseTypeSpec( RetFile* fromSource, SymTable* usingSymTable,
  TypeSpec* toTypeSpec );

int ParseEnum( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable );

int ParseUnion( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable );

int ParseStruct( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable );

int ParseType( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable );

int ParseConst( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable );

int ParseVar( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable );

int ParseFuncDecl( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable );
int ParseImport( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable );

void ParseLocalVar( RetFile* fromSource, CFile* toCgen,
  SymTable* usingSymTable, SymTable* usingLocalTable );

int ParseStatement( RetFile* fromSource, CFile* toCgen,
  SymTable* usingSymTable, SymTable* usingLocalTable );
int ParseFuncStatement( RetFile* fromSource, CFile* toCgen,
  SymTable* usingSymTable, SymTable* usingLocalTable );

int ParseFunc( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable );

int ParseObject( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable );
int ParseInterface( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable );
int ParseMethod( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable );

void ParseRun( RetFile* fromSource, CFile* toCgen,
  SymTable* usingSymTable );

void Parse( RetFile* fromSource, CFile* toCgen, SymTable* usingSymTable );

/*
 *  Main program declarations
 */

void Cleanup();

#endif // H_ORIGOTOC
