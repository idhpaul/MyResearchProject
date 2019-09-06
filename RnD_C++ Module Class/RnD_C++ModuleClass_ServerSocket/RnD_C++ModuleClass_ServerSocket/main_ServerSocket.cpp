#include <iostream>

#include "ServerSocket.h"

int main(void)
{
	/*
		#1 Default constructor
	*/
	//make socket class and need 10 sockets(10 is default)
	ServerSocket server_socket;
	// 10 socket ports for arg
	server_socket.socket_init({ 8090,8091,8092,8093,8094,8095,8096,8097,8098,8099 });

	/*
		#2 User constructor
	*/
	//make socket class and need 3 sockets
	ServerSocket server_socket(3);
	// 3 socket ports for arg
	server_socket.socket_init({ 8090,8091,8092 });

	return 0;
}