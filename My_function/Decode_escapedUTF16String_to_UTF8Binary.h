#ifndef __INCLUDE_GUARD__Decode_escapedUTF16String_to_UTF8Binary
	//既に構造体が定義されている場合は、俗称インクルードガードによって、再定義を回避する。
	#define __INCLUDE_GUARD__Decode_escapedUTF16String_to_UTF8Binary	//[__INCLUDE_GUARD__]を定義し、次回以降読み込まないようにする。

void Decode_escapedUTF16String_to_UTF8Binary( char* in, int &in_ReadSize, char* out, int &out_WriteSize);
	// in  : [例:サロゲートペア] 0x20B9Fなら、「\uD842\uDF9F」を入力すると、
	// out : UTF8で'0xF0','0xA0','0xAE','0x9F'が出力される。
	//
	// in  : あ : UTF16(Unicode)0x3042なら、「\u3042」を入力すると、
	// out : UTF8で'0xE3','0x81','0x82'が出力される。

#endif//__INCLUDE_GUARD__Decode_escapedUTF16String_to_UTF8Binary


