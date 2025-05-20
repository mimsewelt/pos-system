📦 POS-System – Filialbasierte Verkaufssoftware mit C++ Backend

Dieses Projekt ist ein sicheres, leichtgewichtiges und modulbares POS-Backend in C++, das mit einem Swift-Frontend kommunizieren kann. Es unterstützt Benutzerauthentifizierung, Rollenverwaltung (Admin & Lager), Produktabfragen, Statusmeldungen und eigene Benutzerinformationen.

🧩 Features

    🔐 Login mit Passwort-Hash (SHA-256)

    ✅ Token-basierte Authentifizierung

    👤 Rollenbasiertes Rechtemodell („admin“, „lager“)

    📦 GET /products mit Tokenprüfung

    📋 GET /users nur für Admin

    🧠 GET /me – zeige eigene Benutzerdaten

    📶 GET /status – Lebenszeichen

    🧪 Zentrale Testlogik in tests/test.cpp

    ⚙️ Konfigurierbare Dateipfade über Config-Klasse

    📁 Datenablage in data/*.json

📁 Projektstruktur

.
├── include/
│ ├── config.hpp # zentrale Konfigurationsklasse
│ ├── http_server.hpp # Serverdefinition
│ └── sha256.hpp # SHA256 Hash-Funktion
├── src/
│ ├── config.cpp
│ ├── http_server.cpp
│ └── sha256.cpp
├── data/
│ ├── users.json
│ ├── products.json
│ └── log.txt
├── tests/
│ └── test.cpp # C++-basierter zentraler Test
├── CMakeLists.txt
└── README.md

🚀 Kompilieren & Starten
1. Abhängigkeiten installieren (nur beim ersten Mal)

sudo apt install cmake g++ libssl-dev
2. Projekt bauen

cmake -B build
cmake --build build
3. Server starten

./build/branch_server
4. Tests starten (nach Serverstart)

./build/tests

🛠 Konfiguration (Config)

Die Klasse Config ermöglicht dir zentrale Pfade zu ändern:

// Beispiel
std::ifstream users(Config::getUsersFilePath());
int port = Config::getServerPort();

Default-Konfigurationswerte:

    users.json: ../data/users.json

    products.json: ../data/products.json

    log.txt: ../data/log.txt

    Port: 8080

🧪 Tests (tests/test.cpp)

Tests werden automatisch ausgeführt und decken Folgendes ab:

    Login erfolgreich / fehlgeschlagen

    Tokenprüfung bei allen Routen

    GET /status

    GET /me (eigenes Profil)

    GET /products (Token erforderlich)

    GET /users (nur Admin)

📦 Beispiel-Benutzer

{
"users": [
{
"benutzer": "admin",
"rolle": "admin",
"passwort_hash": "<hash von geheim123>",
"token": "auth_admin"
},
{
"benutzer": "lager",
"rolle": "lager",
"passwort_hash": "<hash von lager123>",
"token": "auth_lager"
}
]
}

📡 Server online stellen (Optionen)

    Lokales Testen via WLAN:

    iPad/iPhone verbinden sich über IP (z. B. 192.168.1.5:8080)

    Öffentlich erreichbar (temporär):

    ngrok (https://ngrok.com)

ngrok http 8080

    Forwarded URL verwenden in der iOS-App

    VPS/Cloud Server:

    Beispiel: DigitalOcean, Hetzner

    C++ Server als Dienst starten

📲 Frontend (iOS)

Das Swift-Frontend ruft folgende Endpunkte auf:

    POST /login – Login

    GET /products – Produktliste anzeigen

    GET /me – Eigene Benutzerrolle anzeigen

    (zukünftig: Bestellungen, Lagerverwaltung usw.)

☑️ Status

✅ Schritt A: Struktur aufgeräumt
✅ Schritt B: Login, Token, Rollen
✅ Schritt C: Konfigurationsklasse
🟡 Schritt D: Server-Synchronisation
🔜 Schritt E: iPad App erweitern

🔗 GitHub

https://github.com/mimsewelt/pos-system