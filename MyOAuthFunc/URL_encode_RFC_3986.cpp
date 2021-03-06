#include "URL_encode_RFC_3986.h"


//URLエンコードする関数
std::string URL_encode_RFC_3986( const std::string &str ){
	// str    : URLencodeする対象の文字列
	//
	// return : URLencode処理した文字列を返す
	
	// 変換表ではASCIIコードしか使われていないので、ファイルの保存形式によって、出力が変化する恐れは無い。
	// ただし、入力される文字列が、外部ファイルに静的に埋め込まれている場合は、当然保存形式に左右される。
	
	//変換テーブル :: 入力された数値をその数値の文字列に変換する。 ( 例 :: '0xE3' → "E3" )
	char URL_encode_RFC_3986_Table[][3] = { // 3 なのは、終端文字'\0'が挿入されるため。(どうしても入れないと上手く行かなかった。)
		"00","01","02","03","04","05","06","07","08","09","0A","0B","0C","0D","0E","0F",
		"10","11","12","13","14","15","16","17","18","19","1A","1B","1C","1D","1E","1F",
		"20","21","22","23","24","25","26","27","28","29","2A","2B","2C","2D","2E","2F",
		"30","31","32","33","34","35","36","37","38","39","3A","3B","3C","3D","3E","3F",
		"40","41","42","43","44","45","46","47","48","49","4A","4B","4C","4D","4E","4F",
		"50","51","52","53","54","55","56","57","58","59","5A","5B","5C","5D","5E","5F",
		"60","61","62","63","64","65","66","67","68","69","6A","6B","6C","6D","6E","6F",
		"70","71","72","73","74","75","76","77","78","79","7A","7B","7C","7D","7E","7F",
		"80","81","82","83","84","85","86","87","88","89","8A","8B","8C","8D","8E","8F",
		"90","91","92","93","94","95","96","97","98","99","9A","9B","9C","9D","9E","9F",
		"A0","A1","A2","A3","A4","A5","A6","A7","A8","A9","AA","AB","AC","AD","AE","AF",
		"B0","B1","B2","B3","B4","B5","B6","B7","B8","B9","BA","BB","BC","BD","BE","BF",
		"C0","C1","C2","C3","C4","C5","C6","C7","C8","C9","CA","CB","CC","CD","CE","CF",
		"D0","D1","D2","D3","D4","D5","D6","D7","D8","D9","DA","DB","DC","DD","DE","DF",
		"E0","E1","E2","E3","E4","E5","E6","E7","E8","E9","EA","EB","EC","ED","EE","EF",
		"F0","F1","F2","F3","F4","F5","F6","F7","F8","F9","FA","FB","FC","FD","FE","FF"
	};
	
	//[sprintf();関数]は、入力が、'0xE3'の場合、"FFFFFFE3"を出力する部分について、この挙動は非常に扱いにくいので、変換テーブルを用いる。
	//なお、入力が正の整数??の場合は、例えば、入力'0x25'に対して、"25"が出力される。

	std::string str_re;

	for(int i = 0;
			i < str.size();	//最後まで読み込む。
			i++
	){

		if(	str[i] == '-' ||
			str[i] == '.' ||
			('0' <= str[i] && str[i] <= '9') ||
			('A' <= str[i] && str[i] <= 'Z') ||
			('a' <= str[i] && str[i] <= 'z') ||
			str[i] == '_' ||
			str[i] == '~'
		){
			//ASCII Code 順に、
			//'-'ハイフン、'.'ピリオド、数字、英語大文字、英語小文字、'_'アンダーバー、'~'チルダ
			//は、そのまま。

			str_re += str[i];

		}else{

			//それ以外は%Codingする。

			str_re += '%';
			str_re += URL_encode_RFC_3986_Table[ (unsigned char)str[i] ];	// ここは必ずunsignedにキャストしないと、負の値でバグる。
		}
	}

	return str_re;
}
