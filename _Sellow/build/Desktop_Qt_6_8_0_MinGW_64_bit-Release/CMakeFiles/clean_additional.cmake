# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\Sellow_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Sellow_autogen.dir\\ParseCache.txt"
  "Sellow_autogen"
  )
endif()