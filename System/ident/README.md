# Qident

Microware-C-Nachbau des OS-9-Programms `ident`. Der Modulname bleibt `ident`,
die ausgelieferte Datei heißt `Qident`.

Liest den OS-9-Modulheader aus Dateien und zeigt Name, Größe, Edition,
Typ/Sprache, Zugriff, Ausführungsoffset, Datengröße und Stackgröße an.

## Formaterkennung

`ident` erkennt zusätzlich ELF, Microware-ROF und das libgen-Syncwort
`2D00D5BC`. Für libgen wird derzeit nur das Format identifiziert; Index,
Version, Bibliotheksname und Modulanzahl werden noch nicht dekodiert. Eine
Datei mit mehreren eingebetteten ROF-Syncwörtern wird als mögliche
zusammengefügte ROF-Bibliothek markiert. Das ist ausdrücklich ein Hinweis,
keine sichere Archivvalidierung, weil ein Syncwort auch in Nutzdaten
vorkommen kann.

Die klassischen OS-9/6809-Module haben ein eigenes 9-Byte-Kopfformat mit
Syncwort `$87CD`; `ident` erkennt und prüft dessen Header-Checkbyte. Das
68000-/OS-9000-Kopfformat verwendet `$4AFC` (auf Little-Endian-Systemen als
Bytes `FC 4A`) und hat eine andere, längere Struktur. Diese Syncwörter
unterscheiden die Headerfamilien; die jeweilige Sprache im 6809-Kopf sagt
zusätzlich, ob der Inhalt 6809-Objektcode oder etwa I-Code ist. `ident`
dekodiert noch nicht alle typabhängigen Header-Erweiterungen. Für
detailliertere ELF-/ROF- und Modulberichte bleibt `qid` zuständig.

Die Erkennung ist von der geplanten libgen-Unterstützung in `ql68` getrennt.
