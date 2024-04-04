# Build the app, using the HAL

include_directories(include)
include_directories(../pru-as4/include)
file(GLOB MY_SOURCES "src/*.c")
add_executable(as4 ${MY_SOURCES})

find_package(Threads REQUIRED)
target_link_libraries(as4 LINK_PRIVATE Threads::Threads)

# Make use of the HAL library
target_link_libraries(as4 LINK_PRIVATE hal)

# Copy executable to final location (change `as4` to project name as needed)
add_custom_command(TARGET as4 POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:as4>"
     "~/cmpt433/public/myApps/as4" 
  COMMENT "Copying ARM executable to public NFS directory")
