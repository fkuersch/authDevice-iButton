# iButton-Schließsystem (die Endgeräte an den Türen)

Die Endgeräte an den Türen bestehen aus je einem Arduino, der per RS485 (+ Strom) an den zentralen Raspberry Pi mit je einem separaten USB-RS485-Adapter + Kabel angeschlossen ist.

Der Arduino motitort kontinuierlich den 1Wire-Bus, um nach neuen iButtons zu suchen, nimmt aber ungefragt nie Kontakt mit dem Pi auf. Der Pi fragt pollt kontinuierlich (ca. alle 50ms) den Arduino im [HDLC](https://de.wikipedia.org/wiki/High-Level_Data_Link_Control)-Format, der in jedem Fall darauf antwortet. So kann der Pi schnell erkennen, wenn die Verbindung unterbrochen wurde.

Jedes Endgerät hat eine eigene DEVICE_ID und reagiert nicht, wenn es mit der falschen ID angesprochen wurde.

Auf den Endgeräten werden keine sensitiven Daten dauerhaft gespeichert.

Achtung: Pinbelegung und Relais-Timing sind für die obere Eingangstür anders als bei den Hauseingangstüren unten!

## HDLC-Commands

| Command           | Response           | Funktion                                                     |
| ----------------- | ------------------ | ------------------------------------------------------------ |
| CommandPing       | ResponsePong       | Schickt ein empfangenes Datenpaket (8 Byte) zurück. Für Testzwecke. |
| CommandGetStatus  | ResponseStatus     | Schickt zurück, ob ein iButton vorhanden ist und ggf. dessen Family-Code und Seriennummer. |
| CommandUnlockDoor | ResponseUnlockDoor | Zieht das Relais für eine fest eingestellte Zeit an, schaltet die grüne LED für eine fest eingestellte Zeit an |
| CommandRejectKey  | ResponseRejectKey  | Schaltet die rote LED für eine fest eingestellte Zeit an     |