urnest
======

C++ and odin build bits and pieces to share

-   odin has my customisations to and extras for excellent build tool
    from [sourceforge/projects/odin-build] 
    (http://sourceforge.net/projects/odin-build/)

    -	odin/odin-1.17.10x has my customisations:
        - optimise :map=:xy to only propogate relevant param types, in
 	  order to scale well in some scenarios eg:
	       - building windows C++ software that uses different
	         +define options for building dlls v clients of dlls, which
		 would cause (unnecessary) generation of odin :map 
		 deriviations for #include dependencies, causing long
		 delays in building
        - allow odin to clean up subdirectories eg JOB temporary directories
          when build is aborted via Ctrl-C

    -   odin/pkg has my odin packages.

        Some of these are extended/adjusted versions of the standard odin packages
        (and these have the same names as the standard odin packages in
        [odin-1.17.10/pkg] (odin-1.17.10/pkg)). They are not guarenteed to be
        backward compatible with the standard odin packages.
    
        Highlights:
    
        - generalised hash_inc package to avoid duplicating it 
          in C, C++, IDL etc.

          This also makes it unnecessary to write a package for eg
	  idl compile.
    
        - simulation of derived directory trees via tar files
    
        - :exec package allows failed test runs to keep core dumps, log 
	  files etc for analysis
    
        See the REVISIT: odin/pkg/README.md for more.
	
    -   odin/test has various tests of odin itself

-   xju has my preferred C++ classes for things like posix threads,
    exception handling, assertions, unit testing

-   btt is a simple time tracker

-   hcp is progress towards a utility to split C++-in-single-source-file
    into .h and .cpp files (to make coding more efficient); part of
    this utility is a (partial) C++ parser that I hope to use to write
    other C++ source code handling tools
