# Ginnungagap
A program for editing MXE files in Valkyria Chronicles 4.
This program is capable of fully-unpacking and fully-repacking a complete MXE (database) file. Currently, extracting and re-inserting external strings held in XLB files is not supported.
Definitions of each Entity and Parameter Set defined in the MXEs are provided in JSON files in the `assets` directory. The JSONs do not currently support duplicate keys.

There is no detailed guide for modding using MXEs that I am currently aware of.

Currently this is provided only as a command-line program, with the following usage:
```
Usage: [program path] <-u/-p/--unpack/--pack> [-r] [-t] <input file/directory> <output file/directory>
    -u <input file> <output directory>              : Unpacks a single MXE file to a directory of CSVs, written to <output directory>
    --unpack <input file> <output directory>        : Unpacks a single MXE file to a directory of CSVs, written to <output directory>
    -u -r <input directory> <output directory>      : Unpacks a directory of MXE files to CSVs, written to <output directory>
    --unpack -r <input directory> <output directory>: Unpacks a directory of MXE files to CSVs, written to <output directory>
    -p <input directory> <output directory>         : Packs a directory containing CSV files to a single MXE file, written to <output directory>
    --pack <input directory> <output directory>     : Packs a directory containing CSV files to a single MXE file, written to <output directory>
    -p -r <input directory> <output directory>      : Packs multiple directories containing CSV files to corresponding MXE files, written to <output directory>
    --pack -r <input directory> <output directory>  : Packs multiple directories containing CSV files to corresponding MXE files, written to <output directory>
    Add -t for 'terse mode'; where types are written to CSV as single-character keys.
```

In the above, `[program path]` is the path to GinnungagapCLI on your system. For a Windows console with `GinnungagapCLI` in the current working directory, use `.\GinnungagapCLI` as `[program path]`. For the same situation in a Linux shell, use `./GinnungagapCLI`.

A longer-term plan is to provide a GUI for MXEs that set up battle maps, such that entities can be placed/removed/edited on the map itself.
Support for the original Valkyria Chronicles may follow.
