##
## This file is part of the PulseView project.
##
## Copyright (C) 2012 Joel Holdsworth <joel@airwebreathe.org.uk>
## Copyright (C) 2012 Alexandru Gagniuc <mr.nuke.me@gmail.com>
##
## This program is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.
##

set(pulseview_TEST_SOURCES
	${PROJECT_SOURCE_DIR}/pv/datasnapshot.cpp
	${PROJECT_SOURCE_DIR}/pv/logicdatasnapshot.cpp
	logicdatasnapshot.cpp
	test.cpp
)

add_definitions(-DBOOST_TEST_DYN_LINK)

add_executable(pulseview-test
	${pulseview_TEST_SOURCES}
)

target_link_libraries(pulseview-test ${PULSEVIEW_LINK_LIBS})
