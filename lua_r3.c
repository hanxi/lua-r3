#define LUA_LIB

#include <stdint.h>
#include <lua.h>
#include <lauxlib.h>
#include "r3/include/r3.h"

static int
lr3_create(lua_State *L) {
    int cap = luaL_checkinteger(L, 1);
    R3Node *tree = r3_tree_create(cap);
    lua_pushlightuserdata(L, tree);
    return 1;
}

static int
lr3_free(lua_State *L) {
    R3Node *tree = (R3Node *)lua_touserdata(L, 1);
    if (tree == NULL) {
        return 0;
    }
    r3_tree_free(tree);
    return 0;
}

static int
lr3_insert(lua_State *L) {
    R3Node *tree = (R3Node *)lua_touserdata(L, 1);
    if (tree == NULL) {
        return luaL_error(L, "tree is null.");
    }

    int method = luaL_checkinteger(L, 2);
    size_t path_len = 0;
    const char *path = lua_tolstring(L, 3, &path_len);
    int idx = luaL_checkinteger(L, 4);
    void *data = (void *)(intptr_t)(idx);
    char *errstr = NULL;

    R3Route *route = r3_tree_insert_routel_ex(tree, method, path, path_len, data, &errstr);
    if (route == NULL) {
        lua_pushnil(L);
        lua_pushstring(L, errstr);
        free(errstr);
        return 2;
    }

    lua_pushlightuserdata(L, route);
    return 1;
}

static int
lr3_compile(lua_State *L) {
    R3Node *tree = (R3Node *)lua_touserdata(L, 1);
    if (tree == NULL) {
        return luaL_error(L, "tree is null.");
    }

    char *errstr = NULL;
    int err = r3_tree_compile(tree, &errstr);
    if (err != 0) {
        lua_pushnil(L);
        lua_pushstring(L, errstr);
        free(errstr);
        return 2;
    }

    lua_pushboolean(L, true);
    return 1;
}

static int
lr3_dump(lua_State *L) {
    R3Node *tree = (R3Node *)lua_touserdata(L, 1);
    if (tree == NULL) {
        return luaL_error(L, "tree is null.");
    }
    r3_tree_dump(tree, 0);
    return 0;
}

static int
lr3_match_route(lua_State *L) {
    R3Node *tree = (R3Node *)lua_touserdata(L, 1);
    if (tree == NULL) {
        return luaL_error(L, "tree is null.");
    }

    const char *path = luaL_checkstring(L, 2);
    int method = luaL_checkinteger(L, 3);

    match_entry *entry = match_entry_create(path);
    entry->request_method = method;

    R3Route *matched_route = r3_tree_match_route(tree, entry);
    if (matched_route == NULL) {
        match_entry_free(entry);
        lua_pushnil(L);
        return 1;
    }

    int idx = (int)( (intptr_t) matched_route->data );
    lua_pushinteger(L, idx);

    int i;
    size_t size = entry->vars.slugs.size;
    lua_createtable(L, size, 0);
    for (i = 0; i < size; i++) {
        lua_pushlstring(L, entry->vars.slugs.entries[i].base, entry->vars.slugs.entries[i].len);
        lua_rawseti(L, -2, i+1);
    }

    size = entry->vars.tokens.size;
    lua_createtable(L, size, 0);
    for (i = 0; i < size; i++) {
        lua_pushlstring(L, entry->vars.tokens.entries[i].base, entry->vars.tokens.entries[i].len);
        lua_rawseti(L, -2, i+1);
    }

    match_entry_free(entry);
    return 3;
}

LUAMOD_API int
luaopen_r3_core(lua_State *L) {
    luaL_checkversion(L);
    luaL_Reg l[] = {
        { "create", lr3_create },
        { "free", lr3_free },
        { "insert", lr3_insert },
        { "compile", lr3_compile },
        { "dump", lr3_dump },
        { "match_route", lr3_match_route },
        { NULL,  NULL },
    };

    luaL_newlib(L, l);
    return 1;
}

