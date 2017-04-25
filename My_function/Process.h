#include <stdio.h>	//printf()
#include <string>		//std::string
#include <string.h>	//strstr()
#include "Decode_escapedUTF16String_to_UTF8Binary.h"

#include <vector>
#include <algorithm>							//std::sort
#include <iostream>							//標準出力?のcoutに必要。

#include "../MyOAuthFunc/str_sprintf.h"	//str_sprintf();

#include "./ICHashT.h"						// [ InsertedChainedHashTable ] :: 自作ハッシュテーブル
#include "PostedTweetsDataBase.h"

#include "../rapidjson/document.h"	//JSONのパーサ
using namespace rapidjson;

#ifndef __INCLUDE_GUARD__get_data_from_tweet
	//既に構造体が定義されている場合は、俗称インクルードガードによって、再定義を回避する。
	#define __INCLUDE_GUARD__get_data_from_tweet	//[__INCLUDE_GUARD__]を定義し、次回以降読み込まないようにする。
	struct get_data_from_tweet{
		time_t             time_unixtime;			//取得した値を返す
		unsigned long long tweet_id;				//取得した値を返す
		time_t             time_unixtime_re;		//取得した値を返す	//retweetの場合は値がセットされる。それ以外の場合は0となる。
		unsigned long long tweet_id_re;				//取得した値を返す	//retweetの場合は値がセットされる。それ以外の場合は0となる。
		std::string        text;						//取得した値を返す
		unsigned long long user_id;					//取得した値を返す
		std::string        name_re;					//ユーザー名   例::Tweet-TOPIX(bot)
		std::string        screen_name_re;			//@以降の名前  例::@Rating_Sprite
		unsigned long long followers_count;		//取得した値を返す
		bool               verified;				//認証済みアカウントかどうか
		unsigned long long friends_count;			//取得した値を返す
		unsigned long long retweet_count;			//リツイート数	//リツイート数とお気に入り数の違いが5倍以上だと、スパムの可能性があるかもしれない。無いかもしれない。
		unsigned long long favorite_count;			//お気に入り数	//リツイート数とお気に入り数の違いが5倍以上だと、スパムの可能性があるかもしれない。無いかもしれない。
	
		std::string        language;
		double             average_reTweet_per_second;	//平均リツイートスピード
		double             retweet_efficiency;				//リツイート効率を計算。		//retweet_efficiency = (double)TD.retweet_count / (double)TD.followers_count;		//ゼロ割りに注意する。
	};
#endif//__INCLUDE_GUARD__get_data_from_tweet


int get_data( char*& TweetBegin, struct get_data_from_tweet* &TD );
	//                       char*& TweetBegin :: 検索対象の文字列の先頭。
	// struct get_data_from_tweet & TD         :: 検索対象の文字列から、抽出した要素が、この構造体に格納される。


/*
struct sort_data{
	                                                               int num_of_element;
	                                           //                  int num_of_top_tweet_to_get,		//正確な実装は後で。今回は取りあえず単なるフラグとして使う。
	                                                unsigned long long most_fastest_reTweet_s_ID;
	                                                       std::string most_fastest_reTweet_s_NAME;
	                                                            double average_reTweet_per_second_100th;
};
*/


#ifndef __INCLUDE_GUARD__sort_data
	//既に構造体が定義されている場合は、俗称インクルードガードによって、再定義を回避する。
	#define __INCLUDE_GUARD__sort_data	//[__INCLUDE_GUARD__]を定義し、次回以降読み込まないようにする。
	struct sort_data{
			                                                           int num_of_post;			//毎時15分45分のpostでBloggerに投稿するtweetの数。
			                                                           int num_of_element;		//現在hash tableに格納されているツイートの数
			                               std::vector<unsigned long long> TOP_reTweets_IDs;		//[std::vector]を用いて、動的に可変長な配列とする。
			                               std::vector<    std::string   > TOP_reTweets_NAMEs;	//[std::vector]を用いて、動的に可変長な配列とする。
			                                                        double average_reTweet_per_second_100th;
	};
#endif//__INCLUDE_GUARD__sort_data

int sort_tweet( ICHashT*& pICHashT, struct sort_data &SD, class PostedTweetsDataBase* &pPT_DB, time_t &timep_LOCAL );









