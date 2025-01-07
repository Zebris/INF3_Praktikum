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

    std::vector<std::pair<int, int>> randomStrategy(int MaxX, int MaxY); // generates a random shooting strategy
    std::vector<std::pair<int, int>> everyFieldStrategy(int MaxX, int MaxY); // generates a everyField shooting strategy

public:

    BattleShipClient(TCPclient &cli); // constructor of BattleShipClient
    int playGame(const vector<std::pair<int, int>> &shots); // game logic
    void playRandomStrategy(int MaxX, int MaxY); // plays with the random strategy
    void playEveryFieldStrategy(int MaxX, int MaxY); // plays with the everyField strategy

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


//client.playRandomStrategy(10, 10); // playing with the random strategy

client.playEveryFieldStrategy(10, 10); // playing with the everyField strategy

return 0;

}


BattleShipClient::BattleShipClient(TCPclient &cli) : client(cli) {} // constructor which gives the TCP client


std::vector<std::pair<int, int>> BattleShipClient::randomStrategy(int maxX, int maxY) { // generates a random shooting strategy
    
    std::vector<std::pair<int, int>> shots;
    
    // for loop for every possible coordinates
    for (int x = 0; x < maxX; ++x) {
        for (int y = 0; y < maxY; ++y) {
            shots.push_back({x, y}); // adds every coordinate to the vector
        }
    }

    std::random_shuffle(shots.begin(), (shots.end()));  // shuffles the vector to create a random sequence
    return shots; // returns the mixed vector of shots
}


std::vector<std::pair<int, int>> BattleShipClient::everyFieldStrategy(int maxX, int maxY) {
    std::vector<std::pair<int, int>> shots;

    // Generate shots for every field on the board
    for (int x = 0; x < maxX; ++x) {
        for (int y = 0; y < maxY; ++y) {
            shots.push_back({x, y});
        }
    }

    return shots;  // Return the list of shots in order
}


int BattleShipClient::playGame(const std::vector<std::pair<int, int>>& shots) {

    int shotsFired = 0;   // Counter for the number of shots fired

    for(const auto &shot : shots) //iterate over all shots
    {
        if(firedShots.find(shot) != firedShots.end()) //check if the shot has already been fired
        {
            continue; //skip already fired shots
        }

        std::string shotMessage = "SHOT " + std::to_string(shot.first) + " " + std::to_string(shot.second);
        client.sendData(shotMessage);
        std::string response = client.receive(1024);

        std::cout << "Response of Server: " << response << std::endl;

        if(response == "ALL_SHIPS_DESTROYED") // check if all ships have been destroyed
        {
            client.sendData("END");
            break;
        }

        firedShots.insert(shot); // add the shot to the set of fired shots
        shotsFired++; // increment the counter
    }
    return shotsFired; // return the total number of shots fired
}


void BattleShipClient::playRandomStrategy(int MaxX, int MaxY) // plays with the random strategy
{
    playGame(randomStrategy(MaxX, MaxY)); // play with the generated random strategy
}


void BattleShipClient::playEveryFieldStrategy(int MaxX, int MaxY) // plays with the every field strategy
{ 
    playGame(everyFieldStrategy(MaxX, MaxY)); // plays with the every field strategy    
}