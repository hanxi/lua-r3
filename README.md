# lua-r3

[libr3](https://github.com/c9s/r3) for lua.

### TODO

- `r3.lua` for route

### Build

#### Dependent library

```
# Ubuntu
sudo apt-get install check libpcre3 libpcre3-dev build-essential libtool \
    automake autoconf pkg-config
# CentOS 7
sodu yum install gcc gcc-c++ git make automake autoconf pcre pcre-devel \
    libtool pkgconfig
```

See [Makefile](https://github.com/hanxi/lua-r3/blob/main/Makefile)

Just run `make` commond. Will generate `r3.so` file for lua dynamic lib.

### How to use

```lua
local r3_core = require "r3.core"

-- create tree
local tree = r3_core.create(10)

-- insert route
local METHOD_GET = 2
local node = r3_core.insert(tree, METHOD_GET, "/foo/{bar}", 1)

-- compile tree
local ok,msg = r3_core.compile(tree)
-- dump tree for debug
r3_core.dump(tree)

-- match route
local idx, slugs, tokens = r3_core.match_route(tree, "/foo/foobar", METHOD_GET)

r3_core.free(tree)
```

See [test_core.lua](https://github.com/hanxi/lua-r3/blob/main/test_core.lua)

### Thanks

- [iresty/lua-resty-libr3](https://github.com/iresty/lua-resty-libr3)
- [c9s/r3](https://github.com/c9s/r3)

