#include <crypt.h>
#include <openssl/aes.h>
#include <strstream>

std::vector<std::vector<unsigned char> > make_AES_blocks(
	const std::string& source
){
	std::vector<std::vector<unsigned char> > blocks;
	std::vector<unsigned char> block(AES_BLOCK_SIZE);
	for(std::size_t ii{0}; ii<source.size();++ii){
		auto block_ptr{ii%block.size()};
		block[block_ptr] = source[ii];
		if((block_ptr+1)==block.size()){
			blocks.push_back(block);
		} else if (ii == (source.size()-1)){
			for(auto jj{block_ptr+1}; jj<AES_BLOCK_SIZE; ++jj)
				block[jj] = ' ';
			blocks.push_back(block);
		}
	}
	return(blocks);		
}
std::string encrypt_string_AES(
	std::string keygen,
	std::string input
){
	AES_KEY key;
	keygen.resize(AES_BLOCK_SIZE);
	
	unsigned char uc_key[AES_BLOCK_SIZE];
	for(std::size_t ii{0}; ii<keygen.size();++ii){
		uc_key[ii] = (unsigned char)keygen[ii];
	}
	
	AES_set_encrypt_key(
		uc_key,
		AES_BLOCK_SIZE*8,
		&key
	);
	
	std::vector<std::vector<unsigned char> > blocks{
		make_AES_blocks(input)
	};
	
	std::string scrambled;
	
	for(const auto& block : blocks){
		unsigned char crypt_output[AES_BLOCK_SIZE];
		AES_encrypt(&block[0],crypt_output,&key);
		std::string crypt_string;
		crypt_string.resize(AES_BLOCK_SIZE);
		for(auto ic{0}; ic<AES_BLOCK_SIZE; ++ic)
			crypt_string[ic] = crypt_output[ic];
		scrambled += crypt_string;
	}
	
	std::string outstr;
	for(const auto& cc : scrambled){
		outstr += std::to_string((int)cc) + "\t";
	}
	if(outstr.size()) outstr.resize(outstr.size()-1);
	
	return(outstr);
}



std::string decrypt_string_AES2(
	std::string keygen,
	std::string inseq
){
	std::vector<unsigned char> scrambled_nums;
	std::strstream instream{inseq};
	while(true){
		unsigned char coded_num;
		instream >> coded_num;
		if(instream.eof()) break;
		scrambled_nums.push_back(coded_num);
	}
	
	assert((scrambled_nums.size()%AES_BLOCK_SIZE)==0);
	
	std::vector<std::vector<unsigned char> > scrambled_blocks;
	std::vector<unsigned char> block;
	for(const auto& n : scrambled_nums){
		auto bs{block.size()};
		if(((bs+1)%AES_BLOCK_SIZE)==0){
			scrambled_blocks.push_back(block);
			block.resize(0);
		}
		block.push_back(n);
	}
	
}




std::string decrypt_string_AES(
	std::string keygen,
	std::string input
){
	AES_KEY key;
	keygen.resize(AES_BLOCK_SIZE);
	
	unsigned char uc_key[AES_BLOCK_SIZE];
	for(std::size_t ii{0}; ii<keygen.size();++ii){
		uc_key[ii] = (unsigned char)keygen[ii];
	}
	
	AES_set_decrypt_key(
		uc_key,
		AES_BLOCK_SIZE*8,
		&key
	);
	
	std::vector<std::vector<unsigned char> > blocks{
		make_AES_blocks(input)
	};
	
	std::string outstr;
	
	for(const auto& block : blocks){
		unsigned char crypt_output[AES_BLOCK_SIZE];
		AES_decrypt(&block[0],crypt_output,&key);
		std::string crypt_string;
		crypt_string.resize(AES_BLOCK_SIZE);
		for(auto ic{0}; ic<AES_BLOCK_SIZE; ++ic)
			crypt_string[ic] = crypt_output[ic];
		outstr += crypt_string;
	}

	return(outstr);
}
