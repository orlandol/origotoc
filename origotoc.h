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
void FreePtr( void** ptr );

/*
 *  Path split functions
 */

int SplitPath( const char* fullPath, char** outDir,
    char** outBaseName, char** outExt );
int JoinPath( const char* inDir, const char* inBaseName,
    const char* inExt, char** outPath );

/*
 *  OrigoToC program declarations
 */

#define ORIGOTOC_VERSION MAKEVER(0, 1, 1, ALPHA_RELEASE)
#define ORIGOTOC_VERSTRING "0.1.1 Alpha"
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

int ParseOptions( OrigoOptions* outOptions );

/*
 *  Token declarations
 */

#define IDENT_MAXLEN 32
#define IDENT_MAXINDEX (IDENT_MAXLEN - 1)

enum TokenCode {
  GeneralToken = 0,
  tkEOF,
  tkEOL,

  KeywordToken = 1024,
  kwProgram
};

/*
 *  Warning/error functions
 */

void Error( unsigned code, const char* message );
void Expected( unsigned line, unsigned column, const char* message );
void Unexpected( unsigned line, unsigned column, const char* message );

/*
 *  Lexer declarations
 */

typedef struct RetFile {
  FILE* handle;

  unsigned line;
  unsigned column;

  char curCh;
  char nextCh;
} RetFile;

extern RetFile retFile;

int ReadChar( RetFile* source );

int ReadIdentChar( RetFile* source );
int ReadIdent( RetFile* source, char* ident );

int ReadBinaryDigit( RetFile* source );
int ReadOctalDigit( RetFile* source );
int ReadHexDigit( RetFile* source );
int ReadDecimalDigit( RetFile* source );

int OpenRet( const char* fileName, RetFile* outsource );
void CloseRet( RetFile* outsource );

/*
 *  Symbol table declarations
 */

typedef struct SymTable {
} SymTable;

extern SymTable symtab;
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

int SkipSpace( RetFile* source );
int SkipComment( RetFile* source );
void SkipNonterminals( RetFile* source );

int Match( RetFile* source, const char* text );

int ParseProgram( RetFile* source, CFile* cgen, SymTable* symTable );

int Parse( RetFile* source, CFile* cgen, SymTable* symTable );

/*
 *  Main program declarations
 */

void Cleanup();

#endif // H_ORIGOTOC
