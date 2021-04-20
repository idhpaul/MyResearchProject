//https://www.linuxjournal.com/content/network-programming-enet

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define ENET_DLL
#define ENET_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <unistd.h>
#include "enet.h"

#define HOST "localhost"
#define PORT (7000)
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

    client = enet_host_create(NULL, 1, 2, 5760 / 8, 1440 / 8);
    if (client == NULL) { printf("Could not create client.\n"); return 0; }

    enet_address_set_host(&address, HOST);
    address.port = PORT;

    peer = enet_host_connect(client, &address, 2, 0);
    if (peer == NULL) { printf("Could not connect to server\n"); return 0; }

    if (enet_host_service(client, &event, 1000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT) {
        printf("Connection to %s succeeded.\n", HOST);
        connected++;
        strncpy_s(buffer, _countof(buffer), argv[1], BUFFERSIZE);
        packet = enet_packet_create(buffer, strlen(buffer) + 1, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, 0, packet);
    }
    else {
        enet_peer_reset(peer);
        printf("Could not connect to %s.\n", HOST);
        return 0;
    }

    while (1) {
        while (enet_host_service(client, &event, 1000) > 0) {
            switch (event.type) {
            case ENET_EVENT_TYPE_RECEIVE:
                puts((char*)event.packet->data); break;
            case ENET_EVENT_TYPE_DISCONNECT:
                connected = 0;
                printf("You have been disconnected.\n");
                return 2;
            }
        }

        if (connected) {
            printf("Input: ");
            gets_s(buffer, sizeof(buffer));

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

    enet_deinitialize();
}