# MoveFileEx Case-Sensitivity Test

Although Windows' file systems are case-insensitive, they still preserve the case that you used for a file/folder name:

- Create "MyFile.txt" will always show as "MyFile.txt" but access like "myfile.txt" is permitted
- Then try to create "myFile.txt" and this is not permitted because that file name already exists

Now what if you wanted to rename the file while ONLY changing the case?

- "MyFile.txt" -> "MYFILE.txt"

Should work, right? Yes and programmaticaly it works alright on NTFS, but not on FAT-like systems.

## Testing

**Goal:** Change case of a name by renaming in place. I think in the case here I stopped after running a few manual tests. Below results from memory on Windows 10 22H2 (Build: 19045.3086).

**How:** MoveFileEx from Win32 API

- FAT(16): no
- FAT32: no
- NTFS: good (some users report it not working though)
- ExFAT: i don't remember, probably no?
- ReFS: Not available in my Windows 10 edition

## How to check for file system type

`fsutil fsinfo volumeinfo C:`

Example output:

```
File System Name : FAT
File System Name : FAT32
File System Name : exFAT
File System Name : NTFS
```

## Workaround

1. Move "MyFile.txt" to "MyFile.txt_temp"
2. Move "MyFile.txt_temp" to "MYFILE.txt"

## Context

This is an ancient but seemingly forgotten bug. While Windows swears that the NT kernel supports case-sensitivity, but it only does so internally, to support the POSIX subsystem (and later WSL, version 1, the "Bash for Windows" kind).

There are no API facing functions to get case-sensitivity for yourself. You can read more in this blog post: https://www.tiraniddo.dev/2019/02/ntfs-case-sensitivity-on-windows.html

MoveFile / MoveFileEx WILL NOT RAISE AN ERROR. The renaming "A.txt" -> "a.txt" will not happen but the return value will indicate SUCCESS.

To me it appears like an file system driver bug/limitation, not a file system issue.

## Further work

Prettier code, automate with a batch script to run the program or even create / remove the temporary drives.

Test ReFS and ExFAT again. Test such renames from within WSL/WSL2.
