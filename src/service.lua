local _M = {}

-- XXX Arch Linux-specific
-- XXX not everyone uses sudo

function _M.service(name)
  local service = setmetatable({}, { __call = function(self, command, ...)
    local method = self[command]

    return method(self, ...)
  end
  })

  function service:start()
    os.execute {
      'sudo',
      '/etc/rc.d/' .. name,
      'start'
    }
  end

  function service:stop()
    os.execute {
      'sudo',
      '/etc/rc.d/' .. name,
      'stop'
    }
  end

  return service
end

return _M
