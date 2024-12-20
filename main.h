#ifndef MAIN_H_
#define MAIN_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>

#define PORT 16855
//send 2 uint 8s, action,

typedef enum {
	EMPTY=0x0,
    MAGNIFY,
	CIGARETTE,
	BEER,
	HANDSAW,
	HANDCUFF,
	PHONE,
	INVERTER,
	MEDS=0x8,
	ADRENALINE=0x10, // to steal with adrenaline you send 16 + item
					// stealing meds being 16+8 = 24 = 0xf
} item;

typedef enum {
	SELF=0x40,
	OPP=0x41,
} target;

typedef enum {
	WAITING,
	SETTING_UP,
	GIVING_ITEMS,
	P1_TURN,
	P2_TURN,
	P1_WIN,
	P2_WIN,
} game_state;

typedef struct {
	int charges;
    item items[8];
} player;

typedef struct {
    int shots;
    int live;
    int max_charges;
    int n_player;
    int turn;
	player players[2];
} worldstate;

typedef struct {
	struct pollfd* fd;
	player* associated;
	struct sockaddr_in6 clientname;
} conn;

int handle_player(player* p);
int init_world(worldstate* w);
int new_round(worldstate* w);

#endif // MAIN_H_
