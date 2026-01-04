#include "buckshot.h"

Turn calculateDealerOptions(GameState *gs){
	Turn ret = calculateDealerMoves(gs);
	ret.heuristic=turnHeuristic(gs, ret);
	return ret;
}

std::vector<ACTION> getMoves(GameState *gs){
	std::vector<ACTION> actions = ActionsFromItems(gs);
	actions.push_back(SHOOT_OTHER);
	actions.push_back(SHOOT_SELF);
	return actions;
}

Turn calculateMoves(GameState *gs, bool start){
    if (RoundOver(gs)){
	    return {NA,heuristic(gs),nullptr};
	}
	if (!gs->selfTurn){
		return calculateDealerOptions(gs);
	}
    Turn ret = {NA, LOW_HEURISTIC, nullptr};
    if (liveChance(gs) == 1) {
	    ret.action = SHOOT_OTHER;
		ret.heuristic = turnHeuristic(gs, ret);
		return ret;
	} else if (liveChance(gs) == 0) {
	    ret.action = SHOOT_SELF;
		ret.heuristic = turnHeuristic(gs, ret);
		return ret;
	} else {
		Turn t;
		// try all actions
		std::vector<ACTION> actions = getMoves(gs);
		for (ACTION a : actions){
		    t.action=a;
			t.heuristic=turnHeuristic(gs, t);
			if (start) {
				Print(t);
			}
			if (t.heuristic > ret.heuristic){
			    ret = t;
			}
		}
		return ret;
	}
}

float turnHeuristic(GameState* gs, Turn t){
	float lc = liveChance(gs);
    switch (t.action){
	case SHOOT_ANY:
	{
		return (turnHeuristic(gs, {SHOOT_SELF,0,nullptr}) +
			turnHeuristic(gs, {SHOOT_OTHER,0,nullptr}))/2;
	}
	case USE_PILLS:
		{
		GameState gs_blank = DoMove(*gs, t, true);
		GameState gs_live = DoMove(*gs, t, false);
		return (calculateMoves(&gs_live).heuristic +
			calculateMoves(&gs_blank).heuristic)/2;
		}
	case USE_CIGS:
	case USE_SAW:
	case USE_CUFFS:
		{
		GameState next = DoMove(*gs, t, false);
		return calculateMoves(&next).heuristic;
		break;
		}
	case USE_BEER:
	case USE_GLASS:
	case SHOOT_OTHER:
	case SHOOT_SELF:
	    if (lc == 1){
		    GameState next = DoMove(*gs, t, false);
		    return calculateMoves(&next).heuristic;
		} else if (lc == 0){
		    GameState next = DoMove(*gs, t, true);
		    return calculateMoves(&next).heuristic;
		} else {
		    GameState gs_blank = DoMove(*gs, t, true);
		    GameState gs_live = DoMove(*gs, t, false);
			return lc * calculateMoves(&gs_live).heuristic +
			  (1-lc) * calculateMoves(&gs_blank).heuristic;
		}
	}
	return 0;
}

float heuristic(GameState *gs){
	if (DealerHp(gs) <= 0) {
	    return 1000;
	}
	if (PlayerHp(gs) <= 0) {
	    return -10000;
	} else {
		float hpFactor = (200 * PlayerHp(gs)) - 20 * DealerHp(gs);
		float itemFactor = 0;
		if (gs->selfTurn){
			itemFactor = 5*gs->itemsSelf.size();
			itemFactor -= gs->itemsOther.size();
		} else {
			itemFactor = 5*gs->itemsOther.size();
			itemFactor -= gs->itemsSelf.size();
		}
		return hpFactor + itemFactor;
	}
}
