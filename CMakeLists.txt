cmake_minimum_required(VERSION 3.16)

project(scicalc VERSION 1.1.0 LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

add_executable(scicalc
	main.cpp
	scicalc.cpp
	variables.cpp
	variable.cpp
	scanner.cpp
	print.cpp
	parser.cpp
	parseException.cpp
	dialoggeneralsettings.cpp
	scicalcedit.cpp
	scicalcblock.cpp
	scicalc.h
	variables.h
	variable.h
	token.h
	scanner.h
	print.h
	parser.h
	parseException.h
	dialoggeneralsettings.h
	scicalcedit.h
	scicalcblock.h
	scicalc.ui
	dialoggeneralsettings.ui
	scicalc.qrc
)

target_link_libraries(scicalc PRIVATE Qt5::Widgets)
target_include_directories(scicalc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS scicalc
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES debian/scicalc.desktop
	DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
)

install(FILES logo.svg
	RENAME scicalc.svg
	DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps
)

install(FILES logo.png
	RENAME scicalc.png
	DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps
)

set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "scicalc")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "scicalc")
set(CPACK_PACKAGE_CONTACT "scicalc maintainer")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Scientific calculator with script-style input")
set(CPACK_PACKAGE_DESCRIPTION
	"scicalc is a Qt-based scientific calculator for script-style calculations with variables, constants and SI-prefix formatted output."
)
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_SECTION "science")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_PACKAGE_DIRECTORY "${CMAKE_SOURCE_DIR}/debian")
set(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_SOURCE_DIR}/debian")

include(CPack)
