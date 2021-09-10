execute_process(COMMAND ${CMAKE_COMMAND} -E tar x "${PYTHON_ZIP}")
file(GLOB_RECURSE files LIST_DIRECTORIES FALSE "*")

foreach (file ${files})
    # Update the timestamps to avoid regeneration at each build
    file(TOUCH "${file}")
endforeach()
