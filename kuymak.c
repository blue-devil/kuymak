/**
 * KUYMAK v0.2
 * A tasty tool from Blacksea to debug shellcode!
 *
 * How to compile:
 * Linux:
 * $ gcc -Wall kuymak.c -o kuymak
 * 
 * Windows x86_64:
 * $ x86_64-w64-mingw32-gcc -Wall kuymak.c -o kuymak
 * Windows x86_32:
 * $ i686-w64-mingw32-gcc -Wall kuymak.c -o kuymak
 * 
 * Apple
 * //TODO
 *
 * How to run
 * $ ./kuymak -b shellcode.bin
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef __linux__
#include <unistd.h>     // getopt burada tanımlı
#include <sys/mman.h>   // mmap burada tanımlı
#elif _WIN64
#include <windows.h>
//#include <WinBase.h>
#elif _WIN32
#include <windows.h>
#endif

unsigned char *shellcode;

void usage()
{
    fprintf(stdout, "KUYMAK v0.1\n\n");
    fprintf(stdout, "[*] USAGE:\n");
    fprintf(stdout, "\tkuymak -b shellcode.bin\n");
    fprintf(stdout, "\tkuymak -k \\x0f\\x01\\xf8\\xe8\\5\\0\\0\\0\\x0f\\x01"
                    "\\xf8\\x48\\xcf\n\n");
    fprintf(stdout, "[*] Options:\n");
    fprintf(stdout, "\t-b\tshellcode as a binary\n");
    fprintf(stdout, "\t-k\tshellcode as char array\n");
    fprintf(stdout, "\t-h\tprints this help\n");
    return;
}

/**
 * map_shellcode - maps shellcode into the memory's executable space
 * @file_size: size of shellcode
 *
 * Return value: None
 */
void map_shellcode(ssize_t file_size)
{
#ifdef __linux__
    // allocate an executable/readabşe/writable area inside memory
    void *pSC = mmap(0, sizeof(shellcode), 
            PROT_EXEC | PROT_WRITE | PROT_READ, 
            MAP_ANONYMOUS | MAP_PRIVATE, 
            -1, 0);
    if (pSC == MAP_FAILED) {
        perror("mmap");
        free(shellcode);
        exit(EXIT_FAILURE);
    }
#elif _WIN64
    void *pSC = VirtualAlloc(NULL, (size_t)file_size,
            MEM_COMMIT | MEM_RESERVE,
            PAGE_EXECUTE_READWRITE);
    if (pSC == NULL) {
        fprintf(stdout, "ERROR: VirtualAlloc\n");
        free(shellcode);
        exit(EXIT_FAILURE);
    }
#elif _WIN32
    void *pSC = VirtualAlloc(NULL, (size_t)file_size,
            MEM_COMMIT | MEM_RESERVE,
            PAGE_EXECUTE_READWRITE);
    if (pSC == NULL) {
        fprintf(stdout, "ERROR: VirtualAlloc\n");
        free(shellcode);
        exit(EXIT_FAILURE);
    }
#elif __APPLE__
    // TODO
#endif

    // copy our shellcode to allocated mem space
    memcpy(pSC, shellcode, file_size);
    fprintf(stdout, "[*] Shellcode address: 0x%016lX\n",
            (unsigned long)((uintptr_t)pSC));

    return;
}

/**
 * get_shellcode - gets shellcode from its path and returns as char array
 * @shellcode_path: path of shellcode as a binary
 *
 * Return value:    SUCC    file size
 *                  FAIL    exit process
 */
ssize_t get_shellcode(char *shellcode_path)
{
    FILE *fp;
    static ssize_t file_size = 0;

    fp = fopen(shellcode_path, "rb"); 
    if (fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // get size of shellcode file
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // allocate space in heap as much shellcode size
    shellcode = (unsigned char *)malloc(file_size + 1);
    if (shellcode == NULL) {
        perror("malloc");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    // copy shellcode to heap
    fread(shellcode, file_size, 1, fp);

    // close file
    fclose(fp);

    return file_size;
}

/**
 *
 */
void run_shellcode()
{
    /**
     * @TODO
     * - ask for user if he/she wants to execute shellcode
     * - shellcode runner code here
     */
}

int main(int argc, char **argv)
{
    int opt;

    if (argc < 2) {
        usage();
    }

    while ((opt = getopt(argc, argv, "hb:k:")) != -1) {
        switch (opt) {
        case 'h':
            usage();
            exit(EXIT_SUCCESS);
        case 'b':
            fprintf(stdout, "[*] Shellcode file : %s\n", optarg);
            map_shellcode(get_shellcode(optarg));
            break;
        case 'c':
            fprintf(stdout, "[*] Shellcode as chars : %s", optarg);
            break;
        default:
            //usage();
            exit(EXIT_FAILURE);
        }
    }
    return EXIT_SUCCESS;
}
