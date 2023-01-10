#include "socket_manager.h"

SocketManager::SocketManager(int i_MaxSockets) {
	m_MaxSockets = i_MaxSockets;
	m_Sockets = new Socket*[m_MaxSockets]();
}

SocketManager::~SocketManager() {
	if (m_SocketCount > 0) {
		for (int i = 0; i < m_MaxSockets; i++) {
			delete m_Sockets[i];
		}
	}

	delete[] m_Sockets;
}

bool SocketManager::addSocket(Socket* const i_Socket) {
	bool added = false;
	
	if (m_SocketCount < m_MaxSockets) {
		for (int i = 0; i < m_MaxSockets && !added; i++) {
			if (m_Sockets[i] == nullptr) {
				m_Sockets[i] = i_Socket;
				m_SocketCount++;
				added = true;
			}
		}
	}

	return added;
}

void SocketManager::removeSocket(SOCKET i_ID) {
	bool removed = false;

	if (m_SocketCount > 0) {
		for (int i = 0; i < m_MaxSockets && !removed; i++) {
			if (m_Sockets[i] != nullptr && m_Sockets[i]->getID() == i_ID) {
				delete m_Sockets[i];
				m_Sockets[i] = nullptr;
				m_SocketCount--;
				removed = true;
			}
		}
	}
}

int SocketManager::performSelection() {
	int nfd;

	FD_ZERO(&m_AwaitingReceive);
	for (int i = 0; i < m_MaxSockets; i++)
	{
		if (m_Sockets[i] != nullptr && ((m_Sockets[i]->getState() == Socket::State::LISTEN) || (m_Sockets[i]->getState() == Socket::State::RECEIVE)))
			FD_SET(m_Sockets[i]->getID(), &m_AwaitingReceive);
	}

	FD_ZERO(&m_AwaitingSend);
	for (int i = 0; i < m_MaxSockets; i++)
	{
		if (m_Sockets[i] != nullptr && ((m_Sockets[i]->getState() == Socket::State::SEND) || (m_Sockets[i]->getState() == Socket::State::SEND_AND_TERMINATE)))
			FD_SET(m_Sockets[i]->getID(), &m_AwaitingSend);
	}

	nfd = select(0, &m_AwaitingReceive, &m_AwaitingSend, NULL, NULL);

	return nfd;
}

vector<Socket*> SocketManager::getAwaitingReceive() {
	vector<Socket*> socketsAwaitingReceive;

	for (int i = 0; i < m_MaxSockets; i++) {
		if (m_Sockets[i] != nullptr && FD_ISSET(m_Sockets[i]->getID(), &m_AwaitingReceive)) {
			socketsAwaitingReceive.push_back(m_Sockets[i]);
		}
	}

	return socketsAwaitingReceive;
}

vector<Socket*> SocketManager::getAwaitingSend() {
	vector<Socket*> socketsAwaitingSend;

	for (int i = 0; i < m_MaxSockets; i++) {
		if (m_Sockets[i] != nullptr && FD_ISSET(m_Sockets[i]->getID(), &m_AwaitingSend)) {
			socketsAwaitingSend.push_back(m_Sockets[i]);
		}
	}

	return socketsAwaitingSend;
}