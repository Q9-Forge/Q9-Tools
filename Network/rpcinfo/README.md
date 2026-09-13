# Qrpcinfo

Listet die beim Sun-RPC-Portmapper registrierten Programme eines Hosts und
testet RPC-Programme über TCP oder UDP.

```text
Qrpcinfo -p [host]
Qrpcinfo -t host programm [version]
Qrpcinfo -u host programm [version]
Qrpcinfo -?
```

Ohne Argument wird `127.0.0.1` abgefragt. `-n` ist als historische Option
erkannt, aber der feste-Port-Aufruf ist in der aktuellen RPC-Bibliotheksbasis
noch nicht angeschlossen.
