all:
	gcc -Wall -o ex sem-pc.c -g -pthread
	
	gcc -c -fpic rcache_client_lib.c -Wall -Werror
	gcc -shared -o librcache.so rcache_client_lib.o
	gcc -L. -o client rcache_client_m.c -Wall -Werror -lrcache -g
	
	gcc -o server rcache_server_m.c -Wall -Werror -g -pthread
	
run:
	./server

clean:
	rm client
	rm ex
	rm librcache.so
	rm rache_client_lib.o
	rm server