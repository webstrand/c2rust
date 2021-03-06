# Create a Makefile

in `libxml2/repo`:

`./autogen.sh && ./configure`

You will need to edit the newly generated Makefile. Add these changes:
* `am__libxml2_la_SOURCES_DIST` needs `variadic.c`
* `am_libxml2_la_OBJECTS` needs `variadic.lo`
* `am_runsuite_OBJECTS`, `am_runtest_OBJECTS`, `am_runxmlconf_OBJECTS`, `am_testAutomata_OBJECTS`, `am_testC14N_OBJECTS`, `am_testHTML_OBJECTS`, `am_testModule_OBJECTS`, `am_testReader_OBJECTS`, `am_testRegexp_OBJECTS`, `am_testRelax_OBJECTS`, `am_testSAX_OBJECTS`, `am_testSchemas_OBJECTS`, `am_testThreads_OBJECTS`, `am_testURI_OBJECTS`, `am_testXPath_OBJECTS`, `am_testapi_OBJECTS`, `am_testchar_OBJECTS`, `am_testdict_OBJECTS`, `am_testlimits_OBJECTS`, `am_testrecurse_OBJECTS`, `am_xmlcatalog_OBJECTS` need `variadic.$(OBJEXT)`
* `am_xmllint_OBJECTS` needs `variadic.$(OBJEXT)` and `xmllint_variadic.$(OBJEXT)`

# Create a compile_commands.json

in `libxml2/repo`:

`../../../dependencies/Bear-2.*.*/build/bear/bear make check` so that we can translate all necessary files (including tests).

If your `compile_commands.json` enables optimizations(`-O2`) you will need to remove them so that unsupported compiler_builtins are less likely to be generated and leave you in an uncompilable state.

Run `rm .libs/*.o` here to get rid of gcc generated staticlibs or else you may see `CRITICAL:root:error: some ELF objects were not compiled with clang:` in the next step

# Generate Rust Code

in `libxml2`:

`./translate.py` to translate all required c files (including tests) into the `libxml2/repo/rust/src` and `libxml2/repo/rust/examples` directories.

# Fix Known Translation Issues

in `libxml2`:

`./patch_translated_code.py` to apply patches to some known issues in the generated code.

# Run Libxml2 C Tests

Since each of these tests have their own main file, we decided to move them to the rust examples directory instead of trying to wrap them in the test framework.

You can run a test like so: `cargo run --example EXAMPLE` where `EXAMPLE` is one of the files in `libxml2/repo/rust/examples`, not including the file extension.

# Outstanding Test Issues

## Runnable

* `runtest` seems to mostly work, but has inconsistent success. C version seems to be more consistently successful (maybe the test is just poorly written?)
* `testReader` seems to be mostly working identically but with some slight formatting differences. Try `testReader --valid test/japancrlf.xml`

## Working

* `testRelax` seems to work equivalently with files as in C
* `testXPath` seems to work equivalently with files as in C
* `xmllint` seems to work equivalently with files as in C
* `testSAX` prints out nothing on success, just like C version
* `testModule` prints "Success!"
* `testHTML` works with input files from `test/HTML` and produces same output as C version
* `testRegexp` works with files from `test/regexp` and produces same output as C version
* `testrecurse` prints "Total 9 tests, no errors"
* `testlimits` prints "Total 514 tests, no errors"
    * Note: text output seems noticeably slower than the C version
* `testThreads` prints nothing (but no longer prints parsing errors)
* `testapi` runs successfully and prints "Total: 1172 functions, 280928 tests, 0 errors"
* `testC14N` prints parsed output when given a file to read from `test/c14n`
* `testSchemas` no longer crashes when provided a file from `test/schemas/*.xsd`
* `testchar` prints tests completed
* `testdict` prints "dictionary tests succeeded 20000 strings"
* `testAutomata` takes a file from `test/automata` and produces equivalent output to C run
* `testURI` waits on input from stdin, needs example input from `test/URI`. See `Makefile.am` and `result/URI/uri.data` for examples
