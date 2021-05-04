//color macros
#define RESET       "\033[0m"
#define RED         "\033[31m"
#define BOLDRED     "\033[1m\033[31m"
#define GREEN       "\033[32m"
#define BOLDGREEN   "\033[1m\033[32m" 
#define BOLDYELLOW  "\033[1m\033[33m"
#define BOLDWHITE   "\033[1m\033[37m" 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <rc4_enc.c>
#include <rc4_skey.c>
#include <rc4_local.h>
#include <openssl/rand.h>
#include <openssl/rc4.h>
#include <openssl/evp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
//------------------------------------------------
// Fernando's encryption function
//------------------------------------------------

#define SALTED_STR_LEN 16
#define SALT_LEN 8
#define FIXED_KEY_SIZE 16

char* getKey(const char *path)
{
    char *absolutePath = (char *)malloc(sizeof(char)*100);;
    char c = '/';
    strcpy(absolutePath, getenv("HOME"));
    strncat(absolutePath, &c, 1); //adds '/' after {user}
    absolutePath = strcat(absolutePath,path);
    printf("path to key file: %s\n",absolutePath);
    FILE *filePtr = fopen(absolutePath,"r");
    if (filePtr == NULL)
        return "passwordpassword";//if the file is not found use default key
    fseek (filePtr,0,SEEK_END); // get file length with fseek and ftell system calls
    int fileLength = ftell(filePtr);
    fseek (filePtr,0,SEEK_SET);
    
    char* fileCpy = (char*)malloc(fileLength*sizeof(*fileCpy)); 
    char buffer[1];
    int i= 0;

    printf("reading key... \n");
    while (fread(buffer,1,1,filePtr) == 1) //reads through file and copies to fileCpy
    {
        fileCpy[i++] = buffer[0];
    }
    fclose(filePtr);
    //maybe free memory if it works :)    
    printf("done.\n");
    printf("read key from %s: %s\n", absolutePath,fileCpy);
    fileCpy[16] = '\0';
    return fileCpy;
}


void printUsage()
{
    printf( BOLDWHITE"Usage:"RESET" ./rc4 {inputFile} {outputFile} {key} {-en/-de} {optional:-nosalt}\n" );
}
void encrypt_file(int ifd, int ofd, char*rawKey,int mode)
{
    struct stat sb;
    if (fstat(ifd,&sb)==-1)
        perror("stat");
    int blockSize = sb.st_blksize; // block size for stream writin
    int keySize = strlen(rawKey);
    RC4_KEY* key = (RC4_KEY*)malloc(sizeof(*key));
    unsigned char* salt = (unsigned char *)malloc(SALT_LEN*sizeof(*salt));
    unsigned char* salted__ = (unsigned char *)malloc(SALTED_STR_LEN*sizeof(*salted__));
    unsigned char* hashedKey = (unsigned char *)malloc(FIXED_KEY_SIZE*sizeof(*hashedKey));
    unsigned char* inBuf = (unsigned char *)malloc(blockSize*sizeof(*inBuf));
    unsigned char* outBuf = (unsigned char *)malloc(blockSize*sizeof(*outBuf));

    int offset = 0;
    //generate key
    if (mode==1) //we generate salt
        {
            RAND_bytes(salt, SALT_LEN);
            EVP_BytesToKey(EVP_rc4(), EVP_sha256(), (const unsigned char *)salt, (const unsigned char *)rawKey, FIXED_KEY_SIZE, 1, hashedKey, NULL);
            //salt output
            sprintf((char *)salted__,"Salted__%s",(char *)salt);
            write(ofd,salted__,SALTED_STR_LEN);
            offset+=SALTED_STR_LEN;
        }

    else if(mode==2)//no salt 
        {
            EVP_BytesToKey(EVP_rc4(), EVP_sha256(), NULL, (const unsigned char *)rawKey, FIXED_KEY_SIZE, 1, hashedKey, NULL);
        }
    //set key
    RC4_set_key(key,FIXED_KEY_SIZE,(const unsigned char*)hashedKey);

    //do encryption
    int bytes;
    
    while (bytes = read(ifd,inBuf,blockSize))
    {
        RC4(key, bytes, inBuf, outBuf);
        write(ofd, outBuf, bytes);
        offset += bytes;
    }
    ftruncate(ofd,offset);//getting extra bytes on nosalt so using truncate
}
void decrypt_file(int ifd, int ofd, char*rawKey, int mode)
{
    struct stat sb;
    if (fstat(ifd,&sb)==-1)
        perror("stat");
    int blockSize = sb.st_blksize; // block size for stream writin
    int keySize = strlen(rawKey);
    RC4_KEY* key = (RC4_KEY*)malloc(sizeof(*key));
    unsigned char* salt = (unsigned char *)malloc(SALT_LEN*sizeof(*salt));
    unsigned char* salted__ = (unsigned char *)malloc(SALTED_STR_LEN*sizeof(*salted__));
    unsigned char* hashedKey = (unsigned char *)malloc(FIXED_KEY_SIZE*sizeof(*hashedKey));
    unsigned char* inBuf = (unsigned char *)malloc(blockSize*sizeof(*inBuf));
    unsigned char* outBuf = (unsigned char *)malloc(blockSize*sizeof(*outBuf));

    //get salt from ifd

    if (mode == 3)
        {
            lseek(ifd,0,SEEK_SET);
            read(ifd, salted__, SALTED_STR_LEN);
            for(int i = 8; i < SALTED_STR_LEN; i++)
                salt[i-8] = salted__[i]; 
            salt = salted__ + SALT_LEN; //assignes only salt bytes
            EVP_BytesToKey(EVP_rc4(), EVP_sha256(), (const unsigned char *)salt, (const unsigned char *)rawKey, FIXED_KEY_SIZE, 1, hashedKey, NULL);
        }
    else if (mode == 4)
        EVP_BytesToKey(EVP_rc4(), EVP_sha256(), NULL, (const unsigned char *)rawKey, FIXED_KEY_SIZE, 1, hashedKey, NULL);
    //set key
    RC4_set_key(key,FIXED_KEY_SIZE,(const unsigned char*)hashedKey);
    //do decryption
    int bytes;
    int offset = 0;
    while (bytes = read(ifd,inBuf,blockSize))
    {
        RC4(key, bytes, inBuf, outBuf);
        write(ofd, outBuf, bytes);
        offset += bytes;
    }
    ftruncate(ofd,offset);//should be correct size since file is only appended new data but just incase;
}
int main (int argc, char* argv[])
{
    if (argc<4)
        {
            printf(BOLDRED"Error:"RESET " too few arguments""\n");
            printUsage();
            exit(1);
        }
    //set mode based on args
    int ifd, ofd, mode;
    if (strcmp(argv[4],"-en") == 0)
        mode = 1;
    else if (strcmp(argv[4],"-de") == 0)
        mode = 3;
    if (argc == 5)
        printf("mode set: salted : %d\n",mode);
    else if (strcmp(argv[5],"-nosalt") == 0)
        {
            mode++;
            printf("mode set: unsalted : %d\n",mode);
            
        }
    //check if arg1 is not real file
    if (access(argv[0], F_OK) != 0)
    {
        printf(BOLDRED"Error:"RESET " no file with name \"%s\" in current directory "RESET"\n", argv[1]);
        printUsage();
        exit(1);
    } 
        
    ifd = open(argv[1], O_RDONLY);
    ofd = open(argv[2], O_CREAT | O_WRONLY,0644);

    if (mode == 1||mode == 2)           // 1  encrypt with salt //2 encrypt nosalt
        encrypt_file(ifd,ofd,argv[3],mode);
    else if(mode == 3 || mode == 4)     //3  decrypt with salt //4 decrypt nosalt
        decrypt_file(ifd,ofd,argv[3],mode);
    //close files
    close(ifd);
    close(ofd);
    exit(0);
}