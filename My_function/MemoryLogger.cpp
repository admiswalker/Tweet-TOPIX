#include "MemoryLogger.h"


//コンストラクタ
MemoryLogger::MemoryLogger(){
	MemoryLogger_Init();
}
//ディストラクタ
MemoryLogger::~MemoryLogger(){}


//実行中のメモリを監視する。メモリの使用量をファイルに書き出し。

int MemoryLogger::MemoryLogger_Init(){
	FileName = str_sprintf("MemoryLog_unixtime_%ld.txt",time(0));		// time(0) : ローカル(PC上で)のunixtimeを取得する。
	std::string buf;
	buf = str_sprintf("ps -p %d u | awk 'NR==1' | tr -d '\r' | tr -d '\n' >> %s;",getpid(),FileName.c_str());{int result_sys = system(buf.c_str());if(result_sys!=0){printf("memory_check.txt was failed. ERROR CODE :: %d\n",result_sys);return 0;}}
	buf = str_sprintf("echo ' Unixtime File Function Line' >> %s;"                                           ,FileName.c_str());{int result_sys = system(buf.c_str());if(result_sys!=0){printf("memory_check.txt was failed. ERROR CODE :: %d\n",result_sys);return 0;}}
	return 0;
}
int MemoryLogger::MemoryLogged_entity(const char *file, const char *function, int line ){
	std::string buf;
	buf = str_sprintf("ps -p %d u | awk 'NR==2' | tr -d '\r' | tr -d '\n' >> %s;",getpid(),FileName.c_str());{int result_sys = system(buf.c_str());if(result_sys!=0){printf("memory_check.txt was failed. ERROR CODE :: %d\n",result_sys);return 0;}}
	buf = str_sprintf("echo -n ' ' >> %s;"                                                ,FileName.c_str());{int result_sys = system(buf.c_str());if(result_sys!=0){printf("memory_check.txt was failed. ERROR CODE :: %d\n",result_sys);return 0;}}

	//ファイル名、関数名、行数の書き込み
	time_t unixtime = time(0);
	buf = str_sprintf("echo \'%ld %s %s %d\' >> %s;"          ,unixtime,file,function,line,FileName.c_str());{int result_sys = system(buf.c_str());if(result_sys!=0){printf("memory_check.txt was failed. ERROR CODE :: %d\n",result_sys);return 0;}}
	
	//コンソールへ出力
	printf("for debag MemoryLogged::unixtime::%ld\n",unixtime);		// time(0) : ローカル(PC上で)のunixtimeを取得する。
	
	return 0;
}


/*
int MemoryLogger::MemoryLogger_Init(){
	FileName = str_sprintf("MemoryLog_unixtime_%ld.txt",time(0));		// time(0) : ローカル(PC上で)のunixtimeを取得する。
	std::string buf;
	buf = str_sprintf("ps -p %d u | awk 'NR==1' | tr -d '\r' | tr -d '\n' >> %s;",getpid(),FileName.c_str());{int result_sys = system(buf.c_str());if(result_sys!=0){printf("memory_check.txt was failed. ERROR CODE :: %d\n",result_sys);return 0;}}
	buf = str_sprintf("echo ' Unixtime File Function Line' >> %s;"                                           ,FileName.c_str());{int result_sys = system(buf.c_str());if(result_sys!=0){printf("memory_check.txt was failed. ERROR CODE :: %d\n",result_sys);return 0;}}
	return 0;
}
int MemoryLogger::MemoryLogged_entity(const char *file, const char *function, int line ){
	std::string buf;
	buf = str_sprintf("ps -p %d u | awk 'NR==2' | tr -d '\r' | tr -d '\n' >> %s;",getpid(),FileName.c_str());{int result_sys = system(buf.c_str());if(result_sys!=0){printf("memory_check.txt was failed. ERROR CODE :: %d\n",result_sys);return 0;}}
	buf = str_sprintf("echo -n ' ' >> %s;"                                                ,FileName.c_str());{int result_sys = system(buf.c_str());if(result_sys!=0){printf("memory_check.txt was failed. ERROR CODE :: %d\n",result_sys);return 0;}}
	buf = "date +%s";//文字列"%s"を、コマンドとして渡したい。([date +%s]で、unixtimeを表示するコマンド)
	buf = str_sprintf("%s | tr -d '\r' | tr -d '\n' >> %s;"                   ,buf.c_str(),FileName.c_str());{int result_sys = system(buf.c_str());if(result_sys!=0){printf("memory_check.txt was failed. ERROR CODE :: %d\n",result_sys);return 0;}}

	//ファイル名、関数名、行数の書き込み
	buf = str_sprintf("echo \' %s %s %d\' >> %s;"                      ,file,function,line,FileName.c_str());{int result_sys = system(buf.c_str());if(result_sys!=0){printf("memory_check.txt was failed. ERROR CODE :: %d\n",result_sys);return 0;}}
	return 0;
}
*/


/*
int MemoryLogger::MemoryLogger_Init(){
	FileName = str_sprintf("MemoryLog_unixtime_%ld.txt",time(0));		// time(0) : ローカル(PC上で)のunixtimeを取得する。
	std::string buf;
	buf = str_sprintf("ps -p %d u | awk 'NR==1' | tr -d '\r' | tr -d '\n' >> %s;",getpid(),FileName.c_str());{int result_sys = system(buf.c_str());if(result_sys!=0){printf("memory_check.txt was failed. ERROR CODE :: %d\n",result_sys);return 0;}}
	buf = str_sprintf("echo ' unixtime' >> %s;"                                           ,FileName.c_str());{int result_sys = system(buf.c_str());if(result_sys!=0){printf("memory_check.txt was failed. ERROR CODE :: %d\n",result_sys);return 0;}}
	return 0;
}
int MemoryLogger::MemoryLogged(){
	std::string buf;
	buf = str_sprintf("ps -p %d u | awk 'NR==2' | tr -d '\r' | tr -d '\n' >> %s;",getpid(),FileName.c_str());{int result_sys = system(buf.c_str());if(result_sys!=0){printf("memory_check.txt was failed. ERROR CODE :: %d\n",result_sys);return 0;}}
	buf = str_sprintf("echo -n ' ' >> %s;"                                                ,FileName.c_str());{int result_sys = system(buf.c_str());if(result_sys!=0){printf("memory_check.txt was failed. ERROR CODE :: %d\n",result_sys);return 0;}}
	buf = "date +%s >> ";
	buf = str_sprintf("%s %s;"                                                ,buf.c_str(),FileName.c_str());{int result_sys = system(buf.c_str());if(result_sys!=0){printf("memory_check.txt was failed. ERROR CODE :: %d\n",result_sys);return 0;}}
	return 0;
}
*/


