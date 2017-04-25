#include <openssl/ssl.h>
#include <string>

#include "Base64_encode.h"	//これが必要!! また、コンパイラオプションに「-lcrypto」が必要!!

//HMAC-SHA1をOpenSSLを利用して計算し、Base64にencodeする。
std::string HMAC_SHA1_and_Base64encode( std::string &signature_key , std::string &signature_data );
	// signature_key       : key
	// signature_data      : data
