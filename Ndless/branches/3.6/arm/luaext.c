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
 * Portions created by the Initial Developer are Copyright (C) 2012-2014
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s): 
 ****************************************************************************/

#include "ndless.h"
#include <lauxlib.h>

// array of modules's memory blocks
#define LUAEXT_MAX_MODULES 30
static void *loaded[LUAEXT_MAX_MODULES];
static unsigned loaded_next_index = 0;


static int require_file_recur_cb(const char *path, void *context) {
	if (strcmp(strrchr(path, '/') + 1, (char*)context) || ld_exec(path, loaded + loaded_next_index))
		return 0;
 	loaded_next_index++; // found and loaded
	return 1;
}

static int require(lua_State *L) {
	char modulepath[FILENAME_MAX];
	const char *name = luaL_checkstring(L, 1);
	if (strlen(name) >= 30) goto require_not_found;
	if (loaded_next_index >= LUAEXT_MAX_MODULES) {
		luaL_error(L, "cannot load module " LUA_QS ": too many modules loaded", name);
		return 1;
	}
	sprintf(modulepath, "%s.luax.tns", name);
	if (!ut_file_recur_each(get_documents_dir(), require_file_recur_cb, modulepath)) {
require_not_found:
		luaL_error(L, "modulee " LUA_QS " not found", name);
		return 1;
	}
  return 1;
}

static const luaL_reg baselib[] = {
	{"nrequire", require},
	{NULL, NULL}
};

// At the end of luaL_openlibs
// OS-specific
static unsigned const interp_startup_addrs[] = {0x10125F30, 0x10126558, 0x10125974, 0x10125FCC};

// At the beginning of lua_close
// OS-specific
static unsigned const interp_shutdown_addrs[] = {0x10825B10, 0x1080A35C, 0x108072FC, 0x10807DB8};

void lua_install_hooks(void) {
	HOOK_INSTALL(interp_startup_addrs[ut_os_version_index], lua_interp_startup);
	HOOK_INSTALL(interp_shutdown_addrs[ut_os_version_index], lua_interp_shutdown);
	nl_relocdata((unsigned*)baselib, (sizeof(baselib) / sizeof(unsigned*)) - 2);
}

static lua_State *luastate = NULL;

lua_State *luaext_getstate(void) {
	return luastate;
}

HOOK_DEFINE(lua_interp_startup) {
	// reg number: OS-specific
	luastate = (lua_State*)HOOK_SAVED_REGS(lua_interp_startup)[6];
	luaL_register(luastate, "_G", baselib);
	HOOK_RESTORE_RETURN(lua_interp_startup);
}

HOOK_DEFINE(lua_interp_shutdown) {
	unsigned i;
	for (i = 0; i < loaded_next_index; i++) {
		ld_free(loaded[i]);
	}
	luastate = NULL;
	HOOK_RESTORE_RETURN(lua_interp_shutdown);
}
