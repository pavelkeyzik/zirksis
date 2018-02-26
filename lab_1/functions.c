#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <fcntl.h>
#include <unistd.h>

#define PRIVAT "./privat.key"
#define PUBLIC "./public.key"

void generateKeys(char *passwordPhrase) {
    RSA * rsa = NULL;
    unsigned long bits = 1024; /* длина ключа в битах */
    FILE *priv_key_file = NULL, *pub_key_file = NULL;

    /* контекст алгоритма шифрования */
    const EVP_CIPHER *cipher = NULL;
    priv_key_file = fopen(PRIVAT, "wb");
    pub_key_file = fopen(PUBLIC, "wb");

    /* Генерируем ключи */
    rsa = RSA_generate_key(bits, RSA_F4, NULL, NULL);

    /* Формируем контекст алгоритма шифрования */
    OpenSSL_add_all_ciphers();
    cipher = EVP_get_cipherbyname("bf-ofb");
    
    /* Получаем из структуры rsa открытый и секретный ключи и сохраняем в файлах.
    * Секретный ключ шифруем с помощью парольной фразы "hello"
    */
    PEM_write_RSAPrivateKey(priv_key_file, rsa, cipher,
    NULL, 0, NULL, passwordPhrase);
    PEM_write_RSAPublicKey(pub_key_file, rsa);
    
    fclose(pub_key_file);
    fclose(priv_key_file);
    /* Освобождаем память, выделенную под структуру rsa */
    RSA_free(rsa);
}

void encrypt(char *enteredFile, char *outputFile) {
    RSA * pubKey = NULL;
    unsigned char *ptext, *ctext;
    FILE * pub_key_file = NULL;
    /* Открываем входной и создаем выходной файлы */
    int inf = open(enteredFile, O_RDWR);
    int outf = open(outputFile, O_CREAT|O_TRUNC|O_RDWR, 0600);

    /* Считываем открытый ключ */
    pub_key_file = fopen(PUBLIC, "rb");
    pubKey = PEM_read_RSAPublicKey(pub_key_file, NULL, NULL, NULL);
    if(pubKey == NULL) {
        printf("pubKey is NUULLLL");
        exit(-1);
    }
    /* Определяем длину ключа */
    int key_size = RSA_size(pubKey);
    ptext = malloc(key_size);
    ctext = malloc(key_size);

    /* Шифруем содержимое входного файла */

    unsigned long inlen = 0 , outlen = 0;

    while(1) {
        inlen = read(inf, ptext, key_size - 11);
        if(inlen <= 0) break;

        outlen = RSA_public_encrypt(inlen, ptext, ctext, pubKey, RSA_PKCS1_PADDING);
        if(outlen != RSA_size(pubKey)) exit(-1);

        write(outf, ctext, outlen);
    }

    fclose(pub_key_file);
}

void decrypt(char *enteredFile, char *outputFile, char *passwordPhrase) {
    RSA *privKey = NULL;
    FILE *priv_key_file;
    unsigned char *ptext, *ctext;

    /* Открываем входной и создаем выходной файл */
    int inf = open(enteredFile, O_RDWR);
    int outf = open(outputFile, O_CREAT|O_TRUNC|O_RDWR, 0600);

    /* Открываем ключевой файл и считываем секретный ключ */
    priv_key_file = fopen(PRIVAT, "rb");
    privKey = PEM_read_RSAPrivateKey(priv_key_file, NULL, NULL, passwordPhrase);
    if(privKey == NULL) {
        printf("privKey is NUULLLL");
        exit(-1);
    }

    /* Определяем размер ключа */
    int key_size = RSA_size(privKey);
    ctext = malloc(key_size);
    ptext = malloc(key_size);
    
    unsigned long inlen = 0, outlen = 0;

    /* Дешифруем файл */
    while(1) {
        inlen = read(inf, ctext, key_size);
        if(inlen <= 0) break;
        
        outlen = RSA_private_decrypt(inlen, ctext, ptext, privKey, RSA_PKCS1_PADDING);
        if(outlen < 0) exit(0);
        
        write(outf, ptext, outlen);
    }

    fclose(priv_key_file);
}