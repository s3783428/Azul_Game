#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <time.h> 
#include <cstdlib>
#include <iterator>
#include "Player.h"
#include "Factory.h"
#include "Types.h"
#include "Tile.h"
#include "Bag.h"

void printMenu();
void printCredits();
void printReferenceBoard();
int loadGame(Factory* (*factories)[NUM_FACTORIES], Factory* table, Bag* bag, Player* player1, Player* player2);
void saveGame(std::string saveName);
void game(int seed, bool load);
void takePlayerTurn(Factory* (*factories)[NUM_FACTORIES], Factory* table, Player* player);
void printFactories(Factory* (*factories)[NUM_FACTORIES], Factory* table);
void printPlayerWall(Player* player);
std::vector<std::string> takeUserInput();

int main(int argc, char const *argv[])
{
   //loop until we quit
   bool quit = false;
   printMenu();
   while (!quit) {
      std::string input = takeUserInput().at(0);
      
      //handle input
      if (input == "1") {
         int seed = 0;
         //new game
         if (argc == 2) {
            //TODO check seed is a valid seed
            seed = atoi(argv[1]);
            srand(seed);
         }
         else {
            //generate a random seed
            // srand(time(0));
            srand(1);
         }

         seed = rand();
         game(seed, false);
         
      } 
      else if (input == "2") {
         //load game
         game(0, true);
      } 
      else if (input == "3") {
         //credits
         printCredits();
      } 
      else if (input == "4") {
         //quit
         quit = true;
      } 
      else if (input == "save") {
         saveGame("bigtest");
      }
      else {
         //bad input, try again
         std::cout << "Choice not recognised, please choose an option from the list" << std::endl;
      }
   }

   return 0;
}

void game(int seed, bool load) {
   std::cout << "Starting a new game" << std::endl;
   int turn = -1;
   //bag init
   Bag* bag = new Bag(seed);

   Factory* table = new Factory();

   Factory* factories[NUM_FACTORIES];
   for (int i = 0; i < NUM_FACTORIES; i++) {
      factories[i] = new Factory();
   }
   //player names
   Player* player1 = new Player("");
   Player* player2 = new Player("");
   if (load) {
      turn = loadGame(&factories,table,bag,player1,player2);
      if (turn == -1) {
         load = false;
         std::cout << "Load Failed. Starting a fresh game." << std::endl;
      }
      else {
         std::cout << "Loaded" << std::endl << std::endl;
      }
   }
   if (!load) {
      bag->fill();
      bag->shuffle();

      std::cout << "Enter a name for player 1" << std::endl << std::endl;
      std::string name = takeUserInput().at(0);
      player1 = new Player(name);

      std::cout << "Enter a name for player 2" << std::endl << std::endl;
      name = takeUserInput().at(0);
      player2 = new Player(name);

      std::cout << "Let's Play!" << std::endl << std::endl;
   }


   
   int round = 0;
   bool roundLoop = true;
   while (roundLoop) {
      //ensure clear factories, then fill
      if (round != 0 && load != true) {
         for (int i = 0; i < NUM_FACTORIES; i++) {
            factories[i]->clearFactory();
            for (int j = 0; j < TILES_PER_FAC; j++) {
               factories[i]->addTile(bag->getTopTile());
            }
         }
         //ensure clean table, add first player tile
         table->clearFactory();
         table->addTile(Tile(TileType::FIRST_PLAYER));
      }
      


      bool tileLoop = true;
      while (tileLoop) {
         //players take their turns
         if (turn == 2) {
            takePlayerTurn(&factories, table, player2);
            turn = -1;
         }
         takePlayerTurn(&factories, table, player1);
         takePlayerTurn(&factories, table, player2);
         //loop until we have no tiles on table
         //factories check

         //check if any tiles are left
         int factoryTileCount = 0;
         int tableTileCount = 0;
         tableTileCount = table->getTileCount();
         for (int i = 0; i < NUM_FACTORIES; i++) {
            //check all factories
            factoryTileCount = factories[i]->getTileCount();
            if (factoryTileCount != 0) {
               i = 5;
            }
         }
         //if any factory has tiles left, let the players go again.
         if (tableTileCount > 0 || factoryTileCount > 0) {
            tileLoop = true;
         }
         else {
            tileLoop = false;
         }
      }

      //SCORE HERE


      //stop looping the rounds when a player wins
      roundLoop = false;
      round++;
   }
   std::cout << "Game Over." << std::endl;
   delete bag;
   delete player1;
   delete player2;
   delete table;
   for (int i = 0; i < 5; i++) {
      delete factories[i];
   }
}

void takePlayerTurn(Factory* (*factories)[NUM_FACTORIES], Factory* table, Player* player) {
   printFactories(factories, table);
   printPlayerWall(player);
   bool valid = false;
   while (!valid) {
      std::vector<std::string> turn = takeUserInput();
      if (turn.size() > 0) {
         std::string command = turn.at(0);
         if (command == "help" || command == "h") {
            std::cout << "This is where you take your turn. Use the command 'turn', ";
            std::cout << "along with your selections to take your turn!" << std::endl;
            std::cout << "eg: '> turn 3 L 3' moves all light blue tiles, from ";
            std::cout << "factory 3, and puts them in storage line 3" << std::endl;
         }
         else if (command == "turn") {
            if (turn.size() != 4) {
               std::cout << "Please use the command format, 'turn <factory> <color_code> <buffer>" << std::endl;
               std::cout << turn.at(1) << std::endl;
            }
            else {
               //check turn input is valid
               int factory = -1;
               int bufferLine = -1;

               //factory int interp
               try {
                  factory = std::stoi(turn.at(1)) -1;
               }
               //catch oob or invalid
               catch (...) {
                  std::cout << "Please input a valid factory ID, between 0 and 5." << std::endl;
               }
               //bufferline in interp
               try {
                  bufferLine = std::stoi(turn.at(3)) -1;
               }
               //same as above
               catch (...) {
                  std::cout << "Please input a valid buffer ID, between 1 and 5." << std::endl;
               }

               //change tile string to TileType
               TileType userTileType = Tile::stringToType(turn.at(2));

               std::string errorCheck = "";

               //errorCheck += checkValidFactory(factory);
               //errorCheck += checkValidTile(tileChar);
               //errorCheck += checkValidBuffer(bufferLine);

               if (errorCheck == "") {
                  int tiles = -1;
                  if (factory == -1) {
                     tiles = table->removeTile(userTileType);
                     if (table->removeTile(TileType::FIRST_PLAYER) != 0) {
                        player->getWall()->addToFloorLine(TileType::FIRST_PLAYER, 1);
                     }
                  }
                  else {
                     tiles = (*factories)[factory]->removeTile(userTileType);
                  }

                  if (tiles > 0) {
                     valid = true;
                     player->getWall()->addToStorageLine(userTileType, tiles, bufferLine);
                     std::vector<Tile> moveToTable;
                     if (factory != -1) {
                        moveToTable = (*factories)[factory]->getTiles();
                        (*factories)[factory]->clearFactory();
                        for (int i = 0; i < TILES_PER_FAC; i++) {
                           (*factories)[factory]->addTile(Tile());
                        }
                     }
                     
                     if (moveToTable.size() > 0) {
                        for (auto i = moveToTable.begin(); i != moveToTable.end(); i++) {
                           if (i->getType() != TileType::NO_TILE) {
                              table->addTile(Tile(i->getType()));
                           }
                        }
                     }
                     
                  }
                  else {
                     std::cout << "Factory number " + std::to_string(factory + 1) + " has no tiles of type ";
                     std::cout << Tile::tileToString(userTileType) << ". Please make a valid selection.";
                     //the player has chosen to take a type of tile that is not in that factory
                  }

               }
               else {
                  //some blah about valid tile types, as well as buffers and factories
               }
            }
         }
         else if (command == "quit") {
            //exit the game
         }
         else if (command == "save") {
            //save the game
         }
      }
      else {
         std::cout << "Please input a command." << std::endl;
      }
   }
   
      
   
}

void printFactories(Factory* (*factories)[NUM_FACTORIES], Factory* table) {

   std::cout << "Factories: " << std::endl << "0: ";
   table->printFactory();
   std::cout << std::endl;

   for (int i = 0; i < NUM_FACTORIES; i++) {
      if (factories != nullptr) {
         std::cout << i + 1 << ": ";
         (*factories)[i]->printFactory();
      }
      std::cout << std::endl;
   }
   std::cout << std::endl;
}

void printPlayerWall(Player* player) {
   std::string playerName = player->getName();
   std::string wallString = player->getWall()->getPlayerWallString();

   std::cout << "Mosaic for " << playerName << ":" << std::endl;
   std::cout << wallString << std::endl;
   std::cout << std::endl;
}

void printMenu() {
   //just print these lines as the main menu
   std::cout << "Welcome to Azul!" << std::endl;
   std::cout << "-------------------" << std::endl;
   std::cout << "Menu" << std::endl;
   std::cout << "----" << std::endl;
   std::cout << "1. New Game" << std::endl;
   std::cout << "2. Load Game" << std::endl;
   std::cout << "3. Credits (Show student information)" << std::endl;
   std::cout << "4. Quit" << std::endl;
}

void printCredits() {
   std::cout << "----------------------------------" << std::endl;
   std::cout << "Name:        Jordan Tori" << std::endl;
   std::cout << "Student ID:  s3542275" << std::endl;
   std::cout << "Email:       s3542275@student.rmit.edu.au" << std::endl;
   std::cout << "Name:        Merhawi Minassi" << std::endl;
   std::cout << "Student ID:  s3668396" << std::endl;
   std::cout << "Email:       s3668396@student.rmit.edu.au" << std::endl;
   std::cout << "Name:        Guy Witherow" << std::endl;
   std::cout << "Student ID:  s3783428" << std::endl;
   std::cout << "Email:       s3783428@student.rmit.edu.au" << std::endl;
   std::cout << "----------------------------------" << std::endl;
}

void printReferenceBoard() {
   std::cout << "This is the pattern each board follows!" << std::endl;
   //maybe add command to bring this up?
   char tiles[WALL_DIM] = { 'B', 'Y', 'R', 'U', 'L' };
   for (int i = 0; i < WALL_DIM; i++) {
      for (int j = 0; j < WALL_DIM; j++) {
         int tileNumber = (j + i) % 5;
         std::cout << tiles[tileNumber] << " ";
      }
      std::cout << std::endl;
   }

   std::cout << std::endl;

   std::cout << "Please make moves using the format 'turn <factory number> <tile color letter> <storage row>" << std::endl;
}

std::vector<std::string> takeUserInput() {
   //TODO Ensure user input is valid
   std::string input = "";
   //user input space is indicated by an arrow
   std::cout << std::endl << "> ";
   std::getline(std::cin,input);

   //break string into arguments
   std::istringstream stream{input};
   using StrIt = std::istream_iterator<std::string>;
   std::vector<std::string> container{ StrIt{stream}, StrIt{} };

   if (container.size() == 0) {
      container.push_back(" ");
   }

   return container;
}

void saveGame(std::string saveName) {

   std::ofstream savefile;
   savefile.open(saveName + ".txt");
   int currentLine = 0;

   while (currentLine <= 35) {
      std::string data = "a";

      if (currentLine == 0) {
         // data = getBag();
      }
      else if (currentLine == 1) {
         // data = getLid();
      }
      else if (currentLine >= 2 && currentLine <= 6) {
         // data = getFactory(currentLine - 2);
      }
      else if (currentLine == 7) {
         // data = getTable();
      }
      else if (currentLine >= 8 && currentLine <= 35) {
         //14 lines of data for a player, * 2
         //Player player = getPlayer((currentLine - 7) / 14);
         int currentDataPoint = (currentLine - 7) % 14;
         if (currentDataPoint == 0) {
            //data = player.getName();
         }
         else if (currentDataPoint == 1) {
            //data = player.getID()????
         }
         else if (currentDataPoint >= 2 && currentDataPoint <= 6) {
            //data = player.getBufferLine(currentDataPoint-3);
         }
         else if (currentDataPoint >= 7 && currentDataPoint <= 11) {
            //data = player.getWallLine(currentDataPoint-8);
         }
         else if (currentDataPoint == 12) {
            //data = player.getFloor();
         }
         else if (currentDataPoint == 13) {
            //data = player.getScore();
         }

      }
      else if (currentLine == 36) {
         // data = getCurrentPlayerID();
      }

      savefile << data + "\n";
      currentLine++;
   }



   savefile.close();

}

int loadGame(Factory* (*factories)[NUM_FACTORIES], Factory* table, Bag* bag, Player* player1, Player* player2) {
   //get user input for which file to load
   std::string input;
   std::cout << "Enter the name of the file which you want to load."
      << std::endl << "> ";
   std::cin >> input;

   int turnToReturn = -1;

   //open specified file
   std::string line;
   std::ifstream loadFile(input + ".txt");
   int currentLine = 0;
   if (loadFile.is_open()) {
      //read lines, discarding # or empty lines
      while (getline(loadFile, line)) {
         std::string lineCheck = line;
         if (line[0] == '#' || line.length() == 0) {
            //do nothin
         }

         //read in the data
         else {
            int dataIndex = 0;

            //identify what part of the file we are reading
            if (currentLine == 0) {
               bag->load(line);
            }
            else if (currentLine >= 1 && currentLine <= 5) {
               dataIndex = currentLine - 1;
               (*factories)[dataIndex]->load(line);
            }
            else if (currentLine == 6) {
               table->load(line);
            }
            else if (currentLine >= 7 && currentLine <= 32) {
               //13 lines of data for a player, * 2
               dataIndex = currentLine - 7;
               if (dataIndex / 13 == 0) {
                  player1->load(line, dataIndex % 13);
               }
               else {
                  player2->load(line, dataIndex % 13);
               }
            }
            else if (currentLine == 33) {
               try {
                  turnToReturn = std::stoi(line);
               }
               catch (...) {
                  std::cout << "Current Turn could not be read, assuming player 1" << std::endl;
                  turnToReturn = 1;
               }
            }
            currentLine++;
         }
      }

      loadFile.close();
   }
   else std::cout << "Unable to open file" << std::endl;
   return turnToReturn;
}