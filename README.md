# Drohnenprojekt mit ESP32-CAM und Arduino Mega

## Übersicht

Dieses Repository enthält den Quellcode und die Dokumentation für ein Drohnenprojekt, das mit einem ESP32-CAM und einem Arduino Mega entwickelt wurde. Die Drohne ist in der Lage, Bilder in Echtzeit zu übertragen und kann über ein WLAN-Netzwerk gesteuert werden.

## Projekte

### 1. ESP32-CAM Webserver

- **Beschreibung**: Die ESP32-CAM fungiert als Webserver, der die Kamera steuert und Live-Streaming von Bildern ermöglicht.
- **Funktionen**:
  - Echtzeitübertragung von Kamerabildern.
  - Empfang von Steuerbefehlen über ein Webinterface.

### 2. Arduino Mega Motorsteuerung

- **Beschreibung**: Der Arduino Mega steuert die Motoren der Drohne basierend auf den Eingaben, die von der ESP32-CAM empfangen werden.
- **Funktionen**:
  - Steuerung der Motoren für Vorwärts-, Rückwärts- und Drehbewegungen.
  - Verarbeitung von Benutzereingaben zur Anpassung der Geschwindigkeit.

### 3. Externer Webserver

- **Beschreibung**: Der externe Webserver dient als zentrale Plattform für die Steuerung der Drohne und die Anzeige des Livestreams.
- **Funktionen**:
  - Benutzeroberfläche zur Steuerung der Drohne.
  - Anzeige der Live-Übertragung von der Kamera.
  - Steuerungselemente für die Drohne (Joystick, Geschwindigkeitskontrolle).

## Kommunikationsprotokolle

Die Kommunikation zwischen den verschiedenen Komponenten erfolgt über definierte Protokolle:

- **HTTP**: Für die Kommunikation zwischen dem externen Webserver und der ESP32-CAM.
- **Serielle Kommunikation**: Für die Übertragung von Steuerbefehlen zwischen der ESP32-CAM und dem Arduino Mega.

## Entwicklungsumgebungen

Für die Programmierung der Drohne wurden zwei Entwicklungsumgebungen in Betracht gezogen:

- **Arduino IDE**: Benutzerfreundlich und ideal für Anfänger.
- **PlatformIO**: Bietet erweiterte Funktionen, war jedoch komplexer in der Einrichtung.

## Installation

1. Klone das Repository:
   ```bash
   git clone <repository-url>
