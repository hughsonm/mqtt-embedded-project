#include <iostream>
#include <vector>
#include <mqtt/async_client.h>
#include <mqtt/topic.h>
#include <wiringPi.h>
#include <chrono>
#include <ratio>

const std::string broker_address{"130.179.196.54:1883"};

int main(int argc,char** argv){
	
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
	mqtt::topic top(cl,"maxtopic/periodic/collatz");
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
			top.publish(std::move(std::to_string(ii)));
		}
	}
	
	std::cout << "Disconnecting client" << std::endl;
	cl.disconnect();
	
	std::cout << "Client is currently ";
	if(not(cl.is_connected())) std::cout << "not ";
	std::cout << "connnected" << std::endl;
	
	
	return(0);
}
