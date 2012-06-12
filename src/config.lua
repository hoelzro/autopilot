load_module 'network-source-wicd'

on('network/connected', function(network)
  print(network.address)
end)
