# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/app/custom/_deps/libs-src"
  "/app/custom/_deps/libs-build"
  "/app/custom/_deps/libs-subbuild/libs-populate-prefix"
  "/app/custom/_deps/libs-subbuild/libs-populate-prefix/tmp"
  "/app/custom/_deps/libs-subbuild/libs-populate-prefix/src/libs-populate-stamp"
  "/app/custom/_deps/libs-subbuild/libs-populate-prefix/src"
  "/app/custom/_deps/libs-subbuild/libs-populate-prefix/src/libs-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/app/custom/_deps/libs-subbuild/libs-populate-prefix/src/libs-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/app/custom/_deps/libs-subbuild/libs-populate-prefix/src/libs-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
