#include <string.h>			//MAC_Addressの取得	//memcpy()
#include <sys/types.h>		//MAC_Addressの取得
#include <sys/socket.h>		//MAC_Addressの取得
#include <sys/ioctl.h>		//MAC_Addressの取得
#include <netinet/in.h>		//MAC_Addressの取得
#include <net/if.h>			//MAC_Addressの取得
#include <unistd.h>			//MAC_Addressの取得	//close()

#include <sys/time.h>		//時刻の取得				//gettimeofday()
#include <sys/resource.h>	//時刻の取得				//gettimeofday()

#include "str_sprintf.h"								//str_sprintf()

std::string Generate_oauth_nonce();
	//あらゆる状況に置いて、ユニークな文字列を生成できるように、
	//[MAC_Adress] + [unixtimeで、1usec単位の現在時刻]
	//を繋ぎ合わせた文字列を生成する。
	//
	//「#include "./My_function/str_sprintf.h"」が必要。
