#include "str_sprintf.h"



std::string str_sprintf(const char* format,...){
	//sprintf()がchar型に提供する機能を、str_sprintf()はstd::string型で提供する。
	//ただし、引数の位置は異なるので注意。(この方が、戻り値を「+=」演算でバッファに追加書き込みしたり、「<<」演算に渡したりできるので、便利。)
	//
	// const char* format : %d,%s,%c,%f,%lf,%x,%X,...などで、可変引数に渡すフォーマットを指定する。
	// ...                   : 可変引数
	//
	// return             : std::string
	
#ifdef _WIN32	//windows環境の場合は「_WIN32」が定義されているので、こちら。
	va_list args;				//可変長引数を扱うときのお約束

	// retrieve the variable arguments
	//可変変数の引数を取得する。
	va_start( args, format );	//可変長引数を扱うときのお約束
    
	//必要なメモリサイズを計算
	int len = _vscprintf( format, args ) // _vscprintf doesn't count
										+ 1; // terminating '\0'
    
	//メモリの確保
	char *buffer = (char*)malloc( len * sizeof(char) );

	vsprintf_s( buffer, len, format, args );

	//std::stringへcopy
	std::string result_string = buffer;

	//メモリの解放
	free( buffer );

	va_end(args);
	return result_string;
#else			//Linux環境の場合は、「_WIN32」が定義されていないので、こちら。
	va_list ap;
	va_start(ap, format);
	char* alloc;
	if(vasprintf(&alloc,format,ap) == -1){ //vasprintf() は、内部的にメモリを確保しているので、使用後に解放する必要がある。
		return std::string("");
	}
	va_end(ap);
	std::string result_string = std::string(alloc);
	free(alloc);
	return result_string;
#endif
}
