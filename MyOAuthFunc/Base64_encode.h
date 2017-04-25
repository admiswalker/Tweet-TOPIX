#include <stdio.h>
#include <string>		//std::string
#include <stdlib.h>	//malloc()

char* Base64_encode( unsigned char* str , size_t str_len );
	// str     : decodingするデータ
	// str_len : 終了コードを含まない str の長さを渡す。
	//
	// return  : encodeした文字列を返す。(使用後は free() で解放する必要がある。)

std::string Base64_encode_std_string( std::string in_string );
	// in_string : decodingするデータ
	//
	// return    : encodeした文字列を返す。
