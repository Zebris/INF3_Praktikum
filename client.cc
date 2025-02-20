/*
 * client.C
 *
 *  Created on: 11.09.2019
 *      Author: aml
 */

 #include <string>
 #include <vector>
 #include <iostream>
 #include <algorithm>    // std::random_shuffle für shuffling the shots
 #include <ctime>
 #include <fstream>      // needed for data output
 #include <cstring>
 #include <cstdlib>
 #include <unistd.h>     // contains various constants
 #include <set>          // needed for duplicate avoidance of shots
 #include <stdint.h>     // needed for std::uint32_t etc.
 
 #include "SIMPLESOCKET.h" // definition of the TCP client
 
 class BattleShipClient {
 public:
     BattleShipClient(TCPclient &cli);
     void playGame(const std::vector<std::pair<int, int>> &shots, const std::string &strategyName);
     void gameStart(int MaxX, int MaxY);
     void saveStats(int amountOfShots, const std::string &strategyName);
     ~BattleShipClient();
 
 private:
     TCPclient &client;
     std::set<std::pair<int, int>> firedShots;
 
     std::vector<std::pair<int, int>> randomStrategy(int MaxX, int MaxY);
     std::vector<std::pair<int, int>> everyFieldStrategy(int MaxX, int MaxY);
 };
 
 int main() {
     srand(time(NULL));
     TCPclient tcpclient;
     std::string host = "localhost";
     int port = 2022;
     BattleShipClient client(tcpclient);
 
     if (!tcpclient.conn(host, port)) {
         std::cerr << "Failed to connect to server!" << std::endl;
         return 1;
     } else {
         std::cout << "Connected to the server: Game is starting!" << std::endl;
     }
 
     client.gameStart(10, 10);
     return 0;
 }
 
 BattleShipClient::BattleShipClient(TCPclient &cli) : client(cli) {}
 
 BattleShipClient::~BattleShipClient() {}
 
 std::vector<std::pair<int, int>> BattleShipClient::randomStrategy(int maxX, int maxY) {
     std::vector<std::pair<int, int>> shots;
 
     for (int x = 1; x <= maxX; ++x) {
         for (int y = 1; y <= maxY; ++y) {
             shots.push_back({x, y});
         }
     }
 
     std::random_shuffle(shots.begin(), shots.end());
     return shots;
 }
 
 std::vector<std::pair<int, int>> BattleShipClient::everyFieldStrategy(int maxX, int maxY) {
     std::vector<std::pair<int, int>> shots;
 
     for (int x = 1; x <= maxX; ++x) {
         for (int y = 1; y <= maxY; ++y) {
             shots.push_back({x, y});
         }
     }
 
     return shots;
 }
 
 void BattleShipClient::playGame(const std::vector<std::pair<int, int>> &shots, const std::string &strategyName) {
     std::uint32_t shotcount = 0;
 
     for (const auto &shot : shots) {
         if (firedShots.find(shot) != firedShots.end()) {
             std::cout << "Skipping fired shot: " << shot.first << "," << shot.second << std::endl;
             continue;
         }
 
         std::string shotMessage = "COORD[" + std::to_string(shot.first) + "," + std::to_string(shot.second) + "]";
         client.sendData(shotMessage);
 
         std::string response = client.receive(1024);
         std::cout << "Response of Server: " << response << std::endl;
 
         if (response.empty()) {
             std::cerr << "Empty response of server!" << std::endl;
             break;
         }
 
         if (response.compare(0, 8, "GAMEOVER") == 0) {
             std::cout << "Game over detected! Sending NEWGAME request!" << std::endl;
             client.sendData("NEWGAME");
             std::string newGameResponse = client.receive(1024);
 
             if (newGameResponse.compare(0, 5, "READY") == 0) {
                 std::cout << "New game started successfully!" << std::endl;
             } else {
                 std::cerr << "Failed to start a new game! Server response: " << newGameResponse << std::endl;
             }
 
             break;
         }
 
         firedShots.insert(shot);
         shotcount++;
     }
 
     std::cout << "Total shots fired: " << shotcount << std::endl;
     saveStats(shotcount, strategyName);
     sleep(0.5);
 }
 
 void BattleShipClient::gameStart(int MaxX, int MaxY) {
     std::uint32_t strategyInput = 0;
     std::uint32_t amountOfGames = 0;
 
     std::cout << "Starting Battle Ship Game!" << std::endl;
     std::cout << "How many games should be played? " << std::endl;
     std::cin >> amountOfGames;
 
     std::cout << "Choose a Strategy: 1 = Step-by-Step or 2 = Random" << std::endl;
     std::cin >> strategyInput;
 
     if (strategyInput == 1) {
         for (int i = 0; i < amountOfGames; i++) {
             firedShots.clear();
             playGame(everyFieldStrategy(MaxX, MaxY), "Step-by-Step Strategy");
         }
     } else if (strategyInput == 2) {
         for (int i = 0; i < amountOfGames; i++) {
             firedShots.clear();
             playGame(randomStrategy(MaxX, MaxY), "Random Strategy");
         }
     } else {
         std::cout << "Invalid Strategy!" << std::endl;
     }
 }
 
 void BattleShipClient::saveStats(int amountofShots, const std::string &strategyName) {
     std::ofstream stats("game_stats.txt", std::ios::app);
 
     if (stats.is_open()) {
         stats << strategyName << " : " << amountofShots << " shots needed!" << std::endl;
         stats.close();
         std::cout << "Stats saved for: " << strategyName << "\n" << amountofShots << " shots needed" << std::endl;
     } else {
         std::cerr << "Error opening file to save stats!" << std::endl;
     }
 }
 