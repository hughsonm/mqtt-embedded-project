#ifndef MQTT_CRYPT_H
#define MQTT_CRYPT_H
#include <string>
#include <vector>



std::vector<std::vector<unsigned char> > make_AES_blocks(
	const std::string& source
);

std::string encrypt_string_AES(
	std::string keygen,
	std::string input
);

std::string encrypt_topic_AES(
	std::string keygen,
	std::string topic_prefix,
	std::string topic_suffix
);

std::string decrypt_string_AES(
	std::string keygen,
	std::string input
);
#endif
