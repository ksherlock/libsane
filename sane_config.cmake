INCLUDE (CheckFunctionExists)

SET(CMAKE_EXTRA_INCLUDE_FILES stdio.h)
CHECK_FUNCTION_EXISTS(asprintf HAVE_ASPRINTF)
SET(CMAKE_EXTRA_INCLUDE_FILES)


CONFIGURE_FILE(sane_config.h.in sane_config.h)
