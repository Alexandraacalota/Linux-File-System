Copyright 2022 Alexandra-Maria Calota
# Linux File System

## Description:

- The program represents a C implementation of a hierarchical file system containing files and directories.

- There are multiple possible commands supported by the program, all of them explained down below.

## Supported Commands:

### `touch <filename> [filecontent]`
Creates a file with the specified name and content in the current directory. This function allocates memory to store the new file, including its name, type, parent, and content inside the current directory. It also checks whether another file or directory with the same name already exists, and if so, prints an error message to stdout.

### `ls [arg]`
Lists all the resources stored inside the specified file/directory. If no argument is provided, it lists the resources contained inside the current directory. The `ls` function checks whether there is any content stored inside the specified file or directory and prints its name and content, if available. In case the name of the resource is not found, it prints an error message to stdout.

### `mkdir <dirname>`
Creates a directory with the specified name in the working directory. This function allocates memory to store the new directory, including its name, type, and parent, if the name is not found among the other resources in the current directory. Otherwise, it prints an error message to stdout.

### `cd <path>`
Moves through the file system to the provided destination. It splits the path into tokens and changes the current node to each token, calling itself recursively until there is no path left. It showcases whether the function is called from the main function or from another function, which determines whether to print the error message or not.

### `tree [path]`
Prints the entire structure of the file system, moving through it by calling itself recursively when it encounters a folder. An auxiliary function is implemented for "tree" to store the level of indentation needed to be printed.

### `pwd`
Prints the current directory's absolute path to stdout. It uses a matrix to store the absolute path's elements until reaching the root directory, then prints the elements from the last to the first column of the matrix.

### `rmdir <dirname>`
Deletes the specified directory if it exists and has no content inside. It checks if the directory exists and is empty, remakes the link between the previous and the next resource in the working directory, and frees the allocated memory for storing the directory.

### `rm <filename>`
Deletes the specified file with its content if it exists. It checks if the resource with the given name is a file, remakes the link between the previous and following resource in the current directory, and frees the memory allocated for storing the file and its content.

### `rmrec <resourcename>`
Deletes the specified directory, even if it's not empty, recursively deleting all its contents.

