#   Install a pre-commit hook to bootstrap commit hooks.

find_package(Git)
if (GIT_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/.git/config")

    #   Configure style checking.

    find_program(UNCRUSTIFY uncrustify)

    if (NOT UNCRUSTIFY)
        message(WARNING "Uncrustify not found. You will not be able to commit C++ changes.
Please install this program and reconfigure.")
        execute_process(
            COMMAND           ${GIT_EXECUTABLE} config --bool hooks.uncrustify false
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    else()
        execute_process(
            COMMAND           ${GIT_EXECUTABLE} config --bool hooks.uncrustify true
            COMMAND           ${GIT_EXECUTABLE} config uncrustify.command "${UNCRUSTIFY}"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    endif()

    #   Install the git pre-commit hook.

    set(WARNING_MESSAGE "WARNING: DO NOT MODIFY\n# This file has been automatically generated from utils/git/pre-commit.")
    configure_file(${CMAKE_SOURCE_DIR}/utils/git/pre-commit ${CMAKE_SOURCE_DIR}/.git/hooks/pre-commit @ONLY)
endif()
