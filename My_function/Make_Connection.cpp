#include "Make_Connection.h"

//[valgrind --leak-check=full --show-leak-kinds=all ./EXEfile]の実行結果より、
//OpenSSLがメモリリークしているので、その対策として(ネット上の誰かの)自作関数を用意する。
//http://ztms.blog.fc2.com/blog-entry-57.html
#include "openssl/conf.h"
#include "openssl/engine.h"
void SSL_library_fin( void ){
	ENGINE_cleanup();
	CONF_modules_unload(1);
	ERR_free_strings();
	CRYPTO_cleanup_all_ex_data();
	EVP_cleanup();
	ERR_remove_state(0);
//	sk_SSL_COMP_free(SSL_COMP_get_compression_methods());

//	sk_SSL_COMP_free(SSL_COMP_get_compression_methods());
//デバッグモードに置いて、これを実行する場合、
//「
//■■■ print sorted data. (  end  ) ■■■
//■■■ Sort. (  end  ) ■■■
//
//■■■ Get HTML code. ( start ) ■■■
//Segmentation fault (コアダンプ)
//[root@localhost TweetTopics_Ver03.09_07]# 
//」
//となる。

////sk_SSL_COMP_free(SSL_COMP_get_compression_methods());だけ実行しなかった場合
//「
//==13293== LEAK SUMMARY:
//==13293==    definitely lost: 0 bytes in 0 blocks
//==13293==    indirectly lost: 0 bytes in 0 blocks
//==13293==      possibly lost: 0 bytes in 0 blocks
//==13293==    still reachable: 912 bytes in 4 blocks	←たしかlevelDBの残骸ではなかったかな??
//==13293==         suppressed: 0 bytes in 0 blocks
//==13293== Reachable blocks (those to which a pointer was found) are not shown.
//==13293== To see them, rerun with: --leak-check=full --show-leak-kinds=all
//」
////sk_SSL_COMP_free();を含め、SSL_library_fin()全て実行した場合(コアダンプするの■■■ Get HTML code. ( start ) ■■■箇所だけコメントアウト)
//「
//==31316== LEAK SUMMARY:
//==31316==    definitely lost: 0 bytes in 0 blocks
//==31316==    indirectly lost: 0 bytes in 0 blocks
//==31316==      possibly lost: 0 bytes in 0 blocks
//==31316==    still reachable: 912 bytes in 4 blocks	←たしかlevelDBの残骸ではなかったかな??
//==31316==         suppressed: 0 bytes in 0 blocks
//==31316== Reachable blocks (those to which a pointer was found) are not shown.
//==31316== To see them, rerun with: --leak-check=full --show-leak-kinds=all
//」
////SSL_library_fin()すべて実行しなかった場合
//「
//==24675== LEAK SUMMARY:
//==24675==    definitely lost: 0 bytes in 0 blocks
//==24675==    indirectly lost: 0 bytes in 0 blocks
//==24675==      possibly lost: 320 bytes in 1 blocks
//==24675==    still reachable: 88,104 bytes in 2,869 blocks
//==24675==         suppressed: 0 bytes in 0 blocks
//==24675== Reachable blocks (those to which a pointer was found) are not shown.
//==24675== To see them, rerun with: --leak-check=full --show-leak-kinds=all
//」
//となり、全て実行しなかった場合は確実にリークが増しているが、それ以外に関しては変化が無い。
//従って、コアダンプするし、sk_SSL_COMP_free(SSL_COMP_get_compression_methods());だけ実行しないでおく。
}
//見事、
//[All heap blocks were freed -- no leaks are possible]
//となり、メモリリークが消えた。ネットの誰か凄い。

int Make_Connection_Init_and_Open( Make_Connection_struct &MC ){
	struct addrinfo addressinfo;				//配列として用意される訳では無いので、freeaddrinfo(addressinfo);の必要は無い。
	
	//addressinfo.ai_flags    = AI_PASSIVE;	//これを指定すると accept(2) するためのソケットを bind(2) するのに適したものとなる。通常サーバ側のプログラムで用いられる。
														//今回はクライアント側なので、コメントアウト。
	
	addressinfo.ai_flags    = 0;				//addressinfo.ai_flags に AI_PASSIVE フラグがセットされていない場合、 返されるソケットアドレスは connect(2), sendto(2), sendmsg(2) での使用に適したものとなる。
	addressinfo.ai_family   = AF_UNSPEC;		//getaddrinfo()にIPv4かIPv6のアドレスのバイナリ値??を変えさせる。
	addressinfo.ai_socktype = SOCK_STREAM;		//トランスポート層??の設定。
														//SOCK_STREAM:恐らくTCP接続の事。
														//SOCK_DGRAM :恐らくUDP
														//他にもいくつか指定できる。詳細は、「$ man socket」に譲る。
	addressinfo.ai_protocol = 0;				//ネットワーク層??の設定。
														//0:getaddrinfo()が任意のプロトコルのソケットアドレスを返す事を許可する。
														//httpsでアクセスする場合など、SSL/TLSなど任意のプロトコルを許可する……みたいなかんじかな?
														//
														//恐らく、IPv4やIPv6などのいずれか。
	
	struct addrinfo *addressinfo_result;	//addressinfoと同一にしても問題無い??→アドレスで指定されていると言う事は、配列として値を返されると言う事。
//	以下の内容は、getaddrinfo()実行後に格納される。
//	addressinfo.ai_addrlen;					//以下のアドレスの長さ。
//	addressinfo.ai_addr;						//アドレスへのポインタ
//	addressinfo.ai_canonname;				//ai_flagsにAI_CANONNAME フラグが含まれている場合、返されるリストの最初の addrinfo 構造体の ai_canonname フィールドはホストの公式な名前を指すように設定される。
//	addressinfo.ai_next;						//リストの次のデータへのアドレス。

	int result = getaddrinfo( (const char*)(MC.host_name_or_address).c_str(), (const char*)(MC.service).c_str(), &addressinfo, &addressinfo_result );
//	int result = getaddrinfo( (const char*)(MC.host_name_or_address).c_str(), (const char*)(MC.service).c_str(), &addressinfo, &&addressinfo );//addressinfoと同一にしても問題無い??(後で確認する。)
	if(result!=0){
		printf("getaddrinfo was failed : %s\n",gai_strerror(result));	//gai_strerror()関数は、数値を可読可能なエラーコードに変換してくれる……はず。
		return -1;
	}
	
	struct addrinfo *addressinfo_result_buf;

	for(addressinfo_result_buf  = addressinfo_result;				//解放する必要があるのでコピーしておく。
	    addressinfo_result_buf != NULL;								//次が無くなるまでループを回す。
	    addressinfo_result_buf  = addressinfo_result_buf->ai_next	//次のアドレスに書き換える。
	){
		//ソケットの生成
		//printf("Generate socket.\n");
		MC.sock = socket(addressinfo_result_buf->ai_family, addressinfo_result_buf->ai_socktype, addressinfo_result_buf->ai_protocol);

		if(MC.sock!=-1){
			//MC.sock!=-1

			//回線の接続
			if(connect(MC.sock, addressinfo_result_buf->ai_addr, addressinfo_result_buf->ai_addrlen)==0){ //connect():戻り値は、成功すると0、失敗すると-1となる。
				//printf("Connection Success\n");
				break;	//接続に成功したので、for文を抜けて次の処理へ移行する。
			}
			printf("connect() failed\n");
			close(MC.sock);	//失敗したのでソケットを閉じる。(socket()関数に失敗した場合は、そもそも閉じるソケットが無いと思われるので、何もしない。)
		}else{
			//MC.sock==-1
			//エラーが発生したため、現在の設定(addressinfo_result_buf)でのsocket()関数を終了する。(forの次の値へ)
			printf("socket() failed\n");
		}
	}

	if(addressinfo_result_buf==NULL){
		//全て失敗した場合。
		printf("All connection failed\n");

		//close(MC.sock);					//全て失敗しているなら、socketはそもそも開いていないハズなので、閉じる必要も無い。
		freeaddrinfo(addressinfo_result);
		return -1;
	}
	freeaddrinfo(addressinfo_result);	//もう必要無いので解放してしまう。
	
	
	//ここからOpenSSL
		//初期化
	SSL_load_error_strings();
	SSL_library_init();

		//暗号化の設定
//	MC.ctx = SSL_CTX_new( SSLv2_client_method());	// SSLver2だけを使用する
//	MC.ctx = SSL_CTX_new( SSLv3_client_method());	// SSLver3だけを使用する
//	MC.ctx = SSL_CTX_new( TLSv1_client_method());	// TLSver1だけを使用する
	MC.ctx = SSL_CTX_new(SSLv23_client_method());	// SSLv2,SSLv3,TLSv1すべてだけを使用する
	if(MC.ctx==NULL){
		ERR_print_errors_fp(stderr);
		return -1;
	}

	MC.ssl = SSL_new( MC.ctx );
	if(MC.ssl==NULL){
		ERR_print_errors_fp(stderr);
		return -1;
	}
	
		//SSL通信の開始
	result = SSL_set_fd( MC.ssl, MC.sock );	//OpenSSLにソケットを渡す。
	if ( result == 0 ){
		ERR_print_errors_fp(stderr);
		return -1;
	}
	
	result = SSL_connect( MC.ssl );
	if ( result == 0 ){
		ERR_print_errors_fp(stderr);
		return -1;
	}
	
	return 0;
}



int Make_Connection_Close( Make_Connection_struct &MC ){
	
		//終了処理
	SSL_shutdown(MC.ssl);	//SSL_connect()の終了
	SSL_free(MC.ssl);			//SSL_new()    で確保した領域を解放する。
	SSL_CTX_free(MC.ctx);	//SSL_CTX_new()で確保した領域を解放する。
	
	SSL_library_fin();
	
		//回線の切断
	close(MC.sock);
	
	return 0;
}



int Make_Connection_Write( Make_Connection_struct &MC ){
//	printf("start_send\n");
	int result = SSL_write( MC.ssl, (MC.SendMsg).c_str(), (int)(MC.SendMsg).length() );			//サーバーへメッセージを転送
	if(result<=0){
		//SSL_write();関数の戻り値は、実際に書き込んだバイト数となる。従って、0バイト以下の場合は失敗している。
		Make_Connection_Close( MC );
		return -1;
	}
//	printf("Send length: %d.\n",result);
//	SSL_write( MC.ssl, (MC.SendMsg).c_str(), (int)strlen((MC.SendMsg).c_str()) );	//サーバーへメッセージを転送	//どちらでも同じ意味。
//	printf("end_send\n");
	
	return 0;
}



//スレッドを毎回生成せず、使い回すようにすれば、スレッド生成分のオーバーヘッドを削減できると考えられるが、実装が手間なので、今回は見送る。暇があったら、後で書き直す。
int Make_Connection_Read_Initialize( Make_Connection_struct &MC ){

	MC.ch_ReceivedMsg = (char*)malloc(1*1024*1024*sizeof(char));	//message_headerを一回で格納できるだけの、十分な大きさのバッファが必要
	
//	MC.ch_ReceivedMsg = (char*)malloc(10*1024*1024*sizeof(char));	//message_headerを一回で格納できるだけの、十分な大きさのバッファが必要
//	MC.ch_ReceivedMsg = (char*)malloc(100*1024*1024*sizeof(char));	//message_headerを一回で格納できるだけの、十分な大きさのバッファが必要
	//恐らくここのメモリの管理をしていないせいで、時々コアダンプするので、応急処置として、メモリの量を10倍にしておく。(100MByte)
	
	return 0;
}
int Make_Connection_Read_Finalize( Make_Connection_struct &MC ){
	free(MC.ch_ReceivedMsg);
	return 0;
}
void* Make_Connection_Read_thread( void *MC_buf ){
	
	struct Make_Connection_struct *MC = (Make_Connection_struct*)MC_buf;		//構造体の名前が長いので、毎回キャストするのは大変だから、ここで変換しておく。
	
	int result;
//	result = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype);
	result = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);			//pthread_cancel();関数からの命令で、直ちにスレッドが(強制)終了されるように設定しておく。
	if(result!=0){
		printf("pthread_setcanceltype(); failed.\n");
		MC->result_of_thread = -1;
		return (void*)&MC->result_of_thread;
	//	pthread_exit();
	//	return -1;		//[void*]型なので、[int*]は良いが、[int]型は無理。
	}
	
		//サーバーからメッセージを受信
//	MC->read_size = SSL_read(MC->ssl, MC->ch_ReceivedMsg, malloc_usable_size(MC->ch_ReceivedMsg)-1);		//以前のバージョンでは、ここでハングアップする事があった。←これは、間違い。「-2」としておかないと、後で、直ぐ下のnull文字で埋める場合にアクセス違反(Segmentation fault (コアダンプ))となる事がある。
	MC->read_size = SSL_read(MC->ssl, MC->ch_ReceivedMsg, malloc_usable_size(MC->ch_ReceivedMsg)-2);		//以前のバージョンでは、ここでハングアップする事があった。
	//NULL文字で埋める分のメモリを考慮して、-2としておく。
		//LANケーブルを抜くと、受信待機状態になるようで、「SSL_read();」関数から制御が返って来なくなる。
		//一旦LANケーブルを引き抜いて、すぐにもう一度挿した場合は、「Read Size :: 0」が返される事がある。
		//時間を置いてから、もう一度LANケーブルを挿した場合は、「SSL_read();」関数から制御が返って来ないままで、
		//「Read Size :: 0」が読み込まれる事も無いので注意が必要である。従って、タイムアウトを設けてエラーとして返す必要がある。
		//
		//→「SSL_read();」関数から制御が返って来なくなると、対処のしようがないので、受信処理を別スレッドで実行し、制御が帰って来ない場合は、スレッドごと強制終了する事にした。
	
	if (MC->read_size <= 0) {		//通常の場合では必ず0以上の値を返す。(恐らく値が返るまで受信待機している。)
		printf("\nSSL_read failed\n");
		MC->result_of_thread = -1;
		return (void*)&MC->result_of_thread;
	//	pthread_exit();
	//	return -1;		//[void*]型なので、[int*]は良いが、[int]型は無理。
	}
//	printf("Read Size :: %d\n",MC.read_size);	//for debag
	MC->ch_ReceivedMsg[MC->read_size] = '\0';//読み込んだデータの末尾にnull文字を追加。
	
	MC->result_of_thread = 0;
	return (void*)&MC->result_of_thread;
//	pthread_exit();
//	return 0;		//[void*]型なので、[int*]は良いが、[int]型は無理。
}
int Make_Connection_Read( struct Make_Connection_struct &MC ){
	
	pthread_t p_thread;
	pthread_create( &p_thread, NULL, Make_Connection_Read_thread, (void*)&MC );
	
	int* thread_return;
	
	struct timespec ts;
	if( clock_gettime(CLOCK_REALTIME, &ts) == -1 ){
		printf("time: %ld.%09ld sec\n", ts.tv_sec, ts.tv_nsec);	//for debag
		printf("clock_gettime() was failed.\n");					//for debag
	}
	ts.tv_sec += 60;	//指定したいタイムアウト時間を加算する。
	int result = pthread_timedjoin_np( p_thread, (void**)&thread_return, &ts );
//	pthread_detach( p_thread );//実験的に全ての場合で入れてみる。(結局returnは常に0のように感じる)←いや、二重にdetuchしようとした場合?などにはエラーを吐く。
	
	if(result!=0){
		pthread_cancel( p_thread );		//スレッドを強制終了
		printf("time out has occurd.\n");
		
		printf("■■■ Dump debag data. ( start ) ■■■\n");
		int result_of_detach = pthread_detach( p_thread );		//このままだと、強制終了したスレッドのメモリが、メモリーリークするので、detachして、システムにメモリーを返す。
		if(result_of_detach==0){
			printf("pthread_detach() was success.\n");
		}else{
			printf("pthread_detach() was failed. Error::%d\n",result_of_detach);
			printf("Serious error!!!\n");
			//exit(1);//プログラムの終了
		}
		printf("■■■ Dump debag data. (  end  ) ■■■\n");
		
		Make_Connection_Read_Finalize( MC );	//free();
		Make_Connection_Close( MC );//切断
		return -1;
	}
	
//	if(MC.result_of_thread!=0){		//このようにしても、動作としては全く同じ。
	if(*thread_return!=0){
	//	printf("SSL_read failed\n");
		
		/*
		//通常こちら側ではスレッドは正常に終了されているので、メモリは解放されているとは思うが、様子を見たいので、入れておく。
		printf("■■■ Dump debag data. ( start ) ■■■\n");
		int result_of_detach = pthread_detach( p_thread );		//このままだと、強制終了したスレッドのメモリが、メモリーリークするので、detachして、システムにメモリーを返す。
		if(result_of_detach==0){
			printf("pthread_detach() was success.\n");
		}else{
			printf("pthread_detach() was failed. Error::%d\n",result_of_detach);
			printf("Serious error!!!\n");
			//exit(1);//プログラムの終了
		}
		printf("■■■ Dump debag data. (  end  ) ■■■\n");
		//*/
		
		Make_Connection_Read_Finalize( MC );	//free();
		Make_Connection_Close( MC );//切断
		return -1;
	}
	
	return 0;
}
