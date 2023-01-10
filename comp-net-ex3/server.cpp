#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "server.h"
#include <iostream>

void Server::initializeWinsock() {
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &m_WSAData))
	{
		throw "Server: Error at WSAStartup()";
	}
}

void Server::createListenSocket() {
	SOCKET id = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == id)
	{
		throw "Time Server: Error at socket(): " + WSAGetLastError();
	}

	m_ListenSocket = new Socket(id, Socket::State::LISTEN);
}

void Server::bindListenSocket(int i_Port) {
	sockaddr_in serverService;
	serverService.sin_family = AF_INET;
	serverService.sin_addr.s_addr = INADDR_ANY;
	serverService.sin_port = htons(i_Port);

	if (SOCKET_ERROR == bind(m_ListenSocket->getID(), (SOCKADDR*)&serverService, sizeof(serverService)))
	{
		throw "Server: Error at bind(): " + WSAGetLastError();
	}
}

void Server::startListening(int i_Backlog) {
	if (SOCKET_ERROR == listen(m_ListenSocket->getID(), i_Backlog))
	{
		throw "Server: Error at listen():" + WSAGetLastError();
	}
	m_SocketManager.addSocket(m_ListenSocket);
}

void Server::acceptConnection(Socket* socket) {
	struct sockaddr_in from;
	int fromLen = sizeof(from);

	SOCKET msgSocketID = accept(socket->getID(), (struct sockaddr*)&from, &fromLen);
	if (INVALID_SOCKET == msgSocketID)
	{
		cout << "Server: Error at accept(): " << WSAGetLastError() << endl;
		return;
	}
	Socket* msgSocket = new Socket(msgSocketID, Socket::State::RECEIVE);
	cout << "Server: Client " << inet_ntoa(from.sin_addr) << ":" << ntohs(from.sin_port) << " is connected." << endl;

	unsigned long flag = 1;
	if (ioctlsocket(msgSocket->getID(), FIONBIO, &flag) != 0)
	{
		cout << "Server: Error at ioctlsocket(): " << WSAGetLastError() << endl;
		closesocket(msgSocket->getID());
		delete msgSocket;
	}

	else if (m_SocketManager.addSocket(msgSocket) == false)
	{
		cout << "\t\tToo many connections, dropped!\n";
		closesocket(msgSocket->getID());
		delete msgSocket;
	}
}

void Server::receiveMessage(Socket* socket) {
	char* buffer = new char[m_DefaultSocketBufferLength];
	int bytesRecv = recv(socket->getID(), buffer, m_DefaultSocketBufferLength - 1, 0);

	if (SOCKET_ERROR == bytesRecv)
	{
		cout << "Server: Error at recv(): " << WSAGetLastError() << endl;
		delete[] buffer;
		closesocket(socket->getID());
		m_SocketManager.removeSocket(socket->getID());
		return;
	}
	if (bytesRecv == 0)
	{
		delete[] buffer;
		closesocket(socket->getID());
		m_SocketManager.removeSocket(socket->getID());
		return;
	}
	else
	{
		buffer[bytesRecv] = '\0';
		socket->appendToIncomingMessage(buffer);
		cout << "Server: Recieved: " << bytesRecv << " bytes of \n\"" << buffer << "\"\nmessage.\n";
		delete[] buffer;

		int headerEndIndex = socket->getIncomingMessage().find("\r\n\r\n");
		int contentLength = 0;

		if (headerEndIndex != string::npos) {
			HttpRequest request = m_RequestManager.getRequest(socket->getIncomingMessage());
			bool hasBody = request.headerExists("Content-Length"), bodyReceived = false;
			if (hasBody) {
				contentLength = stoi(request.getHeaderValue("Content-Length"));
				bodyReceived = socket->getIncomingMessage().substr(headerEndIndex + 4).length() >= contentLength;
			}

			if (hasBody) {
				if (!bodyReceived) {
					return;
				}

				else {
					request.setMessageBody(m_RequestManager.getRequestBody(socket->getIncomingMessage(), headerEndIndex + 4, contentLength));
				}
			}

			HttpResponse response = m_RequestManager.processRequest(request);
			socket->setIncomingMessage(socket->getIncomingMessage().erase(0, headerEndIndex + 4 + contentLength));
			socket->appendToOutgoingMessage(response.toString());
			if (!request.headerExistsWithValue("Connection", "close")) {
				socket->setState(Socket::State::SEND);
			}
			else {
				socket->setState(Socket::State::SEND_AND_TERMINATE);
			}

		}
	}
}

void Server::handleSocketsAwaitingReceive(int& nfd) {
	for (Socket* socket : m_SocketManager.getAwaitingReceive()) {
		nfd--;
		switch (socket->getState())
		{
		case Socket::State::LISTEN:
			acceptConnection(socket);
			break;

		case Socket::State::RECEIVE:
			receiveMessage(socket);
			break;
		}
	}
}

void Server::sendMessage(Socket* socket) {
	string outgoingMessage = socket->getOutgoingMessage();
	int bytesSent = send(socket->getID(), outgoingMessage.c_str(), outgoingMessage.size(), 0);
	if (SOCKET_ERROR == bytesSent)
	{
		cout << "Server: Error at send(): " << WSAGetLastError() << endl;
		return;
	}

	cout << "Server: Sent: " << bytesSent << "\\" << outgoingMessage.size() << " bytes of \n\"" << outgoingMessage << "\"\nmessage.\n";

	if (socket->getState() == Socket::State::SEND) {
		socket->setOutgoingMessage(string());
		socket->setState(Socket::State::RECEIVE);
	}
	else if (socket->getState() == Socket::State::SEND_AND_TERMINATE) {
		closesocket(socket->getID());
		m_SocketManager.removeSocket(socket->getID());
	}
}

void Server::handleSocketsAwaitingSend(int& nfd) {
	for (Socket* socket : m_SocketManager.getAwaitingSend()) {
		nfd--;
		sendMessage(socket);
	}
}

void Server::acceptAndHandleConnections() {
	while (true)
	{
		int nfd = m_SocketManager.performSelection();

		if (nfd == SOCKET_ERROR)
		{
			throw "Server: Error at select(): " + WSAGetLastError();
		}

		handleSocketsAwaitingReceive(nfd);
		handleSocketsAwaitingSend(nfd);
	}
}

Server::Server(int i_MaxSockets, int i_DefaultSocketBufferLength) : m_SocketManager(i_MaxSockets), m_DefaultSocketBufferLength(i_DefaultSocketBufferLength) {
	initializeWinsock();
	createListenSocket();
}

Server::~Server() {
	if (!WSANOTINITIALISED) {
		if (m_ListenSocket != nullptr) {
			shutdown();
			delete m_ListenSocket;
		}
		WSACleanup();
	}
}

void Server::start(int i_Port, int i_Backlog) {
	bindListenSocket(i_Port);
	startListening(i_Backlog);
	acceptAndHandleConnections();
}

void Server::shutdown() {
	closesocket(m_ListenSocket->getID());
}