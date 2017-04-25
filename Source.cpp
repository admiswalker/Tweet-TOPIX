#define Release

// デバッグ用の設定
#include "./DebagMode.h"		//デバッグモードに入れるかどうかの定義。(複数ファイルにまたがるため、ヘッダを読み込む。)
								//[Source.cpp][PostedTweetsDataBase.h][ICHashT.h]から読み込まれている。
#define MemoryLogged_enable		//この定義が有効である場合は、[class MemoryLogger* ML = new MemoryLogger();]によって、[ML->MemoryLogged();]のある箇所のメモリ使用量がファイルに書き出される。
#define SendGmail				//デバッグの際に、Gmailを送信するかどうかを決める。				( OR演算で定義しているので、Releaseが定義されている際は、必ず送信される。      )
//#define NumOfLoop 1 			//デバッグの際に、これが定義されている場合は、NumOfLoop回ループする。		( OR演算で定義しているので、Releaseが定義されている際は、必ず無限にループする。 )(InfinityLoopが設定されている場合は、そちらが優先される。)(投稿される数は1ツイートを10回。投稿するタイミングは、必要な数のツイートを取得次第ただちに。)
			//#define num_of_post_Debag 20	//デバック時の投稿ツイート数
#define InfinityLoop			//デバッグの際に、これが定義されている場合は、無限にループする。		( OR演算で定義しているので、Releaseが定義されている際は、必ず無限にループする。 )(この場合、ポストされるタイミングは、自動的に毎時15分と45分になる。また、ポストされる数も、通常の数となる。)
//#define SendReply				//デバッグの際に、これが定義されている場合は、リプライを送信する。	( OR演算で定義しているので、Releaseが定義されている際は、必ず送信される。      )
//#define Post_only_00_15		//デバッグの際であっても、dailyのメールを00:15のタイミングでしか送信しない。( OR演算で定義しているので、Releaseが定義されている際は、00:15でしか送信されない。      )
//#define UseKeyInterrupter		//[Ctrl+Q]キーで終了できるように設定する。(ただし、これを利用する場合、実行時に[]として実行する必要があるため、作成されるファイルがすべてルート権限になって面倒なのと、間違って終了させてしまいそうなので、デフォルトでは使用しない。)(Releaseが定義されている場合でも、この行が有効でなければ無効)

#define Post_ja			//この定義は、Releaseの定義に影響されない。
#define Post_ja_daily	//この定義は、Releaseの定義に影響されない。
#define Post_en			//この定義は、Releaseの定義に影響されない。
#define Post_en_daily	//この定義は、Releaseの定義に影響されない。
#define Post_ko			//この定義は、Releaseの定義に影響されない。

//□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■
#ifdef Release
	// リリース用の設定
#else
	// デバッグ用の設定
#endif //Release

#if defined(Release) || defined(SendGmail)
#else
#endif //defined(Release) || defined(SendGmail)

#if defined(Release) || defined(InfinityLoop)
#else
#endif //defined(Release) || defined(InfinityLoop)

#if defined(Release) || defined(SendReply)
#else
#endif //defined(Release) || defined(SendReply)

#if defined(Release) || defined(Post_only_00_15)
#else
#endif //defined(Release) || defined(Post_only_00_15)

#if defined(UseKeyInterrupter)
#else
#endif //defined(UseKeyInterrupter)

#if defined(Post_ja)
#else
#endif //defined(Post_ja)

#if defined(Post_ja_daily)
#else
#endif //defined(Post_ja_daily)

#if defined(Post_en)
#else
#endif //defined(Post_en)

#if defined(Post_en_daily)
#else
#endif //defined(Post_en_daily)
//□■□■□■□■□■□■□■□■□■□■□■□■□■□■□■

//実行方法
//	01.UseKeyInterrupterが定義されていない場合
//		[$ ./EXEfile]
//
//	02.UseKeyInterrupterが定義されている場合
//		[$ sudo ./EXEfile]
//
//		権限が足りない場合下記の命令が実行できない。
//		[int result_sys = system("sudo ntpdate ntp.nict.jp");	//コマンドを実行	//Ubuntu 15.01]←こちらに関しては、いずれにしてもパスワードを要求される。
//		[class KeyInterrupter* KI = new KeyInterrupter();	//プログラムを実行する際に[$ sudo ./EXEfile]のように実行しないと、キーイベントを取得する先のディレクトリにアクセスできず、失敗する。]←現在は権限が足りない場合、落ちるように設定してある。


#include "./My_function/TwitterFunctions.h"	// [ GetTweet_HTML_Code() ],[ Send_reply_to_the_tweet_posted_tweet() ]

#include <stdlib.h>	//[system();]関数に必要。
#include <stdio.h>
#include <string>
#include <iostream>		//標準出力?のcoutに必要。
//#include <sys/time.h>	//gettimeofday()
#include <time.h>


#include "./My_function/Make_Connection.h"
#include "./My_function/GetOneTweetJSON.h"
#include "./My_function/Process.h"
#include "./My_function/ICHashT.h"			// [ InsertedChainedHashTable ] :: 自作ハッシュテーブル
#include "./My_function/Filters.h"
	#ifdef MemoryLogged_enable
#include "./My_function/MemoryLogger.h"		//for debag
	#endif //MemoryLogged_enable
//#include "./My_function/PostedTweetsDataBase.h"//インクルードが再帰的になって、コンパイルできなくなるので、mainのファイルではコメントアウト
#include "./My_function/SendGmail.h"
	#if defined(UseKeyInterrupter)
#include "./My_function/KeyInterrupter.h"
	#endif //defined(UseKeyInterrupter)
#include "./MyOAuthFunc/Generate_CustomHeadre_of_Authorization.h"	//Generate_CustomHeadre_of_Authorization()

//========================================================================================================================================
//========================================================================================================================================


//[class GetTweet]を利用した関数。
int GetTweet_Initialize_infinity( class GetTweet* &GT,  struct OAuth_parameters &OA ){
	while(1){
		int result = GT->GetTweet_Initialize( OA );
		if(result==0){return 0;}	//通信エラーが発生した場合は、もう一周。	//(もし回線断などの場合は、mallocとfreeを延々と繰り返す事になるので、何とかする。)
		std::cout << "GetTweet():Communication error has occurred. 10 seconds later, system tries to reconnect.";
		fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
		sleep(10);	//10[second]
		std::cout << " → System is trying to reconnect." << std::endl;
	}
}


//========================================================================================================================================
//========================================================================================================================================


bool HashTable_Data_StockOrRenewOrThrowAway( struct get_data_from_tweet*& TD, ICHashT*& pICHashT, struct sort_data& SD ){
	// struct get_data_from_tweet*& TD       :: 格納対象のデータ。
	//                    ICHashT*& pICHashT :: ハッシュテーブルへのポインタ 
	//           struct sort_data & SD       :: 100位のツイートのリツイートスピードが記録されているので、必要。
	
	// true  :: ハッシュテーブルにデータを格納した場合。
	// false :: ハッシュテーブルにデータを格納しなかった場合。
	
	//前回ソートしたデータの100番目に大きな値より大きいか小さいか比較し、小さい場合は何もせずにreturn;。大きい場合は次の処理へ進む。
	if( (TD->average_reTweet_per_second) < SD.average_reTweet_per_second_100th ){
		printf("S");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
		fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
//		delete TD;			//メモリの解放
		return false;
	}
	
	
	// TD->tweet_id_re がハッシュテーブルに登録されているか?
	void** return_value_pp;
	bool   result;			// set2();関数の実行結果が書き込まれる。
	pICHashT->set2( TD->tweet_id_re, (void*&)TD, return_value_pp, result );		//set2():keyが既に格納されている場合は、valueを取得してresultを書き込んでreturn;(valueには手を加えないが、voidポインタなので、中身の判断と書き換えができる。)
	
	
	if( result == false ){
		//ハッシュテーブルにkeyが登録されている場合。
		printf("◎");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
		
		//もし登録されているデータよりも、今回取得したデータのリツイート・スピードの方が速ければ、リツイート・スピードを上書きする。
		if( (TD->average_reTweet_per_second) > ((*((struct get_data_from_tweet**)return_value_pp))->average_reTweet_per_second) ){ // [ (((struct get_data_from_tweet*)*((struct get_data_from_tweet**)(void**)return_value_pp)) ]
			((*((struct get_data_from_tweet**)return_value_pp))->average_reTweet_per_second) = (TD->average_reTweet_per_second);
			printf("←〆");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
		}
		
//		delete TD;			//メモリの解放 ( 同じツイートに対して、二つ同じ内容(速度とか以外)のメモリを持つことになるので、消す。 )この場合、[set2();関数] では、valueは書き換えられていないので、ハッシュテーブル上の方ではなく、こちらを消す。
		
		fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
		return false;
	}else{
		//ハッシュテーブルにkeyが登録されていない場合。
		SD.num_of_element++;
		printf("□");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
		fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
	}
	
	//*
	//ソート効率が落ちるので、ハッシュテーブル上にツイートデータが800件以上溜まっている場合は、ソートして、上位100件のみにする。
	//また、毎時15分と45分にもソートして、上位100件だけにすると共に、リツイート・スピードが最も速いツイートを、メールでBloggerに投稿する。(ここでは時間のチェックだけ。)
	if( SD.num_of_element>=800 ){
		
		int num_of_post_buf = SD.num_of_post;	//単にハッシュテーブルの大きさが800件を超えたので、[SD.num_of_post==0]とする事で、「1～100」番目を書き戻し、101番目以降を破棄する。(余り要素が多くなると、ソート効率が低下するため。)
		SD.num_of_post=0;							//この際に、特別なフラグを用意するのは手間なので、[SD.num_of_post]を0にする事で対応する。
														//変更した[SD.num_of_post]は、最後に書き戻す。
														//
														//注意:非同期的に[SD.num_of_post]をユーザーが変更できるようなコードを書く場合は、最悪のタイミングで書き換えが発生しないように注意!!!!!
		class PostedTweetsDataBase* pPT_DB;//この場合には、実際にアクセスされることはないので、いいかげんに。
		time_t timep_LOCAL;					//この場合には、実際にアクセスされることはないので、いいかげんに。
		sort_tweet( pICHashT, SD, pPT_DB, timep_LOCAL );
		//SD.num_of_element = 100;//「sort_tweet();」関数内部でこのように代入されている。(正確には、おおむね100になるが、足りない場合は100を下回るので注意。)
		
		SD.num_of_post = num_of_post_buf;	//値を書き戻す。
	}//*/
	
	return true;
}


//========================================================================================================================================
//========================================================================================================================================
void GetTweet_HTML_Code_with_LOOP
(
	   struct OAuth_parameters  &OA,
	               std::string  &language,
	   struct        sort_data  &SD,
      std::vector<std::string>  &HTMLCode
)
{
	/*
//	SD.most_fastest_reTweet_s_ID = 584455082453762050;		//for debag
	//失敗したら、データの出力操作は行わず、今回のデータは破棄し、ツイートの取得に戻る。
	int result;
	for( int num=0; num<SD.num_of_post; num++ ){
		result = GetTweet_HTML_Code( OA, SD.TOP_reTweets_IDs[num], HTMLCode[num] );	//Bloggerに投稿するために、twitterAPIでツイートのIDから、ブログに貼り付けるためのHTMLタグを貰ってくる。
																									//[sort_data]構造体をそのまま渡さないのは、配列のどの値かを指定する必要があるから。
		if(result!=0){break;}
	}
	if(result!=0){return -1;}
	*/
	//失敗しても、成功するまで、何度でもやり直す。		//APIの使用制限などで失敗した場合に処理がどうなるのかを、確認しておく必要がある。
													//既に取得対象のツイートが削除されている場合の動作を確認しておく必要がある。
	{//変数名numは他でも使っているので、範囲を限定しておく。
		int break_num=0;//失敗した回数をカウントし、上限を越えたらそのツイートの取得は諦めて、次のツイートの取得へ移る。
		int num=0;
		while( num<SD.num_of_post ){
			int result = GetTweet_HTML_Code( OA, language, SD.TOP_reTweets_IDs[num], HTMLCode[num] );	//Bloggerに投稿するために、twitterAPIでツイートのIDから、ブログに貼り付けるためのHTMLタグを貰ってくる。
																										//[sort_data]構造体をそのまま渡さないのは、配列のどの値かを指定する必要があるから。
			if(result<=-2){
				break_num=0;
				num++;
				continue;	//通信以外のエラー(鍵アカや、既に削除されている場合、は、次のツイートの取得へ移行する。)
			}
			if(break_num<=5){
				if(result!=0){	//for文でcontinueすると、numが加算されてしまうので、注意。
					std::cout << "GetTweet_HTML_Code():Communication error has occurred. 10 seconds later, system tries to reconnect.";
					fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
					sleep(10);	//10[second]
					std::cout << " → System is trying to reconnect." << std::endl;
					break_num++;
					continue;	//通信エラーが発生した場合は、while()文の始めからやり直す。(continue文は処理をスキップする。)
				}
			}else{
				//10回以上失敗した場合は諦める。
				std::cout << "GetTweet_HTML_Code():Communication error has occurred. This failed more than 5 times. Then system gives up retry, and move to next data collection sequence." << std::endl;
				//本来であれば、ここで、足りない分のデータをハッシュテーブルをソートし直して取得すべきだが、面倒なので、後で実装する。
			}
			break_num=0;
			num++;
		}
	}
}

//========================
void SendHTMLmailByGmail_with_LOOP( Mail_struct  &Mail )
{
	/*
	//失敗したら、データの出力操作は行わず、今回のデータは破棄し、ツイートの取得に戻る。
	result = SendHTMLmailByGmail( Mail );	//BloggerにGmailで投稿する。
	if(result!=0){return -1;}
	*/
	//失敗しても、成功するまで、何度でもやり直す。		//APIの使用制限などで失敗した場合に処理がどうなるのかを、確認しておく必要がある。
													//失敗と認識されていても、ギリギリデータが送信されている場合が起こり得るかどうか、確認する必要がある。→おそらく、最後のwriteに成功して、readに失敗した場合は、まさにこのパターンとなる。
	{
		int break_num=0;//失敗した回数をカウントし、上限を越えたらbreakする。
		while(1){
			int result = SendHTMLmailByGmail( Mail );	//BloggerにGmailで投稿する。
			if(break_num>=1){break;}//1回以上失敗した場合は諦める。//多重投稿されている原因の可能性があるので……。(元々は5回以上の設定だった……。)
			if(result!=0){	//for文でcontinueすると、numが加算されてしまうので、注意。
				std::cout << "SendHTMLmailByGmail():Communication error has occurred. 10 seconds later, system tries to reconnect.";
				fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
				sleep(10);	//10[second]
				std::cout << " → System is trying to reconnect." << std::endl;
				break_num++;
				continue;	//通信エラーが発生した場合は、while()文の始めからやり直す。(continue文は処理をスキップする。)
			}
			break;
		}
	}
}

void Send_reply_to_the_tweet_posted_tweet_with_LOOP
(
	   struct OAuth_parameters  &OA,
	               std::string  &language,
	                       int  &num_of_send_reply,
	   struct        sort_data  &SD,
	               std::string  &BlogURL
)
{
	//失敗したら、データの出力操作は行わず、今回のデータは破棄し、ツイートの取得に戻る。
//	for(int num=0; num<num_of_send_reply; num++){
//		result = Send_reply_to_the_tweet_posted_tweet( OA, SD.TOP_reTweets_IDs[num], SD.TOP_reTweets_NAMEs[num] );	//[sort_data]構造体をそのまま渡さないのは、配列のどの値かを指定する必要があるから。
//		if(result!=0){break;}
//	}
//	if(result!=0){return -1;}
	
	//失敗しても、成功するまで、何度でもやり直す。
	{//変数名numは他でも使っているので、範囲を限定しておく。
		int break_num=0;//失敗した回数をカウントし、上限を越えたらbreakする。
		int num=0;
		while( num<num_of_send_reply ){	//for文でcontinueすると、numが加算されてしまうので、注意。
		
			//Twitterでの表示が逆順
		//	int result = Send_reply_to_the_tweet_posted_tweet( OA, language, SD.TOP_reTweets_IDs[num], SD.TOP_reTweets_NAMEs[num] );	//[sort_data]構造体をそのまま渡さないのは、配列のどの値かを指定する必要があるから。
			
			//Twitterでの表示が正順
			int result = Send_reply_to_the_tweet_posted_tweet( OA, language, SD.TOP_reTweets_IDs[(num_of_send_reply-1)-num], SD.TOP_reTweets_NAMEs[(num_of_send_reply-1)-num], BlogURL );	//[sort_data]構造体をそのまま渡さないのは、配列のどの値かを指定する必要があるから。			
			
			if(break_num<=5){
				if(result!=0){
					std::cout << "Send_reply_to_the_tweet_posted_tweet():Communication error has occurred. 10 seconds later, system tries to reconnect.";
					fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
					sleep(10);	//10[second]
					std::cout << " → System is trying to reconnect." << std::endl;
					break_num++;
					continue;	//通信エラーが発生した場合は、while()文の始めからやり直す。(continue文は処理をスキップする。)
				}
			}else{
				//5回以上失敗した場合は諦める。
				std::cout << "Send_reply_to_the_tweet_posted_tweet():Communication error has occurred. This failed more than 5 times. Then system gives up retry, and move to next data collection sequence." << std::endl;
			}
			break_num=0;
			num++;
		}
	}
}

int SortHashTable_GetHTMLode_SendGmail_SaveInTheDataBase_SendReply
(
	   struct OAuth_parameters  &OA,
	               std::string  &language,
	                       int  &num_of_send_reply,
	               Mail_struct  &Mail,
	                   ICHashT* &pICHashT,
	   struct        sort_data  &SD,
	class PostedTweetsDataBase* &pPT_DB,
	                    time_t  &timep_LOCAL,
	               std::string  &BlogURL
)
{
	std::cout << "■■■ Sort. ( start ) ■■■";
	sort_tweet( pICHashT, SD, pPT_DB , timep_LOCAL );//SD.num_of_element = 100;//「sort_tweet();」関数内部でこのように代入されている。(正確には、おおむね100になるが、足りない場合は100を下回るので注意。)
	std::cout << "■■■ Sort. (  end  ) ■■■" << std::endl;
	std::cout << std::endl;
	
	std::cout << "■■■ Get HTML code. ( start ) ■■■" << std::endl;
	std::vector<std::string> HTMLCode(SD.num_of_post);		//指定要素数で[vector]配列を生成。(恐らく、要素数を先に指定しておかないと、確保されていない領域にアクセスして、アクセス違反を引き起こす。)//始めに指定した数以上の要素数を持たせたいのであれば、push_back()によって追加しなければいけない。//マルチ言語対応する場合はSDが言語依存である事に注意。
	GetTweet_HTML_Code_with_LOOP(OA,language,SD,HTMLCode);
	std::cout << "■■■ Get HTML code. (  end  ) ■■■" << std::endl;
	std::cout << std::endl;
	
#if defined(Release) || defined(SendGmail)
	std::cout << "■■■ Send Gmail to Blogger. ( start ) ■■■" << std::endl;
	
	//上手く[GetTweet_HTML_Code();]関数に([std::vector<std::string> HTMLCode]では無く)[std::string HTMLCode]を渡せれば、
		//(例えば、[.length()]で位置を指定したりする。)
		//メモリコピーが不要になるかもしれないが、一応今の使用では(使っていないのでどうかとも思うが、)[levelDB]にHTML Codeを格納する仕様になっているので、
		//バッファにメモリをコピーしてからメールで送信する事とする。
	std::string DATA_buf;	//	現在は、main()関数内の「while(1){}」内で定義されているので、「while(){}文」を抜けるたびに、破棄されるため、[.clear()]を実行する必要は無い。
	for(int num=0; num<SD.num_of_post-1; num++){
		DATA_buf += HTMLCode[num];
		DATA_buf += "<br />";		//HTMLを送信しているのだから、改行させたいのであれば、HTMLタグの改行を送る必要がある。
	//	DATA_buf += "<hr noshade=\"\" />";		//HTMLタグで、水平線		//(ツイートとツイートの境界が分かり難かったので、挿入。)
													//余りいい感じでは無かった。
	}
	DATA_buf += HTMLCode[SD.num_of_post-1];
	
	Mail.DATA = &DATA_buf;			//アドレスをコピー		//[Mail_struct]構造体の[Mail_struct.DATA]の定義をポインタから通常の実体にして、そこに直接代入しても良かったのだが、特にメリットも無いのでこのままにしておく。また、while文の外で定義されており、毎回作り直される訳では無いので、メモリー的には自由度は……上がる??のか??
	
	SendHTMLmailByGmail_with_LOOP(Mail);
	std::cout << "■■■ Send Gmail to Blogger. (  end  ) ■■■" << std::endl;
	std::cout << std::endl;
#endif //defined(Release) || defined(SendGmail)
	
	//なぜか、ここの実装が、おそらくProcess.cppにある。
	std::cout << "■■■ Save the tweets which was posted to the Blogger in the PostedTweetsDB. ( start ) ■■■" << std::endl;
	std::cout << "■■■ Save the tweets which was posted to the Blogger in the PostedTweetsDB. (  end  ) ■■■" << std::endl;
	std::cout << std::endl;
	
#if defined(Release) || defined(SendReply)
	std::cout << "■■■ Send reply. ( start ) ■■■" << std::endl;
	Send_reply_to_the_tweet_posted_tweet_with_LOOP(OA,language,num_of_send_reply,SD,BlogURL);
	std::cout << "■■■ Send reply. (  end  ) ■■■" << std::endl;
//	std::cout << std::endl;
#endif //defined(Release) || defined(SendReply)
	return 0;
}

struct SendMail_daily{
	int num_of_post;
	int post_interval;
	std::vector<std::string> HTMLCode;
	struct Mail_struct Mail;
};

void* thread_SendMail_daily( void* aParam ){
	struct SendMail_daily* MD = (struct SendMail_daily*)aParam;
	
	std::string MailTitle_buf = MD->Mail.Subject;
	
	//post_interval==5;
	for(int step_num=1; step_num<=(MD->num_of_post/MD->post_interval); step_num++){
	#if defined(Release) || defined(SendGmail)
		std::cout << "■■■ Send Gmail to Blogger. ( start ) ■■■" << std::endl;
		
		//上手く[GetTweet_HTML_Code();]関数に([std::vector<std::string> HTMLCode]では無く)[std::string HTMLCode]を渡せれば、
			//(例えば、[.length()]で位置を指定したりする。)
			//メモリコピーが不要になるかもしれないが、一応今の使用では(使っていないのでどうかとも思うが、)[levelDB]にHTML Codeを格納する仕様になっているので、
			//バッファにメモリをコピーしてからメールで送信する事とする。
		std::string DATA_buf;	//	現在は、main()関数内の「while(1){}」内で定義されているので、「while(){}文」を抜けるたびに、破棄されるため、[.clear()]を実行する必要は無い。
		int num =  MD->num_of_post - MD->post_interval*(step_num);
		for(;num < MD->num_of_post - MD->post_interval*(step_num-1) - 1;
		     num++ )
		{
			DATA_buf += MD->HTMLCode[num];
			DATA_buf += "<br />";		//HTMLを送信しているのだから、改行させたいのであれば、HTMLタグの改行を送る必要がある。
		//	DATA_buf += "<hr noshade=\"\" />";		//HTMLタグで、水平線		//(ツイートとツイートの境界が分かり難かったので、挿入。)
													//余りいい感じでは無かった。
		}
		DATA_buf += MD->HTMLCode[num];
		MD->Mail.DATA = &DATA_buf;			//アドレスをコピー		//[Mail_struct]構造体の[Mail_struct.DATA]の定義をポインタから通常の実体にして、そこに直接代入しても良かったのだが、特にメリットも無いのでこのままにしておく。また、while文の外で定義されており、毎回作り直される訳では無いので、メモリー的には自由度は……上がる??のか??
		
		// Set Mail Title
//		std::string MailTitle = (std::string)str_sprintf( "%04d_%02d_%02d_%02d_%02d_%02dof%02d", 1900 + tmp_LOCAL.tm_year, 1 + tmp_LOCAL.tm_mon, tmp_LOCAL.tm_mday, tmp_LOCAL.tm_hour, tmp_LOCAL.tm_min, step_num, (SD.num_of_post/post_interval) );
		MD->Mail.Subject      = (std::string)str_sprintf(                       "%s_%02dof%02d", MailTitle_buf.c_str()                                                                                 , (MD->num_of_post/MD->post_interval) - step_num + 1, (MD->num_of_post/MD->post_interval) );
		SendHTMLmailByGmail_with_LOOP(MD->Mail);
		DATA_buf.clear();
		std::cout << "■■■ Send Gmail to Blogger. (  end  ) ■■■" << std::endl;
		std::cout << std::endl;
	#endif //defined(Release) || defined(SendGmail)
		
		//なぜか、ここの実装が、おそらくProcess.cppにある。
		std::cout << "■■■ Save the tweets which was posted to the Blogger in the PostedTweetsDB. ( start ) ■■■" << std::endl;
		std::cout << "■■■ Save the tweets which was posted to the Blogger in the PostedTweetsDB. (  end  ) ■■■" << std::endl;
//		std::cout << std::endl;
		
		sleep(70);//70秒間のwait
	}
	
//現状ではリプライを送る予定はない。
//#if defined(Release) || defined(SendReply)
//	std::cout << "■■■ Send reply. ( start ) ■■■" << std::endl;
//	Send_reply_to_the_tweet_posted_tweet_with_LOOP(OA,language,num_of_send_reply,SD,BlogURL);
//	std::cout << "■■■ Send reply. (  end  ) ■■■" << std::endl;
//	std::cout << std::endl;
//#endif //defined(Release) || defined(SendReply)
	
	delete MD;
	pthread_exit(NULL);//スレッドの終了
	return 0;
}

int SortHashTable_GetHTMLode_SendGmail_SaveInTheDataBase_SendReply_daily
(
	   struct OAuth_parameters  &OA,
	               std::string  &language,
//	                       int  &num_of_send_reply,
	               Mail_struct  &Mail,
	                   ICHashT* &pICHashT,
	   struct        sort_data  &SD,
	class PostedTweetsDataBase* &pPT_DB,
	                    time_t  &timep_LOCAL,
//	               std::string  &BlogURL
                           int  &post_interval
)
{
	std::cout << "■■■ Sort. ( start ) ■■■";
	sort_tweet( pICHashT, SD, pPT_DB , timep_LOCAL );//SD.num_of_element = 100;//「sort_tweet();」関数内部でこのように代入されている。(正確には、おおむね100になるが、足りない場合は100を下回るので注意。)
	std::cout << "■■■ Sort. (  end  ) ■■■" << std::endl;
	std::cout << std::endl;
	
	std::cout << "■■■ Get HTML code. ( start ) ■■■" << std::endl;
	std::vector<std::string> HTMLCode(SD.num_of_post);		//指定要素数で[vector]配列を生成。(恐らく、要素数を先に指定しておかないと、確保されていない領域にアクセスして、アクセス違反を引き起こす。)//始めに指定した数以上の要素数を持たせたいのであれば、push_back()によって追加しなければいけない。//マルチ言語対応する場合はSDが言語依存である事に注意。
	GetTweet_HTML_Code_with_LOOP(OA,language,SD,HTMLCode);
	std::cout << "■■■ Get HTML code. (  end  ) ■■■" << std::endl;
	std::cout << std::endl;
	
	//メール送信用のスレッドを起動する。(Bloggerは1分以上間隔を開けてポストしないと、正常に認識しないため、70秒のスリープを挟みつつポストするため。)
	//クリティカルセクションを回避するために、必要なデータはすべて動的に確保したメモリにコピーしてから渡す。(開放の責任はスレッドが持つ。)(静的に確保すると、メインのスレッドがスコープから外れた場合に、メモリが開放されてしまう。)
	struct SendMail_daily* Mail_daily = new (struct SendMail_daily);
	Mail_daily->num_of_post   = SD.num_of_post;
	Mail_daily->post_interval = post_interval;
	Mail_daily->HTMLCode      = HTMLCode;
	Mail_daily->Mail          = Mail;
	
	pthread_attr_t tattr;
	pthread_attr_init(&tattr);
	pthread_attr_setdetachstate(&tattr,PTHREAD_CREATE_DETACHED);
	pthread_t pthread;
	pthread_create(&pthread,&tattr,thread_SendMail_daily,Mail_daily);
	
	return 0;
}

//========================================================================================================================================
//========================================================================================================================================

int main(int argc, char *argv[]){
	#ifdef Release
		// リリース用の設定
	#else
		// デバッグ用の設定
		printf("Debag mode!!\n");
	#endif //Release
	
#ifdef Release
	// リリース用の設定
		//for distribute
		//OAuth認証用のコード
	
		//Japanese
	struct OAuth_parameters OA_ja;
	OA_ja.oauth_consumer_key    = URL_encode_RFC_3986( ""                           );	//(API Key)		//「oauth_consumer_key」   は、英数字なので、URLencodeしても変化が無く、無意味だが、定義上URLencodeする事になっている。
	OA_ja.oauth_consumer_secret = URL_encode_RFC_3986( ""  );	//(API Secret)	//「oauth_consumer_secret」は、英数字なので、URLencodeしても変化が無く、無意味だが、定義上URLencodeする事になっている。
	OA_ja.oauth_token           = URL_encode_RFC_3986( ""  );
	OA_ja.oauth_token_secret    = URL_encode_RFC_3986( ""       );
	
		//Japanese-daily
	struct OAuth_parameters OA_ja_daily;
	//Generate_CustomHeadre_of_Authorization()の内部でencodeすると、encodeしているのかしていないのか、外から分かり難くなるので、先にエンコードしてから渡す事にする。
	OA_ja_daily.oauth_consumer_key    = URL_encode_RFC_3986( ""                           );	//(API Key)		//「oauth_consumer_key」   は、英数字なので、URLencodeしても変化が無く、無意味だが、定義上URLencodeする事になっている。
	OA_ja_daily.oauth_consumer_secret = URL_encode_RFC_3986( ""  );	//(API Secret)	//「oauth_consumer_secret」は、英数字なので、URLencodeしても変化が無く、無意味だが、定義上URLencodeする事になっている。
	OA_ja_daily.oauth_token           = URL_encode_RFC_3986( ""  );
	OA_ja_daily.oauth_token_secret    = URL_encode_RFC_3986( ""       );
	
		//English
	struct OAuth_parameters OA_en;
	//Generate_CustomHeadre_of_Authorization()の内部でencodeすると、encodeしているのかしていないのか、外から分かり難くなるので、先にエンコードしてから渡す事にする。
	OA_en.oauth_consumer_key    = URL_encode_RFC_3986( ""                           );	//(API Key)		//「oauth_consumer_key」   は、英数字なので、URLencodeしても変化が無く、無意味だが、定義上URLencodeする事になっている。
	OA_en.oauth_consumer_secret = URL_encode_RFC_3986( ""  );	//(API Secret)	//「oauth_consumer_secret」は、英数字なので、URLencodeしても変化が無く、無意味だが、定義上URLencodeする事になっている。
	OA_en.oauth_token           = URL_encode_RFC_3986( ""  );
	OA_en.oauth_token_secret    = URL_encode_RFC_3986( ""       );
	
		//English-daily
	struct OAuth_parameters OA_en_daily;
	//Generate_CustomHeadre_of_Authorization()の内部でencodeすると、encodeしているのかしていないのか、外から分かり難くなるので、先にエンコードしてから渡す事にする。
	OA_en_daily.oauth_consumer_key    = URL_encode_RFC_3986( ""                           );	//(API Key)		//「oauth_consumer_key」   は、英数字なので、URLencodeしても変化が無く、無意味だが、定義上URLencodeする事になっている。
	OA_en_daily.oauth_consumer_secret = URL_encode_RFC_3986( ""  );	//(API Secret)	//「oauth_consumer_secret」は、英数字なので、URLencodeしても変化が無く、無意味だが、定義上URLencodeする事になっている。
	OA_en_daily.oauth_token           = URL_encode_RFC_3986( ""  );
	OA_en_daily.oauth_token_secret    = URL_encode_RFC_3986( ""       );
	
		//Korean
	struct OAuth_parameters OA_ko;
	//Generate_CustomHeadre_of_Authorization()の内部でencodeすると、encodeしているのかしていないのか、外から分かり難くなるので、先にエンコードしてから渡す事にする。
	OA_ko.oauth_consumer_key    = URL_encode_RFC_3986( ""                           );	//(API Key)		//「oauth_consumer_key」   は、英数字なので、URLencodeしても変化が無く、無意味だが、定義上URLencodeする事になっている。
	OA_ko.oauth_consumer_secret = URL_encode_RFC_3986( ""  );	//(API Secret)	//「oauth_consumer_secret」は、英数字なので、URLencodeしても変化が無く、無意味だが、定義上URLencodeする事になっている。
	OA_ko.oauth_token           = URL_encode_RFC_3986( ""  );
	OA_ko.oauth_token_secret    = URL_encode_RFC_3986( ""       );
	
	//Gmailの設定
		//Japanese
		//http://tweet-topix.blogspot.jp/
	struct Mail_struct Mail_ja;
	Mail_ja.user    = "";
	Mail_ja.pass    = "";
	Mail_ja.RCPT_TO = "";	//送信先	(投稿先)
//	Mail_ja.RCPT_TO = "";							//送信先	(投稿先)		//Bloggerは投稿制限が厳しいので、Gmailの方に送ってdebagする。

		//Japanese-daily
		//http://tweet-topix-daily.blogspot.jp/
	struct Mail_struct Mail_ja_daily;
	Mail_ja_daily.user    = "";
	Mail_ja_daily.pass    = "";
	Mail_ja_daily.RCPT_TO = "";	//送信先	(投稿先)
//	Mail_ja_daily.RCPT_TO = "";					//送信先	(投稿先)		//Bloggerは投稿制限が厳しいので、Gmailの方に送ってdebagする。

		//English
		//http://tweet-topix-en.blogspot.jp/
	struct Mail_struct Mail_en;
	Mail_en.user    = "";
	Mail_en.pass    = "";
	Mail_en.RCPT_TO = "";	//送信先	(投稿先)
//	Mail_en.RCPT_TO = "";							//送信先	(投稿先)		//Bloggerは投稿制限が厳しいので、Gmailの方に送ってdebagする。

		//English-daily
		//http://tweet-topix-en-daily.blogspot.jp/
	struct Mail_struct Mail_en_daily;
	Mail_en_daily.user    = "";
	Mail_en_daily.pass    = "";
	Mail_en_daily.RCPT_TO = "";	//送信先	(投稿先)
//	Mail_en_daily.RCPT_TO = "";					//送信先	(投稿先)		//Bloggerは投稿制限が厳しいので、Gmailの方に送ってdebagする。

		//Korean
		//http://tweet-topix-ko-for-debag.blogspot.com/
	struct Mail_struct Mail_ko;
	Mail_ko.user    = "";
	Mail_ko.pass    = "";
	Mail_ko.RCPT_TO = "";			//送信先	(投稿先)
//	Mail_ko.RCPT_TO = "";							//送信先	(投稿先)		//Bloggerは投稿制限が厳しいので、Gmailの方に送ってdebagする。

#else
	// デバッグ用の設定
		//for debag
		//OAuth認証用のコード
	
		//Japanese
	struct OAuth_parameters OA_ja;
	//Generate_CustomHeadre_of_Authorization()の内部でencodeすると、encodeしているのかしていないのか、外から分かり難くなるので、先にエンコードしてから渡す事にする。
	OA_ja.oauth_consumer_key    = URL_encode_RFC_3986( ""                           );	//(API Key)		//「oauth_consumer_key」   は、英数字なので、URLencodeしても変化が無く、無意味だが、定義上URLencodeする事になっている。
	OA_ja.oauth_consumer_secret = URL_encode_RFC_3986( ""  );	//(API Secret)	//「oauth_consumer_secret」は、英数字なので、URLencodeしても変化が無く、無意味だが、定義上URLencodeする事になっている。
	OA_ja.oauth_token           = URL_encode_RFC_3986( ""  );
	OA_ja.oauth_token_secret    = URL_encode_RFC_3986( ""       );
	
		//Japanese-daily
	struct OAuth_parameters OA_ja_daily;
	//Generate_CustomHeadre_of_Authorization()の内部でencodeすると、encodeしているのかしていないのか、外から分かり難くなるので、先にエンコードしてから渡す事にする。
	OA_ja_daily.oauth_consumer_key    = URL_encode_RFC_3986( ""                           );	//(API Key)		//「oauth_consumer_key」   は、英数字なので、URLencodeしても変化が無く、無意味だが、定義上URLencodeする事になっている。
	OA_ja_daily.oauth_consumer_secret = URL_encode_RFC_3986( ""  );	//(API Secret)	//「oauth_consumer_secret」は、英数字なので、URLencodeしても変化が無く、無意味だが、定義上URLencodeする事になっている。
	OA_ja_daily.oauth_token           = URL_encode_RFC_3986( ""  );
	OA_ja_daily.oauth_token_secret    = URL_encode_RFC_3986( ""       );

		//English
	struct OAuth_parameters OA_en;
	//Generate_CustomHeadre_of_Authorization()の内部でencodeすると、encodeしているのかしていないのか、外から分かり難くなるので、先にエンコードしてから渡す事にする。
	OA_en.oauth_consumer_key    = URL_encode_RFC_3986( ""                           );	//(API Key)		//「oauth_consumer_key」   は、英数字なので、URLencodeしても変化が無く、無意味だが、定義上URLencodeする事になっている。
	OA_en.oauth_consumer_secret = URL_encode_RFC_3986( ""  );	//(API Secret)	//「oauth_consumer_secret」は、英数字なので、URLencodeしても変化が無く、無意味だが、定義上URLencodeする事になっている。
	OA_en.oauth_token           = URL_encode_RFC_3986( ""  );
	OA_en.oauth_token_secret    = URL_encode_RFC_3986( ""       );
	
		//English-daily
	struct OAuth_parameters OA_en_daily;
	//Generate_CustomHeadre_of_Authorization()の内部でencodeすると、encodeしているのかしていないのか、外から分かり難くなるので、先にエンコードしてから渡す事にする。
	OA_en_daily.oauth_consumer_key    = URL_encode_RFC_3986( ""                           );	//(API Key)		//「oauth_consumer_key」   は、英数字なので、URLencodeしても変化が無く、無意味だが、定義上URLencodeする事になっている。
	OA_en_daily.oauth_consumer_secret = URL_encode_RFC_3986( ""  );	//(API Secret)	//「oauth_consumer_secret」は、英数字なので、URLencodeしても変化が無く、無意味だが、定義上URLencodeする事になっている。
	OA_en_daily.oauth_token           = URL_encode_RFC_3986( ""  );
	OA_en_daily.oauth_token_secret    = URL_encode_RFC_3986( ""       );
	
		//Korean
	struct OAuth_parameters OA_ko;
	//Generate_CustomHeadre_of_Authorization()の内部でencodeすると、encodeしているのかしていないのか、外から分かり難くなるので、先にエンコードしてから渡す事にする。
	OA_ko.oauth_consumer_key    = URL_encode_RFC_3986( ""                           );	//(API Key)		//「oauth_consumer_key」   は、英数字なので、URLencodeしても変化が無く、無意味だが、定義上URLencodeする事になっている。
	OA_ko.oauth_consumer_secret = URL_encode_RFC_3986( ""  );	//(API Secret)	//「oauth_consumer_secret」は、英数字なので、URLencodeしても変化が無く、無意味だが、定義上URLencodeする事になっている。
	OA_ko.oauth_token           = URL_encode_RFC_3986( ""  );
	OA_ko.oauth_token_secret    = URL_encode_RFC_3986( ""       );
	
	//Gmailの設定
		//Japanese
		//http://tweet-topix-debag.blogspot.jp/
	struct Mail_struct Mail_ja;
	Mail_ja.user    = "";
	Mail_ja.pass    = "";
	Mail_ja.RCPT_TO = "";	//送信先	(投稿先)
//	Mail_ja.RCPT_TO = "";							//送信先	(投稿先)		//Bloggerは投稿制限が厳しいので、Gmailの方に送ってdebagする。
	
		//Japanese-daily
		//http://tweet-topix-daily.blogspot.jp/
	struct Mail_struct Mail_ja_daily;
	Mail_ja_daily.user    = "";
	Mail_ja_daily.pass    = "";
	Mail_ja_daily.RCPT_TO = "";	//送信先	(投稿先)
//	Mail_ja_daily.RCPT_TO = "";					//送信先	(投稿先)		//Bloggerは投稿制限が厳しいので、Gmailの方に送ってdebagする。
	
		//English
		//http://tweet-topix-en-debag.blogspot.jp/
	struct Mail_struct Mail_en;
	Mail_en.user    = "";
	Mail_en.pass    = "";
	Mail_en.RCPT_TO = "";	//送信先	(投稿先)
//	Mail_en.RCPT_TO = "";							//送信先	(投稿先)		//Bloggerは投稿制限が厳しいので、Gmailの方に送ってdebagする。
	
		//English-daily
		//http://tweet-topix-en-daily.blogspot.jp/
	struct Mail_struct Mail_en_daily;
	Mail_en_daily.user    = "";
	Mail_en_daily.pass    = "";
	Mail_en_daily.RCPT_TO = "";	//送信先	(投稿先)
//	Mail_en_daily.RCPT_TO = "";					//送信先	(投稿先)		//Bloggerは投稿制限が厳しいので、Gmailの方に送ってdebagする。
	
		//Korean
		//http://tweet-topix-ko-for-debag.blogspot.com/
	struct Mail_struct Mail_ko;
	Mail_ko.user    = "";
	Mail_ko.pass    = "";
	Mail_ko.RCPT_TO = "";		//送信先	(投稿先)
//	Mail_ko.RCPT_TO = "";							//送信先	(投稿先)		//Bloggerは投稿制限が厳しいので、Gmailの方に送ってdebagする。

#endif //Release
	
	std::cout << "■■■ StartUP ( start ) ■■■" << std::endl;
	
	//*
	//仮想環境で実行しているため、マシンをスリープにいれた場合などに、時刻がずれて、twitterの認証に失敗するので、強制的に時刻を合わせておく。
	{
//		int result_sys = system("/usr/sbin/ntpdate ntp.nict.jp");	//コマンドを実行	//CentOS7
		int result_sys = system("sudo ntpdate ntp.nict.jp");	//コマンドを実行	//Ubuntu 15.01
		if(result_sys!=0){
			printf("Time set command was failed. ERROR CODE :: %d\n",result_sys);
			return 0;
		}
	}
	//*/
	
	//実行中のメモリを監視する。メモリの使用量をファイルに書き出し。	//for debag
#ifdef MemoryLogged_enable
	class MemoryLogger* ML = new MemoryLogger();	//コンストラクタの呼び出し。
	ML->MemoryLogged();
#endif //MemoryLogged_enable
	
	//Sortされたデータのやり取りをする構造体の初期化
		//Japanese
	struct sort_data SD_ja;
#if defined(Release) || defined(InfinityLoop)	// リリース用の設定
	SD_ja.num_of_post                      = 6;	//毎時15分45分のpostでBloggerに投稿するtweetの数。
#else				// デバッグ用の設定
	SD_ja.num_of_post                      = 6;//1;	//毎時15分45分のpostでBloggerに投稿するtweetの数。
#endif //defined(Release) || defined(InfinityLoop)
	SD_ja.num_of_element                   = 0;	//初期化しておかないと、アクセス違反を引き起こす。
	SD_ja.average_reTweet_per_second_100th = 0;	//初期化しておかないと、起動時に動かない事になる。
	int num_of_send_reply_ja = 6;						//Bloggerにポストしたツイート全てに対して、replyを送信すると、ツイッターに規制されるので、数を指定する。(最大6まで。それ以上は規制される。)
	if(num_of_send_reply_ja>=SD_ja.num_of_post){
		num_of_send_reply_ja = SD_ja.num_of_post;		//指定した数が多すぎる場合は、書き換える。
	}
	
		//Japanese_daily
	struct sort_data SD_ja_daily;
#if defined(Release) || defined(Post_only_00_15)		// リリース用の設定
	SD_ja_daily.num_of_post                      = 100;	//00:15のpostでBloggerに投稿するtweetの数。
#else				// デバッグ用の設定
	SD_ja_daily.num_of_post                      = 10;	//00:15のpostでBloggerに投稿するtweetの数。
#endif //defined(Release) || defined(InfinityLoop)
	SD_ja_daily.num_of_element                   = 0;	//初期化しておかないと、アクセス違反を引き起こす。
	SD_ja_daily.average_reTweet_per_second_100th = 0;	//初期化しておかないと、起動時に動かない事になる。
	int num_of_send_reply_ja_daily = 6;						//Bloggerにポストしたツイート全てに対して、replyを送信すると、ツイッターに規制されるので、数を指定する。(最大6まで。それ以上は規制される。)
	if(num_of_send_reply_ja_daily>=SD_ja_daily.num_of_post){
		num_of_send_reply_ja_daily = SD_ja_daily.num_of_post;		//指定した数が多すぎる場合は、書き換える。
	}
	int post_interval_ja_daily                   = 5;	//5ツイート間隔でポストしていく//必ず、(SD.num_of_post/post_interval)で余りが発生しないような値にする。(ポストする際にバグる。)
#ifdef Release
	// リリース用の設定
	if(post_interval_ja_daily>=SD_ja_daily.num_of_post){
		post_interval_ja_daily = SD_ja_daily.num_of_post;		//指定した数が多すぎる場合は、書き換える。
	}
#else
	// デバッグ用の設定
//	post_interval_ja_daily = SD_ja_daily.num_of_post;
#endif //Release
	
		//English
	struct sort_data SD_en;
#if defined(Release) || defined(InfinityLoop)		// リリース用の設定
	SD_en.num_of_post                      = 10;	//毎時15分45分のpostでBloggerに投稿するtweetの数。
#else				// デバッグ用の設定
	SD_en.num_of_post                      = 10;//1;		//毎時15分45分のpostでBloggerに投稿するtweetの数。
#endif //defined(Release) || defined(InfinityLoop)
	SD_en.num_of_element                   = 0;		//初期化しておかないと、アクセス違反を引き起こす。
	SD_en.average_reTweet_per_second_100th = 0;		//初期化しておかないと、起動時に動かない事になる。
	int num_of_send_reply_en = 6;						//Bloggerにポストしたツイート全てに対して、replyを送信すると、ツイッターに規制されるので、数を指定する。(最大6まで。それ以上は規制される。)
	if(num_of_send_reply_en>=SD_en.num_of_post){
		num_of_send_reply_en = SD_en.num_of_post;		//指定した数が多すぎる場合は、書き換える。
	}
	
		//English_daily
	struct sort_data SD_en_daily;
#if defined(Release) || defined(Post_only_00_15)		// リリース用の設定
	SD_en_daily.num_of_post                      = 100;	//00:15のpostでBloggerに投稿するtweetの数。
#else				// デバッグ用の設定
	SD_en_daily.num_of_post                      = 10;	//00:15のpostでBloggerに投稿するtweetの数。
#endif //defined(Release) || defined(InfinityLoop)
	SD_en_daily.num_of_element                   = 0;	//初期化しておかないと、アクセス違反を引き起こす。
	SD_en_daily.average_reTweet_per_second_100th = 0;	//初期化しておかないと、起動時に動かない事になる。
	int num_of_send_reply_en_daily = 6;						//Bloggerにポストしたツイート全てに対して、replyを送信すると、ツイッターに規制されるので、数を指定する。(最大6まで。それ以上は規制される。)
	if(num_of_send_reply_en_daily>=SD_en_daily.num_of_post){
		num_of_send_reply_en_daily = SD_en_daily.num_of_post;		//指定した数が多すぎる場合は、書き換える。
	}
	int post_interval_en_daily                   = 5;	//5ツイート間隔でポストしていく//必ず、(SD.num_of_post/post_interval)で余りが発生しないような値にする。(ポストする際にバグる。)
#ifdef Release
	// リリース用の設定
	if(post_interval_en_daily>=SD_en_daily.num_of_post){
		post_interval_en_daily = SD_en_daily.num_of_post;		//指定した数が多すぎる場合は、書き換える。
	}
#else
	// デバッグ用の設定
//	post_interval_en_daily = SD_en_daily.num_of_post;
#endif //Release

		//Korean
	struct sort_data SD_ko;
#if defined(Release) || defined(InfinityLoop)		// リリース用の設定
	SD_ko.num_of_post                      = 6;		//毎時15分45分のpostでBloggerに投稿するtweetの数。
#else				// デバッグ用の設定
	SD_ko.num_of_post                      = 6;//1;		//毎時15分45分のpostでBloggerに投稿するtweetの数。
#endif //defined(Release) || defined(InfinityLoop)
	SD_ko.num_of_element                   = 0;		//初期化しておかないと、アクセス違反を引き起こす。
	SD_ko.average_reTweet_per_second_100th = 0;		//初期化しておかないと、起動時に動かない事になる。
	int num_of_send_reply_ko = 6;					//Bloggerにポストしたツイート全てに対して、replyを送信すると、ツイッターに規制されるので、数を指定する。(最大6まで。それ以上は規制される。)
	if(num_of_send_reply_ko>=SD_ko.num_of_post){
		num_of_send_reply_ko = SD_ko.num_of_post;	//指定した数が多すぎる場合は、書き換える。
	}
	
	time_t timep_StartUpDate_LOCAL = time(0);									// time(0) : ローカル(PC上で)のunixtimeを取得する。
	int StartUpDate;
	{
		struct tm *tm_StartUpTime = localtime(&timep_StartUpDate_LOCAL);		// PC上でのローカル時間「unix時間[time_t型]」を、ローカル時間の[tm構造体]へ変換
		StartUpDate = tm_StartUpTime->tm_mday;
	}
	
	// DBの読み込み
	// ハッシュテーブルの初期化と、ハッシュテーブルへのデータのロード。
	class PostedTweetsDataBase* pPT_DB_ja       = new PostedTweetsDataBase( "PostedTweets", "ja"      , timep_StartUpDate_LOCAL );
	class PostedTweetsDataBase* pPT_DB_ja_daily = new PostedTweetsDataBase( "PostedTweets", "ja_daily", timep_StartUpDate_LOCAL );
	class PostedTweetsDataBase* pPT_DB_en       = new PostedTweetsDataBase( "PostedTweets", "en"      , timep_StartUpDate_LOCAL );
	class PostedTweetsDataBase* pPT_DB_en_daily = new PostedTweetsDataBase( "PostedTweets", "en_daily", timep_StartUpDate_LOCAL );
	class PostedTweetsDataBase* pPT_DB_ko       = new PostedTweetsDataBase( "PostedTweets", "ko"      , timep_StartUpDate_LOCAL );
	
	// ハッシュテーブルの確保、及び、初期化
	class InsertedChainedHashTable* pICHashT_ja       = new ICHashT(2048);	//ハッシュテーブルを動的に確保する。	// [class ICHashT XXX;]のようにtypedefされた名前の場合、前に[class]を付けると、コンパイルエラーとなる。
	class InsertedChainedHashTable* pICHashT_ja_daily = new ICHashT(2048);	//ハッシュテーブルを動的に確保する。	// [class ICHashT XXX;]のようにtypedefされた名前の場合、前に[class]を付けると、コンパイルエラーとなる。
	class InsertedChainedHashTable* pICHashT_en       = new ICHashT(2048);	//ハッシュテーブルを動的に確保する。	// [class ICHashT XXX;]のようにtypedefされた名前の場合、前に[class]を付けると、コンパイルエラーとなる。
	class InsertedChainedHashTable* pICHashT_en_daily = new ICHashT(2048);	//ハッシュテーブルを動的に確保する。	// [class ICHashT XXX;]のようにtypedefされた名前の場合、前に[class]を付けると、コンパイルエラーとなる。
	class InsertedChainedHashTable* pICHashT_ko       = new ICHashT(2048);	//ハッシュテーブルを動的に確保する。	// [class ICHashT XXX;]のようにtypedefされた名前の場合、前に[class]を付けると、コンパイルエラーとなる。
	
	struct get_data_from_tweet* TD=NULL;	//必ず初期化する。		// ポインタ用のバッファ(メモリを使わなかった場合は、次も使い回せるように、ポインタ部分だけ外に定義する。)
	//↑↑↑↑↑↑ツイートをBloggerへポストするタイミングとは関係無しにメモリの使用量が上昇している気がする。この変数について、もう一度チェックする。
	
	#if defined(NumOfLoop)
		int LoopCount=0;
	#endif
	
	//実行中のメモリを監視する。メモリの使用量をファイルに書き出し。
#ifdef MemoryLogged_enable
	ML->MemoryLogged();
#endif //MemoryLogged_enable
	
#if defined(UseKeyInterrupter)
	class KeyInterrupter* KI = new KeyInterrupter();	//プログラムを実行する際に[$ sudo ./EXEfile]のように実行しないと、キーイベントを取得する先のディレクトリにアクセスできず、失敗する。
#endif //defined(UseKeyInterrupter)
	
	std::cout << "■■■ StartUP (  end  ) ■■■" << std::endl;
	std::cout << std::endl;
	
	//========================================================================================================================================================================
	//========================================================================================================================================================================
	
#if defined(UseKeyInterrupter)
	while(KI->ReadInterrupt()!=true){			//[Ctrl+Q]が押されるまで、ループを実行する。
#else
	while(1){
#endif //defined(UseKeyInterrupter)
		std::cout << "■■■ Information ( start ) ■■■"                                       << std::endl;
	#if defined(UseKeyInterrupter)
		std::cout << "If you want to quit this program, press [Ctrl] + [Q] on your key board."  << std::endl;
	#else
		std::cout << "If you want to quit this program, press [Ctrl] + [C] on your key board."  << std::endl;
		std::cout << "If you press [Ctrl] + [Z], you can stop this program until you press \"fg\" + [Enter].(Ofcource you can press [Ctrl] + [C] to quit.)"  << std::endl;
	#endif //defined(UseKeyInterrupter)
		std::cout << "■■■ Information (  end  ) ■■■"                                       << std::endl;
		std::cout << std::endl;
		
		std::cout << "■■■ Get tweet. ( start ) ■■■"                                << std::endl;
		std::cout << "□ : one tweet"                                                   << std::endl;
	//	std::cout << "≡ : verified account"                                            << std::endl;
	//	std::cout << "≠ : spam tweet or not improtant tweet"                           << std::endl;		//	std::cout << "≠ : spam tweet ( There is more than 5 times diffrrence between retweet_count and favorite_count. )" << std::endl;
		std::cout << "× : in the black list"                                           << std::endl;
		std::cout << "    ← : Additional description for the left sign."               << std::endl;
		std::cout << "    n : Initial of Word or Phrase beyond n was in the user name"  << std::endl;
		std::cout << "    t : Initial of Word or Phrase beyond t was in the text"       << std::endl;
		std::cout << "☆ : aleady posted to Blogger"                                    << std::endl;
		std::cout << "◎ : in the hash table"                                           << std::endl;
		std::cout << "    ← : Additional description for the left sign."               << std::endl;
		std::cout << "    〆: parameater renewd"                                        << std::endl;
		std::cout << "S : smaller than SD.average_reTweet_per_second_100th."            << std::endl;
		std::cout << std::endl;
		std::cout << "←J : left sign is Japanese."                                     << std::endl;
		std::cout << "←E : left sign is English."                                      << std::endl;
	//	std::cout << "N : Not retweet"                                                  << std::endl;
	//	std::cout << "F : Few retweets, followers or friends."                          << std::endl;
		std::cout << std::endl;
		
		
	#ifdef ICHashT_OperationalDebagMode//ICHashT.hに、(今回に関しては、このファイルSource.cpp)[ICHashT_OperationalDebagMode]が、定義されているかどうか。
		// 運用上のデバッグモードの設定
		printf("pICHashT_ja ::     TableLength :: %llu\n",(UINT64)(pICHashT_ja->GetTableLength()    ));
		printf("pICHashT_ja :: TrueTableLength :: %llu\n",pICHashT_ja->GetTrueTableLength());
		printf("\n");
		
		printf("pICHashT_en ::     TableLength :: %llu\n",pICHashT_en->GetTableLength()    );
		printf("pICHashT_en :: TrueTableLength :: %llu\n",pICHashT_en->GetTrueTableLength());
		printf("\n");
		
		printf("pPT_DB_ja   ::     TableLength :: %llu\n",(pPT_DB_ja->Get_pICHashT())->GetTableLength()    );
		printf("pPT_DB_ja   :: TrueTableLength :: %llu\n",(pPT_DB_ja->Get_pICHashT())->GetTrueTableLength());
		printf("\n");
		
		printf("pPT_DB_en   ::     TableLength :: %llu\n",(pPT_DB_en->Get_pICHashT())->GetTableLength()    );
		printf("pPT_DB_en   :: TrueTableLength :: %llu\n",(pPT_DB_en->Get_pICHashT())->GetTrueTableLength());
		printf("\n");
	#else //ICHashT_OperationalDebagMode
		// 運用上のリリースモードの設定
	#endif//ICHashT_OperationalDebagMode
		
		//実行中のメモリを監視する。メモリの使用量をファイルに書き出し。
	#ifdef MemoryLogged_enable
		ML->MemoryLogged();
	#endif //MemoryLogged_enable

		//ここで取得した時間情報は使用されないが、始め[tmp_UTC]にアクセスする際、[tmp_UTC]はポインタで宣言されているので、一度以上時間情報を格納して、ポインタの先に[tm_min]や[tm_min]を用意しておかないと、アクセス違反を起こし、「Segmentation fault (コアダンプ)」となる。
		time_t timep_LOCAL = time(0);									// time(0) : ローカル(PC上で)のunixtimeを取得する。
		struct tm *tmp_UTC = gmtime(&timep_LOCAL);					// PC上でのローカル時間「unix時間[time_t型]」を、協定世界時（UTC）[tm構造体]へ変換
		struct tm  tmp_LOCAL;
		
		class GetTweet* GT = new GetTweet();	//コンストラクタの呼び出し。
		GetTweet_Initialize_infinity( GT, OA_ja );	//接続が成功するまで、関数内で無限ループしている。
		
//		int PostTime15or45;	// for Mail Title
		
	#if defined(UseKeyInterrupter)
		while(KI->ReadInterrupt()!=true){			//[Ctrl+Q]が押されるまで、ループを実行する。
	#else
		while(1){
	#endif //defined(UseKeyInterrupter)
			
			static bool onece_flag_15 = false;	//初期状態でfalseにしておけば、ある程度ツイートを取得してからしかメールでツイートを送信しない。
			static bool onece_flag_45 = false;	//初期状態でfalseにしておけば、ある程度ツイートを取得してからしかメールでツイートを送信しない。
			
			//投稿する時間かどうかを確認する。
			if( 15 <= tmp_UTC->tm_min && tmp_UTC->tm_min < 45 && onece_flag_15==true ){
				onece_flag_15=false;
				onece_flag_45=true;
				std::cout << std::endl;
				std::cout << "break" << std::endl;
				
				tmp_LOCAL = *localtime(&timep_LOCAL);
				tmp_LOCAL.tm_min = 15;
				tmp_LOCAL.tm_sec = 00;
				timep_LOCAL = mktime((struct tm *)&tmp_LOCAL);

				if( tmp_UTC->tm_hour==0 && StartUpDate!=tmp_LOCAL.tm_mday ){//時刻と日付により、起動から一回以上日付変更線を跨いでいることの確認。
					// 日付変更後の[00:15]の場合は、DBのデータについて、(今日を含めず、)31日前のデータを削除。
					pPT_DB_ja->DeleteOldestDateDataFromHashTable( timep_LOCAL );	// 入力データとしては、日付が変わった後(直後を想定。)の時間を与える。
					pPT_DB_en->DeleteOldestDateDataFromHashTable( timep_LOCAL );	// 入力データとしては、日付が変わった後(直後を想定。)の時間を与える。
					StartUpDate = -1;														//[1～31]以外の不正な値を代入しておく。(一月以上経過した後に一日だけ同じ日付の時、メモリを解放できないため。)
				}
				
				break;					//while文を抜けて、sort処理と、Bloggerへの投稿を行う
			}else if( (45 <= tmp_UTC->tm_min || tmp_UTC->tm_min < 15) && onece_flag_45==true ){
				onece_flag_15=true;
				onece_flag_45=false;
				std::cout << std::endl;
				std::cout << "break" << std::endl;
				
				tmp_LOCAL = *localtime(&timep_LOCAL);
				tmp_LOCAL.tm_min = 45;
				tmp_LOCAL.tm_sec = 00;
				timep_LOCAL = mktime((struct tm *)&tmp_LOCAL);
				
				break;					//while文を抜けて、sort処理と、Bloggerへの投稿を行う
			}else if(		SD_ja.num_of_element>=50		//マルチ言語対応の時に注意!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
						&&	SD_en.num_of_element>=50
						&&	onece_flag_15==false && onece_flag_45==false ){
				//起動時のみの処理(両方falseの場合)
			//	onece_flag_15=true;	//両方trueにしても、後でどちらかがfalseになるので、問題無い。(投稿時間は毎時15分と45分からずれるが、50件以上溜まった段階で、さっさと一つ目を投稿するようになる。)
			//	onece_flag_45=true;	//両方trueにしても、後でどちらかがfalseになるので、問題無い。(投稿時間は毎時15分と45分からずれるが、50件以上溜まった段階で、さっさと一つ目を投稿するようになる。)
				if( 45 <= tmp_UTC->tm_min || tmp_UTC->tm_min < 15 ){
					onece_flag_15=true;
				}else if( 15 <= tmp_UTC->tm_min && tmp_UTC->tm_min < 45 ){
					onece_flag_45=true;
				}
			}
			
		#if defined(Release) || defined(InfinityLoop)
		#else
			//for degab
			if(		SD_ja.num_of_element       >= SD_ja.num_of_post
				&&	SD_ja_daily.num_of_element >= SD_ja_daily.num_of_post
				&&	SD_en.num_of_element       >= SD_en.num_of_post
				&&	SD_ko.num_of_element       >= SD_ko.num_of_post
			){
				std::cout << std::endl;
				tmp_LOCAL = *localtime(&timep_LOCAL);	//デバッグモードでは、ここで初期化しておかないと、データを送信しようとする際に、コアダンプする。
				break;		//ハッシュテーブル上にツイートが10以上溜まったらwhile文を抜ける。
			}
		#endif //defined(Release) || defined(InfinityLoop)
		
			//ツイートのデータを取得する。
			{
				int result = GT->GetOneTweet();
				if(result!=0){
					std::cout << "GetOneTweet():Communication error has occurred. 10 seconds later, system tries to reconnect.";
					fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
					sleep(10);	//10[second]
					std::cout << " → System is trying to reconnect." << std::endl;
					GetTweet_Initialize_infinity( GT, OA_ja );	//接続が成功するまで、関数内で無限ループしている。
					continue;	//通信エラーが発生した場合は、while()文の始めからやり直す。(continue文は処理をスキップする。)
				}
			}
			
			//この時点で。[GT->OneTweet]に、一つ分のツイートデータ(JSON)がバッファされている。
			
			//JSONの解析
			if( TD == NULL ){
				//なるべく確保したメモリは使い回すようにする。
				TD = new (struct get_data_from_tweet);			// ツイート一つ分のメモリの確保(必ず解放すること。)	// メモリの解放は、[sort_tweet();関数]で行われるので、ここがメモリーリークの原因である場合は、そのあたりを確かめること!!
			}
			char* OneTweet = (char*)(GT->OneTweet).c_str();	//[std::string型]のままだと、constなので、書き換えられないから、そのままでは無理。→一つ噛ませる。(関数内部で書き換えていく事になるし、やはり必要。)
			get_data( OneTweet, TD );							//  [ツイートデータ一つ分のJSON形式のテキスト] → [TD構造体] のように変換される。

			// for Debag
			//printf("%lf,%lf,%d,%llu,%d,%llu,%llu,%llu,%llu,%llu,%llu,%s,%s\n", TD->retweet_efficiency, TD->average_reTweet_per_second, TD->time_unixtime, TD->tweet_id, TD->time_unixtime_re, TD->tweet_id_re, TD->user_id, TD->followers_count, TD->friends_count, TD->retweet_count, TD->favorite_count, TD->screen_name_re->c_str(), TD->text->c_str() );
			
			//時間の取得 :: in order to calculate average_reTweet_per_second
			     timep_LOCAL = time(0);								// time(0) : ローカル(PC上で)のunixtimeを取得する。								//	time_t timep_LOCAL = time(0);
			         tmp_UTC = gmtime(&timep_LOCAL);				//PC上でのローカル時間「unix時間[time_t型]」を、協定世界時（UTC）[tm構造体]へ変換		//	struct tm *tmp_UTC     = gmtime(&timep_LOCAL);
			time_t timep_UTC = mktime(tmp_UTC);					//[tm構造体]で指定された時間(今回はUTC)を、unix時間[time_t型]に変換する。
			
			
			if(TD->language.length()==0){usleep(500*1000);continue;}	// 500ms	// Twitter側の供給が追い付いていないと思われるので、スリープを入れておく。
																							// (空のツイートの表示が続く場合は大抵何個か続くので、無駄に読み込みに行くのはやめて、
																							//  スリープを入れて、CPU負荷を下げる。)
			
			
			//解析したJSONが、最低限の条件を満たしているかどうかを確認する。
			if(     BasicFilter( TD            )!=0 ){continue;}		//各種スパムのフィルタ、公式アカウントのフィルタ
			if(      TimeFilter( TD, timep_UTC )!=0 ){continue;}		//時間のフィルタ、リツイート速度の計算、リツイート効率の計算
			
			
			if(strcmp(TD->language.c_str(),"ja")==0){					//日本語のツイートのみを抽出
				if( BlackListFilter_ja( TD            )!=0 ){continue;}		//only for Japanese
				if(  DataBaseFilter   ( TD, pPT_DB_ja )!=0 ){continue;}		//過去にBloggerにポストされたツイートを除く。
				
			//	printf("\n");
			//	printf("%lf,%lf,%d,%llu,%d,%llu,%llu,%llu,%llu,%llu,%llu,%s,%s\n", TD.retweet_efficiency, TD->average_reTweet_per_second, TD.time_unixtime, TD.tweet_id, TD.time_unixtime_re, TD->tweet_id_re, TD.user_id, TD.followers_count, TD.friends_count, TD.retweet_count, TD.favorite_count, TD.screen_name_re.c_str(), TD.text.c_str() );
				
				//ハッシュテーブルにデータを格納する。
				{
					int result = HashTable_Data_StockOrRenewOrThrowAway( TD, pICHashT_ja, SD_ja );
					if( result == true ){
						struct get_data_from_tweet* TD2 = new (struct get_data_from_tweet);//上記ハッシュテーブルから管理されるので、新しいコピーを作る。
						*TD2 = *TD;//これで本当に正しくコピーできるのかには疑問が残る。→うまくコピーできてた。
						TD = TD2;
					}	// TDをNULLに初期化して、次のループで、新しくメモリを確保させる。
					if(      TimeFilter_daily( TD, timep_UTC       )!=0 ){continue;}		//時間のフィルタのみ。
					
					//データの更新が[DataBaseFilter]内で行われるため、先に入れ替えてからフィルタにかける。
					TD->average_reTweet_per_second = (double)TD->retweet_count;//全てのソートは[TD->average_reTweet_per_second]に対して行われるので、入れ替える。(非常に強引な実装ではあるが、[average_reTweet_per_second]を[sort_target]のような名前に変更すれば、納得のいく実装となる。ただし、[average_reTweet_per_second]型がdouble型である事に問題がないかどうかだけ確認する必要がある。)
					if(  DataBaseFilter      ( TD, pPT_DB_ja_daily )!=0 ){continue;}		//過去にBloggerにポストされたツイートを除く。
					
					result = HashTable_Data_StockOrRenewOrThrowAway( TD, pICHashT_ja_daily, SD_ja_daily );//ここでソートされる場合のソート順をリツイート順に買える必要がある。
					if( result == true ){TD=NULL;}	// TDをNULLに初期化して、次のループで、新しくメモリを確保させる。
				}
				
				printf("←J");																	//動作確認用(長いと直ぐにコンソールが埋まるため。)
				fflush( stdout );																//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			}else if(strcmp(TD->language.c_str(),"en")==0){			//extract only English tweets
				if( BlackListFilter_en( TD            )!=0 ){continue;}		//only for English
				if(  DataBaseFilter   ( TD, pPT_DB_en )!=0 ){continue;}		//過去にBloggerにポストされたツイートを除く。
				
				//ハッシュテーブルにデータを格納する。
				{
					int result = HashTable_Data_StockOrRenewOrThrowAway( TD, pICHashT_en, SD_en );
					if( result == true ){
						struct get_data_from_tweet* TD2 = new (struct get_data_from_tweet);//上記ハッシュテーブルから管理されるので、新しいコピーを作る。
						*TD2 = *TD;//これで本当に正しくコピーできるのかには疑問が残る。→うまくコピーできてた。
						TD = TD2;
					}	// TDをNULLに初期化して、次のループで、新しくメモリを確保させる。
					if(      TimeFilter_daily( TD, timep_UTC       )!=0 ){continue;}		//時間のフィルタのみ。
					
					//データの更新が[DataBaseFilter]内で行われるため、先に入れ替えてからフィルタにかける。
					TD->average_reTweet_per_second = (double)TD->retweet_count;//全てのソートは[TD->average_reTweet_per_second]に対して行われるので、入れ替える。(非常に強引な実装ではあるが、[average_reTweet_per_second]を[sort_target]のような名前に変更すれば、納得のいく実装となる。ただし、[average_reTweet_per_second]型がdouble型である事に問題がないかどうかだけ確認する必要がある。)
					if(  DataBaseFilter      ( TD, pPT_DB_en_daily )!=0 ){continue;}		//過去にBloggerにポストされたツイートを除く。
					
					result = HashTable_Data_StockOrRenewOrThrowAway( TD, pICHashT_en_daily, SD_en_daily );//ここでソートされる場合のソート順をリツイート順に買える必要がある。
					if( result == true ){TD=NULL;}	// TDをNULLに初期化して、次のループで、新しくメモリを確保させる。
				}
				
				printf("←E");																	//動作確認用(長いと直ぐにコンソールが埋まるため。)
				fflush( stdout );																//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			}else if(strcmp(TD->language.c_str(),"ko")==0){			//extract only Korean tweets
				if( BlackListFilter_ko( TD            )!=0 ){continue;}		//only for Korean
				if(  DataBaseFilter   ( TD, pPT_DB_ko )!=0 ){continue;}		//過去にBloggerにポストされたツイートを除く。
				
				//ハッシュテーブルにデータを格納する。
				{
					int result = HashTable_Data_StockOrRenewOrThrowAway( TD, pICHashT_ko, SD_ko );
					if( result == true ){ TD=NULL; }	// TDをNULLに初期化して、次のループで、新しくメモリを確保させる。
				}
				
				printf("←K");																	//動作確認用(長いと直ぐにコンソールが埋まるため。)
				fflush( stdout );																//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			}
		}
		//実行中のメモリを監視する。メモリの使用量をファイルに書き出し。
	#ifdef MemoryLogged_enable
		ML->MemoryLogged();
	#endif //MemoryLogged_enable
		
		GT->GetTweet_Finalize();
		delete GT;				//ディストラクタの呼び出し。

		//実行中のメモリを監視する。メモリの使用量をファイルに書き出し。
	#ifdef MemoryLogged_enable
		ML->MemoryLogged();
	#endif //MemoryLogged_enable
		
		std::cout << "■■■ Get tweet. (  end  ) ■■■" << std::endl;
		std::cout << std::endl;
		
		//英語版も、日本時間で運営する。(アメリカだけでも時計が4つとサマータイムがあるので。)
		
		// Set Mail Title
		std::string MailTitle = (std::string)str_sprintf( "%04d_%02d_%02d_%02d_%02d", 1900 + tmp_LOCAL.tm_year, 1 + tmp_LOCAL.tm_mon, tmp_LOCAL.tm_mday, tmp_LOCAL.tm_hour, tmp_LOCAL.tm_min );
		Mail_ja.Subject       = MailTitle;	//メールのタイトル
		Mail_ja_daily.Subject = MailTitle;	//メールのタイトル
		Mail_en.Subject       = MailTitle;	//メールのタイトル
		Mail_en_daily.Subject = MailTitle;	//メールのタイトル
		Mail_ko.Subject       = MailTitle;	//メールのタイトル
		
		//example                                           http://tweet-topix.blogspot.jp/2015/09/201509201545.html
		std::string BlogURL_ja = (std::string)str_sprintf( "http://tweet-topix.blogspot.jp/%04d/%02d/%04d%02d%02d%02d%02d.html"    , 1900 + tmp_LOCAL.tm_year, 1 + tmp_LOCAL.tm_mon, 1900 + tmp_LOCAL.tm_year, 1 + tmp_LOCAL.tm_mon, tmp_LOCAL.tm_mday, tmp_LOCAL.tm_hour, tmp_LOCAL.tm_min ); //ツイートの投稿先のブログのURL
		std::string BlogURL_en = (std::string)str_sprintf( "http://tweet-topix-en.blogspot.com/%04d/%02d/%04d%02d%02d%02d%02d.html", 1900 + tmp_LOCAL.tm_year, 1 + tmp_LOCAL.tm_mon, 1900 + tmp_LOCAL.tm_year, 1 + tmp_LOCAL.tm_mon, tmp_LOCAL.tm_mday, tmp_LOCAL.tm_hour, tmp_LOCAL.tm_min ); //ツイートの投稿先のブログのURL
		std::string BlogURL_ko = (std::string)str_sprintf( "http://tweet-topix-ko.blogspot.kr/%04d/%02d/%04d%02d%02d%02d%02d.html" , 1900 + tmp_LOCAL.tm_year, 1 + tmp_LOCAL.tm_mon, 1900 + tmp_LOCAL.tm_year, 1 + tmp_LOCAL.tm_mon, tmp_LOCAL.tm_mday, tmp_LOCAL.tm_hour, tmp_LOCAL.tm_min ); //ツイートの投稿先のブログのURL
		
		static std::string language_ja = "ja";
		static std::string language_en = "en";
		static std::string language_ko = "ko";
	#if defined(Post_ja)
		std::cout << "■■■ Tweet-TOPIX-ja. ( start ) ■■■" << std::endl;
		SortHashTable_GetHTMLode_SendGmail_SaveInTheDataBase_SendReply( OA_ja , language_ja , num_of_send_reply_ja , Mail_ja , pICHashT_ja , SD_ja , pPT_DB_ja, timep_LOCAL, BlogURL_ja );
		std::cout << "■■■ Tweet-TOPIX-ja. (  end  ) ■■■" << std::endl;
		std::cout << std::endl;
	#endif //defined(Post_ja)
	#if defined(Post_en)
		std::cout << "■■■ Tweet-TOPIX-en. ( start ) ■■■" << std::endl;
		SortHashTable_GetHTMLode_SendGmail_SaveInTheDataBase_SendReply( OA_en , language_en , num_of_send_reply_en , Mail_en , pICHashT_en , SD_en , pPT_DB_en, timep_LOCAL, BlogURL_en );
		std::cout << "■■■ Tweet-TOPIX-en. (  end  ) ■■■" << std::endl;
		std::cout << std::endl;
	#endif //defined(Post_en)
	#if defined(Post_ko)
		std::cout << "■■■ Tweet-TOPIX-ko. ( start ) ■■■" << std::endl;
		SortHashTable_GetHTMLode_SendGmail_SaveInTheDataBase_SendReply( OA_ko , language_ko , num_of_send_reply_ko , Mail_ko , pICHashT_ko , SD_ko , pPT_DB_ko, timep_LOCAL, BlogURL_ko );
		std::cout << "■■■ Tweet-TOPIX-ko. (  end  ) ■■■" << std::endl;
		std::cout << std::endl;
	#endif //defined(Post_ko)
		
	#if defined(Release) || defined(Post_only_00_15)
		if( tmp_LOCAL.tm_hour==0 && tmp_LOCAL.tm_min==15 ){
	#else
		if( tmp_LOCAL.tm_min==15 ){
	#endif //defined(Release) || ~defined(Post_only_00_15)
		#if defined(Post_ja_daily)
			//Tweet-TOPIX-daily(00:15更新分)
			// 現在時刻より、12?24時間前に取得したツイート(日本語では144件)について、上からいくつか(日本語では25件)を取得、ソートしてGmailで送信する。
			//いや、ここでポストすると、dailyが各時間帯の後ろになってしまうので、余計に目立たなくなるから、変更。→しかし、通常のブログポストの後に判定処理を入れるのは非常に面倒なので、ひとまずここに実装する。
			std::string BlogURL_ja_daily = (std::string)str_sprintf( "http://tweet-topix-daily.blogspot.jp/%04d/%02d/%04d%02d%02d%02d%02d.html"    , 1900 + tmp_LOCAL.tm_year, 1 + tmp_LOCAL.tm_mon, 1900 + tmp_LOCAL.tm_year, 1 + tmp_LOCAL.tm_mon, tmp_LOCAL.tm_mday, tmp_LOCAL.tm_hour, tmp_LOCAL.tm_min ); //ツイートの投稿先のブログのURL
			std::cout << "■■■ Tweet-TOPIX-ja-daily. ( start ) ■■■" << std::endl;
//			SortHashTable_GetHTMLode_SendGmail_SaveInTheDataBase_SendReply_daily( OA_ja_daily , language_ja , num_of_send_reply_ja_daily , Mail_ja_daily , pICHashT_ja_daily , SD_ja_daily , pPT_DB_ja_daily, timep_LOCAL, BlogURL_ja_daily, post_interval_ja_daily );
			SortHashTable_GetHTMLode_SendGmail_SaveInTheDataBase_SendReply_daily( OA_ja_daily , language_ja ,                              Mail_ja_daily , pICHashT_ja_daily , SD_ja_daily , pPT_DB_ja_daily, timep_LOCAL,                   post_interval_ja_daily );
			std::cout << "■■■ Tweet-TOPIX-ja-daily. (  end  ) ■■■" << std::endl;
			std::cout << std::endl;
		#endif //defined(Post_ja_daily)
		#if defined(Post_en_daily)
			//Tweet-TOPIX-daily(00:15更新分)
			// 現在時刻より、12?24時間前に取得したツイート(日本語では144件)について、上からいくつか(日本語では25件)を取得、ソートしてGmailで送信する。
			//いや、ここでポストすると、dailyが各時間帯の後ろになってしまうので、余計に目立たなくなるから、変更。→しかし、通常のブログポストの後に判定処理を入れるのは非常に面倒なので、ひとまずここに実装する。
			std::string BlogURL_en_daily = (std::string)str_sprintf( "http://tweet-topix-daily.blogspot.jp/%04d/%02d/%04d%02d%02d%02d%02d.html"    , 1900 + tmp_LOCAL.tm_year, 1 + tmp_LOCAL.tm_mon, 1900 + tmp_LOCAL.tm_year, 1 + tmp_LOCAL.tm_mon, tmp_LOCAL.tm_mday, tmp_LOCAL.tm_hour, tmp_LOCAL.tm_min ); //ツイートの投稿先のブログのURL
			std::cout << "■■■ Tweet-TOPIX-ja-daily. ( start ) ■■■" << std::endl;
			SortHashTable_GetHTMLode_SendGmail_SaveInTheDataBase_SendReply_daily( OA_en_daily , language_en ,                              Mail_en_daily , pICHashT_en_daily , SD_en_daily , pPT_DB_en_daily, timep_LOCAL,                   post_interval_en_daily );
			std::cout << "■■■ Tweet-TOPIX-ja-daily. (  end  ) ■■■" << std::endl;
			std::cout << std::endl;
		#endif //defined(Post_en_daily)
		}
		
		//実行中のメモリを監視する。メモリの使用量をファイルに書き出し。
	#ifdef MemoryLogged_enable
		ML->MemoryLogged();
	#endif //MemoryLogged_enable
		
	#if defined(Release) || defined(InfinityLoop)
	#elif defined(NumOfLoop)
		if(LoopCount<NumOfLoop-1){
			LoopCount++;
		}else{
			break;//while文から抜けて、プログラムを終了する。
		}
	#else
		break;//while文から抜けて、プログラムを終了する。
	#endif //defined(Release) || defined(InfinityLoop)
	}
	
	//========================================================================================================================================================================
	//========================================================================================================================================================================
	
	//実行中のメモリを監視する。メモリの使用量をファイルに書き出し。
#ifdef MemoryLogged_enable
	ML->MemoryLogged();
#endif //MemoryLogged_enable
	
	if( TD != NULL ){
		delete TD;
	}
	
#if defined(UseKeyInterrupter)
	delete KI;//class KeyInterrupter
#endif //defined(UseKeyInterrupter)
	
	// 全要素走査 ( ハッシュテーブル上に記録されているすべての key value ペアの取り出し。 )
	// ハッシュテーブル自体を解放する前に、テーブル上に動的に確保されているデータを全て解放する必要がある。
	{
		AllScanPlace NowPlace;
		for(pICHashT_ja->AllScanInit(NowPlace); NowPlace < pICHashT_ja->AllScanEnd; pICHashT_ja->AllScanNextShift(NowPlace)){
			
			UINT64 key;		//ダミー(実際には使われない。)
			void* value_p;
			pICHashT_ja->AllScanGetData(key,value_p,NowPlace);	// データの取り出し。 [InsertedChainedHashTable]
			
			delete (struct get_data_from_tweet*)value_p;
		}
	}
	delete pICHashT_ja;
	
	// 全要素走査 ( ハッシュテーブル上に記録されているすべての key value ペアの取り出し。 )
	// ハッシュテーブル自体を解放する前に、テーブル上に動的に確保されているデータを全て解放する必要がある。
	{
		AllScanPlace NowPlace;
		for(pICHashT_ja_daily->AllScanInit(NowPlace); NowPlace < pICHashT_ja_daily->AllScanEnd; pICHashT_ja_daily->AllScanNextShift(NowPlace)){
			
			UINT64 key;		//ダミー(実際には使われない。)
			void* value_p;
			pICHashT_ja_daily->AllScanGetData(key,value_p,NowPlace);	// データの取り出し。 [InsertedChainedHashTable]
			
			delete (struct get_data_from_tweet*)value_p;
		}
	}
	delete pICHashT_ja_daily;
	
	// 全要素走査 ( ハッシュテーブル上に記録されているすべての key value ペアの取り出し。 )
	// ハッシュテーブル自体を解放する前に、テーブル上に動的に確保されているデータを全て解放する必要がある。
	{
		AllScanPlace NowPlace;
		for(pICHashT_en->AllScanInit(NowPlace); NowPlace < pICHashT_en->AllScanEnd; pICHashT_en->AllScanNextShift(NowPlace)){
			
			UINT64 key;		//ダミー(実際には使われない。)
			void* value_p;
			pICHashT_en->AllScanGetData(key,value_p,NowPlace);	// データの取り出し。 [InsertedChainedHashTable]
			
			delete (struct get_data_from_tweet*)value_p;
		}
	}
	delete pICHashT_en;
	
	// 全要素走査 ( ハッシュテーブル上に記録されているすべての key value ペアの取り出し。 )
	// ハッシュテーブル自体を解放する前に、テーブル上に動的に確保されているデータを全て解放する必要がある。
	{
		AllScanPlace NowPlace;
		for(pICHashT_en_daily->AllScanInit(NowPlace); NowPlace < pICHashT_en_daily->AllScanEnd; pICHashT_en_daily->AllScanNextShift(NowPlace)){
			
			UINT64 key;		//ダミー(実際には使われない。)
			void* value_p;
			pICHashT_en_daily->AllScanGetData(key,value_p,NowPlace);	// データの取り出し。 [InsertedChainedHashTable]
			
			delete (struct get_data_from_tweet*)value_p;
		}
	}
	delete pICHashT_en_daily;
	
	// 全要素走査 ( ハッシュテーブル上に記録されているすべての key value ペアの取り出し。 )
	// ハッシュテーブル自体を解放する前に、テーブル上に動的に確保されているデータを全て解放する必要がある。
	{
		AllScanPlace NowPlace;
		for(pICHashT_ko->AllScanInit(NowPlace); NowPlace < pICHashT_ko->AllScanEnd; pICHashT_ko->AllScanNextShift(NowPlace)){
			
			UINT64 key;		//ダミー(実際には使われない。)
			void* value_p;
			pICHashT_ko->AllScanGetData(key,value_p,NowPlace);	// データの取り出し。 [InsertedChainedHashTable]
			
			delete (struct get_data_from_tweet*)value_p;
		}
	}
	delete pICHashT_ko;
	
	delete pPT_DB_ja;
	delete pPT_DB_ja_daily;
	delete pPT_DB_en;
	delete pPT_DB_en_daily;
	delete pPT_DB_ko;
	
	//実行中のメモリを監視する。メモリの使用量をファイルに書き出し。
#ifdef MemoryLogged_enable
	ML->MemoryLogged();
	delete ML;
#endif //MemoryLogged_enable
	
	return 0;
}







