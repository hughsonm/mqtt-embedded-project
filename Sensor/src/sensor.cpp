#include <iostream>
#include <fstream>
#include <vector>
#include <mqtt/async_client.h>
#include <mqtt/topic.h>
#include <wiringPi.h>
#include <chrono>
#include <ratio>
#include <openssl/aes.h>
#include "crypt.h"


const std::string broker_address{"130.179.196.54:1883"};
const std::string keygen_filename{"/home/pi/aes_key_gen.txt"};
const std::string raw_topic{"maxtopic/periodic/collatz"};

class Sensor{
	double ym2{0.0};
	double ym1{0.0};
	double y{0.0};
	double xm2{1.0};
	int count{0};
public:
	double read_normalized_value();
};

double Sensor::read_value(){
	y = 0.9*(1.905*ym1-ym2+xm2);
	ym2 = ym1;
	ym1 = y;
	xm2 = 0.0;
	if(100<++count){
		xm2 = 1.0;
		count = 0;
	}
	return(y);
}


int main(int argc,char** argv){
	
	std::ifstream reader;
	reader.open(keygen_filename,std::ifstream::in);
	std::string keygen;
	reader >> keygen;
	reader.close();
		
	const std::string enc_topic{
		encrypt_topic_AES(keygen,"hughson",raw_topic)
	};
	
	std::cout << enc_topic <<"\n";
	
	std::cout << "Running...\n"
	
	std::cout << "Constructing client...\n";
	mqtt::async_client cl(broker_address,"hpSensor");
	
	std::cout << "Client is currently ";
	if(not(cl.is_connected())) std::cout << "not ";
	std::cout << "connnected.\n";
	
	std::cout << "Connecting client\n";
	cl.connect();
	
	while(not(cl.is_connected()));
	std::cout << "Client is now connected\n";
	
	mqtt::topic top(cl,enc_topic);
	std::cout << "Broadcasting on topic " << enc_topic << "\n";
	Sensor meter;
	auto t_i{std::chrono::steady_clock::now()};
	
	while(true){
		const auto t_f{std::chrono::steady_clock::now()};
		const auto dt {
			std::chrono::duration_cast<std::chrono::seconds>(t_f-t_i)
			};
		if(5 <=  dt.count()){
			t_i = t_f;
			const double reading{meter.read_value()};
			const std::string enc_reading{
				encrypt_string_AES(keygen,std::to_string(reading))
			};
			std::cout << "Sending new reading:\n";
			std::cout << "\tReading:    " << reading << "\n";
			std::cout << "\tEncrypted:  " << enc_reading << "\n";
			top.publish(
				std::move(enc_reading)
			);
		}
	}
	
	std::cout << "Disconnecting client\n";
	cl.disconnect();
	
	std::cout << "Client is currently ";
	if(not(cl.is_connected())) std::cout << "not ";
	std::cout << "connnected\n";
	
	return(0);
}
