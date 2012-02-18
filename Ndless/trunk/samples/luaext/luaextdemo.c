#include <os.h>
#include <lauxlib.h>

static int hello(lua_State *L) {
	const char *param = luaL_checkstring(L, 1);
	printf("hello %s!\n", param);
	return 0;
}

static const luaL_reg lualib[] = {
	{"hello", hello},
	{NULL, NULL}
};

int main(void) {
	lua_State *L = nl_lua_getstate();
	if (!L) return 0; // not being called as Lua module
	nl_relocdata((unsigned*)lualib, (sizeof(lualib) / sizeof(unsigned*)) - 2);
	luaL_register(L, "luaextdemo", lualib);
	return 0;
}
