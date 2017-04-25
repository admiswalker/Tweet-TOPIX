# Tweet-TOPIX

"Tweet-TOPIX: https://tweet-topix.blogspot.jp/" は、現在ツイッターで話題となっているツイートを自動抽出する bot プログラムです。軽量のため、ノート PC 上で十分動作可能です。必要な通信量は、約 500 [GB/Month] ですので、従量課金制のクラウドサーバでの運用は避けてください (StreamingAPI での通信で圧縮を指定する改良を行えば、多少減らすことはできるかもしれません)。

動作としては、Twitter の StreamingAPI から、全ツイートの 1 % を取得し、ブラックリストによるフィルタでスパムツイートを除去したのち、ハッシュテーブル上に集め、毎時 15 分と 45 分に、単位時間あたりのリツイート数によってソートを行い、その結果をメールで Blogger に送信しています。

C/C++ の練習のために作成したプログラムです。リファクタリングを行ってから公開する予定でしたが、いつまでもリファクタリングを行う気にならなかったので、公開します。  

既知の問題 (自分めも)  
・はぁリファクタリング・・・  
・ハッシュテーブルがバグったままなので、std::unorderd_map に換装するか、修正済みの "https://github.com/admiswalker/InsertedChainedHashTable" に換装する。  
・makefile を "https://github.com/admiswalker/GenericMakefile" に変更したほうがたぶん楽になるはず？  
・OpenSSL がブロッキングモードで時々制御を返さなくなるため、新しいスレッドを生成して、スレッドが指定時間以上制御を返さない場合、スレッドごと消す操作をしている。これを、OpenSSL の ノンブロッキングモードなら、新規スレッドを生成せずに済むか試す。  
・スパムフィルタを Deep でよきにする？(せめてO(1)になるようにハッシュテーブルで

