# iso2kr & kr2iso Emulator Integration - Status Report

## Fertig ✅

1. **Quellprojekte erstellt**
   - `/Volumes/SSD1TB/projects/Q9-Forge/Q9-Tools/iso2kr/`
   - `/Volumes/SSD1TB/projects/Q9-Forge/Q9-Tools/kr2iso/`
   
2. **Shell-Wrapper funktionsfähig** (macOS Host)
   - `iso2kr.sh` - testet & konvertiert ISO → K&R
   - `kr2iso.sh` - testet & konvertiert K&R → ISO
   - Beide nutzen clang-format mit angepassten .clang-format Konfigurationen

3. **OS9-Image vorbereitet**
   - Image: `/Volumes/SSD1TB/projects/Q9-Forge/Q9-Flux-68k/local_images/OS9SYS.iso2kr-kr2iso-qcc.hda`
   - QCC Compiler bereits vorhanden
   - Basis: OS9SYS.qcc-selfhost.hda (neueste)

## Nächste Schritte

### Phase 2: C-Implementierung
Für die Kompilierung im Emulator benötigen wir C-Sourcedateien. Optionen:

**Option A: Einfache Wrapper** (empfohlen für schnelle Implementierung)
```c
// iso2kr.c - minimal wrapper
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: iso2kr <inputfile> [-o <outputfile>]\n");
        return 1;
    }
    
    // Call clang-format with K&R config
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "clang-format --style=file:/dd/PROJECTS/iso2kr/.clang-format %s", argv[1]);
    return system(cmd);
}
```

**Option B: Eigenständiger Parser** (mehr Arbeit, flexibler)
- Würde K&R und ISO Parsing selbst implementieren
- Keine Abhängigkeit von clang-format
- ~500-1000 Zeilen C Code

### Phase 3: Build-Integration

Mit ToolShed Befehlen (oder Python FAT16 Library):
```bash
# Verzeichnisse erstellen
os9 makdir <image> /dd/PROJECTS/iso2kr
os9 makdir <image> /dd/PROJECTS/kr2iso

# Dateien kopieren
os9 copy iso2kr.c .clang-format <image> /dd/PROJECTS/iso2kr/
os9 copy kr2iso.c .clang-format <image> /dd/PROJECTS/kr2iso/
```

Im Emulator:
```
chd /dd/PROJECTS/iso2kr
qcc iso2kr.c -o /cmds/iso2kr
qcc ../kr2iso/kr2iso.c -o /cmds/kr2iso
```

### Phase 4: Testing
- Test mit Example-Dateien im Emulator
- Round-trip Konvertierung (ISO → K&R → ISO)
- Validierung gegen Original clang-format Ausgabe

## Problem: ToolShed FAT16 Image Tool

ToolShed (`os9` Binary) hängt bei Verzeichnis- und Datei-Operationen. Alternativen:

1. **Python FAT16 Library** - Q9-Flux hat tools/build_os9sys_image.py
2. **Direktes Disk-Imaging** - mit `dd` und FAT16 Mounting (macOS)
3. **Emulator selbst nutzen** - Dateien über Emulator-Tools hinzufügen
4. **Manuell ins Image einfügen** - mit Hex-Editor (nicht praktisch)

## Empfohlener Weg Forward

1. **Schnelle Lösung**: Option A (Simple Wrapper) + Python FAT16 Tools
2. **Langfristig**: Option B (eigenständiger Parser) für Unabhängigkeit
3. **Integration**: Mit bestehenden Q9-Tools-Struktur (wie Network/*, System/*)

## Dateien bereit

- ✅ iso2kr/.clang-format (K&R Konfiguration)
- ✅ kr2iso/.clang-format (ISO Konfiguration)  
- ✅ iso2kr/README.md
- ✅ kr2iso/README.md
- ✅ iso2kr/example_iso.c (Testdatei)
- ⏳ iso2kr.c (C-Wrapper - muss noch geschrieben werden)
- ⏳ kr2iso.c (C-Wrapper - muss noch geschrieben werden)
- ⏳ Makefile (für QCC Kompilierung)
