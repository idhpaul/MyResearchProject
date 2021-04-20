//https://www.linuxjournal.com/content/network-programming-enet

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define ENET_DLL
#define ENET_IMPLEMENTATION

#include "Render_D11.h"
#include "enet.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>
#include <memory>
#include <thread>


#define HOST "192.168.0.202"
//#define HOST "210.183.46.213"
#define PORT (20020)
#define BUFFERSIZE (1000)
char  buffer[BUFFERSIZE];

ENetHost* client;
ENetAddress address;
ENetEvent   event;
ENetPeer* peer;
ENetPacket* packet;

int  main(int argc, char** argv) {
    int connected = 0;

    if (argc != 1) { printf("Usage: client username\n"); exit(1); }
    if (enet_initialize() != 0) { printf("Could not initialize enet.\n"); return 0; }

    client = enet_host_create(NULL, 32,2,0,0);
    if (client == NULL) { printf("Could not create client.\n"); return 0; }

    enet_address_set_host(&address, HOST);
    address.port = PORT;

    peer = enet_host_connect(client, &address, 2, 0);
    if (peer == NULL) { printf("Could not connect to server\n"); return 0; }

    if (enet_host_service(client, &event, 1000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT) {
        printf("Connection to %s succeeded.\n", HOST);
        connected++;
       
        strncpy_s(buffer, _countof(buffer), "hello server", BUFFERSIZE);
        packet = enet_packet_create(buffer, strlen(buffer) + 1, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, packet);
    }
    else {
        enet_peer_reset(peer);
        printf("Could not connect to %s.\n", HOST);
        return 0;
    }


    //! SDL Init
    std::shared_ptr<RenderD11> RenderObj = std::make_shared<RenderD11>(1280, 720, 1920, 1080);
    RenderObj->Initialize(RenderDriverType::DXD9);

    std::thread th1([&]()
        {
            while (1) {
                while (enet_host_service(client, &event, 100) > 0) {
                    switch (event.type) {
                    case ENET_EVENT_TYPE_RECEIVE:

                        printf("do render \n");
                        printf("recv data size : %d\n", event.packet->dataLength);
                        RenderObj->Render((uint8_t*)event.packet->data, 1920, 1080);

                        break;
                    case ENET_EVENT_TYPE_DISCONNECT:
                        connected = 0;
                        printf("You have been disconnected.\n");
                        return 2;
                    }
                }

                if (connected) {
                    /*printf("Input: ");
                    gets_s(buffer, sizeof(buffer));*/

                    memcpy(buffer, "requset frame data", 19);
                    printf("requset frame data\n");

                    if (strlen(buffer) == 0) { continue; }
                    if (strncmp("q", buffer, BUFFERSIZE) == 0) {
                        connected = 0;
                        enet_peer_disconnect(peer, 0);
                        continue;
                    }

                    packet = enet_packet_create(buffer, strlen(buffer) + 1, ENET_PACKET_FLAG_RELIABLE);
                    enet_peer_send(peer, 0, packet);
                }
            }
        });

    RenderObj->Procedd_poll(60);

    enet_deinitialize();
}