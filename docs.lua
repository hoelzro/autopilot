module 'autopilot'

--- Adds a callback for responding to a signal.  Currently,
--- the only signals defined are network/connected and network/disconnected.
--- The callback is passed any arguments that the signal originator passed along;
--- in the case of network/connected, it's a network object.
function on(signal, callback)
end

--- Loads a plugin.
function load_module(module_name)
end

--- Behaves the exact same as os.execute from stock Lua, except when a table of
--- arguments is passed, a fork/exec is performed, and the arguments in the table
--- are passed as the arguments to exec.
function os.execute(args)
end

--- Behaves as require does in stock Lua, except each module is loaded in its own
--- namespace, and the namespace is returned on success.
function require(modname)
end

--- Behaves just like require, except the symbols from the require'd namespace are
--- imported into that of the caller.
function import(modname)
end
