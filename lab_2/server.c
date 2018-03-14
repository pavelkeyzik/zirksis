#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "transfer-methods.c"
#include "constants.c"
#include <jansson.h>

char *searchText(char *text)
{
  char first[] = "grep -r '";
  char second[sizeof(text)];
  strcpy(second, text);
  char third[] = "' ./files/*.txt > founded.txt";

  strncat(first, second, sizeof(second));
  strncat(first, third, sizeof(third));

  system(first);
  return "Hello";
}

int main(int argc, char **argv)
{
  int app_port = atoi(argv[1]);
  int sock, listener;
  struct sockaddr_in addr;
  char buf[BUFF_SIZE];
  int bytes_read;
  listener = socket(AF_INET, SOCK_STREAM, 0);

  if (listener < 0)
  {
    perror("socket");
    exit(1);
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(app_port);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  printf("Server listen on port %d...\n", app_port);
  fflush(stdout);
  if (bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
  {
    perror("bind");
    exit(2);
  }

  listen(listener, 1);

  while (1)
  {
    sock = accept(listener, NULL, NULL);
    if (sock < 0)
    {
      perror("accept");
      exit(3);
    }
    int flag = 1;
    char *response;

    while(flag) {
      int meFlag = 1;
      int heFlag = 1;

      while(meFlag) {
        recv_all(sock, buf, BUFF_SIZE);
        response = searchText(buf);
        printf("CLIENT: %s\n", buf);
        fflush(stdout);
        meFlag = 0;
      }

      while(heFlag) {
        send_all(sock, response, BUFF_SIZE);
        printf("ME: '%s'\n", response);
        fflush(stdout);
        heFlag = 0;
      }
      flag = 0;
    }
  }
  close(sock);
  return 0;
}