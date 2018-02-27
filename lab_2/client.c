#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
  int server_port = atoi(argv[1]);
  char message[10];
  printf("Введите строку для поиска: ");
  scanf("%s", message);
  char buf[sizeof(message)];
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

  send(sock, message, sizeof(message), 0);
  recv(sock, buf, sizeof(message), 0);
  close(sock);

  return 0;
}