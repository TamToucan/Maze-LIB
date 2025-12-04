# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/app/custom/_deps/gdextension-src"
  "/app/custom/_deps/gdextension-build"
  "/app/custom/_deps/gdextension-subbuild/gdextension-populate-prefix"
  "/app/custom/_deps/gdextension-subbuild/gdextension-populate-prefix/tmp"
  "/app/custom/_deps/gdextension-subbuild/gdextension-populate-prefix/src/gdextension-populate-stamp"
  "/app/custom/_deps/gdextension-subbuild/gdextension-populate-prefix/src"
  "/app/custom/_deps/gdextension-subbuild/gdextension-populate-prefix/src/gdextension-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/app/custom/_deps/gdextension-subbuild/gdextension-populate-prefix/src/gdextension-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/app/custom/_deps/gdextension-subbuild/gdextension-populate-prefix/src/gdextension-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
