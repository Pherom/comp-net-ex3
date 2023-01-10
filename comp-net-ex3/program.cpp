#include "server.h"

void main() {
	Server server(60, 3000);
	server.start(8080, 5);
}