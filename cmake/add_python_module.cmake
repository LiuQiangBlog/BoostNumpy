# add_python_module(<name> <additional_library_list> src1 src2 ... srcN) is
# used to build modules for python.
FUNCTION(add_python_module _NAME _ADD_LIB_LIST)
    GET_PROPERTY(_TARGET_SUPPORTS_SHARED_LIBS
        GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS)
    OPTION(PYTHON_ENABLE_MODULE_${_NAME}
        "Add module ${_NAME}" TRUE)
    OPTION(PYTHON_MODULE_${_NAME}_BUILD_SHARED
        "Add module ${_NAME} shared" ${_TARGET_SUPPORTS_SHARED_LIBS})

    # Mark these options as advanced
    MARK_AS_ADVANCED(
        PYTHON_ENABLE_MODULE_${_NAME}
        PYTHON_MODULE_${_NAME}_BUILD_SHARED)

    IF(PYTHON_ENABLE_MODULE_${_NAME})
        IF(PYTHON_MODULE_${_NAME}_BUILD_SHARED)
            SET(PY_MODULE_TYPE MODULE)
        ELSE(PYTHON_MODULE_${_NAME}_BUILD_SHARED)
            SET(PY_MODULE_TYPE STATIC)
            SET_PROPERTY(GLOBAL APPEND PROPERTY PY_STATIC_MODULES_LIST ${_NAME})
        ENDIF(PYTHON_MODULE_${_NAME}_BUILD_SHARED)

        SET_PROPERTY(GLOBAL APPEND PROPERTY PY_MODULES_LIST ${_NAME})
        ADD_LIBRARY(${_NAME} ${PY_MODULE_TYPE} ${ARGN})
        TARGET_LINK_LIBRARIES(${_NAME} ${PYTHON_LIBRARIES} ${_ADD_LIB_LIST})

        IF(PYTHON_MODULE_${_NAME}_BUILD_SHARED)
            SET_TARGET_PROPERTIES(${_NAME} PROPERTIES PREFIX "")
            SET_TARGET_PROPERTIES(${_NAME} PROPERTIES SUFFIX ${PYTHON_MODULE_EXTENSION})
        ENDIF()
    ENDIF(PYTHON_ENABLE_MODULE_${_NAME})
ENDFUNCTION(add_python_module)