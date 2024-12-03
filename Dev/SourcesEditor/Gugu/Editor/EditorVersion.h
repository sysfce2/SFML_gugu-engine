#pragma once

////////////////////////////////////////////////////////////////
// Includes

#include "Gugu/Common.h"

////////////////////////////////////////////////////////////////
// Macros

#define GUGU_EDITOR_VERSION_MAJOR 0
#define GUGU_EDITOR_VERSION_MINOR 3
#define GUGU_EDITOR_VERSION_PATCH 1
#define GUGU_EDITOR_VERSION_IS_DEV false
#define GUGU_EDITOR_VERSION_IS_PRE_RELEASE false
#define GUGU_EDITOR_VERSION_IS_POST_RELEASE false

#define GUGU_EDITOR_VERSION_NUMBER GUGU_STRINGIZE(GUGU_EDITOR_VERSION_MAJOR) "." GUGU_STRINGIZE(GUGU_EDITOR_VERSION_MINOR) "." GUGU_STRINGIZE(GUGU_EDITOR_VERSION_PATCH)

#if GUGU_EDITOR_VERSION_IS_DEV
#define GUGU_EDITOR_VERSION GUGU_EDITOR_VERSION_NUMBER "-dev"
#elif GUGU_EDITOR_VERSION_IS_PRE_RELEASE
#define GUGU_EDITOR_VERSION GUGU_EDITOR_VERSION_NUMBER "-rc"
#elif GUGU_EDITOR_VERSION_IS_POST_RELEASE
#define GUGU_EDITOR_VERSION GUGU_EDITOR_VERSION_NUMBER "-post"
#else
#define GUGU_EDITOR_VERSION GUGU_EDITOR_VERSION_NUMBER
#endif
