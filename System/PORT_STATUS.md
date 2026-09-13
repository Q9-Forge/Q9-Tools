# Q9-Tools Portierungsstatus

## Grundsatz

Die Tools werden zunächst mit dem funktionierenden Microware-OS-9-Kernel
gebaut und getestet. Später müssen sie auf den neuen Q9-Kernel portiert
werden.

## Offene Systemfunktionen

- `mdir` und `devs`: Die Originale verwenden `F$CpyMem`, um Kernelstrukturen
  in den Prozessspeicher zu kopieren. Unsere sicheren Varianten zeigen daher
  bisher nur Metadaten.
- `del -e`: Das vollständige Löschen der belegten Disksektoren benötigt noch
  den originalen Low-Level-Aufruf; `unlinkx()` allein reicht nicht.
- `del -f`: Das Erzwingen des Löschens bei fehlenden Schreibrechten muss noch
  gegen das originale Attribut-/Löschverhalten geprüft werden.
- `touch`: Das Setzen des Änderungsdatums benötigt einen OS-9-Setstat-Aufruf;
  `stdio` stellt dafür keine ausreichende Funktion bereit.
- `-x` bei mehreren Tools: Die Option wird akzeptiert, die gemeinsame
  Execution-Directory-Auflösung wird noch zentral ergänzt.

## Gebaute Tools

`Qdate`, `Qdevs`, `Qdir`, `Qlink`, `Qload`, `Qmdir`, `Qmfree`, `Qprocs`,
`Qsleep`, `Qattr`, `Qmakdir`, `Qdel`, `Qlist`, `Qident` und `Qrename` wurden
mit dem Microware-Compiler gebaut und per `os9 ident` auf Header, Parität und
CRC geprüft.

Zusätzlich gebaut und geprüft: `Qwhat`, `Qcmp`, `Qtee`, `Qcount`, `Qtr`,
`Qgrep`, `Qunlink`, `Qhostname`, `Qpr` und `Qdeldir`.
Außerdem: `Qmv`, `Qchown` und `Qbuild`.
Zusätzlich als erste funktionsfähige Basis: `Qcopy`.
Neu ergänzt: `Qhelp`, `Qdump` und `Qfree`; alle drei wurden gebaut und per
`os9 ident` geprüft.
Zusätzlich gebaut und geprüft: `Qiniz` und `Qdeiniz`.
Zusätzlich gebaut und geprüft: `Qsetime`, `Qsave`, `Qpd` und `Qevents`.
Zusätzlich gebaut und geprüft: `Qtouch` und `Qpaths`.
Zusätzlich gebaut und geprüft: `Qfixmod`.
Zusätzlich gebaut und geprüft: `Qdcheck` als read-only RBF-Basis.
Zusätzlich gebaut und geprüft: `Qbinex` als S-Record-Konverter.
Zusätzlich gebaut und geprüft: `Qbreak` als Systemdebugger-Aufruf.

`Qprintenv` verwendet jetzt die Microware-Umgebungsvariablen-Schnittstelle
`_environ` und wurde erfolgreich gelinkt und mit `os9 ident` geprüft.

Neu ergänzt und geprüft: `Qecho` als fertiges Modul mit `-?`, `-n`, `-r`
und `-z`; außerdem `Qqsort` mit `-f`, `-c`, `-z` und `-?`.

## Nächster 20er-Block

Geprüft bzw. eingeordnet wurden: `build`, `chown`, `copy`, `deldir`, `free`,
`login`, `makdir`, `maps`, `mv`, `paths`, `pd`, `printenv`, `pr`, `save`,
`setime`, `touch`, `undel`, `events`, `help`, `hostname`, `grep` und `unlink`.

Die noch offenen Kandidaten sind absichtlich getrennt: `copy` benötigt die
vollständige Pfad-/Attributlogik, `free` und `maps` greifen auf Dateisystem-
bzw. Prozessdatenstrukturen zu, `login` und `setime` sind Systemverwaltung,
`touch` braucht Setstat, `undel` benötigt Dateisystem-Metadaten und `events`,
`paths`, `pd`, `save` und `help` benötigen jeweils eigene OS-9-Systemtabellen.

### Priorität des aktuellen 20er-Blocks

Direkt als C-Programme weiterbearbeiten: `expand`, `dump` (Dateimodus),
`build`-Erweiterungen und `help`.

Mit vorhandenen OS-9-C-Funktionen portieren: `pd`, `paths`, `setime`,
`touch`, `free` und `chown`-Erweiterungen.

Später mit Dateisystem-/Archivlogik: `copy`-Erweiterungen, `backup`,
`frestore`, `tar`, `dcheck`, `undel` und `fixmod`.

System-/Netzwerkabhängig und deshalb zurückgestellt: `login`, `maps`,
`events`, `mount` und `dird`.
