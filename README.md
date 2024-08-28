# CP Clone

This project is a clone of the Unix `cp` command, implemented in C. It provides functionality to copy files and directories, with various options for recursive copying, forced overwriting, and directory handling.

## Features

- Copy individual files
- Copy directories (with `-d` flag)
- Recursive copying (with `-r` flag)
- Forced overwriting (with `-f` flag)
- Path resolution and validation

## Usage

```
./cp_clone [-rfd] <source> <destination>
```

### Options

- `-r`: Recursive copy (copy subdirectories)
- `-f`: Force overwrite of existing files
- `-d`: Copy directories
- `-h`: Display help information

## Building

This project uses a Makefile for building. To compile the project, ensure you're in the project directory and run:

```
make
```

This command will use the Makefile to compile all necessary source files and produce the `cp_clone` executable.

If you need to clean the build artifacts, you can use:

```
make clean
```

## Implementation Details

The project is split into multiple files:

- `main.c`: Contains the main function, command-line argument parsing, and the main_copy function
- `copy.c`: Implements the core copying functionality
- `pathresolve.c`: Handles path resolution
- `copy.h` and `pathresolve.h`: Header files for the respective .c files
- `Makefile`: Defines the build process for the project

Key functions include:

- `main_copy`: The main copying logic
- `parse`: Parses and validates source and destination paths
- `r_copy`: Recursive copying for directories
- `copy`: Copying individual files

## Limitations

- Error handling could be improved in some areas
- Some edge cases may not be fully covered
- The implementation may not perfectly match all features of the original `cp` command

## Contributing

Contributions to improve the functionality, error handling, or performance are welcome. Please submit pull requests or open issues for any bugs or feature requests.