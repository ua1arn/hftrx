unzipLIB
--------
Copyright (c) 2021 BitBank Software, Inc.<br>
Written by Larry Bank<br>
bitbank@pobox.com<br>

What is it?
-----------
An 'embedded-friendly' (aka Arduino) library to extract and decompress files from ZIP archives<br>
<br>

Why did you write it?
---------------------
I wanted to be able to unzip files on MCUs for various projects and I couldn't find any libraries that would do the job. The thing that prevents most Linux open source projects from running on embedded boards is their use of the file system and heap management. There is support for malloc/free on Arduino, but it's better to control how memory is used more directly (e.g. TCM vs FLASH vs DDR).
<br>

What's special about it?
------------------------
I designed the unzip library to not rely on malloc/free, but instead to have a fixed class/structure which could be managed by the user. I also removed the dependency on the file system and instead offer a set of callback hooks to allow zip files to be read from memory, SDCard or any media. These two features give it maximum flexibility to run on low end systems.
<br>

Feature summary:
----------------
- Runs on any MCU with at least 41K of free RAM<br>
- No external dependencies (including malloc/free, file system calls)<br>
- unzip API implemented in C and C++<br>
- ZIP files can come from RAM, FLASH, SDCard or any media by using callbacks<br>
- Decompress files by reading any sized chunk at a time (full sized buffer not needed)<br>
- Compiles on any target CPU<br>
<br>

Documentation:
--------------
Detailed information about the API is in the Wiki<br>
See the examples folder for easy starting points<br>
<br>
