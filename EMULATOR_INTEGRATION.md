# Emulator Integration - iso2kr & kr2iso

## Status: Work Image vorbereitet

**Image:** `/Volumes/SSD1TB/projects/Q9-Forge/Q9-Flux/local_images/OS9SYS.iso2kr-kr2iso.hda`

Dieses Image wurde erstellt und ist bereit, die Projektdateien aufzunehmen.

## Nächste Schritte

### 1. Verzeichnisse im Image erstellen
```bash
cd /Volumes/SSD1TB/projects/Retro\ OS-9\ Images/repos/toolshed/build/unix/os9

./os9 makdir /Volumes/SSD1TB/projects/Q9-Forge/Q9-Flux/local_images/OS9SYS.iso2kr-kr2iso.hda /dd/PROJECTS/iso2kr
./os9 makdir /Volumes/SSD1TB/projects/Q9-Forge/Q9-Flux/local_images/OS9SYS.iso2kr-kr2iso.hda /dd/PROJECTS/kr2iso
```

### 2. Dateien ins Image kopieren
```bash
./os9 copy iso2kr/*.c iso2kr/*.sh iso2kr/.clang-format \
  /Volumes/SSD1TB/projects/Q9-Forge/Q9-Flux/local_images/OS9SYS.iso2kr-kr2iso.hda /dd/PROJECTS/iso2kr/

./os9 copy kr2iso/*.c kr2iso/*.sh kr2iso/.clang-format \
  /Volumes/SSD1TB/projects/Q9-Forge/Q9-Flux/local_images/OS9SYS.iso2kr-kr2iso.hda /dd/PROJECTS/kr2iso/
```

### 3. Mit QCC im Emulator kompilieren
- Die Quelldateien befinden sich dann unter `/dd/PROJECTS/iso2kr` und `/dd/PROJECTS/kr2iso`
- QCC verwenden für die Kompilierung
- Binaries unter `/cmds` verfügbar machen

## ToolShed Referenz

ToolShed bindet sich unter:
```
/Volumes/SSD1TB/projects/Retro\ OS-9\ Images/repos/toolshed/build/unix/os9/os9
```

Kommando-Referenz:
```
os9 makdir <image> <directory>     # Verzeichnis erstellen
os9 copy <src> <image> <dst>       # Dateien kopieren
os9 dir <image> <path>             # Verzeichnis anzeigen
os9 del <image> <file>             # Datei löschen
```

## Dateien im Projekt

### iso2kr (ISO → K&R Format)
- `iso2kr.sh` - Shell-Wrapper (Bash-Script für Host)
- `iso2kr.c` - C-Implementation (für Emulator-Kompilierung)
- `.clang-format` - K&R Konfiguration
- `README.md` - Dokumentation

### kr2iso (K&R → ISO Format)  
- `kr2iso.sh` - Shell-Wrapper (Bash-Script für Host)
- `kr2iso.c` - C-Implementation (für Emulator-Kompilierung)
- `.clang-format` - ISO/LLVM Konfiguration
- `README.md` - Dokumentation

## Status

- ✅ Projektstruktur unter `/Volumes/SSD1TB/projects/Q9-Forge/Q9-Tools/`
- ✅ Shell-Scripts funktionsfähig (auf macOS Host)
- ✅ Arbeitsimage erstellt: `OS9SYS.iso2kr-kr2iso.hda`
- ⏳ Dateien ins Image einfügen (manuell mit ToolShed)
- ⏳ C-Implementation für QCC Compiler
- ⏳ Mit QCC kompilieren
- ⏳ Binaries in `/cmds` ablegen
