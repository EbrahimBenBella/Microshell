# Microshell

## Overview
Micro Shell is a simple command-line shell implemented in C. It supports basic shell commands, local variables, environment variable export, and I/O redirection.

## Features
- Execute system commands.
- Support for `cd`, `pwd`, `exit`, and `export`.
- Local variable assignment.
- I/O Redirection:
  - `<` for input redirection.
  - `>` for output redirection.
  - `2>` for error redirection.

## Compilation
To compile Micro Shell, use:
```bash
gcc -o microshell microshell.c
```

## Usage
Run the shell using:
```bash
./microshell
```

### Example Commands
```bash
Micro Shell Prompt > echo Hello
Hello
Micro Shell Prompt > VAR=value
Micro Shell Prompt > echo $VAR
value
Micro Shell Prompt > ls > output.txt
Micro Shell Prompt > cat < output.txt
```

## Code Structure
- `execute_command()`: Parses and executes commands.
- `set_local_variable()`: Handles local variable assignments.
- `get_local_variable()`: Retrieves stored variable values.
- `export_variable()`: Exports local variables to the environment.
- `main()`: Runs the shell loop.

## I/O Redirection
- `< filename`: Reads input from `filename`.
- `> filename`: Writes output to `filename`.
- `2> filename`: Writes error messages to `filename`.

## Error Handling
- Proper handling of missing files.
- Checks for valid commands and redirections.
- Ensures system calls return expected results.



