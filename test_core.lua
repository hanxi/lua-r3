local r3_core = require "r3.core"

function dump(o)
    if type(o) == 'table' then
        local s = '{ '
        for k,v in pairs(o) do
            if type(k) ~= 'number' then k = '"'..k..'"' end
            s = s .. '['..k..'] = ' .. dump(v) .. ', '
        end
        return s .. '} '
    else
        return string.format("%q", o)
    end
end

local METHOD_GET = 2
local tree = r3_core.create(10)

local node1 = r3_core.insert(tree, METHOD_GET, "/1/{id}", 1)
local node2 = r3_core.insert(tree, METHOD_GET, "/2", 2)
local node3 = r3_core.insert(tree, METHOD_GET, "/3/{name}/{fuck}/{}/{}", 3)

local ok,msg = r3_core.compile(tree)
assert(ok, msg)

print("---dump tree---")
r3_core.dump(tree)
print("---------------")

local function test_match(path, ret)
    print("--- try match", path)
    local idx, slugs, tokens = r3_core.match_route(tree, path, METHOD_GET)
    print("match result. idx:", idx)
    print("slugs:", dump(slugs))
    print("tokens:", dump(tokens))
    assert(idx == ret, "match failed")
    print("match", path, "ok")
    print("--- end match", path, "\n")
end

test_match("/1", nil)
test_match("/1/hanxi", 1)
test_match("/2", 2)
test_match("/3/n/f/k1/k2", 3)

r3_core.free(tree)

print("test core ok")
