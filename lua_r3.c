#define LUA_LIB

#include <stdint.h>
#include <lua.h>
#include <lauxlib.h>
#include "r3/include/r3.h"

typedef struct _snode {
    char *s;
    struct _snode *next;
} snode;

static snode *
snode_create(const char *s, size_t len) {
    snode *node = (snode *)malloc(sizeof(snode));
    node->s = (char *)malloc(len + 1);
    strcpy(node->s, s);
    node->next = NULL;
    return node;
}

static void
snode_insert(snode **p_slink, snode *node) {
    node->next = *p_slink;
    *p_slink = node;
}

typedef struct _r3_tree {
    R3Node *r3node;
    snode *slink;
} r3_tree;

static int
lr3_create(lua_State *L) {
    int cap = luaL_checkinteger(L, 1);
    r3_tree *tree = (r3_tree *)lua_newuserdata(L, sizeof(r3_tree));
    tree->r3node = r3_tree_create(cap);
    tree->slink = NULL;
    return 1;
}

static int
lr3_free(lua_State *L) {
    r3_tree *tree = (r3_tree *)lua_touserdata(L, 1);
    if (tree == NULL) {
        return 0;
    }

    snode *head = tree->slink;
    while (head != NULL) {
        free(head->s);
        snode *tmp = head->next;
        free(head);
        head = tmp;
    }
    tree->slink = NULL;

    if (tree->r3node != NULL) {
        r3_tree_free(tree->r3node);
    }
    tree->r3node = NULL;
    return 0;
}

static int
lr3_insert(lua_State *L) {
    r3_tree *tree = (r3_tree *)lua_touserdata(L, 1);
    if (tree == NULL) {
        return luaL_error(L, "tree is null.");
    }

    int method = luaL_checkinteger(L, 2);
    size_t path_len = 0;
    const char *path = lua_tolstring(L, 3, &path_len);
    int idx = luaL_checkinteger(L, 4);
    void *data = (void *)(intptr_t)(idx);
    char *errstr = NULL;

    snode *n = snode_create(path, path_len);
    snode_insert(&tree->slink, n);
    R3Route *route = r3_tree_insert_routel_ex(tree->r3node, method, n->s, path_len, data, &errstr);
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
    r3_tree *tree = (r3_tree *)lua_touserdata(L, 1);
    if (tree == NULL) {
        return luaL_error(L, "tree is null.");
    }

    char *errstr = NULL;
    int err = r3_tree_compile(tree->r3node, &errstr);
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
    r3_tree *tree = (r3_tree *)lua_touserdata(L, 1);
    if (tree == NULL) {
        return luaL_error(L, "tree is null.");
    }
    r3_tree_dump(tree->r3node, 0);
    return 0;
}

static int
lr3_match_route(lua_State *L) {
    r3_tree *tree = (r3_tree *)lua_touserdata(L, 1);
    if (tree == NULL) {
        return luaL_error(L, "tree is null.");
    }

    const char *path = luaL_checkstring(L, 2);
    int method = luaL_checkinteger(L, 3);

    match_entry *entry = match_entry_create(path);
    entry->request_method = method;

    R3Route *matched_route = r3_tree_match_route(tree->r3node, entry);
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

