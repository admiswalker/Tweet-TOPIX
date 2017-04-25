#include "Decode_escapedUTF16String_to_UTF8Binary.h"


//Decode_escapedUTF16String_to_UTF8Binary()で使う関数。
void Translate4ByteStringTo2ByteBinary( char* in, char* out ){
	// in     : 4Byte char string "\\uABCD"		(エスケープ・シーケンスを考慮して「\」を「\\」として表記している。)
	// out    : 2Byte char binary '0xAB','0xCD'
	// Endian : 処理系のendian

	//'\\'と'u'の二つ分ポインタを進める。
	in+=2;	//コールバイ・バリューなので、この操作はこの関数の中でのみ有効。

	for(int i=0,i2=0; i<2; i++,i2+=2){
		char buf;
		
		if('0'<=in[0+i2] && in[0+i2]<='9'){
			buf = in[0+i2] - '0';
		}else if('A'<=in[0+i2] && in[0+i2]<='F'){
			buf = in[0+i2] - 'A' + 10;
		}else{
			//'a'<=in[0+i2]&&in[0+i2]<='f'
			buf = in[0+i2] - 'a' + 10;
		}
		
		buf = buf<<4;	//一桁目を桁上げ	//「buf*=16;」と同じ意味。
		
		if('0'<=in[1+i2] && in[1+i2]<='9'){
			buf += in[1+i2] - '0';
		}else if('A'<=in[1+i2] && in[1+i2]<='F'){
			buf += in[1+i2] - 'A' + 10;
		}else{
			//'a'<=in[1+i2] && in[1+i2]<='f'
			buf += in[1+i2] - 'a' + 10;
		}

		out[i] = buf;	//実行する環境のendianに関係なく、この順番で格納する。(endianに依存しないUTF8まで一気に変換するので。)
	}
}


//outバッファの使い方が分かり難いので、後で、やり直す。

void Decode_escapedUTF16String_to_UTF8Binary( char* in, int &in_ReadSize, char* out, int &out_WriteSize){
	// in  : [例:サロゲートペア] 0x20B9Fなら、「\uD842\uDF9F」を入力すると、
	// out : UTF8で'0xF0','0xA0','0xAE','0x9F'が出力される。
	//
	// in  : あ : UTF16(Unicode)0x3042なら、「\u3042」を入力すると、
	// out : UTF8で'0xE3','0x81','0x82'が出力される。

	//文字列の読み込み。「\uD842\uDF9F」なら、前半分の「\uD842」だけ読み込む。
	Translate4ByteStringTo2ByteBinary( in , out );

	//value0は2Byte以上必要。(サロゲートペアなら3Byte以上必要。)
	unsigned long value0 = ((out[0]<<8)&0xFF00) | (out[1]&0x00FF);	//'0xD8','0x42' を 0xD842 のように、一つにまとめる。


	//サロゲートペアでは無い場合。
	//そのままUTF16をUTF8に変換する。

	if(/*0x00<=value0 && */value0<=0x7F){

		//UTF8で1Byteの文字
			//1Byte目
		out[0] = 0x00 | value0;			//out[0] = 0x00 | (value0 & 0x7F);	//最上位ビットなので、ビットマスクする必要が無い。

		in_ReadSize   = 6;	//[\uXXXX]
		out_WriteSize = 1;	//一文字

	}else if(/*0x80<=value0 && */value0<=0x7FF){

		//UTF8で2Byteの文字
			//2Byte目
		out[1] = 0x80 | (value0 & 0x3F);
			//1Byte目
		value0 = value0>>6;
		out[0] = 0xC0 | value0;			//out[0] = 0x00 | (value0 & 0x7F);	//最上位ビットなので、ビットマスクする必要が無い。

		in_ReadSize   = 6;	//[\uXXXX]
		out_WriteSize = 2;	//一文字

//	}else if(0x800<=value0 && value0<=0xFFFF){		//「0xD800<=value0 && value0<=0xDBFF」の範囲がサロゲートペアなので、その手前(0xD800-1)まで。
	}else if(value0<=(0xD800-1)||(0xDBFF+1)<=value0){

		//UTF8で3Byteの文字
			//3Byte目
		out[2] = 0x80 | (value0 & 0x3F);
			//2Byte目
		value0 = value0>>6;
		out[1] = 0x80 | (value0 & 0x3F);
			//1Byte目
		value0 = value0>>6;
		out[0] = 0xE0 | value0;			//out[0] = 0x00 | (value0 & 0x7F);	//最上位ビットなので、ビットマスクする必要が無い。

		in_ReadSize   = 6;	//[\uXXXX]
		out_WriteSize = 3;	//一文字

	}else{
//	}else if(0xD800<=value0 && value0<=0xDBFF){//この範囲の場合はサロゲートペアとして処理する必要がある。
//注意:区間[0xD800,0xDBFF]は、Unicodeの範囲と言うより、サロゲートペアが割り当てられている範囲。

		//サロゲートペアの処理。
		//サロゲートペアの処理

		//文字列の読み込み。「\uD842\uDF9F」なら、後ろ半分の「\uDF9F」だけ読み込む。
		Translate4ByteStringTo2ByteBinary( &in[6] , &out[2] );

		//value1は2Byte以上必要。
//		unsigned short value1 = ((out[2]<<8)&0xFF00) | (out[3]&0x00FF);	//'0xD8','0x42' を 0xD842 のように、一つにまとめる。
		unsigned short value1 = (out[2]<<8) | (out[3]&0x00FF);	//'0xD8','0x42' を 0xD842 のように、一つにまとめる。←このように、自分より大きい範囲を指定する場合でもビットマスクが必要。ただし、ビットシフトした方はゼロ埋めされるので不要。
		//この時、サロゲートペアの後半なので、value1は、[0xDC00,0xDFFF]の範囲に収まるはず。(収まっていなければ、文字コードの方がおかしいが、エラーの確認は省く。)

		//サロゲートペアによって4Byteに分割されたUTF16を、結合して元に戻す。(「\uD842\uDF9F」⇒「0x20B9F」の変換)
			//符号の除去
		value0 = value0 & 0x3FF;	//value0 -= 0xD800;と同じ。
//		value1 = value1 & 0x3FF;	//value1 -= 0xDC00;と同じ。//下記の結合処理でビットマスクする必要があるので、そちらにまとめる。(自分のサイズより大きい範囲にコピーするので、マスクしておく必要がある。ビットシフトした方はゼロ埋めされるので問題ない。)

		//(16～11Bitは符号ビットの部分なので、)10Bit目までを、単純につなげる。
		//代入先(value0)には3Byte以上必要
		value0 = (value0<<10)|(value1&0x3FF);

		value0 += 0x10000;//符号を加算

		//UTF16をUTF8に変換する。
		//サロゲートペアの場合は、「0x10000～0x10FFFFの範囲の文字」なので、必ず「0x 1 00 00 ～ 0x10 FF FF」の範囲に収まる。
		//→実際には「0x00 01 00 00  ～  0x00 1F FF FF」の範囲の文字は4ByteのUTF8が割り当てられており、
		//  4Byte以上のUTF8は現在定義されていないので、if文を回すまでも無く4Byteと判断できる。
		//→サロゲートペアの場合、UTF8に変換すると、必ず4Byteになる。
		//
		// 1Byte目           2Byte目           3Byte目           4Byte目
		// 1 1 1 1 0□□□   1 0□□□□□□   1 0□□□□□□   1 0□□□□□□
		//           1 0 0       0 0 1 1 1 1       1 1 1 1 1 1       1 1 1 1 1 1 == 0x10FFFF ← これはサロゲージペアを用いて表せる最大の範囲となる。時々ネットのサンプルで、UTF8で定義されていない5Byteや6Byteの範囲も計算できるように「一応」実装してみた、というパターンがあるが、UTF16のBinaryで表せる(Binaryで表せない==保存すらできない。(保存する規格が決まっていない。))最大値のサロゲージペアを使っても表せない領域なので、おおよそ意味を成さない。
		//           0 0 0       1 0 0 0 0 0       1 0 1 1 1 0       0 1 1 1 1 1 == 0x20B9F
			//4Byte目
		out[3] = 0x80 | (value0 & 0x3F);
			//3Byte目
		value0 = value0>>6;
		out[2] = 0x80 | (value0 & 0x3F);
			//2Byte目
		value0 = value0>>6;
		out[1] = 0x80 | (value0 & 0x3F);
			//1Byte目
		value0 = value0>>6;
		out[0] = 0xF0 | value0;				//out[0] = 0xF0 | (value0 & 0x07);	//最上位ビットなので、ビットマスクする必要が無い。

		in_ReadSize   = 6*2;	//[\uXXXX\uXXXX]
		out_WriteSize = 4;	//一文字
	}
}
