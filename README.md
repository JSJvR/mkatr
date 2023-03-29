ATR disk image creation tool
----------------------------

This program creates Atari `ATR` disk images from a list of files given
in the command line, in the SpartaDOS / BW-DOS disk format.

Program Usage
-------------

    mkatr [options] output [+attributes] filenames

Options:

- `-b`  Selects the next file in the command line as the _boot_ file,
        the file will be loaded when the computer boots, and must be
        in the standard Atari binary file format.
        The bootloader for 128 bytes per sector needs 613 bytes, from $700 to
        $965, and the bootloader for 256 bytes per sector needs 848 bytes, from
        $700 to $A50, both can be relocated with the `-B` option.

- `-x`  Output image with exact sector count for all available content.
        This will use non-standard sector counts, and return images with
        128 bytes per sector if the image is smaller than about 8MB.

- `-s`  Specify the minimum size of the output image, in bytes. The image
        will be of this size or larger instead of the smaller possible.
        This allows creating images with available free space.

- `-B`  Specify the page (high part of address) of the start of the bootloader.
        Use this option to load games or other software with a low loading
        address that conflicts with the standard address of the bootloader.  A
        safe value is from page 6 instead of the default page 7, but page 4 or
        5 is also possible.

- `-h`  Shows a brief help.

- `-v`  Shows version information.

Also, in front of each file name you can optionally add a list of attributes to
be set for that file:

- `+p`  Marks this file as protected (read-only).

- `+h`  Hide file from directory. This only works in SpartaDOS-X.

- `+a`  Marks the file as "archived". This only works in SpartaDOS-X.

To place files inside a sub-directory, simply add the directory *before*
all the files inside that directory.

The resulting image will be the smaller size that fits all the given files (or
the minimum specified with `-s`), from the following list (except when the `-x`
option is used):

| Sector Count | Sector Size | Total Size | Name                     |
|         ---: |        ---: |       ---: | :---                     |
|       720    |       128   |      90k   | SD                       |
|      1040    |       128   |     130k   | ED                       |
|       720    |       256   |     180k   | DD                       |
|      1440    |       256   |     360k   | DSDD                     |
|      2048    |       256   |     512k   | hard disk                |
|      4096    |       256   |       1M   | hard disk                |
|      8192    |       256   |       2M   | hard disk                |
|     16384    |       256   |       4M   | hard disk                |
|     32768    |       256   |       8M   | hard disk                |
|     65535    |       256   |      16M   | biggest possible image   |

Usage Examples
--------------

To create an image named `disk1.atr` with all the files in the "atari"
directory:

    mkatr disk1.atr atari/*

To create a bootable BW-DOS image with a hidden and protected DOS file inside
a subdirectory and a protected `startup.bat` file outside:

    mkatr bwdos.atr dos/ -b +ph dos/xbw130.dos +p startup.bat

To create an image with only one file that will be loaded at boot:

    mkatr game.atr -b mygame.com


Compilation
-----------

Compile with `make` and copy the resulting `mkatr` program to your bin
folder.

