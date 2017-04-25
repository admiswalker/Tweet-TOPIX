#ifndef __INCLUDE_GUARD__TwitterFunctions
	//既に構造体が定義されている場合は、俗称インクルードガードによって、再定義を回避する。
	#define __INCLUDE_GUARD__TwitterFunctions	//[__INCLUDE_GUARD__]を定義し、次回以降読み込まないようにする。

//#include <stdlib.h>	//[system();]関数に必要。
//#include <stdio.h>
//#include <string>
//#include <iostream>		//標準出力?のcoutに必要。
//#include <sys/time.h>	//gettimeofday()
//#include <time.h>

//#include "../MyOAuthFunc/Base64_encode.h"
#include "../MyOAuthFunc/Generate_CustomHeadre_of_Authorization.h"
#include "../MyOAuthFunc/Generate_oauth_nonce.h"
//#include "../MyOAuthFunc/HMAC_SHA1_and_Base64encode.h"
#include "../MyOAuthFunc/URL_encode_RFC_3986.h"
#include "../MyOAuthFunc/str_sprintf.h"

#include "../My_function/Decode_escapedUTF16String_to_UTF8Binary.h"
//#include "../My_function/Filters.h"
//#include "../My_function/GetOneTweetJSON.h"
#include "../My_function/ICHashT.h"			// [ InsertedChainedHashTable ] :: 自作ハッシュテーブル
#include "../My_function/Make_Connection.h"
//#include "../My_function/MemoryLogger.h"		//for debag
//#include "../My_function/PostedTweetsDataBase.h"//インクルードが再帰的になって、コンパイルできなくなるので、mainのファイルではコメントアウト
//#include "../My_function/Process.h"
//#include "../My_function/SendGmail.h"
//#include "../My_function/TwitterFunctions.h"	// [ GetTweet_HTML_Code() ],[ Send_reply_to_the_tweet_posted_tweet() ]

int GetTweet_HTML_Code( struct OAuth_parameters &OA , std::string &language , unsigned long long &TOP_reTweets_IDs, std::string &HTMLCode );
int Send_reply_to_the_tweet_posted_tweet( struct OAuth_parameters &OA, std::string &language, unsigned long long &TOP_reTweets_IDs, std::string &TOP_reTweets_NAMEs, std::string &BlogURL );

#endif//__INCLUDE_GUARD__TwitterFunctions


