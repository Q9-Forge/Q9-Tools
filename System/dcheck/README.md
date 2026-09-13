# Q9dcheck

Read-only RBF-Volumeprüfung. Die erste Version liest Sektor 0, zeigt
Volumenname, Sektoranzahl, Clustergröße und Bitmap-Größe und weist ungültige
Grundparameter zurück. Bitmap-Reparatur, Directory-Prüfung und `-r` werden
erst nach einer vollständigen Gegenprüfung des RBF-Formats ergänzt.
