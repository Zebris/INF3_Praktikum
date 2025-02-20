/*
 * server.C
 *
 *  Created on: 11.09.2019
 *      Author: aml
 */

<<<<<<< HEAD
 #include <iostream>
 #include <cstdio>
 #include <cstdlib>
 #include <cstring>
 #include <cerrno>
 #include <ctime>
 
 #include <unistd.h>
 #include <sys/types.h>
 #include <arpa/inet.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 
 #include "SIMPLESOCKET.h"
 #include "TASK3.cc"
 #include <sstream>
 #include <vector>
 #include <string>
 #include <array>
 
 class ServerShip : public TCPserver {
 public:
     ServerShip(int portNmb, int bSize);
     ~ServerShip();
 
 private:
     ServerShip();
     TASK3::World *world;
 
 protected:
     std::string myResponse(const std::string &input) override;
 };
 
 int main() {
     ServerShip srv(2022, 25);
     srv.run();
     return 0;
 }
 
 ServerShip::ServerShip(int portNmb, int bSize) : TCPserver(portNmb, bSize) {
     world = new TASK3::World();
     std::cout << "Server initialized on port " << portNmb << std::endl;
     std::cout << "Boardsize 10x10" << std::endl;
 }
 
 ServerShip::~ServerShip() {}
 
 std::string ServerShip::myResponse(const std::string &input) {
     auto x = -1;
     auto y = -1;
 
     if (input.compare(0, 7, "NEWGAME") == 0) {
         delete world;
         world = new TASK3::World();
         return std::string("READY");
     }
 
     if (input.compare(0, 6, "COORD[") == 0) {
         if (sscanf(input.c_str(), "COORD[%d,%d]", &x, &y) != 2) {
             return std::string("Error. Could not read coord-data.");
         }
 
         if (x > 10 && y > 10) {
             return std::string("ERRORBORDER");
         }
 
         TASK3::ShootResult result = world->shoot(x, y);
 
         switch (result) {
             case TASK3::WATER:
                 return "WATER";
             case TASK3::SHIP_HIT:
                 return "SHIPHIT";
             case TASK3::SHIP_DESTROYED:
                 return "SHIPDEST";
             case TASK3::ALL_SHIPS_DESTROYED:
             case TASK3::GAME_OVER:
                 return "GAMEOVER";
             default:
                 return "ERROR";
         }
     }
 
     return std::string("UNKNOWNCOMMAND");
 }
 
=======
#include <unistd.h>     // contains various constants
#include <sys/types.h>  // contains a number of basic derived types that should be used whenever appropriate
#include <arpa/inet.h>  // defines in_addr structure
#include <sys/socket.h> // for socket creation
#include <netinet/in.h> // contains constants and structures needed for internet domain addresses

#include "SIMPLESOCKET.h"
#include "TASK3.h"
#include <sstream> // needed for sscanf
#include <vector> // contains std::vector and functions
#include <string> //

class ServerShip: public TCPserver // the class "ServerShip" inherits public aspects of TCPserver and contains information of the game board
{
public:

    ServerShip(int portNmb, int bSize); // constructor for initializing the Server
	~ServerShip();  // destructor for closing the server

private:
    int m_boardWidth = 10; // width of the game board
    int m_boardHeight = 10; // height of the game board
    int m_remainingShips = 0; // contains the number of remaining ships

    std::vector<std::vector<char>> Board; // 2D vector for the board
    std::vector<int> ShipSizes = {5, 4, 4, 3, 3, 3, 2, 2, 2, 2}; // std::vector which has the size and the amount of ships saved

	ServerShip(); // private constructor which is not used

    void initializeBoard(); // method for initializing the game board
    void placeShips(); // method which places the ships on the board
    bool canPlaceShip(int x, int y, int size, bool horizontal); // method which checks if the placement of the ship is valid or invalid
    void placeShip(int x, int y, int size, bool horizontal); // method which places a singular ship
    std::string processShot(int x, int y); // method for shooting
    bool isShipDestroyed(int x, int y); // method which contains data if the ship is destroyed
    std::string myResponse(const std::string &input) override; // method for server-client interaction


    //Testing

    void printBoard();
};


int main(){
	srand(time(nullptr)); // initializes a random generator 
	ServerShip srv(2022,25); // creates a ServerShip object with port number and puffer size 
	srv.run(); // calls the run function of the ServerShip object
}


ServerShip::ServerShip(int portNmb, int bSize): TCPserver(portNmb, bSize) // constructor of the ServerShip class which initializes server and board
{

    srand(static_cast<unsigned>(time(nullptr))); // initializes random generator
    initializeBoard(); // initializes board
	std::cout << "Server initialized on port " << portNmb << std::endl; // output to check if the server initiation was successfull

}


ServerShip::~ServerShip(){} // destructor of the ServerShip class, which outputs a shutdown text


void ServerShip::initializeBoard() // initialization of the board with sizes of x and y
{
    Board.resize(m_boardHeight, std::vector<char>(m_boardWidth, '.')); // sets the board size to Height and Width and fills with "."
    placeShips(); //method call to place the ships

}


void ServerShip::placeShips() // places ships on the board
{
    m_remainingShips = ShipSizes.size(); // sets the amount of remaining ships to the size of the ShipSizes vector


    for(int i = 0; i < ShipSizes.size(); ++i)
    {
        int size = ShipSizes[i];
        bool placed = false;
        while (!placed) { // if the ship was not placed, repeat the process until every ship is placed
            int x = rand() % m_boardWidth; // random x coordinate for the beginning of the ship
            int y = rand() % m_boardHeight; // random y coordinate for the beginning of the ship
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
        if (x + size > m_boardWidth) return false; // if the ship was placed horizontally but there is not enough space the placement is invalid
        for (int i = 0; i < size; ++i) { // check every segment of the ship
            if (Board[y][x + i] != '.') return false; // if the slot is occupied (not '.') the ship can not be placed
        }
    } else {
        if (y + size > m_boardHeight) return false; // if the ship was placed vertically but there is not enough space the placement is invalid
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

    if (x < 0 || x >= m_boardWidth || y < 0 || y >= m_boardHeight) return "INVALID"; // invalid coordinates

    if (Board[y][x] == 'S')// if the position is a ship
    { 
        Board[y][x] = 'X'; // mark the position as "HIT"
            if (isShipDestroyed(x, y)) { // check if the ship was destroyed
            m_remainingShips--; // if the ship was destroyed lower the amount of ships
            std::string result = (m_remainingShips  == 0) ? "ALL_SHIPS_DESTROYED" : "HIT_AND_DESTROYED"; // Ternary Operator potentially < 0 than == 0
            printBoard();
            return  result;
        }
        printBoard();
    return "HIT"; // ship was hit
    }

    else if (Board[y][x] == '.') 
    {
        Board[y][x] = 'O'; // if no ship was hit mark as "MISS"
        printBoard();
    return "MISS";
    } 

    return "ALREADY_SHOT"; // logic to tell how often the same position was shot
}


bool ServerShip::isShipDestroyed(int x, int y) { // checks all adjacent fields to check if a ship is destroyed
    for (int dx = -1; dx <= 1; ++dx) { // for loop for adjacent fields in x |(3x3)|
        for (int dy = -1; dy <= 1; ++dy) { // for loop for adjacent fields in y
            int newX = x + dx; // calculates the new x-coordinate
            int newY = y + dy; // calculates the new y-coordinate

            if (newX >= 0 && newX < m_boardWidth && newY >= 0 && newY < m_boardHeight && Board[newY][newX] == 'S'){ // check if the new coordinates are within the board size and if a ship is adjacent
                    return false; // segment of the ship is still intact
            }
        }
    }
    return true; // ship fully destroyed
}


std::string ServerShip::myResponse(const std::string &input) {
        if (input.find("SHOT") == 0) { // checks if the command "SHOT" is part of the "&input" string
            int x, y;
            sscanf(input.c_str(), "SHOT %d %d", &x, &y); // checks 
            return processShot(x, y);
        }else{
            return "GOODBYE"; // returns a default response if the command is not recognized
            }
    }


void ServerShip::printBoard() {
    std::cout << "Game Board:" << std::endl;

    for (int y = 0; y < m_boardHeight; ++y) {
        for (int x = 0; x < m_boardWidth; ++x) {
            std::cout << Board[y][x] << " ";  // Output of each field of the board
        }
        std::cout << std::endl;  // New line after each row
    }
    std::cout << std::endl;
}




//Weiter Schritte:

// CLient mit Strategie ausstatten, ob getroffen oder nicht
// Server beinhaltet das Spielfeld
// Protokoll(Kommunikation):
// Serverkommand | Antwort | Kommentar (Fehlerfälle) |
// undefined string | ERROR | ...
// Koordinatenbereich eingehalten | SPezifisch / ERROR 
//telnet localhost:2022

// mit new im Konstruktor erzeugen und TASK3::World *world anlegen
>>>>>>> 7bf4156293cc70d0ce1af185e9d8e27149eafe2e
