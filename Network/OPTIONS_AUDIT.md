# Options-Audit

Stand: 2026-08-10. Die historischen Optionen wurden aus den vorhandenen
68000-OS-9-Binaries und den lokalen Q9-Quellen verglichen.

## Bereits nachgezogen

- `Qrpcinfo`: `-p`, `-t`, `-u`, `-?`; `-n` wird erkannt, ist aber noch nicht
  an einen festen RPC-Port-Aufruf angeschlossen.
- `Qroute`: `-n`, `-?`; die Abfrage bleibt read-only.
- Die meisten Systemtools besitzen bereits die gemeinsame `-?`-Hilfe sowie
  ihre jeweils implementierten Optionen.

## Noch offen oder bewusst eingeschränkt

- `Qnetstat`: Originaloptionen `-s -a -i -p -n -I -d -A -f -w` benötigen
  zusätzliche Socket-/Statistik-APIs. `-r` und `-n` sind umgesetzt; die
  übrigen Varianten bleiben offen.
- `Qifconfig`: Das Original kann Interfaces konfigurieren. Qifconfig ist
  bewusst read-only, weil SetStat/Ioctl-Codes und Rechte noch nicht geprüft
  sind.
- `Qtelnet`: `-n` ist umgesetzt; `-o` und `-d` werden erkannt, aber wegen
  fehlender Telnet-Option-Aushandlung bzw. Socket-Debug-Ausgabe abgewiesen.
- `Qarp`: Originaloptionen `-a -d -s -n`; die öffentliche SPF-BSD-Headerbasis
  exportiert keinen stabilen ARP-Tabellen-Iterator. Erst Treiber-/GetStat-
  Schnittstelle klären.
- `Qrup` und `Qrusers`: RPC-XDR-Strukturen und Versionsvarianten müssen aus
  den Originalmodulen rekonstruiert werden.
- `Qftp`: FTP-Kommandos und Login-/Datenkanal sind noch nicht portiert.

## System-Tools: bekannte Abweichungen

- `Qfree`: `-b` wird erkannt, die historische Puffergrößenwirkung ist noch
  nicht umgesetzt.
- `Qcmp`: `-t`, `-s` und `-b[=]<size>` sind umgesetzt; `-x` fehlt noch.
- `Qcopy`: `-v`, `-p`, `-r`, `-n`, `-a`, `-b[=]<size>` und `-z[=]path` sind
  umgesetzt bzw. akzeptiert. `-z` liest nun Quellenlisten und kopiert in ein
  Zielverzeichnis; `-a` bricht bei Fehlern sofort ab. Die historischen
  Varianten `-c`, `-f`, `-w` und `-x` benötigen noch die OS-9-Pfad-/
  Attributsemantik.
- `Qdump`: `-c`, `-s`, `-m` und `-x` sind vorhanden; `-c` unterdrückt jetzt
  doppelte Zeilen, `-s` rechnet den Offset in Sektoren um, `-m` liest ein
  resident geladenes Modul und `-x` öffnet im Execution Directory.
  Adressbereich (`-a`) und Shift-JIS (`-k`) bleiben offen.
- `Qtr`: `-d`, `-c`, `-v`, `-s` und `-z[=]path` sind vorhanden; die
  Listeneinträge werden nacheinander verarbeitet.
- `Qcount`: `-b` erzeugt jetzt die kombinierte Aufschlüsselung für Zeilen,
  Wörter und Zeichen.
- `Qgrep`: `-s` unterdrückt jetzt die Treffer-Ausgabe, `-m=<num>` gibt
  zusätzliche Zeilen nach einem Treffer aus, und `-e=<expr>`/`-f=<file>`
  liefern zusätzliche Suchmuster.
- `Qsave`: `-r`, `-x` und `-z` sind vorhanden. `-x` erzeugt nun über
  `_os_create(..., FAM_EXEC, ...)` im Execution Directory; die vollständige
  Mehrfachmodul-Semantik von `-f` bleibt noch offen.
- `Qmv`: `-z[=]path` verarbeitet jetzt eine Quellenliste in ein
  Zielverzeichnis; `-x` bleibt offen. Bei `rename()`-Fehlern wird mit
  Byteprüfung kopiert und erst danach die Quelle gelöscht.

Bei diesen Punkten ist die fehlende Funktion jeweils dokumentiert; sie wird
nicht stillschweigend als kompatibel bezeichnet.

Diese Punkte bleiben als konkrete Arbeitsliste bestehen; nicht verfügbare
Funktionen werden nicht als implementiert ausgegeben.
