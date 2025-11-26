scicalc changelog
=================

# V1.0.1
* Accounting-Modus überspringt nun Leer- und Kommentarzeilen und verknüpft konsequent mit dem nächsten gültigen Ergebnis
* Diverse kleinere Stabilitäts- und Dokumentationsverbesserungen

# V1.0.0
* Added Accounting-Modus Option inkl. Operator-Verkettung beim Editieren
* Neue Skriptfunktionen `setDigits(n)`, `setTrailingZeros(b)` und 
  `setAccounting(b)` zur temporären Laufzeitkonfiguration
* Backspace/Delete verhalten sich wie erwartet bei Blockgrenzen und 
  aktualisieren die Ergebnisse sofort
* Verbesserte Enter-/Zeilenteilung inklusive Undo nach `Strg+N`
* Einfügen aus der Zwischenablage entfernt nun konsequent alle Formatierungen

# V0.91.3
* Fixed fatal bug with low number of significant bits. fracDigits must not get negative.
* support both decimal separators: '.' and ','

# V0.91.2
* Resource File angelegt für Hilfe-Doku
* Resource File für Icon

# V0.91.1
* moderne Standardfarben
* Kompatibilität mit Qt 5.12
* QSettings als String statt QVariant

# V0.91
Initiale Version von 2013
