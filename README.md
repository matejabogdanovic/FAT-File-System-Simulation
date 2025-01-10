# FAT-File-System-Simulation

This is <b>my FAT file system simulation.</b> <br>

* FAT entry is 1 byte.

* File `harddisk.txt` represents a disk. <br>
  `HDisk` is a singleton class which has a method to read and a method to write in a given block. <br>
  Those are the only 2 operations that are possible to perform on a disk.
    * There are $2^8$ blocks.
    * Each block is $2^8$ bytes.

## Supported file types

There are 2 possible file types:

- **.mb** - **regular file**.
    - Supports reading, writing and cursor operations.
- **.dir** (or blank) - **directory**.
    - Supports directory traversal operations.

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

- **`open $path$`**\
  Create a file or directory if doesn't exist.
    - If the file exists, it will take an OFT entry.
    - Note that if the directory exists and `open` is called, it will return an error.

- **`write $path$`**  
  Write to a file.

- **`read $path$ [num_of_characters]`**  
  Read from a file.

    - Optionally specify `num_of_characters` to read. If not specified, it will read until the end of the file.\
      It behaves the same as `read $path$ -1`.

  Here is the correct way to **read all data** from an **opened** file.

```C++
 // f is a pointer to the file to read from
auto eof = f->getEOF();                  // data count/end of file cursor
if(eof < 0)return -1;                    // error: file not opened
char *data = new char[eof + 1]{ 0 };       // + 1 for null terminator
f->seek(0);                              // set cursor to file start
f->read(data, eof);
std::cout << data;                       // print data
delete[] data; 
```

- **`eof $path$`**  
  Return a cursor to the end of file.

- **`cursor $path$`**  
  Return a cursor.

- **`seek $path$ $cursor$`**  
  Set a cursor.
    - Should be used in combination with `cursor` or `eof`.

- **`close $path$`**  
  Close a file.
    - Releases the taken OFT entry.

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

## Limitations and possible improvements

- **Maximum file/directory name:** 16 chars
- **Maximum file size:** 252 blocks + 1 block for FCB
    - Possible improvement is to store 8 FCBs in one block, because one FCB is exactly 1/8th of a block.
- **Maximum path length:** 256 chars
    - It's possible to improve this using dynamic allocation or using string type for paths.