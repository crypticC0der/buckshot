#include "main.h"

int setup_port(){
	int sockfd = socket(AF_INET6, SOCK_STREAM,0);
	if (sockfd==-1){
		printf("failed to open socket");
		return 1;
	}
    struct sockaddr_in6 servAddr;
	servAddr.sin6_port = htons(PORT);
	servAddr.sin6_family = AF_INET6;
	servAddr.sin6_addr = in6addr_any;
	if (bind(sockfd,(struct sockaddr*)&servAddr, sizeof(servAddr)) == -1){
		printf("failed to bind socket");
		return 1;
	}
	return sockfd;
}

int main(){
	struct pollfd files[3];
	size_t n_files = 1;
	worldstate world;
	conn clients[2];

	files[0].events = POLLIN;

	if ((files[0].fd = setup_port())==-1){
		return 1;
	}

	init_world(&world);

	if (listen(files[0].fd,1)< 0){
		printf("failed to listen");
		return 1;
	}

	while (n_files>0){
		if(poll(files,n_files,60)==-1){;
			printf("poll error");
			return 1;
		}
		//if new connection on socket
		if(files[0].revents != 0){
			//setup client
			clients[n_files-1].fd = files+n_files;
			clients[n_files-1].associated = world.players+n_files-1;
			clients[n_files-1].fd->events = POLLIN;

			//add connection
			size_t s = sizeof(clients[n_files-1].clientname);
			if((files[n_files].fd =
				accept(files[n_files].fd,
					(struct sockaddr*)&clients[n_files-1].clientname,
					(socklen_t*)&s))
				== -1){
				printf("failed to accept");
				return 1;
			}
			n_files++;
		}else{
			//handle player updates
			for(int i=1;i < n_files;i++){
				if(files[i].revents != 0){
					handle_player(world.players+(i-1));
				}
			}
		}
	}

}
