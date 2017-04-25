//Gmail側の設定
//
//歯車マーク
//→設定
//→アカウントとインポート
//→アカウント設定を変更
//→その他の Google アカウントの設定
//→ログインとセキュリティ
//→接続済みのアプリとサイト
//→安全性の低いアプリの許可:無効
//を、
//→安全性の低いアプリの許可:有効
//に変更する。

#ifndef __INCLUDE_GUARD__SendGmail
	//既に構造体が定義されている場合は、俗称インクルードガードによって、再定義を回避する。
	#define __INCLUDE_GUARD__SendGmail	//[__INCLUDE_GUARD__]を定義し、次回以降読み込まないようにする。

#include "Make_Connection.h"
#include "../MyOAuthFunc/Base64_encode.h"

struct Mail_struct{
	std::string  user;			//Gmailのuser名
	std::string  pass;			//Gmailのpassword
	std::string  MAIL_FROM;		//メールの送信元。[user]より自動生成
	std::string  RCPT_TO;		//メールの送信先
	std::string  Subject;		//メールの件名										//構造体のポインタになっていない部分は、基本的に変更しないので、このまま。マルチ言語対応する場合は、それもポインタにしてしまうか、或いは言語ごとに構造体の実体を用意する。
	std::string *DATA;			//メールの本文(今回はHTMLmailなので、HTMLを記述)		//メモリコピーが大量に発生するため、ポインタ渡しにする。(ポインタの受け渡しすら無くそうと思うと、[struct Mail_struct]構造体における、[std::string DATA]定義がより複雑になり(std::vector<std::string>とか??)手におえないので廃案)
};

int SendHTMLmailByGmail( Mail_struct &Mail );

#endif//__INCLUDE_GUARD__SendGmail


