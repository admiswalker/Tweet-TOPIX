#include "Base64_encode.h"

char* Base64_encode( unsigned char* str , size_t str_len ){
	// str     : decodingするデータ
	// str_len : 終了コードを含まない str の長さを渡す。
	//
	// return  : encodeした文字列を返す。(使用後は free() で解放する必要がある。)


	char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";	//Base64の64文字

	int result_len = sizeof(char)*(((str_len)*4+2)/3);	//切り上げになるように計算する。(「=」で埋める分を除いた、encodeしたデータを格納するのに必要なサイズを計算。)
//	int result_len = sizeof(char)*(((str_len)*4)/3);の計算について、上記では切り上げになるように「+2」してから計算している。(整数形は必ず切り捨てられる事を考えると、「分母 - 1」を分子に足しておけば、切り上げにできる。)

	int result_len_remainder = result_len % 4;	//余りを計算する。

	int total_result_len;	//(「=」で埋める分を含めた、)実際に出力するデータのサイズ。
	if(result_len_remainder != 0){
		total_result_len = result_len - result_len_remainder + 4;	//バッファが4の倍数になるように調整する。
	}else{
		total_result_len = result_len;								//既に4の倍数のため、そのままにする。
	}


	char* result = (char*)malloc(sizeof(char)*(total_result_len+1));	//decodeするので、サイズは3/4倍になる。
																		//終了コードの分だけ余分に確保しておく。
	result[total_result_len] = '\0';	//先に終了コードを仕込んでおく。

	//4の倍数分を計算する。
	int i=0,j=0;
	for(; i<(result_len - result_len_remainder); j+=3){
		result[i++] =                (str[j+0]>>2)  & 0x3F;	//ビットシフトとOR「|」演算で結合し、「&」演算でビットマスクする。
		result[i++] = ((str[j+0]<<4)|(str[j+1]>>4)) & 0x3F;
		result[i++] = ((str[j+1]<<2)|(str[j+2]>>6)) & 0x3F;
		result[i++] =  (str[j+2]<<0)                & 0x3F;
	}

	//4の倍数から溢れた分を計算する。
	//[i],[j]は先ほどの続きから。
	//
	//そのままstr[]にアクセスすると、str[]が終了コードNULLで終わっていない場合に、値が不定となってしまうので、0x00としておく。
	//また、終了コードで終わっていない場合は、(0x00とせず、そのままstr[]にアクセスすると、)メモリが確保されていなければ、アクセス違反となるので、注意が必要である。
	if(result_len_remainder==1){		//余り1の場合
		result[i++] =                (str[j+0]>>2)  & 0x3F;	//ビットシフトとOR「|」演算で結合し、「&」演算でビットマスクする。
	}else if(result_len_remainder==2){	//余り2の場合
		result[i++] =                (str[j+0]>>2)  & 0x3F;	//ビットシフトとOR「|」演算で結合し、「&」演算でビットマスクする。
		result[i++] =  (str[j+0]<<4)                & 0x3F;
	}else if(result_len_remainder==3){	//余り3の場合
		result[i++] =                (str[j+0]>>2)  & 0x3F;	//ビットシフトとOR「|」演算で結合し、「&」演算でビットマスクする。
		result[i++] = ((str[j+0]<<4)|(str[j+1]>>4)) & 0x3F;
		result[i++] =  (str[j+1]<<2)                & 0x3F;
	}
	//result_len_remainder==0の場合は何もせずに抜ける。

	int i2=0;
	for(; i2<result_len; i2++){
		result[i2] = b64_table[(int)result[i2]];
	}
	for(; i2<total_result_len; i2++){	//[i2]は先ほどの続きから。
		result[i2] = '=';				//残りは「=」で埋める。
	}

	return result;
}


std::string Base64_encode_std_string( std::string in_string ){
	// str     : decodingするデータ
	// str_len : 終了コードを含まない str の長さを渡す。
	//
	// return  : encodeした文字列を返す。(使用後は free() で解放する必要がある。)
	
	char* buf = Base64_encode( (unsigned char*)in_string.c_str() , in_string.length() );
	
	std::string return_result = buf;
	free(buf);
	
	return return_result;
}
