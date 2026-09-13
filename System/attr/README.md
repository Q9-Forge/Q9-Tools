# Qattr

Microware-C-Nachbau des OS-9-Programms `attr`. Der Modulname bleibt `attr`,
die ausgelieferte Datei heißt `Qattr`.

Unterstützt werden die Anzeige und Änderung der OS-9-Dateiattribute über
`stat()` und `chmod()`, außerdem `-?`, `-a`, `-q` und die Dateilistenoptionen
`-z`/`-z=<path>`. Die Attributbuchstaben sind `d`, `s`, `pe`, `pw`, `pr`,
`e`, `w` und `r`; mit `-n` werden folgende Attribute abgeschaltet.

`-x` wird zunächst als kompatible Option akzeptiert. Die Auflösung über das
Execution Directory wird ergänzt, sobald die gemeinsame Q9-Argument- und
Pfadauflösung bereitsteht.
