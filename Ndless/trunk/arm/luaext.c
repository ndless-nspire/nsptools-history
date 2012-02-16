/****************************************************************************
 * Lua extensions support
 *
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is Ndless code.
 *
 * The Initial Developer of the Original Code is Olivier ARMAND
 * <olivier.calc@gmail.com>.
 * Portions created by the Initial Developer are Copyright (C) 2012
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s): 
 ****************************************************************************/

#include "ndless.h"
#include <lauxlib.h>

// Demo
static int myfunc(lua_State *L) {
  puts("myfunc");
  return 0;
}

static const luaL_reg ndlesslib[] = {
	{"myfunc", myfunc},
	{NULL, NULL}
};

// At the end of luaL_openlibs
// OS-specific
static unsigned const interp_startup_addrs[] = {0x0, 0x0, 0x0, 0x1010052C};

// At the beginning of lua_close
// OS-specific
static unsigned const interp_shutdown_addrs[] = {0x0, 0x0, 0x0, 0x106B2C38};

void lua_install_hooks(void) {
	HOOK_INSTALL(interp_startup_addrs[ut_os_version_index], lua_interp_startup);
	HOOK_INSTALL(interp_shutdown_addrs[ut_os_version_index], lua_interp_shutdown);
}

static lua_State *luastate = NULL;

HOOK_DEFINE(lua_interp_startup) {
	puts("startup");
	luastate = (lua_State*)HOOK_SAVED_REGS(lua_interp_startup)[4];
	
	// Demo
	nl_relocdata((unsigned*)ndlesslib, (sizeof(ndlesslib) / sizeof(unsigned*)) - 2);
	luaL_register(luastate, "ndless", ndlesslib);
	
	HOOK_RESTORE_RETURN(lua_interp_startup);
}

HOOK_DEFINE(lua_interp_shutdown) {
	puts("shutdown");
	luastate = NULL;
	HOOK_RESTORE_RETURN(lua_interp_shutdown);
}
