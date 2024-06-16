project(server)
message("CMAKE_PROJECT_NAME = ${CMAKE_PROJECT_NAME}")
message("PROJECT_NAME       = ${PROJECT_NAME}")

add_definitions(-DPROJECT_NAME=\"${PROJECT_NAME}\")





add_compile_options(-D_GNU_SOURCE)


include_directories(include)

aux_source_directory(include include_SRC)


add_executable( ${PROJECT_NAME} main.cpp  ${include_SRC} ) 

target_link_libraries(${PROJECT_NAME} PRIVATE pthread protobuf) 

