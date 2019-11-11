#include <iostream>
#include <vector>
#include <mqtt/async_client.h>
#include <mqtt/topic.h>
#include <mqtt/callback.h>
#include <wiringPi.h>
#include <chrono>

const std::string broker_address{"130.179.196.54:1883"};

class callback : public virtual mqtt::callback{
	void message_arrived(mqtt::const_message_ptr msg) override {
		std::cout << "Message arrived" << std::endl;
		std::cout << "\ttopic:\t'" << msg->get_topic() << "'" << std::endl;
		std::cout << "\tpayload:\t'" << msg->to_string() << "'\n" << std::endl;
	};
};

int main(int argc,char** argv){
	std::cout << "Running..." << std::endl;

	std::cout << "Constructing client..." << std::endl;
	mqtt::async_client cl(broker_address,"hughsonPiReader");
	callback cb;
	cl.set_callback(cb);
	
	std::cout << "Connecting client..." << std::endl;
	cl.connect();
	while(not(cl.is_connected()));
	std::cout << "Connected!" << std::endl;

	//mqtt::topic top(cl,"maxtopic/periodic/collatz");
	cl.subscribe("maxtopic/periodic/collatz",1);
		
	while (std::tolower(std::cin.get()) != 'q');
	
	std::cout << "Disconnecting client";
	cl.disconnect();
	while(cl.is_connected()) std::cout << ".";
	std::cout << std::endl;
	
	
	
	return(0);
}
