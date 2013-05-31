urnest
======

C++ and odin build bits and pieces to share

-   odin-1.17.10 has my customisations to excellent build tool
    from [sourceforge/projects/odin-build] 
    (http://sourceforge.net/projects/odin-build/)

-   odin-test has various tests of odin itself

To come:

-   odin-pkg has my personal odin packages

    Some of these are extended/adjusted versions of the standard odin packages
    (and these have the same names as the standard odin packages in
    [odin-1.17.10/pkg] (odin-1.17.10/pkg)). They are not guarenteed to be
    backward compatible with the standard odin packages.

    Highlights:

    - generalised hash_inc package to avoid duplicating it in C, C++, IDL etc.
      This also makes it unnecessary to write a package for eg idl compile.

    - simulation of derived directory trees via tar files

    - :exec package allows failed test runs to keep core dumps, log files
      etc for analysis

    See the odin-pkg/README.md for more.

-   xju has my preferred C++ classes for things like posix threads,
    exception handling, assertions, unit testing
