include_directories (
	${HOLDINGNUTS_SOURCE_DIR}/src
	${HOLDINGNUTS_SOURCE_DIR}/src/libpoker
	${HOLDINGNUTS_SOURCE_DIR}/src/system
	${HOLDINGNUTS_SOURCE_DIR}/src/server
	${HOLDINGNUTS_SOURCE_DIR}/src/client
	${HOLDINGNUTS_SOURCE_DIR}/src/bot
)
link_directories(
	${HOLDINGNUTS_SOURCE_DIR}/src/libpoker
	${HOLDINGNUTS_SOURCE_DIR}/src/system
)

#IF (WIN32)
#	IF (MINGW)
#		ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/pclient_res.obj
#			COMMAND ${WINDRES}
#			-I${CMAKE_CURRENT_SOURCE_DIR}
#			-o ${CMAKE_CURRENT_BINARY_DIR}/pclient_res.obj
#			< ${CMAKE_CURRENT_SOURCE_DIR}/pclient.rc
#		)
#		
#		LIST(APPEND aux_obj pclient_res.obj)
#	ELSE (MINGW)
#		# FIXME: use VC resource compiler rc.exe
#	ENDIF (MINGW)
#ENDIF (WIN32)


SET( QT_USE_QTNETWORK true )

Find_Package ( Qt4 REQUIRED )

INCLUDE(${QT_USE_FILE})#

#SET(QtApp_RCCS pclient.qrc)

QT4_ADD_RESOURCES(QtApp_RCC_SRCS ${QtApp_RCCS})#

## Warning: older CMake versions (2.6-patch-0) do not provide moc-qt4 with preprocessor definitions
QT4_WRAP_CPP (
	sources_MOC
	pbot.hpp 
	HoldemTournamentAI.h PlayerHoleCards.h PlayerStats.h
	msgqueue.h resource.h
)

add_executable (
	holdingnutsbot
	WIN32    # when building for windows: do not show the console window
	pbot.cpp ${aux_obj}
	CompilePlayerStats.cpp GameController.cpp HoldemTournamentAI.cpp HoldemTournamentAIBase.cpp
	HoldemTournamentAIManual.cpp MsgQueue.cpp MsgQueueBase.cpp PlayerHoleCards.cpp
	PlayerStats.cpp PokerPirate.cpp
	${aux_src}
	${sources_MOC} ${QtApp_RCC_SRCS}

)

target_link_libraries (
   holdingnutsbot
   Poker SysAccess System
   ${QT_LIBRARIES}
   ${aux_lib}
)

INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/holdingnutsbot DESTINATION
	${CMAKE_INSTALL_PREFIX}/bin)

##############################################################

## data-directory
INSTALL(DIRECTORY ${HOLDINGNUTS_SOURCE_DIR}/data DESTINATION
	${CMAKE_DATA_PATH}/holdingnuts)

##############################################################
