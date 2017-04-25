#include "PostedTweetsDataBase.h"



//=====================================================================================================================================================
//PostedTweetsDataBase :: PostedTweetsDB // 時間の処理基準は全てローカル時間(日本標準時)とする。
//=====================================================================================================================================================


// コンストラクタ
PostedTweetsDB::PostedTweetsDataBase( const char* DB_RootName, const char* DB_Name, time_t& timep_LOCAL ){
	
	num_of_load = 30;		//過去30日分のデータを読み込む。
	
	// 内部で保存しておく。
	DB_RootName_str = DB_RootName;
	DB_Name_str     = DB_Name;
	
	//コンストラクタ内に初期化を直接書くと、returnとか、いろいろできなかった気がするから、このままコンストラクタ内に初期化関数を入れた状態のままにしておこう。
	PostedTweetDB_Init_and_LoadData( DB_RootName, DB_Name, timep_LOCAL );
}
// ディストラクタ
PostedTweetsDB::~PostedTweetsDataBase(){
	
	// 全要素走査 ( ハッシュテーブル上に記録されているすべての key value ペアの取り出し。 )
	// ハッシュテーブル自体を解放する前に、テーブル上に動的に確保されているデータを全て解放する必要がある。
	{
		AllScanPlace NowPlace;
		for(pICHashT->AllScanInit(NowPlace); NowPlace < pICHashT->AllScanEnd; pICHashT->AllScanNextShift(NowPlace)){
			
			UINT64 key;		//ダミー(実際には使われない。)
			void* value_p;
			pICHashT->AllScanGetData(key,value_p,NowPlace);	// データの取り出し。 [InsertedChainedHashTable]
			
			delete (struct SaveOnHashTable*)value_p;		// 解放する構造体にキャストする。
		}
	}
	
	delete pICHashT;
}


void PostedTweetsDB::AllScanOneText_Open            ( struct OneLine_obj& OneLine, const char* TagetTEXTFile ){
	OneLine.EndFlag = 0;//初回の読み込みを通すために、必ず初期化しておく。
	pDB_txt = fopen( TagetTEXTFile, "r" );		// 読み込みモードでファイルを開く。(書き込みモードなどだと、ファイルが存在しない場合に新規作成されてしまうので、注意。)
	if( pDB_txt==NULL ){ OneLine.EndFlag=AllScanOneText_EndMark; return; }	// 指定した日付が存在しない場合は、読み込まない。
	// 目的のファイルが存在した場合は、データをロードする。

	//for debag
//	std::cout << TagetTEXTFile << std::endl;	// 参照対象のデータのディレクトリを表示

	OneLine.line        = NULL;	// 必ず初期化すること!!
	size_t mallocedSize = 0;		// 最大格納文字数。(実体としては、確保されたメモリサイズだと推測される。)
	ssize_t ReadSize;				// 「 ReadSize = [半角換算の文字数]+[\n]+[\0] 」となる。

	OneLine.EndFlag = getline( &OneLine.line, &mallocedSize, pDB_txt );	// 最初の一行目は。コメントなので空読み。
	if( OneLine.EndFlag==AllScanOneText_EndMark ){ free(OneLine.line); fclose( pDB_txt ); return; }	// 例え空のファイルであっても、解放する必要がある。(lengthが最低でも120だから。)(たぶん、メモリを使い回しているので、解放は一回で良い。)
	free(OneLine.line);
	OneLine.line        = NULL;
	
	//char buf01[1024],buf02[1024],buf03[1024];
	//for(int i=0; i<1024; i++){
	//	buf01[i] = 0;
	//	buf02[i] = 0;
	//	buf03[i] = 0;		// 終端にNULLが付加されないので、先に初期化しておく。
	//}
	//sscanf( line, "%s%s%s\n", buf01, buf02, buf03 );	// [sscanf();関数]は、文字列の境界を[半角スペース]でしか??判断できないので、注意。[,(カンマ)]などは無効。(従って、テキストのフォーマットは、文字列の境界に、[半角スペース]を用いる。)
														// [sscanf( line, "%s %s %s\n", buf01, buf02, buf03 );]でも、[sscanf( line, "%s%s%s\n", buf01, buf02, buf03 );]でも同じ動作。
	//printf( "%s::\n", buf01 );
	//printf( "%s::\n", buf02 );
	//printf( "%s::\n", buf03 );

	AllScanOneText_GetData_or_Close( OneLine );	//一つ目の項目を読みに行く。
}
void PostedTweetsDB::AllScanOneText_GetData_or_Close( struct OneLine_obj& OneLine ){
	size_t mallocedSize = 0;		// 最大格納文字数。(実体としては、確保されたメモリサイズだと推測される。)
	ssize_t ReadSize;				// 「 ReadSize = [半角換算の文字数]+[\n]+[\0] 」となる。
	while(1){
		OneLine.EndFlag = getline( &OneLine.line, &mallocedSize, pDB_txt );
		if( OneLine.EndFlag==AllScanOneText_EndMark){ free(OneLine.line); fclose( pDB_txt ); return; }	// 例え空のファイルであっても、解放する必要がある。(lengthが最低でも120だから。)(たぶん、メモリを使い回しているので、解放は一回で良い。)
		
		//printf( "%d\n", read   );
		//printf( "%d\n", length );
		//printf( "%d\n", line   );
		//std::cout << line;// << std::endl;
		
	//	int                hour    = 0, min         = 0;	// 初期化しておかないと、同じメモリ領域が使い回されているため、改行だけの行でも、前回の値が表示される。
	//	unsigned long long user_id = 0, tweet_id_re = 0;	// この辺りの名称は、[struct get_data_from_tweet構造体]と整合性を取っている……つもり。
	//	double average_reTweet_per_second = 0.0;
		OneLine.hour    = 0, OneLine.min         = 0;	// 初期化しておかないと、同じメモリ領域が使い回されているため、改行だけの行でも、前回の値が表示される。
		OneLine.user_id = 0, OneLine.tweet_id_re = 0;	// この辺りの名称は、[struct get_data_from_tweet構造体]と整合性を取っている……つもり。
		OneLine.average_reTweet_per_second = 0.0;
		sscanf( OneLine.line, "%02d%02d,%llu,%llu,%lf\n", &OneLine.hour, &OneLine.min, &OneLine.user_id, &OneLine.tweet_id_re, &OneLine.average_reTweet_per_second );
	free(OneLine.line);
		if( OneLine.hour==0 && OneLine.min==0 && OneLine.user_id==0 && OneLine.tweet_id_re==0 ){continue;}	// 改行だけの行の場合は、全て初期化時のまま。
		break;
	}
	//printf( "%02d\n", OneLine.hour                       );
	//printf( "%02d\n", OneLine.min                        );
	//printf( "%llu\n", OneLine.user_id                    );
	//printf( "%llu\n", OneLine.tweet_id_re                );
	//printf( "%lf\n" , OneLine.average_reTweet_per_second );
}



void PostedTweetsDB::PostedTweetDB_Init_and_LoadData( const char* DB_RootName, const char* DB_Name, time_t& timep_LOCAL ){
	// 時間の処理基準は全てローカル時間(日本標準時)とする。
	
	
	//ハッシュテーブルの初期化
	unsigned long long num_of_elements = (unsigned long long)floor(  ((double)(24*2*6*num_of_load)) * ((double)(1.5))  );		//LoadFactorが6割程度となるように調整
	pICHashT = new ICHashT(num_of_elements);	//ハッシュテーブルを動的に確保する。	//2^14==16384	//[24*2*6*30=8640ツイート/月]となる事を想定。
//	pICHashT = new ICHashT(16384);				//ハッシュテーブルを動的に確保する。	//2^14==16384	//[24*2*6*30=8640ツイート/月]となる事を想定。
	
	//============================================================================================================================================================
	// ここから、 [ 過去30日 + 今日 ] の分の投稿ツイートを取得する。
	
	//時間情報の取得
//	time_t  timep_LOCAL = time(0);											// time(0) : ローカル(PC上で)のunixtimeを取得する。	//どこまでのデータを読み込んでいるのかを、main関数から推測できるように、時間は外部から与える。
	time_t un_TagetDate = timep_LOCAL - (time_t)60*60*24*num_of_load;	// 30日前のunixtimeを取得する。

	while( un_TagetDate <= timep_LOCAL ){
		struct tm *tm_TagetDate = localtime(&un_TagetDate);		// PC上でのローカル時間「unix時間[time_t型]」を、ローカル時間の[tm構造体]へ変換

		bool GenerateNewDirectory = false;
		std::string TextAdd;
		bool result;
		GenerateAddressToTheTextFile( *tm_TagetDate, GenerateNewDirectory, TextAdd, result );

		if(result==true){
			// 対象となるテキストファイルが存在する場合。
			
			struct OneLine_obj OneLine;
			//テキストファイル一つ分の読み込み処理。
			for( AllScanOneText_Open(OneLine,(const char*)TextAdd.c_str()); OneLine.EndFlag!=AllScanOneText_EndMark; AllScanOneText_GetData_or_Close(OneLine) ){
				//printf( "%02d\n", OneLine.hour                       );
				//printf( "%02d\n", OneLine.min                        );
				//printf( "%llu\n", OneLine.user_id                    );
				//printf( "%llu\n", OneLine.tweet_id_re                );
				//printf( "%lf\n" , OneLine.average_reTweet_per_second );
				
				struct tm tmp;
				tmp.tm_year = tm_TagetDate->tm_year;
				tmp.tm_mon  = tm_TagetDate->tm_mon;
				tmp.tm_mday = tm_TagetDate->tm_mday;
				tmp.tm_hour = OneLine.hour;
				tmp.tm_min  = OneLine.min;
				tmp.tm_sec  = 0;
	
				//ここから、取得したデータをハッシュテーブルへ格納していく。
		
				struct SaveOnHashTable* pSoHT = new SaveOnHashTable;			//メモリの動的確保
				pSoHT->tm_BlogPostTime            = tmp;
				pSoHT->user_id                    = OneLine.user_id;
				pSoHT->average_reTweet_per_second = OneLine.average_reTweet_per_second;
				
				void** return_value_pp;
				bool result_2;
				pICHashT->set2( OneLine.tweet_id_re, (void*&)pSoHT, return_value_pp, result_2 );	//ハッシュテーブルにデータを追加
				if(result_2==false){
					printf("Error::PostedTweetsDB::PostedTweetDB_Init_and_LoadData,time::%d:%d:%d_%d:%d,usrID::%lld\n"
							,tmp.tm_year,tmp.tm_mon,tmp.tm_mday,tmp.tm_hour,tmp.tm_min,pSoHT->user_id);
					delete pSoHT;
				}
				
//				pICHashT->set1( OneLine.tweet_id_re, (void*&)pSoHT );	//ハッシュテーブルにデータを追加
																				// key    :: tweet_id_re
																				// value* :: struct SaveOnHashTable*
				
				/*{
				//for debag
					bool result;
					pICHashT->find( OneLine.tweet_id_re, result );
					printf("%s\n", result ? "true" : "false");
				}*/
			}
		}
		
		// データの読み込みが完了したら、次のファイルの探索、及び、読み込みへ移行する。
		un_TagetDate+=60*60*24;continue;
	}
	// ここまでで、必要なデータのハッシュテーブルへの格納が完了。
	
	//============================================================================================================================================================
}


void PostedTweetsDB::get0( unsigned long long& key, void*& value, bool& result ){
	pICHashT->get0( key, value, result );
}



//すでにブログにポストされたツイートについて、テキストファイル上に保存されている、「平均リツイートスピード」を更新する。
void PostedTweetsDB::RenewSpeedInTheText( unsigned long long key_tweet_id_re, struct SaveOnHashTable &pSoHT ){ // key value
	// 入力データとしては、 [ double average_reTweet_per_second ] 更新済みの [ struct SaveOnHashTable ] 構造体を渡す。
	
	bool GenerateNewDirectory = false;
	std::string TextAdd;
	bool result;
	GenerateAddressToTheTextFile( pSoHT.tm_BlogPostTime, GenerateNewDirectory, TextAdd, result );
	
	pDB_txt = fopen( TextAdd.c_str(), "r+" );	// 更新(読み取り及び書き込みモードでファイルを開く。(書き込みモードなどだと、ファイルが存在しない場合に新規作成されてしまうので、注意。)
	
	// もしここでファイルが開けないような場合があれば、他がバグっている。
	
	
	char  *line         = NULL;	// 必ず初期化すること!!
	size_t mallocedSize = 0;		// 最大格納文字数。(実体としては、確保されたメモリサイズだと推測される。)
	ssize_t ReadSize;				// 「 ReadSize = [半角換算の文字数]+[\n]+[\0] 」となる。

	ReadSize = getline( &line, &mallocedSize, pDB_txt );	// 最初の一行目は。コメントなので空読み。

	//char buf01[1024],buf02[1024],buf03[1024];
	//for(int i=0; i<1024; i++){
	//	buf01[i] = 0;
	//	buf02[i] = 0;
	//	buf03[i] = 0;		// 終端にNULLが付加されないので、先に初期化しておく。
	//}
	//sscanf( line, "%s%s%s\n", buf01, buf02, buf03 );	// [sscanf();関数]は、文字列の境界を[半角スペース]でしか??判断できないので、注意。[,(カンマ)]などは無効。(従って、テキストのフォーマットは、文字列の境界に、[半角スペース]を用いる。)
														// [sscanf( line, "%s %s %s\n", buf01, buf02, buf03 );]でも、[sscanf( line, "%s%s%s\n", buf01, buf02, buf03 );]でも同じ動作。
	//printf( "%s::\n", buf01 );
	//printf( "%s::\n", buf02 );
	//printf( "%s::\n", buf03 );
	
	
	// 一つ前のファイルポインタをコピーしておき、指定しようとしたら、どうやっても上手く行かなかったので、大人しくシークすることにした。
	long previous = ftell( pDB_txt );	// 所定の行と判断するためには、読み込まないといけないが、読み込んだ時点で、書き換え対象の行を過ぎてしまうので、一つ前の値を記録しておく。
	
	while( (ReadSize = getline( &line, &mallocedSize, pDB_txt )) != -1 ){	// データを一行読み込み。
		//printf( "%d\n", read   );
		//printf( "%d\n", length );
		//printf( "%d\n", line   );
		//std::cout << line;// << std::endl;
		
		int                hour    = 0, min         = 0;	// 初期化しておかないと、同じメモリ領域が使い回されているため、改行だけの行でも、前回の値が表示される。
		unsigned long long user_id = 0, tweet_id_re = 0;	// この辺りの名称は、[struct get_data_from_tweet構造体]と整合性を取っている……つもり。
		double average_reTweet_per_second = 0.0;
		sscanf( line, "%02d%02d,%llu,%llu,%lf\n", &hour, &min, &user_id, &tweet_id_re, &average_reTweet_per_second );
		if( hour==0 && min==0 && user_id==0 && tweet_id_re==0 ){continue;}	// 改行だけの行の場合は、全て初期化時のまま。

		//printf( "%02d\n", hour                       );
		//printf( "%02d\n", min                        );
		//printf( "%llu\n", user_id                    );
		//printf( "%llu\n", tweet_id_re                );
		//printf( "%lf\n", average_reTweet_per_second );
		
	//	struct tm tmp;
	//	tmp.tm_year = pSoHT.tm_BlogPostTime.tm_year;
	//	tmp.tm_mon  = pSoHT.tm_BlogPostTime.tm_mon;
	//	tmp.tm_mday = pSoHT.tm_BlogPostTime.tm_mday;
	//	tmp.tm_hour = hour;
	//	tmp.tm_min  = min;
	//	tmp.tm_sec  = 0;
	//	time_t time_unixtime_LOCAL = mktime((struct tm *)&tmp);
		
		if(    hour        == pSoHT.tm_BlogPostTime.tm_hour
		    && min         == pSoHT.tm_BlogPostTime.tm_min
		    && tweet_id_re == key_tweet_id_re              )
		{
			// ファイルポインタを所定の位置にセットする。
			fseek( pDB_txt, previous, SEEK_SET );
			
			std::string double_buf = str_sprintf( "%.7lf",pSoHT.average_reTweet_per_second );//double型について、整数部と小数部を合わせた最大文字数を指定できないので、一度文字列に変換する。// 文字数を合わせるため、[.7]によって必ず0埋めしておく。(そして、文字列として書き込むときに、余分なゼロが捨てられる。余分かどうかの判定は大変なので、取りあえず0を付けておくこと!!)
			//std::cout << double_buf << std::endl; //for debag
			// 時刻が一致する行に対して、データの更新を行う。(面倒なので、一行丸ごと更新する。)
			fprintf( pDB_txt, "%02d%02d,%020llu,%020llu,%.8s", pSoHT.tm_BlogPostTime.tm_hour, pSoHT.tm_BlogPostTime.tm_min, pSoHT.user_id, tweet_id_re, double_buf.c_str() );
			break;
		}
		
		previous = ftell( pDB_txt );	
	}
	free(line);					// 例え空のファイルであっても、解放する必要がある。(lengthが最低でも120だから。)(たぶん、メモリを使い回しているので、解放は一回で良い。)
	
	
	
	// データの読み込みが完了。
	fclose( pDB_txt );
}



void PostedTweetsDB::GenerateAddressToTheTextFile( struct tm& tm_OpenTextTime, bool& GenerateNewDirectory, std::string& TextAdd, bool& result ){
	//   struct tm& tm_OpenTextTime      :: 探索対象のファイルの日付
	//        bool& GenerateNewDirectory :: 規定のディレクトリが存在しなかった場合に、新規作成するかどうか。
	// std::string& return_Address       :: 探索対象のテキストファイルが存在するかもしれないディレクトリへのアドレス
	//        bool& result               :: 実行結果(対象のディレクトリがあったかどうあかを返す。)
	
	
	struct stat st;
	//============================================================================================================================================================
	// [投稿したツイートの保存先]となる、ルートディレクトリの探索。存在しない場合は生成。
	
	TextAdd  = "./";
	TextAdd += DB_RootName_str;
	if( stat( TextAdd.c_str(), &st ) != 0 ){
		result = false;
		if(GenerateNewDirectory==true){
			mkdir( TextAdd.c_str(), 0775 );
		}else{
			return;
		}
	}
	
	//============================================================================================================================================================
	// [投稿したツイートの言語名]で分類されたディレクトリの探索。存在しない場合は生成。
	
	TextAdd += "/";
	TextAdd += DB_Name_str;
	if( stat( TextAdd.c_str(), &st ) != 0 ){
		result = false;
		if(GenerateNewDirectory==true){
			mkdir( TextAdd.c_str(), 0775 );
		}else{
			return;
		}
	}
	
	//============================================================================================================================================================
	
	// ディレクトリの存在確認
	TextAdd += "/";
	TextAdd += (std::string)str_sprintf( "%04d", 1900 + tm_OpenTextTime.tm_year );
	if( stat( TextAdd.c_str(), &st ) != 0 ){
		result = false;
		if(GenerateNewDirectory==true){
			mkdir( TextAdd.c_str(), 0775 );
		}else{
			return;
		}
	}

	// ディレクトリの存在確認
	TextAdd += "/";
	TextAdd += (std::string)str_sprintf( "%02d",    1 + tm_OpenTextTime.tm_mon  );
	if( stat( TextAdd.c_str(), &st ) != 0 ){
		result = false;
		if(GenerateNewDirectory==true){
			mkdir( TextAdd.c_str(), 0775 );
		}else{
			return;
		}
	}
	
	result = true;
	
	TextAdd += "/";
	TextAdd += (std::string)str_sprintf(  "%04d_%02d_%02d_%s.txt", 1900 + tm_OpenTextTime.tm_year, 1 + tm_OpenTextTime.tm_mon, tm_OpenTextTime.tm_mday, DB_Name_str.c_str() );
	
	//============================================================================================================================================================
	
}


void PostedTweetsDB::SaveAndSet_Open ( time_t un_BlogPostTime ){
	//データのHDDへの保存先となる、テキストファイルを開く(存在しない場合は生成する)作業。
	
	// 投稿時間の算出
	tm_BlogPostTime = *localtime(&un_BlogPostTime);		// PC上でのローカル時間「unix時間[time_t型]」を、ローカル時間の[tm構造体]へ変換
	
	//=========================================
	
	struct stat st;
	
	bool GenerateNewDirectory = true;
	std::string TextAdd;
	bool result;
	GenerateAddressToTheTextFile( tm_BlogPostTime, GenerateNewDirectory, TextAdd, result );
	
	// テキストファイルの存在確認  ( [追加書き込み専用]でfopen();するので、fopenによるファイルの有無の確認はできない。 )
	if( stat( TextAdd.c_str(), &st ) != 0 ){
		pDB_txt = fopen( TextAdd.c_str(), "a" );	// [追加書き込み専用]でファイルを開く。(ファイルが存在しない場合は新規作成される。)
		fprintf( pDB_txt, "投稿時間 リツイートされたツイートのユーザー番号 リツイートされたのツイートのツイート番号\n" );		//ファイルを生成したので、まずは、一行目に要素の内容を示すコメントを挿入しておく。
																															//[\n]一回で改行は入っているようだが、[gedit]での表示が???二回[\n\n]としないと改行されない??(たぶん最終行の改行だけが一回省略されて表示されている。(たとえ改行が入っていても。))
	}else{
		pDB_txt = fopen( TextAdd.c_str(), "a" );	// [追加書き込み専用]でファイルを開く。(ファイルが存在しない場合は新規作成される。)
	}
	
	//=========================================
	
}
void PostedTweetsDB::SaveAndSet      ( struct get_data_from_tweet* &TD ){
	//入力される時間は毎時15分か45分に強制的に合わせておく。
	
	// ハッシュテーブルへ追加するデータとしては、新しくメモリを動的に確保することはせず、
	// 既に確保された[struct get_data_from_tweet構造体]のポインタを譲り受けることも考えたが、
	// 余分なデータも多く含んでいるため、新しい構造体を定義して、それを確保することにする。

	struct SaveOnHashTable* pSoHT = new SaveOnHashTable;		//メモリの動的確保
	pSoHT->tm_BlogPostTime            = tm_BlogPostTime;			// unix time		//(WeeklyTPOIXやMonthlyTOPIXをやる場合に使う??)
	pSoHT->user_id                    = TD->user_id;
	pSoHT->average_reTweet_per_second = TD->average_reTweet_per_second;
	
	// ハッシュテーブルに追加
	pICHashT->set1( TD->tweet_id_re, (void*&)pSoHT );		//set1():keyが既に格納されている場合は、result==falseで、何もせずにreturn;
	
	
	//weeklyTOPIXとかMonthlyTOPIXとかやる時に使うかもしれない項目。
//	void** return_value_pp;
//	bool   result;			// set2();関数の実行結果が書き込まれる。
//	pICHashT->set2( TD->tweet_id_re, (void*&)pSoHT, return_value_pp, result );		//set2():keyが既に格納されている場合は、valueを取得してresultを書き込んでreturn;(valueには手を加えないが、voidポインタなので、中身の判断と書き換えができる。)
	//if( result == false ){
	//	//ハッシュテーブルにkeyが登録されている場合。
	//}else{
	//	//ハッシュテーブルにkeyが登録されていない場合。
	//}
	
	
	// ここから、テキストファイルに保存========================================================
	
	std::string double_buf = str_sprintf( "%.7lf", TD->average_reTweet_per_second );//double型について、整数部と小数部を合わせた最大文字数を指定できないので、一度文字列に変換する。// 文字数を合わせるため、[.7]によって必ず0埋めしておく。(そして、文字列として書き込むときに、余分なゼロが捨てられる。余分かどうかの判定は大変なので、取りあえず0を付けておくこと!!)
	
	// これからBloggerに投稿するツイートをテキストに保存する。
	fprintf( pDB_txt, "%02d%02d,%020llu,%020llu,%.8s\n", tm_BlogPostTime.tm_hour, tm_BlogPostTime.tm_min, TD->user_id, TD->tweet_id_re, double_buf.c_str() );		//[リツイートされたツイートのユーザー番号]については、SD構造体に無いので、とりあえず0埋めしておく。
	
	// ========================================================ここまで、テキストファイルに保存
}
void PostedTweetsDB::SaveAndSet_Close(){
	fclose( pDB_txt );
}



void PostedTweetsDB::DeleteOldestDateDataFromHashTable( time_t& un_NewDate ){
	// もし、ハッシュテーブルに追加した分だけ直ぐにハッシュテーブルから削除するのであれば、そもそも、読み込みが一日単位ではいけなくなり、非常に面倒なので、しない。
	
	// 入力データとしては、日付が変わった後(直後を想定。)の時間を与える。
	
	// (今日を含めず、)過去30日分のテキストデータを、動作途中で破棄した場合は、30日経過しても、メモリを解放できなくなるので、注意。
	
	
	
	time_t un_TagetDate = un_NewDate - 60*60*24*( num_of_load + 1 );
	struct tm *tm_TagetDate = localtime(&un_TagetDate);				// PC上でのローカル時間「unix時間[time_t型]」を、ローカル時間の[tm構造体]へ変換
	
	// テキストファイル、及び、ディレクトリの算出========================================
	bool GenerateNewDirectory = false;
	std::string TextAdd;
	bool result;
	GenerateAddressToTheTextFile( *tm_TagetDate, GenerateNewDirectory, TextAdd, result );
	//printf("%s\n", result ? "true" : "false"); // for debag
	if(result==false){return;}
	//================================================================================
	
	//std::cout << "ForDebag here :: " << TextAdd << std::endl;
	
	// ハッシュテーブル上にある、一番古いデータ1日分を削除する。=========================
	struct OneLine_obj OneLine;
	//テキストファイル一つ分の読み込み処理。
	for( AllScanOneText_Open(OneLine,(const char*)TextAdd.c_str()); OneLine.EndFlag!=AllScanOneText_EndMark; AllScanOneText_GetData_or_Close(OneLine) ){
		//printf( "%02d\n", OneLine.hour                       );
		//printf( "%02d\n", OneLine.min                        );
		//printf( "%llu\n", OneLine.user_id                    );
		//printf( "%llu\n", OneLine.tweet_id_re                );
		//printf( "%lf\n" , OneLine.average_reTweet_per_second );
		
		void* value_p;
		bool  result_erase1;
		pICHashT->erase1( OneLine.tweet_id_re, result_erase1, value_p );	// ハッシュテーブルから対象のデータを削除する。
	
		//[value_p]の解放
#ifdef PostedTweetsDataBase_DebagMode
		//デバッグ用のテキストデータは、同一データが混じっているので、そのまま解放するとバグる。(データを用意するのが面倒だった。)
		if(result_erase1==true){ delete (struct SaveOnHashTable*)value_p; 	}
#else  // PostedTweetsDataBase_DebagMode
		delete (struct SaveOnHashTable*)value_p;
#endif // PostedTweetsDataBase_DebagMode
	}
	//================================================================================
}


#ifdef ICHashT_OperationalDebagMode
	// 運用上のデバッグモードの設定
	class InsertedChainedHashTable* PostedTweetsDB::Get_pICHashT(){return pICHashT;}
#else //ICHashT_OperationalDebagMode
	// 運用上のリリースモードの設定
#endif//ICHashT_OperationalDebagMode


//	void PostedTweetsDB::CreateDaily( time_t& un_NewDate ){}



//=====================================================================================================================================================
//PostedTweetsDataBase :: PostedTweetsDB
//=====================================================================================================================================================


