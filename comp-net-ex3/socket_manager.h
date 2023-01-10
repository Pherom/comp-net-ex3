#pragma once

#include "socket.h"
#include <vector>

class SocketManager {

private:
	int m_MaxSockets;
	int m_SocketCount = 0;
	Socket** m_Sockets;
	fd_set m_AwaitingReceive;
	fd_set m_AwaitingSend;

public:
	SocketManager(int i_MaxSockets);
	~SocketManager();
	bool addSocket(Socket* const i_Socket);
	void removeSocket(SOCKET i_ID);
	int performSelection();
	vector<Socket*> getAwaitingReceive();
	vector<Socket*> getAwaitingSend();

};