#include "dealer.h"

Turn calculateDealerMoves(GameState *gs){
    Turn ret = {NA, LOW_HEURISTIC, nullptr};
    if (liveChance(gs) == 0) {
	    ret.action = SHOOT_SELF;
		// auto it = std::find(gs->itemsSelf.begin(), gs->itemsSelf.end(), INV);
		// if (it != gs->itemsSelf.end()){
		// 	ret.action = USE_INV;
		// }
    } else if (liveChance(gs) == 1) {
	    ret.action = SHOOT_OTHER;
	} else {
	    ret.action = SHOOT_ANY;
		std::unordered_set<ITEM> item_set(gs->itemsSelf.begin(), gs->itemsSelf.end());
		if (gs->hpSelf < HP  && item_set.find(CIGS) != item_set.end()){
		 	ret.action = USE_CIGS;
		}
		else if (gs->hpSelf < HP && gs->hpSelf > 1 && item_set.find(PILLS) != item_set.end()){
		 	ret.action = USE_PILLS;
		}
		else if (!gs->cuffsUsed && item_set.find(CUFFS) != item_set.end()){
		 	ret.action = USE_CUFFS;
		}
		else if (item_set.find(GLASS) != item_set.end()){
		 	ret.action = USE_GLASS;
		}
		else if (item_set.find(BEER) != item_set.end()){
		 	ret.action = USE_BEER;
		}
	}
	return ret;
}
