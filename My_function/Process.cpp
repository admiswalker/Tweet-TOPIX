#include "Process.h"

//=========================================================================================================================================================================================================================
// [ Process.cpp ] :: begin :: [ Process.cpp ] では、テキストデータを解析して、リツイート数などの情報を抽出する。(テキストは、 [ class GetTweet ] によって、ツイート一件分ずつに切り分けられたデータを用いる。)
//=========================================================================================================================================================================================================================
//=========================================================================================================================================================================================================================



int Search_and_Get_JSON( char* &JSON, std::string &key, std::string &result_of_value ){
	
	char* JSON_Begin = strstr( JSON, (char*)key.c_str() );	//「key」を検索する。
	if( JSON_Begin == NULL ){return -1;}
	
	JSON_Begin += key.length();							//「key」の字数分進める
//	printf("%s\n\n",JSON_Begin);

	for(int read=0; JSON_Begin[read]!='\"'; read++){	//["]の場合に処理を終了する。
		result_of_value += JSON_Begin[read];
	}
	
	return 0;
}
int Search_JSON_and_Get_number( char* &JSON, std::string &key, unsigned long long &result_of_num ){
	
	char* JSON2 = strstr( JSON, (char*)key.c_str() );	//「key」を検索する。
	if( JSON2 == NULL ){
//		printf("NULL\n");//for debag
		return -1;
	}else{
//		printf("NOT NULL\n");//for debag
		JSON = JSON2;
	}
	
	JSON += key.length();							//「key」の字数分進める
//	printf("%s\n\n",JSON_Begin);

	result_of_num=0;//initialize
	for(int read=0; JSON[read]!=','; read++){	//[,]まで読み込み
		//printf("result_of_num: %d\n\n",result_of_num);
		result_of_num = result_of_num*10 + (JSON[read] - '0');
	}
	
	return 0;
}

//RapidJSONのメンバーチェック用
bool HasMember(Document &doc,const char* MemberName){
	//存在確認
	if(doc.HasMember(MemberName)==false){
//		printf(" [ERROR:Process.cpp:%s:Member is not exist.] ",MemberName);
		return false;
	}
	return true;
}
//RapidJSONのメンバーチェック用
Value::ConstMemberIterator FindMember(Document &doc,const char* MemberName){
	Value::ConstMemberIterator itr = doc.FindMember(MemberName);
	//存在確認
	if(itr != doc.MemberEnd()){
		return itr;
	}else{
//		printf(" [ERROR:Process.cpp:%s:Member is not exist.] ",MemberName);
		return doc.MemberEnd();
	}
}
	//多重定義(オーバーロード)
Value::ConstMemberIterator FindMember(Document &doc,Value::ConstMemberIterator &itr_given,const char* MemberName){
	Value::ConstMemberIterator itr = itr_given->value.FindMember(MemberName);
	//存在確認
	if(itr != doc.MemberEnd()){
		return itr;
	}else{
//		printf(" [ERROR:Process.cpp:%s:Member is not exist.] ",MemberName);
		return doc.MemberEnd();
	}
}
int get_data( char*& TweetBegin, struct get_data_from_tweet* &TD )
{
	// http://qiita.com/k2ymg/items/eef3b15eaa27a89353ab
	// http://befool.co.jp/blog/chainzhang/cocos2dx-rapid-json/
	
	//あとで、引用については、新たにメモリを確保して別のツイートとして、ハッシュテーブルに保存するように変更する。
	//->データ収集率向上のため。
	
	
	Document doc;
	
	
	doc.Parse(TweetBegin);
	bool error = doc.HasParseError();
	if(error){
		printf("parse error\n");
		return -1;//パースに失敗した場合は、-1を返す。
	}
	
	//有効なツイートであることの確認
	if(HasMember(doc,"created_at")==false){return -1;}//メンバの存在確認
	/*bool exist = doc.HasMember("created_at");
	if(exist==false){
		//無効なツイートの場合
		//例 : 
		//{"delete":{"status":{"id":751463034808373249,"id_str":"751463034808373249","user_id":571333151,"user_id_str":"571333151"},"timestamp_ms":"1467998788211"}}
		return -1;
	}
	*/
	
	
	Value::ConstMemberIterator itr = doc.FindMember("retweeted_status");//リツイートの存在確認
	Value::ConstMemberIterator itrUSER;
	Value::ConstMemberIterator itrbuf;
	if(itr != doc.MemberEnd()){
		//リツイートの解析
		//printf("This tweet is retweet.\n");
		
		
		//get tweeted time. (created_at)
		if((itrbuf=FindMember(doc,itr,"created_at"))==doc.MemberEnd()){return -1;}//メンバの存在確認
		const char* ReT_created_at_str = itrbuf->value.GetString();
		//const char* ReT_created_at_str = itr->value["created_at"].GetString();
		
		//解析対象の情報のサンプル
		//"created_at":"Fri Feb 06 18:05:24 +0000 2015"
		//"created_at":"Mon Feb 23 09:00:20 +0000 2015"
		//これをunix時間に変換する。
		struct tm tmp;
		strptime( ReT_created_at_str, "%a %b %d %H:%M:%S +0000 %Y", &tmp );
		TD->time_unixtime_re = mktime((struct tm *)&tmp);								//[tm構造体]で指定された時間を、unix時間[time_t型]に変換する。
		//printf("unixtimeOfReT = %ld\n\n",TD->time_unixtime_re);
		
		
		//get retweet id
		if((itrbuf=FindMember(doc,itr,"id"))==doc.MemberEnd()){return -1;}//メンバの存在確認
		TD->tweet_id_re = itrbuf->value.GetUint64();
		//TD->tweet_id_re = itr->value["id"].GetUint64();
		//printf("retweet_id = %llu\n\n",TD->tweet_id_re);
		
		
		//get text
		if((itrbuf=FindMember(doc,itr,"text"))==doc.MemberEnd()){return -1;}//メンバの存在確認
		TD->text.clear();													//文字の長さを0にする。(メモリが解放される訳では無い。)
		TD->text = itrbuf->value.GetString();							//無駄なメモリーコピー (もっと上手い方法はないだろうか。)(コピーしたくないので、はじめから、こちらに書き込んでほしい。)
		//TD->text = itr->value["text"].GetString();							//無駄なメモリーコピー (もっと上手い方法はないだろうか。)(コピーしたくないので、はじめから、こちらに書き込んでほしい。)
		//printf("retweet text = %s\n\n", TD->text.c_str());
		
		
		//get user id
		if((itrUSER=FindMember(doc,itr,"user"))==doc.MemberEnd()){return -1;}//メンバの存在確認
		if((itrbuf=FindMember(doc,itrUSER,"id"))==doc.MemberEnd()){return -1;}//メンバの存在確認
		TD->user_id = itrbuf->value.GetUint64();
		//TD->user_id = itr->value["user"]["id"].GetUint64();
		//printf("user_id = %llu\n\n",TD->user_id);
		
		
		//get user name
		if((itrbuf=FindMember(doc,itrUSER,"name"))==doc.MemberEnd()){return -1;}//メンバの存在確認
		TD->name_re.clear();												//文字の長さを0にする。(メモリが解放される訳では無い。)
		TD->name_re = itrbuf->value.GetString();				//無駄なメモリーコピー (もっと上手い方法はないだろうか。)(コピーしたくないので、はじめから、こちらに書き込んでほしい。)
		//TD->name_re = itr->value["user"]["name"].GetString();				//無駄なメモリーコピー (もっと上手い方法はないだろうか。)(コピーしたくないので、はじめから、こちらに書き込んでほしい。)
		//printf("user name =  %s\n\n", TD->name_re.c_str());
		
		
		//get screen_name
		if((itrbuf=FindMember(doc,itrUSER,"screen_name"))==doc.MemberEnd()){return -1;}//メンバの存在確認
		TD->screen_name_re.clear();											//文字の長さを0にする。(メモリが解放される訳では無い。)	
		TD->screen_name_re = itrbuf->value.GetString();	//無駄なメモリーコピー (もっと上手い方法はないだろうか。)(コピーしたくないので、はじめから、こちらに書き込んでほしい。)
		//TD->screen_name_re = itr->value["user"]["screen_name"].GetString();	//無駄なメモリーコピー (もっと上手い方法はないだろうか。)(コピーしたくないので、はじめから、こちらに書き込んでほしい。)
		//printf("user name = %s\n\n", TD->screen_name_re.c_str());
		
		
		//get verified
		if((itrbuf=FindMember(doc,itrUSER,"verified"))==doc.MemberEnd()){return -1;}//メンバの存在確認
		TD->verified = itrbuf->value.GetBool();
		//TD->verified = itr->value["user"]["verified"].GetBool();
		//printf("verified = %s\n\n", TD->verified ? "true" : "false");
		
		
		//followers_count
			//ここのuserはrootから直接のusernaので注意
		if((itrbuf=FindMember(doc,"user"))==doc.MemberEnd()){return -1;}//メンバの存在確認
		if((itrbuf=FindMember(doc,itrbuf,"followers_count"))==doc.MemberEnd()){return -1;}//メンバの存在確認
		TD->followers_count = itrbuf->value.GetUint64();
		//TD->followers_count = doc["user"]["followers_count"].GetUint64();
		//printf("friends_count = %llu\n\n", TD->followers_count);
		
		
		//get friends_count
			//"user/friends_count" は負の値を取ることがある。<-意味がわからないが、確かに負の数をとっていることがあった。
		if((itrbuf=FindMember(doc,itrUSER,"friends_count"))==doc.MemberEnd()){return -1;}//メンバの存在確認
		long long int friends_count = itrbuf->value.GetInt64();
		//long long int friends_count = itr->value["user"]["friends_count"].GetInt64();
		if(friends_count<0){
			//後のコードが負の数を考慮していないので、正の整数に丸める。
			friends_count=0;
		}
		TD->friends_count = (unsigned long long int)friends_count;
		//printf("friends_count = %llu\n\n", TD->friends_count);
		
		
		//get retweet_count
		if((itrbuf=FindMember(doc,itr,"retweet_count"))==doc.MemberEnd()){return -1;}//メンバの存在確認
		TD->retweet_count = itrbuf->value.GetUint64();
		//TD->retweet_count = itr->value["retweet_count"].GetUint64();
		//printf("retweet_count = %llu\n\n", TD->retweet_count);
		
		
		//get favorite_count
		if((itrbuf=FindMember(doc,itr,"favorite_count"))==doc.MemberEnd()){return -1;}//メンバの存在確認
		TD->favorite_count = itrbuf->value.GetUint64();
		//TD->favorite_count = itr->value["favorite_count"].GetUint64();
		//printf("favorite_count = %llu\n\n", TD->favorite_count);
		
		
		//get language
		if((itrbuf=FindMember(doc,itr,"lang"))==doc.MemberEnd()){return -1;}//メンバの存在確認
		TD->language.clear();												//文字の長さを0にする。(メモリが解放される訳では無い。)
		TD->language = itrbuf->value.GetString();				//無駄なメモリーコピー (もっと上手い方法はないだろうか。)(コピーしたくないので、はじめから、こちらに書き込んでほしい。)
		//TD->language = itr->value["lang"].GetString();				//無駄なメモリーコピー (もっと上手い方法はないだろうか。)(コピーしたくないので、はじめから、こちらに書き込んでほしい。)
		//printf("language = %s\n\n", TD->language.c_str());
		
		
	}else{
		//printf("This tweet is not retweet.\n");
		TD->time_unixtime_re = 0;
		TD->tweet_id_re      = 0;
		
		
		//通常のツイートの解析
		
		//get tweeted time. (created_at)
		if((itr=FindMember(doc,"created_at"))==doc.MemberEnd()){return -1;}//メンバの存在確認
		const char* created_at_str = itr->value.GetString();
		//const char* created_at_str = doc["created_at"].GetString();
		//printf("string = %s\n", created_at_str);

		//解析対象の情報のサンプル
		//"created_at":"Fri Feb 06 18:05:24 +0000 2015"
		//"created_at":"Mon Feb 23 09:00:20 +0000 2015"
		//これをunix時間に変換する。
		struct tm tmp;
		strptime( created_at_str, "%a %b %d %H:%M:%S +0000 %Y", &tmp );
		TD->time_unixtime = mktime((struct tm *)&tmp);								//[tm構造体]で指定された時間を、unix時間[time_t型]に変換する。
		//printf("unixtime = %ld\n\n",TD->time_unixtime);
		
		
		//get tweet id
		if((itr=FindMember(doc,"id"))==doc.MemberEnd()){return -1;}//メンバの存在確認
		TD->tweet_id = itr->value.GetUint64();
		//TD->tweet_id = doc["id"].GetUint64();
		//printf("tweet_id = %llu\n", TD->tweet_id);
		
		
		//get text
		if((itr=FindMember(doc,"text"))==doc.MemberEnd()){return -1;}//メンバの存在確認
		TD->text.clear();													//文字の長さを0にする。(メモリが解放される訳では無い。)
		TD->text = itr->value.GetString();									//無駄なメモリーコピー (もっと上手い方法はないだろうか。)(コピーしたくないので、はじめから、こちらに書き込んでほしい。)
		//TD->text = doc["text"].GetString();									//無駄なメモリーコピー (もっと上手い方法はないだろうか。)(コピーしたくないので、はじめから、こちらに書き込んでほしい。)
		//printf("tweet text = %s\n\n", TD->text.c_str());
		
		
		//get user id
		if((itrUSER=FindMember(doc,"user"))==doc.MemberEnd()){return -1;}//メンバの存在確認
		if((itr=FindMember(doc,itrUSER,"id"))==doc.MemberEnd()){return -1;}//メンバの存在確認
		TD->user_id = itr->value.GetUint64();
		//TD->user_id = doc["user"]["id"].GetUint64();
		//printf("user_id = %llu\n\n",TD->user_id);
		
		
		//get user name
			//reではないが、すべてreとして処理(暇なら書きなおす。)
		if((itr=FindMember(doc,itrUSER,"name"))==doc.MemberEnd()){return -1;}//メンバの存在確認
		TD->name_re.clear();												//文字の長さを0にする。(メモリが解放される訳では無い。)
		TD->name_re = itr->value.GetString();						//無駄なメモリーコピー (もっと上手い方法はないだろうか。)(コピーしたくないので、はじめから、こちらに書き込んでほしい。)
		//TD->name_re = doc["user"]["name"].GetString();						//無駄なメモリーコピー (もっと上手い方法はないだろうか。)(コピーしたくないので、はじめから、こちらに書き込んでほしい。)
		//printf("user name =  %s\n\n", TD->name_re.c_str());
		
		
		//get screen_name
			//reではないが、すべてreとして処理(暇なら書きなおす。)
		if((itr=FindMember(doc,itrUSER,"screen_name"))==doc.MemberEnd()){return -1;}//メンバの存在確認
		TD->screen_name_re.clear();											//文字の長さを0にする。(メモリが解放される訳では無い。)	
		TD->screen_name_re = itr->value.GetString();		//無駄なメモリーコピー (もっと上手い方法はないだろうか。)(コピーしたくないので、はじめから、こちらに書き込んでほしい。)
		//TD->screen_name_re = doc["user"]["screen_name"].GetString();		//無駄なメモリーコピー (もっと上手い方法はないだろうか。)(コピーしたくないので、はじめから、こちらに書き込んでほしい。)
		//printf("user name = %s\n\n", TD->screen_name_re.c_str());
		
		
		//get verified
		if((itr=FindMember(doc,itrUSER,"verified"))==doc.MemberEnd()){return -1;}//メンバの存在確認
		TD->verified = itr->value.GetBool();
		//TD->verified = doc["user"]["verified"].GetBool();
		//printf("verified = %s\n\n", TD->verified ? "true" : "false");
		
		
		//followers_count
		if((itr=FindMember(doc,itrUSER,"followers_count"))==doc.MemberEnd()){return -1;}//メンバの存在確認
		TD->followers_count = itr->value.GetUint64();
		//TD->followers_count = doc["user"]["followers_count"].GetUint64();
		//printf("friends_count = %llu\n\n", TD->followers_count);
		
		
		//get friends_count
			//"user/friends_count" は負の値を取ることがある。<-意味がわからないが、確かに負の数をとっていることがあった。
		if((itr=FindMember(doc,itrUSER,"friends_count"))==doc.MemberEnd()){return -1;}//メンバの存在確認
		long long int friends_count = itr->value.GetInt64();
		//long long int friends_count = doc["user"]["friends_count"].GetInt64();
		if(friends_count<0){
			//後のコードが負の数を考慮していないので、正の整数に丸める。
			friends_count=0;
		}
		TD->friends_count = (unsigned long long int)friends_count;
		//printf("friends_count = %llu\n\n", TD->friends_count);
		
		
		//get retweet_count
		if((itr=FindMember(doc,"retweet_count"))==doc.MemberEnd()){return -1;}//メンバの存在確認
		TD->retweet_count = itr->value.GetUint64();
		//TD->retweet_count = doc["retweet_count"].GetUint64();
		//printf("retweet_count = %llu\n\n", TD->retweet_count);
		
		
		//get favorite_count
		if((itr=FindMember(doc,"favorite_count"))==doc.MemberEnd()){return -1;}//メンバの存在確認
		TD->favorite_count = itr->value.GetUint64();
		//TD->favorite_count = doc["favorite_count"].GetUint64();
		//printf("favorite_count = %llu\n\n", TD->favorite_count);
		
		
		//get language
		if((itr=FindMember(doc,itrUSER,"lang"))==doc.MemberEnd()){return -1;}//メンバの存在確認
		//if(HasMember(doc,"lang")==false){return -1;}//メンバの存在確認
		TD->language.clear();												//文字の長さを0にする。(メモリが解放される訳では無い。)
		TD->language = itr->value.GetString();						//無駄なメモリーコピー (もっと上手い方法はないだろうか。)(コピーしたくないので、はじめから、こちらに書き込んでほしい。)
		//TD->language = doc["lang"].GetString();						//無駄なメモリーコピー (もっと上手い方法はないだろうか。)(コピーしたくないので、はじめから、こちらに書き込んでほしい。)
		//printf("language = %s\n\n", TD->language.c_str());
		
		
		TD->time_unixtime_re = TD->time_unixtime;	//Filter.cppで結局TD->time_unixtime_reだけを見るようになっているため。(TD->time_unixtime_re==0の場合、BasicFilterで弾かれる。)
		TD->tweet_id_re      = TD->tweet_id;
	}
	
	
	
	
	return 0;
}



struct tweet_sort_data{
	unsigned long long tweet_id_re;						//key		//retweetの場合は値がセットされる。それ以外の場合は0となる。
	double             average_reTweet_per_second;	//value	//平均リツイートスピード
};
// 比較関数を定義
bool cmp_sort( const struct tweet_sort_data& left, const struct tweet_sort_data& right ) {
	return left.average_reTweet_per_second > right.average_reTweet_per_second;	//降順
}
int sort_tweet( ICHashT*& pICHashT, struct sort_data &SD, class PostedTweetsDataBase* &pPT_DB, time_t &timep_LOCAL ){
	// 現在のコードはハッシュテーブルの耐久試験も兼ねているので、およそ、動的にハッシュテーブルを再確保して、必要な分だけ書き写した方が効率的だとは考えられるが、ハッシュテーブルを交換する前と同じ処理手順で実装する。(その内に、より効率的なコードに書き直すこと!!)
	
	// SD.num_of_element
	// SD.average_reTweet_per_second_100th : 100番目に速いリツイート・スピードの値を記録する。
	
	std::vector<struct tweet_sort_data> SORTed_tweet;
	
	// 全要素走査 ( ハッシュテーブル上に記録されているすべての key value ペアの取り出し。 )
	{
		AllScanPlace NowPlace;
		for(pICHashT->AllScanInit(NowPlace); NowPlace < pICHashT->AllScanEnd; pICHashT->AllScanNextShift(NowPlace)){
			
			UINT64 key;
			void* value_p;
			pICHashT->AllScanGetData(key,value_p,NowPlace);	// データの取り出し。 [InsertedChainedHashTable]
			
			//ここのコピーが最適化されない(一旦構造体にバッファするのは余計な操作なので、直接書き込める方が手っ取り早い)のなら、ポインタで構造体の先を示すように書き直す手もある。
			struct tweet_sort_data tweet_sort_data_buf;
			tweet_sort_data_buf.tweet_id_re                = key;
			tweet_sort_data_buf.average_reTweet_per_second = ((struct get_data_from_tweet*)(void*)value_p)->average_reTweet_per_second;	// [ ((struct get_data_from_tweet*)(void*)value_p) ]
			
			SORTed_tweet.push_back( tweet_sort_data_buf );	//set data for [std::sort]
		}
	}
	
	
	std::sort( SORTed_tweet.begin(), SORTed_tweet.end(), cmp_sort );							//sort	//全てソートする場合				//今回の実装では、どちらでソートしても必要分しか実装しないので、特に問題ない。
	
	
	std::cout << std::endl;	//ここまで改行されないので、改行しておく。
	std::cout << "■■■ print sorted data. ( start ) ■■■" << std::endl;
	for(int num=0; num < SD.num_of_element && num < 20; num++){	//実際には、一番速度が速かったツイートをBloggerにメールで送信するので、2番目～101番目までをハッシュテーブルに書き戻す。	//全て書き出すとコンソールが一気に埋まっていくし、どちらにしても全部見る訳では無いので、上位20件までしか表示させない。	//[num_of_element]がおそらく50以上程度だった気がするので、「num < 20」で事足りるとは思うが、念のため。
		//[get_data_from_tweet]構造体をそのままコピーしている。
		
		UINT64 key = SORTed_tweet[num].tweet_id_re;
		void*  value_p;
		bool   result;
		pICHashT->get0( key, value_p, result );
		
	//code for debag
		// このように、ポインタからアロー演算子[->]でアクセスさせる分には、ディストラクタが誤って呼ばれてしまうことは無い。
		struct get_data_from_tweet *TD_for_debag = ((struct get_data_from_tweet*)value_p);		//ハッシュテーブルのデータをソート順に読み出し		// printf();関数の値について、全てキャストしてやると、(コードが)長くなるので、一旦構造体にコピーする。
		printf("%lf,%lf,%ld,%llu,%ld,%llu,%llu,%llu,%llu,%llu,%llu,%s,%s,%s\n", TD_for_debag->retweet_efficiency, TD_for_debag->average_reTweet_per_second, TD_for_debag->time_unixtime, TD_for_debag->tweet_id, TD_for_debag->time_unixtime_re, TD_for_debag->tweet_id_re, TD_for_debag->user_id, TD_for_debag->followers_count, TD_for_debag->friends_count, TD_for_debag->retweet_count, TD_for_debag->favorite_count, TD_for_debag->name_re.c_str(), TD_for_debag->screen_name_re.c_str(), TD_for_debag->text.c_str() );
		
		// これをやると、スコープを外れた瞬間に、構造体のディストラクタが呼ばれて、構造体の先にポインタで接続されているデータが吹き飛ぶ。(この問題は、構造体自身はコピー(複製)されているのに対して、構造体がポインタとして持っているデータまでは、複製されていないことが原因となって発生する。)(従って、二つの異なる構造体が、一つのデータに対してポインタで接続されている状況となってしまう。)
//		struct get_data_from_tweet TD_for_debag = *((struct get_data_from_tweet*)value_p);		//ハッシュテーブルのデータをソート順に読み出し		// printf();関数の値について、全てキャストしてやると、(コードが)長くなるので、一旦構造体にコピーする。
//		printf("%lf,%lf,%ld,%llu,%ld,%llu,%llu,%llu,%llu,%llu,%llu,%s,%s,%s\n", TD_for_debag.retweet_efficiency, TD_for_debag.average_reTweet_per_second, TD_for_debag.time_unixtime, TD_for_debag.tweet_id, TD_for_debag.time_unixtime_re, TD_for_debag.tweet_id_re, TD_for_debag.user_id, TD_for_debag.followers_count, TD_for_debag.friends_count, TD_for_debag.retweet_count, TD_for_debag.favorite_count, TD_for_debag.name_re->c_str(), TD_for_debag.screen_name_re->c_str(), TD_for_debag.text.c_str() );
	}
	std::cout << "■■■ print sorted data. (  end  ) ■■■" << std::endl;
	
	
	//前回Bloggerに投稿したデータを削除する。
	SD.TOP_reTweets_IDs  .erase( SD.TOP_reTweets_IDs  .begin(), SD.TOP_reTweets_IDs  .end() );		//配列内のデータを全て削除
	for(auto itr = SD.TOP_reTweets_NAMEs.begin(); itr != SD.TOP_reTweets_NAMEs.end(); ++itr) {
		//[std::string]の解放
		itr->clear();							//文字の長さを0にする。(メモリが解放される訳では無い。)
		itr->shrink_to_fit();				//メモリの解放
	}
	SD.TOP_reTweets_NAMEs.erase( SD.TOP_reTweets_NAMEs.begin(), SD.TOP_reTweets_NAMEs.end() );		//配列内のデータを全て削除
	//メール送信部分の「std::string HTMLCode;」のように、main()関数内の「while(1){}」内で定義されていれば、「while(){}文」を抜けるたびに、
	//破棄されるので、わざわざメモリを削除する必要は無くなる。
	//ただ、構造体として分離すると面倒なので、例えば、ポインタ渡しなどによって、毎回取得し直すようにすれば、この削除の作業は不要となる。
	//→実際速度にどちらが有利かは置いておいて、上手くやってもコードが複雑になる。
	//→しかし、[Send_reply_to_the_tweet_posted_tweet();]関数とかにとっては、この二つの値しか使わないので、むしろ分かれている方が自然かもしれない。
	
	
	//上から[SD.num_of_post]個のデータを取得し、[SD.TOP_reTweets_IDs]及び、[SD.TOP_reTweets_NAMEs]へ格納する。
	if(SD.num_of_post!=0){ // ゼロの場合は、[class PostedTweetsDataBase* &pPT_DB][time_t &timep_LOCAL]に不正な値が入っているので、そもそも使わないように注意。(800件を超えた場合のソートなので。)
		pPT_DB->SaveAndSet_Open( timep_LOCAL );
		for(int num=0; num<SD.num_of_post; num++){
		
			UINT64 key = SORTed_tweet[num].tweet_id_re;
			void* value_p;
			bool  result;
			pICHashT->erase1( key, result, value_p );	//ハッシュテーブルのデータの動的確保先をソート順に読み出しすると同時に、もう使わないデータなので、一緒に削除する。		// [ ((struct get_data_from_tweet*)(void*)value_p) ]
		
			SD.TOP_reTweets_IDs  .push_back(   ((struct get_data_from_tweet*)(void*)value_p)->tweet_id_re       );	//set data to send Blogger and who tweet.
			SD.TOP_reTweets_NAMEs.push_back(  (((struct get_data_from_tweet*)(void*)value_p)->screen_name_re)   );
			
			// DBに保存する。=====
			pPT_DB->SaveAndSet( (struct get_data_from_tweet *&)value_p ); 				// Save posts to the text and Set posts to the hash table.
			//====================
		
			//[value_p]の解放
			delete (struct get_data_from_tweet*)value_p;
		}
		pPT_DB->SaveAndSet_Close();
	}
	
	
	//現在取得した、上から[SD.num_of_post]個のデータ、を除き、100位以内に入らなかったデータを、ハッシュテーブル上から削除する。
	//つまり、データの収集量に不足が無ければ、ハッシュテーブル内のデータ数が、100個になるように調整する。
	for(int delete_num=(100 + SD.num_of_post); delete_num<SD.num_of_element; delete_num++){
		
		UINT64 key = SORTed_tweet[delete_num].tweet_id_re;
		void*  value_p;
		bool   result;
		pICHashT->erase1( key, result, value_p );	//ハッシュテーブルから消去		// [ ((struct get_data_from_tweet*)(void*)value_p) ]
		
		//[value_p]の解放
		delete (struct get_data_from_tweet*)value_p;
	}
	
	//現在ハッシュテーブル上にあるツイートの数を計算する。
	//SD.num_of_element = 100;	//このように書くと、データが足りない場合に数が合わなくなる可能性がある。
	if( SD.num_of_element >= (100 + SD.num_of_post) ){
		SD.num_of_element = 100;
	}else{
		SD.num_of_element -= SD.num_of_post;
	}
	
	//データ収集効率を上げるために、ソートした順番の内、「100番目に面白いと評価されたツイートの、速度」を記録しておき、速度がこれより遅いツイートに関しては、ハッシュテーブルには格納せずに、そのまま破棄したい。
	//そのために、「100番目に面白いと評価されたツイートの、速度」を記録する。
	//ただし、ハッシュテーブル上に十分な量のデータが確認されない場合は、「速度」を「0」としておく事で、(フィルタを通った)全てのツイートをハッシュテーブルに格納する事で、収集を加速させる。
	if(SD.num_of_element>=100){
		UINT64 key = SORTed_tweet[100+SD.num_of_post-1].tweet_id_re;
		void*  value_p;
		bool   result;
		pICHashT->get0( key, value_p, result );
		
		SD.average_reTweet_per_second_100th = ((struct get_data_from_tweet*)(void*)value_p)->average_reTweet_per_second;	//一番速いツイートを除き、100番目(つまり101番目)に速いリツイート・スピードの値を記録する。(num実際にはデータが足りない場合を考えてnum番目となっている。)
	}else{
		SD.average_reTweet_per_second_100th = 0;
	}
//	SD.average_reTweet_per_second_100th = 0;//for debag

	/*
	//[SD.most_fastest_reTweet_s_ID==0]の場合、一番リツイート・スピードの速いツイートを取得する。
	//[SD.most_fastest_reTweet_s_ID!=0]の場合、は何もしない。
	int count_num;
	int num_of_element_for_return__buf = SD.num_of_element;
	if(SD.most_fastest_reTweet_s_ID==0){
		//[SD.most_fastest_reTweet_s_ID==0]の場合は、「1」番目の最も速いツイートのデータのIDをBloggerにメールでポストする為に返し、「2～101」番目をハッシュテーブルに書き戻すようにする。
		SD.most_fastest_reTweet_s_ID   = SORTed_tweet[0].tweet_id_re;		//Bloggerにメールで送信するツイートのIDをコピー。
		SD.most_fastest_reTweet_s_NAME.clear();
		SD.most_fastest_reTweet_s_NAME = SD.hash_table[ SORTed_tweet[0].tweet_id_re ].screen_name_re;	//Bloggerにメールで送信するツイートのNAMEをコピー。
		
		SD.hash_table.erase(SORTed_tweet[0].tweet_id_re);			//ハッシュテーブルから消去
		
	//	SD.num_of_element--;
		num_of_element_for_return__buf--;
		count_num = 101;
	}else{
		//[SD.most_fastest_reTweet_s_ID!=0]の場合は、単にハッシュテーブルの大きさが800件を超えたので、「1～100」番目を書き戻す。最も速いのリツイートもそのままハッシュテーブルに書き戻す。
		count_num = 100;
	}
	
	
	//100位以内に入らなかったデータを消去する。
	for(int delete_num=count_num; delete_num<SD.num_of_element; delete_num++){
		SD.hash_table.erase(SORTed_tweet[delete_num].tweet_id_re);
	}
	if(count_num>=SD.num_of_element){
		SD.average_reTweet_per_second_100th = SD.hash_table[ SORTed_tweet[count_num].tweet_id_re ].average_reTweet_per_second;	//一番速いツイートを除き、100番目(つまり101番目)に速いリツイート・スピードの値を記録する。(num実際にはデータが足りない場合を考えてnum番目となっている。)
	}else{
		SD.average_reTweet_per_second_100th = 0;
	}
	//SD.num_of_element = 100;	//このように書くと、データが足りない場合に数が合わなくなる可能性がある。
	if( SD.num_of_element >= count_num ){
		SD.num_of_element = 100;
	}else{
		SD.num_of_element = num_of_element_for_return__buf;
	}
	*/
	/*
	if(SD.num_of_element>count_num){
		SD.num_of_element=count_num;
	}else{
		if(count_num==101){
			SD.num_of_element--;
		}else if(count_num==100){
			//何もしない。
		}
	}
	*/
	
	return 0;
}



//=========================================================================================================================================================================================================================
//=========================================================================================================================================================================================================================
// [ Process.cpp ] :: end ::
//=========================================================================================================================================================================================================================

