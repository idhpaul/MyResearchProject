#include <iostream>

#include "ClientSocket.h"

int main(void)
{
	/*
		#1 Default constructor(for localhost)
	*/
	//make socket class and need 10 sockets(10 is default)
	ClientSocket client_socket;
	// Default IP(127.0.0.1) and 10 socket ports for arg
	client_socket.socket_init({ 8090,8091,8092,8093,8094,8095,8096,8097,8098,8099 });

	/*
		#2 User constructor(input server ip)
	*/
	//make socket class and need 10 sockets(10 is default)
	ClientSocket client_socket;
	// Declaration IP(192.168.0.5) and 10 socket ports for arg
	client_socket.socket_init("192.168.0.5",{ 8090,8091,8092,8093,8094,8095,8096,8097,8098,8099 });

	/*
		#3 User constructor(for localhost)
	*/
	//make socket class and need 3 sockets
	ClientSocket client_socket(3);
	// Default IP(127.0.0.1) and 3 socket ports for arg
	client_socket.socket_init({ 8090,8091,8092 });

	/*
		#4 User constructor(input server ip)
	*/
	//make socket class and need 3 sockets
	ClientSocket client_socket(3);
	// 3 socket ports for arg
	client_socket.socket_init("127.0.0.1", { 8090,8091,8092 });

	return 0;
}