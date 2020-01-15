#include <iostream>
#include <fstream>
#include <vector>
#include <mqtt/async_client.h>
#include <mqtt/topic.h>
#include <wiringPi.h>
#include <chrono>
#include <ratio>
#include <openssl/aes.h>

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
	
	std::string outstr;
	
	for(const auto& block : blocks){
		unsigned char crypt_output[AES_BLOCK_SIZE];
		AES_encrypt(&block[0],crypt_output,&key);
		std::string crypt_string;
		crypt_string.resize(AES_BLOCK_SIZE);
		for(auto ic{0}; ic<AES_BLOCK_SIZE; ++ic)
			crypt_string[ic] = crypt_output[ic];
		outstr += crypt_string;
	}

	return(outstr);
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

const std::string broker_address{"130.179.196.54:1883"};
const std::string keygen_filename{"~/aes_key_gen.txt"};
const std::string raw_topic{"maxtopic/periodic/collatz"};


int main(int argc,char** argv){
	
	std::ifstream reader;
	reader.open(keygen_filename,std::ifstream::in);
	std::string keygen;
	reader >> keygen;
	reader.close();
	
	const std::string enc_topic{encrypt_string_AES(keygen,raw_topic)};
	
	std::cout << "Running..." << std::endl;
	
	std::cout << "Constructing client..." << std::endl;
	mqtt::async_client cl(broker_address,"hughsonPiSensor");
	
	std::cout << "Client is currently ";
	if(not(cl.is_connected())) std::cout << "not ";
	std::cout << "connnected" << std::endl;
	
	std::cout << "Connecting client" << std::endl;
	cl.connect();
	
	while(not(cl.is_connected())){
		std::cout << ".";
	}
	std::cout << std::endl;
	
	// Every T seconds, publish a message to maxtopic/periodic/collatz
	mqtt::topic top(cl,enc_topic);
	int nn{2};
	int ii{nn};
	auto t_i{std::chrono::steady_clock::now()};
	while(true){
		const auto t_f{std::chrono::steady_clock::now()};
		const auto dt {
			std::chrono::duration_cast<std::chrono::seconds>(t_f-t_i)
			};
		if(5 <=  dt.count()){
			t_i=t_f;
			if(ii == 1){
				nn++;
				ii=nn;
			} else {
				if(ii%2){
					ii = 3*ii+1;
				} else{
					ii /= 2;
				}
			}
			std::cout << ii << std::endl;
			const std::string enc_ii{
				encrypt_string_AES(
					keygen,
					std::to_string(ii)
				)
			};
			top.publish(
				std::move(std::to_string(ii))
			);
		}
	}
	
	std::cout << "Disconnecting client" << std::endl;
	cl.disconnect();
	
	std::cout << "Client is currently ";
	if(not(cl.is_connected())) std::cout << "not ";
	std::cout << "connnected" << std::endl;
	
	
	return(0);
}
