#include "Generate_oauth_nonce.h"

std::string Generate_oauth_nonce(){
	//あらゆる状況に置いて、ユニークな文字列を生成できるように、
	//[MAC_Adress] + [unixtimeで、1usec単位の現在時刻]
	//を繋ぎ合わせた文字列を生成する。
	//
	//「#include "./My_function/str_sprintf.h"」が必要。
	
	std::string result;
	
	//[MAC_Address]の取得
//	int fd = socket(AF_INET, SOCK_STREAM, 0);	//SOCK_STREAMでも、SOCK_DGRAMでも、どちらでも良い。
	int fd = socket(AF_INET, SOCK_DGRAM , 0);	//
	struct ifreq ifr;
	ifr.ifr_addr.sa_family = AF_INET;
//	memcpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);		//昔は「eth0」だったようだが、現在は「enp0s3」に変わっている。
	memcpy(ifr.ifr_name, "enp0s3", IFNAMSIZ-1);	//(端末で、「ifconfig」でMAC Addressなどを確認できる。その中で、インターフェース名が、enp0s3からとenp0s3表記されるようになった事が確認できる。)
	ioctl(fd, SIOCGIFHWADDR, &ifr);
	close(fd);
	
	//デバッグ用の表示
//	printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
//		(unsigned char)ifr.ifr_hwaddr.sa_data[0],
//		(unsigned char)ifr.ifr_hwaddr.sa_data[1],
//		(unsigned char)ifr.ifr_hwaddr.sa_data[2],
//		(unsigned char)ifr.ifr_hwaddr.sa_data[3],
//		(unsigned char)ifr.ifr_hwaddr.sa_data[4],
//		(unsigned char)ifr.ifr_hwaddr.sa_data[5]);
	
	result += str_sprintf("%02x",(unsigned char)ifr.ifr_hwaddr.sa_data[0]);
	result += str_sprintf("%02x",(unsigned char)ifr.ifr_hwaddr.sa_data[1]);
	result += str_sprintf("%02x",(unsigned char)ifr.ifr_hwaddr.sa_data[2]);
	result += str_sprintf("%02x",(unsigned char)ifr.ifr_hwaddr.sa_data[3]);
	result += str_sprintf("%02x",(unsigned char)ifr.ifr_hwaddr.sa_data[4]);
	result += str_sprintf("%02x",(unsigned char)ifr.ifr_hwaddr.sa_data[5]);
	
	
	//[unixtimeで、1usec単位の現在時刻]の取得
	struct timeval time_micro;
	gettimeofday(&time_micro,NULL);
	
	//デバッグ用の表示
//	printf("%lf\n",(time_micro.tv_sec)+(time_micro.tv_usec*1.0E-6));
//	printf("%.lf\n",(time_micro.tv_sec*1.0E+6)+(time_micro.tv_usec));
//	printf("%X\n",time_micro.tv_sec );	//文字数は無駄に増やしたくないので、16進数で出力する。
//	printf("%X\n",time_micro.tv_usec);	//「%X」を用いて表示させようとする場合、「(time_micro.tv_sec*1.0E+6)+(time_micro.tv_usec)」と、まとめて処理しようとするとバグるので、一つ一つ処理する。
	
	result += str_sprintf("%x",time_micro.tv_sec );
	result += str_sprintf("%x",time_micro.tv_usec);
	
	return result;
}
