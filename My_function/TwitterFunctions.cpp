#include "./TwitterFunctions.h"

//========================================================================================================================================
//========================================================================================================================================

/*
■■■ Get HTML code. ( start ) ■■■
Error005:
■■■ Dump data below. ( start ) ■■■
{"errors":[{"message":"Sorry, that page does not exist","code":34}]}
■■■ Dump data below. (  end  ) ■■■
GetTweet_HTML_Code():Communication error has occurred. 10 seconds later, system tries to reconnect. → System is trying to reconnect.
～省略～
■■■ Get HTML code. (  end  ) ■■■
*/
/*
■■■ Get HTML code. ( start ) ■■■
Error005:
■■■ Dump data below. ( start ) ■■■
{"error":"Sorry, you are not authorized to see this status.","request":"\/1.1\/statuses\/oembed.json?id=646637189598593024"}
■■■ Dump data below. (  end  ) ■■■
GetTweet_HTML_Code():Communication error has occurred. 10 seconds later, system tries to reconnect. → System is trying to reconnect.
～省略～
■■■ Get HTML code. (  end  ) ■■■
*/

int GetTweet_HTML_Code( struct OAuth_parameters &OA , std::string &language , unsigned long long &TOP_reTweets_IDs, std::string &HTMLCode ){
//	HTMLCode.clear();//メモリの解放がどうなるかによって必要になるかもしれない
//	現在は、main()関数内の「while(1){}」内で定義されているので、「while(){}文」を抜けるたびに、破棄されるため、[.clear()]を実行する必要は無い。
	
	//このAPIは[GET]のみに対応している。[POST]は不可。(curlのコマンドをtwitterに生成させた場合でも失敗した。)
	//今は態々StreamingAPIの接続を一度切ってからこちらの処理をしているが、どうやら別に切る必要は無い事が分かった。
	
	struct Make_Connection_struct MC;
	MC.host_name_or_address = "api.twitter.com";	//argv[1];	//ex:[www.google.co.jp]
	MC.service              = "https";				//argv[2];	//ex:[http]	//ポート番号を指定する代わり。(httpのポート番号80番をここに指定しても上手く動作する事を確認した。)
	
	//Generate_CustomHeadre_of_Authorization()の内部でencodeすると、encodeしているのかしていないのか、外から分かり難くなるので、先にエンコードしてから渡す事にする。
	OA.request_method        = URL_encode_RFC_3986( "GET"                                                 );	//リクエストメソッド	//英数字なのでURLencodeしても同じだが、そうする事になっている。
	OA.request_URL           = URL_encode_RFC_3986( "https://api.twitter.com/1.1/statuses/oembed.json"    );	//リクエストURL
									//OAuth_parameters.request_parameterについては、単純に、URL_encode_RFC_3986();関数を通してはいけない。
	OA.request_parameter_1   = "id=";
	OA.request_parameter_1  += URL_encode_RFC_3986( str_sprintf( "%llu", TOP_reTweets_IDs ) );
	OA.request_parameter_2   = "";
	
	Generate_CustomHeadre_of_Authorization( OA );		//[OAuth_parameters]構造体に格納したデータを、カスタムヘッダの[Authorization]に加工する。
//	std::cout << OA.return_Custom_Header << endl << endl;
	
	//埋め込み用のHTMLコードを取得する
//	MC.SendMsg  = "GET /1.1/statuses/oembed.json?id=388202585036816384 HTTP/1.1\r\n";
	MC.SendMsg  = "GET /1.1/statuses/oembed.json?id=";
	MC.SendMsg += str_sprintf( "%llu", TOP_reTweets_IDs );
	MC.SendMsg += " HTTP/1.1\r\n";
	if(strcmp(language.c_str(),"ja")==0){
		MC.SendMsg += "User-Agent: Rating_Sprite\r\n";
	}else if(strcmp(language.c_str(),"ja_daily")==0){
		MC.SendMsg += "User-Agent: Raily_Sprite\r\n";
	}else if(strcmp(language.c_str(),"en")==0){
		MC.SendMsg += "User-Agent: Eisel_Howard\r\n";
	}
	MC.SendMsg += "Host: api.twitter.com\r\n";
	MC.SendMsg += "Accept: */*\r\n";
	MC.SendMsg += OA.return_Custom_Header;
	MC.SendMsg += "\r\n";
	MC.SendMsg += "Connection: close\r\n";			//通信を切断して、socketをcloseする場合は、[close]を指定して、接続を切断する。
//	MC.SendMsg += "Connection: Keep-Alive\r\n";	//連続して通信する場合は、[Keep-Alive]を指定して、接続を継続する。
	MC.SendMsg += "\r\n";
	
	//Initialize
	int result = Make_Connection_Init_and_Open( MC );
	if( result!=0 ){printf("Error001:");return -1;}
	
	result = Make_Connection_Write( MC );		//send message
	if( result!=0 ){printf("Error002:");return -1;}					//失敗した場合のエラー処理は、Make_Connection_Write();関数内部で行われているので、不要。(Make_Connection_Close();関数を内部で実行している。)ただし、Make_Connection_Read_Finalize();関数、は実行されないので、Make_Connection_Read_Initialize();関数が、既に実行されている場合は、Make_Connection_Read_Finalize();関数を実行する必要がある。
	
	Make_Connection_Read_Initialize( MC );
	
	result = Make_Connection_Read( MC );		//get message header
	if( result!=0 ){printf("Error003:");return -1;}					//失敗した場合のエラー処理は、Make_Connection_Read();関数内部で行われているので、不要。(Make_Connection_Read_Finalize();関数、及び、Make_Connection_Close();関数を内部で実行している。)
//	printf("%s", MC.ch_ReceivedMsg );			//取得したメッセージを出力
	
	result = Make_Connection_Read( MC );		//get message header
	if( result!=0 ){printf("Error004:");return -1;}					//失敗した場合のエラー処理は、Make_Connection_Read();関数内部で行われているので、不要。(Make_Connection_Read_Finalize();関数、及び、Make_Connection_Close();関数を内部で実行している。)
//	printf("%s\n", MC.ch_ReceivedMsg );			//取得したメッセージを出力
	
	//切断
	Make_Connection_Close( MC );
	
	//=========
	//ここの処理自体は[Process.cpp]ファイルの[]関数とほぼ同じだが、
	//関数化するのが面倒なのと、改行コードの扱いが異なるので、取りあえず今回は[GenerateHTMLCodeFromOembedJson_Ver00.00_01]からそのままコピーした。
	
	char* htmlBegin = strstr( MC.ch_ReceivedMsg , "\"html\"" );	//「\"html\"」を検索する。
	if( htmlBegin == NULL ){
		int return_value = -1;
		
		printf("Error005:");printf("\n");
		printf("■■■ Dump data below. ( start ) ■■■\n");
		printf("%s",MC.ch_ReceivedMsg);
		printf("\n");
		printf("■■■ Dump data below. (  end  ) ■■■\n");
		
		const char* error_msg2 = "Sorry, that page does not exist";						//{"errors":[{"message":"Sorry, that page does not exist","code":34}]}
		const char* error_msg3 = "Sorry, you are not authorized to see this status.";	//{"error":"Sorry, you are not authorized to see this status.","request":"\/1.1\/statuses\/oembed.json?id=682525426481102848"}
		const char* error_msg4 = "Bad Authentication data.";							//{"errors":[{"code":215,"message":"Bad Authentication data."}]}
		char* error;
		error = strstr( MC.ch_ReceivedMsg, error_msg2 );if( error != NULL ){return_value = -2;}	//error_msg2を検索する。
		error = strstr( MC.ch_ReceivedMsg, error_msg3 );if( error != NULL ){return_value = -3;}	//error_msg3を検索する。
		error = strstr( MC.ch_ReceivedMsg, error_msg4 );if( error != NULL ){return_value = -4;}	//error_msg4を検索する。
		
		Make_Connection_Read_Finalize( MC );		//受信用バッファの解放	//ここでのエラーは、通信エラーではないので、メモリを開放する必要がある。
		return return_value;
	}
	
//	printf("%s\n\n",htmlBegin);
	
	htmlBegin += 8;	//「\"html\":"」の字数分進める
//	printf("%s\n\n",htmlBegin);
	
	char* htmlend = strstr( htmlBegin , "\"," );	//「\",」を検索する。
//	printf("%s\n\n",htmlend);
	*htmlend = '\0';	//終了コードの挿入
	
//	printf("%s\n\n",htmlBegin);
	
	for(int read=0; htmlBegin[read]!='\0'; ){
		
		if( htmlBegin[read] == '\\'){	//「\」の場合
			if( htmlBegin[read+1] == 'u' ){
				//UTF16's escape sequence.( example : \uXXXX )
				
				int in_ReadSize;
				int out_WriteSize;
				//UTF16 -> UTF8
				//現在のUTF8は最長でも4Byteまでの長さなので、同じバッファに書き戻しても問題無い。
				char text_buf[4];
				Decode_escapedUTF16String_to_UTF8Binary( &htmlBegin[read], in_ReadSize, text_buf, out_WriteSize );
				for(int num=0; num < out_WriteSize; num++){
					HTMLCode += text_buf[num];
				}
				read  += in_ReadSize;
			//	write += out_WriteSize;	//今回は使用しない。(後で書き直す。)
				
			}else if( htmlBegin[read+1] == 'n' ){
				//改行コード[\n]はUTF8などの文字コードでは無く、プラットフォームに依存する。
				//今回はLinuxでの改行コードに変換する。
				//Windows : CR + LF : [0x0D,0x0A]
				//UNIX系   :       LF : [     0x0A]←Linuxはこれ。
				
				read += 2;
				HTMLCode += 0x0A;
				
			}else{
				//[\]によるエスケープを解除する。
				read+=1;
				HTMLCode += htmlBegin[read++];	//そのまま出力へ回す。
			}
		}else{
			HTMLCode += htmlBegin[read++];		//そのまま出力へ回す。
		}
	}
//	HTMLCode += '\0';		//親切で入れてやったら、メールでNULL文字として送信されたようで、Bloggerの表示がバグったので、入れない。
//	printf("%s\n\n",HTMLCode.c_str());
	
	//=========
	
	Make_Connection_Read_Finalize( MC );		//受信用バッファの解放
	
	return 0;
}


//========================================================================================================================================
//========================================================================================================================================


int Send_reply_to_the_tweet_posted_tweet( struct OAuth_parameters &OA, std::string &language, unsigned long long &TOP_reTweets_IDs, std::string &TOP_reTweets_NAMEs, std::string &BlogURL ){
	//ツイートの投稿方法
	//POST
	//https://api.twitter.com/1.1/statuses/update.json?status=[URLencodeしたツイート内容]
	//
	//リプライ(返信)の投稿方法
	//POST
	//https://api.twitter.com/1.1/statuses/update.json?status=[URLencodeしたツイート内容]?in_reply_to_status_id=[リプライ(返信)する相手のID]
	
	struct Make_Connection_struct MC;
	MC.host_name_or_address = "api.twitter.com";		//argv[1];	//ex:[www.google.co.jp]
	MC.service              = "https";					//argv[2];	//ex:[http]	//ポート番号を指定する代わり。(httpのポート番号80番をここに指定しても上手く動作する事を確認した。)
	
	//Generate_CustomHeadre_of_Authorization()の内部でencodeすると、encodeしているのかしていないのか、外から分かり難くなるので、先にエンコードしてから渡す事にする。
	OA.request_method        = URL_encode_RFC_3986( "POST"                                                );	//リクエストメソッド	//英数字なのでURLencodeしても同じだが、そうする事になっている。
	OA.request_URL           = URL_encode_RFC_3986( "https://api.twitter.com/1.1/statuses/update.json"    );	//リクエストURL
									//OAuth_parameters.request_parameterについては、単純に、URL_encode_RFC_3986();関数を通してはいけない。
	//status=[URLencodeしたツイート内容]	//同じ内容のツイートを投稿しようとすると、[Status is a duplicate.]とエラーを吐かれてツイートできないので注意
	OA.request_parameter_1   = "status=";	// ここは、「URL_encode_RFC_3986( "status=" );」のようにURL_encodeしてはいけない。もし誤ってURLエンコードすると、「{"errors":[{"code":32,"message":"Could not authenticate you."}]}」と返されてしまう。
	
		//ここのメッセージだけ言語依存のため、言語毎にしていする。(英語、日本語、スペイン語、マレー語、ポルトガル語、アラビア語、中国語、ドイツ語、フランス語、ロシア語、韓国語、イタリア語、その他)
	if(strcmp(language.c_str(),"ja")==0){
		OA.request_parameter_1  += URL_encode_RFC_3986( "このツイートは大きな注目を集めていると推測されます。m(＿ ＿)m\r\n" );
//		OA.request_parameter_1  += URL_encode_RFC_3986( "http://tweet-topix.blogspot.jp/\r\n" );
		OA.request_parameter_1  += URL_encode_RFC_3986( BlogURL );
		OA.request_parameter_1  += URL_encode_RFC_3986( "\r\n" );
										//このツイートは大きな注目を集めていると推測されます。m(＿ ＿)m
										//http://tweet-topix.blogspot.jp/
										//                                   ↑ここで[改行]
	}else if(strcmp(language.c_str(),"en")==0){
		OA.request_parameter_1  += URL_encode_RFC_3986( "I estimate that this tweet might be received a lot of attention. m(__ __)m\r\n" );
//		OA.request_parameter_1  += URL_encode_RFC_3986( "http://tweet-topix-en.blogspot.com/\r\n" );
		OA.request_parameter_1  += URL_encode_RFC_3986( BlogURL );
		OA.request_parameter_1  += URL_encode_RFC_3986( "\r\n" );
										//[原文(投稿内容)]I estimate that this tweet might be received a lot of attention. m(__ __)m
										//[   日本語訳   ]私は、「このツイートは大きな注目を集めている」と推測(推定)します。 m(__ __)m
										//http://tweet-topix-en.blogspot.com/
										//                                       ↑ここで[改行]
	}else{
		//日本語と英語以外は、すべて英語でリプライする。
		OA.request_parameter_1  += URL_encode_RFC_3986( "I estimate that this tweet might be received a lot of attention. m(__ __)m\r\n" );
//		OA.request_parameter_1  += URL_encode_RFC_3986( "http://tweet-topix-en.blogspot.com/\r\n" );
		OA.request_parameter_1  += URL_encode_RFC_3986( BlogURL );
		OA.request_parameter_1  += URL_encode_RFC_3986( "\r\n" );
										//[原文(投稿内容)]I estimate that this tweet might be received a lot of attention. m(__ __)m
										//[   日本語訳   ]私は、「このツイートは大きな注目を集めている」と推測(推定)します。 m(__ __)m
										//http://tweet-topix-en.blogspot.com/
										//                                       ↑ここで[改行]
	}
	OA.request_parameter_1  += "%0D%0A";	//改行
	OA.request_parameter_1  += "@";						//リプライするのであれば必要	//リプライしないのであればコメントアウト
	OA.request_parameter_1  += TOP_reTweets_NAMEs;		//リプライするのであれば必要	//リプライしないのであればコメントアウト
	OA.request_parameter_1  += "%0D%0A";	//改行		//リプライするのであれば必要	//リプライしないのであればコメントアウト
//	OA.request_parameter_1  += " ";			//スペース	//リプライするのであれば必要	//リプライしないのであればコメントアウト//←はバグって失敗した？と思われる。
	OA.request_parameter_1  += "https%3A%2F%2Ftwitter.com%2F";
//	OA.request_parameter_1  +=               "twitter.com%2F";
	OA.request_parameter_1  += TOP_reTweets_NAMEs;
	OA.request_parameter_1  += "%2Fstatus%2F";
	OA.request_parameter_1  += (std::string)str_sprintf( "%llu", TOP_reTweets_IDs );
								//[表示例]
									//このツイートは大きな注目を集めていると推測されます。m(＿ ＿)m
									//http://tweet-topix.blogspot.jp/
									//
									//@katsuyatakeyuki
									//https://twitter.com/katsuyatakeyuki/status/599009755642789888
									
									//このツイートは大きな注目を集めていると推測されます。m(＿ ＿)m
									//tweet-topix.blogspot.jp
									//
									//@katsuyatakeyuki
									//twitter.com/katsuyatakeyuki/status/...
	/*
	//[Example]
	//test[2015_05_15]to[2015_05_16]～
	OA.request_parameter_1  += "%E3%81%93%E3%81%AE%E3%83%84%E3%82%A4%E3%83%BC%E3%83%88%E3%81%AF%E5%A4%A7%E3%81%8D%E3%81%AA%E6%B3%A8%E7%9B%AE%E3%82%92%E9%9B%86%E3%82%81%E3%81%A6%E3%81%84%E3%82%8B%E3%81%A8%E6%8E%A8%E6%B8%AC%E3%81%95%E3%82%8C%E3%81%BE%E3%81%99%E3%80%82m%28%EF%BC%BF%20%EF%BC%BF%29m";
	OA.request_parameter_1  += "%0D%0A";	//改行
	OA.request_parameter_1  += "http%3A%2F%2Ftweet-topix.blogspot.jp%2F";
	OA.request_parameter_1  += "%0D%0A";	//改行
									//このツイートは大きな注目を集めていると推測されます。m(＿ ＿)m
									//http://tweet-topix.blogspot.jp/
									//                                   ↑ここで[改行]
									
	OA.request_parameter_1  += "%0D%0A";	//改行
	OA.request_parameter_1  += "@";
	OA.request_parameter_1  += TOP_reTweets_NAMEs;
	OA.request_parameter_1  += "%0D%0A";	//改行
	OA.request_parameter_1  += "https%3A%2F%2Ftwitter.com%2F";
//	OA.request_parameter_1  +=               "twitter.com%2F";
	OA.request_parameter_1  += TOP_reTweets_NAMEs;
	OA.request_parameter_1  += "%2Fstatus%2F";
	OA.request_parameter_1  += (std::string)str_sprintf( "%llu", TOP_reTweets_IDs );
								//[表示例]
									//このツイートは大きな注目を集めていると推測されます。m(＿ ＿)m
									//http://tweet-topix.blogspot.jp/
									//
									//@katsuyatakeyuki
									//https://twitter.com/katsuyatakeyuki/status/599009755642789888
									
									//このツイートは大きな注目を集めていると推測されます。m(＿ ＿)m
									//tweet-topix.blogspot.jp
									//
									//@katsuyatakeyuki
									//twitter.com/katsuyatakeyuki/status/...
	//*/
	
	
	//in_reply_to_status_id=[リプライ(返信)する相手のID]
	OA.request_parameter_2   = "in_reply_to_status_id=";
	OA.request_parameter_2  += URL_encode_RFC_3986( (std::string)str_sprintf( "%llu", TOP_reTweets_IDs ) );		//リプライ(返信)先のID
//	OA.request_parameter_2   = "";
	
	Generate_CustomHeadre_of_Authorization( OA );		//[OAuth_parameters]構造体に格納したデータを、カスタムヘッダの[Authorization]に加工する。
//	std::cout << OA.return_Custom_Header << endl << endl;
	MC.SendMsg  = "POST /1.1/statuses/update.json?";
	MC.SendMsg += OA.request_parameter_1;
	MC.SendMsg += '&';
	MC.SendMsg += OA.request_parameter_2;
	MC.SendMsg += " HTTP/1.1\r\n";
	
	if(strcmp(language.c_str(),"ja")==0){
		MC.SendMsg += "User-Agent: Rating_Sprite\r\n";
	}else if(strcmp(language.c_str(),"en")==0){
		MC.SendMsg += "User-Agent: Eisel_Howard\r\n";
	}
	MC.SendMsg += "Host: api.twitter.com\r\n";
	MC.SendMsg += "Accept: */*\r\n";
	MC.SendMsg += OA.return_Custom_Header;
	MC.SendMsg += "\r\n";
	MC.SendMsg += "Connection: close\r\n";			//通信を切断して、socketをcloseする場合は、[close]を指定して、接続を切断する。
//	MC.SendMsg += "Connection: Keep-Alive\r\n";	//連続して通信する場合は、[Keep-Alive]を指定して、接続を継続する。
	MC.SendMsg += "\r\n";
	
	
	//Initialize
	int result = Make_Connection_Init_and_Open( MC );
	if( result!=0 ){printf("Error001:");return -1;}
	
	result = Make_Connection_Write( MC );		//send message
	if( result!=0 ){printf("Error002:");return -1;}					//失敗した場合のエラー処理は、Make_Connection_Write();関数内部で行われているので、不要。(Make_Connection_Close();関数を内部で実行している。)ただし、Make_Connection_Read_Finalize();関数、は実行されないので、Make_Connection_Read_Initialize();関数が、既に実行されている場合は、Make_Connection_Read_Finalize();関数を実行する必要がある。
	
	Make_Connection_Read_Initialize( MC );
	
	result = Make_Connection_Read( MC );		//get message header
	if( result!=0 ){printf("Error003:");return -1;}					//失敗した場合のエラー処理は、Make_Connection_Read();関数内部で行われているので、不要。(Make_Connection_Read_Finalize();関数、及び、Make_Connection_Close();関数を内部で実行している。)
//	printf("%s", MC.ch_ReceivedMsg );			//取得したメッセージを出力
	
	result = Make_Connection_Read( MC );		//get message header
	if( result!=0 ){printf("Error004:");return -1;}					//失敗した場合のエラー処理は、Make_Connection_Read();関数内部で行われているので、不要。(Make_Connection_Read_Finalize();関数、及び、Make_Connection_Close();関数を内部で実行している。)
//	printf("%s\n", MC.ch_ReceivedMsg );		//取得したメッセージを出力
	
	Make_Connection_Read_Finalize( MC );

	//切断
	Make_Connection_Close( MC );

	return 0;
}


//========================================================================================================================================
//========================================================================================================================================


