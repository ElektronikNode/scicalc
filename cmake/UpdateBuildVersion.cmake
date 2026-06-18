if(NOT DEFINED SCICALC_BASE_VERSION)
	message(FATAL_ERROR "SCICALC_BASE_VERSION is not set")
endif()

if(NOT DEFINED SCICALC_BUILD_NUMBER_FILE)
	message(FATAL_ERROR "SCICALC_BUILD_NUMBER_FILE is not set")
endif()

if(NOT DEFINED SCICALC_VERSION_HEADER)
	message(FATAL_ERROR "SCICALC_VERSION_HEADER is not set")
endif()

if(EXISTS "${SCICALC_BUILD_NUMBER_FILE}")
	file(READ "${SCICALC_BUILD_NUMBER_FILE}" current_build_number)
	string(STRIP "${current_build_number}" current_build_number)
else()
	set(current_build_number "000")
endif()

if(NOT current_build_number MATCHES "^[0-9]+$")
	message(FATAL_ERROR "Invalid build number in ${SCICALC_BUILD_NUMBER_FILE}: ${current_build_number}")
endif()

math(EXPR next_build_number "${current_build_number} + 1")
if(next_build_number GREATER 999)
	message(FATAL_ERROR "Build number exceeded 999")
elseif(next_build_number LESS 10)
	set(formatted_build_number "00${next_build_number}")
elseif(next_build_number LESS 100)
	set(formatted_build_number "0${next_build_number}")
else()
	set(formatted_build_number "${next_build_number}")
endif()

file(WRITE "${SCICALC_BUILD_NUMBER_FILE}" "${formatted_build_number}\n")

get_filename_component(version_header_dir "${SCICALC_VERSION_HEADER}" DIRECTORY)
file(MAKE_DIRECTORY "${version_header_dir}")
file(WRITE "${SCICALC_VERSION_HEADER}"
"#ifndef SCICALC_VERSION_H\n"
"#define SCICALC_VERSION_H\n"
"\n"
"#define SCICALC_VERSION_BASE \"${SCICALC_BASE_VERSION}\"\n"
"#define SCICALC_VERSION_BUILD \"${formatted_build_number}\"\n"
"#define SCICALC_VERSION \"${SCICALC_BASE_VERSION}-${formatted_build_number}\"\n"
"\n"
"#endif\n"
)

message(STATUS "scicalc version: ${SCICALC_BASE_VERSION}-${formatted_build_number}")
