#include <stdio.h>
#include <openssl/pem.h>
#include "functions.c"

/* Имена ключевых файлов */
// #define PRIVAT "./privat.key"
// #define PUBLIC "./public.key"

// arg[1] - Входной файл
// arg[2] - Ключ
// arg[3] - true - шифрование, false - дешифрование
// arg[4] - Выходной файл

void main(int argc, char **argv)
{
    OpenSSL_add_all_algorithms();
    OpenSSL_add_all_ciphers();
    
    if(argv[3]) {
        generateKeys(argv[2]);
        encrypt(argv[1], argv[4]);
    }
    else {
        decrypt(argv[1], argv[4]);
    }
}