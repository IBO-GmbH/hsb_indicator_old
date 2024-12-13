macro (mold_init_versions)
	if(NOT MOLD_VERSION_MAJOR)
		set(MOLD_VERSION_MAJOR 0)
	endif()

	if(NOT MOLD_VERSION_MINOR)
		set(MOLD_VERSION_MINOR 0)
	endif()

	if(NOT MOLD_VERSION_BUILD)
		set(MOLD_VERSION_BUILD 0)
	endif()
endmacro ()

#
# git/ci versions query functions
#

macro(mold_get_git_revision_count)
    execute_process(COMMAND           git rev-list --first-parent --count HEAD
                    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                    OUTPUT_VARIABLE   GIT_REVISION_COUNT)
    string(STRIP ${GIT_REVISION_COUNT} GIT_REVISION_COUNT)
endmacro()

macro(mold_get_git_branch_name)
    set(GIT_BRANCH_NAME "$ENV{CI_COMMIT_REF_NAME}")
    if (NOT GIT_BRANCH_NAME)
        execute_process(COMMAND           git rev-parse --abbrev-ref HEAD
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                        OUTPUT_VARIABLE   GIT_BRANCH_NAME)
        string(STRIP ${GIT_BRANCH_NAME} GIT_BRANCH_NAME)
    endif()
endmacro()

macro(mold_get_git_minor_major)
    message(STATUS "git branch name is ${GIT_BRANCH_NAME}")
    string(REGEX MATCH "^release\/([0-9]+).([0-9]+)" DUMMY "${GIT_BRANCH_NAME}")
    set(GIT_RELEASE_VERSION_MAJOR ${CMAKE_MATCH_1})
    set(GIT_RELEASE_VERSION_MINOR ${CMAKE_MATCH_2})

    if (NOT "${GIT_RELEASE_VERSION_MAJOR}" STREQUAL "" AND NOT "${GIT_RELEASE_VERSION_MINOR}" STREQUAL "")
        set(MOLD_IS_CI_RELEASE_BRANCH_BUILD TRUE)
    endif()

    if (MOLD_IS_CI_RELEASE_BRANCH_BUILD)
        message(STATUS "Getting release version from git branch ${GIT_BRANCH_NAME}: ${GIT_RELEASE_VERSION_MAJOR}.${GIT_RELEASE_VERSION_MINOR}")
    else()
        message(STATUS "Not on a release branch")
    endif()
endmacro()

macro(mold_get_git_commit_hash)
    execute_process(COMMAND           git rev-parse --short HEAD
                    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
                    OUTPUT_VARIABLE   GIT_COMMIT_HASH)
    string(STRIP ${GIT_COMMIT_HASH} GIT_COMMIT_HASH)
endmacro()

macro(mold_get_version_build)
    set(CI_PIPELINE_IID "$ENV{CI_PIPELINE_IID}")

    if(NOT CI_PIPELINE_IID MATCHES "[0-9]+")
        message(STATUS "Build version not set, using git revision count")
        set(MOLD_VERSION_BUILD ${GIT_REVISION_COUNT})
        set(MOLD_VERSION_BUILD_IS_SET_FROM_GIT TRUE)
    else()
        MATH(EXPR BUILD_OFFSET "${CI_PIPELINE_IID} - ${VERSION_FILE_PIPELINE_OFFSET}")

	if(${BUILD_OFFSET} LESS 0)
            message(FATAL_ERROR "BUILD_OFFSET negative, Pipeline id smaller than offset: pipeline id '${CI_PIPELINE_IID}' vs offset '${VERSION_FILE_PIPELINE_OFFSET}'")

        message(STATUS "Using actual pipeline ID ${CI_PIPELINE_IID} with offset ${VERSION_FILE_PIPELINE_OFFSET}")
	endif()

        set(MOLD_VERSION_BUILD "${BUILD_OFFSET}")
    endif()
endmacro()

#
# versions file functions
#

macro(mold_read_versions_file)
    file(STRINGS "${PROJECT_SOURCE_DIR}/version.txt" VERSION_FILE_CONTENT LIMIT_COUNT 2)
    list(GET VERSION_FILE_CONTENT 0 VERSION_FILE_VERSION_STRING)
    list(GET VERSION_FILE_CONTENT 1 VERSION_FILE_PIPELINE_OFFSET_STRING)

    string(REGEX MATCH "^version=([0-9]+).([0-9]+)" DUMMY "${VERSION_FILE_VERSION_STRING}")
    set(VERSION_FILE_VERSION_MAJOR ${CMAKE_MATCH_1})
    set(VERSION_FILE_VERSION_MINOR ${CMAKE_MATCH_2})

    message(STATUS "version.txt version=${VERSION_FILE_VERSION_MAJOR}.${VERSION_FILE_VERSION_MINOR}")

    string(REGEX MATCH "^pipeline_offset=([0-9]+)" DUMMY "${VERSION_FILE_PIPELINE_OFFSET_STRING}")
    set(VERSION_FILE_PIPELINE_OFFSET ${CMAKE_MATCH_1})

    message(STATUS "version.txt pipeline_offset=${VERSION_FILE_PIPELINE_OFFSET}")
endmacro()

macro(mold_check_git_version_against_versions_file)
    if (MOLD_IS_CI_RELEASE_BRANCH_BUILD)
        if ("${GIT_RELEASE_VERSION_MAJOR}.${GIT_RELEASE_VERSION_MINOR}" STREQUAL "${VERSION_FILE_VERSION_MAJOR}.${VERSION_FILE_VERSION_MINOR}")
            set(MOLD_VERSION_CHECK_SUCCESS TRUE)
            message(STATUS "${GIT_RELEASE_VERSION_MAJOR}.${GIT_RELEASE_VERSION_MINOR} = ${VERSION_FILE_VERSION_MAJOR}.${VERSION_FILE_VERSION_MINOR}")
        else()
            message(FATAL_ERROR "versions differ in branch and version file: '${MOLD_VERSION_MAJOR}.${MOLD_VERSION_MINOR}' STREQUAL '${VERSION_FILE_VERSION_MAJOR}.${VERSION_FILE_VERSION_MINOR}'")
        endif()
    endif()
endmacro()

macro(mold_determine_versions)
    mold_get_git_revision_count()
    mold_get_git_branch_name()
    mold_get_git_commit_hash()
    mold_get_git_minor_major()

    mold_read_versions_file()

    mold_check_git_version_against_versions_file()

    set(MOLD_VERSION_MAJOR ${VERSION_FILE_VERSION_MAJOR})
    set(MOLD_VERSION_MINOR ${VERSION_FILE_VERSION_MINOR})

    mold_get_version_build()
endmacro()

#
# Generate versions for target functions
#

macro(mold_set_version_strings)
    set(MOLD_VERSION "${MOLD_VERSION_MAJOR}.${MOLD_VERSION_MINOR}.${MOLD_VERSION_BUILD}")
endmacro()
