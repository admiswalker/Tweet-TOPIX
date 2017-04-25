#include "Generate_CustomHeadre_of_Authorization.h"


#include <iostream>		//標準出力?のcoutに必要。//For DeBag

void Generate_CustomHeadre_of_Authorization( OAuth_parameters &OA ){
	//OAuth_parameters構造体の内、
	//「
	//  oauth_consumer_key
	//  oauth_consumer_secret
	//
	//  request_URL
	//  request_method
	//」
	//の値を指定してGenerate Custom Headre()関数を実行する。
	//
	//最初の認証では、「oauth_token_secret」の値を取りに行くので、
	//「oauth_token_secret」の値は、必要無い。

	//Generate key
	std::string signature_key;
	            signature_key  = OA.oauth_consumer_secret;
	            signature_key += '&';
	            signature_key += OA.oauth_token_secret;
	std::string G_oauth_nonce = Generate_oauth_nonce();				//ユニークな長さのユニークな値を生成する。	//実際には長さはそれほどユニークにはしていない。恐らく、10進数→16進数の変換の過程で、桁数が変化する(事がある)だけ。
	std::string n_oauth_nonce             = "oauth_nonce=";
	            n_oauth_nonce            += G_oauth_nonce;
	std::string n_oauth_consumer_key      = "oauth_consumer_key=";					//(API Key)
	            n_oauth_consumer_key     += OA.oauth_consumer_key;
	std::string n_oauth_token             = "oauth_token=";
	            n_oauth_token            += OA.oauth_token;
	std::string n_oauth_signature_method  = "oauth_signature_method=HMAC-SHA1";
	std::string n_oauth_timestamp         = "oauth_timestamp=";
//	                   time_t timep_LOCAL = time(0)+600;								//何故だかわからないが、twitterのサーバの時間が10分程度ずれているので、認証に失敗するため、(+600)進めておく。
	                   																		//→たぶん、仮想(VirtualBox上)でCentOS7を動かしている関係で、スリープを入れた場合などに、時刻が正しく反映されず、ずれてしまったと考えられる。
	                   time_t timep_LOCAL = time(0);									// time(0) : ローカル(PC上で)のunixtimeを取得する。
//	                   struct tm *tmp     = gmtime(&timep_LOCAL);					//PC上でのローカル時間「unix時間[time_t型]」を、協定世界時（UTC）[tm構造体]へ変換
//	                   time_t timep_UTC   = mktime(tmp);								//[tm構造体]で指定された時間(今回はUTC)を、unix時間[time_t型]に変換する。
//	            n_oauth_timestamp        += str_sprintf("%d", timep_UTC );		//UTCを指定すると失敗した。
	            n_oauth_timestamp        += str_sprintf("%d",timep_LOCAL);					// time(0) : unixtimeを取得する。
	std::string n_oauth_version           = "oauth_version=1.0";
	
	std::vector<std::string> v;
	v.push_back ( n_oauth_nonce            );
	v.push_back ( n_oauth_consumer_key     );
	v.push_back ( n_oauth_token            );
	v.push_back ( n_oauth_signature_method );
	v.push_back ( n_oauth_timestamp        );
	v.push_back ( n_oauth_version          );
	
	if(OA.request_parameter_1.length()!=0){
		v.push_back ( OA.request_parameter_1   );
	}
	if(OA.request_parameter_2.length()!=0){
		v.push_back ( OA.request_parameter_2   );
	}
	std::sort( v.begin(), v.end() );					//ソート
	
	std::string parameter;
	//*
	for( int i=0; i<v.size()-1; i++){
		//パラメータはアルファベット順にソートして接続する必要がある。
		parameter += v[i];
		parameter += '&' ;
	}
	parameter += v[v.size()-1];
	parameter  = URL_encode_RFC_3986( parameter );		//URL encode
	
	//「リクエストメソッド」「リクエストURL」「パラメータ」の順番に&で繋げる。
	std::string signature_data  = OA.request_method;
	            signature_data += '&';
	            signature_data += OA.request_URL;
	            signature_data += '&';
	            signature_data += parameter;
//	std::cout << "signature_data\n\n";// << endl << endl;
//	std::cout << signature_data << "\n\n";//endl << endl;
//	std::cout << "signature_data_END\n\n";// << endl << endl;
	
	//作成したデータをHMAC-SHA1で変換し、Base64encodeする。
	std::string oauth_signature = HMAC_SHA1_and_Base64encode( signature_key , signature_data );
	
	std::string nD_oauth_signature  = "oauth_signature=\"";
	            nD_oauth_signature += URL_encode_RFC_3986( oauth_signature );
	            nD_oauth_signature += '\"';
	
	std::string nD_oauth_nonce             = "oauth_nonce=\"";
	            nD_oauth_nonce            += G_oauth_nonce;
	            nD_oauth_nonce            += '\"';
	std::string nD_oauth_consumer_key      = "oauth_consumer_key=\"";						//(API Key)
	            nD_oauth_consumer_key     += OA.oauth_consumer_key;
	            nD_oauth_consumer_key     += '\"';
	std::string nD_oauth_signature_method  = "oauth_signature_method=\"HMAC-SHA1\"";
	std::string nD_oauth_timestamp         = "oauth_timestamp=\"";
//	            nD_oauth_timestamp        += str_sprintf("%d",timep_UTC);
	            nD_oauth_timestamp        += str_sprintf("%d",timep_LOCAL);					// time(0) : unixtimeを取得する。
	            nD_oauth_timestamp        += '\"';
	std::string nD_oauth_token             = "oauth_token=\"";						//(API Key)
	            nD_oauth_token            += OA.oauth_token;
	            nD_oauth_token            += '\"';
	std::string nD_oauth_version           = "oauth_version=\"1.0\"";
	
	//Generate Custom Header
	OA.return_Custom_Header  = "Authorization: OAuth ";
	OA.return_Custom_Header += nD_oauth_consumer_key;
	OA.return_Custom_Header += ", ";	//',';
	OA.return_Custom_Header += nD_oauth_nonce;
	OA.return_Custom_Header += ", ";	//',';
	OA.return_Custom_Header += nD_oauth_signature;
	OA.return_Custom_Header += ", ";	//',';
	OA.return_Custom_Header += nD_oauth_signature_method;
	OA.return_Custom_Header += ", ";	//',';
	OA.return_Custom_Header += nD_oauth_timestamp;
	OA.return_Custom_Header += ", ";	//',';
	OA.return_Custom_Header += nD_oauth_token;
	OA.return_Custom_Header += ", ";	//',';
	OA.return_Custom_Header += nD_oauth_version;
	/*
	//Generate Custom Header :: example 実際には改行無しの一行のデータ
	Authorization: OAuth oauth_consumer_key="", oauth_nonce="", oauth_signature="", oauth_signature_method="HMAC-SHA1", oauth_timestamp="", oauth_token="", oauth_version="1.0"
		//改行を入れて見やすくすると、下記
	Authorization: OAuth 
	oauth_consumer_key="", 
	oauth_nonce="", 
	oauth_signature="", 
	oauth_signature_method="HMAC-SHA1", 
	oauth_timestamp="", 
	oauth_token="", 
	oauth_version="1.0"
	*/
}
