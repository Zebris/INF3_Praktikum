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
    std::vector<std::pair<int, int>> everyFieldStrategy(int MaxX, int MaxY);

public:

    BattleShipClient(TCPclient &cli); // constructor of BattleShipClient
    int playGame(const vector<std::pair<int, int>> &shots); // game logic
    int playRandomStrategy(int MaxX, int MaxY, int repetitions); // plays with the random strategy
    int playEveryFieldStrategy(int MaxX, int MaxY, int repetitions);

};



BattleShipClient client(tcpclient); // creation of a BattleShipClient object


//client.playRandomStrategy(10, 10, 300); // playing with the random strategy


//client.playGridStrategy(10, 10, 100); // playing with the grid strategy

client.playEveryFieldStrategy(10, 10, 100);

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

    // Schieße auf jedes einzelne Feld, um vollständige Abdeckung zu erreichen
    for (int x = 0; x < maxX; ++x) {
        for (int y = 0; y < maxY; ++y) {
            shots.push_back({x, y});
        }
    }

    return shots;  // Rückgabe der Schüsse in der Reihenfolge
}





int BattleShipClient::playGame(const std::vector<std::pair<int, int>>& shots) {

    int shotsFired = 0;   // Zähler für die abgefeuerten Schüsse

    for(const auto &shot : shots)
    {
        if(firedShots.find(shot) != firedShots.end())
        {
            continue;
        }

        std::string shotMessage = "SHOT " + std::to_string(shot.first) + " " + std::to_string(shot.second);
        client.sendData(shotMessage);
        std::string response = client.receive(1024);

        std::cout << "Response of Server: " << response << std::endl;

        if(response == "ALL_SHIPS_DESTROYED")
        {
            client.sendData("END");
            break;
        }

        firedShots.insert(shot);
        shotsFired++;
    }
    return shotsFired;
}




int BattleShipClient::playRandomStrategy(int MaxX, int MaxY, int repetitions) { // plays with the random strategy for a given amount of repetitions
    
    int totalRandomShots = 0;

    for (int i = 0; i < repetitions; ++i) { // repeat the game for the amount of repetitions
        std::vector<std::pair<int, int>> randomShots = randomStrategy(MaxX, MaxY); // generate a random strategy
        totalRandomShots += playGame(randomShots); // play with the generated random strategy
    }

    return totalRandomShots; // returns the number of total shots needed
}


int BattleShipClient::playEveryFieldStrategy(int MaxX, int MaxY, int repetitions) { // plays with the random strategy for a given amount of repetitions
    
    int totalRandomShots = 0;

    for (int i = 0; i < repetitions; ++i) { // repeat the game for the amount of repetitions
        std::vector<std::pair<int, int>> randomShots = everyFieldStrategy(MaxX, MaxY); // generate a random strategy
        totalRandomShots += playGame(randomShots); // play with the generated random strategy
    }

    return totalRandomShots; // returns the number of total shots needed
}
