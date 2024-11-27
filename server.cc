/*
 * server.C
 *
 *  Created on: 11.09.2019
 *      Author: aml
 */
#include <cstdio> // standard input and output library
#include <cstdlib> // this includes functions regarding memory allocation
#include <cstring> // contains string functions
#include <cerrno> //It defines macros for reporting and retrieving error conditions through error codes
#include <ctime> //contains various functions for manipulating date and time

#include <unistd.h> //contains various constants
#include <sys/types.h> //contains a number of basic derived types that should be used whenever appropriate
#include <arpa/inet.h> // defines in_addr structure
#include <sys/socket.h> // for socket creation
#include <netinet/in.h> //contains constants and structures needed for internet domain addresses

#include "SIMPLESOCKET.h"

class ServerShip: public TCPserver
{
public:
	ServerShip(int portNmb, int bSize);
	~ServerShip();

private:
	ServerShip();

protected:

string myResponse(string input);
};




int main(){
	srand(time(nullptr));
	ServerShip srv(2023,25);
	srv.run();
}

string ServerShip::myResponse(string input)
{
	if (0 == input.compare(0,3,"ABC"))
	{
		return string("xyz");
	}


	return string("#"+input+"#");
}


ServerShip::~ServerShip()
{

}


ServerShip::ServerShip(int portNmb, int bSize): TCPserver(portNmb, bSize)
{
	//Spielfeldinit.
}

//Weiter Schritte:

// CLient mit Strategie ausstatten, ob getroffen oder nicht
// Server beinhaltet das Spielfeld
// Protokoll(Kommunikation):
// Serverkommand | Antwort | Kommentar (Fehlerfälle) |
// undefined string | ERROR | ...
// Koordinatenbereich eingehalten | SPezifisch / ERROR   