#include "select_server.hpp"
#include <iostream>

using namespace std;

int main()
{
	// SelectServer s;
	// s.InitSelectServer();
	// s.Loop();
	SelectServer* s = new SelectServer();
	s->InitSelectServer();
	s->Loop();
	return 0;
}
