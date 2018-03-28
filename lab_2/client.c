#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "transfer-methods.c"
#include "constants.c"
#include "json-methods.c"
#include <jansson.h>

int main(int argc, char **argv)
{
  int server_port = atoi(argv[1]);
  char message[BUFF_SIZE];
  printf("Введите строку для поиска: ");
  scanf("%s", message);
  char buf[BUFF_SIZE];
  int sock;
  struct sockaddr_in addr;
  sock = socket(AF_INET, SOCK_STREAM, 0);

  if (sock < 0)
  {
    perror("socket");
    exit(1);
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(server_port);
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

  if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
  {
    perror("connect");
    exit(2);
  }

  int flag = 1;

  while(flag) {
    int meFlag = 1;
    int heFlag = 1;

    while(meFlag) {
      send_all(sock, message, BUFF_SIZE);
      printf("ME: %s\n", message);
      fflush(stdout);
      meFlag = 0;
    }

    while(heFlag) {
      printf("Wait for data..\n");
      recv_all(sock, buf, BUFF_SIZE);
      printf("\n RESULT FROM SERVERS: \n------------\n%s\n", buf);
      json_t *obj = load_json(buf);
      fflush(stdout);
      heFlag = 0;
    }
    close(sock);
    flag = 0;
  }
  return 0;
}