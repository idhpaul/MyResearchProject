//https://www.linuxjournal.com/content/network-programming-enet

#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define ENET_DLL
#define ENET_IMPLEMENTATION

#include "enet.h"
#include "DuplicationManager.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>
#include <memory>
#include <vector>

#define WIDTH		1920
#define HEIGHT		1080

#define HOST "localhost"
#define PORT (20020)
#define BUFFERSIZE (1000)

ENetAddress  address;
ENetHost* server;
ENetEvent  event;
ENetPacket* packet;

char    buffer[BUFFERSIZE];

int  main(int argc, char** argv)
{
    int  i;

    DUPL_RETURN Ret;
	int nv12_texture_size = WIDTH * WIDTH * 3 / 2;
	std::vector<uint8_t> nv12_texture(nv12_texture_size);

	// DXGI
	std::unique_ptr<DUPLICATIONMANAGER> DXGI_Capture_;
	DXGI_Capture_ = std::make_unique<DUPLICATIONMANAGER>();
	Ret = DXGI_Capture_->InitDupl();
	if (Ret != DUPL_RETURN_SUCCESS)
	{
		std::cout << "First Duplication Manager couldn't be initialized. (" << GetLastError() << ")" << std::endl;
		return -1;
	}

    if (enet_initialize() != 0) { printf("Could not initialize enet."); return 0; }

    address.host = ENET_HOST_ANY;
    address.port = PORT;
    server = enet_host_create(&address, 32, 2, 0, 0);
    if (server == NULL) { printf("Could not start server.\n"); return 0; }

    while (1) {
        while (enet_host_service(server, &event, 33) > 0) {
            switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT: break;
            case ENET_EVENT_TYPE_RECEIVE:
                if (event.peer->data == NULL) {
                    event.peer->data = malloc(strlen((char*)event.packet->data) + 1);
                    strcpy((char*)event.peer->data, (char*)event.packet->data);

                    sprintf_s(buffer, sizeof(buffer), "%s has connected\n", (char*)event.packet->data);
                    packet = enet_packet_create(buffer,
                        strlen(buffer) + 1, 0);
                    enet_host_broadcast(server, 1, packet);
                    enet_host_flush(server);
                }
                else {
                    // Capture Frame
                    // 프레임 가져오기(BGRA, GPU)
                    Ret = DXGI_Capture_->GetFrame();
                    if (Ret == DUPL_RETURN_GET_FRAME_TIME_OUT)
                    {
                        std::cout << "Timeout" << std::endl;
                    }
                    else if (Ret == DUPL_RETURN_SUCCESS)
                    {
                        // 정상적으로 프레임을 가져온 상태

                        // NV12 색상변환
                        Ret = DXGI_Capture_->TransformFrame(nv12_texture);
                        if (Ret != DUPL_RETURN_SUCCESS)
                        {
                            //  오류
                            printf("Could not transform the frame.");
                        }

                        // send frame
                    
                        ENetPacket* packet = enet_packet_create(
                            nv12_texture.data(),             // data
                            nv12_texture.size(),             // data length
                            ENET_PACKET_FLAG_RELIABLE);

                        enet_peer_send(event.peer, 0, packet);
                        enet_host_flush(server);

                        std::cout << "Send packet" << std::endl;
                    }
                    else
                    {
                        // 오류
                        printf("Could not get the frame.\n");
                        break;
                    }
                    /*sprintf_s(buffer, sizeof(buffer), "%s: %s", (char*)event.peer->data, (char*)event.packet->data);
                    packet = enet_packet_create(buffer, strlen(buffer) + 1, 0);
                    enet_peer_send(server->peers, 0, packet);
                    enet_host_flush(server);*/
                }
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                sprintf_s(buffer, sizeof(buffer), "%s has disconnected.", (char*)event.peer->data);
                packet = enet_packet_create(buffer, strlen(buffer) + 1, 0);
                enet_host_broadcast(server, 1, packet);
                free(event.peer->data);
                event.peer->data = NULL;
                break;
            default: printf("Tick tock.\n"); break;
            }
        }
        //while (enet_host_service(server, &event, 1000) > 0) {
        //    switch (event.type) {
        //    case ENET_EVENT_TYPE_NONE:
        //        printf("type none\n");
        //        break;
        //    case ENET_EVENT_TYPE_CONNECT: 
        //        printf("type connect\n");

        //        printf("connect : ", event.data);

        //        while (true)
        //        {
        //            

        //            Sleep(1000);
        //            
        //        }

        //        break;
        //    case ENET_EVENT_TYPE_DISCONNECT:
        //        printf("type disconect\n");
        //        break;
        //    case ENET_EVENT_TYPE_RECEIVE:
        //        printf("type receive\n");
        //        break;
        //    case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
        //        printf("type disconnect timeout\n");
        //        break;
        //    default: printf("Tick tock.\n"); break;
        //    }
        //}
    }

    enet_host_destroy(server);
    enet_deinitialize();
}