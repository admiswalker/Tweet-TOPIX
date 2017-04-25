#include <stdarg.h>		//va_start()
#include <string>		//std::string


#ifdef _WIN32	//windows環境の場合は「_WIN32」が定義されているので、こちら。

#else			//Linux環境の場合は、「_WIN32」が定義されていないので、こちら。
	#include <stdio.h>	//vasprintf()
	#include <stdlib.h>	//free()

#endif

std::string str_sprintf(const char* format,...);
	//sprintf()がchar型に提供する機能を、str_sprintf()はstd::string型で提供する。
	//ただし、引数の位置は異なるので注意。(この方が、戻り値を「+=」演算でバッファに追加書き込みしたり、「<<」演算に渡したりできるので、便利。)
	//
	// const char* format : %d,%s,%c,%f,%lf,%x,%X,...などで、可変引数に渡すフォーマットを指定する。
	// ...                   : 可変引数
	//
	// return             : std::string
