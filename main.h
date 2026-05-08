#ifndef MAIN_H
#define MAIN_H

#define APP_VERSION_MAJOR 1
#define APP_VERSION_MINOR 0
#define APP_VERSION_PATCH 0

#define _STR(x) #x
#define STR(x) _STR(x)

#define APP_VERSION_STRING STR(APP_VERSION_MAJOR) "." STR(APP_VERSION_MINOR) "." STR(APP_VERSION_PATCH)

#endif /* MAIN_H */
