local env = ...

local require = env.require

function env.import(name)
  local result    = require(name)
  local callerenv = getfenv(2)

  for k, v in pairs(result) do
    callerenv[k] = v
  end

  return result
end
