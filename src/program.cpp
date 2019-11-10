#include <iostream>
#include <vector>
#include <mqtt/client.h>
#include <wiringPi.h>
#include <chrono>

const std::string broker_address{"130.179.196.54:1883"};

int main(int argc,char** argv){
	auto period{std::chrono::seconds(5)};
	std::cout << "Running..." << std::endl;
	
	std::cout << "Constructing client..." << std::endl;
	mqtt::client cl(broker_address,"hughsonPi");
	
	std::cout << "Client is currently ";
	if(not(cl.is_connected())) std::cout << "not ";
	std::cout << "connnected" << std::endl;
	
	std::cout << "Connecting client" << std::endl;
	cl.connect();
	
	std::cout << "Client is currently ";
	if(not(cl.is_connected())) std::cout << "not ";
	std::cout << "connnected" << std::endl;
	
	if(cl.is_connected()){
		// Subscribe to something.
		cl.subscribe("maxtopic/+");
		// Then unsubscribe.
		cl.unsubscribe("maxtopic/+");
	}
	
	std::cout << "Disconnecting client" << std::endl;
	cl.disconnect();
	
	std::cout << "Client is currently ";
	if(not(cl.is_connected())) std::cout << "not ";
	std::cout << "connnected" << std::endl;
	
	
	return(0);
}
