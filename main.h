#ifndef MAIN_H_
#define MAIN_H_
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <ctime>

#define LOW_HEURISTIC -1000000000
#define HP 3
#define BULLETS 8
#define STARTITEMS {GLASS, PILLS};

typedef enum {
	BEER,
	GLASS,
	CIGS,
	SAW,
	CUFFS,
	PILLS,
} ITEM;

typedef enum {
	UNKNOWN,
    BLANK,
	LIVE,
} BULLET;

typedef enum {
	SHOOT_SELF,
	SHOOT_OTHER,
	USE_BEER,
	USE_GLASS,
	USE_CIGS,
	USE_SAW,
	USE_CUFFS,
	USE_PILLS,
	SHOOT_ANY,
	NA,
} ACTION;

struct Turn{
    int action;
	float heuristic;
	void *data;
};

struct GameState{
    int live;
    int blank;
    int hpSelf;
    int hpOther;
	bool selfTurn;
	bool inverted;
	bool sawed;
	bool cuffsUsed;
	bool cuffsOn;
	std::vector<BULLET> informationSelf;
	std::vector<BULLET> informationOther;
	std::vector<ITEM> itemsSelf;
	std::vector<ITEM> itemsOther;
};

GameState DoMove(GameState gs, Turn turn, bool blank);

GameState GSInit();
float liveChance(GameState *gs);
bool HasBullets(GameState *gs);
bool PlayersAlive(GameState *gs);
bool RoundOver(GameState *gs);
int PlayerHp(GameState *gs);
int DealerHp(GameState *gs);

void Print(GameState *gs);
void Flip(GameState *gs);
void Print(Turn t);

std::vector<ACTION> ActionsFromItems(GameState *gs);

#endif // MAIN_H_
