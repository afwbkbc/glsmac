CMAKE_MINIMUM_REQUIRED( VERSION 3.13 )

SET( PROJECT_NAME GLSMAC )
PROJECT( ${PROJECT_NAME} LANGUAGES CXX )

SET( CMAKE_MODULE_PATH "." )

SET( EXECUTABLE_OUTPUT_PATH "bin" )

IF ( WIN32 )
SET( SRC "WIN32" )
ELSE ()
SET( SRC "" )
ENDIF ()
SET( SRC ${SRC} resource/icon/glsmac.rc )
SET( PWD "." )

ADD_CUSTOM_TARGET( glsmac_version )

IF ( NOT WIN32 )
	ADD_CUSTOM_COMMAND(
		TARGET glsmac_version PRE_BUILD
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
		COMMAND git show -s --format='\#define GLSMAC_LAST_COMMIT \"%h\"' > ${PROJECT_SOURCE_DIR}/src/tmp/last_commit.h
	)
ENDIF ()

IF ( NOT WIN32 )
	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread" )
ENDIF ()
SET( CMAKE_CXX_STANDARD 17 )
SET( CMAKE_CXX_STANDARD_REQUIRED ON )
IF ( NOT MSVC )
	SET( CMAKE_CXX_FLAGS " -std=c++17 ${CMAKE_CXX_FLAGS} -Wno-pointer-arith " )
ENDIF ()

ADD_SUBDIRECTORY( tools )
SET( EMBED_CPP "src/tmp/embed.cpp" )
SET( EMBED_FILES
	"resource/font/terminus/TerminusTTF-4.49.3-minified.ttf"
	"GLSMAC_data/default"
)
FILE( GLOB_RECURSE EMBED_DATA_DEPENDS CONFIGURE_DEPENDS
	"${PROJECT_SOURCE_DIR}/GLSMAC_data/default/*"
)
SET( EMBED_DEPENDS
	"resource/font/terminus/TerminusTTF-4.49.3-minified.ttf"
	${EMBED_DATA_DEPENDS}
)

FUNCTION( SUBDIR DIR )
	SET( PWD "${PWD}/${DIR}" )
	ADD_SUBDIRECTORY( ${DIR} )
	SET( SRC ${SRC} PARENT_SCOPE )
ENDFUNCTION( SUBDIR )

SUBDIR( src )
ADD_EXECUTABLE( ${PROJECT_NAME}
	${SRC}
	${EMBED_CPP}
)

IF ( MSVC )
	# std::thread inherits this reserve; GSE execution under ASan exceeds MSVC's 1 MiB default.
	TARGET_COMPILE_OPTIONS( ${PROJECT_NAME} PRIVATE /EHsc )
	TARGET_LINK_OPTIONS( ${PROJECT_NAME} PRIVATE /STACK:8388608 )
ENDIF ()

INCLUDE( CTest )
IF ( BUILD_TESTING )
	TARGET_COMPILE_DEFINITIONS( ${PROJECT_NAME} PRIVATE GLSMAC_TESTING=1 )
	SET( TEST_PROFILE_DIR "${CMAKE_CURRENT_BINARY_DIR}/test-profile" )
	FILE( MAKE_DIRECTORY "${TEST_PROFILE_DIR}" )
	ADD_TEST( NAME gse_native COMMAND
		$<TARGET_FILE:${PROJECT_NAME}>
		--prefix "${TEST_PROFILE_DIR}/gse-native"
		--gse-tests
		--gse-tests-native-only
	)
	FILE( MAKE_DIRECTORY "${TEST_PROFILE_DIR}/gse-native" )
	SET_TESTS_PROPERTIES( gse_native PROPERTIES
		LABELS gse
		RESOURCE_LOCK gse_runtime
		TIMEOUT 180
		WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
	)

	FILE( GLOB GLSMAC_GSE_TEST_SCRIPTS CONFIGURE_DEPENDS
		RELATIVE "${PROJECT_SOURCE_DIR}"
		"${PROJECT_SOURCE_DIR}/GLSMAC_data/tests/*.gls.js"
	)
	FOREACH ( GSE_TEST_SCRIPT ${GLSMAC_GSE_TEST_SCRIPTS} )
		GET_FILENAME_COMPONENT( GSE_TEST_FILENAME "${GSE_TEST_SCRIPT}" NAME )
		IF ( NOT GSE_TEST_FILENAME MATCHES "^_" )
			STRING( REGEX REPLACE "\\.gls\\.js$" "" GSE_TEST_NAME "${GSE_TEST_FILENAME}" )
			STRING( MAKE_C_IDENTIFIER "${GSE_TEST_NAME}" GSE_TEST_ID )
			SET( GSE_TEST_PROFILE_DIR "${TEST_PROFILE_DIR}/gse-${GSE_TEST_ID}" )
			FILE( MAKE_DIRECTORY "${GSE_TEST_PROFILE_DIR}" )
			ADD_TEST( NAME "gse_script_${GSE_TEST_ID}" COMMAND
				$<TARGET_FILE:${PROJECT_NAME}>
				--prefix "${GSE_TEST_PROFILE_DIR}"
				--gse-tests
				--gse-tests-script "./${GSE_TEST_SCRIPT}"
			)
			SET_TESTS_PROPERTIES( "gse_script_${GSE_TEST_ID}" PROPERTIES
				LABELS gse
				RESOURCE_LOCK gse_runtime
				TIMEOUT 300
				WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
			)
		ENDIF ()
	ENDFOREACH ()

	SET( GLSMAC_SMAC_PATH "" CACHE PATH "SMAC installation used by the optional runtime smoke test" )
	IF ( GLSMAC_SMAC_PATH )
		SET( RUNTIME_TEST_PROFILE_DIR "${TEST_PROFILE_DIR}/runtime-smoke" )
		FILE( MAKE_DIRECTORY "${RUNTIME_TEST_PROFILE_DIR}" )
		ADD_TEST( NAME runtime_smoke COMMAND
			$<TARGET_FILE:${PROJECT_NAME}>
			--prefix "${RUNTIME_TEST_PROFILE_DIR}"
			--smacpath "${GLSMAC_SMAC_PATH}"
			--mainscript "../tests/_runtime_smoke"
			--quickstart
			--quickstart-mapsize "20x10"
			--quickstart-seed "1:2:3:4"
			--quickstart-faction "GAIANS"
			--nosound
			--headless
			--windowed
			--window-size "1024x768"
			--verbose
		)
		SET_TESTS_PROPERTIES( runtime_smoke PROPERTIES
			FAIL_REGULAR_EXPRESSION "RUNTIME_SMOKE_FAIL"
			PASS_REGULAR_EXPRESSION "RUNTIME_SMOKE_PASS"
			RUN_SERIAL TRUE
			TIMEOUT 150
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		)

		SET( DIPLOMACY_RUNTIME_TEST_PROFILE_DIR "${TEST_PROFILE_DIR}/diplomacy-runtime-smoke" )
		FILE( MAKE_DIRECTORY "${DIPLOMACY_RUNTIME_TEST_PROFILE_DIR}" )
		ADD_TEST( NAME diplomacy_runtime_smoke COMMAND
			$<TARGET_FILE:${PROJECT_NAME}>
			--prefix "${DIPLOMACY_RUNTIME_TEST_PROFILE_DIR}"
			--smacpath "${GLSMAC_SMAC_PATH}"
			--mainscript "../tests/_diplomacy_runtime_smoke"
			--quickstart
			--quickstart-ai "1"
			--quickstart-mapsize "20x10"
			--quickstart-seed "1:2:3:4"
			--quickstart-faction "GAIANS"
			--nosound
			--headless
			--windowed
			--window-size "1024x768"
		)
		SET_TESTS_PROPERTIES( diplomacy_runtime_smoke PROPERTIES
			FAIL_REGULAR_EXPRESSION "DIPLOMACY_RUNTIME_FAIL"
			PASS_REGULAR_EXPRESSION "DIPLOMACY_RUNTIME_PASS"
			RUN_SERIAL TRUE
			TIMEOUT 90
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		)

		SET( PROBE_RUNTIME_TEST_PROFILE_DIR "${TEST_PROFILE_DIR}/probe-runtime-smoke" )
		FILE( MAKE_DIRECTORY "${PROBE_RUNTIME_TEST_PROFILE_DIR}" )
		ADD_TEST( NAME probe_runtime_smoke COMMAND
			$<TARGET_FILE:${PROJECT_NAME}>
			--prefix "${PROBE_RUNTIME_TEST_PROFILE_DIR}"
			--smacpath "${GLSMAC_SMAC_PATH}"
			--mainscript "../tests/_probe_runtime_smoke"
			--quickstart
			--quickstart-ai "1"
			--quickstart-mapsize "20x10"
			--quickstart-seed "1:2:3:4"
			--quickstart-faction "GAIANS"
			--nosound
			--headless
			--windowed
			--window-size "1024x768"
		)
		SET_TESTS_PROPERTIES( probe_runtime_smoke PROPERTIES
			FAIL_REGULAR_EXPRESSION "PROBE_RUNTIME_FAIL"
			PASS_REGULAR_EXPRESSION "PROBE_RUNTIME_PASS"
			RUN_SERIAL TRUE
			TIMEOUT 120
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		)

		SET( RESEARCH_RUNTIME_TEST_PROFILE_DIR "${TEST_PROFILE_DIR}/research-runtime-smoke" )
		FILE( MAKE_DIRECTORY "${RESEARCH_RUNTIME_TEST_PROFILE_DIR}" )
		ADD_TEST( NAME research_runtime_smoke COMMAND
			$<TARGET_FILE:${PROJECT_NAME}>
			--prefix "${RESEARCH_RUNTIME_TEST_PROFILE_DIR}"
			--smacpath "${GLSMAC_SMAC_PATH}"
			--mainscript "../tests/_research_runtime_smoke"
			--quickstart
			--quickstart-mapsize "20x10"
			--quickstart-seed "1:2:3:4"
			--quickstart-faction "GAIANS"
			--nosound
			--windowed
			--window-size "1024x768"
			--verbose
		)
		SET_TESTS_PROPERTIES( research_runtime_smoke PROPERTIES
			FAIL_REGULAR_EXPRESSION "RESEARCH_RUNTIME_FAIL"
			PASS_REGULAR_EXPRESSION "RESEARCH_RUNTIME_PASS"
			RUN_SERIAL TRUE
			TIMEOUT 90
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		)

		SET( PLANET_BUSTER_RUNTIME_TEST_PROFILE_DIR "${TEST_PROFILE_DIR}/planet-buster-runtime-smoke" )
		FILE( MAKE_DIRECTORY "${PLANET_BUSTER_RUNTIME_TEST_PROFILE_DIR}" )
		ADD_TEST( NAME planet_buster_runtime_smoke COMMAND
			$<TARGET_FILE:${PROJECT_NAME}>
			--prefix "${PLANET_BUSTER_RUNTIME_TEST_PROFILE_DIR}"
			--smacpath "${GLSMAC_SMAC_PATH}"
			--mainscript "../tests/_planet_buster_runtime_smoke"
			--quickstart
			--quickstart-ai "1"
			--quickstart-mapsize "20x10"
			--quickstart-seed "1:2:3:4"
			--quickstart-faction "GAIANS"
			--nosound
			--headless
			--windowed
			--window-size "1024x768"
		)
		SET_TESTS_PROPERTIES( planet_buster_runtime_smoke PROPERTIES
			FAIL_REGULAR_EXPRESSION "PLANET_BUSTER_RUNTIME_FAIL"
			PASS_REGULAR_EXPRESSION "PLANET_BUSTER_RUNTIME_PASS"
			RUN_SERIAL TRUE
			TIMEOUT 90
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		)

		SET( ECONOMIC_VICTORY_RUNTIME_TEST_PROFILE_DIR "${TEST_PROFILE_DIR}/economic-victory-runtime-smoke" )
		FILE( MAKE_DIRECTORY "${ECONOMIC_VICTORY_RUNTIME_TEST_PROFILE_DIR}" )
		ADD_TEST( NAME economic_victory_runtime_smoke COMMAND
			$<TARGET_FILE:${PROJECT_NAME}>
			--prefix "${ECONOMIC_VICTORY_RUNTIME_TEST_PROFILE_DIR}"
			--smacpath "${GLSMAC_SMAC_PATH}"
			--mainscript "../tests/_economic_victory_runtime_smoke"
			--quickstart
			--quickstart-ai "1"
			--quickstart-mapsize "20x10"
			--quickstart-seed "1:2:3:4"
			--quickstart-faction "GAIANS"
			--nosound
			--headless
			--windowed
			--window-size "1024x768"
		)
		SET_TESTS_PROPERTIES( economic_victory_runtime_smoke PROPERTIES
			FAIL_REGULAR_EXPRESSION "ECONOMIC_VICTORY_RUNTIME_FAIL"
			PASS_REGULAR_EXPRESSION "ECONOMIC_VICTORY_RUNTIME_PASS"
			RUN_SERIAL TRUE
			TIMEOUT 150
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		)

		SET( PLANETARY_COUNCIL_RUNTIME_TEST_PROFILE_DIR "${TEST_PROFILE_DIR}/planetary-council-runtime-smoke" )
		FILE( MAKE_DIRECTORY "${PLANETARY_COUNCIL_RUNTIME_TEST_PROFILE_DIR}" )
		ADD_TEST( NAME planetary_council_runtime_smoke COMMAND
			$<TARGET_FILE:${PROJECT_NAME}>
			--prefix "${PLANETARY_COUNCIL_RUNTIME_TEST_PROFILE_DIR}"
			--smacpath "${GLSMAC_SMAC_PATH}"
			--mainscript "../tests/_planetary_council_runtime_smoke"
			--quickstart
			--quickstart-ai "1"
			--quickstart-mapsize "20x10"
			--quickstart-seed "1:2:3:4"
			--quickstart-faction "PEACEKEEPERS"
			--nosound
			--headless
			--windowed
			--window-size "1024x768"
		)
		SET_TESTS_PROPERTIES( planetary_council_runtime_smoke PROPERTIES
			FAIL_REGULAR_EXPRESSION "PLANETARY_COUNCIL_RUNTIME_FAIL"
			PASS_REGULAR_EXPRESSION "PLANETARY_COUNCIL_RUNTIME_PASS"
			RUN_SERIAL TRUE
			TIMEOUT 90
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		)

		SET( PLANETARY_DATALINKS_RUNTIME_TEST_PROFILE_DIR "${TEST_PROFILE_DIR}/planetary-datalinks-runtime-smoke" )
		FILE( MAKE_DIRECTORY "${PLANETARY_DATALINKS_RUNTIME_TEST_PROFILE_DIR}" )
		ADD_TEST( NAME planetary_datalinks_runtime_smoke COMMAND
			$<TARGET_FILE:${PROJECT_NAME}>
			--prefix "${PLANETARY_DATALINKS_RUNTIME_TEST_PROFILE_DIR}"
			--smacpath "${GLSMAC_SMAC_PATH}"
			--mainscript "../tests/_planetary_datalinks_runtime_smoke"
			--quickstart
			--quickstart-ai "3"
			--quickstart-mapsize "20x10"
			--quickstart-seed "1:2:3:4"
			--quickstart-faction "GAIANS"
			--nosound
			--headless
			--windowed
			--window-size "1024x768"
			--verbose
		)
		SET_TESTS_PROPERTIES( planetary_datalinks_runtime_smoke PROPERTIES
			FAIL_REGULAR_EXPRESSION "PLANETARY_DATALINKS_RUNTIME_FAIL"
			PASS_REGULAR_EXPRESSION "PLANETARY_DATALINKS_RUNTIME_PASS"
			RUN_SERIAL TRUE
			TIMEOUT 90
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		)

		SET( AIR_RUNTIME_TEST_PROFILE_DIR "${TEST_PROFILE_DIR}/air-runtime-smoke" )
		FILE( MAKE_DIRECTORY "${AIR_RUNTIME_TEST_PROFILE_DIR}" )
		ADD_TEST( NAME air_runtime_smoke COMMAND
			$<TARGET_FILE:${PROJECT_NAME}>
			--prefix "${AIR_RUNTIME_TEST_PROFILE_DIR}"
			--smacpath "${GLSMAC_SMAC_PATH}"
			--mainscript "../tests/_air_runtime_smoke"
			--quickstart
			--quickstart-mapsize "20x10"
			--quickstart-seed "1:2:3:4"
			--quickstart-faction "GAIANS"
			--nosound
			--windowed
			--window-size "1024x768"
			--verbose
		)
		SET_TESTS_PROPERTIES( air_runtime_smoke PROPERTIES
			FAIL_REGULAR_EXPRESSION "AIR_RUNTIME_FAIL"
			PASS_REGULAR_EXPRESSION "AIR_RUNTIME_PASS"
			RUN_SERIAL TRUE
			TIMEOUT 90
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		)

		SET( TRANSPORT_RUNTIME_TEST_PROFILE_DIR "${TEST_PROFILE_DIR}/transport-runtime-smoke" )
		FILE( MAKE_DIRECTORY "${TRANSPORT_RUNTIME_TEST_PROFILE_DIR}" )
		ADD_TEST( NAME transport_runtime_smoke COMMAND
			$<TARGET_FILE:${PROJECT_NAME}>
			--prefix "${TRANSPORT_RUNTIME_TEST_PROFILE_DIR}"
			--smacpath "${GLSMAC_SMAC_PATH}"
			--mainscript "../tests/_transport_runtime_smoke"
			--quickstart
			--quickstart-mapsize "20x10"
			--quickstart-seed "1:2:3:4"
			--quickstart-faction "GAIANS"
			--nosound
			--windowed
			--window-size "1024x768"
			--verbose
		)
		SET_TESTS_PROPERTIES( transport_runtime_smoke PROPERTIES
			FAIL_REGULAR_EXPRESSION "TRANSPORT_RUNTIME_FAIL"
			PASS_REGULAR_EXPRESSION "TRANSPORT_RUNTIME_PASS"
			RUN_SERIAL TRUE
			TIMEOUT 90
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		)

		SET( COMBAT_ACCESS_RUNTIME_TEST_PROFILE_DIR "${TEST_PROFILE_DIR}/combat-access-runtime-smoke" )
		FILE( MAKE_DIRECTORY "${COMBAT_ACCESS_RUNTIME_TEST_PROFILE_DIR}" )
		ADD_TEST( NAME combat_access_runtime_smoke COMMAND
			$<TARGET_FILE:${PROJECT_NAME}>
			--prefix "${COMBAT_ACCESS_RUNTIME_TEST_PROFILE_DIR}"
			--smacpath "${GLSMAC_SMAC_PATH}"
			--mainscript "../tests/_combat_access_runtime_smoke"
			--quickstart
			--quickstart-mapsize "20x10"
			--quickstart-seed "1:2:3:4"
			--quickstart-faction "GAIANS"
			--nosound
			--headless
			--windowed
			--window-size "1024x768"
		)
		SET_TESTS_PROPERTIES( combat_access_runtime_smoke PROPERTIES
			FAIL_REGULAR_EXPRESSION "COMBAT_ACCESS_RUNTIME_FAIL;Event rejected"
			PASS_REGULAR_EXPRESSION "COMBAT_ACCESS_RUNTIME_PASS"
			RUN_SERIAL TRUE
			TIMEOUT 90
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		)

		SET( UNITY_POD_RUNTIME_TEST_PROFILE_DIR "${TEST_PROFILE_DIR}/unity-pod-runtime-smoke" )
		FILE( MAKE_DIRECTORY "${UNITY_POD_RUNTIME_TEST_PROFILE_DIR}" )
		ADD_TEST( NAME unity_pod_runtime_smoke COMMAND
			$<TARGET_FILE:${PROJECT_NAME}>
			--prefix "${UNITY_POD_RUNTIME_TEST_PROFILE_DIR}"
			--smacpath "${GLSMAC_SMAC_PATH}"
			--mainscript "../tests/_unity_pod_runtime_smoke"
			--quickstart
			--quickstart-mapsize "40x20"
			--quickstart-seed "1:2:3:4"
			--quickstart-faction "GAIANS"
			--nosound
			--windowed
			--window-size "1024x768"
			--verbose
		)
		SET_TESTS_PROPERTIES( unity_pod_runtime_smoke PROPERTIES
			FAIL_REGULAR_EXPRESSION "UNITY_POD_RUNTIME_FAIL;Event rejected"
			PASS_REGULAR_EXPRESSION "UNITY_POD_RUNTIME_PASS"
			RUN_SERIAL TRUE
			TIMEOUT 120
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		)

		SET( NATIVE_LIFE_RUNTIME_TEST_PROFILE_DIR "${TEST_PROFILE_DIR}/native-life-runtime-smoke" )
		FILE( MAKE_DIRECTORY "${NATIVE_LIFE_RUNTIME_TEST_PROFILE_DIR}" )
		ADD_TEST( NAME native_life_runtime_smoke COMMAND
			$<TARGET_FILE:${PROJECT_NAME}>
			--prefix "${NATIVE_LIFE_RUNTIME_TEST_PROFILE_DIR}"
			--smacpath "${GLSMAC_SMAC_PATH}"
			--mainscript "../tests/_native_life_runtime_smoke"
			--quickstart
			--quickstart-mapsize "20x10"
			--quickstart-seed "1:2:3:4"
			--quickstart-faction "GAIANS"
			--nosound
			--headless
			--windowed
			--window-size "1024x768"
			--verbose
		)
		SET_TESTS_PROPERTIES( native_life_runtime_smoke PROPERTIES
			FAIL_REGULAR_EXPRESSION "NATIVE_LIFE_RUNTIME_FAIL;Event rejected"
			PASS_REGULAR_EXPRESSION "NATIVE_LIFE_RUNTIME_PASS"
			RUN_SERIAL TRUE
			TIMEOUT 90
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		)

		SET( NATIVE_CAPTURE_RUNTIME_TEST_PROFILE_DIR "${TEST_PROFILE_DIR}/native-capture-runtime-smoke" )
		FILE( MAKE_DIRECTORY "${NATIVE_CAPTURE_RUNTIME_TEST_PROFILE_DIR}" )
		ADD_TEST( NAME native_capture_runtime_smoke COMMAND
			$<TARGET_FILE:${PROJECT_NAME}>
			--prefix "${NATIVE_CAPTURE_RUNTIME_TEST_PROFILE_DIR}"
			--smacpath "${GLSMAC_SMAC_PATH}"
			--mainscript "../tests/_native_capture_runtime_smoke"
			--quickstart
			--quickstart-mapsize "20x10"
			--quickstart-seed "1:2:3:4"
			--quickstart-faction "GAIANS"
			--nosound
			--headless
			--windowed
			--window-size "1024x768"
			--verbose
		)
		SET_TESTS_PROPERTIES( native_capture_runtime_smoke PROPERTIES
			FAIL_REGULAR_EXPRESSION "NATIVE_CAPTURE_RUNTIME_FAIL;Event rejected"
			PASS_REGULAR_EXPRESSION "NATIVE_CAPTURE_RUNTIME_PASS"
			RUN_SERIAL TRUE
			TIMEOUT 90
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		)

		SET( FACILITY_ACTIONS_RUNTIME_TEST_PROFILE_DIR "${TEST_PROFILE_DIR}/facility-actions-runtime-smoke" )
		FILE( MAKE_DIRECTORY "${FACILITY_ACTIONS_RUNTIME_TEST_PROFILE_DIR}" )
		ADD_TEST( NAME facility_actions_runtime_smoke COMMAND
			$<TARGET_FILE:${PROJECT_NAME}>
			--prefix "${FACILITY_ACTIONS_RUNTIME_TEST_PROFILE_DIR}"
			--smacpath "${GLSMAC_SMAC_PATH}"
			--mainscript "../tests/_facility_actions_runtime_smoke"
			--quickstart
			--quickstart-mapsize "20x10"
			--quickstart-seed "1:2:3:4"
			--quickstart-faction "GAIANS"
			--nosound
			--windowed
			--window-size "1024x768"
			--verbose
		)
		SET_TESTS_PROPERTIES( facility_actions_runtime_smoke PROPERTIES
			FAIL_REGULAR_EXPRESSION "FACILITY_ACTIONS_RUNTIME_FAIL;Event rejected"
			PASS_REGULAR_EXPRESSION "FACILITY_ACTIONS_RUNTIME_PASS"
			RUN_SERIAL TRUE
			TIMEOUT 90
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		)

		SET( SEA_COLONY_RUNTIME_TEST_PROFILE_DIR "${TEST_PROFILE_DIR}/sea-colony-runtime-smoke" )
		FILE( MAKE_DIRECTORY "${SEA_COLONY_RUNTIME_TEST_PROFILE_DIR}" )
		ADD_TEST( NAME sea_colony_runtime_smoke COMMAND
			$<TARGET_FILE:${PROJECT_NAME}>
			--prefix "${SEA_COLONY_RUNTIME_TEST_PROFILE_DIR}"
			--smacpath "${GLSMAC_SMAC_PATH}"
			--mainscript "../tests/_sea_colony_runtime_smoke"
			--quickstart
			--quickstart-mapsize "20x10"
			--quickstart-seed "1:2:3:4"
			--quickstart-faction "GAIANS"
			--nosound
			--windowed
			--window-size "1024x768"
			--verbose
		)
		SET_TESTS_PROPERTIES( sea_colony_runtime_smoke PROPERTIES
			FAIL_REGULAR_EXPRESSION "SEA_COLONY_RUNTIME_FAIL;Event rejected"
			PASS_REGULAR_EXPRESSION "SEA_COLONY_RUNTIME_PASS"
			RUN_SERIAL TRUE
			TIMEOUT 90
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		)

		SET( AI_RUNTIME_TEST_PROFILE_DIR "${TEST_PROFILE_DIR}/ai-runtime-smoke" )
		FILE( MAKE_DIRECTORY "${AI_RUNTIME_TEST_PROFILE_DIR}" )
		ADD_TEST( NAME ai_runtime_smoke COMMAND
			$<TARGET_FILE:${PROJECT_NAME}>
			--prefix "${AI_RUNTIME_TEST_PROFILE_DIR}"
			--smacpath "${GLSMAC_SMAC_PATH}"
			--mainscript "../tests/_ai_runtime_smoke"
			--quickstart
			--quickstart-ai "1"
			--quickstart-mapsize "20x10"
			--quickstart-seed "1:2:3:4"
			--quickstart-faction "GAIANS"
			--nosound
			--headless
			--windowed
			--window-size "1024x768"
			--verbose
		)
		SET_TESTS_PROPERTIES( ai_runtime_smoke PROPERTIES
			FAIL_REGULAR_EXPRESSION "AI_RUNTIME_FAIL"
			PASS_REGULAR_EXPRESSION "AI_RUNTIME_PASS"
			RUN_SERIAL TRUE
			TIMEOUT 210
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		)

		SET( AI_ECONOMY_SOAK_PROFILE_DIR "${TEST_PROFILE_DIR}/ai-economy-soak" )
		FILE( MAKE_DIRECTORY "${AI_ECONOMY_SOAK_PROFILE_DIR}" )
		ADD_TEST( NAME ai_economy_soak COMMAND
			$<TARGET_FILE:${PROJECT_NAME}>
			--prefix "${AI_ECONOMY_SOAK_PROFILE_DIR}"
			--smacpath "${GLSMAC_SMAC_PATH}"
			--mainscript "../tests/_ai_economy_soak"
			--quickstart
			--quickstart-ai "1"
			--quickstart-mapsize "20x10"
			--quickstart-seed "1:2:3:4"
			--quickstart-faction "GAIANS"
			--nosound
			--headless
			--windowed
			--window-size "1024x768"
		)
		SET_TESTS_PROPERTIES( ai_economy_soak PROPERTIES
			FAIL_REGULAR_EXPRESSION "AI_ECONOMY_SOAK_FAIL;Event rejected"
			PASS_REGULAR_EXPRESSION "AI_ECONOMY_SOAK_PASS"
			LABELS runtime_long
			RUN_SERIAL TRUE
			TIMEOUT 300
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		)

		SET( AI_OPPONENT_STRATEGY_RUNTIME_TEST_PROFILE_DIR "${TEST_PROFILE_DIR}/ai-opponent-strategy-runtime-smoke" )
		FILE( MAKE_DIRECTORY "${AI_OPPONENT_STRATEGY_RUNTIME_TEST_PROFILE_DIR}" )
		ADD_TEST( NAME ai_opponent_strategy_runtime_smoke COMMAND
			$<TARGET_FILE:${PROJECT_NAME}>
			--prefix "${AI_OPPONENT_STRATEGY_RUNTIME_TEST_PROFILE_DIR}"
			--smacpath "${GLSMAC_SMAC_PATH}"
			--mainscript "../tests/_ai_opponent_strategy_runtime_smoke"
			--quickstart
			--quickstart-ai "1"
			--quickstart-mapsize "20x10"
			--quickstart-seed "1:2:3:4"
			--quickstart-faction "GAIANS"
			--nosound
			--headless
			--windowed
			--window-size "1024x768"
			--verbose
		)
		SET_TESTS_PROPERTIES( ai_opponent_strategy_runtime_smoke PROPERTIES
			FAIL_REGULAR_EXPRESSION "AI_OPPONENT_STRATEGY_RUNTIME_FAIL;Event rejected"
			PASS_REGULAR_EXPRESSION "AI_OPPONENT_STRATEGY_RUNTIME_PASS"
			RUN_SERIAL TRUE
			TIMEOUT 90
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		)

		SET( AI_CONQUEST_RUNTIME_TEST_PROFILE_DIR "${TEST_PROFILE_DIR}/ai-conquest-runtime-smoke" )
		FILE( MAKE_DIRECTORY "${AI_CONQUEST_RUNTIME_TEST_PROFILE_DIR}" )
		ADD_TEST( NAME ai_conquest_runtime_smoke COMMAND
			$<TARGET_FILE:${PROJECT_NAME}>
			--prefix "${AI_CONQUEST_RUNTIME_TEST_PROFILE_DIR}"
			--smacpath "${GLSMAC_SMAC_PATH}"
			--mainscript "../tests/_ai_conquest_runtime_smoke"
			--quickstart
			--quickstart-ai "1"
			--quickstart-mapsize "20x10"
			--quickstart-seed "1:2:3:4"
			--quickstart-faction "GAIANS"
			--nosound
			--headless
			--windowed
			--window-size "1024x768"
			--verbose
		)
		SET_TESTS_PROPERTIES( ai_conquest_runtime_smoke PROPERTIES
			FAIL_REGULAR_EXPRESSION "AI_CONQUEST_RUNTIME_FAIL"
			PASS_REGULAR_EXPRESSION "AI_CONQUEST_RUNTIME_PASS"
			RUN_SERIAL TRUE
			TIMEOUT 60
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		)

		SET( AI_REPAIR_RUNTIME_TEST_PROFILE_DIR "${TEST_PROFILE_DIR}/ai-repair-runtime-smoke" )
		FILE( MAKE_DIRECTORY "${AI_REPAIR_RUNTIME_TEST_PROFILE_DIR}" )
		ADD_TEST( NAME ai_repair_runtime_smoke COMMAND
			$<TARGET_FILE:${PROJECT_NAME}>
			--prefix "${AI_REPAIR_RUNTIME_TEST_PROFILE_DIR}"
			--smacpath "${GLSMAC_SMAC_PATH}"
			--mainscript "../tests/_ai_repair_runtime_smoke"
			--quickstart
			--quickstart-ai "1"
			--quickstart-mapsize "20x10"
			--quickstart-seed "1:2:3:4"
			--quickstart-faction "GAIANS"
			--nosound
			--headless
			--windowed
			--window-size "1024x768"
		)
		SET_TESTS_PROPERTIES( ai_repair_runtime_smoke PROPERTIES
			FAIL_REGULAR_EXPRESSION "AI_REPAIR_RUNTIME_FAIL"
			PASS_REGULAR_EXPRESSION "AI_REPAIR_RUNTIME_PASS"
			RUN_SERIAL TRUE
			TIMEOUT 90
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		)

		SET( AI_AIR_RUNTIME_TEST_PROFILE_DIR "${TEST_PROFILE_DIR}/ai-air-runtime-smoke" )
		FILE( MAKE_DIRECTORY "${AI_AIR_RUNTIME_TEST_PROFILE_DIR}" )
		ADD_TEST( NAME ai_air_runtime_smoke COMMAND
			$<TARGET_FILE:${PROJECT_NAME}>
			--prefix "${AI_AIR_RUNTIME_TEST_PROFILE_DIR}"
			--smacpath "${GLSMAC_SMAC_PATH}"
			--mainscript "../tests/_ai_air_runtime_smoke"
			--quickstart
			--quickstart-ai "1"
			--quickstart-mapsize "20x10"
			--quickstart-seed "1:2:3:4"
			--quickstart-faction "GAIANS"
			--nosound
			--headless
			--windowed
			--window-size "1024x768"
			--verbose
		)
		SET_TESTS_PROPERTIES( ai_air_runtime_smoke PROPERTIES
			FAIL_REGULAR_EXPRESSION "AI_AIR_RUNTIME_FAIL;Event rejected"
			PASS_REGULAR_EXPRESSION "AI_AIR_RUNTIME_PASS"
			RUN_SERIAL TRUE
			TIMEOUT 60
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		)

		SET( AI_REINFORCEMENT_RUNTIME_TEST_PROFILE_DIR "${TEST_PROFILE_DIR}/ai-reinforcement-runtime-smoke" )
		FILE( MAKE_DIRECTORY "${AI_REINFORCEMENT_RUNTIME_TEST_PROFILE_DIR}" )
		ADD_TEST( NAME ai_reinforcement_runtime_smoke COMMAND
			$<TARGET_FILE:${PROJECT_NAME}>
			--prefix "${AI_REINFORCEMENT_RUNTIME_TEST_PROFILE_DIR}"
			--smacpath "${GLSMAC_SMAC_PATH}"
			--mainscript "../tests/_ai_reinforcement_runtime_smoke"
			--quickstart
			--quickstart-ai "1"
			--quickstart-mapsize "20x10"
			--quickstart-seed "1:2:3:4"
			--quickstart-faction "GAIANS"
			--nosound
			--headless
			--windowed
			--window-size "1024x768"
		)
		SET_TESTS_PROPERTIES( ai_reinforcement_runtime_smoke PROPERTIES
			FAIL_REGULAR_EXPRESSION "AI_REINFORCEMENT_RUNTIME_FAIL"
			PASS_REGULAR_EXPRESSION "AI_REINFORCEMENT_RUNTIME_PASS"
			RUN_SERIAL TRUE
			TIMEOUT 120
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		)

		SET( AI_HURRY_RUNTIME_TEST_PROFILE_DIR "${TEST_PROFILE_DIR}/ai-hurry-runtime-smoke" )
		FILE( MAKE_DIRECTORY "${AI_HURRY_RUNTIME_TEST_PROFILE_DIR}" )
		ADD_TEST( NAME ai_hurry_runtime_smoke COMMAND
			$<TARGET_FILE:${PROJECT_NAME}>
			--prefix "${AI_HURRY_RUNTIME_TEST_PROFILE_DIR}"
			--smacpath "${GLSMAC_SMAC_PATH}"
			--mainscript "../tests/_ai_hurry_runtime_smoke"
			--quickstart
			--quickstart-ai "1"
			--quickstart-mapsize "20x10"
			--quickstart-seed "1:2:3:4"
			--quickstart-faction "GAIANS"
			--nosound
			--headless
			--windowed
			--window-size "1024x768"
			--verbose
		)
		SET_TESTS_PROPERTIES( ai_hurry_runtime_smoke PROPERTIES
			FAIL_REGULAR_EXPRESSION "AI_HURRY_RUNTIME_FAIL"
			PASS_REGULAR_EXPRESSION "AI_HURRY_RUNTIME_PASS"
			RUN_SERIAL TRUE
			TIMEOUT 60
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		)

		SET( SEVEN_PLAYER_RUNTIME_TEST_PROFILE_DIR "${TEST_PROFILE_DIR}/seven-player-runtime-smoke" )
		FILE( MAKE_DIRECTORY "${SEVEN_PLAYER_RUNTIME_TEST_PROFILE_DIR}" )
		ADD_TEST( NAME seven_player_runtime_smoke COMMAND
			$<TARGET_FILE:${PROJECT_NAME}>
			--prefix "${SEVEN_PLAYER_RUNTIME_TEST_PROFILE_DIR}"
			--smacpath "${GLSMAC_SMAC_PATH}"
			--mainscript "../tests/_ai_runtime_smoke"
			--quickstart
			--quickstart-ai "6"
			--quickstart-mapsize "40x20"
			--quickstart-seed "1:2:3:4"
			--quickstart-faction "GAIANS"
			--nosound
			--headless
			--windowed
			--window-size "1024x768"
			--verbose
		)
		SET_TESTS_PROPERTIES( seven_player_runtime_smoke PROPERTIES
			FAIL_REGULAR_EXPRESSION "SEVEN_PLAYER_RUNTIME_FAIL"
			PASS_REGULAR_EXPRESSION "SEVEN_PLAYER_RUNTIME_PASS"
			RUN_SERIAL TRUE
			TIMEOUT 90
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		)

		FIND_PACKAGE( Python3 COMPONENTS Interpreter QUIET )
		IF ( Python3_Interpreter_FOUND )
			SET( MULTIPLAYER_TEST_OUTPUT_DIR "${TEST_PROFILE_DIR}/multiplayer-runtime-smoke" )
			ADD_TEST( NAME multiplayer_runtime_smoke COMMAND
				"${Python3_EXECUTABLE}"
				"${PROJECT_SOURCE_DIR}/tools/tests/multiplayer_smoke.py"
				--executable "$<TARGET_FILE:${PROJECT_NAME}>"
				--smac-path "${GLSMAC_SMAC_PATH}"
				--output-dir "${MULTIPLAYER_TEST_OUTPUT_DIR}"
				--workdir "${PROJECT_SOURCE_DIR}"
				--timeout 90
				--malformed-probes
				--exercise-reconnect
			)
			SET_TESTS_PROPERTIES( multiplayer_runtime_smoke PROPERTIES
				RUN_SERIAL TRUE
				TIMEOUT 150
				WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
			)

			SET( RUNNING_RECONNECT_TEST_OUTPUT_DIR "${TEST_PROFILE_DIR}/running-reconnect-smoke" )
			ADD_TEST( NAME running_reconnect_smoke COMMAND
				"${Python3_EXECUTABLE}"
				"${PROJECT_SOURCE_DIR}/tools/tests/running_reconnect_smoke.py"
				--executable "$<TARGET_FILE:${PROJECT_NAME}>"
				--smac-path "${GLSMAC_SMAC_PATH}"
				--output-dir "${RUNNING_RECONNECT_TEST_OUTPUT_DIR}"
				--workdir "${PROJECT_SOURCE_DIR}"
				--timeout 90
			)
			SET_TESTS_PROPERTIES( running_reconnect_smoke PROPERTIES
				RUN_SERIAL TRUE
				TIMEOUT 300
				WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
			)
		ENDIF ()
	ENDIF ()
ENDIF ()

# Some 32-bit targets implement 64-bit std::atomic operations in libatomic.
INCLUDE( CheckCXXSourceCompiles )
SET( ATOMIC_TEST_SOURCE "
	#include <atomic>
	#include <cstdint>
	std::atomic<std::uint64_t> value{0};
	int main() {
		value.fetch_add( 1, std::memory_order_relaxed );
		return static_cast<int>( value.load( std::memory_order_relaxed ) );
	}
" )
CHECK_CXX_SOURCE_COMPILES( "${ATOMIC_TEST_SOURCE}" HAVE_CXX_ATOMICS_WITHOUT_LIB )
IF ( NOT HAVE_CXX_ATOMICS_WITHOUT_LIB )
	SET( SAVED_CMAKE_REQUIRED_LIBRARIES "${CMAKE_REQUIRED_LIBRARIES}" )
	SET( CMAKE_REQUIRED_LIBRARIES "${CMAKE_REQUIRED_LIBRARIES};atomic" )
	CHECK_CXX_SOURCE_COMPILES( "${ATOMIC_TEST_SOURCE}" HAVE_CXX_ATOMICS_WITH_LIBATOMIC )
	SET( CMAKE_REQUIRED_LIBRARIES "${SAVED_CMAKE_REQUIRED_LIBRARIES}" )
	IF ( NOT HAVE_CXX_ATOMICS_WITH_LIBATOMIC )
		MESSAGE( FATAL_ERROR "64-bit std::atomic operations are not supported by the compiler or libatomic" )
	ENDIF ()
	TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE atomic )
ENDIF ()

IF ( NOT WIN32 )
	TARGET_COMPILE_DEFINITIONS( ${PROJECT_NAME} PRIVATE TMP_LAST_COMMIT_H_GENERATED=1 )
ENDIF ()

IF ( CMAKE_BUILD_TYPE STREQUAL "" )
	SET( CMAKE_BUILD_TYPE "Release" ) # release by default
ENDIF ()

# not using -Og for debug builds because it optimizes some variables out
IF ( MSVC )
	SET( DEBUG_FLAGS "" )
ELSE ()
	SET( DEBUG_FLAGS "-ggdb -O0" )
ENDIF ()
IF ( SANITIZER )
	SET( DEBUG_FLAGS "${DEBUG_FLAGS} -fsanitize=address -fsanitize=undefined" )
ENDIF ()
IF ( CMAKE_CONFIGURATION_TYPES )
	TARGET_COMPILE_DEFINITIONS( ${PROJECT_NAME} PRIVATE
		$<$<CONFIG:Debug>:DEBUG=1>
		$<$<CONFIG:FastDebug>:FASTDEBUG=1>
	)
ENDIF ()
IF ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
	IF ( NOT CMAKE_CONFIGURATION_TYPES )
		TARGET_COMPILE_DEFINITIONS( ${PROJECT_NAME} PRIVATE DEBUG=1 )
	ENDIF ()
	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DEBUG_FLAGS}" )
ELSEIF ( CMAKE_BUILD_TYPE STREQUAL "FastDebug" )
	IF ( NOT CMAKE_CONFIGURATION_TYPES )
		TARGET_COMPILE_DEFINITIONS( ${PROJECT_NAME} PRIVATE FASTDEBUG=1 )
	ENDIF ()
	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DEBUG_FLAGS}" )
ELSE ()
	IF ( MSVC )
		SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2" )
	ELSE ()
		SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3" )
	ENDIF ()
	IF ( CMAKE_BUILD_TYPE STREQUAL "Portable" )
		MESSAGE( FATAL_ERROR "Portable target is deprecated, please use Portable32 or Portable64 instead" )
	ELSEIF ( CMAKE_BUILD_TYPE STREQUAL "Portable64" OR CMAKE_BUILD_TYPE STREQUAL "Portable32" )
		TARGET_COMPILE_DEFINITIONS( ${PROJECT_NAME} PRIVATE PORTABLE=1 )
		SET( VENDORED_DEPENDENCIES 1 ) # for static linking
		IF ( WIN32 )
			TARGET_LINK_OPTIONS( ${PROJECT_NAME} PRIVATE -static ) # this won't work on linux because there may be binary video drivers without static libraries
		ELSE ()
			TARGET_LINK_OPTIONS( ${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++ )
		ENDIF ()
		IF ( CMAKE_BUILD_TYPE STREQUAL "Portable32" )
			SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -march=i686 -mtune=i686" )
		ELSEIF ( CMAKE_BUILD_TYPE STREQUAL "Portable64" )
			SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -march=x86-64 -mtune=k8" )
		ENDIF ()
	ELSEIF ( CMAKE_BUILD_TYPE STREQUAL "Release" )
		IF ( NOT MSVC )
			SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native" )
		ENDIF ()
	ELSE ()
		MESSAGE( FATAL_ERROR "Invalid CMAKE_BUILD_TYPE \"${CMAKE_BUILD_TYPE}\", use one of: \nDebug\nRelease\nPortable32\nPortable64" )
	ENDIF ()
ENDIF ()

ADD_CUSTOM_COMMAND(
	OUTPUT ${EMBED_CPP}
	DEPENDS ${EMBED_DEPENDS} embedder
	COMMAND "$<TARGET_FILE:embedder>" "${EMBED_CPP}" "${PROJECT_SOURCE_DIR}" ${EMBED_FILES}
)
ADD_CUSTOM_TARGET( embed ALL DEPENDS ${EMBED_DEPENDS} embedder )

INCLUDE_DIRECTORIES( "src" )

IF ( VISUAL_STUDIO ) # (Provided in CMakePresets)
	ADD_COMPILE_DEFINITIONS( _ITERATOR_DEBUG_LEVEL=0 )
	ADD_COMPILE_DEFINITIONS( VISUAL_STUDIO )
ELSEIF ( WIN32 ) # probably mingw or github runner
	TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE wsock32 )
	TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE ws2_32 )
ENDIF ()

# needed for some uncommon IDE and OS combinations
FIND_LIBRARY( LIBCPP_LIBRARY NAMES libc++.so libc++.a )
IF ( LIBCPP_LIBRARY )
	TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE -lc++ )
ENDIF ()
FIND_LIBRARY( LIBM_LIBRARY NAMES libm.so libm.a )
IF ( LIBM_LIBRARY )
	TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE -lm )
ENDIF ()

IF ( VENDORED_DEPENDENCIES )
	ADD_SUBDIRECTORY( dependencies )
	TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE libglew_static )
	TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE freetype )
	TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE SDL2-static )
	TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE SDL2_image )
	IF ( NOT WIN32 ) # TODO: make it buildable on windows
		TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE ossp-uuid )
	ENDIF ()
	TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE yaml-cpp )
	ADD_COMPILE_DEFINITIONS( VENDORED_DEPENDENCIES )

	TARGET_INCLUDE_DIRECTORIES( ${PROJECT_NAME} PUBLIC
		"${glew_SOURCE_DIR}/include"
		"${OSSP_UUID_INCLUDE_DIR}"
		"${YAML_CPP_SOURCE_DIR}/include"
	)
	TARGET_LINK_OPTIONS( ${PROJECT_NAME} PUBLIC
		"${OSSP_UUID_LINK_OPTIONS}"
	)
ELSE ()
	INCLUDE( FindPkgConfig )

	PKG_SEARCH_MODULE( FT2 REQUIRED freetype2 )
	PKG_SEARCH_MODULE( SDL2 REQUIRED sdl2 )
	PKG_SEARCH_MODULE( SDL2IMAGE REQUIRED SDL2_image>=2.0.0 )
	PKG_SEARCH_MODULE( GLEW REQUIRED glew )
	IF ( NOT APPLE )
		PKG_SEARCH_MODULE( OSSPUUID REQUIRED ossp-uuid )
	ELSE ()
		PKG_SEARCH_MODULE( OSSPUUID REQUIRED uuid )
	ENDIF ()
	PKG_SEARCH_MODULE( YAMLCPP REQUIRED yaml-cpp )

	INCLUDE_DIRECTORIES( ${FT2_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${OSSPUUID_INCLUDE_DIRS} ${YAMLCPP_INCLUDE_DIRS} )
	TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE ${FT2_LIBRARIES} ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${GLEW_LIBRARIES} ${OSSPUUID_LIBRARIES} ${YAMLCPP_LIBRARIES} )
ENDIF ()

IF ( NOT WIN32 )
	ADD_DEPENDENCIES( ${PROJECT_NAME} glsmac_version )
	TARGET_LINK_OPTIONS( ${PROJECT_NAME} PRIVATE ${FT2_LDFLAGS} ${GLEW_LDFLAGS} ${SDL2_LDFLAGS} ${SDL2IMAGE_LDFLAGS} )
ENDIF ()

IF ( APPLE )
	TARGET_LINK_OPTIONS( ${PROJECT_NAME} PRIVATE -framework OpenGL )
ENDIF ()

IF ( WIN32 )
	TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE advapi32 )
	IF ( MSVC )
		TARGET_LINK_LIBRARIES( ${PROJECT_NAME} PRIVATE ws2_32 )
	ELSE ()
		TARGET_LINK_OPTIONS( ${PROJECT_NAME} PRIVATE -lws2_32 )
	ENDIF ()
ENDIF ()

IF ( NOT MSVC )
	IF (
		CMAKE_BUILD_TYPE STREQUAL "Release" OR
		CMAKE_BUILD_TYPE STREQUAL "Portable64" OR
		CMAKE_BUILD_TYPE STREQUAL "Portable32"
	)
		TARGET_LINK_OPTIONS( ${PROJECT_NAME} PRIVATE "-s" )
	ENDIF ()
ENDIF ()
