MESSAGE( STATUS "Fetching ossp-uuid..." )

FETCHCONTENT_DECLARE( OSSP_UUID
	GIT_REPOSITORY https://github.com/sean-/ossp-uuid
	GIT_TAG origin/master
)

FETCHCONTENT_MAKEAVAILABLE( OSSP_UUID )
