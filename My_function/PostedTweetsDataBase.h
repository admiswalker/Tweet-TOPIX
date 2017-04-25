#ifndef __INCLUDE_GUARD__PostedTweetsDataBase
	//既に構造体が定義されている場合は、俗称インクルードガードによって、再定義を回避する。
	#define __INCLUDE_GUARD__PostedTweetsDataBase	//[__INCLUDE_GUARD__]を定義し、次回以降読み込まないようにする。

#include "../DebagMode.h"	//デバッグモードに入れるかどうかの定義。(複数ファイルにまたがるため、ヘッダを読み込む。)

#include <string>
//#include <iostream>	//標準出力?のcoutに必要。
//#include <stdio.h>	//sprintf()
#include <time.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>		//floor

#include "../MyOAuthFunc/str_sprintf.h"
#include "./ICHashT.h"
#include "Process.h"		// [ struct get_data_from_tweet* &TD 構造体 ]を利用する為に必要。

//#define PostedTweetsDataBase_DebagMode	// この行がコメントアウトされていない場合は、デバッグ用のコードを生成します。

#ifdef PostedTweetsDataBase_DebagMode
#else  // PostedTweetsDataBase_DebagMode
#endif // PostedTweetsDataBase_DebagMode



//=====================================================================================================================================================
//PostedTweetsDataBase :: PostedTweetsDB // 時間の処理基準は全てローカル時間(日本標準時)とする。
//=====================================================================================================================================================
//=====================================================================================================================================================


// ハッシュテーブルのvalueとして動的に確保する構造体。	// 何か要素を追加したい場合は、こちらに追加すればよい。
struct SaveOnHashTable{
	struct tm tm_BlogPostTime;			//unix time	//(WeeklyTPOIXやMonthlyTOPIXをやる場合に使う??)							// [struct tm *tmp_LOCAL]などとすると、実体ではないので、扱いにくい。からやめた。←じゃあ、実体で渡せば??
	unsigned long long user_id;
	double average_reTweet_per_second;
};


#define AllScanOneText_EndMark -1	//ファイルポインタが開いていることを確認する。
struct OneLine_obj{
	char* line;
	int EndFlag;
	
	int hour;
	int min;
	unsigned long long user_id;
	unsigned long long tweet_id_re;
	double average_reTweet_per_second;
};


typedef class PostedTweetsDataBase{
private:
	int num_of_load;	//(今日を除き、)過去何日分のデータを読み込むか。
	
	std::string DB_RootName_str;
	std::string DB_Name_str;
	
	FILE *pDB_txt;									// テキストファイルへのファイルポインタ。
	class InsertedChainedHashTable* pICHashT;		// ハッシュテーブルへのポインタ
	
	struct tm tm_BlogPostTime;
	
	void GenerateAddressToTheTextFile( struct tm& tm_OpenTextTime, bool& GenerateNewDirectory, std::string& TextAdd, bool& result );
	
	void PostedTweetDB_Init_and_LoadData( const char* DB_RootName, const char* DB_Name, time_t& timep_LOCAL );
	
	void AllScanOneText_Open            ( struct OneLine_obj& OneLine, const char* TagetTEXTFile );
	void AllScanOneText_GetData_or_Close( struct OneLine_obj& OneLine );
	// 使い方
	//	struct OneLine_obj OneLine;
	//	for( AllScanOneText_Open(OneLine,TagetTEXTFile); OneLine.EndFlag!=AllScanOneText_EndMark; AllScanOneText_GetData_or_Close(OneLine) ){
	//		printf( "%02d\n", OneLine.hour                       );
	//		printf( "%02d\n", OneLine.min                        );
	//		printf( "%llu\n", OneLine.user_id                    );
	//		printf( "%llu\n", OneLine.tweet_id_re                );
	//		printf( "%lf\n" , OneLine.average_reTweet_per_second );
	//	}
	
public:
	PostedTweetsDataBase( const char* DB_RootName, const char* DB_Name, time_t& timep_LOCAL );		// コンストラクタ
	~PostedTweetsDataBase();																					// ディストラクタ
	
	//一致するツイートがあるかどうか検索する。
	void get0( unsigned long long& key, void*& value, bool& result );			// [ key :: unsigned long long tweet_id_re ]
	
	//すでにブログにポストされたツイートについて、テキストファイル上に保存されている、「平均リツイートスピード」を更新する。
	void RenewSpeedInTheText( unsigned long long key_tweet_id_re, struct SaveOnHashTable &pSoHT ); // key value
	
	//Bloggerにデータ投稿時に投稿したツイートを新しくハッシュテーブルと、テキストファイルに保存する。
	void SaveAndSet_Open ( time_t un_BlogPostTime          ); //file open
	void SaveAndSet      ( struct get_data_from_tweet* &TD ); // Save posts to the text and Set posts to the hash table.
	void SaveAndSet_Close(                                 ); //file close
	
	// 一日経過した時点で、ハッシュテーブルから、
	// 31日前のテキストファイル ( 読み込み時点では、30日前だったテキストファイル )
	// のデータをハッシュテーブルから削除する関数。
	void DeleteOldestDateDataFromHashTable( time_t& un_NewDate );
		// 入力データとしては、日付が変わった後(直後を想定。)の時間を与える。
		// (今日を含めず、)過去30日分のテキストデータを、動作途中で破棄した場合は、30日経過しても、メモリを解放できなくなるので、注意。
	
//	void CreateDaily( time_t& un_NewDate );
		// 入力データとしては、日付が変わった後の時間を与える。
		// 入力された日付より、一日前のデータを全て読み込み、速度順にソート。上位50件を返す。
		// 生のデータを扱う場合には、リツイート順にするだけだとゴミが多く混じったような気もするが、この場合は、既に取得されたデータについてしか扱わないので、単純なリツイート順でも問題ないと考えられる。
	
#ifdef ICHashT_OperationalDebagMode
	// 運用上のデバッグモードの設定
	class InsertedChainedHashTable* Get_pICHashT();
#else //ICHashT_OperationalDebagMode
	// 運用上のリリースモードの設定
#endif//ICHashT_OperationalDebagMode
}PostedTweetsDB;
//=====================================================================================================================================================
//=====================================================================================================================================================
//PostedTweetsDataBase :: PostedTweetsDB
//=====================================================================================================================================================



#endif//__INCLUDE_GUARD__PostedTweetsDataBase

