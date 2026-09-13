# Qdel

Microware-C-Nachbau des OS-9-Programms `del`. Der Modulname bleibt `del`,
die ausgelieferte Datei heißt `Qdel`.

Unterstützt werden `-?`, `-f`, `-p`, `-q`, `-x`, `-z` und `-z=<path>`. Das
Löschen erfolgt über die native Microware-Funktion `unlinkx()`.
