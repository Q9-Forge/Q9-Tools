# Nächster 20er-Block – Analyse und Reihenfolge

Die drei direkt portierbaren Kandidaten dieses Durchlaufs sind gebaut:
`Qevents`, `Qtouch` und `Qpaths`. Alle drei wurden mit dem Microware-
Compiler gelinkt und mit `os9 ident` auf CRC und Header-Parität geprüft.

Die folgenden Originalmodule sind als nächste Arbeiten eingeordnet:

| Tool | Einordnung | Nächster Schritt |
| --- | --- | --- |
| `expand` | eigenes Kompressionsformat | Format und Bitstream aus Original analysieren |
| `fixmod` | Modulheader/CRC | Header-Parser und kontrolliertes Neuschreiben |
| `dcheck` | RBF-Dateisystem | read-only Bitmap- und Segmentprüfung |
| `backup` | Rohsektor-Kopie | sichere RBF-Geräteprüfung, danach Kopierlauf |
| `fsave` | Dateisystem-Abbild | Format und Metadaten dokumentieren |
| `frestore` | Dateisystem-Wiederherstellung | Gegenstück zu `fsave` |
| `tar` | Archivformat | vorhandene Archivstruktur wiederverwenden |
| `format` | destruktiv | erst Emulator-Test und Schutzabfragen |
| `mount` | Geräte-/Netzwerkverwaltung | lokale RBF-Variante abtrennen |
| `undel` | RBF-Metadaten | Directory und freie Segmente rekonstruieren |
| `maps` | Kernel-/Prozessdaten | Kernel-DB-Schnittstelle ermitteln |
| `irqs` | Kernel-/Interruptdaten | privilegierten Aufruf identifizieren |
| `login` | Prozess-/Benutzerverwaltung | Authentifizierungsmodell dokumentieren |
| `dird` | Directory-Service | OS-9-Directory-Struktur prüfen |
| `dsave` | Disk-Image | Formatvergleich mit `backup` |
| `diskcache` | Gerätestatus | SetStat-Codes und Gerätetypen bestimmen |
| `moded` | Geräteparameter | nur nach getstat/setstat-Matrix |
| `pcformat` | PC-kompatibles Medium | getrennt von RBF-`format` behandeln |
| `compress` | Kompressionswerkzeug | Verhältnis zu `expand` klären |
| `editmod` | Modul-Editor | erst gemeinsame Modul-Header-Library bauen |

`Qevents`, `Qtouch` und `Qpaths` bleiben bewusst als erste funktionsfähige
Basen dokumentiert; die vollständige Originalausgabe und alle historischen
Sonderoptionen folgen nach der jeweiligen Systemanalyse.
