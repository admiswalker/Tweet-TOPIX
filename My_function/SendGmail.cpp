#include "SendGmail.h"


//HTMLmailをGmailで送信する。
int SendHTMLmailByGmail( Mail_struct &Mail ){
	
	struct Make_Connection_struct MC;	//smtp://smtp.gmail.com:587
	MC.host_name_or_address = "smtp.gmail.com";	//argv[1]; //ex:[www.google.co.jp]
	MC.service              = "465";				//SMTP over SSL			//"https";	//argv[2]; //ex:[http]	//ポート番号を指定する代わり。(httpのポート番号80番をここに指定しても上手く動作する事を確認した。)
//	MC.service              = "587";				//TLS:587ポート番号 // 変わりに"TLS"を指定した場合「socket」が失敗した。				//"https";	//argv[2]; //ex:[http]	//ポート番号を指定する代わり。(httpのポート番号80番をここに指定しても上手く動作する事を確認した。)
	
	//Initialize
	int result = Make_Connection_Init_and_Open( MC );
	if( result!=0 ){return -1;}
	
	//他のバージョンでここから送信しているが、間違い。エラー処理をしていないために、上手く動いてしまっているだけ。
//	result = Make_Connection_Write( MC );		//send message
//	if( result!=0 ){return -1;}
	
	Make_Connection_Read_Initialize( MC );
	result = Make_Connection_Read( MC );		//get message header
	if( result!=0 ){return -1;}					//失敗した場合のエラー処理は、Make_Connection_Read();関数内部で行われているので、不要。(Make_Connection_Read_Finalize();関数、及び、Make_Connection_Close();関数を内部で実行している。)
//	printf("%s", MC.ch_ReceivedMsg );			//取得したメッセージを出力
	
	MC.SendMsg  = "EHLO localhost\r\n";
	//MC.SendMsg  = "EHLO\r\n";
//	cout << "Send Data >> " << MC.SendMsg;
	result = Make_Connection_Write( MC );		//send message
	if( result!=0 ){return -1;}
	
	result = Make_Connection_Read( MC );		//get message header
	if( result!=0 ){return -1;}					//失敗した場合のエラー処理は、Make_Connection_Read();関数内部で行われているので、不要。(Make_Connection_Read_Finalize();関数、及び、Make_Connection_Close();関数を内部で実行している。)
//	printf("%s", MC.ch_ReceivedMsg );			//取得したメッセージを出力
	
	MC.SendMsg.clear();							//文字の長さを0にする。(メモリが解放される訳では無い。)
	MC.SendMsg  = "AUTH LOGIN\r\n";
//	cout << "Send Data >> " << MC.SendMsg;
	result = Make_Connection_Write( MC );		//send message
	if( result!=0 ){return -1;}
	
	result = Make_Connection_Read( MC );		//get message header
	if( result!=0 ){return -1;}					//失敗した場合のエラー処理は、Make_Connection_Read();関数内部で行われているので、不要。(Make_Connection_Read_Finalize();関数、及び、Make_Connection_Close();関数を内部で実行している。)
//	printf("%s", MC.ch_ReceivedMsg );			//取得したメッセージを出力
	
	MC.SendMsg.clear();							//文字の長さを0にする。(メモリが解放される訳では無い。)
	MC.SendMsg  = Mail.user;	//Gmail User ID
//	cout << "Send Data >> " << MC.SendMsg << endl;
	MC.SendMsg  = Base64_encode_std_string(MC.SendMsg);
	MC.SendMsg += "\r\n";
//	cout << "Send Data >> " << MC.SendMsg;
	result = Make_Connection_Write( MC );		//send message
	if( result!=0 ){return -1;}
	
	result = Make_Connection_Read( MC );		//get message header
	if( result!=0 ){return -1;}					//失敗した場合のエラー処理は、Make_Connection_Read();関数内部で行われているので、不要。(Make_Connection_Read_Finalize();関数、及び、Make_Connection_Close();関数を内部で実行している。)
//	printf("%s", MC.ch_ReceivedMsg );			//取得したメッセージを出力
	
	MC.SendMsg.clear();								//文字の長さを0にする。(メモリが解放される訳では無い。)
	MC.SendMsg  = Mail.pass;	//Gmail pass
//	cout << "Send Data >> " << MC.SendMsg << endl;
	MC.SendMsg  = Base64_encode_std_string(MC.SendMsg);
	MC.SendMsg += "\r\n";
//	cout << "Send Data >> " << MC.SendMsg;
	result = Make_Connection_Write( MC );		//send message
	if( result!=0 ){return -1;}
	
	result = Make_Connection_Read( MC );		//get message header
	if( result!=0 ){return -1;}					//失敗した場合のエラー処理は、Make_Connection_Read();関数内部で行われているので、不要。(Make_Connection_Read_Finalize();関数、及び、Make_Connection_Close();関数を内部で実行している。)
//	printf("%s", MC.ch_ReceivedMsg );			//取得したメッセージを出力
	
	MC.SendMsg.clear();								//文字の長さを0にする。(メモリが解放される訳では無い。)
	MC.SendMsg  = "MAIL FROM: <";
	MC.SendMsg += Mail.user;
	MC.SendMsg += "@gmail.com>\r\n";
//	cout << "Send Data >> " << MC.SendMsg;
	result = Make_Connection_Write( MC );		//send message
	if( result!=0 ){return -1;}
	
	result = Make_Connection_Read( MC );		//get message header
	if( result!=0 ){return -1;}					//失敗した場合のエラー処理は、Make_Connection_Read();関数内部で行われているので、不要。(Make_Connection_Read_Finalize();関数、及び、Make_Connection_Close();関数を内部で実行している。)
//	printf("%s", MC.ch_ReceivedMsg );			//取得したメッセージを出力
	
	MC.SendMsg.clear();								//文字の長さを0にする。(メモリが解放される訳では無い。)
	//MC.SendMsg  = "RCPT TO: <@>\r\n";
	MC.SendMsg  = "RCPT TO: <";
	MC.SendMsg += Mail.RCPT_TO;
	MC.SendMsg += ">\r\n";
//	cout << "Send Data >> " << MC.SendMsg;
	result = Make_Connection_Write( MC );		//send message
	if( result!=0 ){return -1;}
	
	result = Make_Connection_Read( MC );		//get message header
	if( result!=0 ){return -1;}					//失敗した場合のエラー処理は、Make_Connection_Read();関数内部で行われているので、不要。(Make_Connection_Read_Finalize();関数、及び、Make_Connection_Close();関数を内部で実行している。)
//	printf("%s", MC.ch_ReceivedMsg );			//取得したメッセージを出力
	
	MC.SendMsg.clear();							//文字の長さを0にする。(メモリが解放される訳では無い。)
	MC.SendMsg  = "DATA\r\n";
//	cout << "Send Data >> " << MC.SendMsg;
	result = Make_Connection_Write( MC );		//send message
	if( result!=0 ){return -1;}
	
	result = Make_Connection_Read( MC );		//get message header
	if( result!=0 ){return -1;}					//失敗した場合のエラー処理は、Make_Connection_Read();関数内部で行われているので、不要。(Make_Connection_Read_Finalize();関数、及び、Make_Connection_Close();関数を内部で実行している。)
//	printf("%s", MC.ch_ReceivedMsg );			//取得したメッセージを出力
	
	MC.SendMsg.clear();							//文字の長さを0にする。(メモリが解放される訳では無い。)	
	std::string Subject_str = Mail.Subject;
	//std::string Subject_str = "TESTING_NOW_テスト投稿_＿（全角アンダーバーとか入れてみる。）＿（全角アンダーバーとか入れてみる。）文字数制限を確認する為に、長いタイトルを送信してみる。たしか、約80文字以上に長いタイトルの場合は、文字ごとにまとめて改行を施すなど、多少面倒な操作が必要とされるらしい。まだ長さが足りないので、もう少し文字数を増やして見る。果たしてちゃんと（？）失敗するだろうか？いやいやぁ、まだまだこの程度の長さでは全くもう失敗しないので、もう少しもうだいぶ長くしてみようと思うんだけど、これ本当に失敗するのかな？Gmailが優秀だから失敗しないのか、それとも、そもそも最近の規格では考える必要が無くなったのか、その辺りをはっきりさせたいのだが、そろそろキーボードを叩くのが面倒になってきた。(半角『』)結局失敗しなかった……。もう成功って事で良いよね？??？";	//Gmailの送信箱や受信箱では、おおよそ167文字以上からの表示がバグる。ただし、Blogger側での表示は正常。
	//MC.SendMsg  = "Subject: \r\n";
	//MC.SendMsg  = "Subject: =?ISO-2022-JP?Q?XXXXXXX?=\r\n";
	//MC.SendMsg  = "Subject: =?ISO-2022-JP?B?XXXXXXX?=\r\n";
	//MC.SendMsg  = "Subject: =?UTF-8?Q?";
	MC.SendMsg  = "Subject: =?UTF-8?B?";
	MC.SendMsg += Base64_encode_std_string(Subject_str);
	MC.SendMsg += "?=\r\n";
	
	MC.SendMsg += "Mime-Version: 1.0;\r\n";
//	MC.SendMsg += "Content-Type: text/html; charset=\"ISO-8859-1\";\r\n";
	MC.SendMsg += "Content-Type: text/html; charset=\"UTF-8\";\r\n";
	MC.SendMsg += "Content-Transfer-Encoding: 7bit;\r\n";
	MC.SendMsg += "\r\n";
	MC.SendMsg += (*Mail.DATA);
	MC.SendMsg += "\r\n";
	MC.SendMsg += ".\r\n";
//	cout << "Send Data >> " << MC.SendMsg;
	result = Make_Connection_Write( MC );		//send message
	if( result!=0 ){return -1;}
	
	result = Make_Connection_Read( MC );		//get message header
	if( result!=0 ){return -1;}					//失敗した場合のエラー処理は、Make_Connection_Read();関数内部で行われているので、不要。(Make_Connection_Read_Finalize();関数、及び、Make_Connection_Close();関数を内部で実行している。)
//	printf("%s", MC.ch_ReceivedMsg );			//取得したメッセージを出力
	
	
	MC.SendMsg.clear();								//文字の長さを0にする。(メモリが解放される訳では無い。)
	std::string(MC.SendMsg).swap(MC.SendMsg);		//メモリの内容を書き写して、新たに確保し直す。(不要なメモリだけ解放する事はできないので、作り直す。)
	
	Make_Connection_Read_Finalize( MC );		//受信用バッファの解放
	
	//切断
	Make_Connection_Close( MC );
	
//	printf("end\n");
	return 0;
}

/*
//when test in this source code.

[administrator@localhost SendGmail_Ver00.00_01]$ ./EXEfile
220 mx.google.com ESMTP sf7sm40261535pbc.29 - gsmtp
Send Data >> EHLO
250-mx.google.com at your service, [180.199.154.240]
250-SIZE 35882577
250-8BITMIME
250-AUTH LOGIN PLAIN XOAUTH XOAUTH2 PLAIN-CLIENTTOKEN
250-ENHANCEDSTATUSCODES
250-PIPELINING
250-CHUNKING
250 SMTPUTF8
Send Data >> AUTH LOGIN
334 VXNlcm5hbWU6
Send Data >> [UserName]
Send Data >> [Base64 Encoded UserName]
334 UGFzc3dvcmQ6
Send Data >> [PassWord]
Send Data >> [Base64 Encoded PassWord][Base64 Encoded PassWord]
235 2.7.0 Accepted
Send Data >> MAIL FROM: <[UserName]@gmail.com>
250 2.1.0 OK sf7sm40261535pbc.29 - gsmtp
Send Data >> RCPT TO: <@>
250 2.1.5 OK sf7sm40261535pbc.29 - gsmtp
Send Data >> DATA
354  Go ahead sf7sm40261535pbc.29 - gsmtp
Send Data >> Subject: =?UTF-8?B?VEVTVElOR19OT1df44OG44K544OI5oqV56i/XzLjg5DjgqTjg4jjgrPjg7zjg4njgoLoh6rnlLHoh6rlnKghIQ==?=
(SendData >>)Mime-Version: 1.0;
(SendData >>)Content-Type: text/html; charset="UTF-8";
(SendData >>)Content-Transfer-Encoding: 7bit;
(SendData >>)
(SendData >>)<blockquote class="twitter-tweet">国立国会図書館に行ってみたいな。 <a href="http://t.co/ol6M1dwYMO">http://t.co/ol6M1dwYMO</a><br />? フクロウ風子 (@fukuroufuuko) <a href="https://twitter.com/fukuroufuuko/statuses/388202585036816384">October 10, 2013</a></blockquote><script async="" charset="utf-8" src="//platform.twitter.com/widgets.js"></script>
(SendData >>).
250 2.0.0 OK 1424849114 sf7sm40261535pbc.29 - gsmtp
end
[administrator@localhost SendGmail_Ver00.00_01]$ 
*/


/*
2016_03_30_03_15以降

何故か
	MC.SendMsg  = "EHLO\r\n";
を
	MC.SendMsg  = "EHLO localhost\r\n";
に修正するとうまく動作した。

administrator@ADMIS:~/デスクトップ/Projectsへのリンク/SendGmail_Ver00.00_02$ ./EXEfile
220 smtp.gmail.com ESMTP u64sm1761044pfa.86 - gsmtp
Send Data >> EHLO
501 5.5.4 HELO/EHLO argument  invalid, closing connection. u64sm1761044pfa.86 - gsmtp
Send Data >> AUTH LOGIN
Send Data >> [UserName]
Send Data >> [Base64 Encoded UserName]
Send Data >> [PassWord]
Send Data >> [Base64 Encoded PassWord]
Send Data >> MAIL FROM: <[UserName]@gmail.com>
Send Data >> RCPT TO: <[UserName]@gmail.com>
Send Data >> DATA
Send Data >> Subject: =?UTF-8?B?VEVTVElOR19OT1df44OG44K544OI5oqV56i/XzLjg5DjgqTjg4jjgrPjg7zjg4njgoLoh6rnlLHoh6rlnKghIQ==?=
Mime-Version: 1.0;
Content-Type: text/html; charset="UTF-8";
Content-Transfer-Encoding: 7bit;

<blockquote class="twitter-tweet">国立国会図書館に行ってみたいな。 <a href="http://t.co/ol6M1dwYMO">http://t.co/ol6M1dwYMO</a><br />? フクロウ風子 (@fukuroufuuko) <a href="https://twitter.com/fukuroufuuko/statuses/388202585036816384">October 10, 2013</a></blockquote><script async="" charset="utf-8" src="//platform.twitter.com/widgets.js"></script>
.
end
end
*/
