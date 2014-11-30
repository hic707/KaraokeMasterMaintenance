// DBテーブル定義
#pragma once
#include <basetsd.h>
#include <minwindef.h>

#define TEXT_SIZE 256

// Category
typedef struct{
	BYTE	byCategoryCd[7];
	BYTE	byCategoryName[51];
	INT32	i32GenreCd;
	BYTE	byPath[256];
	BYTE	bySubCategoryCd[7];
	BYTE	dummy[3];
}ST_CATEGORY;

// User
typedef struct{
	INT32	i32UserID;
	BYTE	byUserName[256];
	BYTE	byPhotoPath[256];
	BOOL	bDeleteFlg;
//	BYTE	dummy[2];
}ST_USER;

// Genre
typedef struct{
	INT32	i32GenreCd;
	BYTE	byGenreName[51];
	INT32	i32ExcelTemplate;
	BYTE	byTieUpText[51];
	INT32	i32SortField1;
	INT32	i32SortField2;
	BOOL	bDeleteFlg;
	BYTE	dummy[2];
}ST_GENRE;

// SongFile
typedef struct{
	INT32	i32SongID;
	BYTE	byCategoryCd[TEXT_SIZE];
	BYTE	bySubCategoryCd[TEXT_SIZE];
	BYTE	bySequenceNum[TEXT_SIZE];
	BYTE	byFileName[TEXT_SIZE];
	BYTE	bySongName[TEXT_SIZE];
	BYTE	bySongKana[TEXT_SIZE];
	BYTE	bySinger[TEXT_SIZE];
	BYTE	bySingerKana[TEXT_SIZE];
	INT32	i32VocalClassification;
	INT32	i32SubVocalClassification;
	INT32	i32DuoFlag;
	BYTE	byDuoColor1[TEXT_SIZE];
	BYTE	byDuoColor2[TEXT_SIZE];
	BYTE	byDuoColor3[TEXT_SIZE];
	BYTE	byDuoColor4[TEXT_SIZE];
	BYTE	byDuoColor5[TEXT_SIZE];
	BYTE	byDuoColorAll[TEXT_SIZE];
//	BYTE	dummy[0];
}ST_SONG_FILE;
