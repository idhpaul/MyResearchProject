#ifndef MY_RTP_H
#define MY_RTP_H

class MYRTP {
        public:
		void MYRTP_parsing(const char* packet_data, int packet_len, char* payload_data, int* payload_data_size);
	private:
};

#endif