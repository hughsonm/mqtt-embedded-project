#include <iostream>
#include <vector>
#include <mqtt/client.h>



int main(int argc,char** argv){
	auto period{std::chrono::seconds(5)};
	mqtt::client cl("130.179.196.54:1883","",10,"data-persist");
	std::cout << "Running..." << std::endl;
	return(0);
}
