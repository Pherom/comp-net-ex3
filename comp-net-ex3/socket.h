#pragma once

#include <WinSock2.h>
#include <string>

using namespace std;

class Socket {

public:
	enum class State {
		LISTEN, RECEIVE, SEND, SEND_AND_TERMINATE
	};

private:
	SOCKET m_ID;
	State m_State;
	string m_IncomingMessage;
	string m_OutgoingMessage;

public:
	Socket(SOCKET i_ID, State i_InitialState);
	SOCKET getID() const;
	State getState() const;
	void setState(const State& i_State);
	string getIncomingMessage() const;
	void setIncomingMessage(const string& i_NewMessage);
	void appendToIncomingMessage(char* i_AdditionStr);
	string getOutgoingMessage() const;
	void setOutgoingMessage(const string& i_NewMessage);
	void appendToOutgoingMessage(const string& i_AdditionStr);

};
