// You have to .pem(¿Œ¡ıº≠) file

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <iostream>

using namespace boost;

class SyncSSLClient {
public:
	SyncSSLClient(const std::string& raw_ip_address,
		unsigned short port_num) :
		m_ep(asio::ip::address::from_string(raw_ip_address),
			port_num),
		m_ssl_context(asio::ssl::context::tls_client),
		m_ssl_stream(m_ios, m_ssl_context)
	{
		// Set ssl context verification file    
		m_ssl_context.load_verify_file("./user.pem");

		// Set verification mode and designate that 
		// we want to perform verification.
		m_ssl_stream.set_verify_mode(asio::ssl::verify_peer);

		// Set verification callback. 
		m_ssl_stream.set_verify_callback([this](
			bool preverified,
			asio::ssl::verify_context& context)->bool {
			return on_peer_verify(preverified, context);
		});
	}

	void connect() {
		// Connect the TCP socket.
		m_ssl_stream.lowest_layer().connect(m_ep);

		// Perform the SSL handshake.
		m_ssl_stream.handshake(asio::ssl::stream_base::client);
	}

	void close() {
		// We ignore any errors that might occur
		// during shutdown as we anyway can't
		// do anything about them.
		boost::system::error_code ec;

		m_ssl_stream.shutdown(ec); // Shutown SSL.

		// Shut down the socket.
		m_ssl_stream.lowest_layer().shutdown(
			boost::asio::ip::tcp::socket::shutdown_both, ec);

		m_ssl_stream.lowest_layer().close(ec);
	}

	std::string emulate_long_computation_op(
		unsigned int duration_sec) {

		// std::string request = "EMULATE_LONG_COMP_OP "
		// 	+ std::to_string(duration_sec)
		// 	+ "\n";

		// send_request(request);
		return receive_response();
	};

private:
	bool on_peer_verify(bool preverified,
		asio::ssl::verify_context& context)
	{
		// Here the certificate should be verified and the
		// verification result should be returned.
		char subject_name[256];
		X509* cert = X509_STORE_CTX_get_current_cert(context.native_handle());
		X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
		std::cout << "Verifying " << subject_name << "\n";

		return preverified;

	}

	void send_request(const std::string& request) {
		asio::write(m_ssl_stream, asio::buffer(request));
	}

	std::string receive_response() {
		asio::streambuf buf;
		asio::read_until(m_ssl_stream, buf, '\n');

		std::string response;
		std::istream input(&buf);
		std::getline(input, response);

		return response;
	}

private:
	asio::io_service m_ios;
	asio::ip::tcp::endpoint m_ep;

	asio::ssl::context m_ssl_context;
	asio::ssl::stream<asio::ip::tcp::socket> m_ssl_stream;
};

int main()
{
	const std::string raw_ip_address = "192.168.81.1";
	const unsigned short port_num = 9090;

	int i = 0;

	try {
		SyncSSLClient client(raw_ip_address, port_num);

		// Sync connect.
		client.connect();

		while (true)
		{
			if (i > 10)
			{
				break;
			}

			std::string response = client.emulate_long_computation_op(10);

			std::cout << "Response received: " << response << std::endl;

			i++;
		}


		// Close the connection and free resources.
		client.close();
	}
	catch (system::system_error &e) {
		std::cout << "Error occured! Error code = " << e.code()
			<< ". Message: " << e.what();

		return e.code().value();
	}

	return 0;
}
