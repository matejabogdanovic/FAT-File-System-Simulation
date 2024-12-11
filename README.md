# FAT-File-System-Simulation
This is <b>my FAT file system simulation.</b> <br>
* FAT entry is 1B

* File `harddisk.txt` represents a disk. <br>
`HDisk` is a singleton class which has a method to read and a method to write in a given block. <br>
Those are the only 2 operations which are possible to perform on a disk.
  * There are $2^8$ blocks
  * Each block is $2^8$*1B