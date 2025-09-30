# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "Assignment_Lecture_4_autogen"
  "CMakeFiles\\Assignment_Lecture_4_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Assignment_Lecture_4_autogen.dir\\ParseCache.txt"
  )
endif()
