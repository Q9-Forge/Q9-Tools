# Qmv

Microware-C-Nachbau von `mv` für eine Quelle und ein Ziel. Zunächst wird
`rename()` verwendet. Wenn das nicht möglich ist, kopiert Qmv die Datei,
vergleicht den vollständigen Inhalt und löscht die Quelle erst nach
erfolgreicher Prüfung. Damit ist auch der Cross-Device-Fall vorbereitet.

Die historische `-z`-Listenverarbeitung und die vollständige
Execution-Directory-Semantik von `-x` sind noch offen.
