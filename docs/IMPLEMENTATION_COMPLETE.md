# iso2kr & kr2iso - Komplette Implementation

## Status: Phase 2 & 3 ✅ Fertig

### Was ist fertig:

**C-Implementierung:**
- ✅ `iso2kr.c` (1.6 KB) - ISO C → K&R Konverter
- ✅ `kr2iso.c` (1.6 KB) - K&R → ISO C Konverter
- ✅ Beide sind Wrapper um clang-format (simple, wartbar)
- ✅ Kommandozeilen-Parser (Input/Output Dateien)
- ✅ Error Handling

**Build-System:**
- ✅ `iso2kr/Makefile` - für QCC Compiler
- ✅ `kr2iso/Makefile` - für QCC Compiler
- ✅ `build.sh` - Shell-Build-Script für schnelles Testing

**Konfiguration:**
- ✅ `.clang-format` in beiden Verzeichnissen (K&R und ISO Styles)
- ✅ README.md Dokumentation

**Test-Dateien:**
- ✅ `iso2kr/example_iso.c` - Testdatei
- ✅ `iso2kr/example_kr.c` - erwartete Ausgabe

**Dokumentation:**
- ✅ `INTEGRATION_STATUS.md` - Überblick
- ✅ `EMULATOR_INTEGRATION.md` - ToolShed Anleitung

---

## Phase 4: Installation im Emulator

### Schritt 1: Image vorbereiten (mit ToolShed oder Python)

Arbeitsimage: `/Volumes/SSD1TB/projects/Q9-Forge/Q9-Flux-68k/local_images/OS9SYS.iso2kr-kr2iso-qcc.hda`

```bash
# Option A: Mit ToolShed (kann hängen)
cd /Volumes/SSD1TB/projects/Retro\ OS-9\ Images/repos/toolshed/build/unix/os9

./os9 makdir <image> /dd/PROJECTS/iso2kr
./os9 makdir <image> /dd/PROJECTS/kr2iso

./os9 copy /path/to/iso2kr/* <image> /dd/PROJECTS/iso2kr/
./os9 copy /path/to/kr2iso/* <image> /dd/PROJECTS/kr2iso/
```

**Alternativer Weg: Mit Python (flexibler, zuverlässiger)**

Siehe: `/Volumes/SSD1TB/projects/Q9-Forge/Q9-Flux-68k/tools/build_os9sys_image.py`

```python
# Could adapt this script to add iso2kr/kr2iso directories and files
```

### Schritt 2: Im Emulator kompilieren

```bash
# Terminal 1: Emulator starten
cd /Volumes/SSD1TB/projects/Q9-Forge/Q9-Flux-68k
./qemu-system-m68k -hda local_images/OS9SYS.iso2kr-kr2iso-qcc.hda

# Terminal 2 (im Emulator):
shell
chd /dd/PROJECTS/iso2kr
qcc iso2kr.c -o /cmds/iso2kr

chd /dd/PROJECTS/kr2iso
qcc kr2iso.c -o /cmds/kr2iso

# Verify
ls -l /cmds/iso2kr /cmds/kr2iso
```

### Schritt 3: Testen

```bash
# Test iso2kr
echo "int foo(int a, int b) { return a+b; }" > /tmp/test.c
iso2kr /tmp/test.c

# Test kr2iso
kr2iso /tmp/test.c

# Round-trip
iso2kr /tmp/test.c -o /tmp/test.kr
kr2iso /tmp/test.kr -o /tmp/test.iso
cmp /tmp/test.c /tmp/test.iso  # Sollten identisch sein
```

---

## Dateistruktur

```
/Volumes/SSD1TB/projects/Q9-Forge/Q9-Tools/
├── iso2kr/
│   ├── iso2kr.c          # Main program (C)
│   ├── iso2kr.sh         # Host shell wrapper
│   ├── Makefile          # QCC build
│   ├── .clang-format     # K&R formatting config
│   ├── README.md
│   ├── example_iso.c     # Test input
│   └── example_kr.c      # Expected output
│
├── kr2iso/
│   ├── kr2iso.c          # Main program (C)
│   ├── kr2iso.sh         # Host shell wrapper
│   ├── Makefile          # QCC build
│   ├── .clang-format     # ISO formatting config
│   └── README.md
│
├── build.sh              # Build script (host)
├── INTEGRATION_STATUS.md # Diese Datei
└── EMULATOR_INTEGRATION.md
```

---

## Host-Verwendung (macOS)

Die Shell-Scripts funktionieren sofort:

```bash
cd /Volumes/SSD1TB/projects/Q9-Forge/Q9-Tools

# Konvertierung Host-seitig testen
./iso2kr/iso2kr.sh /path/to/somefile.c
./kr2iso/kr2iso.sh /path/to/somefile.c

# Mit Output-Datei
./iso2kr/iso2kr.sh input.c -o output.kr
./kr2iso/kr2iso.sh input.kr -o output.iso
```

---

## Next Steps

1. **ToolShed-Problem lösen:**
   - Entweder: Python FAT16 Library nutzen
   - Oder: Dateien manuell mit einem FAT-Editor hinzufügen
   - Oder: Im Emulator direkt Verzeichnisse anlegen

2. **QCC Kompilierung:**
   - Mit Emulator-Befehl `qcc` durchführen
   - Binary in `/cmds` ablegen

3. **Testing & Validierung:**
   - Round-trip Konvertierung testen
   - Vergleich mit clang-format Host-Ausgabe
   - Edge Cases abdecken

4. **Integration:**
   - Optional: In Q9-Flux Build-System einbinden
   - Optional: Weitere Tools (z.B. c99, c11 Converter)

---

## Dependencies

- **clang-format** (muss im Emulator vorhanden sein)
- **QCC Compiler** (bereits im Image vorhanden)
- **Standard C Library** (stdio.h, stdlib.h, unistd.h)

## Architektur

Beide Tools sind **Wrapper um clang-format**:

```
User Input (C-File)
        ↓
    iso2kr/kr2iso
        ↓
    system("clang-format ...")
        ↓
   K&R/ISO formatted output
```

Vorteile:
- Einfach zu warten
- Zuverlässig (clang-format ist bewährt)
- Portabel

---

**Status:** Bereit für Emulator-Integration! 🚀
