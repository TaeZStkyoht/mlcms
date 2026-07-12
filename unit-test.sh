#!/bin/bash
TOP_LEVEL="$(git rev-parse --show-toplevel)"
BUILD_DIR="${TOP_LEVEL}/build"

echo "Executing script for unit test"
error=0

if [ -z "${TEST_REPORT_DIR}" ]; then
   TEST_REPORT_DIR="${BUILD_DIR}/coverage/gcov"
fi
echo "TEST_REPORT_DIR: ${TEST_REPORT_DIR}"

for i in $(find ${TOP_LEVEL} -iname *.gcda); do
    rm -rf ${i}
done

TEST_DIR="${BUILD_DIR}/bin/test_binary"
cd "$TEST_DIR" || {
    echo "No such directory: $TEST_DIR"
    exit 1
}

TEST_FILE_PREFIX="*unit_test_*"

# Check if there are any files starting with the prefix
if ! ls ${TEST_FILE_PREFIX} 1> /dev/null 2>&1; then
    echo "No test file found with search '${TEST_FILE_PREFIX}'. Exiting."
    exit 1
fi

# Run tests
for file in ${TEST_FILE_PREFIX};
do
    if [ -x "$file" ]; then
        ./${file}
        let error=$error+$?
    fi
done

# Print results
if [ $error -ne 0 ]; then
    echo "${error} test(s) have returned ERROR."
else
    echo "All tests have returned OK."
fi

# Generate coverage report
echo "==========[ gcov ]=========="
echo -n "Version: "
gcov --version

rm -rf ${TEST_REPORT_DIR}
mkdir -p ${TEST_REPORT_DIR}

GCDA_PATHS=($(find ${BUILD_DIR} -name "*.gcda"))
for i in "${GCDA_PATHS[@]}"
do
   gcov $i > /dev/null 2>&1
done

cd ${TOP_LEVEL}

GCOV_PATHS=($(find . -name "*.gcov"))
for i in "${GCOV_PATHS[@]}"
do
	mv $i ${TEST_REPORT_DIR}
done

# Generate coverage report in html format
if [ "$1" == "--coverage" ]; then
    COVERAGE_PATH="${BUILD_DIR}/coverage/report"

    if [ ! -d "${COVERAGE_PATH}" ]; then
        mkdir -p ${COVERAGE_PATH}
    else
        rm -rf ${COVERAGE_PATH}/*
    fi

    gcovr -r . -e "/usr/*" -e test -e build --exclude-unreachable-branches --exclude-throw-branches --html --html-details -o $COVERAGE_PATH/coverage.html
    echo "gcovr returned " $?
fi

exit $error

