#ifndef TILE_H
#define TILE_H

#include "Types.h"
#include <iostream>
#include <string>

class Tile {
   public:
   //Tile constructor 
   Tile(TileType type);

   //Deconstructor
   ~Tile();

   //clear function used in Deconstructor
   void tileClear();

   //Returns a toString of the Tile
   std::string tileToString();

   TileType getType();

   private:
   TileType type;

};

#endif
