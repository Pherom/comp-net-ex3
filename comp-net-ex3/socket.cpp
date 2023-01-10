#include "socket.h"

Socket::Socket(SOCKET i_ID, State i_InitialState) {
	m_ID = i_ID;
	m_State = i_InitialState;
}

SOCKET Socket::getID() const {
	return m_ID;
}

Socket::State Socket::getState() const {
	return m_State;
}

void Socket::setState(const State& i_State) {
	m_State = i_State;
}

string Socket::getIncomingMessage() const {
	return m_IncomingMessage;
}

void Socket::appendToIncomingMessage(char* i_AdditionStr) {
	m_IncomingMessage.append(i_AdditionStr);
}

void Socket::setIncomingMessage(const string& i_NewMessage) {
	m_IncomingMessage = i_NewMessage;
}

string Socket::getOutgoingMessage() const {
	return m_OutgoingMessage;
}

void Socket::setOutgoingMessage(const string& i_NewMessage) {
	m_OutgoingMessage = i_NewMessage;
}

void Socket::appendToOutgoingMessage(const string& i_AdditionStr) {
	m_OutgoingMessage.append(i_AdditionStr);
}