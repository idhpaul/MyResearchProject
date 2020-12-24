#include "SocketUtil.h"
#include "TCPSocket.h"

#include "SDL.h"

#include <iostream>
#include <thread>
#include <vector>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//The window we'll be rendering to
SDL_Window* gWindow = NULL;
//The window renderer
SDL_Renderer* gRenderer = NULL;
SDL_Cursor* cursor = NULL;

bool SocketStart();
void SocketStop();

#define TX_IP "192.168.0.58"
#define TX_PORT 8099

std::shared_ptr<TCPSocket> tcpSocket_cursor(new TCPSocket);

bool SocketStart()
{
	bool bResult;
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
	    std::cerr << "Win socket start failed" << std::endl;
	    WSACleanup();
	}

	tcpSocket_cursor->create();
	std::cout << "Socket Connect Start!" << std::endl;

	bResult = tcpSocket_cursor->connect(TX_IP, TX_PORT);
	if (FALSE == bResult)
	{
		std::cerr << "video connect failed" << std::endl;
		std::cerr << "GetLastError : " << WSAGetLastError() << std::endl;
		return false;
	}
	std::cout << "Video Connect OK" << std::endl;
}

void SocketStop()
{
	tcpSocket_cursor->close();
	WSACleanup();
}

void GetMouseCursorShape()
{
	//Socket
	int nBytesRead;

	std::vector<uint8_t>image;
	char buffer[4096];

	while (true)
	{
		int nBytesRead = recv(tcpSocket_cursor->fd(), (char*)buffer, 4096, 0);
		if (nBytesRead == -1)
		{
			// Error Handling;
			std::cout << "Error Handling Please at header recv" << std::endl;
			std::cout << "WGL : " << WSAGetLastError() << std::endl;

		}
		else if (nBytesRead == 0)
		{
			std::cout << "This socket is dead. close this socket at header recv" << std::endl;
			std::cout << "WGL : " << WSAGetLastError() << std::endl;

		}
		else // Recv OK
		{
			std::cout << "All Received!" << nBytesRead << std::endl;
		}

		SDL_FreeCursor(cursor);

		//Render Somting
		// #1 Get Mouse cursor data

		// #2 Make Mouse cursor surface
		//		use : surface = SDL_CreateRGBSurfaceWithFormatFrom((void*)image.data90, 32,32,32,32*4, SDL_PIXELFORMAT_ARGB32)
		SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom((void*)buffer, 32, 32, 32, 32 * 4, SDL_PIXELFORMAT_BGRA32);

		// #3 Create Color Cursor
		//		use : SDL_CreateColorCursor(surface,0,0)
		cursor = SDL_CreateColorCursor(surface, 0, 0);

		// #4 Set Color Cursor
		//		use : SDL_SetCursor(cursor)
		SDL_SetCursor(cursor);

		
	}

	
}

int main()
{
	//Initialization flag
	bool success = true;

	SocketStart();

	//Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

			}
		}
	}

	// Mouse Cursor Shapte Thread
	auto tMouseCursor = std::thread(GetMouseCursorShape);

	//Main loop flag
	bool quit = false;

	//Event handler
	SDL_Event e;

	//While application is running
	while (!quit)
	{
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
		}

		//Clear screen
		SDL_SetRenderDrawColor(gRenderer, 0x30, 0xAA, 0x1F, 0xFF);
		SDL_RenderClear(gRenderer);

		//Update screen
		SDL_RenderPresent(gRenderer);
		std::cout << "Rendering" << std::endl;
	}

	SocketStop();

	return 0;
}