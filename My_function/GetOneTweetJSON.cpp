#include "GetOneTweetJSON.h"



//コンストラクタ
GetTweet::GetTweet(){}
//ディストラクタ
GetTweet::~GetTweet(){}


int GetTweet::GetTweet_Initialize( struct OAuth_parameters &OA ){
	//https://stream.twitter.com/1.1/statuses/sample.json
	
	MC.host_name_or_address = "stream.twitter.com";	//argv[1];	//ex:[www.google.co.jp]
	MC.service              = "https";					//argv[2];	//ex:[http]	//ポート番号を指定する代わり。(httpのポート番号80番をここに指定しても上手く動作する事を確認した。)
	
	//Generate_CustomHeadre_of_Authorization()の内部でencodeすると、encodeしているのかしていないのか、外から分かり難くなるので、先にエンコードしてから渡す事にする。
	OA.request_method        = URL_encode_RFC_3986( "POST"                                                );	//リクエストメソッド	//英数字なのでURLencodeしても同じだが、そうする事になっている。
	OA.request_URL           = URL_encode_RFC_3986( "https://stream.twitter.com/1.1/statuses/sample.json" );	//リクエストURL
									//OAuth_parameters.request_parameterについては、単純に、URL_encode_RFC_3986();関数を通してはいけない。
	OA.request_parameter_1   = "";
	OA.request_parameter_2   = "";
	
	Generate_CustomHeadre_of_Authorization( OA );		//[OAuth_parameters]構造体に格納したデータを、カスタムヘッダの[Authorization]に加工する。
//	std::cout << OA.return_Custom_Header << endl << endl;

	MC.SendMsg  = "POST /1.1/statuses/sample.json?";
	MC.SendMsg += OA.request_parameter_1;
	MC.SendMsg += '&';
	MC.SendMsg += OA.request_parameter_2;
	MC.SendMsg += " HTTP/1.1\r\n";
	MC.SendMsg += "User-Agent: Rating_Sprite\r\n";
	MC.SendMsg += "Host: stream.twitter.com\r\n";
	MC.SendMsg += "Accept: */*\r\n";
	MC.SendMsg += OA.return_Custom_Header;
	MC.SendMsg += "\r\n";
//	MC.SendMsg += "Connection: close\r\n";			//通信を切断して、socketをcloseする場合は、[close]を指定して、接続を切断する。
	MC.SendMsg += "Connection: Keep-Alive\r\n";	//連続して通信する場合は、[Keep-Alive]を指定して、接続を継続する。
	MC.SendMsg += "\r\n";
	
	
	//Initialize
	int result = Make_Connection_Init_and_Open( MC );
	if( result!=0 ){printf("ERROR::GetTweet_Initialize01\n");return -1;}
	
	result = Make_Connection_Write( MC );	//send message
	if( result!=0 ){printf("ERROR::GetTweet_Initialize02\n");return -1;}
	
	Make_Connection_Read_Initialize( MC );
	result = Make_Connection_Read( MC );	//get message header	//失敗した場合のエラー処理は、Make_Connection_Read();関数内部で行われているので、不要。(Make_Connection_Read_Finalize();関数、及び、Make_Connection_Close();関数を内部で実行している。)
	if( result!=0 ){
		printf("ERROR::GetTweet_Initialize03\n");
		//printf("%s", MC.ch_ReceivedMsg );				//取得したメッセージを出力
		return -1;
	}
	printf("%s", MC.ch_ReceivedMsg );					//取得したメッセージを出力
	
	return 0;
}
int GetTweet::GetTweet_Finalize(){
	Make_Connection_Read_Finalize( MC );	//free();
	Make_Connection_Close( MC );			//切断
	return 0;
}
//メンバ関数
int GetTweet::GetOneTweet(){
//	std::string OneTweet;
	OneTweet.clear();		//前回のゴミを消す。
	while(1){
		result = Make_Connection_Read( MC );	//get message header
		if( result!=0 ){return -1;}				//失敗した場合のエラー処理は、Make_Connection_Read();関数内部で行われているので、不要。(Make_Connection_Read_Finalize();関数、及び、Make_Connection_Close();関数を内部で実行している。)
		
//		printf("%s", MC.ch_ReceivedMsg );				//取得したメッセージを出力

		//改行毎にデータサイズが格納されているので、それを計算して受信したデータの末尾まで移動し、一回改行なら続きのデータと結合させ、
		//二回改行なら、次のツイートなので、処理を分ける。
		
		int NextStringSize=0;//メッセージは「データサイズ」と「実際の文字列(JSON)」が「改行」と組み合わさって順番に来るので、示されたデータサイズをまずは確認し、NextStringSizeへ文字列を整数型に直して格納する。
		for( int read_size=0; MC.ch_ReceivedMsg[read_size]!='\r' && MC.ch_ReceivedMsg[read_size+1]!='\n' && read_size < MC.read_size; read_size++ ){
			NextStringSize*=16;//前回の桁を上げておく。(始めの一回はbufを0に初期化しているので意味を成さない。)
			if('0'<=MC.ch_ReceivedMsg[read_size] && MC.ch_ReceivedMsg[read_size]<='9'){
				NextStringSize += MC.ch_ReceivedMsg[read_size] - '0';
			}else if('A'<=MC.ch_ReceivedMsg[read_size] && MC.ch_ReceivedMsg[read_size]<='F'){
				NextStringSize += MC.ch_ReceivedMsg[read_size] - 'A' + 10;
			}else{
				//'a'<=MC.ch_ReceivedMsg[i] && MC.ch_ReceivedMsg[i]<='f'
				NextStringSize += MC.ch_ReceivedMsg[read_size] - 'a' + 10;
			}
			//printf("::%c::",MC.ch_ReceivedMsg[read_size]);
		}
		//printf("Next string size :: %d\n\n",NextStringSize);
		//printf("\n\n");

		result = Make_Connection_Read( MC );	//get message header
		if( result!=0 ){return -1;}				//失敗した場合のエラー処理は、Make_Connection_Read();関数内部で行われているので、不要。(Make_Connection_Read_Finalize();関数、及び、Make_Connection_Close();関数を内部で実行している。)
		
		for( int read_size=0; read_size<NextStringSize-2; read_size++ ){
			OneTweet += MC.ch_ReceivedMsg[read_size];
		}
		if( MC.ch_ReceivedMsg[NextStringSize-2]=='\r' && MC.ch_ReceivedMsg[NextStringSize-1]=='\n' ){
			//最後の二文字が"\r\n"の場合は、一つのツイートデータはここまでとなる。
			//そうで無い場合は、一続きのツイートのデータは最後まで読み込まれていない。
			
			result = Make_Connection_Read( MC );	//"\r\n"を空読み
			if( result!=0 ){return -1;}				//失敗した場合のエラー処理は、Make_Connection_Read();関数内部で行われているので、不要。(Make_Connection_Read_Finalize();関数、及び、Make_Connection_Close();関数を内部で実行している。)
			
			
			//ツイートデータ一つ分確保できたので、ここから処理に回す。
		//	printf("%s\n\n\n",OneTweet.c_str());	//for debag
			return 0;
		//	OneTweet.clear();
		}else{
			OneTweet += MC.ch_ReceivedMsg[NextStringSize-2];
			OneTweet += MC.ch_ReceivedMsg[NextStringSize-1];
			
			//まだツイートデータ一つ分確保できていないので、もう一度読み込みに行く。
			result = Make_Connection_Read( MC );	//"\r\n"を空読み
			if( result!=0 ){return -1;}				//失敗した場合のエラー処理は、Make_Connection_Read();関数内部で行われているので、不要。(Make_Connection_Read_Finalize();関数、及び、Make_Connection_Close();関数を内部で実行している。)
		}
		//printf("%s",OneTweet.c_str());
		
		
		/*
		//TEST Code(データ解析用)
		int read_size=0;
		//while(MC.ch_ReceivedMsg[read_size]!='\0'){
		while(MC.ch_ReceivedMsg[read_size]!='\0' && read_size < MC.read_size){		//念のためオーバーフローしないようにチェックもしておく。
			if(MC.ch_ReceivedMsg[read_size]=='\r'){
				printf("%s","■\\r■");
			}else if(MC.ch_ReceivedMsg[read_size]=='\n'){
				printf("%s","■\\n■");
			}else{
				printf("%c",MC.ch_ReceivedMsg[read_size]);
			}
			read_size++;
		}
		printf("%s","■□■");
		//*/
	}
}






