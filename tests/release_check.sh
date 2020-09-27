#!/bin/sh

# check version numbers between configure.ac and CMakeLists.txt.

Configure_major_version=`sed  -e '1{s/^.*\[\([0-9]\+\)\].*$/\1/};1!d' ../configure.ac`
Configure_minor_version=`sed  -e '2{s/^.*\[\([0-9]\+\)\].*$/\1/};2!d' ../configure.ac`
Configure_micro_version=`sed  -e '3{s/^.*\[\([0-9]\+\)\].*$/\1/};3!d' ../configure.ac`

Configure_version_string="$Configure_major_version.$Configure_minor_version.$Configure_micro_version"

Cmake_version_string=`grep project ../CMakeLists.txt | sed 's/^.*VERSION \([^ ]\+\) .*$/\1/'`

if [ "$Configure_version_string" != "$Cmake_version_string" ]; then
	echo "ERROR: Version number defined in configure.ac is not equal to the number defined in CMakeLists.txt."
	echo "configure.ac: $Configure_version_string"
	echo "CMakeLists.txt: $Cmake_version_string"
fi

NEWS_version_string=`grep -m 1 Version ../NEWS | sed 's/^Version \([^ ]\+\) (.*$/\1/'`

if [ "$Configure_version_string" != "$NEWS_version_string" ]; then
	echo "ERROR: Version number defined in configure.ac is not equal to the newest number described in NEWS."
	echo "configure.ac: $Configure_version_string"
	echo "NEWS: $NEWS_version_string"
fi

README_version_string=`grep -m 1 Attention ../README.md | sed 's/^.*version \([^ ]\+\)\.$/\1/'`

if [ "$Configure_version_string" != "$README_version_string" ]; then
	echo "ERROR: Version number defined in configure.ac is not equal to the newest number described in README.md."
	echo "configure.ac: $Configure_version_string"
	echo "README.md: $README_version_string"
fi
