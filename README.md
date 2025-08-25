# Custom Shell Program

This project implements a simple Unix-like shell in C. The shell provides a prompt where users can enter commands, supports built-in commands (`cd`, `pwd`, `history`, `help`, `exit`), as well as running external programs. It also handles background processes and signal management.

---

## Features

- **Interactive Prompt**  
  Displays the current working directory followed by `$ `.

- **Built-in Commands**
  - `pwd` → Prints the current working directory.  
  - `cd [dir]` → Changes directory. Supports:
    - `cd` → go to home directory  
    - `cd ~` → go to home directory  
    - `cd -` → go to previous directory  
  - `help [command]` → Displays general help or help for a specific command.  
  - `history` → Prints the command history (last 10 commands).  
  - `!!` → Re-runs the last command.  
  - `!<n>` → Re-runs the command at index *n* in history.  
  - `exit` → Exits the shell.  

- **External Commands**  
  Runs system programs (e.g., `ls`, `cat`, etc.) using `fork()` and `execvp()`.

- **Background Execution**  
  Appending `&` at the end of a command runs it in the background.

- **Signal Handling**
  - `Ctrl-C` (`SIGINT`) is caught and does not terminate the shell. Instead, it shows help and redraws the prompt.
  - Background processes (`SIGCHLD`) are handled to avoid zombie processes.

---

## Project Structure

```
.
├── include/
│   ├── method.h        # Function declarations
│   ├── msgs.h          # Error/help messages
├── src/
│   ├── shell.c         # Main entry point
│   ├── method.c        # Core shell logic
├── CMakeLists.txt      # Build configuration
```

---

## Build Instructions

The project uses **CMake** and **GCC/Clang**.  

```bash
# Clone and enter directory
git clone <your-repo-url>
cd <your-repo-name>

# Create build directory
mkdir build && cd build

# Run CMake and build
cmake ..
make
```

This will generate an executable called:

```
./shell
```

---

## Usage

Run the shell:

```bash
./shell
```

Example session:

```
/home/user$ pwd
/home/user
/home/user$ cd /tmp
/tmp$ ls
file1.txt file2.log
/tmp$ history
0   pwd
1   cd /tmp
2   ls
3   history
/tmp$ !!
history
/tmp$ exit
```

---

## Testing

Unit tests are provided with **GoogleTest**. To run them:

```bash
cd build
ctest
```

---

## Limitations

- History is limited to the last **10 commands**.
- Does not support pipes (`|`) or redirection (`>`, `<`).
- Minimal error messages compared to a full Unix shell.

