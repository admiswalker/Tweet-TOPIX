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



#ifndef __INCLUDE_GUARD__InsertedChainedHashTable
//既に構造体が定義されている場合は、俗称インクルードガードによって、再定義を回避する。
#define __INCLUDE_GUARD__InsertedChainedHashTable	//[__INCLUDE_GUARD__]を定義し、次回以降読み込まないようにする。

#include "../DebagMode.h"

//=====================================================================================================================================================
//InsertedChainedHashTable
//=====================================================================================================================================================
//=====================================================================================================================================================


//#define ICHashT_DebagMode				// この行がコメントアウトされていない場合は、デバッグ用のコードを生成します。
//#define ICHashT_OperationalDebagMode	// この行がコメントアウトされていない場合は、運用上のデバッグ用のコードを生成します。

#ifdef ICHashT_DebagMode
	// デバッグ用の設定
#else //ICHashT_DebagMode
	// リリース用の設定
#endif//ICHashT_DebagMode


#ifdef ICHashT_OperationalDebagMode
	// 運用上のデバッグモードの設定
#else //ICHashT_OperationalDebagMode
	// 運用上のリリースモードの設定
#endif//ICHashT_OperationalDebagMode


#include <functional>	//[std::hash]に必要


#ifdef _WIN32
	typedef unsigned __int64 UINT64;
#else
	typedef unsigned long long UINT64;
#endif

//typedef UINT64 KeyType;	//ユーザーが適当にint型とかchar型を使う事を防ぐため、別名を定義しておく。

typedef UINT64 AllScanPlace;	//ユーザーが適当にint型とかchar型を使う事を防ぐため、別名を定義しておく。
#ifdef ICHashT_DebagMode
	// デバッグ用の設定
typedef UINT64 DumpAll_Place;	//ユーザーが適当にint型とかchar型を使う事を防ぐため、別名を定義しておく。
#endif//ICHashT_DebagMode


struct HashStruct{
	UINT64 key;					// key
	void* value;				// value
	unsigned char previous;		// 前の要素へのシフト数。チェーンの先頭の場合は[ previous ==   0 ]。また、[ previous == 255 ]とすることで、要素が空であることを示す。
	unsigned char next;			// 次の要素へのシフト数。
											// いずれも、右シフトしか許可されていない。
											// シフト0で自分自身を示す。
											// unsigned char型で定義しているので、最大で255回の右シフトが可能。
											// シフト許可幅を大きく取れば取るほどデータの挿入は容易になるが、
											// シフト許可幅を大きく取れば取るほどキャッシュミスによって速度が低下する可能性が大きくなる。
};


typedef class InsertedChainedHashTable{
private:
	struct HashStruct* HashTable;		//mallocで確保されたハッシュテーブル本体のアドレスを記録する。

	UINT64 NumOfElementsInTheTable;	//テーブル上に格納されているkey valueペアの数。(テーブルの充填率を計算する際に用いる。)

	UINT64 TableLength;			//見かけ上のテーブルサイズ
	UINT64 AllowedSumOfShift;	//チェーンの合計シフト数制限。
	unsigned char AllowedRightShiftNum;	//右シフトを許可する最大値の指定。(0～254まで。)
										//このサイズ分だけ、ハッシュ値重複に備えて、隠しテーブルを確保しておく。(シフトは右にしか許可していないため、テーブルの一番最後のハッシュ値が重複した場合に要素を保存する先が無いため。)
										//[ previous == 255 ]は、配列が空である事を表す予約済みの数値。そのため、シフト数は[0～254]までに制限される。
										//[ next     == 255 ]は、previousが0～254までに制限されている以上、使われない。
	UINT64 TrueTableLength;	//[ TableLength + AllowedRightShiftNum ]となる。

	double LoadFactor;	//テーブルの使用率、負荷係数、(座席利用率)
						//[ LoadFactor = NumOfElementsInTheTable / TrueTableLength; ]で計算される。
						//(今の所、)毎回使用する訳ではないので、常に最新の値が記録されている訳では無い。(必要な場合の直前でのみ計算。)

	unsigned char PrimeList_Num;		//テーブルサイズが、配列「SmallestPrimeList_LargerThanPower2」の何番目の要素かを記録する。「SmallestPrimeList_LargerThanPower2」の要素数が64なので、PrimeList_Numは、0～63まで。

	void reHash();						// リハッシュ //ハッシュテーブルの再構成。テーブルサイズをおよそ2倍に拡大して再構成する。(「およそ」なのは、テーブルサイズを素数にする関係上。)(2倍以上でも良いが、2倍以下だと、一要素ごとの演算量に換算しても定数時間で終わらなくなるのでマズイ。効率は一度に拡大するサイズが大きければ大きいほど向上する。(ただし、確保された要素が使われないのであれば無駄なので、やはり2倍ずつ拡大する事にする。))

	void setX          ( char& set_function_type, UINT64& key, void*& value, void**& return_value_pp, bool& result, bool& NeedReHash, struct HashStruct*& HashTable_setX, UINT64& TableLength_setX, UINT64& TrueTableLength_setX );	// [set0]～[set2]関数を[set_function_type]によって切り替える。(別々に書くと、バグ発生時や、機能変更を行う場合に、非常に手間(管理コスト)が増えるので。)
	void setXwithReHash( char  set_function_type, UINT64& key, void*& value, void**& return_value_pp, bool& result,                   struct HashStruct*& HashTable_setX, UINT64& TableLength_setX, UINT64& TrueTableLength_setX );	// setX();関数にリハッシュ機能を付けたもの。(再帰処理を回避するため。)

	void getX          ( char  set_function_type, UINT64& key, void*& value, void**& pp_value,        bool& result );

	void eraseX        ( char  set_function_type, UINT64& key, bool& result, void*&  return_value_p );

//	bool CompareKEY( KeyType key );	//keyの比較部分のみ関数化。(keyのタイプを変更する場合に、迅速に変更できるように、処理をまとめる。)
									//ちゃんと書けば、keyの型の変更が、この関数と、[typedef UINT64 KeyType;]の変更のみで可能となる。はず。
									//inline展開だと、場合によって展開されないので、#defineマクロの方がいいのか???
public:
	InsertedChainedHashTable();								//この時点では、「typedef」されておらず、「ICHashT」は不可なので注意。
	InsertedChainedHashTable( UINT64 TableLength_set );		//オーバーロード(多重定義)される。
	~InsertedChainedHashTable();							//この時点では、「typedef」されておらず、「ICHashT」は不可なので注意。

	//全要素走査用の変数・関数
	AllScanPlace AllScanEnd;
	void AllScanInit     (                            AllScanPlace& AllScanNowPlace );	// AllScanNowPlaceを最初の読み込み場所に初期化する。
	void AllScanNextShift(                            AllScanPlace& AllScanNowPlace );	// AllScanNowPlaceを次の読み込み場所に初期化する。
	void AllScanGetData  ( UINT64& key, void*& value, AllScanPlace& AllScanNowPlace );	// 現在の読み込み位置AllScanNowPlaceにあるデータを読み込む。
		//[AllScanPlace変数]を構造体にして、key valueを持たせれば、イテレータを使った場合のように、[AllScanGetData();関数]を、[AllScanNextShift();関数]内に埋め込み、行数を減らすこともできるが、その場合、実装によっては、用意したその構造体から、さらに、目的の変数や構造体へデータをコピーする必要が生じるので、やめる。

#ifdef ICHashT_DebagMode
	// デバッグ用の関数

	//ハッシュテーブル上で空の要素も含めた全ての要素のダンプ
	DumpAll_Place DumpAll_End;
	void DumpAll_for_Debag_Init   (                                                                          DumpAll_Place& DumpAll_NowPlace );	// DumpAll_NowPlaceを開始場所に初期化し、DumpAll_Endを終了場所に設定する。
	void DumpAll_for_Debag_GetData( UINT64& key, void*& value, unsigned char& previous, unsigned char& next, DumpAll_Place& DumpAll_NowPlace );	// 現在DumpAll_NowPlace、で指定されている値の読み込み。
#endif//ICHashT_DebagMode

	void reHash_to_fit();				// テーブルサイズが充填率約50%程度となるようにリハッシュ。(テーブルサイズを縮小させる事もできる。)
										// ただし、現状のテーブルサイズが、充填率が50%以下で、最も50%に近い(素数表上にあり選択可能な)テーブルサイズの場合は、何もせずにreturn;
										// テーブル上の要素を大幅に削除して、テーブルを縮小したい場合などに用いる。

	void Reconstruction_to_fit();		//理想的なテーブルサイズとなるように、強制的にリハッシュ。

	void ReconstrucTheTable();			// テーブルの再構成。同じサイズのテーブルとしてテーブルを再構成する。
										// テーブルのチェーンの断片化が激しい場合に用いる(ことを想定)。

	void makeHash( UINT64& key, UINT64& return_Hash, UINT64& TableLength_makeHash );
#ifdef ICHashT_DebagMode
	// デバッグ用の関数
	bool Use_define_return_Hash_flag;		//デバッグモード用。このフラグがtrueの場合に限り、makeHash();関数は、[return_Hash]の値として、常に[define_return_Hash]を返す。
	UINT64 define_return_Hash;				//makeHash();関数の返す[return_Hash]の値を指定する。
#endif//ICHashT_DebagMode
	
	void mallocTable( struct HashStruct*& HashTable_p_for_malloc, UINT64& TrueTableLength_mallocTable );
	void freeTable  ( struct HashStruct*& HashTable_p_for_free );

	void set0( UINT64& key, void*& value );												// 最も一般的な動作をする関数
	void set1( UINT64& key, void*& value );												// 特殊関数
	void set2( UINT64& key, void*& value, void**& return_value_pp, bool& result );		// 特殊関数
		//set0():keyが既に格納されている場合は、result==falseで、valueを上書きしてreturn;
		//set1():keyが既に格納されている場合は、result==falseで、何もせずにreturn;
		//set2():keyが既に格納されている場合は、result==falseで、valueを取得してreturn;(valueには手を加えないが、voidポインタなので、中身の判断と書き換えができる。)
		//→いずれも、何事も無ければ、そのまま値を書き込みreturn;
#ifdef ICHashT_DebagMode
	// デバッグ用の設定
	void set_for_Debag( UINT64 TableElementNum, UINT64 key, void* value, unsigned char previous, unsigned char next ); //指定した配列番号に、任意のデータをセットする。
#endif//ICHashT_DebagMode

	void get0( UINT64& key, void* &    value, bool& result );				// 一般的な動作をする関数	// 指定した key に対応する value を取得する。
	void get1( UINT64& key, void**& pp_value, bool& result );				// 特殊関数					// 指定した key に対応する value を取得する。この時、書き換えができるように、ダブルポインタで渡す。

	void find( UINT64& key,                bool& result );					// 指定した key に対応する value が存在するかどうかを確認する。

	void erase0( UINT64& key, bool& result );								// 一般的な動作をする関数	// key 及び value の削除(もしも対象のkeyが発見されなかった場合はfalseを返す。)
	void erase1( UINT64& key, bool& result, void*& return_value_p );		// 特殊関数					// key 及び value の削除(もしも対象のkeyが発見されなかった場合はfalseを返す。)
		// erase0(); :: 指定したkeyの要素を削除するだけ。
		// erase1(); :: 指定したkeyの要素を削除すると同時に、 key とペアになっていた value を取得する。
		//              ( データを削除する際に、valueの先にメモリが動的に確保されており、メモリを解放する必要がある場合、
		//                また、データを取得した後に必ずデータを削除する場合、を想定。                                    )

	void AllClear();	//テーブルを初期化する。(テーブル上のデータは全て削除される。)


#ifdef ICHashT_OperationalDebagMode
	// 運用上のデバッグモードの設定
	UINT64 GetTableLength();
	UINT64 GetTrueTableLength();
#else //ICHashT_OperationalDebagMode
	// 運用上のリリースモードの設定
#endif//ICHashT_OperationalDebagMode



//	void SaveTableToTheFile();		//ハッシュテーブルをバイナリのままファイルに保存する。
//	void ReadTableFromTheFile();	//バイナリのまま保存したハッシュテーブルをファイルから読み込む。
			//いやいや、Saveしたくても、keyは兎も角valueはただのvoid*だから普通には無理。
}ICHashT;
//=====================================================================================================================================================
//=====================================================================================================================================================
//InsertedChainedHashTable
//=====================================================================================================================================================


#endif//__INCLUDE_GUARD__InsertedChainedHashTable
