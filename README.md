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
    - `$path$` can be relative or absolute.
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

---

#### Manipulation

- **`open $path$`**  
  Open a file or directory.

- **`write $path$`**  
  Writes to a file.

- **`read $path$`**  
  Read from a file.\
  Here is the correct way to **read all data** from an **opened** file.
  ```C++
   // f is pointer to a file
   auto eof = f->getEOF();                  // data count/end of file cursor
   if(eof < 0)return -1;                    // error: file not opened
   char *data = new char[eof + 1]{0};       // + 1 for null terminator
   f->seek(0);                              // set cursor to file start
   f->read(data, eof); 
   std::cout << data;                       // print data
   delete[] data; 
  ```

- **`eof $path$`**  
  Returns a cursor to the end of file.

- **`cursor $path$`**  
  Returns a cursor.

- **`seek $path$ $cursor$`**  
  Set a cursor. Should be used in combination with `cursor` or `eof`.

- **`close $path$`**  
  Close a file or directory.

- **`rename $path$ $name$`**  
  Rename a file or directory to `$name$`.

- **`remove $path$`**  
  Remove a file or directory.

---

#### Other Printing

- **`oft [last_entry_to_print]`**  
  List the Open File Table (OFT).

- **`fat [last_entry_to_print]`**  
  List the File Allocation Table (FAT).

- **`exit`**  
  Quit the console.