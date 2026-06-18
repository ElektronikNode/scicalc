scicalc changelog
=================

# V2.1.0
* Komplexe Zahlen mit `i` und `j`, z.B. `3+4i`, `2j` und `5k+3mj #V`
* Komplexe Ausgabe mit scicalc-Suffixen, z.B. `(5k + 3mj)V`
* Komplexe Vektoren und Matrizen inklusive Multiplikation, Transpose und `inv`
* Mathematische Funktionen arbeiten komplexfaehig und elementweise auf Matrizen
* Neue Funktionen `real`, `imag`, `conj` und `angle`
* CLI-Modus: `scicalc datei.sc` schreibt Eingaben und Ergebnisse auf stdout
* Robustere SI-Formatierung fuer sehr kleine Werte

# V2.0.1
* Version auf `2.0.1` angehoben
* Neue Funktion `round`
* Mathematische Funktionen arbeiten nun elementweise auf Vektoren und Matrizen
* `atan2(y,x)` unterstuetzt elementweise Auswertung und Skalar-Broadcast

# V2.0.0
* Erste Matlab-aehnliche Syntax fuer Vektoren und Matrizen
* Matrixliterale mit Leerzeichen, Komma und Semikolon, z.B. `[1 2; 3 4]`
* Range-Operator `start:end` und `start:step:end`
* Matrixausgabe mit ausgerichteten Spalten und Unicode-Rahmen
* Matrixoperatoren `+`, `-`, `*`, `/`, `.*`, `./`, `.^` und Transpose `'`
* Matrixinversion mit `inv(A)` auf Basis von Eigen
* Neue Funktionen `ceil`, `floor` und `getVersion`
* Interner `Value`-Typ fuer Skalare und Matrizen
* Automatische dreistellige Buildnummer, z.B. `2.0.0-###`
* Neue Demo- und Regressionstestdateien `test-demo.sc` und `test-cases.sc`

# V1.1.0
* Variablen-, Konstanten- und Funktionsvorschlaege im Editor
* Completion automatisch ab erstem Namenszeichen und manuell mit Strg+Space
* Variablenvorschlaege zeigen Wert und Einheit
* `#Ohm` wird als `Ω` ausgegeben
* Umstellung auf CMake mit Build-Unterverzeichnis `build/`
* Debian-Paketierung mit CPack, Desktop-Launcher und Icon-Installation

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
