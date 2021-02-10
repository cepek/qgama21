For Qt6 CMake is the standard 3rd party build system.

CMake build was added in gama-q2 version 1.02. If you use Qt Creator
to build the project, you can use CMakeLists.txt as it is.

If you want to run cmake directly you have to either

* set the variable CMAKE_PREFIX_PATH in the top level CMakeLists.txt
  (which is empty by default), see two examples given in comments.

* or set up the environment variable CMAKE_PREFIX_PATH, for example

  * export CMAKE_PREFIX_PATH=~/Qt/6.0.1/gcc_64/
  * cmake .. -GNinja
  * cmake --build .

