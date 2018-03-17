# Polynomials
My big 3-part project written in C on 2nd semester of studies. The project uses Doxygen to generate html and pdf documentation.

First part: source code with all logic.
Second part: writing a calculator parsing input and producing output. The polynomials are stored on the global stack. Prints logs to stderr when the input is incorrect
Third part: mocking std functions with Cmocka so it is possible to write unit tests in C with assertions.

Compile:
mkdir  release
cd  release
cmake  ..
make
make  doc

Debug:
mkdir  debug
cd  debug
cmake  -D CMAKE_BUILD_TYPE=Debug ..
make
make  doc
