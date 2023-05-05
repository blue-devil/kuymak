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
 *
 *  KUYMAK
 *  A tasty cross-platform tool from Blacksea to debug shellcode!
 *
 *  How to compile:
 *  If you have installed GCC or MingW on your host platform, you can compile
 *  as simple as with this command:
 *
 *  $ gcc -Wall kuymak.c -o kuymak
 *
 *  You can compile for Windows if you installed cross-compilers.
 *  Windows x86_64:
 *  $ x86_64-w64-mingw32-gcc -Wall kuymak.c -o kuymak.exe
 *  Windows x86_32:
 *  $ i686-w64-mingw32-gcc -Wall kuymak.c -o kuymak.exe
 *
 *  How to run:
 *  Save your shellcode to a binary file and run as easily as:
 *  $ ./kuymak -b shellcode.bin
 *
 *  You can run your shellcode with "\x" specifiers directly from command line
 *  $ ./kuymak -c "\x48\x83\xEC\x28\x48 <snipped> \x00\x48\x8D"
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h> // getopt defined here
#ifdef __linux__
#include <sys/mman.h> // mmap defined here
#elif _WIN64
#include <windows.h>
// #include <WinBase.h>
#elif _WIN32
#include <windows.h>
#elif __APPLE__
#include <sys/mman.h>
#endif

const char *ver = "0.4.0";
const char *date = "06/05/2023";
unsigned char *shellcode;

void usage()
{
    fprintf(stdout, "KUYMAK v%s\n\n", ver);
    fprintf(stdout, "[*] USAGE:\n");
    fprintf(stdout, "\tkuymak -b shellcode.bin\n");
    fprintf(stdout, "\tkuymak -c \\x0f\\x01\\xf8\\xe8\\x05\\x00\\x00\\x00"
                    "\\x0f\\x01\\xf8\\x48\\xcf\n\n");
    fprintf(stdout, "[*] Options:\n");
    fprintf(stdout, "\t-b\tshellcode as a binary\n");
    fprintf(stdout, "\t-c\tshellcode as char array\n");
    fprintf(stdout, "\t-h\tprints this help\n");
    return;
}

void print_banner(void)
{
    char *bannerfmt =
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
        " |     |  | @version  : %s                                  |  |     |\n"
        " +<===>+--| @date     : %s                             |--+<===>+\n"
        "       |  | @license  : GPLv3                                  |  |\n"
        "        -+| @info     : Cross-platform shellcode runner        |+-\n"
        "          |____________________________________________________|\n\n";
    char banner[2048];
    sprintf(banner, bannerfmt, ver, date);
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
    // allocate an executable/readable/writable area inside memory
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
 * get_shellcode2 - gets shellcode from commandline
 * @sc: shellcode from commandline as "\x53\x43\X54"
 *
 * Return value:    SUCC    file size
 *                  FAIL    exit process
 */
ssize_t get_shellcode2(char *sc)
{
    static ssize_t file_size = 0;
    file_size = strlen(sc) / 4 + 1;

    // allocate space in heap as much shellcode size
    shellcode = (unsigned char *)malloc(file_size + 1);
    if (shellcode == NULL)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    // lowercase the sc bytes if user uses \X instead of \x
    char c;
    for (int i = 0; i < strlen(sc); i++)
    {
        c = sc[i];
        sc[i] = tolower(c);
    }

    size_t i = 0;
    for (char *tok = strtok(sc, "\\x"); tok; tok = strtok(NULL, "\\x"))
    {
        sscanf(tok, "%02hhx", shellcode + i);
        i++;
    }

    return file_size;
}

/**
 * run_shellcode - takes shellcode's pointer as a parameter; jumps that
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

    while ((opt = getopt(argc, argv, "hb:c:")) != -1)
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
            fprintf(stdout, "[*] Shellcode as chars : %s\n", optarg);
            run_shellcode(map_shellcode(get_shellcode2(optarg)));
            break;
        default:
            // usage();
            exit(EXIT_FAILURE);
        }
    }
    return EXIT_SUCCESS;
}
