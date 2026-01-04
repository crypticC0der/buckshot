#include "main.h"
#include "buckshot.h"
#include "dealer.h"
#include <iostream>

GameState GSInit(){
	int live = BULLETS / 2;
	std::vector<ITEM> items = STARTITEMS;
	std::vector<BULLET> info(BULLETS,UNKNOWN);
	GameState gs;
	gs.live = live;
	gs.blank= BULLETS-live;
	gs.hpSelf = gs.hpOther = HP;
	gs.selfTurn = true;
	gs.inverted = gs.sawed = false;
	gs.informationSelf = info;
	gs.informationOther = info;
	gs.itemsSelf = items;
	gs.itemsOther= items;
	return gs;
}

float liveChance(GameState *gs){
    int bullets = gs->live + gs->blank;
	if (bullets == 0){
		return 0;
	}
	float chance;
	switch (gs->informationSelf[bullets -1]) {
	case LIVE:
		chance = 1;
		break;
	case BLANK:
		chance = 0;
		break;
	default:
		chance = ((float)(gs->live))/bullets;
		break;
	}
	if (gs->inverted){
		return 1 - chance;
	}
	return chance;
}

std::vector<ACTION> ActionsFromItems(GameState *gs){
	std::unordered_set<ITEM> item_set(gs->itemsSelf.begin(), gs->itemsSelf.end());
	std::vector<ACTION> actions;
	for (ITEM i : item_set){
		switch (i) {
		case CUFFS:
			if (!gs->cuffsUsed){
				actions.push_back((ACTION)((int)i+2));
			}
			break;
		case SAW:
			if (!gs->sawed){
				actions.push_back((ACTION)((int)i+2));
			}
			break;
		default:
			actions.push_back((ACTION)((int)i+2));
			break;
		}
	}
	return actions;
}

bool HasBullets(GameState *gs){
    return gs->live + gs->blank > 0;
}

bool PlayersAlive(GameState *gs){
    return gs->hpSelf > 0 && gs->hpOther > 0;
}

bool RoundOver(GameState *gs){
	return !PlayersAlive(gs) || !HasBullets(gs);
}

int PlayerHp(GameState *gs){
	if (gs->selfTurn){
		return gs->hpSelf;
	} else {
		return gs->hpOther;
	}
}

int DealerHp(GameState *gs){
	if (gs->selfTurn){
		return gs->hpOther;
	} else {
		return gs->hpSelf;
	}
}

void Print(GameState *gs){
		std::cout << "hp: " << std::endl <<
			"\tplayer: " << PlayerHp(gs) << std::endl <<
			"\tdealer: " << DealerHp(gs) << std::endl;
		std::cout << "bullets: " << gs->live << "/" << gs->live + gs->blank<< std::endl;
		if (gs->selfTurn){
			std::cout << "PLAYERS TURN" << std::endl;
		} else {
			std::cout << "DEALERS TURN" << std::endl;
		}
		// for (ITEM i : gs->itemsSelf){
		// 	std::cout << i << ",";
		// }
		// std::cout << std::endl;
		// for (ITEM i : gs->itemsOther){
		// 	std::cout << i << ",";
		// }
		// std::cout << std::endl;
}

void Print(Turn t){
	std::cout << "move: ";
	switch (t.action) {
	case SHOOT_OTHER:
		std::cout << "shoot other";
		break;
	case SHOOT_SELF:
		std::cout << "shoot self";
		break;
	case USE_BEER:
		std::cout << "drink beer";
		break;
	case USE_GLASS:
		std::cout << "magnifying glass";
		break;
	case USE_CIGS:
		std::cout << "smoke a cig";
		break;
	case USE_SAW:
		std::cout << "sawed off";
		break;
	case USE_CUFFS:
		std::cout << "handcuff";
		break;
	default:
		std::cout << "UNKNOWN MOVE";
		break;
	}
	std::cout << std::endl << "val: " << t.heuristic << std::endl;
}

void Flip(GameState *gs){
	if (gs->cuffsOn){
		gs->cuffsOn = false;
		return;
	}
	gs->cuffsUsed = false;
	gs->selfTurn = !gs->selfTurn;
	int temp_hp = gs->hpSelf;
	gs->hpSelf = gs->hpOther;
	gs->hpOther = temp_hp;
	std::vector<ITEM> temp_items = gs->itemsSelf;
	gs->itemsSelf = gs->itemsOther;
	gs->itemsOther = temp_items;
	std::vector<BULLET> temp_info = gs->informationSelf;
	gs->informationSelf = gs->informationOther;
	gs->informationOther = temp_info;
}

GameState DoMove(GameState gs, Turn turn, bool blank){
	switch (turn.action){
	case SHOOT_SELF:
		if (blank) {
			gs.blank--;
		} else {
			gs.live--;
			gs.hpSelf--;
			if (gs.sawed){
				gs.hpSelf--;
			}
			Flip(&gs);
		}
		gs.sawed = false;
		gs.inverted = false;
		break;
	case SHOOT_OTHER:
		if (!gs.selfTurn && !gs.sawed){
			auto it = std::find(gs.itemsSelf.begin(), gs.itemsSelf.end(), SAW);
			if (it != gs.itemsSelf.end()){
				gs = DoMove(gs,{USE_SAW, 0, nullptr},false);
			}
		}
		if (blank) {
			gs.blank--;
		} else {
			gs.live--;
			if (gs.sawed){
				gs.hpOther--;
			}
			gs.hpOther--;
		}
		gs.sawed = false;
		Flip(&gs);
		break;
	case USE_BEER:
		{
		auto it = std::find(gs.itemsSelf.begin(), gs.itemsSelf.end(), BEER);
		gs.itemsSelf.erase(it);
		if (blank) {
			gs.blank--;
		} else {
			gs.live--;
		}
		break;
		}
	case USE_GLASS:
		{
		auto it = std::find(gs.itemsSelf.begin(), gs.itemsSelf.end(), GLASS);
		gs.itemsSelf.erase(it);
		if (blank) {
			gs.informationSelf[gs.live + gs.blank - 1] = BLANK;
		} else {
			gs.informationSelf[gs.live + gs.blank - 1] = LIVE;
		}
		break;
		}
	case USE_SAW:
		{
		auto it = std::find(gs.itemsSelf.begin(), gs.itemsSelf.end(), SAW);
		gs.itemsSelf.erase(it);
		gs.sawed = true;
		break;
		}
	case USE_CIGS:
		{
		auto it = std::find(gs.itemsSelf.begin(), gs.itemsSelf.end(), CIGS);
		gs.itemsSelf.erase(it);
		if (gs.hpSelf < HP){
			gs.hpSelf++;
		}
		break;
		}
	case USE_CUFFS:
		{
		auto it = std::find(gs.itemsSelf.begin(), gs.itemsSelf.end(), CUFFS);
		gs.itemsSelf.erase(it);
		gs.cuffsUsed = true;
		gs.cuffsOn = true;
		break;
		}
	case USE_PILLS:
		{
		auto it = std::find(gs.itemsSelf.begin(), gs.itemsSelf.end(), CUFFS);
		gs.itemsSelf.erase(it);
		if (blank){
			gs.hpSelf--;
		}else{
			gs.hpSelf+=2;
			if (gs.hpSelf > HP){
				gs.hpSelf=HP;
			}
		}
		break;
		}
	}
	return gs;
}

int main(int argc, char *argv[]){
    std::srand(std::time({})); // use current time as seed for random generator
	GameState g = GSInit();

	std::vector<BULLET> bullets_in_gun(BULLETS,BLANK);
	int live_to_place = g.live;
	while(live_to_place > 0) {
		int place = std::rand() % BULLETS;
		if (bullets_in_gun[place] == BLANK) {
			bullets_in_gun[place] = LIVE;
			live_to_place--;
		}
	}

	while (!RoundOver(&g)){
		BULLET *b = &bullets_in_gun[g.live + g.blank -1];
		Print(&g);
		Turn t;
		if (g.selfTurn){
			t = calculateMoves(&g,false);
		}else{
			t = calculateDealerMoves(&g);
			if (t.action == SHOOT_ANY){
				if (std::rand() % 2 == 0){
					t.action = SHOOT_SELF;
				} else {
					t.action = SHOOT_OTHER;
				}
			}
		}
		Print(t);
		if (*b == BLANK){
			std::cout << "BLANK" << std::endl;
		} else {
			std::cout << "HIT" << std::endl;
		}
		bool blank = *b == BLANK;
		if (t.action == USE_PILLS){
			blank = std::rand() % 2 == 0;
		}
		g = DoMove(g, t, blank);
		std::cout << "--------------------"<< std::endl;
	}
	Print(&g);
	std::cout << "--------------------"<< std::endl;

	if (PlayerHp(&g) <= 0) {
		std::cout << "DEALER WINS" << std::endl;
	} else if (DealerHp(&g) <= 0) {
		std::cout << "PLAYER WINS" << std::endl;
	} else {
		std::cout << "TIE" << std::endl;
	}
	return 0;
}
