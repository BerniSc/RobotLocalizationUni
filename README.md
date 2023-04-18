# RobotLocalizationUni

![grafik](https://user-images.githubusercontent.com/117440705/232849202-608efc88-84a3-4ca6-96f3-d42264eed958.png)

In diesem Bild lässt sich die Funktionsweise des Programmes erkennen. Ein Rechteck wird aus Eckpukten erkannt und freigestellt. In diesem wird ein Kreisförmiger Roboter erkannt. Die erkannte und zur Kameraposition relativierte absolute Position des Roboters wird an den Roboterregler gesendet und der Roboter lässt sich Regeln.

Für eine genaue Erklärung, Zusammenfassung und Inbetriebnahme lässt sich eine ausführliche Anleitung auf der RoFo Wiki-Seite der Hochschule finden. Zur zusammenfassung hier jedoch noch einmal das wichtigste:

- Grundvorraussetzungen: **Ubuntu 20.04 (Focal Fossa)**, **opencv4**, **ROS 1 Noetic**, **pkg-config**
- Während dem Betrieb des Programmes muss in einem Seperaten Terminal der **roscore** gestartet sein
- Bauen lässt sich das Programm über das integrierte Makefile durch ausführen des Befehles **make** in der Projektdirectory
- Starten ist durch außführen über **./output** in der Projektdirectory möglich


Einige Tasten der Tastatur sind mit Shortcuts belegt:
- **'q'**: sorgt für Beenden des Programmes
- **'p'**: gibt die aktuellen Einstellungen der Dynamischen Parameter an
- **'m'**: wechselt zwischen dem Kallibrierungsmodus und dem Detektierungsmodus
- **'l'** + **'[0-9]'**: läd eine vorgespeicherte Konfiguration aus der Konfigurationsdatei
- **'c'**: zurücksetzen und neu erkennen der erkannten Eckpunkte -> neu-kalibrierung
- **'+'** & **'-'**: experimentelles einstellen der Werte für die Adaptive Threshold

Einige der Parameter lassen sich dynamisch über die geöffneten Fenster durch bewegen der Slider einstellen. Hier ist etwas ausprobieren gefragt.
- eine allgemein recht erfolgreich funktionierende Kombination lässt sich durch **'l'** + **'2'** laden

Eine beispielhafte Kombination zur Inbetriebnahme wäre dementsprechend: **'l'** + **'2'**, **'c'**, **'m'**

Ein Einspeichern von passenden Konfigurationen lässt sich durch einfügen der von **'p'** ausgegebenen Konfigurationen in die **".pacf"** Datei erreichen. Dies ist aber nur ein entwicklungsfeature und als solches noch nicht zur wirklichen Verwendung gedacht und Instabil. Weitere Informationen finden sich ebenfalls auf dem Wiki.

## TODO
- Kommentare finalisieren
- Zusätliche Slider-Manager einarbeiten (z.B. für Hough-Transformation)
- Konfigurationsladen als echtes Feature implementieren und nicht nur als *quick fix*
- Skalierung über Slider ausweiten und auf restliche Parameter auswirken lassen
- Über weiteres Erkennungsmerkmal auf mehrere Roboter gleichzeitig ausweiten
