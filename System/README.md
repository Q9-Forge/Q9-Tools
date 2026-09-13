# Q9 System Tools

Die Systemtools werden mit dem Microware-OS-9/68000-Compiler gebaut und
zunächst gegen den vorhandenen OS-9-Kernel getestet. Die Moduldateien tragen
im Dateinamen das Präfix `Q`; der interne OS-9-Modulname bleibt kompatibel.

## Aktueller Stand

Die folgenden Werkzeuge besitzen eine Q9-C-Implementierung und wurden als
OS-9-Modul gebaut und mit `os9 ident` geprüft:

`Qattr`, `Qbinex`, `Qbreak`, `Qbuild`, `Qchown`, `Qcmp`, `Qcopy`, `Qcount`,
`Qdate`, `Qdcheck`, `Qdeiniz`, `Qdel`, `Qdeldir`, `Qdir`, `Qdump`, `Qecho`,
`Qevents`, `Qfixmod`, `Qfree`, `Qgrep`, `Qhelp`, `Qhostname`, `Qident`,
`Qiniz`, `Qlink`, `Qlist`, `Qload`, `Qmakdir`, `Qmdir`, `Qmfree`, `Qmv`,
`Qpaths`, `Qpd`, `Qpr`, `Qprintenv`, `Qprocs`, `Qqsort`, `Qrename`, `Qsave`,
`Qsetime`, `Qsleep`, `Qtee`, `Qtouch`, `Qtr`, `Qunlink` und `Qwhat`.

Jedes dieser Programme besitzt `-?`. Historische Optionen, deren OS-9-
Semantik noch nicht sicher reproduziert ist, werden in der Hilfe ausdrücklich
als `not implemented` gekennzeichnet.

## Bewusst zurückgestellt

- Dateisystemnahe Funktionen wie `free -b`, `copy -c/-f/-w`, `dump -a/-k`
  und `mv -x` benötigen noch originale SetStat-/Pfadsemantik.
- `login`, `maps`, `mount`, `undel`, `backup`, `tar` und `frestore` greifen
  auf Systemverwaltung oder Dateisystem-Metadaten zu.
- Für den neuen Q9-Kernel sind die Tools erst nach dessen Fertigstellung zu
  portieren; bis dahin bleibt der funktionierende Microware-Kernel die
  Referenzumgebung.

## Bauen

Die lokalen Makefiles verwenden die Microware-Toolchain. Nach einem Build wird
das Modul mit `os9 ident` auf Header, CRC und Header-Parity geprüft. Die
vollständige Optionsmatrix steht in
[`../Network/OPTIONS_AUDIT.md`](../Network/OPTIONS_AUDIT.md).

Der detailliertere Arbeitsstand und bekannte Kernelgrenzen stehen in
[`PORT_STATUS.md`](PORT_STATUS.md).
