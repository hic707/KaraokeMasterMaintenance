// SQLite3を使うようにするヘッダ
#pragma once
#include "../../include/sqlite3.h"

// sqlite3.dll内関数ポインタ型定義
typedef int(__cdecl * pOpen)(char *, sqlite3 **);
typedef int(__cdecl * pExec)(sqlite3 *, const char *, sqlite3_callback, void *, char **);
typedef int(__cdecl * pGetTable)(sqlite3 *, const char *, char ***, int *, int *, char **);
typedef int(__cdecl * pFreeTable)(char **);
typedef int(__cdecl * pClose)(sqlite3 *);

typedef const char *(__cdecl * pGetVer)( void );
