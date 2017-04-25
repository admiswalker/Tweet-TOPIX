#ifndef __INCLUDE_GUARD__Make_Connection_struct
	//既に構造体が定義されている場合は、俗称インクルードガードによって、再定義を回避する。
	#define __INCLUDE_GUARD__Make_Connection_struct	//[__INCLUDE_GUARD__]を定義し、次回以降読み込まないようにする。


#include <string>
#include <unistd.h>		//close()に必要
#include <netdb.h>		//freeaddrinfo()
#include <malloc.h>		//malloc_usable_size()
#include <openssl/ssl.h>


struct Make_Connection_struct{
		//Socketの通信部分
	int sock;
	std::string host_name_or_address;	//ex:[www.google.co.jp]
	std::string service;					//ex:[http]	//ポート番号を指定する代わり。(httpのポート番号80番をここに指定しても上手く動作する事を確認した。)

		//OpenSSLの部分
	SSL_CTX *ctx;
	SSL     *ssl;
	
	int result_of_thread;		//タイムアウトのためのスレッドのreturnを格納する。

		//送信するメッセージ
	std::string SendMsg;

		//受信したメッセージ
	std::string ReceivedMsg;		//こちらを利用する関数は、たぶん少なくとも現在は使用していないと思われる。
	char*       ch_ReceivedMsg;
	int         read_size;
};


int Make_Connection_Init_and_Open( Make_Connection_struct &MC );
	// Make_Connection_struct &MC : 値の受け渡し用の構造体
	// return                     : エラーが無い場合は、「0」を返す。(エラーがある場合は、0以外の値を返す。)(※エラーが起きた場合に、必ず0を返すとは限らない。)
	//
	//SocketとOpenSSLの初期化を済ませ、
	//[Make_Connection_struct.host_name_or_address]及び、
	//[Make_Connection_struct.service]で、
	//指定されたアドレスとの接続を確立する。
	//[Make_Connection_struct.ReceivedMsg]
	//に、返信を格納する。
	//
	//使用後は、Make_Connection_Close()で、接続を切断する必要がある。

int Make_Connection_Read_Initialize( Make_Connection_struct &MC );
int Make_Connection_Read           ( Make_Connection_struct &MC );
int Make_Connection_Read_Finalize  ( Make_Connection_struct &MC );

int Make_Connection_Write          ( Make_Connection_struct &MC );
	// Make_Connection_struct &MC : 値の受け渡し用の構造体
	// return                     : エラーが無い場合は、「0」を返す。(エラーがある場合は、0以外の値を返す。)(※エラーが起きた場合に、必ず0を返すとは限らない。)
	//
	//[Make_Connection_struct.SendMsg]に設定されたメッセージを、
	//[Make_Connection_struct.host_name_or_address]及び、
	//[Make_Connection_struct.service]で、
	//指定されたアドレスに送信する。

int Make_Connection_Close          ( Make_Connection_struct &MC );
	// Make_Connection_struct &MC : 値の受け渡し用の構造体
	// return                     : エラーが無い場合は、「0」を返す。(エラーがある場合は、0以外の値を返す。)(※エラーが起きた場合に、必ず0を返すとは限らない。)
	//
	//接続を切断する。


#endif//__INCLUDE_GUARD__Make_Connection_struct



