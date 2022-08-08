#!/bin/sh

d=$(cd $(dirname "$0") && pwd)

set -ex

mkdir SOURCES BUILD  BUILDROOT  RPMS  SRPMS

cat > xju-odin.spec <<EOF
Name:           xju-odin
Version:        1.17.11
Release:        1%{?dist}
Summary:        odin-build fork

License:        BSD
Source:         xju-odin.tar.gz

BuildRequires: gcc g++

%description
odin-build fork see https://github.com/urnest/urnest

%prep
  cd $d
  ./CLEAN
  cd ..
  tar czf %{_sourcedir}/xju-odin.tar.gz xju-odin 

%build
  echo "!!!build: \$(pwd)"
  cd $d
  ./configure

%install
  echo "!!!install: \$(pwd)"
  ( 
    cd $d
    ./INSTALL %{buildroot}
  )
  find %{buildroot} -mindepth 2 -type f | xargs fgrep -l '%{buildroot}' |
    while read n
    do
      cp \$n \$n.new
      cat \$n |
        python3 -c "import sys; sys.stdout.write(sys.stdin.read().replace('%{buildroot}',''))" > \$n.new
      mv \$n.new \$n
    done
  find %{buildroot} | xargs chmod u+w
  find %{buildroot} -name .build-id | xargs rm -rf

%clean
  echo "!!!clean: \$(pwd)"
  ( 
    cd $d
  )

%files
%doc /man/man1/odin.1
/bin/odin
/lib/Odin

EOF


rpmbuild --define="_topdir $(pwd)" --define="_build_id_links none" xju-odin.spec -ba
