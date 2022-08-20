![Kuymak](https://gitlab.com/bluedevil/kuymak/-/raw/master/art/banner_kuymak_eng.png)

# Kuymak

A tasty tool from Blacksea to debug shellcode.

## Building
Host LINUX:
```
$ gcc -Wall kuymak.c -o kuymak
```

Host WINDOWS
Install MSYS2, then install mingw32
```
winget install -e --id msys2.msys2
```
Compile with mingw32
```
 * Windows x86_64:
 * $ x86_64-w64-mingw32-gcc -Wall kuymak.c -o kuymak
 * Windows x86_32:
 * $ i686-w64-mingw32-gcc -Wall kuymak.c -o kuymak
```

Host APPLE
* TODO

## TODO

* [ ] make a version for macOS(use __APPLE__ preprocessor expression)
* [ ] create a shellcode runner
* [ ] get rid of unistd+getopt dependency
* [ ] can we compile it with cl?
* [ ] add changelog

## Sources
Checkout other shellcode runner/debugger projects:
* [BlobRunner][web-blobrunner]
* [Crab-Runner][web-crabrunner]

## LICENSE

This project is licensed under GPLv3

[web-blobrunner]: https://github.com/OALabs/BlobRunner
[web-crabrunner]: https://github.com/cdong1012/Crab-Runner