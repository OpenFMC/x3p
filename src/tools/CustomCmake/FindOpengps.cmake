# - Try to find OPENGPS library installation
# 
#
# The follwoing variables are optionally searched for defaults
#  OPENGPS_ROOT_DIR:            Base directory of OPENGPS tree to use.
#  OPENGPS_FIND_REQUIRED_COMPONENTS : FIND_PACKAGE(OPENGPS COMPONENTS ..) 
#   
#
# The following are set after configuration is done: 
#  OPENGPS_FOUND
#  OPENGPS_INCLUDE_DIR
#  OPENGPS_LIBRARIES
#  OPENGPS_LINK_DIRECTORIES
#


INCLUDE(CheckCXXCompilerFlag)

MACRO(DBG_MSG _MSG)
    MESSAGE(STATUS "${CMAKE_CURRENT_LIST_FILE}(${CMAKE_CURRENT_LIST_LINE}):\n${_MSG}")
ENDMACRO(DBG_MSG)





# typical root dirs of installations, exactly one of them is used
SET (OPENGPS_POSSIBLE_ROOT_DIRS
   "$ENV{ProgramFiles}/OPENGPS"
	"$ENV{LIB_DIR}"
  /usr/local
  /usr  
 "$ENV{PATH}"
  )



#
# select exactly ONE  base directory/tree 
# to avoid mixing different version headers and libs
#
FIND_PATH(OPENGPS_ROOT_DIR 
  NAMES 
  iso5436-2-xml/include/opengps/opengps.h
  include/opengps/opengps.h
  PATHS ${OPENGPS_POSSIBLE_ROOT_DIRS}
  )



# header include dir suffixes appended to OPENGPS_ROOT_DIR
SET(OPENGPS_INCDIR_SUFFIXES
  include
  OPENGPS
  include/OPENGPS
  iso5436-2-xml
  include/iso5436-2-xml
  iso5436-2-xml/include
  )

# library linkdir suffixes appended to OPENGPS_ROOT_DIR 
SET(OPENGPS_LIBDIR_SUFFIXES
  lib
  OPENGPS/lib
  iso5436-2-xml/lib
  so5436-2-xml
  )


#
# find incdir for each lib
#
FIND_PATH(OPENGPS_INCLUDE_DIR
  NAMES
  opengps/opengps.h      
  PATHS ${OPENGPS_ROOT_DIR} 
  PATH_SUFFIXES ${OPENGPS_INCDIR_SUFFIXES} 
  )
##



# library linkdir suffixes appended to OPENGPS_ROOT_DIR 
SET(OPENGPS_BINDIR_SUFFIXES
  bin
  OPENGPS/bin
  iso5436-2-xml/bin
  so5436-2-xml
  )

##

#
# find absolute path to all libraries 
# some are optionally, some may not exist on Linux
#
IF(WIN32)
#DBG_MSG("OPENGPS WIN32")
##CHECK_CXX_COMPILER_FLAG("/machine:x64" IS64BIT)
if(CMAKE_CL_64)

FIND_LIBRARY(OPENGPS_LIBRARY   
  NAMES iso5436-2-xml64
  PATHS ${OPENGPS_ROOT_DIR}  
  PATH_SUFFIXES  ${OPENGPS_LIBDIR_SUFFIXES} 
  )
  FIND_FILE(OPENGPS_SHARED_LIBRARY   
  NAMES iso5436-2-xml64.dll
  PATHS ${OPENGPS_ROOT_DIR}  
  PATH_SUFFIXES  ${OPENGPS_BINDIR_SUFFIXES} 
  )
 else()

  FIND_LIBRARY(OPENGPS_LIBRARY   
  NAMES iso5436-2-xml
  PATHS ${OPENGPS_ROOT_DIR}  
  PATH_SUFFIXES  ${OPENGPS_LIBDIR_SUFFIXES} 
  )
  FIND_FILE(OPENGPS_SHARED_LIBRARY   
  NAMES iso5436-2-xml.dll
  PATHS ${OPENGPS_ROOT_DIR}  
  PATH_SUFFIXES  ${OPENGPS_BINDIR_SUFFIXES} 
  )
  endif(CMAKE_CL_64)
ELSE()

FIND_LIBRARY(OPENGPS_LIBRARY   
  NAMES iso5436-2-xml64
  PATHS ${OPENGPS_ROOT_DIR}  
  PATH_SUFFIXES  ${OPENGPS_LIBDIR_SUFFIXES} 
  )
endif(WIN32)




#
# Logic selecting required libs and headers
#
SET(OPENGPS_FOUND ON)


#DBG_MSG("OPENGPS_INCLUDE_DIR=${OPENGPS_INCLUDE_DIRS}")
#DBG_MSG("OPENGPS_LIBRARY=${OPENGPS_LIBRARIES}")


MARK_AS_ADVANCED(
  OPENGPS_LIBRARY
  OPENGPS_INCLUDE_DIR
  
  )


# display help message
IF(NOT OPENGPS_FOUND)
  # make FIND_PACKAGE friendly
  IF(NOT OPENGPS_INCLUDE_DIR_FIND_QUIETLY)
    IF(OPENGPS_INCLUDE_DIR_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR
        "OPENGPS required but some headers or libs not found. Please specify it's location with OPENGPS_INCLUDE_DIR_ROOT_DIR env. variable.")
    ELSE(OPENGPS_INCLUDE_DIR_FIND_REQUIRED)
      MESSAGE(STATUS 
        "ERROR: OPENGPS was not found.")
    ENDIF(OPENGPS_INCLUDE_DIR_FIND_REQUIRED)
  ENDIF(NOT OPENGPS_INCLUDE_DIR_FIND_QUIETLY)
ENDIF(NOT OPENGPS_FOUND)
