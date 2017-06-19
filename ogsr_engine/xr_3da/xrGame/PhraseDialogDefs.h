#pragma once
//#include "boost/shared_ptr.hpp"

class CPhraseDialog;
//typedef boost::shared_ptr<CPhraseDialog>	DIALOG_SHARED_PTR;
typedef intrusive_ptr<CPhraseDialog>		DIALOG_SHARED_PTR;


//. typedef int PHRASE_ID;

//. typedef shared_str	PHRASE_DIALOG_ID;

 DEFINE_VECTOR(shared_str,		DIALOG_ID_VECTOR,		DIALOG_ID_IT);
//. #define  NO_PHRASE				-1
//. #define  START_PHRASE			0
//. #define  START_PHRASE_STR		"0"
#include "PhraseDialog.h"