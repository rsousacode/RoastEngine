#parse("C File Header.h")
// some stuff
#if (${HEADER_FILENAME})
#[[#include]]# "${HEADER_FILENAME}"
#end
