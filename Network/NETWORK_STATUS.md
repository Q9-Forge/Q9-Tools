# Q9 Network Tools

## Gebaut und geprüft

- `Qping` – ICMP Echo, Hostname/IP, `-c`, `-s`, `-?`
- `Qbeam` – UDP-Testpakete, Host, Anzahl und Port
- `Qtcpsend` / `Qtcprecv` – einfacher TCP-Dateitransfer
- `Qmsend` / `Qmrecv` – UDP-Multicast-Senden und -Empfang
- `Qifconfig` – schreibgeschützte Interface-Abfrage
- `Qhost`, `Qservice`, `Qproto` – NetDB-Diagnosewerkzeuge
- `Qnetstat` – schreibgeschützte IPv4-Routingtabelle
- `Qrpcinfo` – RPC-Portmapper-Abfrage sowie TCP/UDP-NULLPROC-Test (`-p`, `-t`, `-u`)
- `Qtelnet` – zeilenorientierte TCP-Terminalbasis
- `Qtftp` – grundlegender RFC-1350-Client für `get` und `put`
- `Qrdir` / `Qdird` – RPC-Verzeichnisdienst: Client und Server
- `Qrmsg` / `Qmsgd` – RPC-Nachrichtendienst: Client und Server
- `Qrsort` / `Qsortd` – RPC-Sortierdienst: Client und Server
- `Qrpcnull` – RPC-`NULLPROC`-Test über TCP oder UDP
- `Qrpcport` – Portmapper-Abfrage für ein einzelnes RPC-Programm
- `Qrpcmap` – kompakte Portmapper-Kartenansicht
- `Qrpcudp` – RPC-`NULLPROC`-Test gezielt über UDP
- `Qroute` – schreibgeschützte IPv4-Routingtabelle über NetDB
- `Qintent` – schreibgeschützte Interface-Intent-Abfrage
- `Qinetd` – schreibgeschützte inetd-Service-Abfrage
- `Qbootptest` – sendet ein einzelnes BOOTP-Diagnosepaket
- `Qbootpwait` – empfängt eine BOOTP-Antwort mit zehn Sekunden Timeout
- `Qrpcscan` – scannt RPC-Programmnummern im Portmapper
- `Qroutectl` – kontrolliertes Hinzufügen/Löschen von NetDB-Routen

Die Module werden mit den passenden Microware-Netzwerkbibliotheken gelinkt
und wurden mit `os9 ident` auf CRC und Header-Parität geprüft.

Die Socket-Portierung benötigt die historischen `getstat/setstat`-Brücken;
diese sind in den jeweiligen Q9-Quellen dokumentiert. `Qping` wartet in der
aktuellen Basisfassung noch ohne Timeout auf die Antwort.

Die RPC-Demodienste basieren auf den Microware-RPC-Demos und wurden mit
`rpc.l`, `socket.l`, `netdb.l` und `ndblib.l` gelinkt. Alle zehn neuen Module
haben einen gültigen OS-9-Header und CRC; ein Lauf im Emulator steht noch aus.

`Qroute`, `Qintent` und `Qinetd` sind bewusst read-only. `Qhostconf` bleibt
vorerst als Analysequelle liegen, weil die vorhandene Toolchain dafür keinen
auflösbaren NetDB-Iterator exportiert.

`Qbootptest` sendet aktuell nur das Request-Paket; Antwortauswertung,
DHCP-Optionen und Retransmits folgen in einem späteren Durchlauf.

`Qbootpwait` ergänzt den Empfangspfad und zeigt zugewiesene Adresse und
Boot-Datei an. `Qrpcscan` fragt nur den Portmapper ab und ruft keine fremden
RPC-Prozeduren auf.

`Qroutectl` verändert die Routingtabelle ausschließlich mit dem expliziten
Zusatz `--apply`; die read-only-Abfrage bleibt weiterhin `Qroute`.
