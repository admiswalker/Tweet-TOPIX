#include "KeyInterrupter.h"


//コンストラクタ
KeyInterrupter::KeyInterrupter(){
	this->Initialize();
}
//ディストラクタ
KeyInterrupter::~KeyInterrupter(){
	this->Finalize();
}


void KeyInterrupter::Initialize(){
	this->InterruptFlag=false;
	this->FinalizeFlag=false;
	
	pthread_mutex_init( &this->InterruptMutex, NULL );	//InterruptMutexの作成
	pthread_mutex_init( &this-> FinalizeMutex, NULL );	// FinalizeMutexの作成
	
	//キー入力イベント取得用のスレッドを起動する。
	pthread_create(&this->pthread,NULL,KeyInterrupter::thread,this);
	
	return;
}


void KeyInterrupter::Finalize(){
	pthread_mutex_lock( &this->FinalizeMutex );		//FinalizeMutexが返却されるまで待つ。返却されたら、FinalizeMutexを取得して他のスレッドをブロック。
	this->FinalizeFlag=true;
	pthread_mutex_unlock( &this->FinalizeMutex );	//FinalizeMutexを返却
	
	pthread_join(this->pthread,NULL);	//スレッドの終了まで待機
	
	pthread_mutex_destroy(&this->InterruptMutex);
	pthread_mutex_destroy(&this-> FinalizeMutex);
	return;
}

void* KeyInterrupter::thread( void* aParam ){
	
	int ctrl_state=0;	//0:released,1:pressed
	
	class KeyInterrupter *pKI = (KeyInterrupter*)aParam;//スレッド内部では[this->]できないので、受け取る必要がある。
	
	struct input_event event;	//一つずつしか処理するつもりがないので、1バッファ分しか確保していないが、増やしたほうが効率的かもしれない。
	int key_event = open("/dev/input/event2", O_RDONLY ); if(key_event<0){printf("KeyInterrupter_open_error\n");exit(EXIT_FAILURE);}
	
	while(1){
		
		if(pthread_mutex_trylock(&pKI->FinalizeMutex)!=EBUSY){//FinalizeMutexが返却されていれば、処理を実行。それ以外の場合は、諦める。
			if(pKI->FinalizeFlag==true){
				pthread_mutex_unlock( &pKI->FinalizeMutex );	//FinalizeMutexを返却
				pthread_exit(NULL);//スレッドの終了
			}
			pthread_mutex_unlock( &pKI->FinalizeMutex );		//FinalizeMutexを返却
		}
		
		{
			int length;
			do{
				length = read(key_event, &event, sizeof(event));
//				usleep(100*1000);
//				printf("here\n");//これより、readがブロッキング読み込みであることがわかったので、上記のsleep()は不要。//いや、これだとディストラクタが呼び出されてもキー入力がない限りスレッドが終了しないので、だめ。やはり、タイムアウトを設定してやる必要がある。
			}while(length<=0);
		}
		
		switch(event.type){
			case EV_KEY:
				switch(event.code){
		//			case KEY_RIGHTCTRL:	//右Ctrlキー
		//			case KEY_RIGHTSHIFT://右Shiftキー
		//			case KEY_RIGHTALT:	//右Altキー

		//			case KEY_LEFTCTRL:	//左Ctrlキー
		//			case KEY_LEFTSHIFT:	//左Shiftキー
		//			case KEY_LEFTALT:	//左Altキー

					case KEY_LEFTCTRL:	//左Ctrlキー
						if(event.value>0){
							//key down(push)(press)
							ctrl_state=1;
						}else{
							//key up(release)
							ctrl_state=0;
						}
						break;
					case KEY_Q:
						if(ctrl_state){
							//[Ctrl+Q]の検出
							pthread_mutex_lock( &pKI->InterruptMutex );		//InterruptMutexが返却されるまで待つ。返却されたら、InterruptMutexを取得して他のスレッドをブロック。
							pKI->InterruptFlag=true;				//処理
							pthread_mutex_unlock( &pKI->InterruptMutex );	//InterruptMutexを返却
						}
						break;
					default:
						break;
				}
				break;
			default:
				break;
		}
	}
	
	return 0;
}


//外部から割り込みの有無を確認する。
bool KeyInterrupter::ReadInterrupt(){
	if(pthread_mutex_trylock(&this->InterruptMutex)!=EBUSY){//InterruptMutexが返却されていれば、処理を実行。それ以外の場合は、諦める。
		bool buf = this->InterruptFlag;
		InterruptFlag = false;
		pthread_mutex_unlock( &this->InterruptMutex );	//InterruptMutexを返却
		return buf;
	}
	return false;
}


