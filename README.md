urnest
======

C++ and odin build bits and pieces to share

-   odin/ has my customisations to and extras for excellent build tool
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
        - note that in order to realise the potential of this optimisation,
	  you should use the xjupkg packages (see below) rather than the
	  standard odin packages, because the standard packages have
	  FILE -> .h etc derivations (via :GENERIC), which causes
	  them to propogate *all* param types to :map=:c_inc_all

        - allow odin to clean up subdirectories eg JOB temporary directories
          when build is aborted via Ctrl-C

	- message adjustments in odin shell script output to align with
	  naming used in the Odin reference manual

    -   odin/odin-1.17.10x/xjupkg has my odin packages.

        Some of these are extended/adjusted versions of the standard 
        odin packages
        (and these have the same names as the standard odin packages in
        [odin-1.17.10x/pkg] (odin/odin-1.17.10x/pkg)). They are not 
        guarenteed to be backward compatible with the standard odin packages.
    
        Highlights:
    
        - add generalised hash_inc package to avoid duplicating it 
          in C, C++, IDL etc. (see hash_inc.dg)

          This also makes it unnecessary to write a package for eg
	  idl compile.
    
        - simulation of derived directory trees via tar files
          (see run.dg exec.* derivations)

        - tree - like sm but does not imply source files; I use it
	  to collect up hierarchies of unit test runs

        - vir_tree - like vir_dir but can create a multi-level hierarchy,
          useful for eg creating installation packages like RPMs
          (see vir_tree.dg)
    
        - :exec.* derivations allows failed test runs to keep core dumps, 
	  log files etc for analysis (see run.dg) - unlike :stdout which
	  discards all the evidence on failure

        - adjustments to standard package type hierarchy to realise
          :map=:xy optimisation noted above

        - dirtree - builds a list that contains directory and (recursively)
	  all its subdirectories. Useful for building packages

        - removed the subtle :o_of derivation, which seemed to be a hack
	  to allow longer link commands; instead link steps first put
	  the .o files into an archive, allowing unlimited numbers of
          input files (making use of ar append function)

        - a java package that does dependency based java compiles
	  (unbelievably, there *are no dependency based compile tools for
	  java* on the net that I can find). I shelved this package
	  when I stopped working on java stuff, but it did allow me
	  to change a .java file and not have to guess what to rebuild
	  (or wait to rebuild everything); the java build environment
	  was windows, which made the package more convoluted

        For more info on the above packages, see 
        odin/odin-1.17.10x/xjupkg/*.dg; also in many packages there
        is a test/Odinfile that illustrates and sometimes even comments
        on usage.
	
    -   odin/test a couple of tests of odin itself

-   xju has my C++ classes for things like posix threads,
    exception handling, assertions; if there is a theme it is
    providing classes that are hard to misuse

-   btt is a simple time tracker, it uses fltk1.3 for UI

-   hcp is progress towards a utility to split C++-in-single-source-file
    into .h and .cpp files (to make coding more efficient); part of
    this utility is a (partial) C++ parser that I hope to use to write
    other C++ source code handling tools

-   xo is my notes on how I would re-write odin so that I could
    understand how it works and therefor fix and improve it :-)
