# Qwhat

Microware-C-Nachbau des OS-9-Programms `what`. Der Modulname bleibt `what`,
die ausgelieferte Datei heißt `Qwhat`.

Sucht nach `@( # )`-Kennungen (ohne die Leerzeichen) und gibt den folgenden
Text bis Anführungszeichen, `>` oder NUL aus. `-s` beendet die Suche pro Datei
nach dem ersten Treffer.
