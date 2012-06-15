local env = ...

function env.require(name)
  local cached = package.loaded[name]

  if cached then
    return cached
  end

  local loaders = package.loaders
  local errors  = {}

  for i, v in ipairs(loaders) do
    local result = loaders[i](name)
    local t      = type(result)

    if t == 'function' then
      local childenv = setmetatable({}, { __index = env })
      setfenv(result, childenv)
      return result(name)
    elseif t == 'string' then
      errors[#errors + 1] = result
    end
  end

  error(table.concat(errors, '\n'), 2)
end

local require = env.require

function env.import(name)
  local result    = require(name)
  local callerenv = getfenv(2)

  for k, v in pairs(result) do
    callerenv[k] = v
  end

  return result
end
