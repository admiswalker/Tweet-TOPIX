#include "HMAC_SHA1_and_Base64encode.h"

//HMAC-SHA1をOpenSSLを利用して計算し、Base64にencodeする。
std::string HMAC_SHA1_and_Base64encode( std::string &signature_key , std::string &signature_data )
{
	// signature_key       : key
	// signature_data      : data
	
	unsigned char result[ SHA_DIGEST_LENGTH + 1 ];
	size_t result_size = strlen((const char*)result);
	
	//HMAC-SHA1の計算
	HMAC(	EVP_sha1() ,
			(const void *         )signature_key.c_str()  , (int)signature_key.length()  ,
			(const unsigned char *)signature_data.c_str() , (int)signature_data.length() ,
			(unsigned char*       )result                 , (unsigned int*)&result_size   );

	//Base64にencodeする。
	char *buf = Base64_encode( result , result_size );
	
	std::string s_buf = buf;
	free(buf);
	return s_buf;
}
