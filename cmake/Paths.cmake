# This file contains the installation paths used by the daemon. It is also used
# by plugins when configured to build using the installed headers.

set(Lotos2_INCLUDE_DIR "/usr/local/include" CACHE PATH "Lotos2 include directory")
#set(Lotos2_LOCALE_DIR "" CACHE PATH "Lotos2 locale directory")
#set(Lotos2_PLUGIN_DIR "" CACHE PATH "Lotos2 plugin directory")
set(Lotos2_SHARE_DIR "" CACHE PATH "Lotos2 share directory")

#mark_as_advanced(Lotos2_LOCALE_DIR)
#mark_as_advanced(Lotos2_PLUGIN_DIR)
mark_as_advanced(Lotos2_SHARE_DIR)
