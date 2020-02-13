#include "mslm_rtp.h"
#include "rtp_packet.h"

#include <cstring>
#include <iostream>

void MYRTP::MYRTP_parsing(const char* packet_data, int packet_len, char* payload_data, int* payload_data_size)
{
    std::string str(packet_data, packet_len);
	std::istringstream is(str);
		
	kaitai::kstream ks(&is);
	rtp_packet_t data(&ks);

	*payload_data_size = static_cast<int>(data.data().size());

	std::cout << "Video Packet sequence number : " << data.sequence_number() << std::endl;
	std::cout << "Video Packet timestamp : " << data.timestamp() << std::endl;
	std::cout << "Video Packet payload data size : " << *payload_data_size << std::endl;

	memcpy(payload_data, data.data().c_str(), *payload_data_size);
}