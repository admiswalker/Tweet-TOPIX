#commands
#	make       : 一時ファイルが古い場合のみ差分コンパイル(通常)
#	make all   : 一時ファイルをすべて削除してからコンパイル
#	make clean : 一時ファイルをすべて削除

#コンパイルする際、差分コンパイルを行うと、ファイルサイズが多少大きくなる。恐らく、最適化のかかり方が弱くなっている。
#差分コンパイル130.5kByte:make
#通常コンパイル126.2kByte:make once

# Makefile for Source.cpp
#EXEfile	: Source.cpp
#	g++ -o EXEfile Source.cpp ./My_function/Make_Connection.cpp ./My_function/Process.cpp ./My_function/SendGmail.cpp ./My_function/Decode_escapedUTF16String_to_UTF8Binary.cpp ./MyOAuthFunc/URL_encode_RFC_3986.cpp ./MyOAuthFunc/str_sprintf.cpp ./MyOAuthFunc/Generate_oauth_nonce.cpp ./MyOAuthFunc/Base64_encode.cpp ./MyOAuthFunc/HMAC_SHA1_and_Base64encode.cpp ./MyOAuthFunc/Generate_CustomHeadre_of_Authorization.cpp ./My_function/Filters.cpp ./My_function/ICHashT.cpp ./My_function/GetOneTweetJSON.cpp ./My_function/PostedTweetsDataBase.cpp ./My_function/MemoryLogger.cpp ./My_function/TwitterFunctions.cpp ./My_function/KeyInterrupter.cpp -lssl -lcrypto -std=gnu++0x -pthread -O2

#         ########################################################
objects = ./make_temp/Source.o\
\
          ./make_temp/Base64_encode.o\
          ./make_temp/Generate_CustomHeadre_of_Authorization.o\
          ./make_temp/Generate_oauth_nonce.o\
          ./make_temp/HMAC_SHA1_and_Base64encode.o\
          ./make_temp/URL_encode_RFC_3986.o\
          ./make_temp/str_sprintf.o\
\
          ./make_temp/Decode_escapedUTF16String_to_UTF8Binary.o\
          ./make_temp/Filters.o\
          ./make_temp/GetOneTweetJSON.o\
          ./make_temp/ICHashT.o\
          ./make_temp/KeyInterrupter.o\
          ./make_temp/Make_Connection.o\
          ./make_temp/MemoryLogger.o\
          ./make_temp/PostedTweetsDataBase.o\
          ./make_temp/Process.o\
          ./make_temp/SendGmail.o\
          ./make_temp/TwitterFunctions.o
#         ########################################################

EXEfile	: $(objects)
#	g++ -O3 -lssl -lcrypto -std=gnu++0x -pthread -o EXEfile Source.o Base64_encode.o Generate_CustomHeadre_of_Authorization.o Generate_oauth_nonce.o HMAC_SHA1_and_Base64encode.o URL_encode_RFC_3986.o str_sprintf.o  Decode_escapedUTF16String_to_UTF8Binary.o Filters.o GetOneTweetJSON.o ICHashT.o KeyInterrupter.o Make_Connection.o MemoryLogger.o PostedTweetsDataBase.o Process.o SendGmail.o TwitterFunctions.o
#上記の順番だとコンパイルが通らないので注意。
	g++ -o EXEfile $(objects) -lssl -lcrypto -std=gnu++0x -pthread -O3

# Source.o

### ./
#
#.o: 
#	g++ -c 

./make_temp/Source.o: Source.cpp
	g++ -c -o ./make_temp/Source.o Source.cpp -O3

# Base64_encode.o Generate_CustomHeadre_of_Authorization.o Generate_oauth_nonce.o HMAC_SHA1_and_Base64encode.o URL_encode_RFC_3986.o str_sprintf.o

### ./MyOAuthFunc/
#
#.o: ./MyOAuthFunc/
#	g++ -c ./MyOAuthFunc/

./make_temp/Base64_encode.o: ./MyOAuthFunc/Base64_encode.cpp
	g++ -c -o ./make_temp/Base64_encode.o ./MyOAuthFunc/Base64_encode.cpp -O3

./make_temp/Generate_CustomHeadre_of_Authorization.o: ./MyOAuthFunc/Generate_CustomHeadre_of_Authorization.cpp
	g++ -c -o ./make_temp/Generate_CustomHeadre_of_Authorization.o ./MyOAuthFunc/Generate_CustomHeadre_of_Authorization.cpp -O3

./make_temp/Generate_oauth_nonce.o: ./MyOAuthFunc/Generate_oauth_nonce.cpp
	g++ -c -o ./make_temp/Generate_oauth_nonce.o ./MyOAuthFunc/Generate_oauth_nonce.cpp -O3

./make_temp/HMAC_SHA1_and_Base64encode.o: ./MyOAuthFunc/HMAC_SHA1_and_Base64encode.cpp
	g++ -c -o ./make_temp/HMAC_SHA1_and_Base64encode.o ./MyOAuthFunc/HMAC_SHA1_and_Base64encode.cpp -O3

./make_temp/URL_encode_RFC_3986.o: ./MyOAuthFunc/URL_encode_RFC_3986.cpp
	g++ -c -o ./make_temp/URL_encode_RFC_3986.o ./MyOAuthFunc/URL_encode_RFC_3986.cpp -O3

./make_temp/str_sprintf.o: ./MyOAuthFunc/str_sprintf.cpp
	g++ -c -o ./make_temp/str_sprintf.o ./MyOAuthFunc/str_sprintf.cpp -O3

# Decode_escapedUTF16String_to_UTF8Binary.o Filters.o GetOneTweetJSON.o ICHashT.o KeyInterrupter.o Make_Connection.o MemoryLogger.o PostedTweetsDataBase.o Process.o SendGmail.o TwitterFunctions.o

### ./My_function/
#
#.o: ./My_function/
#	g++ -c ./My_function/

./make_temp/Decode_escapedUTF16String_to_UTF8Binary.o: ./My_function/Decode_escapedUTF16String_to_UTF8Binary.cpp
	g++ -c -o ./make_temp/Decode_escapedUTF16String_to_UTF8Binary.o ./My_function/Decode_escapedUTF16String_to_UTF8Binary.cpp -O3

./make_temp/Filters.o: ./My_function/Filters.cpp
	g++ -c -o ./make_temp/Filters.o ./My_function/Filters.cpp -O3

./make_temp/GetOneTweetJSON.o: ./My_function/GetOneTweetJSON.cpp
	g++ -c -o ./make_temp/GetOneTweetJSON.o ./My_function/GetOneTweetJSON.cpp -O3

./make_temp/ICHashT.o: ./My_function/ICHashT.cpp
	g++ -c -o ./make_temp/ICHashT.o -std=gnu++0x ./My_function/ICHashT.cpp -O3

./make_temp/KeyInterrupter.o: ./My_function/KeyInterrupter.cpp
	g++ -c -o ./make_temp/KeyInterrupter.o ./My_function/KeyInterrupter.cpp -O3

./make_temp/Make_Connection.o: ./My_function/Make_Connection.cpp
	g++ -c -o ./make_temp/Make_Connection.o ./My_function/Make_Connection.cpp -O3

./make_temp/MemoryLogger.o: ./My_function/MemoryLogger.cpp
	g++ -c -o ./make_temp/MemoryLogger.o ./My_function/MemoryLogger.cpp -O3

./make_temp/PostedTweetsDataBase.o: ./My_function/PostedTweetsDataBase.cpp
	g++ -c -o ./make_temp/PostedTweetsDataBase.o ./My_function/PostedTweetsDataBase.cpp -O3

./make_temp/Process.o: ./My_function/Process.cpp
	g++ -c -o ./make_temp/Process.o --std=c++0x ./My_function/Process.cpp -O3

./make_temp/SendGmail.o: ./My_function/SendGmail.cpp
	g++ -c -o ./make_temp/SendGmail.o ./My_function/SendGmail.cpp -O3

./make_temp/TwitterFunctions.o: ./My_function/TwitterFunctions.cpp
	g++ -c -o ./make_temp/TwitterFunctions.o ./My_function/TwitterFunctions.cpp -O3

###
#
#

all: clean $(objects)
	g++ -o EXEfile $(objects) -lssl -lcrypto -std=gnu++0x -pthread -O3

clean:
	rm -f $(objects)

onece:
	g++ -o EXEfile Source.cpp\
\
                   ./MyOAuthFunc/Base64_encode.cpp\
                   ./MyOAuthFunc/Generate_CustomHeadre_of_Authorization.cpp\
                   ./MyOAuthFunc/Generate_oauth_nonce.cpp\
                   ./MyOAuthFunc/HMAC_SHA1_and_Base64encode.cpp\
                   ./MyOAuthFunc/URL_encode_RFC_3986.cpp\
                   ./MyOAuthFunc/str_sprintf.cpp\
\
                   ./My_function/Decode_escapedUTF16String_to_UTF8Binary.cpp\
                   ./My_function/Filters.cpp\
                   ./My_function/GetOneTweetJSON.cpp\
                   ./My_function/ICHashT.cpp\
                   ./My_function/KeyInterrupter.cpp\
                   ./My_function/Make_Connection.cpp\
                   ./My_function/MemoryLogger.cpp\
                   ./My_function/PostedTweetsDataBase.cpp\
                   ./My_function/Process.cpp\
                   ./My_function/SendGmail.cpp\
                   ./My_function/TwitterFunctions.cpp\
\
                   -lssl -lcrypto -std=gnu++0x -pthread -O3


#[std::unordered_map]など、C++0x 固有の機能を用いる場合、以下のコンパイルオプションが必要[-std=gnu++0x]

#levelDBを使用するには下記のコンパイルオプションが必要。
#[/usr/local/lib/libleveldb.a -lpthread -I /usr/local/include/leveldb/]

#[生成物]:[生成物の材料]
# -o [保存名] : 保存先を指定
# -O2 : 最適化オプション。-O(-O1),-O2,-O3の三種類ある。数字が大きいほど強力な最適化処理が施されるが、-O3はバグを引き起こす可能性もあるので注意。

#以下、端末でのコンパイルは成功したものの、Makefileとしての書き方が分からず。
#$ g++ -o EXEfile ssltcpc.cc $(pkg-config --libs openssl)


#PP = pkg-config
#CXXFLAGS += $(shell ${PP} --cflags openssl)
#LIBS += $(shell ${PP} --libs openssl)
## Makefile for Source.cpp
#EXEfile: Source.cpp
#	$(CXX) $(CXXFLAGS) -o $@ $< $(LIBS)
