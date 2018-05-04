buildServer:
	gcc server.c -o server -lcrypto -lssl -ljansson

buildClient:
	gcc client.c -o client -ljansson -lssl -lcrypto

buildServ:
	gcc -Wall -o ssl-server server-app.c -L/usr/lib -ljansson -lssl -lcrypto

buildClie:
	gcc -Wall -o ssl-client client-app.c -L/usr/lib -ljansson -lssl -lcrypto