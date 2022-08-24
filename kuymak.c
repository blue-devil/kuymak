/*
 *  Kuymak
 *  Copyright (C) 2022  Blue DeviL<bluedevil.SCT@gmail.com>

 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
/**
 * KUYMAK v0.3.0
 * A tasty cross-platform tool from Blacksea to debug shellcode!
 *
 * How to compile:
 * Linux:
 * $ gcc -Wall kuymak.c -o kuymak
 *
 * Windows x86_64:
 * $ x86_64-w64-mingw32-gcc -Wall kuymak.c -o kuymak.exe
 * Windows x86_32:
 * $ i686-w64-mingw32-gcc -Wall kuymak.c -o kuymak.exe
 *
 * Apple:
 * gcc -Wall kuymak.c -o kuymak
 *
 * How to run
 * $ ./kuymak -b shellcode.bin
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h> // getopt burada tanımlı
#ifdef __linux__
#include <sys/mman.h> // mmap burada tanımlı
#elif _WIN64
#include <windows.h>
//#include <WinBase.h>
#elif _WIN32
#include <windows.h>
#elif __APPLE__
#include <sys/mman.h>
#endif

unsigned char *shellcode;

void usage()
{
    fprintf(stdout, "KUYMAK v0.3.0\n\n");
    fprintf(stdout, "[*] USAGE:\n");
    fprintf(stdout, "\tkuymak -b shellcode.bin\n");
    fprintf(stdout, "\tkuymak -k \\x0f\\x01\\xf8\\xe8\\5\\0\\0\\0\\x0f\\x01"
                    "\\xf8\\x48\\xcf\n\n");
    fprintf(stdout, "[*] Options:\n");
    fprintf(stdout, "\t-b\tshellcode as a binary\n");
    fprintf(stdout, "\t-k\tshellcode as char array (not implemented yet)\n");
    fprintf(stdout, "\t-h\tprints this help\n");
    return;
}

void print_banner(void)
{
    char *banner =
        "    __________________________________________________________________\n"
        "   |         ...-...  .'\\\\   _   __                            _      |\n"
        "   |      ./sh0mmm0hs+\\  o  | | / /                           | |     |\n"
        "   |    ./hmPS1SSSSSSm0y/:  | |/ / _   _ _   _ _ __ ___   __ _| | __  |\n"
        "   |    +mq00000000000pm+   |    \\| | | | | | | '_ ` _ \\ / _` | |/ /  |\n"
        "   |   .mq0001010011000pm.  | |\\  \\ |_| | |_| | | | | | | (_| |   <   |\n"
        " +=|   .mq0001000011000pm.  \\_| \\_/\\__,_|\\__, |_| |_| |_|\\__,_|_|\\_\\  |=+\n"
        " | |    +mq00101010000pm+                 __/ |                       | |\n"
        " | |   //yomooooooooom0+.                |___/ github.com/blue-devil/ | |\n"
        " | |   \\  y+sh0mmm0hs/.                                               | |\n"
        " | |    `-'  '''-''' To kuymak or not to kuymak, that's the question! | |\n"
        " | |__________________________________________________________________| |\n"
        " |         ____________________________________________________         |\n"
        " |      -+| @author   : Blue DeviL <bluedevil.SCT@gmail.com>   |+-      |\n"
        " |     |  | @version  : 0.3.0                                  |  |     |\n"
        " +<===>+--| @date     : 23/08/2022                             |--+<===>+\n"
        "       |  | @license  : GPLv3                                  |  |\n"
        "        -+| @info     : Cross-platform shellcode runner        |+-\n"
        "          |____________________________________________________|\n\n";
    fprintf(stdout, "%s", banner);
}

/**
 * map_shellcode - maps shellcode into the memory's executable space
 * @file_size: size of shellcode
 *
 * Return value: None
 */
void *map_shellcode(ssize_t file_size)
{
#ifdef __linux__
    // allocate an executable/readabşe/writable area inside memory
    void *pSC = mmap(0, sizeof(shellcode),
                     PROT_EXEC | PROT_WRITE | PROT_READ,
                     MAP_ANONYMOUS | MAP_PRIVATE,
                     -1, 0);
    if (pSC == MAP_FAILED)
    {
        perror("mmap");
        free(shellcode);
        exit(EXIT_FAILURE);
    }
#elif _WIN64
    void *pSC = VirtualAlloc(NULL, (size_t)file_size,
                             MEM_COMMIT | MEM_RESERVE,
                             PAGE_EXECUTE_READWRITE);
    if (pSC == NULL)
    {
        fprintf(stdout, "ERROR: VirtualAlloc\n");
        free(shellcode);
        exit(EXIT_FAILURE);
    }
#elif _WIN32
    void *pSC = VirtualAlloc(NULL, (size_t)file_size,
                             MEM_COMMIT | MEM_RESERVE,
                             PAGE_EXECUTE_READWRITE);
    if (pSC == NULL)
    {
        fprintf(stdout, "ERROR: VirtualAlloc\n");
        free(shellcode);
        exit(EXIT_FAILURE);
    }
#elif __APPLE__
    void *pSC = mmap(0, sizeof(shellcode),
                     PROT_EXEC | PROT_WRITE | PROT_READ,
                     MAP_ANONYMOUS | MAP_PRIVATE,
                     -1, 0);
    if (pSC == MAP_FAILED)
    {
        perror("mmap");
        free(shellcode);
        exit(EXIT_FAILURE);
    }
#endif

    // copy our shellcode to allocated mem space
    memcpy(pSC, shellcode, file_size);
    fprintf(stdout, "[*] Shellcode address: 0x%016lX\n",
            (unsigned long)((uintptr_t)pSC));
    //((void (*)(void))pSC)();
    return pSC;
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
    if (fp == NULL)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // get size of shellcode file
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // allocate space in heap as much shellcode size
    shellcode = (unsigned char *)malloc(file_size + 1);
    if (shellcode == NULL)
    {
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
 * run_shellcode - takes shellcodes pointer as a parameter; jumps that
 * memory address and executes shellcode
 *
 * @pSC:    pointer of shellcode
 *
 * Return value : None
 */
void run_shellcode(void *pSC)
{
    printf("[+] Do you wanna run shellcode? (y/n) ");
    char r = getchar();
    if (r == 'y' || r == 'Y')
    {
        printf("\n[+] Executing...\n");
        ((void (*)(void))pSC)();
    }
    else
    {
        printf("[+] https://github.com/blue-devil/kuymak\n");
    }
}

int main(int argc, char **argv)
{
    int opt;
    print_banner();

    if (argc < 2)
    {
        usage();
    }

    while ((opt = getopt(argc, argv, "hb:k:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            usage();
            exit(EXIT_SUCCESS);
        case 'b':
            fprintf(stdout, "[*] Shellcode file : %s\n", optarg);
            run_shellcode(map_shellcode(get_shellcode(optarg)));
            break;
        case 'c':
            fprintf(stdout, "[*] Shellcode as chars : %s", optarg);
            break;
        default:
            // usage();
            exit(EXIT_FAILURE);
        }
    }
    return EXIT_SUCCESS;
}
