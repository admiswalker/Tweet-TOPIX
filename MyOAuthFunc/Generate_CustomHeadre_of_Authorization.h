//HMAC_SHA1_and_Base64encode()関数が、OpenSSLを使用している関係で、コンパイラオプションに「-lcrypto」が必要!!

#include <string>
#include <vector>				//vector配列
#include <algorithm>			//sort()

#include "URL_encode_RFC_3986.h"						//URL_encode_RFC_3986()
#include "Generate_oauth_nonce.h"						//Generate_oauth_nonce()	//oauth_nonceを生成する関数
#include "str_sprintf.h"									//str_sprintf()
#include "HMAC_SHA1_and_Base64encode.h"				//HMAC_SHA1_and_Base64encode()	コンパイラオプションに「-lcrypto」が必要!!


#ifndef __INCLUDE_GUARD__OAuth_parameters
	//既に構造体が定義されている場合は、俗称インクルードガードによって、再定義を回避する。
	#define __INCLUDE_GUARD__OAuth_parameters	//[__INCLUDE_GUARD__]を定義し、次回以降読み込まないようにする。
	struct OAuth_parameters{
		std::string oauth_consumer_key;
		std::string oauth_consumer_secret;
	
		std::string oauth_token;
		std::string oauth_token_secret;
	
	//	std::string user_id;
	//	std::string screen_name;
	
	//	std::string oauth_callback;
	
		std::string request_URL;
		std::string request_method;
		std::string request_parameter_1;
		std::string request_parameter_2;
	
		std::string return_Custom_Header;
	};
#endif//__INCLUDE_GUARD__OAuth_parameters


void Generate_CustomHeadre_of_Authorization( OAuth_parameters &OA );
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
