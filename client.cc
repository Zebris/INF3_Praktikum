/*
 * client.C
 *
 *  Created on: 11.09.2019
 *      Author: aml
 */

#include <string>
#include <vector>
#include <iostream>
#include <algorithm> // std::random_shuffle für shuffling the shots
#include <ctime>
#include <unistd.h> // contains various constants
#include <set> // needed for duplicate avoidance of shots

#include "SIMPLESOCKET.h" // definition of the TCP client

class BattleShipClient // Class for client
{
private:
    TCPclient &client; // reference of the TCP client object

    std::set<std::pair<int, int>> firedShots; // set for already shot coordinates to avoid duplicates
    std::vector<std::pair<int, int>> generateRandomStrategy(int MaxX, int MaxY); // generates a random shooting strategy
    std::vector<std::pair<int, int>> generateGridStrategy(int MaxX, int MaxY); // generates a grid based shooting strategy

public:
    BattleShipClient(TCPclient &cli); // constructor of BattleShipClient

    int playGame(const vector<std::pair<int, int>>&shots); // game logic
    void playBothStrategies(int MaxX, int MaxY, int repetitions); // runs both strategies and evaluates
    int playRandomStrategy(int MaxX, int MaxY, int repetitions); // plays with the random strategy
    int playGridStrategy(int MaxX, int MaxY, int repetitions); // plays with the grid based strategy
};


int main() {
	srand(time(NULL)); // initializes random generator with the time
	TCPclient tcpclient; // creation of a TCPclient object
	std::string host = "localhost"; // defines the server host
	int port = 2022; // defines the server port

	// connection to host
	if(!tcpclient.conn(host, port)) // if the connection fails end the program
	{
		std::cerr << "Failed to connect to server!" << std::endl;
		return 1;
	}
	else
	{
		std::cout << "Connected to the server: Game is starting!" << std::endl;
	}


BattleShipClient client(tcpclient); // creation of a BattleShipClient object


client.playRandomStrategy(10, 10, 10); // playing with the random strategy


//client.playGridStrategy(10, 10, 500); // playing with the grid strategy

return 0;

}


BattleShipClient::BattleShipClient(TCPclient &cli) : client(cli) {} // constructor which gives the TCP client

std::vector<std::pair<int, int>> BattleShipClient::generateRandomStrategy(int maxX, int maxY) { // generates a random shooting strategy
    
    std::vector<std::pair<int, int>> shots;
    
    // for loop for every possible coordinates
    for (int x = 0; x < maxX; ++x) {
        for (int y = 0; y < maxY; ++y) {
            shots.push_back({x, y}); // adds every coordinate to the vector
        }
    }

    std::random_shuffle(shots.begin(), shots.end());  // shuffles the vector to create a random sequence
    return shots; // returns the mixed vector of shots
}

std::vector<std::pair<int, int>> BattleShipClient::generateGridStrategy(int maxX, int maxY) { // generates a grid based shooting strategy

    std::vector<std::pair<int, int>> shots;
    
    // for loop for every possible coordinates
    for (int x = 0; x < maxX; ++x) {
        for (int y = 0; y < maxY; ++y) { // selects only coordinates of which x + y is even
            if ((x + y) % 2 == 0) {
                shots.push_back({x, y}); // add these coordinates to the vector
            }
        }
    }
    return shots; // returns vector with grid based coordinates
}

int BattleShipClient::playGame(const std::vector<std::pair<int, int>>& shots) { // Shooting is based on the vectors of shots
    int shotsFired = 0;
    
    for (const auto &shot : shots) {  // ranged base for loop for all shots
        // check if the coordinate was already used
        if (firedShots.find(shot) != firedShots.end()) {
            continue; // if the coordinate was already used skip the shot
        }

        ++shotsFired; // increase the number of shots
        std::string shotMessage = "SHOT " + std::to_string(shot.first) + " " + std::to_string(shot.second);
        client.sendData(shotMessage); // sends shot message to the server
        std::string response = client.receive(1024);  // receive server response
        std::cout << "Antwort vom Server: " << response << std::endl;

        if (response == "ALL_SHIPS_DESTROYED") { // end the game if every ship was destroyed or its game over
            return std::string("GAME_OVER");
        }

        firedShots.insert(shot);  // add the fired coordinates to the set to avoid duplicates
    }
    return shotsFired; // returns the amount of shots fired
}


int BattleShipClient::playRandomStrategy(int MaxX, int MaxY, int repetitions) { // plays with the random strategy for a given amount of repetitions
    int totalRandomShots = 0;

    for (int i = 0; i < repetitions; ++i) { // repeat the game for the amount of repetitions
        std::vector<std::pair<int, int>> randomShots = generateRandomStrategy(MaxX, MaxY); // generate a random strategy
        totalRandomShots += playGame(randomShots); // play with the generated random strategy
    }

    return totalRandomShots; // returns the number of total shots needed
}

int BattleShipClient::playGridStrategy(int MaxX, int MaxY, int repetitions) { // plays with the grid based strategy for a given amount of repetitions
    int totalRasterShots = 0;

    for (int i = 0; i < repetitions; ++i) { // repeat the game for the amount of repetitions
        std::vector<std::pair<int, int>> rasterShots = generateGridStrategy(MaxX, MaxY); // generate a grid based strategy
        totalRasterShots += playGame(rasterShots); // play with the grid strategy
    }

    return totalRasterShots; // returns the number of total shots needed
}
