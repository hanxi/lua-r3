#define LUA_LIB

#include <stdint.h>
#include <lua.h>
#include <lauxlib.h>
#include "r3/include/r3.h"
#include "r3/include/r3_gvc.h"

size_t
r3_match_entry_fetch_slugs(void *entry, size_t idx, char *val,
                                size_t *val_len)
{
    match_entry             *m_entry = entry;
    int                      i;

    if (val ==  NULL) {
        return m_entry->vars.slugs.size;
    }

    if (idx >= m_entry->vars.slugs.size) {
        return -1;
    }

    i = m_entry->vars.slugs.entries[idx].len;
    *val_len = i;

    sprintf(val, "%*.*s", i, i, m_entry->vars.slugs.entries[idx].base);
    return m_entry->vars.slugs.size;
}


size_t
r3_match_entry_fetch_tokens(void *entry, size_t idx, char *val,
                                 size_t *val_len)
{
    match_entry             *m_entry = entry;
    int                      i_len;

    if (val ==  NULL) {
        return m_entry->vars.tokens.size;
    }

    if (idx >= m_entry->vars.tokens.size) {
        return -1;
    }

    i_len = m_entry->vars.tokens.entries[idx].len;
    *val_len = i_len;

    sprintf(val, "%*.*s", i_len, i_len, m_entry->vars.tokens.entries[idx].base);
    return m_entry->vars.tokens.size;
}


void
r3_match_entry_free(void *entry)
{
    match_entry    *r3_entry = (match_entry *)entry;

    if (entry == NULL) {
        return;
    }

    match_entry_free(r3_entry);
    return;
}

static int
lr3_create(lua_State *L) {
    int cap = luaL_checkinteger(L,1);
    R3Node *tree = r3_tree_create(cap);
    lua_pushlightuserdata(L, tree);
    return 1;
}

static int
lr3_free(lua_State *L) {
    R3Node *tree = (R3Node *)lua_touserdata(L,1);
    if (tree == NULL) {
        return 0;
    }
    r3_tree_free(tree);
    return 0;
}

static int
lr3_insert(lua_State *L) {
    R3Node *tree = (R3Node *)lua_touserdata(L,1);
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
    R3Node *tree = (R3Node *)lua_touserdata(L,1);
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

    r3_tree_dump(tree, 0);

    r3_tree_render_file(tree, "png", "check_gvc.png");

    return 1;
}

static int
lr3_match_route(lua_State *L) {
    R3Node *tree = (R3Node *)lua_touserdata(L,1);
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

    match_entry_free(entry);
    return 1;
}

LUAMOD_API int
luaopen_r3_core(lua_State *L) {
    luaL_checkversion(L);
    luaL_Reg l[] = {
        { "r3_create", lr3_create },
        { "r3_free", lr3_free },
        { "r3_insert", lr3_insert },
        { "r3_compile", lr3_compile },
        { "r3_match_route", lr3_match_route },
        { NULL,  NULL },
    };

    luaL_newlib(L,l);

    return 1;
}

