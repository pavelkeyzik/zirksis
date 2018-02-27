#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int searchText(char *text)
{
  char first[] = "grep -r '";
  char second[sizeof(text)];
  strcpy(second, text);
  char third[] = "' ./files/*.txt > founded.txt";

  strncat(first, second, sizeof(second));
  strncat(first, third, sizeof(third));

  system(first);
  return 0;
}

int main(int argc, char **argv)
{
  int app_port = atoi(argv[1]);
  int sock, listener;
  struct sockaddr_in addr;
  char buf[1024];
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
    while (1)
    {
      bytes_read = recv(sock, buf, 1024, 0);
      if (bytes_read <= 0)
        break;

      searchText(buf);
      send(sock, buf, bytes_read, 0);
    }
    close(sock);
  }

  return 0;
}