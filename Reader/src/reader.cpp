#include <iostream>
#include <vector>
#include <mqtt/async_client.h>
#include <mqtt/topic.h>
#include <mqtt/callback.h>
#include <wiringPi.h>
#include <chrono>
#include <fstream>
#include "crypt.h"

class callback : public virtual mqtt::callback{
	std::string _kg;
	void message_arrived(mqtt::const_message_ptr msg) override {
		std::cout << "Message arrived\n";
		std::cout << "\ttopic:\t'" << msg->get_topic() << "'\n";
		const std::string enc_payload{msg->to_string()};
		std::cout << "\tpayload:\t'" << enc_payload << "'\n";
		const std::string dec_payload{
			decrypt_string_AES(_kg,enc_payload)
		};
		std::cout << "\tmessage:\t" << dec_payload << "\n";
	};
public:
	callback(std::string kg){_kg = kg;};
};

const std::string broker_address{"130.179.196.54:1883"};
const std::string keygen_filename{"/home/pi/aes_key_gen.txt"};
const std::string raw_topic{"maxtopic/periodic/secret"};

int main(int argc,char** argv){
	
	std::ifstream reader;
	reader.open(keygen_filename,std::ifstream::in);
	std::string keygen;
	reader >> keygen;
	reader.close();
	
	const std::string enc_topic{
		encrypt_topic_AES(keygen,"hughson",raw_topic)
	};
	
	std::cout << "Running...\n";
	std::cout << "Constructing client...\n";
	mqtt::async_client cl(broker_address,"hpReader");
	callback cb(keygen);
	cl.set_callback(cb);
	
	std::cout << "Connecting client...\n";
	cl.connect();
	while(not(cl.is_connected()));
	std::cout << "Connected!\n";

	std::cout << "Listening to topic " << enc_topic << "\n";
	cl.subscribe(enc_topic,1);
		
	while (std::tolower(std::cin.get()) != 'q');
	
	std::cout << "Disconnecting client";
	cl.disconnect();
	while(cl.is_connected()) std::cout << ".";
	std::cout << std::endl;
		
	return(0);
}
