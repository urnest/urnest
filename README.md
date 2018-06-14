urnest
======

C++, odin build, python bits and pieces to share.

LICENSING

- the omnicxy sub-directory is derived from omniORB 4.2.0 and uses its
  licence - see omnicxy/README.txt

- the odin sub-directory is derived from (and includes a copy of) odin and
  uses its licence - see odin-1.17.10x/README

- all other sub-directories, including xju, hcp and pq are under free licence:

    Permission to use, copy, modify, and/or distribute this software for any purpose with or without fee is hereby granted, provided that the above copyright notice and this permission notice appear in all copies.

    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.


OVERVIEW

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

-   xju has my C++ classes etc for things like posix threads,
    exception handling, assertions; if there is a theme it is
    providing classes that are hard to misuse and that leverage
    C++ strengths:
        - type safety
        - RAII
      status: usable

-   btt is a simple time tracker, it uses fltk1.3 for UI
      status: usable

-   hcp is progress towards a utility to split C++-in-single-source-file
    into .h and .cpp files (to make hand-coding more efficient); part of
    this utility is a (partial) C++ parser that I hope to use to write
    other C++ source code handling tools
      status: usable but room for improvement (see hcp/TODO)

-   omnicxy is progress towards a modern CORBA C++ language mapping 
    (alternative to OMG C++ language mappings). Documented in
    omnicxy/README.txt with examples referenced.
      status: usable, still some stuff unimplemented (see omncxy/TODO.txt)
              currently omniORB linux only

-   xo is my notes on how I would re-write odin so that I could
    understand how it works and therefor fix and improve it :-)
      status: planning

-   pq is jquery-like functionality for python, useful for doing
    server-side stuff as you might do client-side in javascript with
    jquery. See the tests at the bottom of pq/pq.py for examples.
