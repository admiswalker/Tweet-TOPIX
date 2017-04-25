#include "Filters.h"


//Filters======================================================
//======================================================================

//解析したJSONが、最低限の条件を満たしているかどうかを確認する。
int BasicFilter( struct get_data_from_tweet*& TD ){
	
	//対象のツイートが本人のツイートか、リツイートかによって分岐される。(ただし、99.99%リツイートだと思って良い。)
	if( TD->time_unixtime_re == 0 ){
		//Not retweet.
	//	printf("N");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
	//	fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
		return -1;
	}
	
	//0や1だとロクなツイートでない可能性が高いのでフィルタ
	if(TD->retweet_count<1 || TD->followers_count<1 || TD->friends_count<1 ){
		//Few retweets, followers or friends.
	//	printf("F");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
	//	fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
		return -1;
	}
	
	//公式アカウント(認証済みアカウント)の場合は、ライブだったり、マクドナルドやコンビニの宣伝だったりする事が多いので、除外する。
	if(TD->verified == true){
	//	printf("≡");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
	//	fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
		return -1;
	}
	
	//リツイート数と、お気に入り数に、5倍以上の差がある場合は、スパムと判定する。		
	if( TD->retweet_count <= TD->favorite_count ){
		if( TD->retweet_count*5 <= TD->favorite_count ){
	//		printf("≠");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
	//		fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}else{
		if( TD->retweet_count >= TD->favorite_count*5 ){
	//		printf("≠");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
	//		fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	
	//フォロー数とフォロワ―数に100倍以上の差がある場合は、スパムと判定する。(このフィルタに引っかかるのは、大抵公式認証の無い公式の場合が殆ど。)
	if( TD->friends_count <= TD->followers_count ){
		if( TD->friends_count*100 <= TD->followers_count ){
	//		printf("≠");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
	//		fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}else{
		if( TD->friends_count >= TD->followers_count*100 ){
	//		printf("≠");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
	//		fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	
	return 0;
}

int TimeFilter( struct get_data_from_tweet*& TD, time_t &timep_UTC ){
	
	//大抵拾うのはリツイートなので、こちらのはず。
	double buf_time = (double)(timep_UTC - TD->time_unixtime_re);
	if(buf_time<   22*60){return -1;}	//→そこそこ??	//ゴミが混じるので不可 → 時間を置いて試してみるのも一つの手だとは思う。
//	if(buf_time<   30*60){return -1;}	//→そこそこ??	//ゴミが混じるので不可 → 時間を置いて試してみるのも一つの手だとは思う。
//	if(buf_time< 1*60*60){return -1;}	//→そこそこ??	//ゴミが混じるので不可 → 時間を置いて試してみるのも一つの手だとは思う。
//	if(buf_time<1.5*60*60){return -1;}	//良く分からない。→そこそこ??
//	if(buf_time< 2*60*60){return -1;}	//→そこそこ??
//	if(buf_time< 3*60*60){return -1;}	//→それなりに良い??結果???
//	if(buf_time< 6*60*60){return -1;}	//→それなりに良い結果
//	if(buf_time<12*60*60){return -1;}	//ゴミが混じるので不可 → ある程度時間をを置くと、結果が安定する。(最低一時間)(データを溜める)
//	if(buf_time<24*60*60){return -1;}	//OKだがもう少し短くしたい。	//24時間経過していないツイートの場合(もう少し短くてもいけるかもしれないが、試してはいない。)、ステルスマーケティング的な意味の分からないツイートが大量に溢れ出ていて、(たぶん)分離不能なので、これでフィルタする。

	//リツイート速度の計算
	TD->average_reTweet_per_second = (double)TD->retweet_count / (double)(buf_time);
//	TD->average_reTweet_per_second = ( (double)TD->retweet_count + (double)TD->favorite_count ) / (double)(buf_time);
		//[average_reTweet_per_second]そのものの意味では無くなってしまうので、後で、[average_reTweet_per_second]の名前を変えた方が良い。それこそ何とかefficiencyとか。
	
	
	//リツイート効率の計算
	//calculate efficiency
	if(TD->followers_count!=0){//ゼロ割り防止
		TD->retweet_efficiency = (double)log((double)TD->retweet_count) / ( (double)log(1.1) * (double)TD->followers_count );
//				TD->retweet_efficiency = (double)log((double)TD->retweet_count) / ( (double)log(1.1) * (double)log((double)TD->followers_count) );
	}else{
		TD->retweet_efficiency = 0;
	}
	
	return 0;
}

int TimeFilter_daily( struct get_data_from_tweet*& TD, time_t &timep_UTC ){
	
	//大抵拾うのはリツイートなので、こちらのはず。
	double buf_time = (double)(timep_UTC - TD->time_unixtime_re);
	
	//こちらのファイルタは、TimeFilter();で、既に通っているので、かけない。
//	if(buf_time<   22*60){return -1;}	//→そこそこ??	//ゴミが混じるので不可 → 時間を置いて試してみるのも一つの手だとは思う。
//	if(buf_time<   30*60){return -1;}	//→そこそこ??	//ゴミが混じるので不可 → 時間を置いて試してみるのも一つの手だとは思う。
//	if(buf_time< 1*60*60){return -1;}	//→そこそこ??	//ゴミが混じるので不可 → 時間を置いて試してみるのも一つの手だとは思う。
//	if(buf_time<1.5*60*60){return -1;}	//良く分からない。→そこそこ??
//	if(buf_time< 2*60*60){return -1;}	//→そこそこ??
//	if(buf_time< 3*60*60){return -1;}	//→それなりに良い??結果???
//	if(buf_time< 6*60*60){return -1;}	//→それなりに良い結果
//	if(buf_time<12*60*60){return -1;}	//ゴミが混じるので不可 → ある程度時間をを置くと、結果が安定する。(最低一時間)(データを溜める)
//	if(buf_time<24*60*60){return -1;}	//OKだがもう少し短くしたい。	//24時間経過していないツイートの場合(もう少し短くてもいけるかもしれないが、試してはいない。)、ステルスマーケティング的な意味の分からないツイートが大量に溢れ出ていて、(たぶん)分離不能なので、これでフィルタする。
	
	//24時間までのリツイート数で計算する。
	if(buf_time>24*60*60){return -1;}
	
	return 0;
}

int BlackListFilter_ja( struct get_data_from_tweet*& TD ){
	//Unicode値がハングルの帯域を持っていた場合もフィルタした方が良い。
	
	//以下[ユーザ名]でフィルタする。
	{
		char* result_p = strstr( (char*)TD->name_re.c_str() , "転載禁止" );	//「転載禁止」を検索する。
		if( result_p != NULL ){
			printf("×←n転");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	//以下、[ユーザ名]に「ｱﾄﾞｾﾝｽ」「アドセンス」「あどせんす」「ｸﾘｯｸ」「クリック」「くりっく」などの文字がある場合にフィルタする。
	{
		char* result_p = strstr( (char*)TD->name_re.c_str() , "ｱﾄﾞｾﾝｽ" );	//「ｱﾄﾞｾﾝｽ」を検索する。
		if( result_p != NULL ){
			printf("×←nｱ");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->name_re.c_str() , "アドセンス" );	//「アドセンス」を検索する。
		if( result_p != NULL ){
			printf("×←nア");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->name_re.c_str() , "あどせんす" );	//「あどせんす」を検索する。
		if( result_p != NULL ){
			printf("×←nあ");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->name_re.c_str() , "ｸﾘｯｸ" );	//「ｸﾘｯｸ」を検索する。
		if( result_p != NULL ){
			printf("×←nｸ");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->name_re.c_str() , "クリック" );	//「クリック」を検索する。
		if( result_p != NULL ){
			printf("×←nク");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->name_re.c_str() , "くりっく" );	//「くりっく」を検索する。
		if( result_p != NULL ){
			printf("×←nく");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	//以下、[ユーザ名]に「プレゼント」の文字がある場合にフィルタする。(詐欺の防止と、つまらない方法でRT&フォローを稼ぐツイートの排除が目的。)(物欲しさでRTが上がるだけで、全く面白くない。運営の趣旨に反する。)
	//アディダスのパーカープレゼントの詐欺アカウントが発生したため対策。
	{
		char* result_p = strstr( (char*)TD->name_re.c_str() , "プレゼント" );	//「プレゼント」を検索する。
		if( result_p != NULL ){
			printf("×←nプ");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	
	{
		//ヤバ人動画 @yabazine 3011393696
		//ヤバ人動画 @vineyaba____ 2389367012
		//ヤバ人動画 @Selvinram 43094684
		//ヤバ人動画 @yavajin_douga 2937889951
		//ヤバ人動画 @kei09365018 2587425962
		//ヤバ人動画 @bakusy0u_douga 2576881250
		//ヤバ人動画 @bakusy0u_douga_ 1720966008
		//ヤバイ動画 @douga_yaba 2543800472
		//ヤバ人動画 @vineyaba_ 1960192832
		//ヤバ人動画 @ky3114 2583976549
		//ヤバ人診断 @namaesindan 3176122584
		//ヤバ人画像 @bakusy0u_gazou 1302140077
		//ヤバ人best @yavine_best 3162211736
		//ヤバ人【動画】 @yabagin__douga 2421722539
		//ヤバたん動画 @yabatan_douga 2573386010
		//ヤバ人6秒動画 @yabazin_6douga 62945198
		//ヤバ人6秒動画 @sShinada4 2842261334
		//ヤバ人6秒 @yavagod6 2443635601
		//ヤバ人 @gucchi1119 387424691
		//ヤバ人、大集合！！！ @yabazinatumare 3089912450
		//ヤバイ人動画 @koukou_seijiten 2903597466
		//ヤバい人画像 @yabahito_gazou 3022005944
		char* result_p = strstr( (char*)TD->name_re.c_str() , "ヤバ" );	//「ヤバ」を検索する。
		if( result_p != NULL ){
			printf("×←nヤ");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		//できる男できる女の法則 @dekiruotoko0107 1078082875
		//できる男できる女の法則 @dekiruotoko_m_w 1554968708
		//できる男できる女の法則 @dekiruotokoona 2997010782
		//できる男できる女の法則 @dekiru_danjo0
		//できる男できる女の法則 @dekiru_MtoF 2476864009
		//できる男できる女の法則 @qykaqycusyde 1627458806
		//できる男できる女の法則 @otokoonnadeki
		//出来る男と女の法則 @dekirudannjo 2850202160
		char* result_p = strstr( (char*)TD->name_re.c_str() , "女の法則" );	//「女の法則」を検索する。
		if( result_p != NULL ){
			printf("×←nで");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		//LOVEWALL @GOerav 3084688390
		//LOVEWALL @LSreao 3084714250
		//Lovewall @rfyhthy 3146722162
		//Love Wall @Love_Wall_ 2794039591 ???
		//Love Wall @Love_wall__ ???
		//Love Wall @lovelovelove_wa 2936567028
		//Si Love Wall @Love_Wall_ 2794039591 ???
		//LOVE WALL STREET @SUPERLATIVEAAA 877970005
		char* result_p = strstr( (char*)TD->name_re.c_str() , "LOVE" );	//「LOVE」を検索する。
		if( result_p != NULL ){
			printf("×←nL");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
		
		result_p = strstr( (char*)TD->name_re.c_str() , "Love" );	//「Love」を検索する。
		if( result_p != NULL ){
			printf("×←nl");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		//韓流ツイッター @kor_celebrities 146066313
		//韓流ぴあ @hanryu_pia 182338117
		char* result_p = strstr( (char*)TD->name_re.c_str() , "韓" );	//「韓」を検索する。
		if( result_p != NULL ){
			printf("×←n韓");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		//高校生語辞典 koukosei_ziten 2911793209
		//話題の高校生画像 @koukousei_gazou 3006218827
		//高校生の話題と動画☆ @sugeee_doga 2576777280
		//超スゴイ高校生動画 @chousugoidouga 2548746042
		char* result_p = strstr( (char*)TD->name_re.c_str() , "高校生" );	//「高校生」を検索する。
		if( result_p != NULL ){
			printf("×←n高");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		//学生の本音bot @gakusei1999 3175642105
		char* result_p = strstr( (char*)TD->name_re.c_str() , "学生" );	//「学生」を検索する。
		if( result_p != NULL ){
			printf("×←n学");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		//学校では教わらない無駄知識 @School_te_Saiko 3391349560
		char* result_p = strstr( (char*)TD->name_re.c_str() , "学校" );	//「学校」を検索する。
		if( result_p != NULL ){
			printf("×←n学");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->name_re.c_str() , "動画" );	//「動画」を検索する。
		if( result_p != NULL ){
			printf("×←n動");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		//天下一画像会 @tenkaichi_wwww
		char* result_p = strstr( (char*)TD->name_re.c_str() , "画像" );	//「画像」を検索する。
		if( result_p != NULL ){
			printf("×←n画");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		//話題のツイート @twi_pop_ 877369212
		char* result_p = strstr( (char*)TD->name_re.c_str() , "話題" );	//「話題」を検索する。
		if( result_p != NULL ){
			printf("×←n話");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->name_re.c_str() , "ツイート" );	//「ツイート」を検索する。
		if( result_p != NULL ){
			printf("×←nツ");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		//ツイッターグランプリ @tweet__GP 2587380186
		//ツイッターキングダム @tittercoms 4046721739
		char* result_p = strstr( (char*)TD->name_re.c_str() , "ツイッタ" );	//「ツイッタ」を検索する。
		if( result_p != NULL ){
			printf("×←nツ");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->name_re.c_str() , "公式" );	//「公式」を検索する。
		if( result_p != NULL ){
			printf("×←n公");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->name_re.c_str() , "公式" );	//「公式」を検索する。
		if( result_p != NULL ){
			printf("×←n公");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->name_re.c_str() , "official" );	//「official」を検索する。
		if( result_p != NULL ){
			printf("×←n公");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->name_re.c_str() , "Official" );	//「Official」を検索する。
		if( result_p != NULL ){
			printf("×←n公");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->name_re.c_str() , "OFFICIAL" );	//「OFFICIAL」を検索する。
		if( result_p != NULL ){
			printf("×←n公");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->name_re.c_str() , "オフィシャル" );	//「オフィシャル」を検索する。
		if( result_p != NULL ){
			printf("×←n公");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->name_re.c_str() , "ぶろぐ" );	//「ぶろぐ」を検索する。
		if( result_p != NULL ){
			printf("×←nぶ");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->name_re.c_str() , "ブログ" );	//「ブログ」を検索する。
		if( result_p != NULL ){
			printf("×←nブ");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->name_re.c_str() , "まとめ" );	//「まとめ」を検索する。
		if( result_p != NULL ){
			printf("×←nま");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	
	{
		char* result_p = strstr( (char*)TD->name_re.c_str() , "ニュース" );	//「ニュース」を検索する。
		if( result_p != NULL ){
			printf("×←nN");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->name_re.c_str() , "情報" );	//「情報」を検索する。
		if( result_p != NULL ){
			printf("×←n情");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	
	{
		char* result_p = strstr( (char*)TD->name_re.c_str() , "staff" );	//「staff」を検索する。
		if( result_p != NULL ){
			printf("×←nS");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->name_re.c_str() , "Staff" );	//「Staff」を検索する。
		if( result_p != NULL ){
			printf("×←nS");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->name_re.c_str() , "STAFF" );	//「STAFF」を検索する。
		if( result_p != NULL ){
			printf("×←nS");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	
	{
		char* result_p = strstr( (char*)TD->name_re.c_str() , "tour" );	//「TOUR」を検索する。
		if( result_p != NULL ){
			printf("×←nT");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->name_re.c_str() , "Tour" );	//「TOUR」を検索する。
		if( result_p != NULL ){
			printf("×←nT");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->name_re.c_str() , "TOUR" );	//「TOUR」を検索する。
		if( result_p != NULL ){
			printf("×←nT");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->name_re.c_str() , "編集" );	//「編集」を検索する。
		if( result_p != NULL ){
			printf("×←n編");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	
	/*
	//以下、本文に「http」などの文字がある場合にフィルタする。//引用ツイートのリツイート数を上手くパースできていないため、バグるので、応急処置としてhttpを含むツイートを除外する。
	//今まで問題無かったので、恐らく、twitter社がJSONの並びを変えたと思われる。
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "http" );	//「http」を検索する。
		if( result_p != NULL ){
			printf("×←http");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	*/
	
	//以下、本文に「転載禁止」などの文字がある場合にフィルタする。
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "転載禁止" );	//「転載禁止」を検索する。
		if( result_p != NULL ){
			printf("×←t転");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "韓" );	//「韓」を検索する。
		if( result_p != NULL ){
			printf("×←t韓");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	
	//以下、本文に「ｱﾄﾞｾﾝｽ」「アドセンス」「あどせんす」「ｸﾘｯｸ」「クリック」「くりっく」などの文字がある場合にフィルタする。
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "ｱﾄﾞｾﾝｽ" );	//「ｱﾄﾞｾﾝｽ」を検索する。
		if( result_p != NULL ){
			printf("×←tｱ");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "アドセンス" );	//「アドセンス」を検索する。
		if( result_p != NULL ){
			printf("×←tア");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "あどせんす" );	//「あどせんす」を検索する。
		if( result_p != NULL ){
			printf("×←tあ");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	
	
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "ｸﾘｯｸ" );	//「ｸﾘｯｸ」を検索する。
		if( result_p != NULL ){
			printf("×←tｸ");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "クリック" );	//「クリック」を検索する。
		if( result_p != NULL ){
			printf("×←tク");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "くりっく" );	//「くりっく」を検索する。
		if( result_p != NULL ){
			printf("×←tく");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	
	
	//以下、本文に「プレゼント」の文字がある場合にフィルタする。(詐欺の防止と、つまらない方法でRT&フォローを稼ぐツイートの排除が目的。)(物欲しさでRTが上がるだけで、全く面白くない。運営の趣旨に反する。)
	//アディダスのパーカープレゼントの詐欺アカウントが発生したため対策。
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "プレゼント" );	//「プレゼント」を検索する。
		if( result_p != NULL ){
			printf("×←tプ");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	
	
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "アルバム" );	//「アルバム」を検索する。
		if( result_p != NULL ){
			printf("×←tア");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "album" );	//「album」を検索する。
		if( result_p != NULL ){
			printf("×←tア");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "Album" );	//「Album」を検索する。
		if( result_p != NULL ){
			printf("×←tア");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "ALBUM" );	//「ALBUM」を検索する。
		if( result_p != NULL ){
			printf("×←tア");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "グッズ" );	//「グッズ」を検索する。
		if( result_p != NULL ){
			printf("×←tグ");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "DVD" );	//「グッズ」を検索する。
		if( result_p != NULL ){
			printf("×←tD");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "CD" );	//「CD」を検索する。
		if( result_p != NULL ){
			printf("×←tC");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "おしらせ" );	//「おしらせ」を検索する。
		if( result_p != NULL ){
			printf("×←tC");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "お知らせ" );	//「お知らせ」を検索する。
		if( result_p != NULL ){
			printf("×←tC");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "RT" );	//「RT」を検索する。
		if( result_p != NULL ){
			printf("×←tR");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "いいね" );	//「いいね」を検索する。
		if( result_p != NULL ){
			printf("×←tい");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "動画" );	//「動画」を検索する。
		if( result_p != NULL ){
			printf("×←t動");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "チケット" );	//「チケット」を検索する。
		if( result_p != NULL ){
			printf("×←tチ");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "ticket" );	//「ticket」を検索する。
		if( result_p != NULL ){
			printf("×←tチ");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "Ticket" );	//「Ticket」を検索する。
		if( result_p != NULL ){
			printf("×←tチ");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "TICKET" );	//「TICKET」を検索する。
		if( result_p != NULL ){
			printf("×←tチ");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "更新" );	//「更新」を検索する。
		if( result_p != NULL ){
			printf("×←t更");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "解禁" );	//「解禁」を検索する。
		if( result_p != NULL ){
			printf("×←t解");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "登場" );	//「登場」を検索する。
		if( result_p != NULL ){
			printf("×←t登");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "公演" );	//「グッズ」を検索する。
		if( result_p != NULL ){
			printf("×←t演");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "発売" );	//「グッズ」を検索する。
		if( result_p != NULL ){
			printf("×←t発");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "券売" );	//「券売」を検索する。
		if( result_p != NULL ){
			printf("×←t券");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "拡散" );	//「拡散」を検索する。
		if( result_p != NULL ){
			printf("×←t拡");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "入荷" );	//「入荷」を検索する。
		if( result_p != NULL ){
			printf("×←t荷");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "イベント" );	//「イベント」を検索する。
		if( result_p != NULL ){
			printf("×←tイ");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "情報" );	//「情報」を検索する。
		if( result_p != NULL ){
			printf("×←t情");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "コラボ" );	//「コラボ」を検索する。
		if( result_p != NULL ){
			printf("×←tコ");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "ツアー" );	//「ツアー」を検索する。
		if( result_p != NULL ){
			printf("×←tツ");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "無料" );	//「無料」を検索する。
		if( result_p != NULL ){
			printf("×←t無");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "限定" );	//「限定」を検索する。
		if( result_p != NULL ){
			printf("×←t限");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "プレイ" );	//「プレイ」を検索する。
		if( result_p != NULL ){
			printf("×←tプ");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "実施" );	//「実施」を検索する。
		if( result_p != NULL ){
			printf("×←t実");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "ビジュアル" );	//「ビジュアル」を検索する。
		if( result_p != NULL ){
			printf("×←tビ");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "入場" );	//「入場」を検索する。
		if( result_p != NULL ){
			printf("×←t入");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "開催" );	//「開催」を検索する。
		if( result_p != NULL ){
			printf("×←t催");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "出演" );	//「出演」を検索する。
		if( result_p != NULL ){
			printf("×←t演");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "上映" );	//「上映」を検索する。
		if( result_p != NULL ){
			printf("×←t映");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "ライブ" );	//「ライブ」を検索する。
		if( result_p != NULL ){
			printf("×←tラ");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "爆笑" );	//「爆笑」を検索する。
		if( result_p != NULL ){
			printf("×←t笑");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "応援" );	//「応援」を検索する。
		if( result_p != NULL ){
			printf("×←t応");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	
	//以下[ユーザID]でフィルタする。
	
	//線形検索は処理が重いので、後でlevelDBを用いた実装に書き直す。
	
	//そのツイート主が過去にパクリツイートをした事がある場合は破棄する。(主に手作業でフィルタする)(もしあまりに数が増えてきたら、levelDBか、あるいはハッシュテーブルで実装し直す。)
		//本来リツイートである事を仮定すべきではないが、しかし問題になるのは大抵リツイートなので、リツイートの場合のみ考える。(後で全ての処理をリツイートのみに書き換える。
//アドセンスクリックお願いします (@ohyeahSAKKU_)963510787
	if(TD->user_id==71972515){
		//さたけ?ｱﾄﾞｾﾝｽｸﾘｯｸお願いします @satake_take 71972515 (さたけ? @satake_take)
		printf("×");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
		fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
		return -1;
	}else if(TD->user_id== 479218320){printf("×");fflush( stdout );return -1;		//アドセンスクリックお願い太郎 @Blogath 479218320
	}else if(TD->user_id==  70690974){printf("×");fflush( stdout );return -1;		//ウナム日月@アドセンスクリックをよろしく  ?@unamuhiduki 70690974
	}else if(TD->user_id==  84029166){printf("×");fflush( stdout );return -1;		//アドセンスクリックお願いします (@vipanother) 84029166 (Harukarisk＊Land栃木街道店 @vipanother)
	}else if(TD->user_id==2369195478){printf("×");fflush( stdout );return -1;		//アドセンスクリックお願い (@Qdecim) 2369195478
	}else if(TD->user_id==  68146600){printf("×");fflush( stdout );return -1;		//アドセンスクリックお願いします (@no_esis) 68146600
	}else if(TD->user_id==  84029166){printf("×");fflush( stdout );return -1;		//アドセンスクリックお願いします (@vipanother) 84029166
	
	}else if(TD->user_id==3310890074){printf("×");fflush( stdout );return -1;		//勃起タイムbot ‎@bot_erection 3310890074
//	アダルトまたは成人向けコンテンツを含むページに Google 広告を掲載することは許可されません。これには、以下のような内容を扱うページが含まれますが、これらに限定されません。
//
//		性行為を取り上げた読み物や説明
//		性的なジョーク
//		性的コンテンツを扱うフォーラム
//		URL 内の性的または不適切な語句
//		露骨な文章や過度の不適切な表現
	
	}else if(TD->user_id== 108936279){printf("×");fflush( stdout );return -1;		//細かすぎて伝わらないモノマネ選手権 KomakasugiMovie 108936279	//凍結済みアカウント
	}else if(TD->user_id==3106239673){printf("×");fflush( stdout );return -1;		//捕食ちゃん @eatchaan 3106239673
	}else if(TD->user_id==3230576348){printf("×");fflush( stdout );return -1;		//捕食ちゃん @eatcyan 3230576348								//凍結済みアカウント
	}else if(TD->user_id==3083216233){printf("×");fflush( stdout );return -1;		//捕食ちゃん @ectchaan 3083216233
	}else if(TD->user_id==3098239400){printf("×");fflush( stdout );return -1;		//無罪ちゃん @muzaichan 3098239400							//凍結済みアカウント
	}else if(TD->user_id==3043423430){printf("×");fflush( stdout );return -1;		//無罪ちゃん @muzai_lol 3043423430
	}else if(TD->user_id==3115160520){printf("×");fflush( stdout );return -1;		//無罪ちゃん @muzaiJP 3115160520
	}else if(TD->user_id==3299953994){printf("×");fflush( stdout );return -1;		//無罪ちゃん@ワサラー団四天王 @muzaicold 3299953994			//凍結済みアカウント
	}else if(TD->user_id==4513769712){printf("×");fflush( stdout );return -1;		//無罪ちゃん ‎@Lv631c 4513769712
	}else if(TD->user_id==1961672245){printf("×");fflush( stdout );return -1;		//添い寝くん @soine_lol 1961672245
	}else if(TD->user_id==3254439126){printf("×");fflush( stdout );return -1;		//添い寝くん @soinekunnn 3254439126
	}else if(TD->user_id==2856857599){printf("×");fflush( stdout );return -1;		//添い寝くん @soine_com 2856857599
	}else if(TD->user_id==4496588053){printf("×");fflush( stdout );return -1;		//‍吐息くん @T0lKl 4496588053									//凍結済みアカウント
	}else if(TD->user_id==2793550352){printf("×");fflush( stdout );return -1;		//氷天くん @Lv_400 2793550352								//凍結済みアカウント
	}else if(TD->user_id==2874213086){printf("×");fflush( stdout );return -1;		//捕食ちゃん @eat_lov 2874213086
	}else if(TD->user_id==3254422350){printf("×");fflush( stdout );return -1;		//るいとくん @ruLt0_ 3254422350
	
	}else if(TD->user_id==3280620738){printf("×");fflush( stdout );return -1;		//覗き見ちゃん @NozokiChan_ @EGOlZM 3280620738
	}else if(TD->user_id==2916243560){printf("×");fflush( stdout );return -1;		//せぐめん ?@skdwm 2916243560
	}else if(TD->user_id==3059871890){printf("×");fflush( stdout );return -1;		//め ぐ み ん@育成 @pannero_ 3059871890
	}else if(TD->user_id==2582251818){printf("×");fflush( stdout );return -1;		//月見ちゃん @question0x0 2582251818
	}else if(TD->user_id==3743443460){printf("×");fflush( stdout );return -1;		//クエス @_lquestionl_ 3743443460							//凍結済みアカウント
	}else if(TD->user_id==3798826573){printf("×");fflush( stdout );return -1;		//隙間ちゃん @suklmachan 3798826573							//凍結済みアカウント
	}else if(TD->user_id==2793457398){printf("×");fflush( stdout );return -1;		//添い寝くん ‏@o0osoineo0o 2793457398							//凍結済みアカウント
	}else if(TD->user_id==4636907654){printf("×");fflush( stdout );return -1;		//添い寝くん@サブ垢 @soine_Lov 4636907654						//凍結済みアカウント
	}else if(TD->user_id==2776775580){printf("×");fflush( stdout );return -1;		//号泣くん @cry_syounen 2776775580							//凍結済みアカウント
	}else if(TD->user_id==3709496533){printf("×");fflush( stdout );return -1;		//我は嫌われ者の号泣くん S___he 3709496533					//凍結済みアカウント
	}else if(TD->user_id==2363757674){printf("×");fflush( stdout );return -1;		//崩壊たそ @__y_ku 2363757674
	
	}else if(TD->user_id==2856339026){printf("×");fflush( stdout );return -1;		//眼帯ちゃん ‎@vaissuu 2856339026
	}else if(TD->user_id==3255479114){printf("×");fflush( stdout );return -1;		//眼帯ちゃん @gantaityan 3255479114
	}else if(TD->user_id==4410062598){printf("×");fflush( stdout );return -1;		//‍‍天@SHIROlRO 4410062598
	}else if(TD->user_id==2406860035){printf("×");fflush( stdout );return -1;		//ゔぇいん‏@vain_lov 2406860035
	
	}else if(TD->user_id==3244393658){printf("×");fflush( stdout );return -1;		//瑠璃ちゃん@ワサラー団四天王 @Lapislazulichan 3244393658
	}else if(TD->user_id==1612743228){printf("×");fflush( stdout );return -1;		//nene. @N1205_x 1612743228
	}else if(TD->user_id==3239961378){printf("×");fflush( stdout );return -1;		//ゆうのすけ @MF7ecowK2XsopNt 3239961378
	}else if(TD->user_id==3040552530){printf("×");fflush( stdout );return -1;		//ʀᴇɴᴀᴄʜᴀɴ @R__n1214 3040552530
	}else if(TD->user_id==2834230994){printf("×");fflush( stdout );return -1;		//赤いコート 悪魔 おっさん @kiyomatu_ssS 2834230994
	}else if(TD->user_id==2838077966){printf("×");fflush( stdout );return -1;		//にゃル @ny0ru 2838077966
	}else if(TD->user_id==4111308492){printf("×");fflush( stdout );return -1;		//☆BAKUSOKU☆ @nipoonpeople 4111308492
	}else if(TD->user_id==722717344720674816){printf("×");fflush( stdout );return -1;		//六実くん @33_4RH 722717344720674816
	
	}else if(TD->user_id==4887122119){printf("×");fflush( stdout );return -1;		//つぶしちゃん @h0legs5bgw8p2p2 4887122119
	}else if(TD->user_id==3023310433){printf("×");fflush( stdout );return -1;		//ツイ速くん（改） @twisokkn 3023310433
	}else if(TD->user_id==715832009344565248){printf("×");fflush( stdout );return -1;		//-KIWAMI- @kiwami_japan 715832009344565248
	}else if(TD->user_id==4864579993){printf("×");fflush( stdout );return -1;		//てぃんくん @m_ztq 4864579993
	}else if(TD->user_id==3328656314){printf("×");fflush( stdout );return -1;		//@BuzzBird_tweet 3328656314
	}else if(TD->user_id==4751048174){printf("×");fflush( stdout );return -1;		//@twi_carnival 4751048174
	}else if(TD->user_id==1688428596){printf("×");fflush( stdout );return -1;		//@shiontawn 1688428596

	}else if(TD->user_id== 156515147){printf("×");fflush( stdout );return -1;		//マイメロディ【公式】 @Melody_Mariland 156515147

	}else if(TD->user_id==3222600793){printf("×");fflush( stdout );return -1;		//有償フォロ爆 日本人1万人2000円販売 ‎@followerhanbai3 3222600793
	
	}else if(TD->user_id==702852904097419266){printf("×");fflush( stdout );return -1;		//激かわ動物 ‏@gekikawawa 702852904097419266
	}else if(TD->user_id==713029012273016833){printf("×");fflush( stdout );return -1;		//嫉妬くん @shi_10kun 713029012273016833
	
	}else if(TD->user_id==4735992986){printf("×");fflush( stdout );return -1;		//逆罪くん ‎@ReverseSin 4735992986
	}else if(TD->user_id==3849509892){printf("×");fflush( stdout );return -1;		//同罪くん @SameOffense 3849509892
	}else if(TD->user_id==4735990616){printf("×");fflush( stdout );return -1;		//断罪くん @ConvictionBoy 4735990616
	
	}else if(TD->user_id==2927905184){printf("×");fflush( stdout );return -1;		//ばりすた @_Tokyo_coffee_ 2927905184
	}else if(TD->user_id==3966426798){printf("×");fflush( stdout );return -1;		//無名くん @lone_zest 3966426798
	
	}else if(TD->user_id==2764834345){printf("×");fflush( stdout );return -1;		//消しゴム@フォロバ100% @girilla________ 2764834345
	}else if(TD->user_id==1015124330){printf("×");fflush( stdout );return -1;		//びゃくや @vmc_7 1015124330
	}else if(TD->user_id==2925787321){printf("×");fflush( stdout );return -1;		//Riku @sky_ri9u 2925787321
	}else if(TD->user_id==1918545776){printf("×");fflush( stdout );return -1;		//ぜつは @_z2h 1918545776
	}else if(TD->user_id==3255464504){printf("×");fflush( stdout );return -1;		//涙人 @TWIYASUMI 3255464504
	}else if(TD->user_id==2470699502){printf("×");fflush( stdout );return -1;		//@観察ちゃん＠相互 @Nari0a 2470699502	//凍結済みアカウント
	}else if(TD->user_id==3467642473){printf("×");fflush( stdout );return -1;		//れーる @kdjtr 3467642473
	}else if(TD->user_id==3228227106){printf("×");fflush( stdout );return -1;		//いりな @hytrq 3228227106
	}else if(TD->user_id==3299395507){printf("×");fflush( stdout );return -1;		//紫陽花 @ksdwq 3299395507
	}else if(TD->user_id==4649761098){printf("×");fflush( stdout );return -1;		//るな @vkshy 4649761098
	}else if(TD->user_id==4843076018){printf("×");fflush( stdout );return -1;		//るな @vkshyy 4843076018
	}else if(TD->user_id==1574105790){printf("×");fflush( stdout );return -1;		//りっちゃん @RitzChan_ 1574105790
	}else if(TD->user_id==2413763917){printf("×");fflush( stdout );return -1;		//IKAさん @PC_no_Naka 2413763917
	
	}else if(TD->user_id==3184376436){printf("×");fflush( stdout );return -1;		//ぜろとくん ‏@Zerotochan 3184376436							//凍結済みアカウント
	}else if(TD->user_id==3083536818){printf("×");fflush( stdout );return -1;		//かずくん【公式】 @kazukntv 3083536818
	}else if(TD->user_id==4411571058){printf("×");fflush( stdout );return -1;		//とわくん @towagami 4411571058
	}else if(TD->user_id==2540743532){printf("×");fflush( stdout );return -1;		//適当に読み流すツイート。 @tekitou2_ 2540743532
	}else if(TD->user_id==4843995049){printf("×");fflush( stdout );return -1;		//めうる @qkdsi2 4843995049
	
	}else if(TD->user_id==4747454634){printf("×");fflush( stdout );return -1;		//バズったー @buzz_tweet1001 4747454634
	}else if(TD->user_id==3926887578){printf("×");fflush( stdout );return -1;		//【復活】バズったー ‎@buzz_tweet_jp 3926887578
	
	}else if(TD->user_id==2975786033){printf("×");fflush( stdout );return -1;		//甲子園に輝く名言集 @peqoogowowy 2975786033
	}else if(TD->user_id==1945417968){printf("×");fflush( stdout );return -1;		//木曜日のアラタユカ(ニダイメ) @Motion_Blur_ 1945417968
	}else if(TD->user_id== 444501695){printf("×");fflush( stdout );return -1;		//無花果@7/8誕生 @__fig__ 444501695
	}else if(TD->user_id==1272599227){printf("×");fflush( stdout );return -1;		//新星あおば @AOBARING 1272599227
	}else if(TD->user_id==2901526310){printf("×");fflush( stdout );return -1;		//みっきー @32qum 2901526310
	}else if(TD->user_id==3088004528){printf("×");fflush( stdout );return -1;		//シゲル @_si_chan_ 3088004528
	}else if(TD->user_id==2778018812){printf("×");fflush( stdout );return -1;		//後藤ろりしうむ @6Resm 2778018812
	}else if(TD->user_id==2998465453){printf("×");fflush( stdout );return -1;		//託未くん @09Ta_ku 2998465453
	}else if(TD->user_id== 488103057){printf("×");fflush( stdout );return -1;		//pad_sexy　パズル＆ドラゴンズ公式 @pad_sexy 488103057	//公式と書いてあるのに公式マークは無かったが、恐らく公式っぽい。
	}else if(TD->user_id==2396044305){printf("×");fflush( stdout );return -1;		//twi_news @nusosayfop 2396044305
	}else if(TD->user_id==2415641983){printf("×");fflush( stdout );return -1;		//くろろん @_kuroron_ 2415641983
	}else if(TD->user_id==2418502688){printf("×");fflush( stdout );return -1;		//椎菜ぜるくん @5awi 2418502688
	}else if(TD->user_id== 402547102){printf("×");fflush( stdout );return -1;		//utapri_official @utapri_official 402547102
	}else if(TD->user_id==1685298566){printf("×");fflush( stdout );return -1;		//ぺぷ氏【公式】 @yossi_u 1685298566
	}else if(TD->user_id== 344730147){printf("×");fflush( stdout );return -1;		//Copy writing Copy__writing 344730147
	}else if(TD->user_id==2406915096){printf("×");fflush( stdout );return -1;		//time @time_60ss 2406915096
	}else if(TD->user_id==2546258868){printf("×");fflush( stdout );return -1;		//time @time_60ss_ 2546258868
	}else if(TD->user_id==3023340098){printf("×");fflush( stdout );return -1;		//time @time_60sss 3023340098
	}else if(TD->user_id==3843078792){printf("×");fflush( stdout );return -1;		//time @time____60ss 3843078792
	}else if(TD->user_id==3084668403){printf("×");fflush( stdout );return -1;		//time @olmjk 3084668403
	}else if(TD->user_id==3324397694){printf("×");fflush( stdout );return -1;		//time ‏@evwfrg 3324397694
	}else if(TD->user_id==2224317020){printf("×");fflush( stdout );return -1;		//凍結 @_The_cold 2224317020
	}else if(TD->user_id== 201472877){printf("×");fflush( stdout );return -1;		//仰天！世界まる見えBOT @maru_mie_ 201472877
	}else if(TD->user_id==1588020854){printf("×");fflush( stdout );return -1;		//占いランキング @uranai_rkg 1588020854
	}else if(TD->user_id==3145814126){printf("×");fflush( stdout );return -1;		//信じていいの？ @erggvwerf4r 3145814126
	}else if(TD->user_id==3200721707){printf("×");fflush( stdout );return -1;		//信じていいの？ @sinziteiino 3200721707
	}else if(TD->user_id==2611304718){printf("×");fflush( stdout );return -1;		//信じていいの？ @sinzite_ino 2611304718
	}else if(TD->user_id==3042414151){printf("×");fflush( stdout );return -1;		//名前診断【本家】 @namae_shindan 3042414151
	}else if(TD->user_id==2949006426){printf("×");fflush( stdout );return -1;		//=日本の俺bot= @nihonotokotti 2949006426
	}else if(TD->user_id==2572989374){printf("×");fflush( stdout );return -1;		//Riku @sky_rik9 2572989374
	}else if(TD->user_id== 739945171){printf("×");fflush( stdout );return -1;		//てぃむらーくん【公式】 @Eroipu_God 739945171
	}else if(TD->user_id==3194150238){printf("×");fflush( stdout );return -1;		//夜桜アルト ?@_SAKURASAKASOU_ 3194150238
	}else if(TD->user_id==2928539263){printf("×");fflush( stdout );return -1;		//ヤンキーバカにすんなよ @yankeeeee_s 2928539263
	}else if(TD->user_id==3080450959){printf("×");fflush( stdout );return -1;		//バグったー @bagu_tter 3080450959
	}else if(TD->user_id==2677458145){printf("×");fflush( stdout );return -1;		//俺の道 @orenomiti_1 2677458145
	}else if(TD->user_id==2996755637){printf("×");fflush( stdout );return -1;		//ドラマ・映画情報 @movie_info1 3035996792
	}else if(TD->user_id==3182486466){printf("×");fflush( stdout );return -1;		//Moment @last__moment 3182486466
	}else if(TD->user_id==3185928356){printf("×");fflush( stdout );return -1;		//テレビおもしろ☆場面集☆ @waraeru_daisuki 3185928356
	}else if(TD->user_id==2941176504){printf("×");fflush( stdout );return -1;		//テレビおもしろ場面集☆ @akkeeii 2941176504
	}else if(TD->user_id==2941176504){printf("×");fflush( stdout );return -1;		//非公式 テレビおもしろ場面集 @akkeeeii 3165601752
	}else if(TD->user_id==2941176504){printf("×");fflush( stdout );return -1;		//おもしろテレビ場面集 @omosiro_terebi1 2896668606
	}else if(TD->user_id== 227618546){printf("×");fflush( stdout );return -1;		//ふぁいぶ @five1130 227618546
	}else if(TD->user_id==3097948927){printf("×");fflush( stdout );return -1;		//レヴィン(仮) @Kousakahonoka1 3097948927
	}else if(TD->user_id== 581614155){printf("×");fflush( stdout );return -1;		//少女ナイフ @sjn_bot 581614155
	}else if(TD->user_id==3299427366){printf("×");fflush( stdout );return -1;		//全日本マヂキチ選手権 @mjktjpn 3299427366
	}else if(TD->user_id==3604088534){printf("×");fflush( stdout );return -1;		//マヂキチJapan @zen_nippon1 3604088534
	}else if(TD->user_id==4289388434){printf("×");fflush( stdout );return -1;		//マヂキチJAPAN @zen_nippon2 4289388434
	}else if(TD->user_id== 174056039){printf("×");fflush( stdout );return -1;		//ミスタードーナツ @misterdonut_jp 174056039
	}else if(TD->user_id==2571080700){printf("×");fflush( stdout );return -1;		//日本人フォロ爆 Instagram 販売 @tu_o 2571080700
	}else if(TD->user_id==2996755637){printf("×");fflush( stdout );return -1;		//裏少女（R18） @ura_sjoo 2996755637
	}else if(TD->user_id==3135271668){printf("×");fflush( stdout );return -1;		//女の気持ち @jagtjttt 3135271668
	}else if(TD->user_id==3156941898){printf("×");fflush( stdout );return -1;		//女子の本音 @erggvwer44r 3156941898
	}else if(TD->user_id==  91081573){printf("×");fflush( stdout );return -1;		//一番くじ @ichibanKUJI 91081573
	}else if(TD->user_id==3250248014){printf("×");fflush( stdout );return -1;		//三代目J Soul Brothers @Jsoul_3th 3250248014
	}else if(TD->user_id==3277937070){printf("×");fflush( stdout );return -1;		//ナキアメ @sex_syoujyo 3277937070
	}else if(TD->user_id==4747551794){printf("×");fflush( stdout );return -1;		//トレンド速報BEST @tresokubest1 4747551794
	}else if(TD->user_id==3154682270){printf("×");fflush( stdout );return -1;		//速報ニュース通知アプリ PAGEVIEW @pageviewjp 3154682270
	}else if(TD->user_id==734347453571420160){printf("×");fflush( stdout );return -1;		//✩✩10秒で笑える速報✩✩ @10warasoku 734347453571420160
	}else if(TD->user_id==741953776190263300){printf("×");fflush( stdout );return -1;		//クソワロ大全集 @kusowarotanwwww 741953776190263300
	}
	//YabaTube @Yaba_Tube
	
	
	
	//まとめサイトのツイート
	if(TD->user_id==44502620){
		//ハム速 ?@hamusoku 44502620
		printf("×");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
		fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
		return -1;
	}else if(TD->user_id==147471915){
		//やら管 @yarare_kanrinin 147471915
		printf("×");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
		fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
		return -1;
	}else if(TD->user_id==85291785){
		//はちま起稿 @htmk73 85291785
		printf("×");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
		fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
		return -1;
	}else if(TD->user_id==22273191){
		//オレ的ゲーム速報 @Jin115 22273191
		printf("×");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
		fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
		return -1;
	}
	
	
	
	//以下は削除要請によるもの。
	if(TD->user_id==89872620){
		//成宮らい ?@ro_xaxel 89872620
		
		//https://twitter.com/ro_xaxel/status/594039349907324928
		//@Rating_Sprite すいませんがこちらのわたしのツイートははじいてくださいますよう、スクリプトしてください。
		
		printf("×");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
		fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
		return -1;
	}else if(TD->user_id==2605667586){
		//現役女子大生 @334j114514 2605667586
		
		//https://twitter.com/334j114514/status/594663044052975616
		//@Rating_Sprite やめてもらえますか
		
		//https://twitter.com/334j114514/status/594710013257469952
		//こういうBOT嫌い
		//まず許可を取ってこないから
		
		printf("×");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
		fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
		return -1;
	}
	
	
	return 0;
}

int BlackListFilter_en( struct get_data_from_tweet*& TD ){
	
	//以下[ユーザ名]でフィルタする。
	{
		//WORLD STAR (@BestOfWorldStar)
		//WORLD STAR (@WorIdStarComedy)
		//WORLD STAR (@WORLDSTARC0MEDY)
		//WORLD STAR FANS (@WorldStarFunny)
		//WORLD STAR FANS (@WorIdStarFunnyy)
		//WORLD STAR VINE (@WORLDSTARVlNE)
		//WORLD STAR BRUH (@WorldStarComedy)
		//WORLD STAR SPORTS(@unlimited473)
		//WORLD STAR FIGHTS (@WorIdStarFight)
		//World Star (@ComedyWorIdStar)
		//World Star  @WorIdStarFans 330615366
		//World Star (@WorIdStarFunny)
		//World Star (@WorldStarGhetto)
		//World Star Daily (@WorIdStarDaiIy)
		//World Star Vines (@WorIdStarVine)
		//World Star Vines (@WorIdStarFights)
		//World Star Videos (@WorIdStarVidz)
		//World Star Videos (@WorIdStarVideos)
		//WORLDSTAR (@WORIDSTARHIPHOP)
		//WORLDSTAR(@WORLDSTARC0MEDY)
		//WORLDSTAR (@ItsWorIdStar)
		//WORLDSTAR (@FunnyWSHH)
		//WORLDSTARHIPHOP (@WORLDSTAR)
		//WorldStar (@FunnyWorldStar)
		//worldstar (@wshhdaiiy)
		//WorldStar Funny (@WorldStarLaughs)
		//WorldStar Hiphop NYC (@worldstarNYC)
		//WSHH @WORIDSTARHIPH0P
		char* result_p = strstr( (char*)TD->name_re.c_str() , "WORLD" );	//「WORLD」を検索する。
		if( result_p != NULL ){
			printf("×←nW");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
		result_p = strstr( (char*)TD->name_re.c_str() , "World" );	//「World」を検索する。
		if( result_p != NULL ){
			printf("×←nW");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
		result_p = strstr( (char*)TD->name_re.c_str() , "WSHH" );	//「WSHH」を検索する。
		if( result_p != NULL ){
			printf("×←nW");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		//Female Pains (@FemalePains)
		//Female Blogger RT(@FemaleBloggerRT)
		//Female Problems (@femaleproblems)
		//Female Struggles (@femaIe)
		//Female Struggles  @LovableNotebook
		//Female Struggles (@FemaleStruggIes)
		//Female Struggles (@TweetLikeGirIs)
		//Female Struggles (@comedyandtruth)
		//I Speak Female (@TheBossGirls)
		//The Female Book(@thefemaleboook)
		//Feisty Female (@MoragKeith)
		//Fast and Female (@FastandFemale)
		char* result_p = strstr( (char*)TD->name_re.c_str() , "Female" );	//「Female」を検索する。
		if( result_p != NULL ){
			printf("×←nF");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		//Girl Code! @itsGirICode
		//Common Girl (@SoReIatable)
		//Common White Girl (@girlposts)
		//Typical Girl @Iifepost
		//Typical Girl (@RelatabIe)
		//Typical Girl (@SoDamnTrue)
		//Girl Thoughts (@femaIenotes)
		char* result_p = strstr( (char*)TD->name_re.c_str() , "Girl" );	//「Girl」を検索する。
		if( result_p != NULL ){
			printf("×←nG");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
		result_p = strstr( (char*)TD->name_re.c_str() , "GIRL" );	//「GIRL」を検索する。
		if( result_p != NULL ){
			printf("×←nG");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		//Bitch Code. @TheTumblrPosts 798234950
		//Bitch Problems @FemaleTexts 204887235
		char* result_p = strstr( (char*)TD->name_re.c_str() , "Bitch" );	//「Bitch」を検索する。
		if( result_p != NULL ){
			printf("×←nB");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	
	/*
	Chuck Bass (@bwalch5)
	Chuck Bass (@ryhorg1)
	chuck bass (@itstead)
	Chuck Bass (@chacbas)
	Chuck Bass (@PapiChuck)
	Chuck Bass (@Singhness)
	Chuck Bass (@Julie_DNQ)
	Chuck Bass (@bobo11911)
	chuck bass (@AbudyAlami)
	Chuck Bass (@SebxstixnVI)
	Chuck Bass (@charlesbass)
	Chuck Bass (@ltsChuckBass)
	Chuck Bass (@lAmChuckBass)
	Chuck Bass (@Matty_Mac_72)
	Chuck Bass (@ChuckCretino)
	Chuck Bass (@assassin0409)
	Chuck Bass (@F4FCHUCKBASS)
	Chuck Bass (@TrinidadAqeel)
	Chuck Bass (@maddiefbright)
	Chuck Bass (@pablocampos98)
	Chuck Bass (@imchuckbass91)
	Chuck Bass (@iamthesmithson)
	Chuck Bass (@queweytancursi)
	Chuck Bass (@jackman_nick69)
	Chuck Bass. (@KingButhelezie)
	chuck bass. (@ClaraCerfontain)
	Chuck Bass Quotes (@chuckbassquote)
	Chuck and Blair (@parisinthe1920s)
	Tweets with replies by Chuck Bass (@strictlywinning)
	*/
	
	/*
	//以下、本文に「http」などの文字がある場合にフィルタする。//引用ツイートのリツイート数を上手くパースできていないため、バグるので、応急処置としてhttpを含むツイートを除外する。
	//今まで問題無かったので、恐らく、twitter社がJSONの並びを変えたと思われる。
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "http" );	//「http」を検索する。
		if( result_p != NULL ){
			printf("×←http");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	*/
	
	//以下、本文に「retweet」「follow」などの文字がある場合にフィルタする。
	//
	//リツイートやフォローを勧めるツイートは邪魔なので、除外
	//[例]
	//The Tide @TheTide			//Retweet this and I'll do a follow spree for all of you beautiful people
	//?HE FOLLOWED ME @BieberAnnual	//follow everyone who retweets and faves this
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "etweet" );	//「etweet」を検索する。(retweet,Retweet)
		if( result_p != NULL ){
			printf("×←t転");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "ollow" );	//「follow」を検索する。(follow,Follow)
		if( result_p != NULL ){
			printf("×←t韓");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	
	
	if(TD->user_id==3185683113){
		//Charlie Charlie @CharlieChallnge 3185683113
		printf("×");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
		fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
		return -1;
	}else if(TD->user_id== 340431323){printf("×");fflush( stdout );return -1;		//CHARLIE CHARLIE @CharlieVidss 340431323
	}else if(TD->user_id==2344652586){printf("×");fflush( stdout );return -1;		//Charlie @CharlieWelp 2344652586
	}else if(TD->user_id==1191287070){printf("×");fflush( stdout );return -1;		//CharlieCharlie @Charlie_CharIie 1191287070
	}else if(TD->user_id==2200634694){printf("×");fflush( stdout );return -1;		//Chuck Bass @ltsChuckBass 2200634694
	}else if(TD->user_id== 244381573){printf("×");fflush( stdout );return -1;		//Chuck Bass @_erickjordan 244381573
	}else if(TD->user_id==2993049478){printf("×");fflush( stdout );return -1;		//HipHopHotspot @HipHopxHotspot 2993049478
	}else if(TD->user_id== 834095274){printf("×");fflush( stdout );return -1;		//HIP HOP FACTS @OnlyHipHopFacts 834095274
	}else if(TD->user_id== 500328872){printf("×");fflush( stdout );return -1;		//CHILDHOOD RUINER @CHlLDHOODRUINER 500328872
	}else if(TD->user_id== 333168411){printf("×");fflush( stdout );return -1;		//The Female Book @thefemaleboook 333168411
	}else if(TD->user_id== 427748930){printf("×");fflush( stdout );return -1;		//lea @ohlordhes 427748930
	}else if(TD->user_id==2522338143){printf("×");fflush( stdout );return -1;		//votre fille K @kailllaaaa 2522338143
	}else if(TD->user_id==1855410506){printf("×");fflush( stdout );return -1;		//coco @cocoposts 1855410506
	}else if(TD->user_id==2986629179){printf("×");fflush( stdout );return -1;		//BreakLike..WAIT @coco_loko 2986629179
	}else if(TD->user_id==  71378109){printf("×");fflush( stdout );return -1;		//MELDAWG? @Talkingbout_Mel 71378109
	}else if(TD->user_id==1477352390){printf("×");fflush( stdout );return -1;		//mare,!! @recklesmgc 1477352390
	}else if(TD->user_id== 226690054){printf("×");fflush( stdout );return -1;		//Sincerely Tumblr @SincerelyTumblr 226690054
	}else if(TD->user_id==  26195765){printf("×");fflush( stdout );return -1;		//The One & Only @BlizzyBlack 26195765
	}else if(TD->user_id== 500328872){printf("×");fflush( stdout );return -1;		//CHILDHOOD RUINER @CHlLDHOODRUINER 500328872
	}else if(TD->user_id==1370986902){printf("×");fflush( stdout );return -1;		//WSHH FANS @WORIDSTARHlPHOP 1370986902
	}else if(TD->user_id== 132774626){printf("×");fflush( stdout );return -1;		//Common White Girl @girlposts 132774626
	}else if(TD->user_id== 304758187){printf("×");fflush( stdout );return -1;		//Mark 5SOS ? @MarkMendez1 304758187
	}else if(TD->user_id==2206702523){printf("×");fflush( stdout );return -1;		//Tommy #2MP @ThomasGrimes0 2206702523
	}else if(TD->user_id==1915625238){printf("×");fflush( stdout );return -1;		//Guapo @painings 1915625238
	}else if(TD->user_id== 354343449){printf("×");fflush( stdout );return -1;		//Exams finish June 15 @mayowa_daniella 354343449
	}else if(TD->user_id== 524657792){printf("×");fflush( stdout );return -1;		//FunnyPicsDepot @FunnyPicsDepot 524657792
	}else if(TD->user_id==3040107153){printf("×");fflush( stdout );return -1;		//Dooley @DooleyFunnyAf 3040107153
	}else if(TD->user_id==2975498390){printf("×");fflush( stdout );return -1;		//Dooley @OfficialDooley 2975498390
	}else if(TD->user_id==1126581884){printf("×");fflush( stdout );return -1;		//rachel loves niall @lightpinkhoran 1126581884
	}else if(TD->user_id==1640500489){printf("×");fflush( stdout );return -1;		//Epic Clips ? @EpicClips 1640500489
	}else if(TD->user_id==2798804815){printf("×");fflush( stdout );return -1;		//^anessarose @wittlenessa 2798804815
	}else if(TD->user_id== 805230144){printf("×");fflush( stdout );return -1;		//BIG MONEY MIKE @BIGMONEYMIKE6 805230144
	}else if(TD->user_id==1576146192){printf("×");fflush( stdout );return -1;		//@lulxx 1576146192
	}else if(TD->user_id== 386145266){printf("×");fflush( stdout );return -1;		//@mxxyaa 386145266
	}else if(TD->user_id== 944667252){printf("×");fflush( stdout );return -1;		//Nate @_nate9 944667252
	}else if(TD->user_id==3051134435){printf("×");fflush( stdout );return -1;		//dani / 1 @wakefranta 3051134435
	}else if(TD->user_id== 335431654){printf("×");fflush( stdout );return -1;		//@LaurentKelsey 335431654
	}else if(TD->user_id== 371678594){printf("×");fflush( stdout );return -1;		//@Itsry4nn 371678594
	}else if(TD->user_id== 956445410){printf("×");fflush( stdout );return -1;		//n @MUSCULARJACK 956445410
	}else if(TD->user_id== 572064770){printf("×");fflush( stdout );return -1;		//Chill Vibes @ChilledVlbes 572064770
	}else if(TD->user_id==1039764576){printf("×");fflush( stdout );return -1;		//? @smokebieberr 1039764576
	}else if(TD->user_id==2242195845){printf("×");fflush( stdout );return -1;		//PC Coquelin @yung__h 2242195845
	}else if(TD->user_id==2986316007){printf("×");fflush( stdout );return -1;		//idfw @iDoFifaWagers 2986316007
	}else if(TD->user_id== 611959284){printf("×");fflush( stdout );return -1;		//Starbucks Girl @FreakingTrue 611959284
	}else if(TD->user_id== 161521834){printf("×");fflush( stdout );return -1;		//Donnie Tsunami @VvsDonnie 161521834
	}else if(TD->user_id==2873911918){printf("×");fflush( stdout );return -1;		//edward @ljpspenguin 2873911918
	}else if(TD->user_id==  25888849){printf("×");fflush( stdout );return -1;		//Shadina J. @heybuddy1 25888849
	}else if(TD->user_id== 457531267){printf("×");fflush( stdout );return -1;		//Haila @simplyhaila 457531267
	}else if(TD->user_id==1056492074){printf("×");fflush( stdout );return -1;		//mean plastic @meanpIastic 1056492074
	}else if(TD->user_id== 321445166){printf("×");fflush( stdout );return -1;		//Sarcasm @ThatsSarcasm 321445166
	}else if(TD->user_id== 590041928){printf("×");fflush( stdout );return -1;		//She Ratchet @SheeeRatchet 590041928
	}else if(TD->user_id== 724223456){printf("×");fflush( stdout );return -1;		//College Humor @_CollegeHumor_ 724223456
	}else if(TD->user_id==2328344389){printf("×");fflush( stdout );return -1;		//CHEF CURRY @WhoopCurry 2328344389
	}else if(TD->user_id==1273036716){printf("×");fflush( stdout );return -1;		//Ella @TRAPG4RL 1273036716
	}else if(TD->user_id== 238923113){printf("×");fflush( stdout );return -1;		//daphne @tbhdaphne 238923113
	}else if(TD->user_id== 199669993){printf("×");fflush( stdout );return -1;		//Just Friends @FemaIeThings 199669993
	}else if(TD->user_id==2392882075){printf("×");fflush( stdout );return -1;		//marifer @flowerfulkian 2392882075
	}else if(TD->user_id==1351269486){printf("×");fflush( stdout );return -1;		//K E N Z @babiekenz 1351269486
	}else if(TD->user_id==1147538318){printf("×");fflush( stdout );return -1;		//s loves z ??*? @invisibleziam 1147538318
	}else if(TD->user_id== 328350273){printf("×");fflush( stdout );return -1;		//yung anime @lexsenpai 328350273
	}else if(TD->user_id== 488242107){printf("×");fflush( stdout );return -1;		//The Bucket List @TheBucktList 488242107
	}else if(TD->user_id== 119494231){printf("×");fflush( stdout );return -1;		//Shanie P. @___shanie 119494231
	}else if(TD->user_id== 527507895){printf("×");fflush( stdout );return -1;		//mo loves louis ???? @twinklouisx 527507895
	}else if(TD->user_id==  57893319){printf("×");fflush( stdout );return -1;		//louis @sassymcluke 57893319
	}else if(TD->user_id==1459537578){printf("×");fflush( stdout );return -1;		//Louis Tomlinson @zouirinialI 1459537578
	}else if(TD->user_id==2899724324){printf("×");fflush( stdout );return -1;		//Louis Tomlinson @Z0UIRNIALL 2899724324
	}else if(TD->user_id==  57742625){printf("×");fflush( stdout );return -1;		//sassy @tbhldgaf 57742625
	}else if(TD->user_id==2314076370){printf("×");fflush( stdout );return -1;		//Sassy Whale @asassywhale 2314076370
	}else if(TD->user_id==2853832185){printf("×");fflush( stdout );return -1;		//sam @ThriIlest 2853832185
	}else if(TD->user_id==  19991358){printf("×");fflush( stdout );return -1;		//Saun @Sauniell 19991358
	}else if(TD->user_id==1616352889){printf("×");fflush( stdout );return -1;		//??Calum Hood?? @TheIrwinEmpire 1616352889
	}else if(TD->user_id== 337486345){printf("×");fflush( stdout );return -1;		//James @jmondesiro 337486345
	}else if(TD->user_id==2985068540){printf("×");fflush( stdout );return -1;		//Black People Comedy @BlackPplComedy 2985068540
	}else if(TD->user_id== 541772949){printf("×");fflush( stdout );return -1;		//Teenagers @yoteens 541772949
	}else if(TD->user_id== 557455977){printf("×");fflush( stdout );return -1;		//Drizzy @Drrake 557455977
	}else if(TD->user_id== 217630894){printf("×");fflush( stdout );return -1;		//Petty Riley @LowKei_ 217630894
	}else if(TD->user_id== 898698553){printf("×");fflush( stdout );return -1;		//Student Athlete Prob @StuAthProblems 898698553
	}else if(TD->user_id==2878719086){printf("×");fflush( stdout );return -1;		//OG Nesquik @6ixGodQuay 2878719086
	}else if(TD->user_id== 457995411){printf("×");fflush( stdout );return -1;		//mysoftballprobz ? @mysoftballprobz 457995411
	}else if(TD->user_id== 337038376){printf("×");fflush( stdout );return -1;		//Tweeting For Her @TweetingForHer 337038376
	}else if(TD->user_id== 879220309){printf("×");fflush( stdout );return -1;		//FUCCI TUCCI @FUCCl 879220309
	}else if(TD->user_id== 138855937){printf("×");fflush( stdout );return -1;		//BronzeGoddess ?? @Funsized_Cakess 138855937
	}else if(TD->user_id==2443335206){printf("×");fflush( stdout );return -1;		//Randy Graziani @randygraziani 2443335206
	}else if(TD->user_id== 601532797){printf("×");fflush( stdout );return -1;		//childishdante ? @Golfwbu 601532797
	}else if(TD->user_id== 264176734){printf("×");fflush( stdout );return -1;		//Brentan @OhHeyBrent 264176734
	}else if(TD->user_id==2752942779){printf("×");fflush( stdout );return -1;		//Best Vines @TheVinesDepot 2752942779
	}else if(TD->user_id==2379862393){printf("×");fflush( stdout );return -1;		//Athletes Relate @AthRelate 2379862393
	}else if(TD->user_id==1462317296){printf("×");fflush( stdout );return -1;		//?Raine? @Raine4ver 1462317296
	}else if(TD->user_id==1912839878){printf("×");fflush( stdout );return -1;		//Zach Clayton @bruhitszach 1912839878
	}else if(TD->user_id==1407892771){printf("×");fflush( stdout );return -1;		//. @Drvglordkush 1407892771
	}else if(TD->user_id== 474933656){printf("×");fflush( stdout );return -1;		//N@CT @CoolTableTweets 474933656
	}else if(TD->user_id== 489309720){printf("×");fflush( stdout );return -1;		//?Kyro? @CartoonKy 489309720
	}else if(TD->user_id== 395639817){printf("×");fflush( stdout );return -1;		//Sex Freaks @CraziestSex 395639817
	}else if(TD->user_id==2569984021){printf("×");fflush( stdout );return -1;		//softball quotes @funnysoftball 2569984021
	}else if(TD->user_id==3024453651){printf("×");fflush( stdout );return -1;		//Kent Murphy @RealCoachKent 3024453651
	}else if(TD->user_id== 829495351){printf("×");fflush( stdout );return -1;		//Home Of Athletes @HomeOfAthletes 829495351
	}else if(TD->user_id==1028864341){printf("×");fflush( stdout );return -1;		//. @httpflexs 1028864341
	}else if(TD->user_id==1258444693){printf("×");fflush( stdout );return -1;		//I Speak My Heart? @crazylifefacts_ 1258444693
	}else if(TD->user_id== 748137168){printf("×");fflush( stdout );return -1;		//She Wants The D @SheswantstheD 748137168
	}else if(TD->user_id== 953952102){printf("×");fflush( stdout );return -1;		//AT EAZE @AtEaze808 953952102
	}else if(TD->user_id== 941520001){printf("×");fflush( stdout );return -1;		//Athlete Motivation @AthIeteMotivate 941520001
	}else if(TD->user_id==2896294831){printf("×");fflush( stdout );return -1;		//Superhero Feed @SuperheroFeed 2896294831
	}else if(TD->user_id== 204385761){printf("×");fflush( stdout );return -1;		//?????????? @cib110 204385761
	}else if(TD->user_id==3246845476){printf("×");fflush( stdout );return -1;		//Bleacher Report @BlaecherReport 3246845476
	}else if(TD->user_id==  57838735){printf("×");fflush( stdout );return -1;		//milk @miilkkk 57838735
	}else if(TD->user_id== 961394077){printf("×");fflush( stdout );return -1;		//????? ??? ?lizzy . @GLOUPNEXTDOOR 961394077
	}else if(TD->user_id==  38871426){printf("×");fflush( stdout );return -1;		//The Real Smoove?? @iAmRealSmoove 38871426
	}else if(TD->user_id==  48590242){printf("×");fflush( stdout );return -1;		//Deejay $antana @BreesInDaCut 48590242
	}else if(TD->user_id==  99722237){printf("×");fflush( stdout );return -1;		//i follow back bc ily @NarryMyDream 99722237
	}else if(TD->user_id== 863888599){printf("×");fflush( stdout );return -1;		//??9? @NeishaaaNee 863888599
	}else if(TD->user_id==2331173445){printf("×");fflush( stdout );return -1;		//??aee? @vvvx3_ 2331173445
	}else if(TD->user_id==2274715897){printf("×");fflush( stdout );return -1;		//ONLY THE NBA @OnlyTheNBA 2274715897
	}else if(TD->user_id== 142040643){printf("×");fflush( stdout );return -1;		//Muvah $teez @StasMarley 142040643
	}else if(TD->user_id==2214982328){printf("×");fflush( stdout );return -1;		//cayla. @cayjooo 2214982328
	}else if(TD->user_id==2207734063){printf("×");fflush( stdout );return -1;		//B-NARD @youfunnyb 2207734063
	}else if(TD->user_id==2728551676){printf("×");fflush( stdout );return -1;		//DailySexSupply @DailySexSupply 2728551676
	}else if(TD->user_id==2351192184){printf("×");fflush( stdout );return -1;		//Andre Woods ? @itsandrewoods 2351192184
	}else if(TD->user_id== 818941136){printf("×");fflush( stdout );return -1;		//Shit Mexicans Do @SOMEXlCAN 818941136
	}else if(TD->user_id==2926525740){printf("×");fflush( stdout );return -1;		//@IImraM 2926525740
	}else if(TD->user_id==2745269947){printf("×");fflush( stdout );return -1;		//@jazmine_15xo 2745269947
	}else if(TD->user_id==2423027420){printf("×");fflush( stdout );return -1;		//Lil King Ryin @KingRyin 2423027420
	}else if(TD->user_id== 342601720){printf("×");fflush( stdout );return -1;		//King James R @LeBronJames 342601720
	}else if(TD->user_id==1240324158){printf("×");fflush( stdout );return -1;		//Uncle Drew @UncleDrewGB 1240324158
	}else if(TD->user_id== 195909357){printf("×");fflush( stdout );return -1;		//June 7th @_RJ901_ 195909357
	}else if(TD->user_id== 177454759){printf("×");fflush( stdout );return -1;		//Nino3x @SouthernAssBoi 177454759
	}else if(TD->user_id==2751289319){printf("×");fflush( stdout );return -1;		//Gem @therealgemvp 2751289319
	}else if(TD->user_id==1123488680){printf("×");fflush( stdout );return -1;		//FutbolBible @FutbolBible 1123488680
	}else if(TD->user_id==1889198071){printf("×");fflush( stdout );return -1;		//BASED JE$U$ @PRAYINGFORHEAD 1889198071
	}else if(TD->user_id== 774072534){printf("×");fflush( stdout );return -1;		//V (?) N C E @LIOSE7EN 774072534
	}else if(TD->user_id== 411111930){printf("×");fflush( stdout );return -1;		//SAMMY @SammyTellem 411111930
	}else if(TD->user_id== 177454759){printf("×");fflush( stdout );return -1;		//Nino3x @SouthernAssBoi 177454759
	}else if(TD->user_id== 946770918){printf("×");fflush( stdout );return -1;		//NBA RETWEET @RTNBA 946770918
	}else if(TD->user_id==2204466550){printf("×");fflush( stdout );return -1;		//The FOOTBALL Bible @TFBible_ 2204466550
	}else if(TD->user_id==2273626526){printf("×");fflush( stdout );return -1;		//Smile? @smilepIease 2273626526
	}else if(TD->user_id== 501500898){printf("×");fflush( stdout );return -1;		//Best Of Black Twitta @Ratchet 501500898
	}else if(TD->user_id== 324731213){printf("×");fflush( stdout );return -1;		//#KKB @MascotMY_Tweets 324731213
	}else if(TD->user_id==  15137359){printf("×");fflush( stdout );return -1;		//Jeff Lewis @ChicagoPhotoSho 15137359
	}else if(TD->user_id== 228203206){printf("×");fflush( stdout );return -1;		//Victoria @CountOnVic 228203206
	}else if(TD->user_id==  57162282){printf("×");fflush( stdout );return -1;		//Shea Serrano @SheaSerrano 57162282
	}else if(TD->user_id== 371866546){printf("×");fflush( stdout );return -1;		//i love you. @JustTumbIr 371866546
	}else if(TD->user_id==2856207122){printf("×");fflush( stdout );return -1;		//Simba Da Gawd @youknowsimba 2856207122
	}
	
	
	/*
	else if(TD->user_id==){
		//
		printf("×");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
		fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
		return -1;
	}
	*/
	
	return 0;
}

int BlackListFilter_ko( struct get_data_from_tweet*& TD ){
	
	/*
	//以下、本文に「http」などの文字がある場合にフィルタする。//引用ツイートのリツイート数を上手くパースできていないため、バグるので、応急処置としてhttpを含むツイートを除外する。
	//今まで問題無かったので、恐らく、twitter社がJSONの並びを変えたと思われる。
	{
		char* result_p = strstr( (char*)TD->text.c_str() , "http" );	//「http」を検索する。
		if( result_p != NULL ){
			printf("×←http");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
			return -1;
		}
	}
	*/
	
	return 0;
}


int DataBaseFilter( struct get_data_from_tweet*& TD, class PostedTweetsDataBase* &pPT_DB ){
	
	// TD->tweet_id_re がlevelDBに登録されているか?
	
	// 読み込み処理
	bool result;
	struct SaveOnHashTable* pSoHT;
	pPT_DB->get0( TD->tweet_id_re, (void*&)pSoHT, result );
	if (result==true) {
		//DBにBloggerに投稿したと記録がある場合。
		printf("☆");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
		fflush( stdout );	//for debag
		
		if( (TD->average_reTweet_per_second) > (pSoHT->average_reTweet_per_second) ){
			
			// levelDBから、内製データベースへの移行用のコード。(今日を含めず、)30日以上経過したら、コメントアウトする。
			if( pSoHT->tm_BlogPostTime.tm_year <=2015-1900
			 && pSoHT->tm_BlogPostTime.tm_mon  <=9-1
			 && pSoHT->tm_BlogPostTime.tm_mday <=26        )
			{
				return -1;
			}
			
			printf("←〆");		//動作確認用(長いと直ぐにコンソールが埋まるため。)
			fflush( stdout );	//for debag
			
			pSoHT->average_reTweet_per_second = TD->average_reTweet_per_second;				// 平均リツイート速度の更新。
			
			pPT_DB->RenewSpeedInTheText( TD->tweet_id_re, (struct SaveOnHashTable&)*pSoHT );	//テキストに保存しているデータの更新
		}
	
//		fflush( stdout );	//Unix系では「\n」を出力しない限り、コンソールに文字が出力されないので、明示的に出力させる。
		
		return -1;	//既にBloggerに投稿されたツイートなので、破棄する。
	}else{
		//levelDBに記録がない場合。
		//std::cout << "Not found" << std::endl;
	}
	
	return 0;
}
//======================================================================
//======================================================Filters

