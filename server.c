/*
 * server.C
 *
 *  Created on: 11.09.2019
 *      Author: aml
 * 
 * Updated by Group No. 2
 * M. Fichtner, M. Holzer, L. Hornig
 */

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
 #include "TASK3.c"
 #include <sstream>
 #include <vector>
 #include <string>
 #include <array>

 /**
 *
 * \class ServerShip
 *
 * \brief Inherits from class TCPserver.
 *
 *
 */
 
 class ServerShip : public TCPserver {
 public:
     /**
     * @brief Constructs a ServerShip object.
     * @param portNmb Port number on which the server listens.
     * @param bSize Buffer size for incoming messages.
     */
     ServerShip(int portNmb, int bSize);
     /**
     * @brief Destructor to clean up resources.
     */
     ~ServerShip();
 
 private:
     /**
     * @brief Private constructor to prevent instantiation without parameters.
     */
     ServerShip();
     TASK3::World *m_world;
 
 protected:
     /**
     * @brief Processes client requests and returns appropriate responses.
     * @param input Client's request message.
     * @return std::string server's response message.
     */
     std::string myResponse(const std::string &input) override;
 };
 
 int main() {
     ServerShip srv(2022, 25); ///< Creates and initializes the game server.
     srv.run(); ///< Starts the server.
     return 0;
 }
 
 ServerShip::ServerShip(int portNmb, int bSize) : TCPserver(portNmb, bSize) {
     m_world = new TASK3::World(); ///< Initializes the game board.
     std::cout << "Server initialized on port " << portNmb << std::endl;
 }
 
 ServerShip::~ServerShip() {}
 
 std::string ServerShip::myResponse(const std::string &input) {
     auto x = -1;
     auto y = -1;
 
     if (input.compare(0, 7, "NEWGAME") == 0) {
         delete m_world;
         m_world = new TASK3::World(); ///< Resets the game board.
         return std::string("READY");
     }
 
     if (input.compare(0, 6, "COORD[") == 0) {
         if (sscanf(input.c_str(), "COORD[%d,%d]", &x, &y) != 2) {
             return std::string("Error. Could not read coordinates.");
         }
 
         if (x > 10 && y > 10) {
             return std::string("ERRORBORDER");
         }
 
         TASK3::ShootResult result = m_world->shoot(x, y);
 
         switch (result) {
             case TASK3::WATER:
                 return "WATER";
             case TASK3::SHIP_HIT:
                 return "SHIPHIT";
             case TASK3::SHIP_DESTROYED:
                 return "SHIPDESTROYED";
             case TASK3::ALL_SHIPS_DESTROYED:
             case TASK3::GAME_OVER:
                 return "GAMEOVER";
             default:
                 return "ERROR";
         }
     }
 
     return std::string("UNKNOWNCOMMAND");
 }
 
