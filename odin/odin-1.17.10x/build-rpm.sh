#!/bin/sh

d=$(cd $(dirname "$0") && pwd)

set -ex

mkdir BUILD  BUILDROOT  RPMS  SRPMS

cat > xju-odin.spec <<EOF
Name:           xju-odin
Version:        1.17.11
Release:        1%{?dist}
Summary:        odin-build fork

License:        BSD

BuildRequires: gcc g++

%description
odin-build fork see https://github/urnest/urnest

%prep

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
        python -c "import sys; sys.stdout.write(sys.stdin.read().replace('%{buildroot}',''))" > \$n.new
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
%doc /man
/bin
/lib

EOF


rpmbuild --define="_topdir $(pwd)" --define="_sourcedir $d" --define="_build_id_links none" xju-odin.spec -ba
