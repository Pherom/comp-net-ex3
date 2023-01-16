#pragma once

#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include "socket.h"
#include "socket_manager.h"
#include "http_request.h"
#include "http_response.h"
#include "http_request_manager.h"
#include "http_response_builder.h"

using namespace std;

class Server {

private:
	WSAData m_WSAData;
	Socket* m_ListenSocket = nullptr;
	SocketManager m_SocketManager;
	int m_DefaultSocketBufferLength;
	HttpRequestManager m_RequestManager;

	void initializeWinsock();
	void createListenSocket();
	void bindListenSocket(int i_Port);
	void startListening(int i_Backlog);
	void handleDeadSockets();
	void acceptConnection(Socket* socket);
	void receiveMessage(Socket* socket);
	void handleSocketsAwaitingReceive(int& nfd);
	void sendMessage(Socket* socket);
	void handleSocketsAwaitingSend(int& nfd);
	void acceptAndHandleConnections();

public:
	Server(int i_MaxSockets, int i_DefaultSocketBufferLength);
	~Server();
	void start(int i_Port, int i_Backlog);
	void shutdown();

};
