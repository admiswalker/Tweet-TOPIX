#ifndef __INCLUDE_GUARD__KeyInterrupter
	//既に構造体が定義されている場合は、俗称インクルードガードによって、再定義を回避する。
	#define __INCLUDE_GUARD__KeyInterrupter	//[__INCLUDE_GUARD__]を定義し、次回以降読み込まないようにする。

#include <stdio.h>		//printf for error message
#include <stdlib.h>		//EXIT_FAILURE,exit
#include <linux/input.h>
#include <unistd.h>		//read()
#include <errno.h>		//[EBUSY]の定義
#include <pthread.h>
#include <fcntl.h>		//open,O_RDONLY

//このクラスは、キーデバイスをopenするために管理者権限が必要。したがって、[$ sudo ./EXEfile]のように実行する必要がある。
//
//また、デバイスの場所に関しては、環境によって異なるので、openするファイルを調べて書き換える必要がある。
//調べ方
//方法01.
//	[$ sudo evtest]を実行する。
//	詳細な説明が一緒に出る。
//
//	以下、実行例(最後に選択してエンターで更に詳細)
//
//	$ sudo evtest
//	No device specified, trying to scan all of /dev/input/event*
//	Available devices:
//	/dev/input/event0:	Power Button
//	/dev/input/event1:	Sleep Button
//	/dev/input/event2:	AT Translated Set 2 keyboard
//	/dev/input/event3:	ImExPS/2 Generic Explorer Mouse
//	/dev/input/event5:	VirtualBox mouse integration
//	Select the device event number [0-5]: 
//	
//方法02.
//	[$ cd /]
//	[$ cd ./dev/input/]
//	[$ ls]
//	して、どのようなデバイスがあるかを直接見る。この場合、どのデバイスがどうなっているのかよくわからないので、最悪すべて試してキー入力として受け取れたデバイスが正解。
//
//	以下、実行例
//
//	$ cd /dev/input
//	$ ls
//	by-path  event0  event1  event2  event3  event5  js1  mice  mouse0

//=========================================================================================================================================================================================================================
// [ class KeyInterrupter ] :: begin :: Key入力を受け付けるクラス。プログラムの終了用のクラス。Key入力からのプログラムの中断命令を受け付ける。
//=========================================================================================================================================================================================================================
//=========================================================================================================================================================================================================================

class KeyInterrupter{
	private:
		bool InterruptFlag;				// 割り込みフラグ(外部のメインループで確認するフラグ)	//false:割り込みなし,true:割り込み発生
		pthread_mutex_t InterruptMutex;	// Mutex
		
		pthread_t pthread;
		
		void Initialize();
		static void* thread( void* aParam );	//スレッドとして実行する関数は、恐らく実態が必要なので、staticにしておく。(付けないとエラーを吐く。)ただし、.cppファイルで関数の定義自体をstaticにすると、エラーを吐くのでstaticは付けていないことに注意。

		bool FinalizeFlag;				//終了処理フラグ
		pthread_mutex_t FinalizeMutex;	// Mutex
		void Finalize();
		
	public:
				
		//コンストラクタやディストラクタは、戻り値が無く、void型ですら無いため、returnが使えない。余りにも使いにくいので、今回は、初期化関数と終了関数を用意する事とした。
		//コンストラクタ
		KeyInterrupter();
		//ディストラクタ
		~KeyInterrupter();
		
		//メンバ関数
		bool ReadInterrupt();	//外部から割り込みの有無を確認する。
};

//=========================================================================================================================================================================================================================
//=========================================================================================================================================================================================================================
// [ class KeyInterrupter ] :: end ::
//=========================================================================================================================================================================================================================



//使い方
/*
	class KeyInterrupter* KI = new KeyInterrupter();	//コンストラクタの呼び出し。
	
	while(KI->ReadInterrupt()!=true){			//[Ctrl+Q]が押されるまで、ループを実行する。
		//メインループ
		//キー入力を確認する暇がないほど重い処理
	}
	
	delete KI;
*/

//使い方
/*
	class KeyInterrupter* KI = new KeyInterrupter();	//コンストラクタの呼び出し。
	
	while(1){
		while(KI->ReadInterrupt()!=true){			//[Ctrl+Q]が押されるまで、ループを実行する。
			//メインループ
			//キー入力を確認する暇がないほど重い処理
		}
		{
		Do_you_want_to_exit:
			printf("Do you want to exit ? y/n\n");
			char ch;
			ch = getchar();
			
			if      (ch=='y'){break;
			}else if(ch=='n'){continue;
			}else            {goto Do_you_want_to_exit;
			}
		}
	}
	
	delete KI;
*/

//使い方
/*
	class KeyInterrupter* KI = new KeyInterrupter();	//コンストラクタの呼び出し。
	
	while(1){
		while(KI->ReadInterrupt()!=true){			//[Ctrl+Q]が押されるまで、ループを実行する。
			//メインループ
			//キー入力を確認する暇がないほど重い処理
		}
		
		char ch;
		while(1){
			printf("Do you want to exit ? y/n");
			ch = getchar();
			
			if(ch=='y'||ch=='n'){break;
			}else               {continue;
			}
		}
		if(ch=='y'){break;}
	}
	
	delete KI;
*/



#endif//__INCLUDE_GUARD__KeyInterrupter

