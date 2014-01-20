# Find cmake script to search for the numpy Python package.
find_package(PythonInterp REQUIRED)
find_package(PythonLibs ${PYTHON_VERSION_STRING} REQUIRED)

execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import numpy"
    RESULT_VARIABLE NUMPY_FOUND)

if(NUMPY_FOUND EQUAL 0)
  set(NUMPY_FOUND TRUE)
else(NUMPY_FOUND EQUAL 0)
  set(NUMPY_FOUND FALSE)
endif(NUMPY_FOUND EQUAL 0)

if(NUMPY_FOUND)
    message(STATUS "NumPy found.")
    execute_process(COMMAND ${PYTHON_EXECUTABLE} -c
        "import numpy; print(numpy.get_include())"
        OUTPUT_VARIABLE _NUMPY_INCLUDE_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    # Look in some other places, too. This should make it
    # work on OS X, where the headers are in SDKs within XCode.app,
    # but python reports them as being available at /.
    set(NUMPY_INCLUDE_DIR_CANDIDATES ${_NUMPY_INCLUDE_DIR})
    foreach(prefix ${CMAKE_PREFIX_PATH})
        list(APPEND NUMPY_INCLUDE_DIR_CANDIDATES ${prefix}/${_NUMPY_INCLUDE_DIR})
        list(APPEND NUMPY_INCLUDE_DIR_CANDIDATES ${prefix}/../${_NUMPY_INCLUDE_DIR})
    endforeach(prefix ${CMAKE_PREFIX_PATH})
    foreach(prefix ${CMAKE_FRAMEWORK_PATH})
        list(APPEND NUMPY_INCLUDE_DIR_CANDIDATES ${prefix}/${_NUMPY_INCLUDE_DIR})
        list(APPEND NUMPY_INCLUDE_DIR_CANDIDATES ${prefix}/../../../${_NUMPY_INCLUDE_DIR})
    endforeach(prefix ${CMAKE_FRAMEWORK_PATH})

    find_path(NUMPY_INCLUDE_DIR NAMES numpy/ndarrayobject.h PATHS ${NUMPY_INCLUDE_DIR_CANDIDATES})

    message(STATUS "+    include path: ${NUMPY_INCLUDE_DIR}")

endif(NUMPY_FOUND)