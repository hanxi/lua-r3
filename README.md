# lua-r3

[libr3](https://github.com/c9s/r3) for lua.

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

Use `r3.lua` and `r3.so` in your `lua_path` and `lua_cpath` .

```lua
local r3 = require "r3"

-- create route
local route = r3:new()

-- insert route
route:insert("GET", "/foo/{bar}", "t1")

-- compile route
route:compile()

-- dump route tree
route:dump()

-- match route
local data, params = route:match("/foo/foobar", "GET")

```

See [test.lua](https://github.com/hanxi/lua-r3/blob/main/test.lua)

### Thanks

- [iresty/lua-resty-libr3](https://github.com/iresty/lua-resty-libr3)
- [c9s/r3](https://github.com/c9s/r3)

