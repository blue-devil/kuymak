# Changelog

## [0.4.0] - 06-05-2023

### Added

- Commandline parameter `-c` activated. Now kuymak reads raw shellcode from
  commandline.
- ARM32 shellcode added.
- ARM32, ARM64 and statically compiled prebuilt binaries are added.

### Changed

- Markdown linting
- Update Changelog according to keepachangelog.
- Markdown updated (mostly cosmetic)

### Fixed

- Shellcode `sc-lin-kydk.bin` wrong string count is fixed.  

## [0.3.0] - 23-08-2022

### Added

- This Changelog created.
- Now asking user if they want to run shellcode or not
- A seperate shellcode runner function added
- macOS support added. Kuymak now can run macOS shellcodes
- macOS "Hello, world!" shellcode added

### Changed

- New todos added

## [0.2.0] - 21-08-2022

### Changed

- Project translated to english for internationality

## [0.1.0] - 29-12-2020

### Added

- Initial commit
- Maps linux shellcode to memory
- Maps windows shellcode to memory

[0.4.0]: https://github.com/blue-devil/kuymak/compare/v0.3.0...HEAD
[0.3.0]: https://github.com/blue-devil/kuymak/releases/tag/v0.3.0
