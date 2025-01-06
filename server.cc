/*
 * server.C
 *
 *  Created on: 11.09.2019
 *      Author: aml
 */
#include <iostream> // contains standard input output functionalities
#include <cstdio>   // standard input and output library
#include <cstdlib>  // this includes functions regarding memory allocation
#include <cstring>  // contains string functions
#include <cerrno>   // It defines macros for reporting and retrieving error conditions through error codes
#include <ctime>    // contains various functions for manipulating date and time

#include <unistd.h>     // contains various constants
#include <sys/types.h>  // contains a number of basic derived types that should be used whenever appropriate
#include <arpa/inet.h>  // defines in_addr structure
#include <sys/socket.h> // for socket creation
#include <netinet/in.h> // contains constants and structures needed for internet domain addresses

#include "SIMPLESOCKET.h"
#include "TASK3.h"
#include <sstream>
#include <vector>
#include <string>

class ServerShip: public TCPserver // the class "ServerShip" inherits public aspects of TCPserver and contains information of the game board
{
public:
    ServerShip(int portNmb, int bSize); // constructor for initializing the Server
	~ServerShip();  // destructor for closing the server

private:
	ServerShip(); // private constructor which is not used
    std::vector<std::vector<char>> Board; // 2D vector for the board
    int m_remainingShips = 0; // contains the number of remaining ships

    void initializeBoard(int SizeX = 10, int SizeY = 10); // method for initializing the game board
    void placeShips(); // method which places the ships on the board
    bool canPlaceShip(int x, int y, int size, bool horizontal); // method which checks if the placement of the ship is valid or invalid
    void placeShip(int x, int y, int size, bool horizontal); // method which places a singular ship
    std::string processShot(int x, int y); // method for shooting
    bool isShipDestroyed(int x, int y); // method which contains data if the ship is destroyed
    std::string myResponse(const std::string &input) override; // method for server-client interaction
};


int main(){
	srand(time(nullptr)); // initializes a random generator 
	ServerShip srv(2022,25); // creates a ServerShip object with port number and puffer size 
	srv.run(); // calls the run function of the ServerShip object
}


std::string ServerShip::myResponse(const std::string &input) {
        if (input.find("SHOT") == 0) { // checks if the command "SHOT" is part of the "&input" string
            int x, y;
            sscanf(input.c_str(), "SHOT %d %d", &x, &y); // checks 
            return processShot(x, y);
        }else{
            return "GOODBYE";
            }
       return "INVALID_COMMAND";
    }

ServerShip::ServerShip(int portNmb, int bSize): TCPserver(portNmb, bSize) // constructor of the ServerShip class which initializes server and board
{
    srand(static_cast<unsigned>(time(nullptr))); // initializes random generator
    initializeBoard(); // initializes board
	std::cout << "Server initialized on port " << portNmb << std::endl; // output to check if the server initiation was successfull
}

ServerShip::~ServerShip(){} // destructor of the ServerShip class, which outputs a shutdown text

void ServerShip::initializeBoard(int SizeX, int SizeY) // initialization of the board with sizes of x and y
{
    Board.resize(SizeY, std::vector<char>(SizeX, '.')); // sets the board size to SizeX and SizeY and fills with "."
    placeShips(); //method call to place the ships
}

void ServerShip::placeShips() // places ships on the board
{
    std::vector<int> ShipSizes = {5, 4, 4, 3, 3, 3, 2, 2, 2, 2}; // std::vector which has the size and the amount of ships saved
    m_remainingShips = ShipSizes.size(); // sets the amount of remaining ships to the size of the ShipSizes vector

    for (int size : ShipSizes) { // for every ship 
        bool placed = false; // check if the ship was placed successfully 
        while (!placed) { // if the ship was not placed, repeat the process until every ship is placed
            int x = rand() % Board[0].size(); // random x coordinate for the beginning of the ship
            int y = rand() % Board.size(); // random y coordinate for the beginning of the ship
            bool horizontal = rand() % 2 == 0; // random decision if the ship is placed horizontally or vertically
            if (canPlaceShip(x, y, size, horizontal)) { // check if the ship can be placed in that spot
                placeShip(x, y, size, horizontal); // place the ship
                placed = true; // set placed to true to mark, that the ship was placed successfully
            }
        }
    }
}

bool ServerShip::canPlaceShip(int x, int y, int size, bool horizontal) { // checks if a ship can be placed at that position
    if (horizontal) {
        if (x + size > Board[0].size()) return false; // if the ship was placed horizontally but there is not enough space the placement is invalid
        for (int i = 0; i < size; ++i) { // check every segment of the ship
            if (Board[y][x + i] != '.') return false; // if the slot is occupied (not '.') the ship can not be placed
        }
    } else {
        if (y + size > Board.size()) return false; // if the ship was placed vertically but there is not enough space the placement is invalid
        for (int i = 0; i < size; ++i) { // check every segment of the ship
            if (Board[y + i][x] != '.') return false; // if the slot is occupied (not '.') the ship can not be placed
        }
    }
    return true; // if the slot is unoccupied return true
}

void ServerShip::placeShip(int x, int y, int size, bool horizontal) { // places a singular ship on the board
    for (int i = 0; i < size; ++i) { // every segment of the ship
        if (horizontal) {
            Board[y][x + i] = 'S'; // sets the slot to "S" for the ship if horizontal
        } else {
            Board[y + i][x] = 'S'; // sets the slot to "S" for the ship if vertical
        }
    }
}

std::string ServerShip::processShot(int x, int y) { // logic for shooting and checks if the shot connects or not
    if (x < 0 || x >= Board[0].size() || y < 0 || y >= Board.size()) {
        return "INVALID"; // invalid coordinates
    }
    if (Board[y][x] == 'S') { // if the position is a ship
        Board[y][x] = 'X'; // mark the position as "HIT"
        if (isShipDestroyed(x, y)) { // check if the ship was destroyed
            m_remainingShips--; // if the ship was destroyed lower the amount of ships
            if (m_remainingShips == 0) { // if no ships are remaining return "ALL_SHIPS_DESTROYED" if not return "HIT_AND_DESTROYED"
                return "ALL_SHIPS_DESTROYED";
            }
            return "HIT_AND_DESTROYED";
        }
        return "HIT"; // ship was hit
    } else if (Board[y][x] == '.') {
        Board[y][x] = 'O'; // if no ship was hit mark as "MISS"
        return "MISS";
    } else {
        return "ALREADY_SHOT"; // logic to tell how often the same position was shot
    }
}

bool ServerShip::isShipDestroyed(int x, int y) { // checks all adjacent fields to check if a ship is destroyed
    for (int dx = -1; dx <= 1; ++dx) { // for loop for adjacent fields in x
        for (int dy = -1; dy <= 1; ++dy) { // for loop for adjacent fields in y
            int nx = x + dx; // calculates the new x-coordinate
            int ny = y + dy; // calculates the new y-coordinate
            if (nx >= 0 && nx < Board[0].size() && ny >= 0 && ny < Board.size()) { // check if the new coordinates are within the board size
                if (Board[ny][nx] == 'S') { // if an adjacent field contains a ship
                    return false; // segment of the ship is still intact
                }
            }
        }
    }
    return true; // ship fully destroyed
}

//Weiter Schritte:

// CLient mit Strategie ausstatten, ob getroffen oder nicht
// Server beinhaltet das Spielfeld
// Protokoll(Kommunikation):
// Serverkommand | Antwort | Kommentar (Fehlerfälle) |
// undefined string | ERROR | ...
// Koordinatenbereich eingehalten | SPezifisch / ERROR   