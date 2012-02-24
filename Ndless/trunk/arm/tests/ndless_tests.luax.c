/****************************************************************************
 * Automated test cases for Lua extension API
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

#include <os.h>
#include "ndless_tests.h"

static int run(lua_State *L) {
	puts("hello");
	return 0;
}

static const luaL_reg lualib[] = {
	{"run", run},
	{NULL, NULL}
};

int main(void) {
	lua_State *L = nl_lua_getstate();
	if (!L) return 0; // not being called as Lua module
	nl_relocdata((unsigned*)lualib, (sizeof(lualib) / sizeof(unsigned*)) - 2);
	luaL_register(L, "tests", lualib);
	return 0;
}
