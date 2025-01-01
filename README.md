# FAT-File-System-Simulation

This is <b>my FAT file system simulation.</b> <br>

* FAT entry is 1B

* File `harddisk.txt` represents a disk. <br>
  `HDisk` is a singleton class which has a method to read and a method to write in a given block. <br>
  Those are the only 2 operations that are possible to perform on a disk.
    * There are $2^8$ blocks
    * Each block is $2^8$*1B

## Console Commands

Using `Console::open();` and typing `help pls`, these commands will show up.

### Argument Conventions

- `$arg_name$` - Required arguments
- `[arg_name]` - Optional arguments

---

### Commands

#### Directory Traversal

- **`ls`**  
  List files in the current directory.

- **`tree root`**  
  Print the directory tree starting from the root.

- **`cd $path$`**  
  Change directory.  
  `$path$` can be relative or absolute.

---

#### Manipulation

- **`open $path$`**  
  Open a file or directory.  
  `$path$` can be relative or absolute.

- **`close $path$`**  
  Close a file or directory.  
  `$path$` can be relative or absolute.

- **`rename $path$ $name$`**  
  Rename a file or directory to `$name$`.  
  `$path$` can be relative or absolute.

- **`remove $path$`**  
  Remove a file or directory.  
  `$path$` can be relative or absolute.

---

#### Other Printing

- **`oft [last_entry_to_print]`**  
  List the Open File Table (OFT).  
  Optionally specify `last_entry_to_print`.

- **`fat [last_entry_to_print]`**  
  List the File Allocation Table (FAT).  
  Optionally specify `last_entry_to_print`.

- **`exit`**  
  Quit the console.