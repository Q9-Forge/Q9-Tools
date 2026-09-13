# Qdeldir

Microware-C-Nachbau des OS-9-Programms `deldir`. Der Modulname bleibt
`deldir`, die ausgelieferte Datei heißt `Qdeldir`.

Verwendet `_os_delete(..., FAM_DIR)` für die native Verzeichnislöschung und
unterstützt `-x`, `-e`, `-z`, `-z=<path>` und `-?`. Rekursives Löschen und das
physische Löschen der belegten Sektoren bleiben bis zur Low-Level-Analyse
der Originalversion offen.
