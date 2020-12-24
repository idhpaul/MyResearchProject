#include "SocketUtil.h"
#include "TCPSocket.h"
#include "Cursor.h"

#include <iostream>

#define TEST_SDL 1
#if TEST_SDL
#include "SDL.h"
#include <vector>
extern std::vector<uint8_t> image;
#endif

std::shared_ptr<TCPSocket> tcpSocket(new TCPSocket);
SOCKET clientSocketfd = -1;

#define SERVER_IP "0.0.0.0"
#define SERVER_PORT 8099

int SocketStart();
void SocketStop();

int SocketStart()
{
    bool bResult = false;

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData))
    {
        std::cerr << "Win socket start failed" << std::endl;
        WSACleanup();
    }

    tcpSocket->create();
    //SocketUtil::setReuseAddr(tcpSocket->fd());
    //SocketUtil::setReusePort(tcpSocket->fd());
    //SocketUtil::setNonBlock(tcpSocket->fd());
    //SocketUtil::setKeepAlive(tcpSocket->fd());

    bResult = tcpSocket->bind(SERVER_IP, SERVER_PORT);
    if (FALSE == bResult)
    {
        std::cerr << "main tcpSocket bind failed" << std::endl;
        std::cerr << "GetLastError : " << WSAGetLastError() << std::endl;

        SocketStop();
        return -1;
    }

    bResult = tcpSocket->listen(SOMAXCONN);
    if (FALSE == bResult)
    {
        std::cerr << "main tcpSocket listen failed" << std::endl;
        std::cerr << "GetLastError : " << WSAGetLastError() << std::endl;

        SocketStop();
        return -1;
    }

    std::cout << "Listening........" << std::endl;

    clientSocketfd = tcpSocket->accept();
    if (clientSocketfd <= 0)
    {
        std::cerr << "main tcpSocket accept failed" << std::endl;
        std::cerr << "GetLastError : " << WSAGetLastError() << std::endl;

        SocketStop();
        return -1;
    }

    std::cout << "Accept OK!" << std::endl;
}

void SocketStop()
{
    SocketUtil::close(clientSocketfd);
    tcpSocket->close();

    // Unload Windows Socket DLL
    WSACleanup();
}


int main()
{

    SocketStart();

#if TEST_SDL
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Surface* surface = NULL;
    SDL_Cursor* cursor = NULL;
    SDL_bool error = SDL_TRUE;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        goto exit;
    }
    if (SDL_CreateWindowAndRenderer(640, 480, 0, &window, &renderer) < 0) {
        goto exit;
    }
    /*surface = SDL_LoadBMP((1 < argc) ? argv[1] : "cursor.bmp");
    if (!surface) {
        goto exit;
    }*/
    /*cursor = SDL_CreateColorCursor(surface, 0, 0);
    if (!cursor) {
        goto exit;
    }*/
#endif


#if TEST_SDL
    //SDL_SetCursor(cursor);
    
    int ret;
    while (SDL_TRUE)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type) {
            case SDL_MOUSEBUTTONUP:
            case SDL_QUIT:
                error = SDL_FALSE;
                goto exit;
            }
        }

        CursorInfo();

        // TODO : What is diff send return values
        ret = ::send(clientSocketfd, (char*)image.data(), 4096, 0);
        if (ret > 0)
        {
            std::cout << "Send OK - sendsize: " << ret << std::endl;
        }
        else if (ret < 0)
        {
            std::cout << "Send ret < 0" << std::endl;

            int error = WSAGetLastError();

            if (error == WSAEWOULDBLOCK || error == WSAEINPROGRESS || error == 0)
                ret = 0;

            error = SDL_FALSE;
            goto exit;
        }
        else
        {
            std::cout << "Send ret = 0" << std::endl;
            error = SDL_FALSE;
            goto exit;
        }

        //SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom((void*)image.data(), 32, 32, 32, 32*4, SDL_PIXELFORMAT_ARGB32);

        //cursor = SDL_CreateColorCursor(surface, 0, 0);
        //if (!cursor) {
        //    goto exit;
        //}
        //SDL_SetCursor(cursor);

        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

        SDL_RenderPresent(renderer);
        Sleep(1000);
    }
#else
    while (true)
    {
        CursorInfo();
        Sleep(1000);
    }
#endif


	

#if TEST_SDL
    exit :
    if (error) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError());
    }
    if (cursor) {
        SDL_FreeCursor(cursor);
    }
    if (surface) {
        SDL_FreeSurface(surface);
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
    return error;
#endif

	return 0;
}