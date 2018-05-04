#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "transfer-methods.c"
#include "constants.c"
#include "json-methods.c"
#include <jansson.h>
#include "openssl-methods.c"

#define SSL_SERVER_RSA_CERT	"/home/pavel/Documents/psu/zirksis/lab_2/ssl_server.crt"
#define SSL_SERVER_RSA_KEY	"/home/pavel/Documents/psu/zirksis/lab_2/ssl_server.key"
#define SSL_SERVER_RSA_CA_CERT	"/home/pavel/Documents/psu/zirksis/lab_2/ca.crt"
#define SSL_SERVER_RSA_CA_PATH	"/home/pavel/Documents/psu/zirksis/lab_2/"
#define SSL_SERVER_ADDR	"/home/pavel/Documents/psu/zirksis/lab_2/server"
#define OFF	0
#define ON	1

char *searchText(char *text)
{
  char first[] = "grep -r '";
  char second[sizeof(text)];
  strcpy(second, text);
  char third[] = "' ./files/*.txt > founded.txt";

  strncat(first, second, sizeof(second));
  strncat(first, third, sizeof(third));

  system(first);

  json_t *obj = json_object(), *arr = json_array();
  FILE *file;
  char *string;
  char str[255];
  file = fopen("founded.txt", "r");

  while(feof(file) == 0) {
    fscanf(file, "%[^\n]\n", str);
    json_array_append(arr, json_string(str));
  }
  json_object_set(obj, "SERVER_1", arr);
  fclose(file);
  char *jsonParsed = json_dumps(obj, sizeof(obj));
  return jsonParsed;
}

int main(int argc, char **argv)
{
  int app_port = atoi(argv[1]);
  int sock, listener;
  struct sockaddr_in addr;
  char buf[BUFF_SIZE];
  int bytes_read;

  int verify_peer = ON;
  SSL_METHOD *server_meth;
  SSL_CTX *ssl_server_ctx;
  int serversocketfd;
  int clientsocketfd;
  int handshakestatus;

  SSL_library_init();
  SSL_load_error_strings();
  server_meth = SSLv3_server_method();
  ssl_server_ctx = SSL_CTX_new(server_meth);

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
    SSL *serverssl;
    sock = accept(listener, NULL, NULL);
    serverssl = SSL_new(ssl_server_ctx);
    int ret;
    if(!serverssl)
		{
			printf("Error SSL_new\n");
			return -1;
		}
    SSL_set_fd(serverssl, sock);
    if((ret = SSL_accept(serverssl))!= 1)
    {
      printf("Handshake Error %d\n", SSL_get_error(serverssl, ret));
      return -1;
    }

    if(verify_peer)
		{
			X509 *ssl_client_cert = NULL;

			ssl_client_cert = SSL_get_peer_certificate(serverssl);
			
      if(ssl_client_cert)
      {
        long verifyresult;

        verifyresult = SSL_get_verify_result(serverssl);
        if(verifyresult == X509_V_OK)
          printf("Certificate Verify Success\n"); 
        else
          printf("Certificate Verify Failed\n"); 
        X509_free(ssl_client_cert);				
      }
      else
        printf("There is no client certificate\n");
      }

    if (sock < 0)
    {
      perror("accept");
      exit(3);
    }

    SSL_load_error_strings();
    ERR_load_crypto_strings();

    OpenSSL_add_all_algorithms();
    SSL_library_init();

    SSL_CTX *ctx = SSL_CTX_new(SSLv3_server_method());
    
    if(!ssl_server_ctx)
    {
      ERR_print_errors_fp(stderr);
      return -1;
    }
    
    if(SSL_CTX_use_certificate_file(ssl_server_ctx, SSL_SERVER_RSA_CERT, SSL_FILETYPE_PEM) <= 0)	
    {
      ERR_print_errors_fp(stderr);
      return -1;		
    }

    
    if(SSL_CTX_use_PrivateKey_file(ssl_server_ctx, SSL_SERVER_RSA_KEY, SSL_FILETYPE_PEM) <= 0)	
    {
      ERR_print_errors_fp(stderr);
      return -1;		
    }
    
    if(SSL_CTX_check_private_key(ssl_server_ctx) != 1)
    {
      printf("Private and certificate is not matching\n");
      return -1;
    }	

    if(verify_peer)
    {	
      //See function man pages for instructions on generating CERT files
      if(!SSL_CTX_load_verify_locations(ssl_server_ctx, SSL_SERVER_RSA_CA_CERT, NULL))
      {
        ERR_print_errors_fp(stderr);
        return -1;		
      }
      SSL_CTX_set_verify(ssl_server_ctx, SSL_VERIFY_PEER, NULL);
      SSL_CTX_set_verify_depth(ssl_server_ctx, 1);
  }
    
    
    // if (ctx == NULL) {
    //   printf("errored; unable to load context.\n");
    //   ERR_print_errors_fp(stderr);
    //   return -3;
    // }

    // EVP_PKEY *pkey = generatePrivateKey();
    // X509 *x509 = generateCertificate(pkey);

    // SSL_CTX_use_certificate(ctx, x509);
    // SSL_CTX_set_default_passwd_cb(ctx, password_cb);
    // SSL_CTX_use_PrivateKey(ctx, pkey);

    // RSA *rsa=RSA_generate_key(512, RSA_F4, NULL, NULL); 

    // SSL_CTX_set_tmp_rsa(ctx, rsa); 
    // RSA_free(rsa);
    // SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, 0);
        
    // SSL *ssl = SSL_new(ctx);
    // BIO *accept_bio = BIO_new_socket(cfd, BIO_CLOSE);

    // SSL_set_bio(ssl, accept_bio, accept_bio);
    // SSL_accept(ssl);
    // ERR_print_errors_fp(stderr);

    // BIO *bio = BIO_pop(accept_bio);


    // sslctx = SSL_CTX_new( SSLv23_server_method());
    // SSL_CTX_set_options(sslctx, SSL_OP_SINGLE_DH_USE);
    // int use_cert = SSL_CTX_use_certificate_file(sslctx, "/serverCertificate.pem" , SSL_FILETYPE_PEM);
    // int use_prv = SSL_CTX_use_PrivateKey_file(sslctx, "/serverCertificate.pem", SSL_FILETYPE_PEM);
    // cSSL = SSL_new(sslctx);
    // SSL_set_fd(cSSL, sock);
    // int ssl_err = SSL_accept(cSSL);
    // if(ssl_err <= 0)
    // {
        //Error occurred, log and close down ssl
        // ShutdownSSL(cSSL);
    // }
    
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
  
  return 0;
}