# Autopilot

Autopilot is a daemon that you can run on your computer that runs actions to
respond to certain events.  This is a vague description, but the core functionality
of the daemon itself is pretty plain.  All of the actions that you use to respond to events
are scripted using the Lua scripting language, and all the events that you can respond to are
provided by plugins.  The daemon itself is really just a Lua interpreter strapped onto an event
loop and a plugin loader.

# An Example

I have two laptops: a personal one and a work one.  Often, when I come home from work, I want to sync
up some files.  So I need to start ``sshd`` on my home laptop so I can login from my work one.  However,
I often leave my apartment to go to cafés and do work on my laptop, and I certainly don't want to be
running ``sshd`` there!  So I can put the following in my autopilot RC file (``$HOME/.config/autopilot/config.lua``):

```lua

load_module 'network-source-wicd'

local function athome(network)
  return network.is_wireless and network.ssid == 'MY_SSID'
end

import 'services'
on('network/connect', function(network)
  if athome(network) then
    service 'sshd' 'start'
  else
    service 'sshd' 'stop'
  end
end)

```
