#ifndef BUCKSHOT_H_
#define BUCKSHOT_H_

#include <iostream>
#include "main.h"
#include "dealer.h"

Turn calculateDealerOptions(GameState *gs);
Turn calculateMoves(GameState *gs, bool start = false);
float turnHeuristic(GameState *gs, Turn t);
float heuristic(GameState *gs);

#endif // BUCKSHOT_H_
