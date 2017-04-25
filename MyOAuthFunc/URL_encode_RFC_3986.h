#include <string>
#include <stdio.h>	//sprintf()

std::string URL_encode_RFC_3986( const std::string &str );
	// str    : URLencodeする対象の文字列
	//
	// return : URLencode処理した文字列を返す
