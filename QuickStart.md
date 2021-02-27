# QGama (GNU Gama Qt GUI)

_Petra Millarová \<millapet@gnu.org\> and  Aleš Čepek \<cepek@gnu.org\>_

GNU Gama is a project dedicated to adjustment of geodetic networks. It
is intended for use with traditional geodetic surveyings which are
still used and needed in special measurements (e.g., underground or
high precision engineering measurements) where the Global Positioning
System (GPS) cannot be used.

http://www.gnu.org/software/gama/

Qt GUI for geodetic networks is an application `qgama` in which XML
input is replaced by a database. The most simple case is `Sqlite3` file
database, which does not need any special knowledge from the user.

Download and build
------------------

Download source codes from git server and run the following commands

    $ git clone git://git.sv.gnu.org/gama/qt.git
    $ cd qt
    $ git clone https://git.savannah.gnu.org/git/gama.git  # checkout a version if needed
    $ export CMAKE_PREFIX_PATH=~/Qt/6.0.1/gcc_64/          # adjust as needed
    $ mkdir build
    $ cd build
    $ cmake .. -GNinja
    $ cmake --build .
    $ ./qgama &

How to build an example database
--------------------------------

Probably you will also want an example database. You can easily build
`Sqlite3` database from GNU Gama example collection. To create the
database you will also need some programs from GNU Gama project.

    $ mkdir _your-working-directory_
    $ cd _your-working-directory_

Now return to _your-working-directory_, download examples and build
the examples.db database (_Sqlite3_).

    $ cd ..
    $ git clone git://git.sv.gnu.org/gama/examples.git
    $ ls
    examples  gama
    $ cd examples/gama-local/sqlite/
    $ make build-sqlite3-examples.db
    $ ls
    examples.db  Makefile

Known bugs
----------

* Observation editor does not implement tags `from_dh` and `to_dh`

TODO
----

* More interfaces
* import yaml format
* export of gkf (input xml data)
* processing of tags `from_dh` and `to_dh`
* online help
