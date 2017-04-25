/*
 *  Developer name : Landau_LostHeart
 *  Blog           : http://ever-code.blogspot.com/
 *  Profile        : http://www.blogger.com/profile/04252022933953248247
 */

/*
The MIT License (MIT)

Copyright (c) 2015 Landau_LostHeart

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

//日本語訳
/*
The MIT License
Copyright (c) 2015 Landau_LostHeart

以下に定める条件に従い、本ソフトウェアおよび関連文書のファイル（以下「ソフトウェア」）の複製を取得するすべての人に対し、ソフトウェアを無制限に扱うことを無償で許可します。これには、ソフトウェアの複製を使用、複写、変更、結合、掲載、頒布、サブライセンス、および/または販売する権利、およびソフトウェアを提供する相手に同じことを許可する権利も無制限に含まれます。

上記の著作権表示および本許諾表示を、ソフトウェアのすべての複製または重要な部分に記載するものとします。

ソフトウェアは「現状のまま」で、明示であるか暗黙であるかを問わず、何らの保証もなく提供されます。ここでいう保証とは、商品性、特定の目的への適合性、および権利非侵害についての保証も含みますが、それに限定されるものではありません。 作者または著作権者は、契約行為、不法行為、またはそれ以外であろうと、ソフトウェアに起因または関連し、あるいはソフトウェアの使用またはその他の扱いによって生じる一切の請求、損害、その他の義務について何らの責任も負わないものとします。
*/


#include "ICHashT.h"	//デバッグモードに入れるかどうかの定義。(複数ファイルにまたがるため、ヘッダを読み込む。)

//=====================================================================================================================================================
//InsertedChainedHashTable
//=====================================================================================================================================================



//素数表
//prime(X)をX番目の素数とするとき、1<=n<=33 について、
// prime(X-1) < 2^n <= prime(X)
//を満たす素数 prime(X) を表にしている。
//
//	Smallest prime >= 2^n.
const UINT64 SmallestPrimeList_LargerThanPower2[64] = {
	//[ull]は、[unsigned long long]の意味(付けておかないとGCCでのコンパイルで警告が出る。)
	2ull					,	// 2^ 1 + 0   = 2ull
	5ull					,	// 2^ 2 + 1   = 5ull
	11ull					,	// 2^ 3 + 3   = 11ull
	17ull					,	// 2^ 4 + 1   = 17ull
	37ull					,	// 2^ 5 + 5   = 37ull
	67ull					,	// 2^ 6 + 3   = 67ull
	131ull					,	// 2^ 7 + 3   = 131ull
	257ull					,	// 2^ 8 + 1   = 257ull
	521ull					,	// 2^ 9 + 9   = 521ull
	1031ull					,	// 2^10 + 7   = 1031ull
	2053ull					,	// 2^11 + 5   = 2053ull
	4099ull					,	// 2^12 + 3   = 4099ull
	8209ull					,	// 2^13 + 17  = 8209ull
	16411ull				,	// 2^14 + 27  = 16411ull
	32771ull				,	// 2^15 + 3   = 32771ull
	65537ull				,	// 2^16 + 1   = 65537ull
	131101ull				,	// 2^17 + 29  = 131101ull
	262147ull				,	// 2^18 + 3   = 262147ull
	524309ull				,	// 2^19 + 21  = 524309ull
	1048583ull				,	// 2^20 + 7   = 1048583ull
	2097169ull				,	// 2^21 + 17  = 2097169ull
	4194319ull				,	// 2^22 + 15  = 4194319ull
	8388617ull				,	// 2^23 + 9   = 8388617ull
	16777259ull				,	// 2^24 + 43  = 16777259ull
	33554467ull				,	// 2^25 + 35  = 33554467ull
	67108879ull				,	// 2^26 + 15  = 67108879ull
	134217757ull			,	// 2^27 + 29  = 134217757ull
	268435459ull			,	// 2^28 + 3   = 268435459ull
	536870923ull			,	// 2^29 + 11  = 536870923ull
	1073741827ull			,	// 2^30 + 85  = 1073741827ull
	2147483659ull			,	// 2^31 + 11  = 2147483659ull
	4294967311ull			,	// 2^32 + 15  = 4294967311ull
	8589934609ull			,	// 2^33 + 17  = 8589934609ull
	17179869209ull			,	// 2^34 + 25  = 17179869209ull
	34359738421ull			,	// 2^35 + 53  = 34359738421ull
	68719476767ull			,	// 2^36 + 31  = 68719476767ull
	137438953481ull			,	// 2^37 + 9   = 137438953481ull
	274877906951ull			,	// 2^38 + 7   = 274877906951ull
	549755813911ull			,	// 2^39 + 23  = 549755813911ull
	1099511627791ull		,	// 2^40 + 15  = 1099511627791ull
	2199023255579ull		,	// 2^41 + 27  = 2199023255579ull
	4398046511119ull		,	// 2^42 + 15  = 4398046511119ull
	8796093022237ull		,	// 2^43 + 29  = 8796093022237ull
	17592186044423ull		,	// 2^44 + 7   = 17592186044423ull
	35184372088891ull		,	// 2^45 + 59  = 35184372088891ull
	70368744177679ull		,	// 2^46 + 15  = 70368744177679ull
	140737488355333ull		,	// 2^47 + 5   = 140737488355333ull
	281474976710677ull		,	// 2^48 + 21  = 281474976710677ull
	562949953421381ull		,	// 2^49 + 69  = 562949953421381ull
	1125899906842679ull		,	// 2^50 + 55  = 1125899906842679ull
	2251799813685269ull		,	// 2^51 + 21  = 2251799813685269ull
	4503599627370517ull		,	// 2^52 + 21  = 4503599627370517ull
	9007199254740997ull		,	// 2^53 + 5   = 9007199254740997ull
	18014398509482143ull	,	// 2^54 + 159 = 18014398509482143ull
	36028797018963971ull	,	// 2^55 + 3   = 36028797018963971ull
	72057594037928017ull	,	// 2^56 + 81  = 72057594037928017ull
	144115188075855881ull	,	// 2^57 + 9   = 144115188075855881ull
	288230376151711813ull	,	// 2^58 + 69  = 288230376151711813ull
	576460752303423619ull	,	// 2^59 + 131 = 576460752303423619ull
	1152921504606847009ull	,	// 2^60 + 33  = 1152921504606847009ull
	2305843009213693967ull	,	// 2^61 + 15  = 2305843009213693967ull
	4611686018427388039ull	,	// 2^62 + 135 = 4611686018427388039ull
	9223372036854775837ull	,	// 2^63 + 29  = 9223372036854775837ull
//	18446744073709551629ull	,	// 2^64 + 13  = 18446744073709551629ull	//[UINT64]の範囲を超える大きさなので不可。→つまり、[UINT64]の最大値 - シフト許可幅 より小さい最大の素数を割り当てる必要がある。……が、その素数が分からないので、パス。
	18446744073709551360ull		// 2^64 - 254 = 18446744073709551360ull
								// これは     「18446744073709551360」となるので、明らかに素数ではない(10で割れる)が、上記の通り、素数が分からないので、単純に「[UINT64]の最大値 - シフト許可幅の最大値」を割り当てている。
								// 2^64については、後で書き直す。(おそらく自分で素数を計算しない限り不可能。)
};



//コンストラクタ
ICHashT::InsertedChainedHashTable(){
#ifdef ICHashT_DebagMode
	// デバッグ用の設定
	Use_define_return_Hash_flag = false;
#endif//ICHashT_DebagMode

	PrimeList_Num        = 9;													//reHash()でアクセスされるので、必ず初期化しておく。
	
	NumOfElementsInTheTable = 0;	//テーブル上に格納されているkey valueペアの数。(テーブルの充填率を計算する際に用いる。)

	TableLength          = SmallestPrimeList_LargerThanPower2[PrimeList_Num];	//デフォルトのテーブルサイズ(1031)
	AllowedSumOfShift    = 254;	//合計シフト数制限 //許可される右シフトの合計値について、最大値の指定。(0～254まで。)
	AllowedRightShiftNum = 254;	//右シフトを許可する最大値の指定。(0～254まで。)
	TrueTableLength      = TableLength + AllowedRightShiftNum;					//実際に確保するテーブル長を計算
	
	mallocTable(HashTable,TrueTableLength);	//テーブルの確保、及び初期化
}
//コンストラクタのオーバーロード(多重定義)
ICHashT::InsertedChainedHashTable( UINT64 TableLength_set ){
#ifdef ICHashT_DebagMode
	// デバッグ用の設定
	Use_define_return_Hash_flag = false;
#endif//ICHashT_DebagMode

	//[TableLength]が大きすぎる場合のエラー処理は省略。( SmallestPrimeList_LargerThanPower2[64]==18446744073709551360 以下である必要がある。)

	PrimeList_Num = 0;															//reHash()でアクセスされるので、必ず初期化しておく。
	for(; PrimeList_Num<64; PrimeList_Num++){ //32bit対応するのであれば、注意した方が良いのかな?(素数表も32bitならサイズを小さくした方が良い。)
		if( SmallestPrimeList_LargerThanPower2[PrimeList_Num] >= TableLength_set ){
			break;		//for文を抜ける。
		}
	}
	
	NumOfElementsInTheTable = 0;	//テーブル上に格納されているkey valueペアの数。(テーブルの充填率を計算する際に用いる。)

	TableLength          = SmallestPrimeList_LargerThanPower2[PrimeList_Num];
	AllowedSumOfShift    = 254;	//合計シフト数制限 //許可される右シフトの合計値について、最大値の指定。(0～254まで。)
	AllowedRightShiftNum = 254;	//右シフトを許可する最大値の指定。(0～254まで。)
	TrueTableLength      = TableLength + AllowedRightShiftNum;					//実際に確保するテーブル長を計算

	mallocTable(HashTable,TrueTableLength);	//テーブルの確保、及び初期化
}
//ディストラクタ
ICHashT::~InsertedChainedHashTable(){
	freeTable(HashTable);
}



//指定された値からハッシュ値を生成する関数
void ICHashT::makeHash( UINT64& key, UINT64& return_Hash, UINT64& TableLength_makeHash ){
	//keyより、[0 ～ UINT64の最大値]までの範囲の値(ハッシュ値)を生成する。

//	UINT64 HashValue = std::hash<UINT64>()(key);	//ハッシュ値の生成。[std::hash]を利用する。
																		//[UINT64](==[unsigned long long])を指定しても、何故か出力が32bitにしかならない。→読み込み自体は64bit分のデータからハッシュが生成されているように感じる。
																		//従って、始めから32bitごとに切り分けて、を二回実行し、ビット和で結合して64bitの出力を得る。
																		//→ハッシュ値が衝突してダメだった。
																		//→ハッシュ関数を二回繰り返すことにする。
																		//→ハッシュ関数としての性能が悪化し、LoadFactorが7割程度までしか上がらなくなってしまうので、絶対にダメ。
	/*
	//これは絶対にダメ。(LoadFactorが70%～75%以下までしか上がらなくなり、そこでリハッシュされてしまう。)

	unsigned __int32* key_p = (unsigned __int32*)&key;
//	printf("%lX\n",(unsigned __int32)*(key_p  ));
//	printf("%lX\n\n\n",(unsigned __int32)*(key_p+1));
	UINT64 HashValue_2of1 = std::hash<UINT64>()( key);	//ハッシュ値の生成。[std::hash]を利用する。
	UINT64 HashValue_2of2 = std::hash<UINT64>()(~key);	//ハッシュ値の生成。[std::hash]を利用する。//ビットシフトだと情報が欠落するので、ビット反転にした。
//	printf("%lX\n",    (unsigned __int32)HashValue_2of1);
//	printf("%lX\n\n\n",(unsigned __int32)HashValue_2of2);
	UINT64 HashValue = ((UINT64)HashValue_2of2)<<8*4 | HashValue_2of1;
//	printf("%llX\n",(UINT64)HashValue);
	*/

	UINT64 HashValue = std::hash<UINT64>()( key );		//ハッシュ値の生成。[std::hash]を利用する。

#ifdef ICHashT_DebagMode
	// デバッグ用の設定
	if( Use_define_return_Hash_flag == true ){
		return_Hash = define_return_Hash;
	}else{
		return_Hash = HashValue % TableLength_makeHash;
	}
#else //ICHashT_DebagMode
	// リリース用の設定
	return_Hash = HashValue % TableLength_makeHash;
#endif//ICHashT_DebagMode

//	printf("%lld\n",(UINT64)return_Hash);
}



void ICHashT::mallocTable( struct HashStruct*& HashTable_p_for_malloc, UINT64& TrueTableLength_mallocTable ){

	HashTable_p_for_malloc = (HashStruct*)malloc((size_t)TrueTableLength_mallocTable*sizeof(HashStruct));	//size_tが[UINT64]より小さい場合は、ハッシュテーブルのサイズが制限される可能性がある。
																											//あと、本当は、環境ごとのsize_tのサイズを確認し、エラー処理を行った方が良い。
	//mallocに失敗した場合、falseを返すようにした方が良い。(メモリが足りない場合に失敗する。)

	HashStruct HashStruct_for_init;
	HashStruct_for_init.key      = 0;
	HashStruct_for_init.value    = 0;
	HashStruct_for_init.previous = 255;	//[previous==255] 配列が空であることを示す。
	HashStruct_for_init.next     = 0;
	
	//初期化(データをNULLにしておかないと、参照したテーブルが既に使われているのか、挿入しても大丈夫なのかが分からない。)
	for(int i=0; i<TrueTableLength_mallocTable; i++){
		HashTable_p_for_malloc[i] = HashStruct_for_init;
	}
}
void ICHashT::freeTable( struct HashStruct*& HashTable_p_for_free ){
	free(HashTable_p_for_free);
}



void ICHashT::AllClear(){
	
	HashStruct HashStruct_for_init;
	HashStruct_for_init.key      = 0;
	HashStruct_for_init.value    = 0;
	HashStruct_for_init.previous = 255;	//[previous==255] 配列が空であることを示す。
	HashStruct_for_init.next     = 0;

	//初期化(データをNULLにしておかないと、参照したテーブルが既に使われているのか、挿入しても大丈夫なのかが分からない。)
	for(int i=0; i<TrueTableLength; i++){
		HashTable[i] = HashStruct_for_init;
	}
}



//全要素出力・走査→イテレータを用いて実装しようと思ったが、C++11的な、最近の技術で、信用ならないので使わない事にした。
void ICHashT::AllScanInit( AllScanPlace& AllScanNowPlace ){
#define AllScanStart 0x0000000000000000
	AllScanNowPlace = (AllScanPlace)AllScanStart;		//カーソルを初期位置にセット。
	AllScanEnd      = (AllScanPlace)TrueTableLength;
	AllScanNextShift( AllScanNowPlace );//これを一回だけ実行しておかないと、一回目のfor文で、必ず、HashTable[0]のkeyとvalueが引っかかる事になってしまう。
#undef AllScanStart
}
void ICHashT::AllScanNextShift( AllScanPlace& AllScanNowPlace ){
	for(; AllScanNowPlace < AllScanEnd; ){
		if(HashTable[AllScanNowPlace].previous != 255){//配列に要素があるかどうかの確認。
			return;//要素を発見したのでreturn;
		}
		AllScanNowPlace++;//次の要素を検索する。
	}
}
void ICHashT::AllScanGetData( UINT64& key, void*& value, AllScanPlace& AllScanNowPlace ){
	key   = HashTable[AllScanNowPlace].key;
	value = HashTable[AllScanNowPlace].value;
	AllScanNowPlace++;//for文と途中でreturnしたので、加算されていない。そのため、ここで加算しておく。
}



#ifdef ICHashT_DebagMode
	// デバッグ用の設定

//ハッシュテーブル上で空の要素も含めた全ての要素のダンプ
	//全要素出力・走査→イテレータを用いて実装しようと思ったが、C++11的な、最近の技術で、信用ならないので使わない事にした。
void ICHashT::DumpAll_for_Debag_Init( DumpAll_Place& DumpAll_NowPlace ){
#define DumpAll_Start 0x0000000000000000
	DumpAll_NowPlace = (DumpAll_Place)DumpAll_Start;		//カーソルを初期位置にセット。
	DumpAll_End      = (DumpAll_Place)TrueTableLength;
#undef AllScanStart
}
void ICHashT::DumpAll_for_Debag_GetData( UINT64& key, void*& value, unsigned char& previous, unsigned char& next, DumpAll_Place& DumpAll_NowPlace ){
	key      = HashTable[DumpAll_NowPlace].key;
	value    = HashTable[DumpAll_NowPlace].value;
	previous = HashTable[DumpAll_NowPlace].previous;
	next     = HashTable[DumpAll_NowPlace].next;
}

#endif//ICHashT_DebagMode



void ICHashT::setX( char& set_function_type, UINT64& key, void*& value, void**& return_value_pp, bool& result, bool& NeedReHash, struct HashStruct*& HashTable_setX, UINT64& TableLength_setX, UINT64& TrueTableLength_setX ){
	//[char set_function_type]
	//		'0'はset0
	//		'1'はset1
	//		'2'はset2

	//既に同一のkeyが登録されている場合。→valueを更新

	//[previous==255] 配列が空であることを示す。
	//[previous!=255] 配列は使用済み。ただし、ハッシュ値の重複が無いため、チェーンはされていない。
	//[next    ==  0] 要素の末尾
	//[previous==  0] slaveではない通常の要素として使われていることを示す。(必ずチェーンの先頭となる)
	//[ next == 0 && previous == 0 ] 要素数1のチェーンの先頭。
	
	result = true;			//全ての[return=ture;]にtureを入れるのを忘れそうなので、先にtrueにしておく。([false]は数が少ないのでらく。)
	NeedReHash = false;		//全ての[return=false;]にfalseを入れるのを忘れそうなので、先にfalseにしておく。(trueの場合だけ書く。)(毎回毎回falseばかりなのに、それぞれの場所にバラバラと書くということは、冗長なアセンブラコードを生成させる。)
							//setX();関数の実行結果が、[NeedReHash==true]の場合に、リハッシュするように設計する。

	UINT64 first;					//チェーンの先頭、の意味を込めて。
	makeHash(key,first,TableLength_setX);	//ハッシュ値の生成

	if( HashTable_setX[first].previous == 255 ){ //[if( HashTable_setX[return_Hash].key == 0 ){]としないのは、[key]自身が[0]という値を取れなくなってしまうから。[if( HashTable_setX[return_Hash].value == 0 ){]としないのも同様。(今はvalueはvoid*だが、void*は単なる64bitのメモリに過ぎないので、直接値を格納する可能性もある。)
		// テーブルが空いていた場合。
		// [処理番号①]
		HashTable_setX[first].key      = key;
		HashTable_setX[first].value    = value;
		HashTable_setX[first].previous = 0;
		HashTable_setX[first].next     = 0;

		//NeedReHash = false; result = true;
		return;
	}


	//======================================================================================================================================================================================================================================================
	if( HashTable_setX[first].previous ==   0 ){
		// [  HashTable_setX[first] が、keyチェーンの先頭の場合。    ]
		// [  → 最もkeyに近い配列の空きスペースに、要素を格納する。 ]
		// [                                                         ]
		// [  [処理番号①]、[処理番号②]、[処理番号③]               ]

		//*
		// [実装方法01::素直な実装]
		{
			UINT64 NowPlace = first;

			while(1){
				if( HashTable_setX[NowPlace].key == key ){
					// keyが一致した場合。
					// set_function_type == '0'  →  result = false; valueの上書きを行う。
					// set_function_type == '1'  →  result = false; 何も行わずにreturn。
					// set_function_type == '2'  →  result = false; valueの上書きを行わない代わりに、valueへのポインタを返す。
					if( set_function_type == '0' ){
						//NeedReHash = false;
						HashTable_setX[NowPlace].value = value;
						result = false; return;
					}else if( set_function_type=='1' ){
						//NeedReHash = false;
						result = false; return;
					}else{// if( set_function_type == '2' ){
						return_value_pp = &HashTable_setX[NowPlace].value;	//   return_value_pp :: &(HashTable_setX[return_Hash].value)
																			//  *return_value_pp ::   HashTable_setX[return_Hash].value == void* ←これに対してmallocとかしてメモリを割り当てる予定。
																			// **return_value_pp :: mallocで確保していれば、実体がある。
																			//[free((void*)(*return_value_pp));]←mallocで確保した物を解放したいのであれば。
						//NeedReHash = false;
						result = false; return;								//set2():keyが既に格納されている場合は、valueを取得してreturn;(valueには手を加えないが、voidポインタなので、中身の判断と書き換えができる。)→中身を比較判定してから書き換えたい場合に便利。
					}
				}
				UINT64 NextShift = HashTable_setX[NowPlace].next;

				if(NextShift==0){break;}

				NowPlace += HashTable_setX[NowPlace].next;
			}
		}
	

		//keyの一致が無かった場合。
		//空きの走査
		{
			UINT64 last;
			{
				UINT64 NowPlace = first;

				while(1){
					unsigned char NextShift = HashTable_setX[NowPlace].next;

					if(NextShift==0){
						last = NowPlace;
						break;
					}

					for( unsigned char i=1; i<NextShift; i++ ){ //現在地[return_Hash]の1つシフトである、[return_Hash+1]から、[return_Hash+NextShift]の一つ手前である、[return_Hash+NextShift-1]までの間に、空の配列が無いかどうかを走査する。
						if( HashTable_setX[NowPlace+i].previous == 255 ){
							// 空いている配列を発見。
							// [処理番号②]
							UINT64 empty_front = NowPlace;
							UINT64 empty       = NowPlace + i;
							UINT64 empty__back = NowPlace + NextShift;

							HashTable_setX[ empty       ].key      = key;			// データの書き込み処理
							HashTable_setX[ empty       ].value    = value;			// データの書き込み処理

							HashTable_setX[ empty_front ].next     = i;				// next接続を繋ぎ替え     ( 前からある要素 )
							HashTable_setX[ empty       ].previous = i;				// previous接続を設定     (   新しい要素   )
							HashTable_setX[ empty       ].next     = NextShift - i;	// next接続を設定         (   新しい要素   )
							HashTable_setX[ empty__back ].previous = NextShift - i;	// previous接続を繋ぎ替え ( 前からある要素 )

							//NeedReHash = false; result = true;
							return;
						}
					}

					NowPlace += NextShift;
				}
			}


			//keyチェーンの最後まで走査しても、チェーンの間に空きが無かった場合。
			//keyチェーンの後ろ(last以降)に、空きを探す
			{
				UINT64 SumOfShift = last - first;	// [チェーンの末尾] における [合計シフト数] を計算する。
				UINT64 ShiftLimit = AllowedSumOfShift - SumOfShift;	//[AllowedRightShiftNum - SumOfShift]の計算結果が0以下の値、つまり、負になることは、仕組み的には不可能。(そもそも[合計シフト数制限]を越えてチェーンすること自体が禁止されているため。)ただし、もしなった場合は、[unsigned]なので、誤作動する。
				if( ShiftLimit > AllowedRightShiftNum ){
					ShiftLimit = AllowedRightShiftNum;
				}

				UINT64 LimitOfTableSize = TrueTableLength_setX - last;	//これの確認は、keyがチェーンされていると、無限幅シフト可能なので、必須。

				for( unsigned char i=1; i <= ShiftLimit && i <= LimitOfTableSize; i++ ){		//[for( UINT64 i=1; (SumOfShift+i) <= AllowedRightShiftNum; i++ ){]と書くと、VisualStudioでは、毎回足し算を計算し始めてしまう。[for( UINT64 i=1; i <= (AllowedRightShiftNum-SumOfShift); i++ ){]とすれば、一回だけ引き算を行う。

					UINT64 empty = last + i;

					if( HashTable_setX[ empty ].previous == 255 ){
						// 空いている配列を発見。
						// [処理番号③]

						UINT64 empty_front = last;

						HashTable_setX[ empty       ].key      = key;		// データの書き込み処理 (   新しい要素   )
						HashTable_setX[ empty       ].value    = value;	// データの書き込み処理 (   新しい要素   )

						HashTable_setX[ empty_front ].next     = i;		// next接続を繋ぎ替え   ( 前からある要素 )
						HashTable_setX[ empty       ].previous = i;		// データの書き込み処理 (   新しい要素   )
						HashTable_setX[ empty       ].next     = 0;		// データの書き込み処理 (   新しい要素   )

						//NeedReHash = false; result = true;
						return;
					}
				}

				//keyの挿入に失敗。( [合計シフト制限] を超過 → リハッシュの必要がある。 )
					// [setX();]関数内部からリハッシュを実行すると、再帰的な処理が発生するので、[result = false; return;]するだけにする。
					//従って、[setX();]関数の外部で、resultをチェックし、falseの場合には、[reHash();]の呼び出しと、失敗したkeyの挿入のリトライを実装しておく。
				NeedReHash = true; result = false; return;
			}
		}
		//*/


		/*
		//(動作確認してないし、直してないから動かないハズ。)

		// [実装方法02::難解な実装]
		//空きの走査と、keyの一致を同時進行で確認する。ただし、先に空きが発見された場合は、keyの一致が空きまでしか確認されないので、最後までkeyの一致が無い事を確認してから代入している。
		//空きの走査
		{
			UINT64 last;
			UINT64 empty = 0;	//0は使われないので、0で初期化しても問題ない。//[empty]はデータ構造的に、必ず1以上の値を取る。(HashTable_setX[0]がemptyであることは、データ構造上不可能。)
			{
				UINT64 NowPlace = first;

				while(1){
					if( HashTable_setX[NowPlace].key == key ){ //keyの一致の確認。
						// keyが一致した場合。
						// set_function_type=='0'  →  result = false; valueの上書きを行う。
						// set_function_type=='1'  →  result = false; 何も行わずにreturn。
						// set_function_type=='2'  →  result = false; valueの上書きを行わない代わりに、valueへのポインタを返す。
						if( set_function_type=='0' ){
							//NeedReHash = false;
							HashTable_setX[NowPlace].value = value;
							result = false; return;
						}else if(set_function_type=='1'){
							//NeedReHash = false;
							result = false; return;
						}else{// if(set_function_type=='2'){
							return_value_pp = HashTable_setX[NowPlace].value;	//   return_value_pp :: &(HashTable_setX[return_Hash].value)
																				//  *return_value_pp ::   HashTable_setX[return_Hash].value == void* ←これに対してmallocとかしてメモリを割り当てる予定。
																				// **return_value_pp :: mallocで確保していれば、実体がある。
																				//[free((void*)(*return_value_pp));]←mallocで確保した物を解放したいのであれば。
							//NeedReHash = false;
							result = false; return;								//set2():keyが既に格納されている場合は、valueを取得してreturn;(valueには手を加えないが、voidポインタなので、中身の判断と書き換えができる。)→中身を比較判定してから書き換えたい場合に便利。
						}
					}
					UINT64 NextShift = HashTable_setX[NowPlace].next;

					if(NextShift==0){
						last = NowPlace;
						break;
					}

					for( UINT64 i=1; i<NextShift; i++ ){ //現在地[return_Hash]の1つシフトである、[return_Hash+1]から、[return_Hash+NextShift]の一つ手前である、[return_Hash+NextShift-1]までの間に、空の配列が無いかどうかを走査する。
						if( HashTable_setX[NowPlace+i].previous == 255 ){
							//空いている配列を発見。

							//一致するkeyが無いことを、チェーンの最後まで確認する。
							{
								UINT64 NextPlace = NowPlace + NextShift;

								while(1){
									if( HashTable_setX[NextPlace].key == key ){
										// keyが一致した場合。
										// set_function_type=='0'  →  result = false; valueの上書きを行う。
										// set_function_type=='1'  →  result = false; 何も行わずにreturn。
										// set_function_type=='2'  →  result = false; valueの上書きを行わない代わりに、valueへのポインタを返す。
										if( set_function_type=='0' ){
											//NeedReHash = false;
											HashTable_setX[NowPlace].value = value;
											result = false; return;
										}else if( set_function_type=='1' ){
											//NeedReHash = false;
											result = false; return;
										}else{// if(set_function_type=='2'){
											return_value_pp = HashTable_setX[NowPlace].value;	//   return_value_pp :: &(HashTable_setX[return_Hash].value)
																								//  *return_value_pp ::   HashTable_setX[return_Hash].value == void* ←これに対してmallocとかしてメモリを割り当てる予定。
																								// **return_value_pp :: mallocで確保していれば、実体がある。
																								//[free((void*)(*return_value_pp));]←mallocで確保した物を解放したいのであれば。
											//NeedReHash = false;
											result = false; return;								//set2():keyが既に格納されている場合は、valueを取得してreturn;(valueには手を加えないが、voidポインタなので、中身の判断と書き換えができる。)→中身を比較判定してから書き換えたい場合に便利。
										}
									}
									UINT64 NextShift = HashTable_setX[NextPlace].next;

									if(NextShift==0){break;}	//一致するkeyが無かったので、while()文を抜ける。

									NextPlace += HashTable_setX[NextPlace].next;
								}
							}

							//一致するkeyが無い事を確認したので、値を配列の空きに格納する。
							// [処理番号②]
							UINT64 empty = NowPlace + i;

							HashTable_setX[ empty                ].key      = key;				// データの書き込み処理
							HashTable_setX[ empty                ].value    = value;			// データの書き込み処理

							HashTable_setX[ NowPlace             ].next     = i;				// next接続を繋ぎ替え     ( 前からある要素 )

							HashTable_setX[ empty                ].previous = i;				// previous接続を設定     (   新しい要素   )
							HashTable_setX[ empty                ].next     = NextShift - i;	// next接続を設定         (   新しい要素   )

							HashTable_setX[ NowPlace + NextShift ].previous = NextShift - i;	// previous接続を繋ぎ替え ( 前からある要素 )

							//NeedReHash = false; result = true;
							return;
						}
					}

					NowPlace += HashTable_setX[NowPlace].next;
				}
			}


			//keyチェーンの最後まで走査しても、チェーンの間に空きが無かった場合。
			//keyチェーンの後ろ(last以降)に、空きを探す
			{
				UINT64 SumOfShift = last - first;	// [チェーンの末尾] における [合計シフト数] を計算する。
				UINT64 ShiftLimit = AllowedSumOfShift - SumOfShift;	//[AllowedRightShiftNum - SumOfShift]の計算結果が0以下の値、つまり、負になることは、仕組み的には不可能。(そもそも[合計シフト数制限]を越えてチェーンすること自体が禁止されているため。)ただし、もしなった場合は、[unsigned]なので、誤作動する。
				if( ShiftLimit > AllowedRightShiftNum ){
					ShiftLimit = AllowedRightShiftNum;
				}

				UINT64 LimitOfTableSize = TrueTableLength_setX - last;	//これの確認は、keyがチェーンされていると、無限幅シフト可能なので、必須。

				for( UINT64 i=1; i <= ShiftLimit && i <= LimitOfTableSize; i++ ){		//[for( UINT64 i=1; (SumOfShift+i) <= AllowedRightShiftNum; i++ ){]と書くと、VisualStudioでは、毎回足し算を計算し始めてしまう。[for( UINT64 i=1; i <= (AllowedRightShiftNum-SumOfShift); i++ ){]とすれば、一回だけ引き算を行う。

					UINT64 empty = last + i;

					if( HashTable_setX[ empty ].previous == 255 ){
						// 空いている配列を発見。
						// [処理番号③]
						HashTable_setX[ empty ].key      = key;		// データの書き込み処理 (   新しい要素   )
						HashTable_setX[ empty ].value    = value;	// データの書き込み処理 (   新しい要素   )
						HashTable_setX[ empty ].previous = i;		// データの書き込み処理 (   新しい要素   )
						HashTable_setX[ empty ].next     = 0;		// データの書き込み処理 (   新しい要素   )

						HashTable_setX[ last  ].next     = i;		// next接続を繋ぎ替え   ( 前からある要素 )

						//NeedReHash = false; result = true;
						return;
					}
				}

				//keyの挿入に失敗。( [合計シフト制限] を超過 → リハッシュの必要がある。 )
					// [setX();]関数内部からリハッシュを実行すると、再帰的な処理が発生するので、[result = false; return;]するだけにする。
					//従って、[setX();]関数の外部で、resultをチェックし、falseの場合には、[reHash();]の呼び出しと、失敗したkeyの挿入のリトライを実装しておく。
				NeedReHash = true; result = false; return;
			}
		}
		//*/
	//======================================================================================================================================================================================================================================================
	}else{
		// [  HashTable_setX[first] が、keyチェーンの先頭ではない場合。                                                                           ]
		// [  → HashTable_setX[first] に格納されているkeyチェーンを、HashTable_setX[first] 以外で、最もkeyに近い配列の空きスペースに移動させる。 ]
		// [  → データを移動させて空となった HashTable_setX[first] に新しいkeyを格納する。                                                       ]
		// [                                                                                                                                      ]
		// [  [処理番号④]、[処理番号⑥]、[処理番号⑧]                                                                                            ]


		// keyチェーンを遡る方向に、配列の空きを検索し、空きを発見した場合は、 HashTable_setX[first] のデータを移動させ、 HashTable_setX[first] を空ける。
		// → HashTable_setX[first] に新しいkey、valueをセットする。
		//
		// keyチェーンを遡る方向に、配列の空きが無かった場合は、keyチェーンに沿う方向に、配列の空きを検索する。
		//
		// ハッシュ値が異なる時点で、一致keyが存在する事はあり得ないので、調べる必要はない。

		

		{
			UINT64 empty = 0;	//必ず初期化する必要がある。	//シフト値として0は使用されないので、0で初期化をしておく。
			UINT64 empty_front;
			UINT64 empty__back;

			{
				// keyチェーンを遡る方向に、配列の空きを検索。
				UINT64 NowPlace = first;

				while(1){
					UINT64 PreviousShift = HashTable_setX[NowPlace].previous;

					if( PreviousShift == 0 ){
						break;
					}

					for( UINT64 i=1; i<PreviousShift; i++ ){
						if( HashTable_setX[NowPlace-i].previous == 255 ){
							// 空いている配列を発見。
							// [処理番号④]、及び、[処理番号⑥]
							empty__back = NowPlace;
							empty       = NowPlace - i;				//最初に見つけた値が、最もkeyに近い空きとは限らないので、[ PreviousShift == 0 ]となるまで、ループを回す必要がある。
							empty_front = NowPlace - PreviousShift;
						}
					}

					NowPlace -= PreviousShift;
				}
			}

			if(empty!=0){
				// keyチェーンを遡る方向に、配列の空きを検索し、空きを発見した場合。

				// emptyが、配列の空きを示している。
				// [処理番号④]、及び、[処理番号⑥]

				// [key]--[empty_front]- (empty) -[empty__back]--[X]--[X]--[first_front]--[first](want to set key2)--[first__back]--[X]

				HashTable_setX[ empty ].key      = HashTable_setX[ first ].key;		//データの移動処理 ( 前からある要素 )
				HashTable_setX[ empty ].value    = HashTable_setX[ first ].value;	//データの移動処理 ( 前からある要素 )
				HashTable_setX[ empty ].previous = (unsigned char)( empty - empty_front );

				UINT64 ShiftOf_EmptyFront_to_Empty = empty - empty_front;
				HashTable_setX[ empty_front ].next     = (unsigned char)( ShiftOf_EmptyFront_to_Empty );
				HashTable_setX[ empty       ].previous = (unsigned char)( ShiftOf_EmptyFront_to_Empty );

				if( empty__back == first ){
					// [処理番号④]、及び、 [処理番号⑤]

					if( HashTable_setX[ first ].next == 0 ){
						// [処理番号④]

					//	HashTable_setX[ empty       ].next     = 0;
						//[empty]であれば、nextは既に0に初期化されているはずなので、不要。
					}else{
						// [処理番号⑤]
						UINT64 first__back = first + HashTable_setX[first].next;

						UINT64 ShiftOf_Empty_to_FirstBack  = first + HashTable_setX[ first ].next - empty;
						HashTable_setX[ empty       ].next     = (unsigned char)( ShiftOf_Empty_to_FirstBack  );
						HashTable_setX[ first__back ].previous = (unsigned char)( ShiftOf_Empty_to_FirstBack  );
					}
				}else{
					// [処理番号⑥]、及び、[処理番号⑦]

					UINT64 ShiftOf_Empty_to_EmptyBack  = empty__back - empty;
					HashTable_setX[ empty       ].next     = (unsigned char)( ShiftOf_Empty_to_EmptyBack  );
					HashTable_setX[ empty__back ].previous = (unsigned char)( ShiftOf_Empty_to_EmptyBack  );

					UINT64 first_front = first - HashTable_setX[first].previous;

					if( HashTable_setX[first].next == 0 ){
						// [処理番号⑥]

						HashTable_setX[ first_front ].next = 0;
					}else{
						// [処理番号⑦]
						UINT64 first__back = first + HashTable_setX[first].next;

						UINT64 ShiftOf_FirstFront_to_FirstBack = HashTable_setX[first].next + HashTable_setX[first].previous;//== first__back - first_front
						HashTable_setX[ first_front ].next     = (unsigned char)( ShiftOf_FirstFront_to_FirstBack );
						HashTable_setX[ first__back ].previous = (unsigned char)( ShiftOf_FirstFront_to_FirstBack );
					}
				}
				HashTable_setX[ first ].key      = key;		// データの書き込み処理 ( 前からある要素 )
				HashTable_setX[ first ].value    = value;	// データの書き込み処理 ( 前からある要素 )
				HashTable_setX[ first ].previous = 0;		// データの書き込み処理 ( 前からある要素 )
				HashTable_setX[ first ].next     = 0;		// データの書き込み処理 ( 前からある要素 )

				//NeedReHash = false; result = true;
				return;
			}



			// 配列の空きがfirstより前に存在しなかった場合。
			// →firstより後ろの空きを探す。
			//
			// [処理番号⑧]、[処理番号⑨]、及び、[処理番号⑩]、[処理番号⑪]
			{
				UINT64 last;			//( [first] ～ [last] 間の配列に、空きが無い場合に限り、) keyチェーンの終わりを記録する。
				{
					// [first] ～ [last] 間の配列に、空きが無いか走査。
					// [処理番号⑧]、[処理番号⑨] (←空きがある場合の処理。)
					empty = 0;
					UINT64 NowPlace = first;

					while(1){
						UINT64 NextShift = HashTable_setX[NowPlace].next;

						if( NextShift == 0 ){
							last = NowPlace;	//( [first] ～ [last] までの間に、空の配列が無い場合に限り、) keyチェーンの終わりを記録する。
							break;
						}

						for( UINT64 i=1; i<NextShift; i++ ){
							if( HashTable_setX[NowPlace+i].previous == 255 ){
								// 空いている配列を発見。
								empty_front = NowPlace;
								empty       = NowPlace + i;
								empty__back = NowPlace + NextShift;
								UINT64 first_front = first - HashTable_setX[ first ].previous;
								UINT64 first__back = first + HashTable_setX[ first ].next;
								UINT64 ShiftOf_Empty_to_EmptyBack = empty__back - empty;

//								break;								//最初に見つけた値が、最もkeyに近い。

								// =======================
								HashTable_setX[ empty ].key   = HashTable_setX[ first ].key;		// データの移動処理 (   新しい要素   )
								HashTable_setX[ empty ].value = HashTable_setX[ first ].value;		// データの移動処理 (   新しい要素   )
								
								if( first == empty_front ){ //[if( empty != 0 ){]の中に処理を入れないと、 [処理番号⑩] のパターンの場合に、ここの[empty_front]が初期化されないまま使用される事になってしまう。
									// [処理番号⑨]

									UINT64 ShiftOf_FirstFront_to_Empty = empty - first_front;
									HashTable_setX[ first_front ].next     = (unsigned char)( ShiftOf_FirstFront_to_Empty );		//     next接続を繋ぎ替え ( 前からある要素 )
									HashTable_setX[ empty       ].previous = (unsigned char)( ShiftOf_FirstFront_to_Empty );		// previous接続を設定     (   新しい要素   )
								}else{
									// [処理番号⑧]

									UINT64 ShiftOf_FirstFront_to_FirstBack = first__back - first_front;
									HashTable_setX[ first_front ].next     = (unsigned char)( ShiftOf_FirstFront_to_FirstBack );	//     next接続を繋ぎ替え ( 前からある要素 )
									HashTable_setX[ first__back ].previous = (unsigned char)( ShiftOf_FirstFront_to_FirstBack );	// previous接続を繋ぎ替え ( 前からある要素 )

									UINT64 ShiftOf_EmptyFront_to_Empty = empty - empty_front;
									HashTable_setX[ empty_front ].next     = (unsigned char)( ShiftOf_EmptyFront_to_Empty );		//     next接続を繋ぎ替え ( 前からある要素 )
									HashTable_setX[ empty       ].previous = (unsigned char)( ShiftOf_EmptyFront_to_Empty );
								}
								HashTable_setX[ empty       ].next     = (unsigned char)( ShiftOf_Empty_to_EmptyBack  );		//     next接続を設定     (   新しい要素   )
								HashTable_setX[ empty__back ].previous = (unsigned char)( ShiftOf_Empty_to_EmptyBack  );		// previous接続を繋ぎ替え ( 前からある要素 )

								HashTable_setX[ first ].key      = key;		// データの書き込み処理 ( 前からある要素 )
								HashTable_setX[ first ].value    = value;	// データの書き込み処理 ( 前からある要素 )
								HashTable_setX[ first ].previous = 0;		// データの書き込み処理 ( 前からある要素 )
								HashTable_setX[ first ].next     = 0;		// データの書き込み処理 ( 前からある要素 )

								//NeedReHash = false; result = true;
								return;
								// =======================
							}
						}

						NowPlace += NextShift;
					}
				}


				/*
				//breakせず、その部分に埋め込んだので不要。

				if( empty != 0 ){
					HashTable_setX[ empty ].key   = HashTable_setX[ first ].key;		// データの移動処理 (   新しい要素   )
					HashTable_setX[ empty ].value = HashTable_setX[ first ].value;		// データの移動処理 (   新しい要素   )
								
					if( first == empty_front ){ //[if( empty != 0 ){]の中に処理を入れないと、 [処理番号⑩] のパターンの場合に、ここの[empty_front]が初期化されないまま使用される事になってしまう。
						// [処理番号⑨]

						UINT64 ShiftOf_FirstFront_to_Empty = empty - first_front;
						HashTable_setX[ first_front ].next     = (unsigned char)( ShiftOf_FirstFront_to_Empty );		//     next接続を繋ぎ替え   ( 前からある要素 )
						HashTable_setX[ empty       ].previous = (unsigned char)( ShiftOf_FirstFront_to_Empty );				// previous接続を設定       (   新しい要素   )
						HashTable_setX[ empty       ].next     = (unsigned char)( ShiftOf_Empty_to_EmptyBack  );				//     next接続を設定       (   新しい要素   )
						HashTable_setX[ empty__back ].previous = (unsigned char)( ShiftOf_Empty_to_EmptyBack  );		// previous接続を繋ぎ替え   ( 前からある要素 )
					}else{
						// [処理番号⑧]

						UINT64 ShiftOf_FirstFront_to_FirstBack = first__back - first_front;
						HashTable_setX[ first_front ].next     = (unsigned char)( ShiftOf_FirstFront_to_FirstBack );	//     next接続を繋ぎ替え   ( 前からある要素 )
						HashTable_setX[ first__back ].previous = (unsigned char)( ShiftOf_FirstFront_to_FirstBack );	// previous接続を繋ぎ替え   ( 前からある要素 )

						UINT64 ShiftOf_EmptyFront_to_Empty = empty - empty_front;
						HashTable_setX[ empty_front ].next     = (unsigned char)( ShiftOf_EmptyFront_to_Empty );		//     next接続を繋ぎ替え   ( 前からある要素 )
						HashTable_setX[ empty       ].previous = (unsigned char)( ShiftOf_EmptyFront_to_Empty );
						HashTable_setX[ empty       ].next     = (unsigned char)( ShiftOf_Empty_to_EmptyBack  );
						HashTable_setX[ empty__back ].previous = (unsigned char)( ShiftOf_Empty_to_EmptyBack  );		// previous接続を繋ぎ替え   ( 前からある要素 )
					}

					HashTable_setX[ first ].key      = key;		// データの書き込み処理 ( 前からある要素 )
					HashTable_setX[ first ].value    = value;	// データの書き込み処理 ( 前からある要素 )
					HashTable_setX[ first ].previous = 0;		// データの書き込み処理 ( 前からある要素 )
					HashTable_setX[ first ].next     = 0;		// データの書き込み処理 ( 前からある要素 )

					//NeedReHash = false; result = true;
					return;
				}
				*/




				{
					// [first] ～ [last] 間の配列に、空きが無い場合。
					// →[last]より後ろの空きを走査する。
					// [処理番号⑩]、[処理番号⑪]

					UINT64 SumOfShift = last - first;	// [チェーンの末尾] における [合計シフト数] を計算する。
					UINT64 ShiftLimit = AllowedSumOfShift - SumOfShift;	//[AllowedRightShiftNum - SumOfShift]の計算結果が0以下の値、つまり、負になることは、仕組み的には不可能。(そもそも[合計シフト数制限]を越えてチェーンすること自体が禁止されているため。)ただし、もしなった場合は、[unsigned]なので、誤作動する。
					if( ShiftLimit > AllowedRightShiftNum ){
						ShiftLimit = AllowedRightShiftNum;
					}

					UINT64 LimitOfTableSize = TrueTableLength_setX - last;	//これの確認は、keyがチェーンされていると、無限幅シフト可能なので、必須。

					for( unsigned char i=1; i <= ShiftLimit && i <= LimitOfTableSize; i++ ){		//[for( UINT64 i=1; (SumOfShift+i) <= AllowedRightShiftNum; i++ ){]と書くと、VisualStudioでは、毎回足し算を計算し始めてしまう。[for( UINT64 i=1; i <= (AllowedRightShiftNum-SumOfShift); i++ ){]とすれば、一回だけ引き算を行う。

						UINT64 empty = last + i;

						if( HashTable_setX[ empty ].previous == 255 ){
							// 空いている配列を発見。

							UINT64 first_front = first - HashTable_setX[ first ].previous;
							HashTable_setX[ empty ].key      = HashTable_setX[ first ].key;							// データの移動処理 (   新しい要素   )
							HashTable_setX[ empty ].value    = HashTable_setX[ first ].value;						// データの移動処理 (   新しい要素   )

							if( HashTable_setX[ first ].next == 0 ){
								//[処理番号⑪]

								UINT64                                        ShiftOf_FirstFront_to_Empty = empty - first_front;
								HashTable_setX[ first_front ].next     = (unsigned char)ShiftOf_FirstFront_to_Empty;	//     next接続を繋ぎ替え ( 前からある要素 )
								HashTable_setX[ empty       ].previous = (unsigned char)ShiftOf_FirstFront_to_Empty;	// previous接続を繋ぎ替え ( 前からある要素 )
							//	HashTable_setX[ empty       ].next     = 0;												// previous接続を繋ぎ替え ( 前からある要素 )
								//[empty]であれば、nextは既に0に初期化されているはずなので、不要。
							}else{
								//[処理番号⑩]

								UINT64                                                                          first__back = first + HashTable_setX[ first ].next;
								UINT64                                        ShiftOf_FirstFront_to_FirstBack = first__back - first_front;
								HashTable_setX[ first_front ].next     = (unsigned char)ShiftOf_FirstFront_to_FirstBack;	//     next接続を繋ぎ替え ( 前からある要素 )
								HashTable_setX[ first__back ].previous = (unsigned char)ShiftOf_FirstFront_to_FirstBack;	// previous接続を繋ぎ替え ( 前からある要素 )
								
								HashTable_setX[ last  ].next     = i;													//     next接続を繋ぎ替え ( 前からある要素 )
								HashTable_setX[ empty ].previous = i;													// previous接続を設定     (   新しい要素   )
							//	HashTable_setX[ empty ].next     = 0;													//     next接続を設定     (   新しい要素   )
								//[empty]であれば、nextは既に0に初期化されているはずなので、不要。
							}
							HashTable_setX[ first ].key      = key;		// データの書き込み処理 ( 前からある要素 )
							HashTable_setX[ first ].value    = value;	// データの書き込み処理 ( 前からある要素 )
							HashTable_setX[ first ].previous = 0;		// データの書き込み処理 ( 前からある要素 )
							HashTable_setX[ first ].next     = 0;		// データの書き込み処理 ( 前からある要素 )

							//NeedReHash = false; result = true;
							return;
						}
					}

					//keyの挿入に失敗。( [合計シフト制限] を超過 → リハッシュの必要がある。 )
						// [setX();]関数内部からリハッシュを実行すると、再帰的な処理が発生するので、[result = false; return;]するだけにする。
						//従って、[setX();]関数の外部で、resultをチェックし、falseの場合には、[reHash();]の呼び出しと、失敗したkeyの挿入のリトライを実装しておく。
					NeedReHash = true; result = false; return;
				}

			}
		}

	}
	//======================================================================================================================================================================================================================================================
}
void ICHashT::setXwithReHash( char set_function_type, UINT64& key, void*& value, void**& return_value_pp, bool& result, struct HashStruct*& HashTable_setX, UINT64& TableLength_setX, UINT64& TrueTableLength_setX ){

	bool NeedReHash;

ReSet:
	setX( set_function_type, key, value, return_value_pp, result, NeedReHash, HashTable, TableLength, TrueTableLength );
	if( NeedReHash == true ){
		reHash();
		goto ReSet;		//リハッシュした場合は、もう一度key,valueの挿入を試みる必要がある。(do-while()を用いると、比較が一回多くなるので、用いない。)
	}

	if(result==true){
		//要素の挿入に成功した場合は、カウント。( 失敗している場合は、keyが重複している場合で、要素数は変化しない。 )
		NumOfElementsInTheTable++;	//テーブル上に格納されているkey valueペアの数のカウントを加算。
	//	LoadFactor = NumOfElementsInTheTable / TrueTableLength;			//使わない値を毎回計算させるのも……。
	}
}
void ICHashT::set0( UINT64& key, void*& value ){
	void** return_value_pp;	//ダミー(実際には使われない。)
	bool   result;			//ダミー(実際には使われない。)
	setXwithReHash( '0', key, value, return_value_pp, result, HashTable, TableLength, TrueTableLength );
}
void ICHashT::set1( UINT64& key, void*& value ){
	void** return_value_pp;	//ダミー(実際には使われない。)
	bool   result;			//ダミー(実際には使われない。)
	setXwithReHash( '1', key, value, return_value_pp, result, HashTable, TableLength, TrueTableLength );
}
void ICHashT::set2( UINT64& key, void*& value, void**& return_value_pp, bool& result ){
	setXwithReHash( '2', key, value, return_value_pp, result, HashTable, TableLength, TrueTableLength );
}
#ifdef ICHashT_DebagMode
	// デバッグ用の設定
void ICHashT::set_for_Debag( UINT64 TableElementNum, UINT64 key, void* value, unsigned char previous, unsigned char next ){ //指定した配列番号に、任意のデータをセットする。
	HashTable[ TableElementNum ].key      = key;		// データの書き込み処理
	HashTable[ TableElementNum ].value    = value;		// データの書き込み処理
	HashTable[ TableElementNum ].previous = previous;	// データの書き込み処理
	HashTable[ TableElementNum ].next     = next;		// データの書き込み処理
}
#endif//ICHashT_DebagMode


void ICHashT::getX( char set_function_type, UINT64& key, void*& value, void**& pp_value, bool& result ){//keyおよびvalueが共に0を取る場合に、値の有無を確認できないので、[bool& result]で[true]or[false]を返す。
	UINT64 first;				//チェーンの先頭、の意味を込めて。
	makeHash(key,first,TableLength);	//ハッシュ値の生成
	UINT64 PreviousShift = HashTable[first].previous;
	if( PreviousShift != 0 ){
		//NotFound(該当データ無し)
		result = false; return;
	}

	{
		UINT64 NextShift;
		UINT64 TagetPlace = first;

		do{
			if( HashTable[ TagetPlace ].key == key ){//keyの一致を確認
				//keyが一致。
				if(set_function_type=='0'){
					value     =  HashTable[ TagetPlace ].value;
				}else{//if(set_function_type=='1'){
					pp_value  = &HashTable[ TagetPlace ].value;
				}
				result = true; return;
			}
			NextShift = HashTable[ TagetPlace ].next;
			TagetPlace += NextShift;				//次の要素へ移動する量を計算。
		}while( NextShift != 0 );					//チェーンの末尾に到達したかどうか。
		//チェーンの末尾に到達。

		result = false; return;//最後まで探しても無かったので、[false]
	}
}
void ICHashT::get0( UINT64& key, void* & value, bool& result ){
	void** pp_value;	//ダミー(実際には使われない。)
	getX( '0', key, value, pp_value, result );
}
void ICHashT::get1( UINT64& key, void**& pp_value, bool& result ){
	void* value;		//ダミー(実際には使われない。)
	getX( '1', key, value, pp_value, result );
}

//find関数はget関数と全く同じ動作だが、valueを返さない。
void ICHashT::find( UINT64& key, bool& result ){
	UINT64 first;				//チェーンの先頭、の意味を込めて。
	makeHash(key,first,TableLength);	//ハッシュ値の生成
	UINT64 PreviousShift = HashTable[first].previous;
	if( PreviousShift != 0 ){
		//NotFound(該当データ無し)
		result = false; return;
	}

	{
		UINT64 NextShift;
		UINT64 TagetPlace = first;

		do{
			if( HashTable[ TagetPlace ].key == key ){//keyの一致を確認
				//keyが一致。
//				value  = HashTable[ TagetPlace ].value;
				result = true; return;
			}
			NextShift = HashTable[ TagetPlace ].next;
			TagetPlace += NextShift;				//次の要素へ移動する量を計算。
		}while( NextShift != 0 );					//チェーンの末尾に到達したかどうか。
		//チェーンの末尾に到達。

		result = false; return;//最後まで探しても無かったので、[false]
	}
}


void ICHashT::eraseX( char set_function_type, UINT64& key, bool& result, void*& return_value_p ){
	//要素がチェーンされていない場合(要素数1)、あるいは、チェーンの一番後ろの要素を削除する場合。  →  そのまま削除するだけ。
	//チェーンの一番後ろ以外の要素を削除する場合。  →  一番後ろの要素を、本来削除したかった要素へ上書きする。その上で、一番後ろの要素を、削除する。

	//テーブル自体の整理整頓はsetX()関数に一任することにするので、erase()関数においては、これ以上の処理の実装は行わない。(root値が設定されている場合を除く。root_solver();)

	UINT64 first;				//チェーンの先頭head
	UINT64  last;				//チェーンの末尾tail
	
	UINT64 delete_object;		//削除対象
	
//	unsigned char ChainLength = 1;		//チェーンの長さ


	//keyの検索
	makeHash(key,first,TableLength);//ハッシュ値の生成


	//keyが絶対に存在しない場合は [return false;]
	if( HashTable[first].previous!=0 ){
		// [previous==255] :: 配列が空(no data)
		// [previous!=  0] :: データはあるが、チェーンの先頭ではない。
		// [previous==  0] :: チェーンの先頭。
		result = false; return;
	}
	// HashTable[first] が、長さ1以上のチェーンの先頭である事を確認。


	//削除対象の要素までチェーンの検索を進める。
	delete_object = first;	//初期化
	while( HashTable[delete_object].key != key ){ //keyが一致するまでループを回し、削除対象を検索する。
		UINT64 NextShift = HashTable[delete_object].next;
		if( NextShift == 0 ){
			result = false; return;//最後まで一致するkeyが存在しなかったので、 [return false;]
		}
//		ChainLength++;
		delete_object += NextShift;
	}
	

	// [チェーンの長さの計測] 、及び、[チェーンの最後] と [チェーンの最後から二番目] の位置を確認。
	last = delete_object;	//初期化
	while(1){ //チェーンの最後までループを回す。
		UINT64 NextShift = HashTable[last].next;
		if( NextShift == 0 ){
			break;
		}
//		ChainLength++;
		last += NextShift;
	}
	//ここまでで、処理に必要なデータ収集終了。下準備が完了。


	HashStruct HashStruct_for_init;
	HashStruct_for_init.key      = 0;
	HashStruct_for_init.value    = 0;
	HashStruct_for_init.previous = 255;	//[root==255] 配列が空であることを示す。
	HashStruct_for_init.next     = 0;


	/*
	if( last == first ){ //削除対象が配列の一番最後かどうかの確認。
		//要素数1の場合。(配列の先頭が、配列の最後)
		//[処理番号①]
		HashTable[last] = HashStruct_for_init;

		NumOfElementsInTheTable--;	//テーブル上に格納されているkey valueペアの数のカウントを減算。
	//	LoadFactor = NumOfElementsInTheTable / TrueTableLength;		//使わない値を毎回計算させるのも……。
		result = true; return;
	}


	//以下、要素数2以上の場合。

	if( last == delete_object ){ //削除対象が配列の一番最後かどうかの確認。
		//削除対象が配列の一番最後。(かつ、少なくとも要素数2以上)
		UINT64 NextShift     = HashTable[last].next;
		UINT64 PreviousShift = HashTable[last].previous;

		//[処理番号②]
		HashTable[last-PreviousShift].next = 0;		//next接続を切断
		HashTable[last] = HashStruct_for_init;		//要素を初期化

		NumOfElementsInTheTable--;	//テーブル上に格納されているkey valueペアの数のカウントを減算。
	//	LoadFactor = NumOfElementsInTheTable / TrueTableLength;		//使わない値を毎回計算させるのも……。
		result = true; return;
	}
	*/

	
	//上記の、[処理番号①]、及び、[処理番号②] の処理も、ここの処理を行えば、同様に処理されてしまう。
	//従って、上記はコメントアウト。(if文と(場合によっては)無駄なメモリコピーと、総合的にどちらが速いのか……。)
	{
		UINT64 PreviousShift = HashTable[last].previous;

		// [処理番号③]、及び、[処理番号④]
		HashTable[last-PreviousShift].next = 0;					//next接続を切断

		HashTable[delete_object].key   = HashTable[last].key;	//データの移動処理
		if( set_function_type == '0' ){
			// set_function_type == '0'
			// 何もしない。
		}else{
			// set_function_type == '1'
			return_value_p = HashTable[delete_object].value;	//メモリの解放用。あるいは、削除と同時に削除先に動的に保存されているデータの取得を行う。
		}
		HashTable[delete_object].value = HashTable[last].value;	//データの移動処理

		HashTable[last] = HashStruct_for_init;		//要素を初期化

		NumOfElementsInTheTable--;	//テーブル上に格納されているkey valueペアの数のカウントを減算。
	//	LoadFactor = NumOfElementsInTheTable / TrueTableLength;		//使わない値を毎回計算させるのも……。
		result = true; return;
	}
}
void ICHashT::erase0( UINT64& key, bool& result ){
	void* return_value_p;	//ダミー(実際には使われない。)
	eraseX( '0', key, result, return_value_p );
}
void ICHashT::erase1( UINT64& key, bool& result, void*& return_value_p ){
	eraseX( '1', key, result, return_value_p );
}


void ICHashT::reHash(){

#ifdef ICHashT_DebagMode
	// デバッグ用の設定
	printf( "■■■reHash■■■\n");	//for debag

	#ifdef _WIN32
		system("pause");					//for debag
	#endif
#else //ICHashT_DebagMode
	// リリース用の設定
#endif//ICHashT_DebagMode

ReReHash:

	PrimeList_Num++;																//一つ上のテーブルサイズを指定する。
//	if(PrimeList_Num>63){return false;}
	UINT64 TableLength_new     = SmallestPrimeList_LargerThanPower2[PrimeList_Num];	//見かけ上のテーブルサイズ
	UINT64 TrueTableLength_new = TableLength_new + AllowedRightShiftNum;

	struct HashStruct* HashTable_new;
	mallocTable(HashTable_new,TrueTableLength_new);	//新しいテーブルの確保、及び初期化

	//全要素について、ハッシュ値を再計算し、新しいテーブルにコピーし、配置し直す。

	//全要素走査
	AllScanPlace NowPlace;
	for(AllScanInit(NowPlace); NowPlace < AllScanEnd; AllScanNextShift(NowPlace)){
		UINT64 key;
		void* value;
		AllScanGetData(key,value,NowPlace);
//			printf("key::%lld\n"           ,            key );
//			printf("value(Address)::%llX\n",          value );

		//要素を新しい配列へ書き込み
		void** return_value_pp;	//ダミー(実際には使われない。)
		bool   result;
		bool   NeedReHash;
		char   set_function_type = '1';
		setX( set_function_type, key, value, return_value_pp, result, NeedReHash, HashTable_new, TableLength_new, TrueTableLength_new );		//setXwithReHash()関数を使うと、再帰的な処理が発生するので、決して使わない事。
		if( NeedReHash == true ){
			//もしもハッシュ値とkeyが重複し、seX()がfalseを返すような場合があれば、無限にリハッシュしようとするので、注意。

			//setX()が更なるリハッシュを要求してきた場合
			//→現在新しく確保したテーブルへのコピーを中止し、さらに倍サイズのテーブルへのコピーへと操作を変更する。

			free( HashTable_new );			//テーブルの解放
			goto ReReHash;					//gotoで書いた方が、比較が一回少ないので。do{}while(result==false);はやめる。
		}
	}

	freeTable(HashTable);					//古いテーブルの解放
	HashTable       = HashTable_new;		//入れ替え
	TableLength     = TableLength_new;		//入れ替え
	TrueTableLength = TrueTableLength_new;	//入れ替え
}


void ICHashT::reHash_to_fit(){

	//座席占有率(充填率)の計算。
//	LoadFactor = NumOfElementsInTheTable / TrueTableLength;

	//理想とするテーブルサイズを計算。
		//    [理想テーブル占有率] == [実際にテーブル上に存在する key value ペアの要素数] / [理想テーブル長]
		// ⇔   IdealLoadFactor    ==               NumOfElementsInTheTable               / IdealTableLength
		// ⇔   IdealTableLength   ==               NumOfElementsInTheTable               / IdealLoadFactor
		//より、
	double IdealLoadFactor = 0.5;	//理想テーブル占有率 ( 取りあえず50%に設定しておく。 )
	UINT64 IdealTableLength = (UINT64)( ((double)NumOfElementsInTheTable) / IdealLoadFactor );

	//[理想テーブル占有率]が50%以下となり、かつ、最も50%に近いテーブルサイズを、素数表の中から選ぶ。
	unsigned char PrimeList_Num_New = 0;															//reHash()でアクセスされるので、必ず初期化しておく。
	for(; PrimeList_Num_New<64; PrimeList_Num_New++){ //32bit対応するのであれば、注意した方が良いのかな?(素数表も32bitならサイズを小さくした方が良い。)
		if( SmallestPrimeList_LargerThanPower2[PrimeList_Num_New] >= IdealTableLength ){
			break;		//for文を抜ける。
		}
	}

	if( PrimeList_Num_New == PrimeList_Num ){
		//現状のテーブルサイズは理想的なサイズなため、reHash();関数を実行する必要は無い。従って、リハッシュせずにreturn。
		return;
	}else{
		PrimeList_Num = PrimeList_Num_New;	//理想とするテーブルサイズを指定する。
	}

	ReconstrucTheTable();	//強制リハッシュ
}


void ICHashT::Reconstruction_to_fit(){

	//座席利用率の計算。
//	LoadFactor = NumOfElementsInTheTable / TrueTableLength;

	//理想とするテーブルサイズを計算。
		//    [理想座席利用率] == [実際にテーブル上に存在する key value ペアの要素数] / [理想テーブル長]
		// ⇔ IdealLoadFactor  ==               NumOfElementsInTheTable               / IdealTableLength
		// ⇔ IdealTableLength ==               NumOfElementsInTheTable               / IdealLoadFactor
		//より、
	double IdealLoadFactor = 0.5;	//理想座席利用率 ( 取りあえず50%に設定しておく。 )
	UINT64 IdealTableLength = (UINT64)( ((double)NumOfElementsInTheTable) / IdealLoadFactor );

	//[理想座席利用率]が50%以下となり、かつ、最も50%に近いテーブルサイズを、素数表の中から選ぶ。
	unsigned char PrimeList_Num_New = 0;															//reHash()でアクセスされるので、必ず初期化しておく。
	for(; PrimeList_Num_New<64; PrimeList_Num_New++){ //32bit対応するのであれば、注意した方が良いのかな?(素数表も32bitならサイズを小さくした方が良い。)
		if( SmallestPrimeList_LargerThanPower2[PrimeList_Num_New] >= IdealTableLength ){
			break;		//for文を抜ける。
		}
	}

	PrimeList_Num = PrimeList_Num_New;	//理想とするテーブルサイズを指定する。

	ReconstrucTheTable();	//強制リハッシュ
}


void ICHashT::ReconstrucTheTable(){
	PrimeList_Num--;	//reHash();関数は、一つ上のテーブルサイズを指定しようとするので、先に一つ引いておく。
	reHash();			//リハッシュ
}



#ifdef ICHashT_OperationalDebagMode
	// 運用上のデバッグモードの設定
UINT64 ICHashT::GetTableLength(){
	return TableLength;
}
UINT64 ICHashT::GetTrueTableLength(){
	return TrueTableLength;
}
#else //ICHashT_OperationalDebagMode
	// 運用上のリリースモードの設定
#endif//ICHashT_OperationalDebagMode


//=====================================================================================================================================================

//keyに文字列など様々な型を使いたいなら、まずは比較部分を切り離した方がいいかな??

//bool CompareKEY( TableNum, KeyType key ){
//	if( HashTable[TableNum] = key ){
//		return true;
//	}
//	return false;
//}


