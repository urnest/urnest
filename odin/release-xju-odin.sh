#!/bin/sh -ex

release_repo_dir="$1"

test -n "$release_repo_dir"


TBD

# update version, update change history
update version in configure.ac
run autonconf
commit changes

# from empty directory (e.g. mkdir /tmp/xju-odin-release && cd /tmp/xju-odin-release)

# make sure xju-odin/pkg/PKGLST has all packages in it
# i.e. entries for all xju-odin/pkg/*/*.dg

# use a working version of odin (i.e. install previous release) to
# xju-odin/configure and then build xju-odin/pkg/odin%exe
# REVISIT: configure should be automatic and not put generated files
# into source tree
#
# commit any changes (e.g. it will update xju-odin/pkg/odin/boot/default-dg.c


# xju-odin/CLEAN
# build .tar.gz  exclude .pytest_cache, __pycache__, *~
cd ~/release && xju-odin/build-source-tarball.sh

# test install:
# xju-odin/CLEAN &&
# xju-odin/configure && mkdir /tmp/test-odin && xju-odin/INSTALL /tmp/test-odin
# ODIN=/tmp/test-odin-cache /tmp/test-odin/bin/odin xju-odin/pkg%run-all-tests

# git tag source
# commit tarball to release repo and push
