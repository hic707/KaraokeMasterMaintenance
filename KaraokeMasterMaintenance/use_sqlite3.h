// SQLite3���g���悤�ɂ���w�b�_
#pragma once
#include "../../include/sqlite3.h"

// sqlite3.dll���֐��|�C���^�^��`
typedef int(__cdecl * pOpen)(char *, sqlite3 **);
typedef int(__cdecl * pExec)(sqlite3 *, const char *, sqlite3_callback, void *, char **);
typedef int(__cdecl * pGetTable)(sqlite3 *, const char *, char ***, int *, int *, char **);
typedef int(__cdecl * pFreeTable)(char **);
typedef int(__cdecl * pClose)(sqlite3 *);

typedef const char *(__cdecl * pGetVer)( void );
