# Qunlink

Microware-C-Nachbau des OS-9-Programms `unlink`. Der Modulname bleibt
`unlink`, die ausgelieferte Datei heißt `Qunlink`.

Verwendet `_os_link()` und `_os_unlink()` und unterstützt `-f`, `-z`,
`-z=<path>` und `-?`.
