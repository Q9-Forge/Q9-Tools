# qid

`ident`s größerer Bruder -- kein Ersatz, `ident` bleibt unverändert.

Erkennt und beschreibt:
- **alle** OS-9-Modultypen mit ihrem jeweils korrekten Header-Extension-Layout
  (`ident` behandelt jeden Typ wie `mod_exec` -- für Drvr/Desc/Systm/FMan-Module
  ist das schlicht falsch, siehe `module.h`)
- **ROF** (Relocatable Object File, was `r68` vor dem Linken durch `l68`
  schreibt) -- Sync `$DEADFACE`, Header + Modulname
- **ELF** (32/64-Bit, beide Endianness) inkl. Section- und Program-Header-Tabelle

Modul- und ROF/ELF-Parsing sowie alle drei Fundorte (Platte, Ausführungs-
verzeichnis, Speicher) sind gegen den echten Q9-Flux-Emulator getestet
(`test/expect/test_qid.exp` im Q9-Flux-Repo). ROF/ELF-Erkennung ist nach
Spezifikation implementiert, aber mangels Testdatei im Image noch nicht
live gegengeprüft.

## Fundorte

| Option | Durchsucht |
|--------|-----------|
| (keine) | aktuelles Datenverzeichnis, Wildcard-fähig (`*`, `?`) |
| `-x` | aktuelles Ausführungsverzeichnis (über das `FAM_EXEC`-Zugriffsbit -- derselbe Mechanismus, den `F$Link`/`F$Load` für relative Pfadnamen benutzen) |
| `-m` | Module, die aktuell im Speicher resident sind (Modul-Directory) |
| `-a` | alle drei Fundorte zusammen |

**Wichtig:** Die OS-9-Shell expandiert `*`/`?` selbst, bevor `qid` sie zu
sehen bekommt (kein eigenes Globbing im Programm nötig, aber `argv` kann bei
einem gut gefüllten Verzeichnis schnell 100+ Einträge haben -- der
Pattern-Puffer ist deshalb auf 512 ausgelegt). `qid`s eigener `wildmatch()`
greift trotzdem noch dort, wo die Shell ein nicht auflösbares Muster
unexpandiert durchreicht.

## Zwei reale Stolpersteine (beim Bauen gegen den Emulator gefunden, nicht nur vermutet)

1. **Verzeichniseinträge sind High-Bit-terminiert, nicht NUL-terminiert.**
   `_os_read()` auf einen mit `_os_open(".", FAM_READ|FAM_DIR[|FAM_EXEC])`
   geöffneten Pfad liefert rohe `struct dirent`-Records (`direct.h`) --
   aber der Name darin endet wie ein Modulname: letztes Zeichen mit
   gesetztem High-Bit, kein NUL. Naive NUL-Suche liefert einen um ein
   Zeichen abgeschnittenen, unbrauchbaren Namen für praktisch jeden Eintrag.
   Kuriosität am Rande: Ultra-C-kompilierte Module (z.B. `qid` selbst)
   legen ihren *Modulnamen* dagegen NUL-terminiert ab -- `qid_read_os9_name()`
   toleriert deshalb beide Varianten.
2. **`_os_read()` auf einem Verzeichnis liefert nur ganze `DIRBLKSIZ`-Blöcke
   (256 Byte = 8 Records), keine einzelnen 32-Byte-Records.** Ein Read mit
   `n=32` läuft zwar fehlerfrei durch, liefert aber nichts Verwertbares
   (bzw. wurde hier nie erreicht, weil das Programm gleich blockweise liest).

## -m: warum ein eigener Assembler-Stub nötig ist

Das Technical Reference Manual ist bei `mdir` explizit: *"you should never
directly access the modules... use F$CpyMem to copy portions of the
system's address space for inspection. On some systems, directly accessing
the modules may cause address or bus trap errors."* `qid` dereferenziert
`md_mptr` deshalb nie direkt, sondern kopiert jeden Header-Ausschnitt per
`F$CpyMem` ($1B) in einen eigenen Puffer.

Für `F$CpyMem` existiert kein C-Wrapper in `module.h`/`modes.h` (die
Register-Rollen -- a0/a1 als Quell-/Zielzeiger -- passen zu keinem der
`_os_xxx()`-Prototypen). `qid_cpymem.a` ist ein von Hand geschriebener
Trap-Aufruf gemäß der im Ultra C/C++ Processor Guide (Kapitel 1: 68K)
dokumentierten Aufrufkonvention.

`owner_pid=0` ("process ID of external memory's owner") ist gegen den
laufenden Emulator getestet und funktioniert -- Modul-Directory-Einträge
sind system-globales Eigentum, 0 adressiert offenbar "das System". Läuft
aktuell noch der originale OS-9-Kern im Emulator (noch nicht Q9s eigener),
das ist also gegen echtes OS-9 verifiziert, nicht gegen Q9.

## Stack-Größe

Der Linker-Default (3072 Byte) reicht für `qid`s Verzeichnis-Scan nicht --
gegen den Emulator getestet mit `**** Stack Overflow ****` bei einem
Verzeichnis mit ~200 Einträgen. `xcc`s eigene Linker-Optionsweitergabe
kennt `l68`s `-M` (Stack-Speicher) nicht (`invalid phase for stack space
option`; die dokumentierten Linker Options sind nur `-a`/`-i`/`-j`/`-t=`).
Fund aus *Using Ultra C/C++*, Kapitel 5 ("Compiler Phase Options"): Optionen
lassen sich über ein Phasenkürzel ohne Bindestrich weiterreichen -- `ol`
= Object Code Linker, also `-olM=4k` (siehe Makefile). Damit: 3072+4096 =
7168 Byte, im Emulator ohne Overflow verifiziert.

## Ausgabeformate

- (Standard) ein ausführlicher Block pro Fund, wie bei `ident`
- `-q` eine knappe Zeile pro Fund (wie `ident -q`)
- `-t` eine Tabelle, eine Zeile pro Fund
