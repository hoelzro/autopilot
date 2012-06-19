#ifndef AP_LUA_H
#define AP_LUA_H

#include <lua.h>

/** \defgroup Lua Lua Interface
 *  @{
 */

/** \brief Gets the Lua interpreter state associated with this autopilot
 *         instance.
 *
 */

lua_State *autopilot_get_lua(autopilot_context *ap);

/** @} */

#endif
