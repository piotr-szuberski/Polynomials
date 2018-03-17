# Polynomials
My big 3-part project written in C on 2nd semester of studies. The project uses Doxygen to generate html and pdf documentation.

First part: source code with all logic.
Second part: writing a calculator parsing input and producing output. The polynomials are stored on the global stack. Prints logs to stderr when the input is incorrect
Third part: mocking std functions with Cmocka so it is possible to write unit tests in C with assertions.
I wrote the source code of poly, calc_poly, unit_tests_poly, chain_poly. . test_poly and testuj_sktypt was given with the problem.

Compile:
mkdir  release;
cd  release;
cmake  ..;
make;
make  doc

Debug:
mkdir  debug;
cd  debug;
cmake  -D CMAKE_BUILD_TYPE=Debug ..;
make;
make  doc

test_poly usage (all tests, the rest is in the description of file):

./test_poly all

To test chain_poly.sh script:
./testuj_sktypt.sh

It will test the script automating processes

To test calc_poly:
./calc_test.sh ./release/calc_poly ./tests


To test unit_tests_poly cmocka library is needed and CmakeLists_for_unit_tests needs to replace the current CMakeLists.
I divided CMakeLists to make it easier to test the program without installing CMocka
