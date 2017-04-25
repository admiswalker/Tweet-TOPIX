#ifndef __INCLUDE_GUARD__Filters
	//既に構造体が定義されている場合は、俗称インクルードガードによって、再定義を回避する。
	#define __INCLUDE_GUARD__Filters	//[__INCLUDE_GUARD__]を定義し、次回以降読み込まないようにする。

#include "Process.h"
#include <math.h>		//log()

int BasicFilter       ( struct get_data_from_tweet*& TD );
int TimeFilter        ( struct get_data_from_tweet*& TD, time_t &timep_UTC );
int TimeFilter_daily  ( struct get_data_from_tweet*& TD, time_t &timep_UTC );
int BlackListFilter_ja( struct get_data_from_tweet*& TD );
int BlackListFilter_en( struct get_data_from_tweet*& TD );
int BlackListFilter_ko( struct get_data_from_tweet*& TD );
int DataBaseFilter    ( struct get_data_from_tweet*& TD, class PostedTweetsDataBase* &pPT_DB );	// TD.tweet_id_re がlevelDBに登録されているか?

#endif//__INCLUDE_GUARD__Filters


