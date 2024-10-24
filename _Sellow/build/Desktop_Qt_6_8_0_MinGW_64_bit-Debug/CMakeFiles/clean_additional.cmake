# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\_Sellow_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\_Sellow_autogen.dir\\ParseCache.txt"
  "_Sellow_autogen"
  )
endif()
