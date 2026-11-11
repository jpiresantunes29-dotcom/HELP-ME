#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <time.h>
#include <driver/i2s.h>
#include <esp_idf_version.h>
#include <initializer_list>
#include <vector>
#include <deque>

const char* WIFI_SSID        = "NOME_DA_REDE";
const char* WIFI_SENHA       = "SENHA_DA_REDE";
const char* TELEGRAM_TOKEN   = "SEU_TOKEN_AQUI";
const char* CHAT_ID_USUARIA  = "7107756741";
const char* SENHA_CANCELAR   = "4452";
const char* SENHA_COACAO     = "2544";

const char* DEVICE_ID   = "ESP32_MULHER_01";
const char* NOME_MULHER = "Mariana Souza";

const int PINO_BOTAO = 22;
const int PINO_MOTOR = 4;
const int I2S_SCK    = 18;
const int I2S_WS     = 19;
const int I2S_SD     = 21;

const uint32_t SEGURAR_DISPARO_MS   = 1000;
const uint32_t JANELA_CANCELAR_MS   = 20000;
const uint32_t AUDIO_BLOCO_S        = 60;
const int      TENTATIVAS_SENHA     = 3;
const uint32_t SOM_SUSTENTADO_MS    = 1500;
const uint32_t SOM_TOLERANCIA_MS    = 300;
const uint32_t MOTOR_MANUAL_MAX_MS  = 10000;
const uint32_t RECONECTAR_MS        = 10000;
long limiteSom = 400000;

const uint32_t AUDIO_TAXA     = 8000;
const uint32_t AUDIO_BYTES_S  = AUDIO_TAXA * 2;
const int      AUDIO_GANHO    = 8;

const char* LOG_FILE  = "/log.txt";
const char* LOG_OLD   = "/log_old.txt";
const char* HIST_FILE = "/historico.txt";
const char* OC_FILE   = "/ocorrencia.txt";
const char* AUDIO_DIR = "/audio";
const size_t LOG_MAX  = 24576;

const char PAGINA_HTML[] PROGMEM = R"HELPME(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title>HELP ME - Central de Proteção e Monitoramento</title>

  <link rel="preconnect" href="https://fonts.googleapis.com">
  <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
  <link href="https://fonts.googleapis.com/css2?family=Plus+Jakarta+Sans:wght@300;400;500;600;700;800&display=swap" rel="stylesheet">
  <script src="https://unpkg.com/@phosphor-icons/web"></script>

  <link rel="stylesheet" href="https://unpkg.com/leaflet@1.9.4/dist/leaflet.css" />
  <script src="https://unpkg.com/leaflet@1.9.4/dist/leaflet.js"></script>
  <style>
:root {
  --bg-dark: #090a0f;
  --bg-card: #12141c;
  --bg-card-hover: #191c28;
  --border-color: #232736;
  --primary-red: #ff2a4b;
  --primary-red-glow: rgba(255, 42, 75, 0.3);
  --accent-green: #00e676;
  --accent-yellow: #ffb300;
  --text-main: #f3f5fa;
  --text-muted: #94a0b8;
  --text-sub: #64748b;
  --font-family: 'Plus Jakarta Sans', sans-serif;
  --sidebar-width: 320px;
}

* {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
}

body {
  font-family: var(--font-family);
  background-color: var(--bg-dark);
  color: var(--text-main);
  display: flex;
  height: 100vh;
  overflow: hidden;
}

.sidebar {
  width: var(--sidebar-width);
  background-color: #0b0c12;
  border-right: 1px solid var(--border-color);
  display: flex;
  flex-direction: column;
  justify-content: space-between;
  padding: 32px 24px;
  flex-shrink: 0;
  z-index: 10;
}

.brand {
  display: flex;
  align-items: center;
  gap: 16px;
  padding-bottom: 28px;
  border-bottom: 1px solid var(--border-color);
}

.brand-icon {
  width: 48px;
  height: 48px;
  background: var(--primary-red-glow);
  color: var(--primary-red);
  border-radius: 12px;
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 28px;
  border: 1px solid var(--primary-red);
}

.brand-text h2 {
  font-size: 1.3rem;
  font-weight: 800;
  letter-spacing: 1.5px;
}

.brand-text span {
  font-size: 0.7rem;
  color: var(--text-muted);
  letter-spacing: 1px;
  font-weight: 700;
}

.menu {
  margin-top: 28px;
  display: flex;
  flex-direction: column;
  gap: 10px;
  flex-grow: 1;
  overflow-y: auto;
}

.menu-label {
  font-size: 0.7rem;
  font-weight: 800;
  color: var(--text-sub);
  letter-spacing: 1.2px;
  margin-top: 16px;
  margin-bottom: 6px;
  padding-left: 8px;
}

.menu-item {
  display: flex;
  align-items: center;
  gap: 14px;
  padding: 14px 18px;
  color: var(--text-muted);
  text-decoration: none;
  border-radius: 12px;
  font-size: 0.95rem;
  font-weight: 600;
  background: transparent;
  border: none;
  cursor: pointer;
  width: 100%;
  text-align: left;
  transition: all 0.2s ease;
}

.menu-item i {
  font-size: 20px;
}

.menu-item:hover,
.menu-item.active {
  background: var(--bg-card-hover);
  color: var(--text-main);
}

.menu-item.active {
  border-left: 4px solid var(--primary-red);
  background: linear-gradient(90deg, rgba(255,42,75,.15), transparent);
  color: #fff;
}

.btn-action {
  background: #181c28;
  border: 1px solid var(--border-color);
  color: var(--text-main);
}

.btn-action.highlight {
  border-color: rgba(255,42,75,.4);
  background: rgba(255,42,75,.12);
  color: var(--primary-red);
}

.badge {
  margin-left: auto;
  background: var(--primary-red);
  color: #fff;
  font-size: 0.75rem;
  font-weight: 800;
  padding: 3px 10px;
  border-radius: 12px;
}

.sidebar-footer {
  display: flex;
  flex-direction: column;
  gap: 18px;
  padding-top: 20px;
  border-top: 1px solid var(--border-color);
}

.system-status {
  display: flex;
  align-items: center;
  gap: 12px;
  background: var(--bg-card);
  padding: 14px;
  border-radius: 12px;
  border: 1px solid var(--border-color);
}

.system-status strong {
  font-size: 0.82rem;
  display: block;
}

.system-status p {
  font-size: 0.72rem;
  color: var(--text-muted);
}

.user-profile {
  display: flex;
  align-items: center;
  gap: 14px;
  padding: 10px;
  background: #121520;
  border-radius: 12px;
  border: 1px solid var(--border-color);
}

.avatar {
  width: 42px;
  height: 42px;
  border-radius: 50%;
  background: #2a2e3d;
  display: flex;
  align-items: center;
  justify-content: center;
  font-weight: 800;
  font-size: 0.95rem;
  color: var(--primary-red);
  border: 2px solid var(--primary-red);
}

.user-info strong {
  font-size: 0.9rem;
  display: block;
}

.user-info p {
  font-size: 0.75rem;
  color: var(--text-muted);
}

.main-content {
  flex-grow: 1;
  display: flex;
  flex-direction: column;
  overflow: hidden;
}

.topbar {
  min-height: 96px;
  border-bottom: 1px solid var(--border-color);
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 16px 40px;
  background: #0b0c12;
  gap: 24px;
}

.search-area {
  position: relative;
  width: min(580px, 100%);
  flex-shrink: 1;
}

.search-bar {
  display: flex;
  align-items: center;
  gap: 14px;
  background: var(--bg-card);
  padding: 14px 22px;
  border-radius: 12px;
  border: 1px solid var(--border-color);
  width: 100%;
  font-size: 1rem;
}

.search-bar input {
  background: transparent;
  border: none;
  outline: none;
  color: var(--text-main);
  width: 100%;
  font-family: var(--font-family);
  font-size: 1rem;
}

.topbar-actions {
  display: flex;
  align-items: center;
  gap: 24px;
}

.live-monitor {
  font-size: 0.85rem;
  font-weight: 800;
  display: flex;
  align-items: center;
  gap: 10px;
  background: rgba(0,230,118,.1);
  color: var(--accent-green);
  padding: 10px 18px;
  border-radius: 20px;
  border: 1px solid rgba(0,230,118,.3);
}

.status-dot {
  width: 10px;
  height: 10px;
  border-radius: 50%;
}

.status-dot.green {
  background: var(--accent-green);
}

.pulse {
  animation: pulse-animation 1.5s infinite;
}

@keyframes pulse-animation {
  0% { box-shadow: 0 0 0 0 rgba(0,230,118,.7); }
  100% { box-shadow: 0 0 0 10px rgba(0,230,118,0); }
}

.icon-btn {
  background: var(--bg-card);
  border: 1px solid var(--border-color);
  color: var(--text-main);
  width: 48px;
  height: 48px;
  border-radius: 12px;
  cursor: pointer;
  position: relative;
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 22px;
}

.notification-badge {
  position: absolute;
  top: -4px;
  right: -4px;
  background: var(--primary-red);
  font-size: 0.7rem;
  width: 18px;
  height: 18px;
  border-radius: 50%;
  display: flex;
  align-items: center;
  justify-content: center;
  font-weight: 800;
}

.current-date {
  font-size: 0.85rem;
  color: var(--text-muted);
  display: flex;
  align-items: center;
  gap: 8px;
  font-weight: 700;
  text-transform: uppercase;
}

.btn-primary {
  background: #3b82f6;
  color: white;
  border: none;
  padding: 12px 24px;
  border-radius: 12px;
  font-weight: 700;
  cursor: pointer;
  display: flex;
  align-items: center;
  gap: 8px;
  font-size: 0.95rem;
}

.content-scroll {
  flex-grow: 1;
  overflow-y: auto;
  padding: 48px;
  display: flex;
  flex-direction: column;
  gap: 56px;
  scroll-behavior: smooth;
}

.dashboard-section {
  display: flex;
  flex-direction: column;
  gap: 28px;
}

.section-header {
  display: flex;
  justify-content: space-between;
  align-items: flex-end;
}

.subtitle {
  font-size: 0.75rem;
  letter-spacing: 1.5px;
  font-weight: 800;
  color: var(--text-sub);
}

h1 {
  font-size: 2.2rem;
  font-weight: 800;
  margin-top: 4px;
}

h2 {
  font-size: 1.5rem;
  font-weight: 800;
}

.desc {
  font-size: 0.95rem;
  color: var(--text-muted);
  margin-top: 4px;
}

.metrics-grid-2x2 {
  display: grid;
  grid-template-columns: repeat(2, 1fr);
  gap: 24px;
}

.metric-card {
  background: var(--bg-card);
  border: 1px solid var(--border-color);
  border-radius: 16px;
  padding: 28px;
  display: flex;
  flex-direction: column;
  gap: 16px;
}

.metric-card.alert {
  border-color: rgba(255,42,75,.5);
  background: linear-gradient(180deg, rgba(255,42,75,.08), var(--bg-card));
}

.card-icon {
  width: 48px;
  height: 48px;
  background: #181c28;
  border-radius: 12px;
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 24px;
  color: var(--text-muted);
}

.metric-card.alert .card-icon {
  color: var(--primary-red);
  background: var(--primary-red-glow);
}

.card-data .label {
  font-size: 0.75rem;
  font-weight: 800;
  color: var(--text-sub);
  letter-spacing: 0.5px;
}

.value-group {
  display: flex;
  align-items: baseline;
  gap: 12px;
  margin-top: 8px;
}

.value {
  font-size: 2.4rem;
  font-weight: 800;
}

.tag {
  font-size: 0.75rem;
  padding: 4px 8px;
  border-radius: 6px;
  font-weight: 700;
}

.tag.green {
  background: rgba(0,230,118,.15);
  color: var(--accent-green);
}

.tag.neutral-tag {
  background: rgba(148,160,184,.15);
  color: var(--text-muted);
}

.tag.red-alert {
  background: var(--primary-red);
  color: white;
}

.tag.neutral {
  background: #222634;
  color: var(--text-muted);
}

.card-foot {
  font-size: 0.85rem;
  color: var(--text-muted);
  border-top: 1px solid var(--border-color);
  padding-top: 14px;
}

.critical-banner {
  background: var(--bg-card);
  border: 1px solid var(--primary-red);
  border-radius: 16px;
  padding: 36px;
  display: flex;
  flex-direction: column;
  gap: 28px;
  box-shadow: 0 0 35px rgba(255,42,75,.18);
}

.alert-header-status {
  display: flex;
  align-items: center;
  gap: 16px;
  border-bottom: 1px solid var(--border-color);
  padding-bottom: 24px;
}

.pulse-icon {
  font-size: 40px;
  color: var(--primary-red);
  animation: pulse-red 1s infinite alternate;
}

@keyframes pulse-red {
  from { opacity: .5; }
  to { opacity: 1; }
}

.badge-priority {
  margin-left: auto;
  background: var(--primary-red);
  color: white;
  font-size: 0.85rem;
  font-weight: 800;
  padding: 8px 16px;
  border-radius: 20px;
}

.victim-profile-grid {
  display: flex;
  flex-direction: column;
  gap: 24px;
}

.victim-photo-card {
  display: flex;
  align-items: center;
  gap: 24px;
  background: #181c28;
  padding: 28px;
  border-radius: 14px;
}

.avatar-large {
  width: 80px;
  height: 80px;
  border-radius: 50%;
  background: #2a3146;
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 40px;
  color: var(--primary-red);
  border: 2px solid var(--primary-red);
}

.victim-main h2 {
  font-size: 2rem;
}

.address {
  color: var(--text-muted);
  font-size: 1rem;
  margin-top: 6px;
}

.time-callout {
  margin-top: 14px;
  background: rgba(255,42,75,.15);
  display: inline-flex;
  align-items: center;
  gap: 12px;
  padding: 8px 16px;
  border-radius: 8px;
  border: 1px solid var(--primary-red);
}

.time-callout span {
  font-size: 0.75rem;
  color: var(--text-muted);
  font-weight: 700;
}

.time-callout strong {
  font-size: 1.2rem;
  color: var(--primary-red);
}

.audio-proof-box {
  background: #151922;
  border: 1px solid rgba(255,42,75,.3);
  padding: 20px 24px;
  border-radius: 14px;
  display: flex;
  flex-direction: column;
  gap: 16px;
}

.audio-header {
  display: flex;
  align-items: center;
  gap: 14px;
  color: var(--primary-red);
  font-size: 22px;
}

.audio-header .label {
  font-size: 0.75rem;
  font-weight: 800;
  color: var(--text-sub);
  display: block;
  letter-spacing: 0.5px;
}

.audio-header p {
  font-size: 0.85rem;
  color: var(--text-muted);
  margin-top: 2px;
}

.audio-player-mock {
  display: flex;
  align-items: center;
  gap: 16px;
  background: #1c2130;
  padding: 12px 20px;
  border-radius: 10px;
  border: 1px solid var(--border-color);
}

.play-btn {
  width: 38px;
  height: 38px;
  border-radius: 50%;
  background: var(--primary-red);
  color: white;
  border: none;
  display: flex;
  align-items: center;
  justify-content: center;
  cursor: pointer;
  font-size: 16px;
}

.waveform-visual {
  flex-grow: 1;
  display: flex;
  align-items: center;
  gap: 6px;
  height: 36px;
}

.waveform-visual span {
  width: 4px;
  background: var(--primary-red);
  border-radius: 2px;
  opacity: .8;
}

.audio-time {
  font-size: 0.8rem;
  font-weight: 700;
  color: var(--text-muted);
}

.action-buttons-group {
  background: #181c28;
  padding: 24px;
  border-radius: 14px;
  border: 1px solid var(--border-color);
}

.btn-dual {
  display: flex;
  gap: 14px;
  margin-top: 14px;
}

.btn-police,
.btn-samu,
.btn-confirm-success,
.btn-cancel-operator {
  flex: 1;
  padding: 16px;
  border: none;
  border-radius: 12px;
  font-weight: 800;
  cursor: pointer;
  display: flex;
  align-items: center;
  justify-content: center;
  gap: 10px;
  font-size: 0.95rem;
}

.btn-police {
  background: #1e40af;
  color: white;
}

.btn-samu {
  background: #b91c1c;
  color: white;
}

.btn-confirm-success {
  width: 100%;
  margin-top: 14px;
  background: var(--accent-green);
  color: #000;
  font-size: 1rem;
}

.btn-cancel-operator {
  width: 100%;
  margin-top: 10px;
  background: transparent;
  border: 1px solid var(--primary-red);
  color: var(--primary-red);
  font-size: 0.9rem;
}

.btn-cancel-operator:hover {
  background: rgba(255,42,75,.1);
}

.map-container {
  height: 480px;
  width: 100%;
  border-radius: 14px;
  border: 1px solid var(--border-color);
  z-index: 1;
}

.card-panel {
  background: var(--bg-card);
  border: 1px solid var(--border-color);
  border-radius: 16px;
  padding: 32px;
}

.data-table {
  width: 100%;
  border-collapse: collapse;
  text-align: left;
  margin-top: 20px;
}

.data-table th {
  padding: 16px;
  color: var(--text-sub);
  font-size: 0.8rem;
  border-bottom: 1px solid var(--border-color);
}

.data-table td {
  padding: 20px 16px;
  border-bottom: 1px solid var(--border-color);
  font-size: 0.95rem;
}

.status-pill.success {
  background: rgba(0,230,118,.15);
  color: var(--accent-green);
  padding: 6px 12px;
  border-radius: 8px;
  font-weight: 700;
}

.btn-sm {
  background: #2a3146;
  color: white;
  border: none;
  padding: 8px 14px;
  border-radius: 8px;
  cursor: pointer;
  font-weight: 600;
}

.btn-danger-sm {
  background: rgba(255,42,75,.2);
  color: #ff4d6d;
  border: 1px solid var(--primary-red);
  padding: 6px 12px;
  border-radius: 6px;
  cursor: pointer;
  font-weight: 700;
  transition: all .2s;
}

.btn-danger-sm:hover {
  background: var(--primary-red);
  color: white;
}

.flex-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  flex-wrap: wrap;
  gap: 16px;
}

.chart-toggle-group {
  display: flex;
  background: #181c28;
  border: 1px solid var(--border-color);
  padding: 4px;
  border-radius: 12px;
  gap: 4px;
}

.btn-toggle {
  background: transparent;
  border: none;
  color: var(--text-muted);
  padding: 8px 16px;
  font-family: var(--font-family);
  font-weight: 700;
  font-size: 0.85rem;
  border-radius: 8px;
  cursor: pointer;
  transition: all 0.2s ease;
}

.btn-toggle.active {
  background: var(--primary-red);
  color: white;
}

.stats-summary-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));
  gap: 16px;
  margin-top: 24px;
}

.stat-box {
  background: #181c28;
  border: 1px solid var(--border-color);
  padding: 20px;
  border-radius: 14px;
  display: flex;
  flex-direction: column;
  gap: 6px;
}

.stat-box .label {
  font-size: 0.75rem;
  color: var(--text-sub);
  font-weight: 800;
  letter-spacing: 0.5px;
}

.stat-box h3 {
  font-size: 1.8rem;
  font-weight: 800;
  color: var(--text-main);
}

.dynamic-chart-container {
  margin-top: 32px;
  height: 240px;
  background: #151922;
  border: 1px solid var(--border-color);
  border-radius: 12px;
  position: relative;
  display: flex;
  padding: 20px 20px 10px 40px;
}

.chart-y-axis {
  position: absolute;
  left: 10px;
  top: 15px;
  bottom: 15px;
  display: flex;
  flex-direction: column;
  justify-content: space-between;
  font-size: 0.75rem;
  color: var(--text-sub);
  font-weight: 700;
}

.chart-bars-area {
  flex-grow: 1;
  display: flex;
  align-items: flex-end;
  justify-content: space-around;
  height: 100%;
  border-bottom: 1px solid var(--border-color);
  padding-bottom: 2px;
}

.chart-bar-column {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 8px;
  flex: 1;
  height: 100%;
  justify-content: flex-end;
}

.chart-bar {
  width: 28px;
  background: linear-gradient(180deg, var(--primary-red), rgba(255,42,75,.4));
  border-radius: 6px 6px 0 0;
  transition: height 0.4s ease;
  position: relative;
}

.chart-x-axis {
  display: flex;
  justify-content: space-around;
  padding-left: 40px;
  margin-top: 10px;
  font-size: 0.8rem;
  color: var(--text-muted);
  font-weight: 700;
}

.modal-overlay {
  position: fixed;
  inset: 0;
  background: rgba(0,0,0,.85);
  display: none;
  align-items: center;
  justify-content: center;
  z-index: 999;
}

.modal-container {
  background: var(--bg-card);
  border: 1px solid var(--border-color);
  width: 100%;
  max-width: 600px;
  border-radius: 18px;
  padding: 32px;
  display: flex;
  flex-direction: column;
  gap: 20px;
}

.modal-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
}

.btn-close {
  background: transparent;
  border: none;
  color: white;
  font-size: 26px;
  cursor: pointer;
}

.users-modal-container {
  max-width: 850px;
  max-height: 85vh;
  display: flex;
  flex-direction: column;
}

.users-toolbar {
  display: flex;
  justify-content: flex-end;
}

.users-table-wrapper {
  max-height: 50vh;
  overflow-y: auto;
}

#modal-telegram.modal-overlay {
  position: fixed;
  inset: 0;
  pointer-events: none;
  background: transparent;
  z-index: 2000;
  display: none;
}

.resizable-alert {
  position: absolute;
  top: 80px;
  right: 40px;
  width: 320px;
  min-width: 260px;
  max-width: 550px;
  height: auto;
  min-height: 220px;
  padding: 16px;
  background: #12141c;
  border: 1px solid var(--accent-yellow);
  box-shadow: 0 16px 40px rgba(0, 0, 0, 0.8), 0 0 20px rgba(255, 179, 0, 0.2);
  border-radius: 14px;
  pointer-events: auto;
  resize: both;
  overflow: hidden;
  gap: 12px;
}

.drag-handle {
  cursor: move;
  user-select: none;
  background: rgba(255, 179, 0, 0.1);
  padding: 8px 12px;
  border-radius: 8px;
  margin-bottom: 4px;
}

.drag-handle h2 {
  font-size: 1.05rem;
}

.simulator-body {
  display: flex;
  flex-direction: column;
  align-items: center;
  text-align: center;
  gap: 8px;
}

.simulator-body p {
  font-size: 0.85rem;
  color: var(--text-muted);
}

.panic-button-graphic {
  width: 50px;
  height: 50px;
  border-radius: 50%;
  display: flex;
  align-items: center;
  justify-content: center;
  font-size: 26px;
  margin: 4px auto;
  border: 1px solid var(--accent-yellow);
  background: rgba(255, 179, 0, 0.15);
  color: var(--accent-yellow);
}

.countdown-box {
  margin-top: 6px;
  font-size: 0.95rem;
  font-weight: 800;
}

#countdown-timer {
  color: var(--primary-red);
  font-size: 1.3rem;
}

.victim-search-results {
  position: absolute;
  top: calc(100% + 8px);
  left: 0;
  right: 0;
  z-index: 1500;
  max-height: 360px;
  overflow-y: auto;
  background: #12141c;
  border: 1px solid #34394b;
  border-radius: 12px;
  box-shadow: 0 18px 40px rgba(0,0,0,.5);
}

.victim-search-results[hidden] {
  display: none;
}

.victim-search-result {
  display: flex;
  flex-direction: column;
  gap: 5px;
  width: 100%;
  padding: 14px 16px;
  text-align: left;
  color: var(--text-main);
  background: transparent;
  border: 0;
  border-bottom: 1px solid var(--border-color);
  cursor: pointer;
  font: inherit;
}

.victim-search-result:last-child {
  border-bottom: 0;
}

.victim-search-result:hover,
.victim-search-result:focus {
  background: #202536;
  outline: none;
}

.victim-search-result strong {
  font-size: .95rem;
}

.victim-search-result span,
.victim-search-result small {
  color: var(--text-muted);
  font-size: .78rem;
  overflow-wrap: anywhere;
}

.search-empty {
  padding: 18px;
  color: var(--text-muted);
  font-size: .9rem;
}

.victim-form {
  max-height: 90vh;
  overflow-y: auto;
}

.victim-form label {
  display: flex;
  flex-direction: column;
  gap: 8px;
  color: var(--text-muted);
  font-size: .85rem;
  font-weight: 700;
}

.victim-form input,
.victim-form textarea {
  width: 100%;
  padding: 12px 14px;
  color: var(--text-main);
  background: #0b0c12;
  border: 1px solid var(--border-color);
  border-radius: 9px;
  font: inherit;
  resize: vertical;
}

.victim-form input:focus,
.victim-form textarea:focus {
  outline: 1px solid var(--accent-yellow);
  border-color: var(--accent-yellow);
}

.form-actions {
  display: flex;
  justify-content: flex-end;
  gap: 12px;
}

.btn-secondary {
  padding: 12px 20px;
  border-radius: 10px;
  border: 1px solid var(--border-color);
  background: transparent;
  color: var(--text-main);
  cursor: pointer;
  font-weight: 700;
}

.form-message {
  min-height: 1.2em;
  color: var(--accent-green);
  font-size: .85rem;
}

@media (max-width: 900px) {
  .sidebar {
    width: 240px;
    padding: 24px 16px;
  }

  .topbar {
    padding: 16px 20px;
    height: auto;
    flex-wrap: wrap;
  }

  .search-area {
    width: 100%;
  }

  .topbar-actions {
    flex-wrap: wrap;
    gap: 12px;
  }

  .content-scroll {
    padding: 24px;
  }
}

.status-dot.red { background: var(--primary-red); box-shadow: 0 0 10px var(--primary-red); }
.esp-grid { display:grid; grid-template-columns:repeat(auto-fit,minmax(190px,1fr)); gap:12px; margin-top:18px; }
.esp-item { background:var(--bg-card-hover); border:1px solid var(--border-color); border-radius:12px; padding:12px 14px; }
.esp-item .label { font-size:.68rem; color:var(--text-muted); letter-spacing:1px; text-transform:uppercase; }
.esp-item strong { display:block; margin-top:4px; font-size:1rem; word-break:break-all; }
.vol-bar { height:8px; background:#232736; border-radius:6px; overflow:hidden; margin-top:8px; }
.vol-bar span { display:block; height:100%; width:0; background:var(--accent-green); transition:width .3s; }
.esp-actions { display:flex; flex-wrap:wrap; gap:10px; margin-top:18px; align-items:center; }
.esp-actions input { background:#0b0c12; border:1px solid var(--border-color); color:var(--text-main); border-radius:8px; padding:9px 10px; width:130px; }

.hm-log { margin-top: 4px; padding: 0 2px 28px; }
.hm-log-head { display:flex; justify-content:space-between; align-items:baseline; padding-bottom:8px; margin-bottom:8px; border-bottom:1px solid var(--border-color); }
.hm-log-head span { font-size:.72rem; font-weight:700; letter-spacing:1.4px; text-transform:uppercase; color:var(--text-sub); }
.hm-log-head em { font-style:normal; font-weight:500; letter-spacing:.4px; text-transform:none; color:var(--text-muted); margin-left:8px; }
.hm-log-head a { font-size:.74rem; color:var(--text-muted); text-decoration:none; border-bottom:1px dotted var(--text-muted); }
.hm-log-head a:hover { color:var(--text-main); }
#esp-log { margin:0; background:transparent; border:none; padding:0; font-family:Consolas,'Courier New',monospace; font-size:.74rem;
  line-height:1.65; color:var(--text-muted); max-height:240px; overflow:auto; white-space:pre-wrap; }
.hm-banner[hidden] { display:none !important; }
.hm-banner { display:flex; gap:12px; align-items:flex-start; border-radius:12px; padding:12px 16px; margin-bottom:14px; }
.hm-banner i { font-size:26px; }
.hm-banner strong { display:block; letter-spacing:.5px; }
.hm-banner span { font-size:.85rem; color:var(--text-main); opacity:.85; }
.hm-banner-red { background:rgba(255,42,75,.14); border:1px solid var(--primary-red); color:var(--primary-red); }
.hm-banner-yellow { background:rgba(255,179,0,.10); border:1px solid var(--accent-yellow); color:var(--accent-yellow); }
.hm-row { display:flex; flex-wrap:wrap; gap:8px; margin-top:10px; }
.hm-chat { background:var(--bg-card-hover); border:1px solid var(--border-color); border-radius:14px; padding:14px; display:flex; flex-direction:column; gap:10px; }
.hm-chat .label { font-size:.68rem; color:var(--text-muted); letter-spacing:1px; }
.hm-chat-box { max-height:220px; min-height:90px; overflow:auto; display:flex; flex-direction:column; gap:8px; }
.hm-msg { padding:8px 12px; border-radius:10px; font-size:.88rem; max-width:92%; background:#0b0c12; border:1px solid var(--border-color); }
.hm-msg span { display:block; font-size:.68rem; color:var(--text-muted); margin-bottom:2px; }
.hm-msg.hm-usuaria { align-self:flex-start; border-color:rgba(0,230,118,.35); }
.hm-msg.hm-central { align-self:flex-end; border-color:rgba(59,130,246,.5); }
.hm-msg.hm-sistema { align-self:center; font-size:.78rem; color:var(--text-muted); background:transparent; border-style:dashed; }
.hm-chat-form { display:flex; gap:8px; }
.hm-chat-form input { flex:1; background:#0b0c12; border:1px solid var(--border-color); color:var(--text-main); border-radius:8px; padding:10px; font-family:inherit; }
.hm-chat-form input:disabled { opacity:.5; }
.btn-user-cancel { margin-top:14px; width:100%; background:transparent; border:1px solid var(--accent-yellow); color:var(--accent-yellow);
  padding:10px; border-radius:10px; font-weight:700; cursor:pointer; font-family:inherit; }

:root { --sidebar-width: 268px; --radius: 12px; }
body { font-size: 15px; -webkit-font-smoothing: antialiased; line-height: 1.45; }
button, input, select, textarea { font-family: inherit; }
audio { color-scheme: dark; width: 100%; height: 40px; }
::-webkit-scrollbar { width: 8px; height: 8px; }
::-webkit-scrollbar-thumb { background: #262b3a; border-radius: 8px; }
::-webkit-scrollbar-track { background: transparent; }

.sidebar { height: 100vh; overflow: hidden; padding: 22px 16px 18px; }
.brand { gap: 12px; padding-bottom: 18px; }
.brand-icon { width: 40px; height: 40px; font-size: 22px; border-radius: 10px; }
.brand-text h2 { font-size: 1.08rem; letter-spacing: 1.4px; }
.brand-text span { font-size: .62rem; letter-spacing: 1.2px; }
.menu { margin-top: 10px; gap: 2px; overflow: hidden; }
.menu-label { font-size: .62rem; letter-spacing: 1.4px; margin: 14px 0 4px; padding-left: 12px; }
.menu-item { padding: 9px 12px; gap: 11px; font-size: .85rem; font-weight: 600; border-radius: 9px; line-height: 1.25; border: 1px solid transparent; }
.menu-item i { font-size: 18px; flex-shrink: 0; }
.menu-item.active { border-left: 3px solid var(--primary-red); }
.btn-action { background: transparent; border: 1px solid transparent; }
.btn-action.highlight { margin-top: 6px; border: 1px solid rgba(255,42,75,.35); }
.badge { font-size: .68rem; padding: 2px 8px; }
.sidebar-footer { gap: 8px; padding-top: 14px; }
.system-status { padding: 10px 12px; border-radius: 10px; }
.system-status strong { font-size: .82rem; }
.system-status p { font-size: .7rem; }
.user-profile { padding: 8px 10px; gap: 10px; border-radius: 10px; }
.avatar { width: 34px; height: 34px; font-size: .78rem; }
.user-info strong { font-size: .84rem; }
.user-info p { font-size: .7rem; }
.lv-menu-tag { margin-left: auto; font-size: .6rem; font-weight: 800; letter-spacing: .6px; padding: 2px 7px; border-radius: 6px;
  background: #1b1f2c; color: var(--text-sub); text-transform: uppercase; white-space: nowrap; }
.lv-menu-tag.vivo { background: rgba(46,229,157,.14); color: #2ee59d; }
.lv-menu-tag.alerta { background: rgba(255,179,0,.14); color: var(--accent-yellow); }
#menu-lv:disabled { opacity: .45; cursor: default; }
@media (max-height: 700px) { .system-status { display: none; } .menu-label { margin-top: 10px; } }

.topbar { min-height: 72px; padding-top: 14px; padding-bottom: 14px; }
.topbar-actions { gap: 14px; }
.live-monitor, .current-date, .btn-primary { white-space: nowrap; }
.live-monitor { font-size: .74rem; letter-spacing: .6px; padding: 8px 14px; }
.current-date { font-size: .76rem; letter-spacing: .4px; }
.search-bar input { font-size: .88rem; }

.section-header h1, .dashboard-section h1 { font-size: 1.75rem; letter-spacing: -.3px; }
.card-panel h2 { font-size: 1.25rem; letter-spacing: -.2px; }
.subtitle { font-size: .66rem; letter-spacing: 1.6px; }
.desc { font-size: .86rem; }
.metric-card { padding: 22px; gap: 12px; }
.value { font-size: 2rem; letter-spacing: -.5px; }
.card-foot { font-size: .8rem; color: var(--text-muted); }
.tag { font-size: .7rem; font-weight: 700; white-space: nowrap; }
.stat-box .tag { align-self: flex-start; }
.stat-box h3 { font-size: 1.6rem; letter-spacing: -.3px; }
.shift-badge { font-size: .78rem; letter-spacing: .5px; white-space: nowrap; }
.esp-item strong { word-break: normal; overflow-wrap: anywhere; font-size: .95rem; }
.esp-item .label { font-size: .62rem; }
.data-table th { font-size: .66rem; letter-spacing: 1.2px; }
.data-table td { font-size: .86rem; }
.hm-msg { font-size: .86rem; line-height: 1.4; }
.hm-msg span i { font-size: 12px; vertical-align: -1px; margin-right: 2px; }
.lv-reabrir { display: inline-flex; align-items: center; gap: 6px; margin-left: 8px; padding: 4px 10px; border-radius: 8px; font-size: .74rem; font-weight: 700;
  background: rgba(255,42,75,.12); border: 1px solid rgba(255,42,75,.4); color: var(--primary-red); cursor: pointer; }

#lv-flutuante { position: fixed; right: 22px; bottom: 22px; z-index: 4000; display: none; align-items: center; gap: 10px; padding: 11px 16px;
  border-radius: 999px; background: #12141c; border: 1px solid rgba(255,42,75,.55); color: var(--text-main); font-size: .82rem; font-weight: 700;
  box-shadow: 0 10px 30px rgba(0,0,0,.45); cursor: pointer; }
#lv-flutuante.visivel { display: inline-flex; }
#lv-flutuante i { font-size: 18px; color: var(--primary-red); }
#lv-flutuante small { color: var(--text-muted); font-weight: 600; }
#lv-flutuante .ponto { width: 8px; height: 8px; border-radius: 50%; background: var(--text-sub); }
#lv-flutuante .ponto.vivo { background: #2ee59d; animation: lvPulso 1.2s infinite; }
@media print { .sidebar, .topbar, #lv-flutuante, #lv-modal { display: none !important; } body { height: auto; overflow: visible; display: block; } }

</style>
</head>
<body>
  <aside class="sidebar">
    <div class="brand">
      <div class="brand-icon"><i class="ph-fill ph-shield-warning"></i></div>
      <div class="brand-text">
        <h2>HELP ME</h2>
        <span>CENTRAL DE PROTEÇÃO</span>
      </div>
    </div>

    <nav class="menu">
      <p class="menu-label">OPERAÇÃO DA CENTRAL</p>
      <a href="#alerta-ativo" class="menu-item active" onclick="scrollToSection('alerta-ativo', this)">
        <i class="ph-bold ph-warning-octagon"></i> Ocorrência em Andamento
      </a>
      <button class="menu-item" id="menu-lv" onclick="lvAbrirPeloMenu()" title="Abrir o mapa com a localização da usuária">
        <i class="ph-bold ph-map-pin-area"></i> Localização ao vivo
        <span class="lv-menu-tag" id="menu-lv-tag">sem sinal</span>
      </button>
      <a href="#visao-geral" class="menu-item" onclick="scrollToSection('visao-geral', this)">
        <i class="ph-bold ph-squares-four"></i> Visão geral
        <span class="badge" id="active-badge">0</span>
      </a>
      <a href="#dispositivo" class="menu-item" onclick="scrollToSection('dispositivo', this)">
        <i class="ph-bold ph-cpu"></i> Dispositivo ESP32 (ao vivo)
      </a>
      <a href="#mapa-cobertura" class="menu-item" onclick="scrollToSection('mapa-cobertura', this)">
        <i class="ph-bold ph-map-pin"></i> Dispositivos na Região
      </a>
      <a href="#historico" class="menu-item" onclick="scrollToSection('historico', this)">
        <i class="ph-bold ph-clock-counter-clockwise"></i> Histórico de Alertas
      </a>
      <a href="#estatisticas" class="menu-item" onclick="scrollToSection('estatisticas', this)">
        <i class="ph-bold ph-chart-line-up"></i> Relatórios e Estatísticas
      </a>

      <p class="menu-label">GESTÃO E AÇÕES</p>
      <button class="menu-item btn-action" onclick="openModal('modal-usuarios')">
        <i class="ph-bold ph-users"></i> Usuários
      </button>
      <button class="menu-item btn-action" onclick="openModal('modal-cadastro')">
        <i class="ph-bold ph-user-plus"></i> Novo cadastro
      </button>
      <button class="menu-item btn-action highlight" onclick="triggerESP32Alert()">
        <i class="ph-bold ph-broadcast"></i> Simular botão do ESP32
      </button>
    </nav>

    <div class="sidebar-footer">
      <div class="system-status">
        <span class="status-dot green pulse" id="esp-dot"></span>
        <div>
          <strong id="esp-status-title">Conectando ao ESP32...</strong>
          <p id="esp-status-sub">Servidor HTTP na porta 80</p>
        </div>
      </div>
      <div class="user-profile">
        <div class="avatar">LM</div>
        <div class="user-info">
          <strong>Letícia Morais</strong>
          <p>Operadora da central</p>
        </div>
        <i class="ph-bold ph-caret-right"></i>
      </div>
    </div>
  </aside>

  <main class="main-content">
    <header class="topbar">
      <div class="search-area">
        <div class="search-bar">
          <i class="ph-bold ph-magnifying-glass"></i>
          <input
            type="text"
            id="global-search"
            placeholder="Buscar vítima por nome, telefone, CPF ou endereço..."
            oninput="filterTable()"
            autocomplete="off"
            aria-label="Buscar vítimas cadastradas"
          />

        </div>
        <div id="victim-search-results" class="victim-search-results"
             role="listbox" aria-label="Resultados da busca" hidden></div>
      </div>

      <div class="topbar-actions">
        <div class="live-monitor">
          <span class="status-dot pulse" id="top-dot"></span>
          <span id="top-status">CONECTANDO...</span>
        </div>
        <button class="icon-btn" title="Notificações">
          <i class="ph-bold ph-bell"></i>
          <span class="notification-badge" id="notif-badge">0</span>
        </button>
        <div class="current-date" id="current-date-display">
          <i class="ph-bold ph-calendar"></i>
          <span id="date-text">CARREGANDO DATA...</span>
        </div>
        <button class="btn-primary" onclick="alert('Relatório operacional compartilhado com as autoridades de Curitiba!')">
          <i class="ph-bold ph-share-network"></i> Compartilhar
        </button>
      </div>
    </header>

    <div class="content-scroll" id="scroll-container">
      <section id="alerta-ativo" class="dashboard-section">
        <div class="critical-banner" id="active-emergency-container">
          <div class="text-center" style="padding:40px;text-align:center">
            <i class="ph-bold ph-shield-check" style="font-size:52px;color:var(--accent-green);margin-bottom:12px"></i>
            <h2>Nenhuma Emergência Ativa no Momento</h2>
            <p class="desc" style="margin-top:8px">
              Aperte o botão <strong>"Simular Apertar Botão ESP32"</strong> no menu lateral para iniciar o fluxo de alerta.
            </p>
          </div>
        </div>
      </section>

      <section id="visao-geral" class="dashboard-section">
        <div class="section-header">
          <div>
            <p class="subtitle">PAINEL OPERACIONAL HELP ME • CURITIBA</p>
            <h1>Visão geral</h1>
            <p class="desc">Acompanhamento e coordenação das chamadas e botões de emergência física em tempo real.</p>
          </div>
          <div class="shift-badge">
            <i class="ph-bold ph-clock"></i>
            <span id="hm-relogio">HORA ATUAL: --:--:--</span>
          </div>
        </div>

        <div class="metrics-grid-2x2">
          <div class="metric-card">
            <div class="card-icon"><i class="ph-bold ph-bell-ringing"></i></div>
            <div class="card-data">
              <span class="label">OCORRÊNCIAS HOJE</span>
              <div class="value-group">
                <span class="value" id="metric-today-calls">--</span>
                <span class="tag neutral" id="metric-today-vs">--</span>
              </div>
              <p class="card-foot" id="metric-today-foot">carregando...</p>
            </div>
          </div>

          <div class="metric-card alert">
            <div class="card-icon"><i class="ph-bold ph-warning"></i></div>
            <div class="card-data">
              <span class="label">EMERGÊNCIAS ATIVAS</span>
              <div class="value-group">
                <span class="value" id="active-count">0</span>
                <span class="tag neutral" id="metric-active-prio">Nenhuma</span>
              </div>
              <p class="card-foot" id="metric-active-foot">Nenhum alerta em andamento</p>
            </div>
          </div>

          <div class="metric-card">
            <div class="card-icon"><i class="ph-bold ph-check-circle"></i></div>
            <div class="card-data">
              <span class="label">ATENDIMENTOS CONCLUÍDOS</span>
              <div class="value-group">
                <span class="value" id="metric-completed">--</span>
                <span class="tag neutral" id="metric-completed-pct">--</span>
              </div>
              <p class="card-foot" id="metric-completed-foot">carregando...</p>
            </div>
          </div>

          <div class="metric-card">
            <div class="card-icon"><i class="ph-bold ph-timer"></i></div>
            <div class="card-data">
              <span class="label">TEMPO MÉDIO DE RESPOSTA</span>
              <div class="value-group">
                <span class="value" id="metric-resp">--</span>
                <span class="tag neutral" id="metric-resp-vs">--</span>
              </div>
              <p class="card-foot">Do alerta até o atendimento concluído</p>
            </div>
          </div>
        </div>
      </section>

      <section id="dispositivo" class="dashboard-section">
        <div class="card-panel">
          <div class="panel-header">
            <div>
              <p class="subtitle">ESP32 • TEMPO REAL</p>
              <h2>Dispositivo físico</h2>
              <p class="desc">Dados lidos direto do ESP32 pela rede (rota /status), atualizados a cada 1 segundo.</p>
            </div>
          </div>
          <div class="esp-grid">
            <div class="esp-item"><span class="label">Estado do sistema</span><strong id="esp-estado">--</strong></div>
            <div class="esp-item"><span class="label">Conexão Wi-Fi</span><strong id="esp-conexao">--</strong></div>
            <div class="esp-item"><span class="label">Endereço IP</span><strong id="esp-ip">--</strong></div>
            <div class="esp-item"><span class="label">Endereço MAC</span><strong id="esp-mac">--</strong></div>
            <div class="esp-item"><span class="label">Rede (SSID)</span><strong id="esp-ssid">--</strong></div>
            <div class="esp-item"><span class="label">Sinal (RSSI)</span><strong id="esp-rssi">--</strong></div>
            <div class="esp-item"><span class="label">Ligado há</span><strong id="esp-uptime">--</strong></div>
            <div class="esp-item"><span class="label">Hora do ESP32</span><strong id="esp-hora">--</strong></div>
            <div class="esp-item"><span class="label">Microfone INMP441 (volume)</span><strong id="esp-volume">--</strong><div class="vol-bar"><span id="esp-vol-bar"></span></div></div>
            <div class="esp-item"><span class="label">Limite de som alto</span><strong id="esp-limite">--</strong></div>
            <div class="esp-item"><span class="label">Botão de emergência (D22)</span><strong id="esp-botao">--</strong></div>
            <div class="esp-item"><span class="label">Motor de vibração (D4)</span><strong id="esp-motor">--</strong></div>
            <div class="esp-item"><span class="label">Telegram da usuária</span><strong id="esp-telegram">--</strong></div>
            <div class="esp-item"><span class="label">Gravação de áudio</span><strong id="esp-gravacao">--</strong></div>
            <div class="esp-item"><span class="label">Memória flash</span><strong id="esp-flash">--</strong></div>
          </div>
          <div class="esp-actions">
            <button class="btn-primary" onclick="motorRede('on')"><i class="ph-bold ph-vibrate"></i> Ligar motor</button>
            <button class="btn-secondary" onclick="motorRede('off')">Desligar motor</button>
            <button class="btn-secondary" onclick="motorRede('pulso')">Pulso de 0,5 s</button>
            <input id="esp-limite-input" type="number" min="1000" step="1000" placeholder="novo limite" />
            <button class="btn-secondary" onclick="definirLimite()">Ajustar limite</button>
          </div>
        </div>
      </section>

      <section id="mapa-cobertura" class="dashboard-section">
        <div class="card-panel">
          <div class="panel-header">
            <div>
              <p class="subtitle">RASTREAMENTO EM TEMPO REAL</p>
              <h2>Dispositivos na região (Curitiba e Região Metropolitana)</h2>
              <p class="desc"><strong>Instrução:</strong> Clique em qualquer lugar do mapa para cadastrar o dispositivo informando o nome da protegida e o IP do aparelho.</p>
            </div>
          </div>
          <div id="real-map" class="map-container"></div>
        </div>
      </section>

      <section id="historico" class="dashboard-section">
        <div class="card-panel">
          <div class="panel-header">
            <div>
              <p class="subtitle">AUDITORIA OPERACIONAL</p>
              <h2>Histórico de alertas e atendimentos</h2>
            </div>
          </div>
          <div class="table-container">
            <table class="data-table" id="alerts-table">
              <thead>
                <tr>
                  <th>OCORRÊNCIA</th>
                  <th>PESSOA PROTEGIDA</th>
                  <th>DATA E HORA</th>
                  <th>DESFECHO</th>
                  <th>TEMPO DE RESPOSTA</th>
                  <th>AÇÃO</th>
                </tr>
              </thead>
              <tbody>
              </tbody>
            </table>
          </div>
        </div>
      </section>

      <section id="estatisticas" class="dashboard-section">
        <div class="card-panel">
          <div class="panel-header flex-header">
            <div>
              <p class="subtitle">ANÁLISE ANALÍTICA DINÂMICA</p>
              <h2>Estatísticas de Ocorrências e Desempenho</h2>
              <p class="desc">Calculado a partir do histórico real gravado na memória do ESP32 (últimos 7 e 28 dias).</p>
            </div>
            <div class="chart-toggle-group">
              <button class="btn-toggle active" id="btn-weekly" onclick="switchChartPeriod('weekly')">Visão Semanal</button>
              <button class="btn-toggle" id="btn-monthly" onclick="switchChartPeriod('monthly')">Visão Mensal</button>
            </div>
          </div>

          <div class="stats-summary-grid">
            <div class="stat-box">
              <span class="label">Total de Denúncias</span>
              <h3 id="stat-total-denuncias">--</h3>
              <p class="tag neutral-tag" id="stat-total-tag">--</p>
            </div>
            <div class="stat-box">
              <span class="label">Tempo Médio de Resposta</span>
              <h3 id="stat-avg-response">--</h3>
              <p class="tag neutral-tag" id="stat-avg-tag">--</p>
            </div>
            <div class="stat-box">
              <span class="label">Cancelamentos / Falsos Alarmes</span>
              <h3 id="stat-cancellations">--</h3>
              <p class="tag neutral-tag" id="stat-cancel-tag">--</p>
            </div>
          </div>

          <div class="dynamic-chart-container">
            <div class="chart-y-axis" id="chart-y-axis">
              <span>3</span><span>2</span><span>1</span><span>0</span>
            </div>
            <div class="chart-bars-area" id="chart-bars-wrapper"></div>
          </div>
          <div class="chart-x-axis" id="chart-x-labels"></div>
        </div>
      </section>

      <section id="log-sistema" class="dashboard-section hm-log">
        <div class="hm-log-head">
          <span>Log do sistema <em>memória flash · últimas 12 linhas</em></span>
          <a href="/log" target="_blank">ver completo</a>
        </div>
        <pre id="esp-log">carregando...</pre>
      </section>
    </div>
  </main>

  <div id="modal-telegram" class="modal-overlay">
    <div class="modal-container resizable-alert" id="telegram-alert-box">
      <div class="modal-header drag-handle" id="telegram-drag-handle" title="Clique e arraste para mover">
        <div>
          <span class="subtitle" style="color:var(--accent-yellow)"><i class="ph-bold ph-dots-six-vertical"></i> PRÉ-ALERTA • AGUARDANDO A USUÁRIA (ARRASTE AQUI)</span>
          <h2>Botão de Pânico!</h2>
        </div>
      </div>
      <div class="simulator-body">
        <div class="panic-button-graphic">
          <i class="ph-bold ph-telegram-logo"></i>
        </div>
        <p id="pre-alert-text">Sinal acionado. A usuária recebeu o aviso no Telegram e só pode cancelar com a senha.</p>
        <div class="countdown-box">
          Central notificada em
          <span id="countdown-timer">20</span>s
        </div>
      </div>
    </div>
  </div>

  <div id="modal-audio" class="modal-overlay">
    <div class="modal-container">
      <div class="modal-header">
        <div>
          <span class="subtitle">PROVA EM ÁUDIO • MICROFONE I2S</span>
          <h2 id="audio-modal-title">Áudio</h2>
          <p class="desc" id="audio-modal-info"></p>
        </div>
        <button type="button" class="btn-close" aria-label="Fechar" onclick="fecharAudio()">&times;</button>
      </div>
      <audio id="audio-modal-player" controls style="width:100%;margin-top:14px"></audio>
    </div>
  </div>

  <div id="modal-usuarios" class="modal-overlay">
    <div class="modal-container users-modal-container">
      <div class="modal-header">
        <div>
          <span class="subtitle">SISTEMA DE PROTEÇÃO</span>
          <h2>Usuárias Cadastradas</h2>
          <p class="desc">Consulte, cadastre e remova registros salvos na base do sistema.</p>
        </div>
        <button type="button" class="btn-close" aria-label="Fechar" onclick="closeModal('modal-usuarios')">&times;</button>
      </div>

      <div class="users-toolbar">
        <button class="btn-primary" onclick="closeModal('modal-usuarios'); openModal('modal-cadastro')">
          <i class="ph-bold ph-user-plus"></i> Cadastrar Nova Usuária
        </button>
      </div>

      <div class="table-container users-table-wrapper">
        <table class="data-table" id="users-table">
          <thead>
            <tr>
              <th>NOME</th>
              <th>CPF</th>
              <th>TELEFONE</th>
              <th>ENDEREÇO</th>
              <th>AÇÕES</th>
            </tr>
          </thead>
          <tbody id="users-table-body">

          </tbody>
        </table>
      </div>
    </div>
  </div>

  <div id="modal-cadastro" class="modal-overlay">
    <form class="modal-container victim-form" id="victim-form" onsubmit="saveVictim(event)">
      <div class="modal-header">
        <div>
          <span class="subtitle">BASE DE DADOS LOCAL</span>
          <h2>Cadastrar vítima</h2>
          <p class="desc">Os dados ficam salvos neste navegador para consulta posterior.</p>
        </div>
        <button type="button" class="btn-close" aria-label="Fechar cadastro" onclick="closeModal('modal-cadastro')">&times;</button>
      </div>

      <label>Nome completo
        <input name="name" type="text" required maxlength="120" placeholder="Nome da vítima" />
      </label>
      <label>Telefone
        <input name="phone" type="tel" required maxlength="30" placeholder="(41) 99999-9999" />
      </label>
      <label>CPF
        <input name="cpf" type="text" required maxlength="20" placeholder="CPF (fictício para demonstração)" />
      </label>
      <label>Endereço completo
        <textarea name="address" required maxlength="250" rows="2" placeholder="Rua, número, complemento, bairro e cidade"></textarea>
      </label>

      <div class="form-actions">
        <button type="button" class="btn-secondary" onclick="closeModal('modal-cadastro')">Cancelar</button>
        <button type="submit" class="btn-primary">Salvar cadastro</button>
      </div>
      <p id="victim-form-message" class="form-message" aria-live="polite"></p>
    </form>
  </div>

  <script>
let map;
let countdownInterval = null;
let currentVictimIndex = 0;
let activeEmergenciesCount = 0;
let currentPeriod = 'weekly';
let userMarkers = [];

const victimsPool = [
  {
    code: "OC-2408",
    name: "Mariana Souza",
    address: "Rua XV de Novembro, 1200 - Apto 42 • Centro, Curitiba - PR",
    age: "34 anos",
    cpf: "***.482.178-**",
    phone: "(41) 98842-7621",
    device: "187.54.32.118",
    agressor: "Carlos Eduardo Souza",
    history: "Possui histórico de agressão reincidente — 2 registros policiais ativos.",
    coords: [-25.4284, -49.2733]
  }
];

let statsData = {
  weekly: {
    labels: ['Seg', 'Ter', 'Qua', 'Qui', 'Sex', 'Sáb', 'Dom'],
    values: [0, 0, 0, 0, 0, 0, 0],
    total: 0,
    avgResponse: '--',
    cancellations: 0
  },
  monthly: {
    labels: ['Semana 1', 'Semana 2', 'Semana 3', 'Semana 4'],
    values: [0, 0, 0, 0],
    total: 0,
    avgResponse: '--',
    cancellations: 0
  }
};

const VICTIMS_STORAGE_KEY = 'helpMeVictimsDatabase';
let victimsDatabase = [];

document.addEventListener("DOMContentLoaded", () => {
  updateHeaderDate();
  loadVictimsDatabase();
  initMap();
  renderChart();
  makeTelegramModalDraggable();
});

function updateHeaderDate() {
  const dateElement = document.getElementById('date-text');
  if (!dateElement) return;

  const now = new Date();
  const days = ['DOMINGO', 'SEGUNDA-FEIRA', 'TERÇA-FEIRA', 'QUARTA-FEIRA', 'QUINTA-FEIRA', 'SEXTA-FEIRA', 'SÁBADO'];
  const months = ['JAN', 'FEV', 'MAR', 'ABR', 'MAI', 'JUN', 'JUL', 'AGO', 'SET', 'OUT', 'NOV', 'DEZ'];

  const dayName = days[now.getDay()];
  const dayNum = String(now.getDate()).padStart(2, '0');
  const monthName = months[now.getMonth()];
  const year = now.getFullYear();

  dateElement.textContent = `${dayName}, ${dayNum} ${monthName} • ${year}`;
}

function makeTelegramModalDraggable() {
  const box = document.getElementById('telegram-alert-box');
  const handle = document.getElementById('telegram-drag-handle');
  if (!box || !handle) return;

  let isDragging = false;
  let offsetX = 0;
  let offsetY = 0;

  handle.addEventListener('mousedown', (e) => {
    isDragging = true;
    offsetX = e.clientX - box.getBoundingClientRect().left;
    offsetY = e.clientY - box.getBoundingClientRect().top;
    document.body.style.userSelect = 'none';
  });

  document.addEventListener('mousemove', (e) => {
    if (!isDragging) return;
    box.style.left = `${e.clientX - offsetX}px`;
    box.style.top = `${e.clientY - offsetY}px`;
    box.style.right = 'auto';
  });

  document.addEventListener('mouseup', () => {
    isDragging = false;
    document.body.style.userSelect = 'default';
  });
}

function loadVictimsDatabase() {
  const seed = victimsPool.map(v => ({
    code: v.code,
    name: v.name,
    address: v.address,
    age: v.age,
    cpf: v.cpf,
    phone: v.phone,
    device: v.device,
    agressor: v.agressor,
    history: v.history,
    coords: v.coords
  }));

  try {
    const saved = localStorage.getItem(VICTIMS_STORAGE_KEY);
    victimsDatabase = saved ? JSON.parse(saved) : seed;

    if (!Array.isArray(victimsDatabase)) {
      victimsDatabase = seed;
    }
  } catch (error) {
    victimsDatabase = seed;
  }

  if (!victimsDatabase.some(v => v.name === 'Mariana Souza')) {
    victimsDatabase.unshift(seed[0]);
  }

  persistVictimsDatabase();
  renderVictimSearchResults('');
  renderUsersTable();
}

function renderUsersTable() {
  const tableBody = document.getElementById('users-table-body');
  if (!tableBody) return;

  tableBody.innerHTML = '';

  if (victimsDatabase.length === 0) {
    tableBody.innerHTML = `<tr><td colspan="5" style="text-align:center;color:var(--text-muted)">Nenhuma usuária cadastrada no sistema.</td></tr>`;
    return;
  }

  victimsDatabase.forEach((v, index) => {
    const tr = document.createElement('tr');
    tr.innerHTML = `
      <td><strong>${v.name}</strong></td>
      <td>${v.cpf || 'Não informado'}</td>
      <td>${v.phone || 'Não informado'}</td>
      <td><small style="color:var(--text-muted)">${v.address || 'Não informado'}</small></td>
      <td>
        <button class="btn-danger-sm" onclick="deleteVictim(${index})">
          <i class="ph-bold ph-trash"></i> Apagar
        </button>
      </td>
    `;
    tableBody.appendChild(tr);
  });
}

function deleteVictim(index) {
  const v = victimsDatabase[index];
  if (!v) return;

  if (confirm(`Tem certeza que deseja apagar o registro de "${v.name}"?`)) {
    victimsDatabase.splice(index, 1);
    persistVictimsDatabase();
    renderUsersTable();
    filterTable();
  }
}

function persistVictimsDatabase() {
  try {
    localStorage.setItem(
      VICTIMS_STORAGE_KEY,
      JSON.stringify(victimsDatabase)
    );
  } catch (error) {
    console.warn(
      'Não foi possível salvar a base local neste navegador.',
      error
    );
  }
}

function normalizeSearch(value) {
  return String(value || '')
    .normalize('NFD')
    .replace(/[\u0300-\u036f]/g, '')
    .toLowerCase()
    .trim();
}

function filterTable() {
  const input = document.getElementById('global-search');

  renderVictimSearchResults(input ? input.value : '');
}

function renderVictimSearchResults(query) {
  const panel = document.getElementById('victim-search-results');

  if (!panel) return;

  const normalized = normalizeSearch(query);

  const results = victimsDatabase.filter(v => {
    const searchable = [
      v.name,
      v.phone,
      v.cpf,
      v.address
    ].map(normalizeSearch).join(' ');

    return !normalized || searchable.includes(normalized);
  }).slice(0, 8);

  panel.innerHTML = '';

  if (normalized && results.length === 0) {
    const empty = document.createElement('div');
    empty.className = 'search-empty';
    empty.textContent = 'Nenhuma vítima encontrada na base local.';

    panel.appendChild(empty);
    panel.hidden = false;
    return;
  }

  if (!normalized) {
    panel.hidden = true;
    return;
  }

  results.forEach(v => {
    const item = document.createElement('button');

    item.type = 'button';
    item.className = 'victim-search-result';
    item.setAttribute('role', 'option');

    const name = document.createElement('strong');
    name.textContent = v.name;

    const phone = document.createElement('span');
    phone.textContent =
      `Telefone: ${v.phone || 'Não informado'} • CPF: ${v.cpf || 'Não informado'}`;

    const address = document.createElement('small');
    address.textContent = v.address || 'Endereço não informado';

    item.append(name, phone, address);

    item.addEventListener('click', () => {
      document.getElementById('global-search').value = v.name;
      panel.hidden = true;
      showVictimDetails(v);
    });

    panel.appendChild(item);
  });

  panel.hidden = false;
}

function showVictimDetails(victim) {
  const details =
    `Nome: ${victim.name}\n` +
    `Telefone: ${victim.phone || 'Não informado'}\n` +
    `CPF: ${victim.cpf || 'Não informado'}\n` +
    `Endereço: ${victim.address || 'Não informado'}`;

  alert(details);
}

function saveVictim(event) {
  event.preventDefault();

  const form = event.currentTarget;
  const formData = new FormData(form);

  const name = String(formData.get('name') || '').trim();
  const phone = String(formData.get('phone') || '').trim();
  const cpf = String(formData.get('cpf') || '').trim();
  const address = String(formData.get('address') || '').trim();

  const message = document.getElementById('victim-form-message');

  if (!name || !phone || !cpf || !address) {
    message.textContent =
      'Preencha todos os campos para salvar o cadastro.';
    return;
  }

  const cpfDigits = cpf.replace(/\D/g, '');

  const duplicate = victimsDatabase.some(v =>
    v.cpf &&
    v.cpf.replace(/\D/g, '') === cpfDigits &&
    cpfDigits.length > 0
  );

  if (duplicate) {
    message.textContent =
      'Já existe um cadastro com esse CPF na base local.';
    return;
  }

  const newVictim = {
    code: `CAD-${String(Date.now()).slice(-6)}`,
    name,
    phone,
    cpf,
    address,
    age: 'Não informado',
    device: 'Não vinculado',
    agressor: 'Não informado',
    history: 'Cadastro incluído pela central.',
    coords: null
  };

  victimsDatabase.push(newVictim);

  persistVictimsDatabase();
  renderUsersTable();

  message.textContent =
    'Cadastro salvo com sucesso neste navegador.';

  form.reset();
  filterTable();

  setTimeout(() => {
    closeModal('modal-cadastro');
    message.textContent = '';
  }, 900);
}

document.addEventListener('click', event => {
  const area = document.querySelector('.search-area');
  const panel = document.getElementById('victim-search-results');

  if (area && panel && !area.contains(event.target)) {
    panel.hidden = true;
  }
});

function initMap() {
  map = L.map('real-map', {
    zoomControl: true,
    dragging: true,
    scrollWheelZoom: true
  }).setView([-25.4284, -49.2733], 13);

  L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
    attribution: '&copy; OpenStreetMap contributors',
    maxZoom: 19,
    minZoom: 10
  }).addTo(map);

  setTimeout(() => {
    map.invalidateSize();
  }, 200);

  map.on('click', function(e) {
    const lat = e.latlng.lat;
    const lng = e.latlng.lng;

    const popupContent = document.createElement('div');

    popupContent.style.color = '#000';
    popupContent.style.padding = '8px';

    popupContent.innerHTML = `
      <h3 style="margin-bottom:8px;font-size:1rem;color:#111">
        Cadastrar Dispositivo
      </h3>

      <div style="margin-bottom:6px">
        <label style="font-size:.8rem;font-weight:bold;display:block">
          Nome da Protegida:
        </label>
        <input
          type="text"
          id="input-nome-mulher"
          placeholder="Ex: Ana Paula"
          style="width:100%;padding:6px;border:1px solid #ccc;border-radius:4px"
        />
      </div>

      <div style="margin-bottom:10px">
        <label style="font-size:.8rem;font-weight:bold;display:block">
          IP do Aparelho (ESP32):
        </label>
        <input
          type="text"
          id="input-ip-aparelho"
          placeholder="Ex: 187.54.32.119"
          style="width:100%;padding:6px;border:1px solid #ccc;border-radius:4px"
        />
      </div>

      <button
        id="btn-salvar-marcador"
        style="background:#3b82f6;color:#fff;border:none;padding:8px 14px;border-radius:6px;cursor:pointer;font-weight:bold;width:100%"
      >
        Salvar Marcador
      </button>
    `;

    L.DomEvent.disableClickPropagation(popupContent);
    L.DomEvent.disableScrollPropagation(popupContent);

    const tempMarker = L.marker([lat, lng]).addTo(map);

    tempMarker.bindPopup(popupContent).openPopup();

    setTimeout(() => {
      const btnSalvar = document.getElementById('btn-salvar-marcador');

      if (btnSalvar) {
        btnSalvar.onclick = function() {
          const nomeMulher =
            document.getElementById('input-nome-mulher').value.trim();

          const ipAparelho =
            document.getElementById('input-ip-aparelho').value.trim();

          if (!nomeMulher || !ipAparelho) {
            alert('Por favor, preencha o nome da mulher e o IP do aparelho!');
            return;
          }

          map.removeLayer(tempMarker);

          criarMarcadorDefinitivo(
            lat,
            lng,
            nomeMulher,
            ipAparelho
          );
        };
      }
    }, 100);
  });
}

function criarMarcadorDefinitivo(lat, lng, nome, ip) {
  const randomColor = '#' +
    Math.floor(Math.random() * 16777215)
      .toString(16)
      .padStart(6, '0');

  const customPinIcon = L.divIcon({
    className: 'custom-colored-pin',
    html: `
      <div style="width:32px;height:32px;display:flex;align-items:center;justify-content:center;filter:drop-shadow(0 4px 6px rgba(0,0,0,.4))">
        <svg viewBox="0 0 24 24" width="32" height="32" fill="${randomColor}" xmlns="http://www.w3.org/2000/svg">
          <path d="M12 0C7.58 0 4 3.58 4 8c0 5.25 8 16 8 16s8-10.75 8-16c0-4.42-3.58-8-8-8zm0 11c-1.66 0-3-1.34-3-3s1.34-3 3-3 3 1.34 3 3-1.34 3-3 3z"/>
        </svg>
      </div>
    `,
    iconSize: [32, 32],
    iconAnchor: [16, 32],
    popupAnchor: [0, -32]
  });

  const markerIndex = userMarkers.length;

  const marker = L.marker(
    [lat, lng],
    { icon: customPinIcon }
  ).addTo(map);

  const popup = document.createElement('div');

  popup.style.cssText =
    'text-align:left;color:#000;padding:4px;min-width:180px';

  const title = document.createElement('b');
  title.textContent = nome;
  title.style.cssText = 'font-size:1.05rem;color:#111';

  const ipText = document.createElement('div');
  ipText.textContent = `IP: ${ip}`;

  const coordinates = document.createElement('div');
  coordinates.textContent =
    `Lat: ${lat.toFixed(4)}, Lng: ${lng.toFixed(4)}`;

  const deleteButton = document.createElement('button');
  deleteButton.id = `btn-excluir-ini-${markerIndex}`;
  deleteButton.textContent = 'Excluir Marcador';
  deleteButton.style.cssText =
    'background:#ff2a4b;color:#fff;border:none;padding:6px 12px;border-radius:6px;cursor:pointer;font-weight:bold;width:100%;margin-top:10px';

  popup.append(title, document.createElement('br'), ipText, coordinates, deleteButton);

  L.DomEvent.disableClickPropagation(popup);
  L.DomEvent.disableScrollPropagation(popup);

  marker.bindPopup(popup);

  userMarkers.push({
    marker,
    index: markerIndex
  });

  marker.on('popupopen', () => {
    const button = document.getElementById(
      `btn-excluir-ini-${markerIndex}`
    );

    if (button) {
      button.onclick = function(event) {
        L.DomEvent.stopPropagation(event);
        window.confirmarExclusao(markerIndex);
      };
    }
  });
}

window.confirmarExclusao = function(index) {
  const item = userMarkers.find(
    m => m && m.index === index
  );

  if (!item) return;

  const confirmation = document.createElement('div');

  confirmation.style.cssText =
    'color:#000;padding:6px;min-width:180px';

  const message = document.createElement('p');
  message.textContent =
    'Tem certeza que deseja excluir este marcador?';

  message.style.cssText =
    'font-weight:bold;margin-bottom:10px;font-size:.95rem;color:#d90429';

  const buttons = document.createElement('div');
  buttons.style.cssText = 'display:flex;gap:8px';

  const yesButton = document.createElement('button');
  yesButton.textContent = 'Sim, excluir';
  yesButton.style.cssText =
    'flex:1;background:#ff2a4b;color:#fff;border:none;padding:6px;border-radius:4px;cursor:pointer;font-weight:bold';

  const noButton = document.createElement('button');
  noButton.textContent = 'Não, retornar';
  noButton.style.cssText =
    'flex:1;background:#6c757d;color:#fff;border:none;padding:6px;border-radius:4px;cursor:pointer;font-weight:bold';

  buttons.append(yesButton, noButton);
  confirmation.append(message, buttons);

  L.DomEvent.disableClickPropagation(confirmation);
  L.DomEvent.disableScrollPropagation(confirmation);

  item.marker.bindPopup(confirmation).openPopup();

  yesButton.onclick = function(event) {
    L.DomEvent.stopPropagation(event);

    map.removeLayer(item.marker);

    const arrayPos = userMarkers.findIndex(
      m => m && m.index === index
    );

    if (arrayPos !== -1) {
      userMarkers[arrayPos] = null;
    }

    map.closePopup();
  };

  noButton.onclick = function(event) {
    L.DomEvent.stopPropagation(event);
    map.closePopup();
  };
};

function triggerESP32Alert() {
  currentVictimIndex =
    (currentVictimIndex + 1) % victimsPool.length;

  openModal('modal-telegram');

  let timeLeft = 20;

  document.getElementById('countdown-timer').textContent = timeLeft;

  if (countdownInterval) {
    clearInterval(countdownInterval);
  }

  countdownInterval = setInterval(() => {
    timeLeft--;

    document.getElementById('countdown-timer').textContent =
      timeLeft;

    if (timeLeft <= 0) {
      clearInterval(countdownInterval);
      countdownInterval = null;

      closeModal('modal-telegram');
      activateRealEmergency();
      incrementChartData();
    }
  }, 1000);
}

function activateRealEmergency() {
  activeEmergenciesCount = 1;

  const v = victimsDatabase.find(
    item => item.name === victimsPool[0].name
  ) || victimsPool[0];

  const now = new Date();
  const timeString = now.toTimeString().split(' ')[0];

  const container = document.getElementById(
    'active-emergency-container'
  );

  container.style.border = '1px solid var(--primary-red)';
  container.style.boxShadow =
    '0 0 35px rgba(255,42,75,.18)';

  container.innerHTML = `
    <div class="alert-header-status">
      <span class="pulse-icon">
        <i class="ph-fill ph-warning-octagon"></i>
      </span>
      <div>
        <h3>EMERGÊNCIA CRÍTICA EM ANDAMENTO — OCORRÊNCIA ${v.code}</h3>
        <p>Alerta enviado e ativo na central • GPS Ativo com áudio I2S disponível</p>
      </div>
      <span class="badge-priority">PRIORIDADE MÁXIMA</span>
    </div>

    <div class="victim-profile-grid">
      <div class="victim-photo-card">
        <div class="avatar-large">
          <i class="ph-bold ph-user"></i>
        </div>
        <div class="victim-main">
          <h2>${v.name}</h2>
          <p class="address">
            <i class="ph-bold ph-map-pin"></i> ${v.address}
          </p>
          <div class="time-callout">
            <span>ALERTA RECEBIDO ÀS</span>
            <strong>${timeString}</strong>
          </div>
        </div>
      </div>

      <div class="audio-proof-box">
        <div class="audio-header">
          <i class="ph-bold ph-waveform"></i>
          <div>
            <span class="label">GRAVAÇÃO DE ÁUDIO COMO PROVA (MICROFONE I2S)</span>
            <p>Áudio capturado em tempo real pelo dispositivo físico no momento do aperto</p>
          </div>
        </div>
        <div class="audio-player-mock">
          <button class="play-btn" onclick="alert('Reproduzindo áudio gravado...')">
            <i class="ph-fill ph-play"></i>
          </button>
          <div class="waveform-visual">
            <span style="height:14px"></span>
            <span style="height:28px"></span>
            <span style="height:20px"></span>
            <span style="height:35px"></span>
            <span style="height:18px"></span>
            <span style="height:30px"></span>
            <span style="height:22px"></span>
            <span style="height:38px"></span>
          </div>
          <span class="audio-time">00:05 / 00:45</span>
        </div>
      </div>

      <div class="action-buttons-group">
        <h4><i class="ph-bold ph-siren"></i> ACIONAMENTO DIRETO DAS FORÇAS</h4>

        <div class="btn-dual">
          <button class="btn-police" onclick="triggerDispatch('Polícia Militar do Paraná - 190')">
            <i class="ph-fill ph-shield"></i> Acionar PM-PR (190)
          </button>
          <button class="btn-samu" onclick="triggerDispatch('SAMU Curitiba - 192')">
            <i class="ph-fill ph-first-aid-kit"></i> Acionar SAMU (192)
          </button>
        </div>

        <button class="btn-confirm-success" onclick="resolveEmergency()">
          <i class="ph-bold ph-check-circle"></i>
          CONFIRMAR ATENDIMENTO REALIZADO COM SUCESSO
        </button>

        <button class="btn-cancel-operator" onclick="cancelEmergencyByOperator()">
          <i class="ph-bold ph-x-circle"></i>
          CANCELAR ALERTA NA CENTRAL (FALSO ALARME)
        </button>
      </div>
    </div>
  `;

  document.getElementById('active-count').textContent =
    activeEmergenciesCount;

  document.getElementById('active-badge').textContent =
    activeEmergenciesCount;

  document.getElementById('notif-badge').textContent =
    activeEmergenciesCount;

  map.setView(v.coords, 15);

  const alertIcon = L.divIcon({
    className: 'custom-pin-red',
    html: '<div style="background:#ff2a4b;width:18px;height:18px;border-radius:50%;border:3px solid #fff;box-shadow:0 0 12px #ff2a4b"></div>'
  });

  L.marker(v.coords, { icon: alertIcon })
    .addTo(map)
    .bindPopup(`<b>${v.name} (EMERGÊNCIA CONFIRMADA)</b>`)
    .openPopup();

  document.getElementById('alerta-ativo')
    .scrollIntoView({ behavior: 'smooth' });
}

function cancelEmergencyByOperator() {
  if (!confirm(
    'Deseja cancelar este alerta ativo diretamente na central após a análise da gravação? O chamado será descartado sem ir para o histórico.'
  )) {
    return;
  }

  activeEmergenciesCount = 0;

  document.getElementById('active-count').textContent = 0;
  document.getElementById('active-badge').textContent = 0;
  document.getElementById('notif-badge').textContent = 0;

  const emergencyCard = document.getElementById(
    'active-emergency-container'
  );

  if (emergencyCard) {
    emergencyCard.innerHTML = `
      <div class="text-center" style="padding:40px;text-align:center">
        <i class="ph-bold ph-info" style="font-size:52px;color:var(--accent-yellow);margin-bottom:12px"></i>
        <h2>Alerta Cancelado pela Central</h2>
        <p class="desc" style="margin-top:8px">
          A operadora Letícia Morais cancelou o chamado ativo após verificação.
          A ocorrência foi descartada com sucesso sem registro no histórico.
        </p>
      </div>
    `;

    emergencyCard.style.border =
      '1px solid var(--accent-yellow)';

    emergencyCard.style.boxShadow = 'none';
  }

  alert('Alerta cancelado com sucesso pela central de controle!');
}

function switchChartPeriod(period) {
  currentPeriod = period;

  document.getElementById('btn-weekly')
    .classList.toggle('active', period === 'weekly');

  document.getElementById('btn-monthly')
    .classList.toggle('active', period === 'monthly');

  renderChart();
}

function renderChart() {
  const data = statsData[currentPeriod];

  document.getElementById('stat-total-denuncias').textContent =
    data.total;

  document.getElementById('stat-avg-response').textContent =
    data.avgResponse;

  document.getElementById('stat-cancellations').textContent =
    data.cancellations;

  const wrapper = document.getElementById('chart-bars-wrapper');
  const xLabels = document.getElementById('chart-x-labels');

  if (!wrapper || !xLabels) return;

  wrapper.innerHTML = '';
  xLabels.innerHTML = '';

  const maxVal = Math.max(...data.values) * 1.2 || 10;

  data.values.forEach((val, index) => {
    const heightPercent = (val / maxVal) * 100;

    const col = document.createElement('div');
    col.className = 'chart-bar-column';

    const bar = document.createElement('div');
    bar.className = 'chart-bar';
    bar.style.height = `${heightPercent}%`;
    bar.title = `${val} ocorrências`;

    col.appendChild(bar);
    wrapper.appendChild(col);

    const labelSpan = document.createElement('span');
    labelSpan.textContent = data.labels[index];

    xLabels.appendChild(labelSpan);
  });
}

function incrementChartData() {
  statsData.weekly.values[
    statsData.weekly.values.length - 1
  ]++;

  statsData.weekly.total++;

  statsData.monthly.values[
    statsData.monthly.values.length - 1
  ]++;

  statsData.monthly.total++;

  renderChart();
}

function scrollToSection(sectionId, element) {
  const target = document.getElementById(sectionId);

  if (target) {
    target.scrollIntoView({ behavior: 'smooth' });
  }

  document.querySelectorAll('.menu-item').forEach(item => {
    item.classList.remove('active');
  });

  if (element) {
    element.classList.add('active');
  }
}

function openModal(modalId) {
  const modal = document.getElementById(modalId);

  if (modal) {
    modal.style.display = 'flex';
  }
}

function closeModal(modalId) {
  const modal = document.getElementById(modalId);

  if (modal) {
    modal.style.display = 'none';
  }
}

function triggerDispatch(forceName) {
  alert(`${forceName} notificado(a) com sucesso em Curitiba!`);
}

function resolveEmergency() {
  if (!confirm(
    'Deseja confirmar o atendimento realizado com sucesso? A ocorrência será movida para o histórico.'
  )) {
    return;
  }

  const v = victimsPool[0];
  const tableBody = document.querySelector('#alerts-table tbody');

  const newRow = document.createElement('tr');

  newRow.innerHTML = `
    <td><strong>${v.code}</strong><br><small class="text-red">Prioridade Máxima</small></td>
    <td>${v.name}</td>
    <td>${new Date().toLocaleString('pt-BR')}</td>
    <td><span class="status-pill success"><i class="ph-bold ph-check"></i> Concluído</span></td>
    <td>3m 45s</td>
    <td><button class="btn-sm" onclick="alert('Reproduzindo áudio gravado...')">Ver Áudio</button></td>
  `;

  tableBody.prepend(newRow);

  activeEmergenciesCount = 0;

  document.getElementById('active-count').textContent = 0;
  document.getElementById('active-badge').textContent = 0;
  document.getElementById('notif-badge').textContent = 0;

  const emergencyCard = document.getElementById(
    'active-emergency-container'
  );

  if (emergencyCard) {
    emergencyCard.innerHTML = `
      <div class="text-center" style="padding:40px;text-align:center">
        <i class="ph-bold ph-check-circle" style="font-size:52px;color:var(--accent-green);margin-bottom:12px"></i>
        <h2>Nenhuma Emergência Ativa no Momento</h2>
        <p class="desc" style="margin-top:8px">
          Aperte o botão <strong>"Simular Apertar Botão ESP32"</strong>
          para iniciar o fluxo de alerta.
        </p>
      </div>
    `;

    emergencyCard.style.border =
      '1px solid var(--border-color)';

    emergencyCard.style.boxShadow = 'none';
  }

  alert('Atendimento confirmado e ocorrência movida para o Histórico!');
}
</script>
<script>

const ESP = location.protocol.startsWith('http') ? '' : 'http://172.20.10.12';
let espEstadoAnterior = 'normal';
let gravandoAnterior = false;
let ocorrenciaExibida = null;
let chatVisto = -1;
let ultimaLat = 0;
let espOnline = null;
let marcadorEmergencia = null;
let pollOcupado = false;
let ultimoStatus = null;

async function espGet(rota) {
  const r = await fetch(ESP + rota, { cache: 'no-store' });
  const t = await r.text();
  try { return JSON.parse(t); } catch (e) { return t; }
}
const esc = (t) => String(t ?? '').replace(/[&<>"]/g, c => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;' }[c]));

function idbAbrir() {
  return new Promise((res, rej) => {
    const r = indexedDB.open('helpme-audios', 1);
    r.onupgradeneeded = () => r.result.createObjectStore('audios');
    r.onsuccess = () => res(r.result);
    r.onerror = () => rej(r.error);
  });
}
async function idbPut(k, v) {
  const db = await idbAbrir();
  return new Promise((res, rej) => { const tx = db.transaction('audios', 'readwrite'); tx.objectStore('audios').put(v, k); tx.oncomplete = res; tx.onerror = () => rej(tx.error); });
}
async function idbGet(k) {
  const db = await idbAbrir();
  return new Promise((res, rej) => { const q = db.transaction('audios').objectStore('audios').get(k); q.onsuccess = () => res(q.result); q.onerror = () => rej(q.error); });
}
async function guardarAudio(oc) {
  try {
    const r = await fetch(ESP + '/audio?oc=' + encodeURIComponent(oc), { cache: 'no-store' });
    if (!r.ok) return;
    const blob = await r.blob();
    if (blob.size > 44) await idbPut(oc, blob);
  } catch (e) {  }
}

function triggerESP32Alert() {
  espGet('/simular')
    .then(r => { if (r && r.ok === false) alert('Já existe um alerta em andamento no dispositivo.'); pollESP(); })
    .catch(() => alert('Não foi possível falar com o ESP32.'));
}
function motorRede(estado) { espGet('/atuador?estado=' + estado).then(pollESP).catch(() => alert('Não foi possível falar com o ESP32.')); }
function definirLimite() {
  const v = document.getElementById('esp-limite-input').value;
  if (v) espGet('/limite?valor=' + encodeURIComponent(v)).then(pollESP).catch(() => {});
}
function gravarMais() { espGet('/gravar_mais').then(pollESP).catch(() => alert('Não foi possível falar com o ESP32.')); }
function atualizarAudio() {
  const a = document.getElementById('emerg-audio');
  if (a && ocorrenciaExibida) { a.src = ESP + '/audio?oc=' + encodeURIComponent(ocorrenciaExibida) + '&t=' + Date.now(); a.load(); }
}
async function enviarChat(ev) {
  if (ev) ev.preventDefault();
  const inp = document.getElementById('chat-input');
  const texto = inp.value.trim();
  if (!texto) return;
  const r = await espGet('/chat/enviar?texto=' + encodeURIComponent(texto)).catch(() => null);
  if (r && r.ok) { inp.value = ''; carregarChat(); }
  else alert(r && r.erro ? 'Não enviado: ' + r.erro : 'Não foi possível falar com o ESP32.');
}

function resolveEmergency() {
  if (!confirm('Confirmar o atendimento realizado com sucesso? A ocorrência vai para o histórico.')) return;
  const oc = ocorrenciaExibida;
  espGet('/resolver').then(async () => {
    ocorrenciaExibida = null;
    mostrarSemEmergencia();
    if (oc) await guardarAudio(oc);
    carregarHistorico();
    alert('Atendimento confirmado e ocorrência movida para o Histórico!');
  }).catch(() => alert('Não foi possível falar com o ESP32.'));
}
function cancelEmergencyByOperator() {
  if (!confirm('Cancelar este alerta como FALSO ALARME? A ocorrência e o áudio serão descartados.')) return;
  espGet('/falso').then(() => {
    ocorrenciaExibida = null;
    zerarContadores(); removerMarcador();
    cardMensagem('ph-info', 'var(--accent-yellow)', 'Alerta Cancelado pela Central',
      'A central cancelou o chamado após verificação (falso alarme). A ocorrência não foi para o histórico.');
    somarCancelamento();
    alert('Alerta cancelado com sucesso pela central de controle!');
  }).catch(() => alert('Não foi possível falar com o ESP32.'));
}

async function pollESP() {
  if (pollOcupado) return;
  pollOcupado = true;
  try {
    const s = await espGet('/status');
    ultimoStatus = s;
    setEspOnline(true);
    atualizarCardDispositivo(s);

    if (s.estado === 'contagem') {
      openModal('modal-telegram');
      document.getElementById('countdown-timer').textContent = s.restante_s;
      const p = document.getElementById('pre-alert-text');
      if (p) p.textContent = 'Ocorrência ' + s.ocorrencia + ' (' + s.origem + '). Aguardando a usuária no Telegram: ela só cancela com a senha.';
    } else if (espEstadoAnterior === 'contagem') {
      closeModal('modal-telegram');
      if (s.estado === 'normal' && s.ultimo_desfecho === 'cancelado_usuaria') mostrarCanceladoPelaUsuaria();
    }

    if (s.estado === 'emergencia') {
      if (ocorrenciaExibida !== s.ocorrencia) {
        ocorrenciaExibida = s.ocorrencia; chatVisto = -1; ultimaLat = 0;
        mostrarEmergencia(s);
        incrementChartData();
      }
      atualizarEmergencia(s);
      if (s.chat_n !== chatVisto) { chatVisto = s.chat_n; carregarChat(); }
      if (gravandoAnterior && !s.gravando) guardarAudio(s.ocorrencia);
    }
    if (s.estado === 'normal' && ocorrenciaExibida) {
      ocorrenciaExibida = null;
      mostrarSemEmergencia();
      carregarHistorico();
    }
    gravandoAnterior = s.gravando;
    espEstadoAnterior = s.estado;
  } catch (e) {
    setEspOnline(false);
  } finally {
    pollOcupado = false;
  }
}

function setEspOnline(on) {
  if (espOnline === on) return;
  espOnline = on;
  const dot = document.getElementById('esp-dot');
  if (dot) dot.className = 'status-dot pulse ' + (on ? 'green' : 'red');
  setTxt('esp-status-title', on ? 'ESP32 conectado' : 'ESP32 sem resposta');
  const td = document.getElementById('top-dot');
  if (td) td.className = 'status-dot pulse ' + (on ? 'green' : 'red');
  setTxt('top-status', on ? 'ESP32 ONLINE' : 'ESP32 OFFLINE');
  setTxt('esp-status-sub', on ? 'Servidor HTTP na porta 80' : 'Verifique o Wi-Fi e o IP do aparelho');
}

function vitimaVinculada(nome) {
  return victimsDatabase.find(v => v.name === nome) || victimsDatabase[0] || victimsPool[0];
}
const NOMES_PRIORIDADE = { alta: 'PRIORIDADE ALTA', maxima: 'PRIORIDADE MÁXIMA' };

function mostrarEmergencia(s) {
  const v = vitimaVinculada(s.nome);
  const hora = horaReal(s.inicio_emergencia);
  const c = document.getElementById('active-emergency-container');
  c.style.border = '1px solid var(--primary-red)';
  c.style.boxShadow = '0 0 35px rgba(255,42,75,.18)';
  c.innerHTML = `
    <div id="coacao-banner" class="hm-banner hm-banner-red" hidden>
      <i class="ph-fill ph-warning"></i>
      <div><strong>POSSÍVEL COAÇÃO — MODO SILENCIOSO</strong>
      <span>A usuária usou a senha de coação ou errou a senha várias vezes. Para ela, o alerta parece cancelado. Não envie mensagens pelo Telegram.</span></div>
    </div>
    <div id="pedido-banner" class="hm-banner hm-banner-yellow" hidden>
      <i class="ph-bold ph-info"></i>
      <div><strong>A usuária digitou a senha de cancelamento</strong><span>Ela pede para encerrar a ocorrência. Confirme antes de encerrar.</span></div>
    </div>
    <div class="alert-header-status">
      <span class="pulse-icon"><i class="ph-fill ph-warning-octagon"></i></span>
      <div>
        <h3>EMERGÊNCIA EM ANDAMENTO — OCORRÊNCIA ${esc(s.ocorrencia)}</h3>
        <p id="emerg-sub">Dispositivo ${esc(s.id)} • origem: ${esc(s.origem)}</p>
      </div>
      <span class="badge-priority" id="emerg-prioridade">${NOMES_PRIORIDADE[s.prioridade] || 'PRIORIDADE ALTA'}</span>
    </div>
    <div class="victim-profile-grid">
      <div class="victim-photo-card">
        <div class="avatar-large"><i class="ph-bold ph-user"></i></div>
        <div class="victim-main">
          <h2>${esc(v.name)}</h2>
          <p class="address"><i class="ph-bold ph-map-pin"></i> ${esc(v.address || 'Endereço não informado')}</p>
          <p class="address" id="emerg-local"><i class="ph-bold ph-navigation-arrow"></i> Localização em tempo real: aguardando a usuária enviar pelo Telegram</p>
          <div class="time-callout"><span>ALERTA RECEBIDO ÀS</span><strong>${esc(hora)}</strong></div>
        </div>
      </div>

      <div class="audio-proof-box">
        <div class="audio-header">
          <i class="ph-bold ph-waveform"></i>
          <div>
            <span class="label">GRAVAÇÃO DE ÁUDIO COMO PROVA (MICROFONE I2S)</span>
            <p id="emerg-gravacao">Gravação iniciada no aperto do botão</p>
          </div>
        </div>
        <audio id="emerg-audio" controls preload="none" style="width:100%;margin-top:10px"></audio>
        <div class="hm-row">
          <button class="btn-sm" onclick="atualizarAudio()"><i class="ph-bold ph-arrows-clockwise"></i> Carregar áudio gravado até agora</button>
          <button class="btn-sm" id="btn-gravar-mais" onclick="gravarMais()"><i class="ph-bold ph-record"></i> Gravar mais 60 s</button>
        </div>
        <p class="desc" id="emerg-vol" style="margin-top:6px">Microfone: --</p>
      </div>

      <div class="hm-chat">
        <span class="label">CONVERSA COM A USUÁRIA (TELEGRAM)</span>
        <div id="chat-box" class="hm-chat-box"><p class="desc">Carregando...</p></div>
        <form class="hm-chat-form" onsubmit="enviarChat(event)">
          <input id="chat-input" type="text" maxlength="300" placeholder="Mensagem para a usuária..." autocomplete="off" />
          <button class="btn-primary" id="chat-btn" type="submit"><i class="ph-bold ph-paper-plane-tilt"></i></button>
        </form>
      </div>

      <div class="action-buttons-group">
        <h4><i class="ph-bold ph-siren"></i> ACIONAMENTO DIRETO DAS FORÇAS</h4>
        <div class="btn-dual">
          <button class="btn-police" onclick="triggerDispatch('Polícia Militar do Paraná - 190')"><i class="ph-fill ph-shield"></i> Acionar PM-PR (190)</button>
          <button class="btn-samu" onclick="triggerDispatch('SAMU Curitiba - 192')"><i class="ph-fill ph-first-aid-kit"></i> Acionar SAMU (192)</button>
        </div>
        <button class="btn-confirm-success" onclick="resolveEmergency()"><i class="ph-bold ph-check-circle"></i> CONFIRMAR ATENDIMENTO REALIZADO COM SUCESSO</button>
        <button class="btn-cancel-operator" onclick="cancelEmergencyByOperator()"><i class="ph-bold ph-x-circle"></i> CANCELAR ALERTA NA CENTRAL (FALSO ALARME)</button>
      </div>
    </div>`;
  setContadores(1);
  try {
    if (typeof map !== 'undefined' && map && v.coords) {
      map.setView(v.coords, 15);
      colocarMarcador(v.coords, `<b>${esc(v.name)}</b><br>Endereço cadastrado`);
    }
  } catch (e) {  }
  document.getElementById('alerta-ativo').scrollIntoView({ behavior: 'smooth' });
}

function atualizarEmergencia(s) {
  const pri = document.getElementById('emerg-prioridade');
  if (pri) { pri.textContent = NOMES_PRIORIDADE[s.prioridade] || 'PRIORIDADE ALTA'; pri.style.opacity = s.prioridade === 'maxima' ? '1' : '.75'; }
  const cb = document.getElementById('coacao-banner'); if (cb) cb.hidden = !s.coacao;
  const pb = document.getElementById('pedido-banner'); if (pb) pb.hidden = !s.pedido_encerrar;
  setTxt('emerg-gravacao', s.gravando ? `Gravando... ${s.audio_s} s de ${s.audio_limite_s} s` : `Gravação concluída: ${s.audio_s} s salvos no dispositivo`);
  setTxt('emerg-vol', 'Microfone agora: volume ' + s.volume + (s.som_alto ? ' (som alto)' : ''));
  const inp = document.getElementById('chat-input'), btn = document.getElementById('chat-btn');
  if (inp) { inp.disabled = s.coacao; inp.placeholder = s.coacao ? 'Modo silencioso: não envie mensagens' : 'Mensagem para a usuária...'; }
  if (btn) btn.disabled = s.coacao;

}

async function carregarChat() {
  try {
    const r = await espGet('/chat');
    const box = document.getElementById('chat-box');
    if (!box || !r || !Array.isArray(r.msgs)) return;
    box.innerHTML = r.msgs.length ? r.msgs.map(m => `
      <div class="hm-msg hm-${esc(m.de)}"><span><i class="ph-bold ${iconeMsg(m)}"></i> ${m.de === 'usuaria' ? 'Usuária' : m.de === 'central' ? 'Central' : 'Sistema'} • ${esc(horaReal(m.hora))}</span>${esc(semEmoji(m.texto))}</div>`).join('')
      : '<p class="desc">Nenhuma mensagem ainda.</p>';
    box.scrollTop = box.scrollHeight;
  } catch (e) {}
}

function colocarMarcador(coords, html) {
  try {
    removerMarcador();
    const icon = L.divIcon({ className: 'custom-pin-red',
      html: '<div style="background:#ff2a4b;width:18px;height:18px;border-radius:50%;border:3px solid #fff;box-shadow:0 0 12px #ff2a4b"></div>' });
    marcadorEmergencia = L.marker(coords, { icon }).addTo(map).bindPopup(html).openPopup();
  } catch (e) {}
}

function cardMensagem(icone, cor, titulo, texto) {
  const c = document.getElementById('active-emergency-container');
  if (!c) return;
  c.innerHTML = `
    <div class="text-center" style="padding:40px;text-align:center">
      <i class="ph-bold ${icone}" style="font-size:52px;color:${cor};margin-bottom:12px"></i>
      <h2>${titulo}</h2>
      <p class="desc" style="margin-top:8px">${texto}</p>
    </div>`;
  c.style.border = '1px solid ' + (cor.includes('yellow') ? 'var(--accent-yellow)' : 'var(--border-color)');
  c.style.boxShadow = 'none';
}
function mostrarSemEmergencia() {
  zerarContadores(); removerMarcador();
  cardMensagem('ph-check-circle', 'var(--accent-green)', 'Nenhuma Emergência Ativa no Momento',
    'Quando a usuária segurar o botão do dispositivo por 1 segundo, o alerta aparece aqui.');
}
function mostrarCanceladoPelaUsuaria() {
  cardMensagem('ph-info', 'var(--accent-yellow)', 'Alerta Cancelado pela Usuária',
    'A usuária cancelou com a senha correta dentro dos 20 segundos. O áudio foi apagado.');
  somarCancelamento();
}
function setContadores(n) {
  activeEmergenciesCount = n;
  ['active-count', 'active-badge', 'notif-badge'].forEach(id => { const el = document.getElementById(id); if (el) el.textContent = n; });
}
function zerarContadores() { setContadores(0); }
function removerMarcador() { try { if (marcadorEmergencia && map) map.removeLayer(marcadorEmergencia); } catch (e) {} marcadorEmergencia = null; }
function somarCancelamento() { statsData.weekly.cancellations++; statsData.monthly.cancellations++; renderChart(); }

function formatarDuracao(seg) { seg = Number(seg) || 0; return Math.floor(seg / 60) + 'm ' + String(seg % 60).padStart(2, '0') + 's'; }
async function carregarHistorico() {
  try {
    const lista = await espGet('/historico');
    if (!Array.isArray(lista)) return;
    const tbody = document.querySelector('#alerts-table tbody');
    tbody.querySelectorAll('tr[data-esp]').forEach(r => r.remove());
    lista.slice().reverse().forEach(h => {
      const tr = document.createElement('tr');
      tr.dataset.esp = '1';
      tr.innerHTML = `
        <td><strong>${esc(h.codigo)}</strong><br><small class="text-red">${h.prioridade === 'maxima' ? 'Prioridade Máxima' : 'Prioridade Alta'}${h.coacao ? ' • COAÇÃO' : ''}</small></td>
        <td>${esc(h.nome)}</td>
        <td>${esc(h.inicio)}</td>
        <td>${pillDesfecho(h.desfecho)}</td>
        <td>${h.resposta_s >= 0 ? formatarDuracao(h.resposta_s) : '<span style="color:var(--text-muted)">—</span>'}</td>
        <td>${h.audio ? `<button class="btn-sm" onclick="tocarAudio('${esc(h.codigo)}')">Ouvir Áudio</button>` : '<span style="color:var(--text-muted);font-size:.8rem">áudio apagado</span>'}</td>`;
      tbody.prepend(tr);
    });
  } catch (e) {}
}
async function tocarAudio(oc) {
  const player = document.getElementById('audio-modal-player');
  setTxt('audio-modal-title', 'Áudio da ocorrência ' + oc);
  let blob = null;
  try { blob = await idbGet(oc); } catch (e) {}
  if (blob) {
    player.src = URL.createObjectURL(blob);
    setTxt('audio-modal-info', 'Cópia salva neste computador da central.');
  } else {
    player.src = ESP + '/audio?oc=' + encodeURIComponent(oc) + '&t=' + Date.now();
    setTxt('audio-modal-info', 'Tocando direto do dispositivo. Se não tocar, o áudio já foi apagado da memória do ESP32.');
    guardarAudio(oc);
  }
  openModal('modal-audio');
  player.play().catch(() => {});
}
function fecharAudio() { const p = document.getElementById('audio-modal-player'); if (p) p.pause(); closeModal('modal-audio'); }

function setTxt(id, txt) { const el = document.getElementById(id); if (el) el.textContent = txt; }
function formatarUptime(s) { s = Number(s) || 0; const h = Math.floor(s / 3600), m = Math.floor((s % 3600) / 60), x = s % 60; return (h ? h + 'h ' : '') + m + 'min ' + x + 's'; }
function atualizarCardDispositivo(s) {
  const nomes = { normal: 'Monitoramento normal', contagem: 'Pré-alerta (' + s.restante_s + ' s)', emergencia: 'EMERGÊNCIA ' + s.ocorrencia };
  setTxt('esp-estado', nomes[s.estado] || s.estado);
  setTxt('esp-conexao', s.conectado ? 'Conectado' : 'Desconectado');
  setTxt('esp-ip', s.ip);
  setTxt('esp-mac', s.mac);
  setTxt('esp-ssid', s.ssid);
  setTxt('esp-rssi', s.rssi + ' dBm');
  setTxt('esp-uptime', formatarUptime(s.uptime_s));
  setTxt('esp-hora', s.hora_ok ? s.hora : 'aguardando NTP');
  setTxt('esp-volume', s.volume + (s.som_alto ? ' (som alto)' : ''));
  setTxt('esp-limite', s.limite);
  setTxt('esp-botao', s.botao ? 'Pressionado' : 'Solto');
  setTxt('esp-motor', s.motor ? 'Vibrando' : 'Desligado');
  setTxt('esp-telegram', s.telegram_ok ? 'Conectado' : 'Sem conexão');
  setTxt('esp-gravacao', s.gravando ? `Gravando (${s.audio_s} s)` : 'Parada');
  setTxt('esp-flash', s.flash_livre_kb + ' KB livres');
  const pct = Math.min(100, Math.round((s.volume / Math.max(1, s.limite)) * 100));
  const bar = document.getElementById('esp-vol-bar');
  if (bar) { bar.style.width = pct + '%'; bar.style.background = s.som_alto ? 'var(--primary-red)' : 'var(--accent-green)'; }
  const est = document.getElementById('esp-estado');
  if (est) est.style.color = s.estado === 'normal' ? 'var(--accent-green)' : (s.estado === 'contagem' ? 'var(--accent-yellow)' : 'var(--primary-red)');
  const tg = document.getElementById('esp-telegram');
  if (tg) tg.style.color = s.telegram_ok ? 'var(--accent-green)' : 'var(--accent-yellow)';
}

async function atualizarLog() {
  try {
    const t = await espGet('/log?ultimas=12');
    const pre = document.getElementById('esp-log');
    if (pre && typeof t === 'string') pre.textContent = t || '(log vazio)';
  } catch (e) {}
}

document.addEventListener('DOMContentLoaded', () => {
  pollESP();
  setInterval(pollESP, 1000);
  atualizarLog();
  setInterval(atualizarLog, 4000);
  carregarHistorico();
});

function horaReal(txt) {
  txt = txt || '';
  if (!txt || txt.indexOf('sem-hora') === 0) return new Date().toLocaleTimeString('pt-BR');
  return txt.split(' ')[1] || txt;
}
function atualizarRelogio() {
  const el = document.getElementById('hm-relogio');
  if (el) el.textContent = 'HORA ATUAL: ' + new Date().toLocaleTimeString('pt-BR');
}
document.addEventListener('DOMContentLoaded', () => {
  atualizarRelogio();
  setInterval(atualizarRelogio, 1000);
  setInterval(updateHeaderDate, 60000);
});

function tempoTxt(seg) { return seg >= 0 ? formatarDuracao(seg) : '--'; }
function pillDesfecho(d) {
  if (d === 'cancelado_usuaria') return '<span class="status-pill" style="background:rgba(255,193,7,.12);color:var(--accent-yellow)"><i class="ph-bold ph-x"></i> Cancelado pela usuária</span>';
  if (d === 'falso_alarme') return '<span class="status-pill" style="background:rgba(150,150,170,.15);color:var(--text-muted)"><i class="ph-bold ph-x"></i> Falso alarme</span>';
  return '<span class="status-pill success"><i class="ph-bold ph-check"></i> Concluído</span>';
}
function setTag(id, txt, cor) {
  const el = document.getElementById(id);
  if (!el) return;
  el.textContent = txt;
  el.classList.remove('green', 'red-alert', 'neutral');
  if (cor) el.classList.add(cor);
}

function alertaEmAndamento() { return ultimoStatus && ultimoStatus.estado !== 'normal' ? 1 : 0; }

let estatisticas = null;
async function carregarEstatisticas() {
  try {
    const e = await espGet('/estatisticas');
    if (!e || typeof e !== 'object') return;
    estatisticas = e;
    pintarEstatisticas();
  } catch (err) {}
}
function pintarEstatisticas() {
  const e = estatisticas;
  if (!e) return;
  const ativo = alertaEmAndamento();

  const hoje = e.hoje + ativo;
  setTxt('metric-today-calls', e.hora_ok ? hoje : '--');
  if (!e.hora_ok) setTag('metric-today-vs', 'aguardando hora', 'neutral');
  else {
    const dif = hoje - e.ontem;
    setTag('metric-today-vs', dif === 0 ? 'igual a ontem' : (dif > 0 ? '+' : '') + dif + ' vs. ontem (' + e.ontem + ')', dif > 0 ? 'red-alert' : 'green');
  }
  setTxt('metric-today-foot', e.concluidos_hoje + (e.concluidos_hoje === 1 ? ' atendida' : ' atendidas') + ' com sucesso hoje');

  setTxt('metric-completed', e.concluidos);
  const confirmadas = e.concluidos + e.cancel_central;
  setTag('metric-completed-pct', confirmadas ? Math.round(100 * e.concluidos / confirmadas) + '% concluídas' : 'sem dados', 'neutral');
  setTxt('metric-completed-foot', e.total + ' alertas no total • ' + e.cancel_usuaria + ' cancelados pela usuária • ' + e.cancel_central + ' falsos alarmes');

  setTxt('metric-resp', tempoTxt(e.resp_media_s));
  const rs = e.semana.resp_media_s, rp = e.resp_media_semana_passada_s;
  if (rs >= 0 && rp >= 0) {
    const d = rs - rp;
    setTag('metric-resp-vs', (d > 0 ? '+' : d < 0 ? '-' : '') + formatarDuracao(Math.abs(d)) + ' vs. semana passada', d <= 0 ? 'green' : 'red-alert');
  } else setTag('metric-resp-vs', rs >= 0 ? 'esta semana: ' + formatarDuracao(rs) : 'sem dados ainda', 'neutral');

  const sem = e.semana, mes = e.mes;
  const semVals = sem.valores.slice(), mesVals = mes.valores.slice();
  if (ativo && e.hora_ok) { semVals[semVals.length - 1]++; mesVals[mesVals.length - 1]++; }
  statsData.weekly = { labels: sem.labels, values: semVals, total: sem.total + ativo, avgResponse: tempoTxt(sem.resp_media_s), cancellations: sem.cancel_usuaria + sem.cancel_central, det: sem };
  statsData.monthly = { labels: mes.labels, values: mesVals, total: mes.total + ativo, avgResponse: tempoTxt(mes.resp_media_s), cancellations: mes.cancel_usuaria + mes.cancel_central, det: mes };
  renderChart();
}

const renderChartOriginal = renderChart;
renderChart = function () {
  renderChartOriginal();
  const d = statsData[currentPeriod];
  const max = Math.max(3, ...d.values);
  const topo = Math.ceil(max * 1.2 / 3) * 3;
  const y = document.getElementById('chart-y-axis');
  if (y) y.innerHTML = [topo, topo * 2 / 3, topo / 3, 0].map(v => `<span>${v}</span>`).join('');

  document.querySelectorAll('#chart-bars-wrapper .chart-bar').forEach((b, i) => { b.style.height = (d.values[i] / topo * 100) + '%'; });
  const hojeN = estatisticas ? estatisticas.hoje + alertaEmAndamento() : 0;
  setTag('stat-total-tag', '+' + hojeN + ' hoje', hojeN ? 'red-alert' : 'neutral');
  const det = d.det;
  if (det && det.resp_media_s >= 0) setTag('stat-avg-tag', det.resp_media_s <= 600 ? 'Dentro da meta (até 10 min)' : 'Acima da meta (10 min)', det.resp_media_s <= 600 ? 'green' : 'red-alert');
  else setTag('stat-avg-tag', 'nenhum atendimento concluído', 'neutral');
  setTag('stat-cancel-tag', det ? det.cancel_usuaria + ' pela usuária (senha) • ' + det.cancel_central + ' pela central' : '--', 'neutral');
};

incrementChartData = function () { carregarEstatisticas(); };
somarCancelamento = function () { carregarEstatisticas(); };

function pintarAtivas() {
  const s = ultimoStatus;
  if (!s || s.estado === 'normal') {
    setTag('metric-active-prio', 'Nenhuma', 'neutral');
    setTxt('metric-active-foot', 'Nenhum alerta em andamento');
    return;
  }
  if (s.estado === 'contagem') {
    setTag('metric-active-prio', 'Pré-alerta', 'red-alert');
    setTxt('metric-active-foot', 'Aguardando a usuária (' + s.restante_s + ' s para cancelar)');
    return;
  }
  setTag('metric-active-prio', s.prioridade === 'maxima' ? 'Prioridade Máxima' : 'Prioridade Alta', 'red-alert');
  setTxt('metric-active-foot', (s.coacao ? 'COAÇÃO • ' : '') + 'Em atendimento pela central');
}

const LV = { seq: 0, oc: null, pontos: [], status: 'sem_localizacao', ultimaHa: -1, expira: -1, lidoEm: 0,
             coacao: false, mapa: null, pino: null, circulo: null, linha: null, inicio: null,
             seguir: true, abriuAuto: false, statusAnterior: null, ocupado: false, anim: null };

function lvDist(a, b) {
  const R = 6371000, rad = Math.PI / 180;
  const dLat = (b.lat - a.lat) * rad, dLon = (b.lon - a.lon) * rad;
  const h = Math.sin(dLat / 2) ** 2 + Math.cos(a.lat * rad) * Math.cos(b.lat * rad) * Math.sin(dLon / 2) ** 2;
  return 2 * R * Math.asin(Math.sqrt(h));
}
function lvTempo(seg) {
  if (seg < 0) return '--';
  if (seg < 60) return seg + ' s';
  if (seg < 3600) return Math.floor(seg / 60) + ' min ' + String(seg % 60).padStart(2, '0') + ' s';
  return Math.floor(seg / 3600) + ' h ' + String(Math.floor(seg % 3600 / 60)).padStart(2, '0') + ' min';
}

function lvCriarTela() {
  const css = document.createElement('style');
  css.textContent = `
  #lv-modal{position:fixed;inset:0;background:rgba(5,6,10,.72);z-index:5000;display:none;align-items:center;justify-content:center;padding:18px}
  #lv-modal.aberto{display:flex}
  #lv-box{background:var(--bg-card);border:1px solid var(--primary-red);border-radius:18px;width:min(1180px,100%);height:min(780px,100%);display:flex;flex-direction:column;overflow:hidden;box-shadow:0 0 50px rgba(255,42,75,.25)}
  #lv-head{display:flex;align-items:center;gap:14px;padding:14px 18px;border-bottom:1px solid var(--border-color);flex-wrap:wrap}
  #lv-head h3{font-size:1.05rem;margin-right:auto}
  .lv-badge{font-size:.74rem;font-weight:800;letter-spacing:.6px;padding:6px 10px;border-radius:8px;display:inline-flex;align-items:center;gap:7px}
  .lv-badge.ao_vivo{background:rgba(0,200,120,.14);color:#2ee59d}
  .lv-badge.ao_vivo::before{content:'';width:8px;height:8px;border-radius:50%;background:#2ee59d;animation:lvPulso 1.2s infinite}
  .lv-badge.sem_atualizacao{background:rgba(255,193,7,.14);color:var(--accent-yellow)}
  .lv-badge.encerrada{background:rgba(150,150,170,.16);color:var(--text-muted)}
  .lv-badge.fixa{background:rgba(70,140,255,.15);color:#7fb0ff}
  @keyframes lvPulso{0%{box-shadow:0 0 0 0 rgba(46,229,157,.7)}100%{box-shadow:0 0 0 10px rgba(46,229,157,0)}}
  #lv-corpo{flex:1;display:grid;grid-template-columns:1fr 300px;min-height:0}
  #lv-mapa{min-height:300px}
  #lv-lado{border-left:1px solid var(--border-color);padding:16px;display:flex;flex-direction:column;gap:10px;overflow:auto}
  .lv-item{background:var(--bg-card-hover);border:1px solid var(--border-color);border-radius:10px;padding:10px 12px}
  .lv-item span{display:block;font-size:.66rem;color:var(--text-muted);letter-spacing:1px;text-transform:uppercase}
  .lv-item strong{display:block;margin-top:3px;font-size:.95rem;word-break:break-word}
  #lv-lado button{width:100%;padding:10px;border-radius:10px;font-weight:700;cursor:pointer;font-family:inherit;border:1px solid var(--border-color);background:#181c28;color:var(--text-main)}
  #lv-lado button:disabled{opacity:.45;cursor:not-allowed}
  #lv-lado button.ativo{border-color:var(--accent-green);color:var(--accent-green)}
  #lv-aviso{font-size:.8rem;color:var(--text-muted);line-height:1.4}
  #lv-coacao{display:none;background:rgba(255,42,75,.12);color:var(--primary-red);border:1px solid rgba(255,42,75,.4);border-radius:10px;padding:10px;font-size:.8rem;font-weight:700}
  .lv-pino{width:22px;height:22px;border-radius:50%;background:#ff2a4b;border:3px solid #fff;box-shadow:0 0 0 0 rgba(255,42,75,.7);animation:lvPino 1.4s infinite}
  @keyframes lvPino{0%{box-shadow:0 0 0 0 rgba(255,42,75,.7)}100%{box-shadow:0 0 0 18px rgba(255,42,75,0)}}
  #lv-fechar{background:transparent;border:none;color:var(--text-muted);font-size:1.6rem;cursor:pointer;line-height:1}
  @media (max-width:820px){#lv-corpo{grid-template-columns:1fr;grid-template-rows:1fr auto}#lv-lado{border-left:none;border-top:1px solid var(--border-color);max-height:42vh}}`;
  document.head.appendChild(css);
  const m = document.createElement('div');
  m.id = 'lv-modal';
  m.innerHTML = `
    <div id="lv-box">
      <div id="lv-head">
        <h3><i class="ph-bold ph-map-pin"></i> Localização da usuária • <span id="lv-oc">--</span></h3>
        <span class="lv-badge" id="lv-badge">--</span>
        <button id="lv-fechar" title="Fechar (o acompanhamento continua)" onclick="lvFechar()">&times;</button>
      </div>
      <div id="lv-corpo">
        <div id="lv-mapa"></div>
        <div id="lv-lado">
          <div id="lv-coacao"><i class="ph-bold ph-shield-warning"></i> Coação: modo silencioso. Não peça nada à usuária pelo Telegram.</div>
          <div class="lv-item"><span>Última atualização</span><strong id="lv-ha">--</strong></div>
          <div class="lv-item"><span>Coordenadas</span><strong id="lv-coord">--</strong></div>
          <div class="lv-item"><span>Precisão do celular</span><strong id="lv-prec">--</strong></div>
          <div class="lv-item"><span>Velocidade estimada</span><strong id="lv-vel">--</strong></div>
          <div class="lv-item"><span>Distância percorrida</span><strong id="lv-dist">--</strong></div>
          <div class="lv-item"><span>Compartilhamento termina em</span><strong id="lv-expira">--</strong></div>
          <button id="lv-seguir" class="ativo" onclick="lvCentralizar()"><i class="ph-bold ph-crosshair"></i> Seguindo o pino</button>
          <button onclick="lvGoogle()"><i class="ph-bold ph-arrow-square-out"></i> Abrir no Google Maps</button>
          <button onclick="lvCopiar()" id="lv-copiar"><i class="ph-bold ph-copy"></i> Copiar coordenadas (p/ 190)</button>
          <button onclick="lvPedir()" id="lv-pedir"><i class="ph-bold ph-paper-plane-tilt"></i> Pedir localização ao vivo</button>
          <p id="lv-aviso">O Telegram envia um ponto novo quando o celular se move. Parada no mesmo lugar, a posição pode ficar alguns minutos sem mudar.</p>
        </div>
      </div>
    </div>`;
  document.body.appendChild(m);
}

function lvAbrir() {
  const m = document.getElementById('lv-modal');
  if (!m) return;
  m.classList.add('aberto');
  if (!LV.mapa && typeof L !== 'undefined') {
    LV.mapa = L.map('lv-mapa', { zoomControl: true }).setView([-25.4284, -49.2733], 13);
    L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', { attribution: '&copy; OpenStreetMap', maxZoom: 19 }).addTo(LV.mapa);
    LV.mapa.on('dragstart', () => { LV.seguir = false; lvBotaoSeguir(); });
    LV.pontos.forEach((p, i) => lvDesenhar(p, i === LV.pontos.length - 1, false));
  }
  setTimeout(() => { if (LV.mapa) { LV.mapa.invalidateSize(); lvCentralizar(); } }, 150);
  lvPintar();
}
function lvFechar() { const m = document.getElementById('lv-modal'); if (m) m.classList.remove('aberto'); lvAtualizarAtalhos(); }
function lvBotaoSeguir() {
  const b = document.getElementById('lv-seguir');
  if (!b) return;
  b.classList.toggle('ativo', LV.seguir);
  b.innerHTML = LV.seguir ? '<i class="ph-bold ph-crosshair"></i> Seguindo o pino' : '<i class="ph-bold ph-crosshair"></i> Centralizar no pino';
}
function lvCentralizar() {
  LV.seguir = true; lvBotaoSeguir();
  const u = LV.pontos[LV.pontos.length - 1];
  if (LV.mapa && u) LV.mapa.setView([u.lat, u.lon], Math.max(LV.mapa.getZoom(), 16));
}
function lvGoogle() { const u = LV.pontos[LV.pontos.length - 1]; if (u) window.open(`https://www.google.com/maps?q=${u.lat},${u.lon}`, '_blank'); }
function lvCopiar() {
  const u = LV.pontos[LV.pontos.length - 1];
  if (!u) return;
  const txt = `${u.lat.toFixed(6)}, ${u.lon.toFixed(6)}`;
  const ok = () => { const b = document.getElementById('lv-copiar'); if (b) { b.textContent = 'Copiado: ' + txt; setTimeout(() => b.innerHTML = '<i class="ph-bold ph-copy"></i> Copiar coordenadas (p/ 190)', 2500); } };

  if (navigator.clipboard && window.isSecureContext) navigator.clipboard.writeText(txt).then(ok);
  else { const t = document.createElement('textarea'); t.value = txt; document.body.appendChild(t); t.select(); try { document.execCommand('copy'); ok(); } catch (e) {} t.remove(); }
}
async function lvPedir() {
  if (LV.coacao) return;
  try {
    const r = await espGet('/pedir_localizacao');
    const b = document.getElementById('lv-pedir');
    if (b) { b.textContent = r && r.ok ? 'Pedido enviado no Telegram' : 'Não foi possível pedir'; setTimeout(() => { b.textContent = ''; lvPintar(); }, 3000); }
  } catch (e) {}
}

function lvMoverPino(lat, lon) {
  const ini = LV.pino.getLatLng(), t0 = performance.now(), dur = 900;
  cancelAnimationFrame(LV.anim);
  const passo = (t) => {
    const k = Math.min(1, (t - t0) / dur), e = k < .5 ? 2 * k * k : 1 - Math.pow(-2 * k + 2, 2) / 2;
    const pos = [ini.lat + (lat - ini.lat) * e, ini.lng + (lon - ini.lng) * e];
    LV.pino.setLatLng(pos); if (LV.circulo) LV.circulo.setLatLng(pos);
    if (k < 1) LV.anim = requestAnimationFrame(passo);
  };
  LV.anim = requestAnimationFrame(passo);
}
function lvDesenhar(p, ultimo, animar) {
  if (!LV.mapa) return;
  const ll = [p.lat, p.lon];
  if (!LV.inicio) {
    LV.inicio = L.circleMarker(ll, { radius: 5, color: '#fff', weight: 2, fillColor: '#888', fillOpacity: 1 }).addTo(LV.mapa).bindTooltip('Primeira posição recebida');
    LV.linha = L.polyline([ll], { color: '#ff2a4b', weight: 4, opacity: .75 }).addTo(LV.mapa);
  } else LV.linha.addLatLng(ll);
  if (!ultimo) return;
  if (!LV.pino) {
    LV.pino = L.marker(ll, { icon: L.divIcon({ className: '', html: '<div class="lv-pino"></div>', iconSize: [22, 22], iconAnchor: [11, 11] }), zIndexOffset: 1000 }).addTo(LV.mapa);
    LV.circulo = L.circle(ll, { radius: p.prec || 0, color: '#ff2a4b', weight: 1, fillOpacity: .08 }).addTo(LV.mapa);
  } else if (animar) lvMoverPino(p.lat, p.lon);
  else { LV.pino.setLatLng(ll); LV.circulo.setLatLng(ll); }
  LV.circulo.setRadius(p.prec || 0);
  if (LV.seguir) LV.mapa.panTo(ll, { animate: true, duration: .9 });
}

function lvLimpar() {
  if (LV.mapa) [LV.pino, LV.circulo, LV.linha, LV.inicio].forEach(c => { if (c) LV.mapa.removeLayer(c); });
  Object.assign(LV, { seq: 0, pontos: [], status: 'sem_localizacao', ultimaHa: -1, expira: -1, pino: null, circulo: null,
                      linha: null, inicio: null, seguir: true, abriuAuto: false, statusAnterior: null });
  try { removerMarcador(); } catch (e) {}
  lvBotaoSeguir();
}

async function lvBuscar() {
  const s = ultimoStatus;
  if (!s || s.estado === 'normal') {
    if (LV.oc) { lvFechar(); lvLimpar(); LV.oc = null; }
    return;
  }
  if (LV.ocupado) return;
  LV.ocupado = true;
  try {
    if (LV.oc !== s.ocorrencia) { lvLimpar(); LV.oc = s.ocorrencia; }
    const r = await espGet('/localizacao?desde=' + LV.seq);
    if (!r || r.ocorrencia !== LV.oc) return;
    if (r.ultimo_seq < LV.seq) { lvLimpar(); LV.oc = r.ocorrencia; }
    LV.status = r.status; LV.ultimaHa = r.ultima_ha_s; LV.expira = r.expira_em_s; LV.lidoEm = Date.now(); LV.coacao = r.coacao;
    const novos = r.pontos || [];
    novos.forEach((p, i) => {
      p.t = LV.lidoEm / 1000 - p.idade_s;
      LV.pontos.push(p); LV.seq = Math.max(LV.seq, p.seq);
      lvDesenhar(p, i === novos.length - 1, LV.pontos.length > 1);
    });
    if (novos.length) {
      const u = LV.pontos[LV.pontos.length - 1];

      try { if (typeof map !== 'undefined' && map) { if (marcadorEmergencia) marcadorEmergencia.setLatLng([u.lat, u.lon]); else colocarMarcador([u.lat, u.lon], '<b>Usuária</b> • localização do Telegram'); } } catch (e) {}
    }

    const comecouAoVivo = LV.status === 'ao_vivo' && LV.statusAnterior !== 'ao_vivo' && LV.statusAnterior !== 'sem_atualizacao';
    if (LV.pontos.length && (!LV.abriuAuto || comecouAoVivo)) { LV.abriuAuto = true; lvAbrir(); }
    LV.statusAnterior = LV.status;
  } catch (e) {
  } finally { LV.ocupado = false; }
}

const LV_TEXTO = { ao_vivo: 'AO VIVO', sem_atualizacao: 'SEM ATUALIZAÇÃO', encerrada: 'COMPARTILHAMENTO ENCERRADO • ÚLTIMA POSIÇÃO', fixa: 'POSIÇÃO FIXA (NÃO ATUALIZA)', sem_localizacao: 'AGUARDANDO LOCALIZAÇÃO' };
function lvPintar() {
  const ha = LV.ultimaHa < 0 ? -1 : LV.ultimaHa + Math.floor((Date.now() - LV.lidoEm) / 1000);
  const u = LV.pontos[LV.pontos.length - 1];

  const el = document.getElementById('emerg-local');
  if (el) {
    el.innerHTML = !u
      ? '<i class="ph-bold ph-navigation-arrow"></i> Localização: aguardando a usuária enviar pelo Telegram'
      : `<i class="ph-bold ph-navigation-arrow"></i> ${LV.status === 'ao_vivo' ? '<b style="color:#2ee59d">AO VIVO</b>' : esc(LV_TEXTO[LV.status])} • atualizada há ${lvTempo(ha)} <button class="lv-reabrir" onclick="lvAbrir()"><i class="ph-bold ph-map-trifold"></i> Abrir mapa ao vivo</button>`;
  }
  if (!document.getElementById('lv-modal')?.classList.contains('aberto')) return;
  setTxt('lv-oc', LV.oc || '--');
  const b = document.getElementById('lv-badge');
  if (b) {
    b.className = 'lv-badge ' + LV.status;
    b.textContent = LV.status === 'sem_atualizacao' ? 'SEM ATUALIZAÇÃO HÁ ' + lvTempo(ha).toUpperCase() : LV_TEXTO[LV.status];
  }
  setTxt('lv-ha', u ? 'há ' + lvTempo(ha) : '--');
  setTxt('lv-coord', u ? `${u.lat.toFixed(6)}, ${u.lon.toFixed(6)}` : '--');
  setTxt('lv-prec', u ? (u.prec ? '± ' + u.prec + ' m' + (u.prec > 100 ? ' (baixa precisão)' : '') : 'não informada') : '--');
  let vel = '--';

  for (let i = LV.pontos.length - 2; i >= 0; i--) {
    const a = LV.pontos[i], dt = u.t - a.t;
    if (dt > 180) break;
    if (dt >= 10) { let d = 0; for (let k = i + 1; k < LV.pontos.length; k++) d += lvDist(LV.pontos[k - 1], LV.pontos[k]); vel = (d / dt * 3.6).toFixed(1) + ' km/h'; break; }
  }
  setTxt('lv-vel', LV.status === 'ao_vivo' ? vel : '--');
  let d = 0; for (let i = 1; i < LV.pontos.length; i++) d += lvDist(LV.pontos[i - 1], LV.pontos[i]);
  setTxt('lv-dist', u ? (d < 1000 ? Math.round(d) + ' m' : (d / 1000).toFixed(2) + ' km') + ' • ' + LV.pontos.length + (LV.pontos.length === 1 ? ' ponto' : ' pontos') : '--');
  const exp = LV.expira < 0 ? -1 : Math.max(0, LV.expira - Math.floor((Date.now() - LV.lidoEm) / 1000));
  setTxt('lv-expira', LV.status === 'ao_vivo' || LV.status === 'sem_atualizacao' ? (exp < 0 ? 'sem prazo definido' : lvTempo(exp)) : '--');
  const cz = document.getElementById('lv-coacao'); if (cz) cz.style.display = LV.coacao ? 'block' : 'none';
  const pb = document.getElementById('lv-pedir');
  if (pb && !pb.textContent.startsWith('Pedido') && !pb.textContent.startsWith('Não foi')) {
    pb.disabled = LV.coacao || LV.status === 'ao_vivo';
    pb.innerHTML = LV.coacao ? 'Bloqueado: modo silencioso' : LV.status === 'ao_vivo' ? 'Já está ao vivo' : '<i class="ph-bold ph-paper-plane-tilt"></i> Pedir localização ao vivo';
  }
}

function semEmoji(t) { return String(t ?? '').replace(/[\u{1F300}-\u{1FAFF}\u{2600}-\u{27BF}\u{1F100}-\u{1F1FF}\u{FE0F}]/gu, '').replace(/^\s+/, ''); }
function iconeMsg(m) {
  const t = (m.texto || '').toLowerCase();
  if (m.de === 'usuaria') return t.includes('localiza') ? 'ph-map-pin' : t.includes('preciso de ajuda') ? 'ph-siren' : t.includes('estou bem') ? 'ph-check-circle' : 'ph-user';
  if (m.de === 'central') return 'ph-headset';
  if (t.includes('coação') || t.includes('senha')) return 'ph-shield-warning';
  if (t.includes('localiza')) return 'ph-map-pin-area';
  if (t.includes('som alto')) return 'ph-speaker-high';
  if (t.includes('gravação')) return 'ph-microphone';
  return 'ph-info';
}

function lvAbrirPeloMenu() { if (LV.pontos.length) lvAbrir(); }
function lvCriarFlutuante() {
  const b = document.createElement('button');
  b.id = 'lv-flutuante';
  b.title = 'Abrir o mapa com a localização da usuária';
  b.onclick = () => lvAbrir();
  b.innerHTML = '<span class="ponto" id="lv-flu-ponto"></span><i class="ph-bold ph-map-pin-area"></i> Localização da usuária <small id="lv-flu-txt"></small>';
  document.body.appendChild(b);
}
function lvAtualizarAtalhos() {
  const tem = LV.pontos.length > 0;
  const aberto = document.getElementById('lv-modal')?.classList.contains('aberto');
  const ha = LV.ultimaHa < 0 ? -1 : LV.ultimaHa + Math.floor((Date.now() - LV.lidoEm) / 1000);
  const vivo = LV.status === 'ao_vivo';
  const curto = { ao_vivo: 'ao vivo', sem_atualizacao: 'parada', encerrada: 'encerrada', fixa: 'fixa' }[LV.status] || 'sem sinal';

  const menu = document.getElementById('menu-lv'), tag = document.getElementById('menu-lv-tag');
  if (menu) menu.disabled = !tem;
  if (tag) { tag.textContent = tem ? curto : 'sem sinal'; tag.className = 'lv-menu-tag' + (vivo ? ' vivo' : tem && LV.status === 'sem_atualizacao' ? ' alerta' : ''); }

  const f = document.getElementById('lv-flutuante');
  if (f) {
    f.classList.toggle('visivel', tem && !aberto);
    const pt = document.getElementById('lv-flu-ponto'); if (pt) pt.className = 'ponto' + (vivo ? ' vivo' : '');
    setTxt('lv-flu-txt', tem ? curto + ' • há ' + lvTempo(ha) : '');
  }
}

let estadoVisto = null;
document.addEventListener('DOMContentLoaded', () => {
  lvCriarTela();
  lvCriarFlutuante();
  setInterval(lvAtualizarAtalhos, 1000);
  setInterval(lvBuscar, 2000);
  setInterval(lvPintar, 1000);
  carregarEstatisticas();
  setInterval(carregarEstatisticas, 10000);
  setInterval(() => {
    pintarAtivas();
    if (ultimoStatus && ultimoStatus.estado !== estadoVisto) {

      if (estadoVisto !== null) { carregarEstatisticas(); carregarHistorico(); }
      estadoVisto = ultimoStatus.estado;
    }
    pintarEstatisticas();
  }, 1000);
});

</script>
</body>
</html>
)HELPME";

struct Passo { uint16_t ms; bool ligado; };
enum TipoEntrada { ENT_TEXTO, ENT_BOTAO, ENT_LOCAL };
struct Entrada { TipoEntrada tipo; String chat; String texto; double lat; double lon;
                 float precisao; long aoVivoS; bool editada; };
struct Ponto { double lat; double lon; float precisao; uint32_t ms; uint32_t seq; };
struct Saida { String metodo; String json; };
enum Estado { NORMAL, CONTAGEM, EMERGENCIA };
struct Mensagem { String de; String texto; String hora; };

WebServer server(80);
bool fsOk = false;
bool horaOk = false;
SemaphoreHandle_t filaMutex;

String agoraStr() {
  if (horaOk) {
    struct tm t;
    if (getLocalTime(&t, 5)) {
      char b[24];
      strftime(b, sizeof(b), "%d/%m/%Y %H:%M:%S", &t);
      return String(b);
    }
  }
  return "sem-hora(uptime " + String(millis() / 1000) + "s)";
}

String horaCurta() {
  String a = agoraStr();
  int esp = a.indexOf(' ');
  return (horaOk && esp > 0) ? a.substring(esp + 1) : a;
}

void registrar(const char* nivel, const String& msg) {
  String linha = agoraStr() + " [" + nivel + "] " + msg;
  Serial.println(linha);
  if (!fsOk) return;
  File f = LittleFS.open(LOG_FILE, "a");
  if (!f) return;
  f.println(linha);
  size_t tamanho = f.size();
  f.close();
  if (tamanho > LOG_MAX) {
    LittleFS.remove(LOG_OLD);
    LittleFS.rename(LOG_FILE, LOG_OLD);
  }
}

void verificarHora() {
  if (horaOk) return;
  if (time(nullptr) > 1700000000) {
    horaOk = true;
    registrar("INFO", "Hora sincronizada pela internet");
  }
}

Passo padrao[8];
int nPassos = 0, passoAtual = 0;
uint32_t passoDesde = 0;
bool motorManual = false;
uint32_t motorManualDesde = 0;
bool motorLigado = false;

void motorSet(bool on) {
  digitalWrite(PINO_MOTOR, on ? HIGH : LOW);
  motorLigado = on;
}

void vibrar(std::initializer_list<uint16_t> tempos) {
  nPassos = 0;
  bool on = true;
  for (uint16_t t : tempos) {
    if (nPassos < 8) { padrao[nPassos].ms = t; padrao[nPassos].ligado = on; nPassos++; }
    on = !on;
  }
  passoAtual = 0;
  passoDesde = millis();
  motorManual = false;
  if (nPassos > 0) motorSet(padrao[0].ligado);
}

void atualizarMotor() {
  if (motorManual) {
    if (millis() - motorManualDesde >= MOTOR_MANUAL_MAX_MS) {
      motorManual = false;
      motorSet(false);
      registrar("INFO", "Motor desligado automaticamente (limite de 10 s)");
    }
    return;
  }
  if (passoAtual >= nPassos) return;
  if (millis() - passoDesde >= padrao[passoAtual].ms) {
    passoAtual++;
    passoDesde = millis();
    if (passoAtual < nPassos) motorSet(padrao[passoAtual].ligado);
    else motorSet(false);
  }
}

std::deque<Entrada> entradas;
std::deque<Saida> saidas;
volatile bool telegramOk = false;

void porSaida(const String& metodo, const String& json) {
  xSemaphoreTake(filaMutex, portMAX_DELAY);
  if (saidas.size() < 20) saidas.push_back({metodo, json});
  xSemaphoreGive(filaMutex);
}
bool tirarSaida(Saida& s) {
  bool tem = false;
  xSemaphoreTake(filaMutex, portMAX_DELAY);
  if (!saidas.empty()) { s = saidas.front(); saidas.pop_front(); tem = true; }
  xSemaphoreGive(filaMutex);
  return tem;
}
void porEntrada(const Entrada& e) {
  xSemaphoreTake(filaMutex, portMAX_DELAY);
  if (entradas.size() < 20) entradas.push_back(e);
  xSemaphoreGive(filaMutex);
}
bool tirarEntrada(Entrada& e) {
  bool tem = false;
  xSemaphoreTake(filaMutex, portMAX_DELAY);
  if (!entradas.empty()) { e = entradas.front(); entradas.pop_front(); tem = true; }
  xSemaphoreGive(filaMutex);
  return tem;
}

const char* TECLADO_PRE  = "{\"inline_keyboard\":[[{\"text\":\"Cancelar alerta\",\"callback_data\":\"cancelar\"},{\"text\":\"Enviar agora\",\"callback_data\":\"enviar\"}]]}";
const char* TECLADO_MENU = "{\"keyboard\":[[{\"text\":\"📍 Enviar localização atual\",\"request_location\":true}],[{\"text\":\"Estou bem\"},{\"text\":\"Preciso de ajuda\"}]],\"resize_keyboard\":true,\"is_persistent\":true}";

void tgEnviar(const String& texto, const char* teclado) {
  JsonDocument d;
  d["chat_id"] = CHAT_ID_USUARIA;
  d["text"] = texto;
  if (teclado) d["reply_markup"] = serialized(teclado);
  String json;
  serializeJson(d, json);
  porSaida("sendMessage", json);
}
void tgEnviar(const String& texto) { tgEnviar(texto, nullptr); }

Estado estado = NORMAL;

String ocorrencia = "";
String origemAlerta = "";
String inicioContagem = "";
String inicioEmergencia = "";
String prioridade = "alta";
String ultimoDesfecho = "";
bool coacao = false;
bool aguardandoSenha = false;
bool pedidoEncerrar = false;
int tentativasErradas = 0;
double latUsuaria = 0, lonUsuaria = 0;
String horaLocalizacao = "";

std::vector<Ponto> trajeto;
uint32_t locSeq = 0;
uint32_t locUltimaMs = 0;
uint32_t locAoVivoAte = 0;
bool locAoVivo = false;
bool locEncerrada = false;
float locPrecisao = 0;
const size_t TRAJETO_MAX = 150;
const uint32_t LOC_ATRASO_MS = 60000;

void limparLocalizacao() {
  trajeto.clear();
  latUsuaria = lonUsuaria = 0; horaLocalizacao = "";
  locUltimaMs = locAoVivoAte = 0; locAoVivo = locEncerrada = false; locPrecisao = 0;
}

void tgUsuaria(const String& texto) { if (!coacao) tgEnviar(texto); }

const char* MSG_COMO_AO_VIVO =
  "Para a central acompanhar você em tempo real:\n"
  "📎 (clipe) → Localização → \"Compartilhar localização em tempo real\" → escolha 8 horas.";
std::vector<Mensagem> chat;
uint32_t contagemDesde = 0, emergenciaDesde = 0;
uint32_t numOcorrencia = 2400;
uint32_t totalOcorrencias = 0, totalCancelamentos = 0;
time_t epochInicio = 0;
String arquivoParaApagar = "";

void addChat(const String& de, const String& texto) {
  if (chat.size() >= 40) chat.erase(chat.begin());
  chat.push_back({de, texto, horaCurta()});
}

String arquivoAudio(const String& oc) { return String(AUDIO_DIR) + "/" + oc + ".wav"; }

volatile long volume = 0;
volatile bool somAlto = false;
volatile bool gravando = false;
volatile bool pedirGravar = false, pedirParar = false;
volatile uint32_t bytesGravados = 0;
volatile uint32_t bytesLimite = 0;
String arquivoGravacao = "";
bool micOk = false;

void escreverCabecalhoWav(File& f, uint32_t dados) {
  uint8_t h[44];
  uint32_t taxa = AUDIO_TAXA, bps = AUDIO_BYTES_S, riff = dados + 36;
  memcpy(h, "RIFF", 4);  memcpy(h + 4, &riff, 4);  memcpy(h + 8, "WAVEfmt ", 8);
  uint32_t fmt = 16; uint16_t pcm = 1, canais = 1, bloco = 2, bits = 16;
  memcpy(h + 16, &fmt, 4); memcpy(h + 20, &pcm, 2); memcpy(h + 22, &canais, 2);
  memcpy(h + 24, &taxa, 4); memcpy(h + 28, &bps, 4); memcpy(h + 32, &bloco, 2); memcpy(h + 34, &bits, 2);
  memcpy(h + 36, "data", 4); memcpy(h + 40, &dados, 4);
  f.write(h, 44);
}

void corrigirCabecalho(const String& caminho) {
  File f = LittleFS.open(caminho, "r+");
  if (!f) return;
  uint32_t dados = f.size() > 44 ? f.size() - 44 : 0;
  f.seek(0);
  escreverCabecalhoWav(f, dados);
  f.close();
}

void iniciarMicrofone() {
  i2s_config_t cfg;
  memset(&cfg, 0, sizeof(cfg));
  cfg.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX);
  cfg.sample_rate = 16000;
  cfg.bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT;
  cfg.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
  cfg.communication_format = I2S_COMM_FORMAT_STAND_I2S;
  cfg.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
  cfg.dma_buf_count = 8;
  cfg.dma_buf_len = 256;
  cfg.use_apll = false;

  i2s_pin_config_t pinos;
  memset(&pinos, 0, sizeof(pinos));
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 0)
  pinos.mck_io_num = I2S_PIN_NO_CHANGE;
#endif
  pinos.bck_io_num = I2S_SCK;
  pinos.ws_io_num = I2S_WS;
  pinos.data_out_num = I2S_PIN_NO_CHANGE;
  pinos.data_in_num = I2S_SD;

  if (i2s_driver_install(I2S_NUM_0, &cfg, 0, NULL) == ESP_OK && i2s_set_pin(I2S_NUM_0, &pinos) == ESP_OK) {
    micOk = true;
    registrar("INFO", "Microfone INMP441 iniciado (I2S)");
  } else {
    registrar("ERRO", "Falha ao iniciar o microfone INMP441 (I2S)");
  }
}

void tarefaAudio(void*) {
  static int32_t amostras[512];
  static int16_t saida[128];
  File arq;
  String atual = "";
  int canal = 0;
  uint32_t proximaChecagem = 0;
  vTaskDelay(pdMS_TO_TICKS(200));
  for (;;) {
    if (!micOk) { vTaskDelay(pdMS_TO_TICKS(500)); continue; }
    size_t lidos = 0;
    i2s_read(I2S_NUM_0, amostras, sizeof(amostras), &lidos, pdMS_TO_TICKS(100));
    int n = lidos / sizeof(int32_t);
    if (n < 4) continue;

    long melhor = 0;
    for (int c = 0; c < 2; c++) {
      int64_t soma = 0; int qtd = 0;
      for (int i = c; i < n; i += 2) { soma += (amostras[i] >> 8); qtd++; }
      if (!qtd) continue;
      int64_t media = soma / qtd, desvio = 0;
      for (int i = c; i < n; i += 2) { int64_t d = (int64_t)(amostras[i] >> 8) - media; desvio += d < 0 ? -d : d; }
      long v = (long)(desvio / qtd);
      if (v > melhor) { melhor = v; canal = c; }
    }
    volume = melhor;
    somAlto = melhor > limiteSom;

    if (pedirGravar && !gravando) {
      pedirGravar = false;
      atual = arquivoGravacao;
      bool novo = !LittleFS.exists(atual);
      arq = LittleFS.open(atual, novo ? "w" : "a");
      if (arq) {
        if (novo) escreverCabecalhoWav(arq, 0);
        proximaChecagem = bytesGravados;
        gravando = true;
      }
    }
    if (!gravando) continue;

    int k = 0;
    for (int i = canal; i + 2 < n && k < 128; i += 4) {
      int32_t a = (amostras[i] >> 8), b = (amostras[i + 2] >> 8);
      int32_t v = ((a + b) / 2) / 256 * AUDIO_GANHO;
      if (v > 32767) v = 32767;
      if (v < -32768) v = -32768;
      saida[k++] = (int16_t)v;
    }
    arq.write((uint8_t*)saida, k * 2);
    bytesGravados += k * 2;
    if ((bytesGravados % 8192) < (uint32_t)(k * 2)) arq.flush();

    bool semEspaco = false;
    if (bytesGravados >= proximaChecagem) {
      proximaChecagem = bytesGravados + AUDIO_BYTES_S;
      semEspaco = LittleFS.totalBytes() - LittleFS.usedBytes() < 32768;
    }
    if (pedirParar || bytesGravados >= bytesLimite || semEspaco) {
      pedirParar = false;
      arq.close();
      corrigirCabecalho(atual);
      gravando = false;
    }
  }
}

void liberarEspaco(size_t necessario, const String& manter) {
  for (int volta = 0; volta < 20; volta++) {
    size_t livre = LittleFS.totalBytes() - LittleFS.usedBytes();
    if (livre >= necessario) return;
    File dir = LittleFS.open(AUDIO_DIR);
    String maisAntigo = "";
    if (dir) {
      File f = dir.openNextFile();
      while (f) {
        String nome = String(AUDIO_DIR) + "/" + String(f.name()).substring(String(f.name()).lastIndexOf('/') + 1);
        f.close();
        if (nome != manter && (maisAntigo == "" || nome < maisAntigo)) maisAntigo = nome;
        f = dir.openNextFile();
      }
      dir.close();
    }
    if (maisAntigo == "") return;
    LittleFS.remove(maisAntigo);
    registrar("INFO", "Audio antigo apagado para liberar espaco: " + maisAntigo);
  }
}

void comecarGravacao() {
  liberarEspaco(AUDIO_BLOCO_S * AUDIO_BYTES_S + 65536, arquivoAudio(ocorrencia));
  arquivoGravacao = arquivoAudio(ocorrencia);
  bytesGravados = 0;
  bytesLimite = AUDIO_BLOCO_S * AUDIO_BYTES_S;
  pedirGravar = true;
  registrar("INFO", "Gravacao de audio iniciada (" + String(AUDIO_BLOCO_S) + " s)");
}

void pararGravacao() { if (gravando) pedirParar = true; pedirGravar = false; }

void carregarNumOcorrencia() {
  if (!fsOk || !LittleFS.exists(OC_FILE)) return;
  File f = LittleFS.open(OC_FILE, "r");
  if (!f) return;
  long n = f.readStringUntil('\n').toInt();
  f.close();
  if (n > 0) numOcorrencia = (uint32_t)n;
}
void salvarNumOcorrencia() {
  if (!fsOk) return;
  File f = LittleFS.open(OC_FILE, "w");
  if (!f) return;
  f.println(numOcorrencia);
  f.close();
}

void salvarHistorico(uint32_t duracao, const char* desfecho, long resposta) {
  if (!fsOk) return;
  if (epochInicio == 0 && horaOk) epochInicio = time(nullptr) - (millis() - contagemDesde) / 1000;
  String inicio = inicioEmergencia.length() ? inicioEmergencia : inicioContagem;
  if (inicio.startsWith("sem-hora") && epochInicio) {
    struct tm t; localtime_r(&epochInicio, &t);
    char b[24]; strftime(b, sizeof(b), "%d/%m/%Y %H:%M:%S", &t);
    inicio = b;
  }
  File f = LittleFS.open(HIST_FILE, "a");
  if (!f) return;
  f.println(ocorrencia + "|" + NOME_MULHER + "|" + inicio + "|" + String(duracao) + "|" +
            origemAlerta + "|" + prioridade + "|" + (coacao ? "1" : "0") + "|" + desfecho + "|" +
            String(resposta) + "|" + String((long)epochInicio));
  f.close();
}

const char* nomeEstado() {
  if (estado == CONTAGEM) return "contagem";
  if (estado == EMERGENCIA) return "emergencia";
  return "normal";
}

void disparar(const String& origem) {
  if (estado != NORMAL) return;
  estado = CONTAGEM;
  numOcorrencia++;
  salvarNumOcorrencia();
  ocorrencia = "OC-" + String(numOcorrencia);
  origemAlerta = origem;
  contagemDesde = millis();
  inicioContagem = agoraStr();
  inicioEmergencia = "";
  epochInicio = horaOk ? time(nullptr) : 0;
  prioridade = "alta";
  coacao = false; aguardandoSenha = false; pedidoEncerrar = false;
  tentativasErradas = 0;
  limparLocalizacao();
  chat.clear();
  ultimoDesfecho = "";
  comecarGravacao();
  addChat("sistema", "Alerta acionado (" + origem + "). Aguardando 20 s para cancelamento.");
  registrar("AVISO", "ALERTA ACIONADO - " + ocorrencia + " - origem: " + origem);
  tgEnviar("⚠️ Seu alerta foi acionado.\nSe foi engano, toque em \"Cancelar alerta\" e digite sua senha. "
           "Você tem 20 segundos. Se não cancelar, a central será avisada.", TECLADO_PRE);
}

void confirmarEmergencia(const String& motivo) {
  if (estado != CONTAGEM) return;
  estado = EMERGENCIA;
  emergenciaDesde = millis();
  inicioEmergencia = agoraStr();
  aguardandoSenha = false;
  totalOcorrencias++;
  addChat("sistema", "Alerta enviado à central (" + motivo + ").");
  registrar("AVISO", "EMERGENCIA CONFIRMADA - " + ocorrencia + " - " + motivo);
  if (!coacao) tgEnviar("🔴 Alerta enviado à central.\nSe puder, vá para um lugar seguro. "
                        "Você pode mandar mensagens aqui.\n\n" + String(MSG_COMO_AO_VIVO), TECLADO_MENU);
}

void cancelarPelaUsuaria() {
  if (estado != CONTAGEM) return;
  estado = NORMAL;
  totalCancelamentos++;
  ultimoDesfecho = "cancelado_usuaria";
  limparLocalizacao();
  pararGravacao();
  salvarHistorico((millis() - contagemDesde) / 1000, "cancelado_usuaria", -1);
  arquivoParaApagar = arquivoAudio(ocorrencia);
  registrar("INFO", "Alerta " + ocorrencia + " cancelado pela usuaria (senha correta). Audio apagado");
  tgEnviar("✅ Alerta cancelado.");
}

void ativarCoacao(const String& motivo) {
  coacao = true;
  prioridade = "maxima";
  addChat("sistema", "COAÇÃO: " + motivo + ". Modo silencioso: não envie mensagens à usuária.");
  registrar("AVISO", "COACAO - " + ocorrencia + " - " + motivo);
}

void encerrarEmergencia(const String& desfecho, bool vaiParaHistorico, bool apagarAudio) {
  if (estado != EMERGENCIA) return;
  uint32_t duracao = (millis() - emergenciaDesde) / 1000;
  pararGravacao();
  long resposta = vaiParaHistorico ? (long)duracao : -1;
  salvarHistorico(duracao, vaiParaHistorico ? "concluido" : "falso_alarme", resposta);
  if (apagarAudio) arquivoParaApagar = arquivoAudio(ocorrencia);
  estado = NORMAL;
  ultimoDesfecho = desfecho;
  if (locAoVivo) tgUsuaria("Você já pode parar de compartilhar a localização (toque em \"Parar de compartilhar\").");
  limparLocalizacao();
  registrar("INFO", "Ocorrencia " + ocorrencia + " encerrada: " + desfecho + " (tempo " + String(duracao) + " s)");
  if (!coacao) tgEnviar(vaiParaHistorico ? "Atendimento encerrado pela central. Se precisar, segure o botão de novo."
                                         : "O alerta foi encerrado pela central.");
}

void atualizarEstado() {
  if (estado == CONTAGEM && millis() - contagemDesde >= JANELA_CANCELAR_MS)
    confirmarEmergencia(tentativasErradas > 0 ? "tempo esgotado apos senha incorreta" : "tempo de cancelamento esgotado");
  if (arquivoParaApagar.length() && !gravando && !pedirGravar) {
    if (fsOk) LittleFS.remove(arquivoParaApagar);
    arquivoParaApagar = "";
  }
}

uint32_t somDesde = 0, ultimoSomAlto = 0;
void verificarSom() {
  uint32_t agora = millis();
  if (somAlto) { if (somDesde == 0) somDesde = agora; ultimoSomAlto = agora; }
  else if (somDesde && agora - ultimoSomAlto > SOM_TOLERANCIA_MS) somDesde = 0;
  if (estado != NORMAL && prioridade != "maxima" && somDesde && agora - somDesde >= SOM_SUSTENTADO_MS) {
    prioridade = "maxima";
    addChat("sistema", "Som alto detectado pelo microfone (volume " + String(volume) + "). Prioridade máxima.");
    registrar("AVISO", "Som alto durante o alerta " + ocorrencia + " - prioridade maxima");
  }
}

bool botaoLido = HIGH, botaoEstavel = HIGH;
uint32_t botaoMudouEm = 0, botaoApertadoDesde = 0;
bool jaDisparouNesteAperto = false;

void lerBotao() {
  bool leitura = digitalRead(PINO_BOTAO);
  if (leitura != botaoLido) { botaoLido = leitura; botaoMudouEm = millis(); }
  if (millis() - botaoMudouEm > 50 && leitura != botaoEstavel) {
    botaoEstavel = leitura;
    if (botaoEstavel == LOW) { botaoApertadoDesde = millis(); jaDisparouNesteAperto = false; }
  }
  if (botaoEstavel == LOW && !jaDisparouNesteAperto && millis() - botaoApertadoDesde >= SEGURAR_DISPARO_MS) {
    jaDisparouNesteAperto = true;
    if (estado == NORMAL) {
      vibrar({300});
      disparar("botão físico (D22)");
    }
  }
}

bool ehSenha(const String& t) {
  if (t.length() != 4) return false;
  for (unsigned i = 0; i < t.length(); i++) if (!isDigit(t[i])) return false;
  return true;
}

void tratarSenha(const String& t) {
  if (estado == NORMAL) { tgEnviar("Não há alerta ativo no momento."); return; }
  if (estado == CONTAGEM) {
    if (t == SENHA_CANCELAR) { cancelarPelaUsuaria(); return; }
    if (t == SENHA_COACAO) {
      tgEnviar("✅ Alerta cancelado.");
      ativarCoacao("Cancelamento sob COAÇÃO (senha de coação)");
      confirmarEmergencia("senha de coacao");
      return;
    }
    tentativasErradas++;
    if (tentativasErradas >= TENTATIVAS_SENHA) {
      tgEnviar("Senha incorreta.");
      ativarCoacao(String(TENTATIVAS_SENHA) + " senhas incorretas seguidas");
      confirmarEmergencia("senhas incorretas");
    } else {
      tgEnviar("Senha incorreta. Tente de novo.");
      addChat("sistema", "Senha incorreta digitada (" + String(tentativasErradas) + "ª tentativa).");
    }
    return;
  }

  if (t == SENHA_CANCELAR) {
    pedidoEncerrar = true;
    addChat("sistema", "A usuária digitou a senha de cancelamento: pede para encerrar a ocorrência.");
    tgEnviar("Pedido de cancelamento enviado à central.");
  } else if (t == SENHA_COACAO) {
    tgEnviar("Pedido de cancelamento enviado à central.");
    ativarCoacao("Senha de COAÇÃO digitada durante o atendimento");
  } else {
    tgEnviar("Senha incorreta.");
  }
}

void tratarLocalizacao(const Entrada& e) {
  if (estado == NORMAL) {
    if (!e.editada) tgEnviar("Sua localização só é usada durante um alerta, por isso não foi guardada. "
                             "Se precisar de ajuda, segure o botão do dispositivo por 1 segundo.");
    return;
  }
  bool primeira = trajeto.empty();
  uint32_t agora = millis();
  if (trajeto.size() >= TRAJETO_MAX) trajeto.erase(trajeto.begin());
  trajeto.push_back({e.lat, e.lon, e.precisao, agora, ++locSeq});
  latUsuaria = e.lat; lonUsuaria = e.lon; horaLocalizacao = horaCurta();
  locUltimaMs = agora; locPrecisao = e.precisao;

  if (e.aoVivoS > 0) {
    if (!locAoVivo || !e.editada) {

      if (!e.editada) locAoVivoAte = e.aoVivoS > 86400L ? 0 : agora + (uint32_t)e.aoVivoS * 1000UL;
      if (!locAoVivo) {
        addChat("sistema", "A usuária começou a compartilhar a localização em tempo real.");
        registrar("INFO", "Localizacao AO VIVO iniciada - " + ocorrencia + " (" + String(e.aoVivoS / 60) + " min)");
      }
    }
    locAoVivo = true; locEncerrada = false;
    if (!e.editada) tgUsuaria("📡 Localização em tempo real recebida. A central está acompanhando você.");
  } else if (e.editada && locAoVivo) {
    locAoVivo = false; locEncerrada = true;
    addChat("sistema", "A usuária parou de compartilhar a localização em tempo real (última posição mantida).");
    registrar("AVISO", "Localizacao ao vivo encerrada pela usuaria - " + ocorrencia);
  } else if (!e.editada) {
    addChat("usuaria", "Localização enviada (posição fixa)");
    registrar("INFO", "Localizacao fixa recebida - " + ocorrencia + ": " + String(e.lat, 6) + ", " + String(e.lon, 6));
    tgUsuaria("📍 Localização recebida.\n" + String(MSG_COMO_AO_VIVO));
  }
  if (primeira) registrar("INFO", "Primeira localizacao da ocorrencia " + ocorrencia);
}

void verificarLocalizacao() {
  if (locAoVivo && locAoVivoAte && (int32_t)(millis() - locAoVivoAte) > 0) {
    locAoVivo = false; locEncerrada = true;
    addChat("sistema", "O tempo de compartilhamento em tempo real escolhido pela usuária terminou.");
    registrar("AVISO", "Localizacao ao vivo expirou - " + ocorrencia);
    tgUsuaria("O compartilhamento da sua localização terminou. Se ainda precisar de ajuda, compartilhe de novo:\n" + String(MSG_COMO_AO_VIVO));
  }
}

const char* statusLocalizacao() {
  if (trajeto.empty()) return "sem_localizacao";
  if (locAoVivo) return millis() - locUltimaMs > LOC_ATRASO_MS ? "sem_atualizacao" : "ao_vivo";
  if (locEncerrada) return "encerrada";
  return "fixa";
}

void processarEntrada(const Entrada& e) {
  if (e.chat != CHAT_ID_USUARIA) {
    registrar("AVISO", "Telegram: mensagem de um chat desconhecido foi ignorada (" + e.chat + ")");
    return;
  }
  if (e.tipo == ENT_BOTAO) {
    if (e.texto == "cancelar") {
      if (estado == CONTAGEM) { aguardandoSenha = true; tgEnviar("Digite sua senha de 4 dígitos para cancelar."); }
      else tgEnviar("Não há alerta aguardando cancelamento.");
    } else if (e.texto == "enviar") {
      if (estado == CONTAGEM) confirmarEmergencia("usuaria pediu envio imediato");
    }
    return;
  }
  if (e.tipo == ENT_LOCAL) { tratarLocalizacao(e); return; }

  String t = e.texto;
  t.trim();
  if (t.startsWith("/start")) {
    tgEnviar("Olá! Este é o seu canal do HELP ME.\n\n"
             "• Para pedir ajuda: segure o botão do dispositivo por 1 segundo.\n"
             "• Para cancelar um alerta: digite sua senha.\n"
             "• Durante um alerta, escreva aqui para falar com a central.\n\n"
             "/status – ver o dispositivo\n/teste – testar sem avisar a central\n/ajuda – números de emergência", TECLADO_MENU);
    return;
  }
  if (t == "/ajuda") {
    tgEnviar("🚨 Emergência: 190 (Polícia Militar)\n🚑 192 (SAMU)\n📞 180 (Central de Atendimento à Mulher, 24 h, gratuito)");
    return;
  }
  if (t == "/status") {
    String est = estado == NORMAL ? "sem alerta" : (estado == CONTAGEM ? "alerta aguardando cancelamento" : "alerta ativo na central");
    tgEnviar("📟 Dispositivo ligado há " + String(millis() / 60000) + " min.\nWi-Fi: sinal " + String(WiFi.RSSI()) +
             " dBm.\nMicrofone: " + (micOk ? "ok" : "com falha") + ".\nSituação: " + est + ".");
    return;
  }
  if (t == "/teste") {
    registrar("INFO", "Teste do dispositivo pedido pela usuaria");
    tgEnviar("🧪 Teste: o dispositivo está funcionando.\nWi-Fi: sinal " + String(WiFi.RSSI()) + " dBm. Microfone: volume " +
             String(volume) + ".\nNada foi enviado à central.");
    return;
  }
  if (ehSenha(t) && (estado != NORMAL || aguardandoSenha)) { tratarSenha(t); return; }
  if (t == "Estou bem") {
    if (estado != NORMAL) addChat("usuaria", "Estou bem");
    tgEnviar(estado != NORMAL ? "Mensagem enviada à central." : "Que bom. Se precisar, segure o botão do dispositivo.");
    return;
  }
  if (t == "Preciso de ajuda") {
    if (estado == CONTAGEM) confirmarEmergencia("usuaria pediu ajuda");
    if (estado != NORMAL) {
      prioridade = "maxima";
      addChat("usuaria", "Preciso de ajuda");
      registrar("AVISO", "Usuaria pediu ajuda pelo Telegram - prioridade maxima");
      if (!coacao) tgEnviar("A central foi avisada. Fique na linha.");
    } else {
      disparar("pedido de ajuda pelo Telegram");
    }
    return;
  }
  if (estado != NORMAL) {
    addChat("usuaria", t);
    registrar("INFO", "Mensagem da usuaria: " + t);
  } else {
    registrar("INFO", "Mensagem da usuaria (sem alerta): " + t);
    tgEnviar("Mensagem registrada. Para pedir ajuda, segure o botão do dispositivo por 1 segundo.");
  }
}

String chatParaTexto(JsonVariant v) {
  char b[24];
  snprintf(b, sizeof(b), "%lld", v.as<long long>());
  return String(b);
}

void tarefaTelegram(void*) {
  WiFiClientSecure cliente;
  cliente.setInsecure();
  HTTPClient http;
  http.setReuse(true);
  long offset = 0;
  String base = String("https://api.telegram.org/bot") + TELEGRAM_TOKEN + "/";
  for (;;) {
    if (WiFi.status() != WL_CONNECTED || strlen(TELEGRAM_TOKEN) < 20) {
      telegramOk = false;
      vTaskDelay(pdMS_TO_TICKS(2000));
      continue;
    }

    Saida s;
    while (tirarSaida(s)) {
      if (http.begin(cliente, base + s.metodo)) {
        http.addHeader("Content-Type", "application/json");
        int cod = http.POST(s.json);
        telegramOk = cod == 200;
        http.end();
      }
    }

    if (http.begin(cliente, base + "getUpdates?timeout=2&limit=10&allowed_updates=%5B%22message%22,%22edited_message%22,%22callback_query%22%5D&offset=" + String(offset))) {
      http.setTimeout(8000);
      int cod = http.GET();
      if (cod == 200) {
        telegramOk = true;
        JsonDocument doc;
        if (!deserializeJson(doc, http.getString())) {
          for (JsonObject u : doc["result"].as<JsonArray>()) {
            offset = u["update_id"].as<long>() + 1;
            if (u["callback_query"].is<JsonObject>()) {
              JsonObject cb = u["callback_query"];
              Entrada e{ENT_BOTAO, chatParaTexto(cb["message"]["chat"]["id"]), String((const char*)(cb["data"] | "")), 0, 0, 0, 0, false};
              porEntrada(e);
              JsonDocument r;
              r["callback_query_id"] = cb["id"];
              String j; serializeJson(r, j);
              porSaida("answerCallbackQuery", j);
            } else if (u["message"].is<JsonObject>() || u["edited_message"].is<JsonObject>()) {
              bool editada = !u["message"].is<JsonObject>();
              JsonObject m = editada ? u["edited_message"] : u["message"];
              String chatId = chatParaTexto(m["chat"]["id"]);
              if (m["location"].is<JsonObject>()) {
                JsonObject l = m["location"];
                Entrada e{ENT_LOCAL, chatId, "", l["latitude"].as<double>(), l["longitude"].as<double>(),
                          l["horizontal_accuracy"].as<float>(), l["live_period"].as<long>(), editada};
                porEntrada(e);
              } else if (!editada && m["text"].is<const char*>()) {
                Entrada e{ENT_TEXTO, chatId, String(m["text"].as<const char*>()), 0, 0, 0, 0, false};
                porEntrada(e);
              }
            }
          }
        }
      } else {
        telegramOk = false;
      }
      http.end();
    }
    vTaskDelay(pdMS_TO_TICKS(300));
  }
}

bool wifiEstavaConectado = false;
uint32_t ultimaTentativa = 0;

void aoConectar() {
  wifiEstavaConectado = true;
  registrar("INFO", "Wi-Fi conectado. IP=" + WiFi.localIP().toString() + " MAC=" + WiFi.macAddress());
  Serial.println("Abra no navegador: http://" + WiFi.localIP().toString());
  configTime(-3 * 3600, 0, "pool.ntp.org", "a.st1.ntp.br", "time.google.com");
}

void conectarWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(WIFI_SSID, WIFI_SENHA);
  Serial.print("Conectando ao Wi-Fi");
  uint32_t t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 15000) { delay(500); Serial.print("."); }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) aoConectar();
  else registrar("ERRO", "Nao foi possivel conectar ao Wi-Fi. Nova tentativa a cada 10 s");
  ultimaTentativa = millis();
}

void verificarWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    if (!wifiEstavaConectado) aoConectar();
    return;
  }
  if (wifiEstavaConectado) {
    wifiEstavaConectado = false;
    registrar("AVISO", "Conexao Wi-Fi perdida. Tentando reconectar...");
    ultimaTentativa = millis();
  }
  if (millis() - ultimaTentativa >= RECONECTAR_MS) {
    ultimaTentativa = millis();
    Serial.println("Tentando reconectar ao Wi-Fi...");
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_SENHA);
  }
}

void cabecalhos() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Cache-Control", "no-store");
}
void enviarJsonCodigo(int codigo, const String& json) {
  cabecalhos();
  server.send(codigo, "application/json", json);
}
void enviarJson(const String& json) { enviarJsonCodigo(200, json); }
String aspas(const String& s) {
  String r = s;
  r.replace("\\", "\\\\");
  r.replace("\"", "\\\"");
  r.replace("\n", "\\n");
  return "\"" + r + "\"";
}
String jsonOk(bool ok) {
  return String("{\"ok\":") + (ok ? "true" : "false") + ",\"estado\":\"" + nomeEstado() + "\"}";
}
String jb(bool v) { return v ? "true" : "false"; }

void rotaPagina() {
  server.sendHeader("Cache-Control", "no-store");
  server.send_P(200, "text/html; charset=utf-8", PAGINA_HTML);
}

void rotaLocalizacao() {
  uint32_t desde = server.hasArg("desde") ? (uint32_t)server.arg("desde").toInt() : 0;
  uint32_t agora = millis();
  String j = "{\"ocorrencia\":" + aspas(ocorrencia) + ",\"estado\":" + aspas(nomeEstado()) +
             ",\"status\":" + aspas(statusLocalizacao()) + ",\"coacao\":" + jb(coacao) +
             ",\"ultimo_seq\":" + String(locSeq) +
             ",\"ultima_ha_s\":" + String(trajeto.empty() ? -1L : (long)((agora - locUltimaMs) / 1000)) +
             ",\"expira_em_s\":" + String(locAoVivo && locAoVivoAte ? (long)((int32_t)(locAoVivoAte - agora) / 1000) : -1L) +
             ",\"hora\":" + aspas(horaLocalizacao) + ",\"pontos\":[";
  bool primeiro = true;
  for (const Ponto& p : trajeto) {
    if (p.seq <= desde) continue;
    if (!primeiro) j += ",";
    primeiro = false;
    j += "{\"seq\":" + String(p.seq) + ",\"lat\":" + String(p.lat, 6) + ",\"lon\":" + String(p.lon, 6) +
         ",\"prec\":" + String((int)p.precisao) + ",\"idade_s\":" + String((agora - p.ms) / 1000) + "}";
  }
  j += "]}";
  enviarJson(j);
}

void rotaPedirLocalizacao() {
  bool ok = estado != NORMAL && !coacao;
  if (ok) {
    tgEnviar("📍 A central pediu sua localização em tempo real.\n" + String(MSG_COMO_AO_VIVO), TECLADO_MENU);
    addChat("sistema", "Central pediu a localização em tempo real à usuária.");
  }
  enviarJsonCodigo(ok ? 200 : 409, ok ? jsonOk(true) : String("{\"ok\":false,\"erro\":\"") + (coacao ? "modo silencioso (coacao)" : "sem alerta ativo") + "\"}");
}

void rotaStatus() {
  uint32_t restante = 0;
  if (estado == CONTAGEM) {
    uint32_t passou = millis() - contagemDesde;
    restante = passou >= JANELA_CANCELAR_MS ? 0 : (JANELA_CANCELAR_MS - passou + 999) / 1000;
  }
  size_t livre = fsOk ? LittleFS.totalBytes() - LittleFS.usedBytes() : 0;
  String j = "{";
  j += "\"estado\":" + aspas(nomeEstado());
  j += ",\"alerta\":" + jb(estado == EMERGENCIA);
  j += ",\"restante_s\":" + String(restante);
  j += ",\"ocorrencia\":" + aspas(ocorrencia);
  j += ",\"origem\":" + aspas(origemAlerta);
  j += ",\"inicio_contagem\":" + aspas(inicioContagem);
  j += ",\"inicio_emergencia\":" + aspas(inicioEmergencia);
  j += ",\"prioridade\":" + aspas(prioridade);
  j += ",\"coacao\":" + jb(coacao);
  j += ",\"pedido_encerrar\":" + jb(pedidoEncerrar);
  j += ",\"ultimo_desfecho\":" + aspas(ultimoDesfecho);
  j += ",\"lat\":" + String(latUsuaria, 6) + ",\"lon\":" + String(lonUsuaria, 6);
  j += ",\"hora_localizacao\":" + aspas(horaLocalizacao);
  j += ",\"loc_status\":" + aspas(statusLocalizacao());
  j += ",\"chat_n\":" + String((unsigned)chat.size());
  j += ",\"gravando\":" + jb(gravando || pedirGravar);
  j += ",\"audio_s\":" + String(bytesGravados / AUDIO_BYTES_S);
  j += ",\"audio_limite_s\":" + String(bytesLimite / AUDIO_BYTES_S);
  j += ",\"nome\":" + aspas(NOME_MULHER);
  j += ",\"id\":" + aspas(DEVICE_ID);
  j += ",\"conectado\":" + jb(WiFi.status() == WL_CONNECTED);
  j += ",\"ssid\":" + aspas(WiFi.SSID());
  j += ",\"ip\":" + aspas(WiFi.localIP().toString());
  j += ",\"mac\":" + aspas(WiFi.macAddress());
  j += ",\"rssi\":" + String(WiFi.RSSI());
  j += ",\"uptime_s\":" + String(millis() / 1000);
  j += ",\"hora\":" + aspas(agoraStr());
  j += ",\"hora_ok\":" + jb(horaOk);
  j += ",\"volume\":" + String(volume);
  j += ",\"limite\":" + String(limiteSom);
  j += ",\"som_alto\":" + jb(somAlto);
  j += ",\"botao\":" + jb(botaoEstavel == LOW);
  j += ",\"motor\":" + jb(motorLigado);
  j += ",\"telegram_ok\":" + jb(telegramOk);
  j += ",\"flash_livre_kb\":" + String((unsigned)(livre / 1024));
  j += ",\"total_ocorrencias\":" + String(totalOcorrencias);
  j += ",\"total_cancelamentos\":" + String(totalCancelamentos);
  j += "}";
  enviarJson(j);
}

void rotaSensor() {
  enviarJson("{\"volume\":" + String(volume) + ",\"limite\":" + String(limiteSom) + ",\"som_alto\":" + jb(somAlto) +
             ",\"botao\":" + jb(botaoEstavel == LOW) + ",\"gravando\":" + jb(gravando) + "}");
}

void rotaAtuador() {
  String e = server.arg("estado");
  if (e == "on") {
    nPassos = 0; motorManual = true; motorManualDesde = millis(); motorSet(true);
    registrar("INFO", "Motor ligado pela rede");
  } else if (e == "off") {
    nPassos = 0; motorManual = false; motorSet(false);
    registrar("INFO", "Motor desligado pela rede");
  } else if (e == "pulso") {
    vibrar({500});
    registrar("INFO", "Pulso de 0,5 s no motor pela rede");
  } else {
    enviarJsonCodigo(400, "{\"ok\":false,\"erro\":\"use estado=on, off ou pulso\"}");
    return;
  }
  enviarJson("{\"ok\":true,\"motor\":" + jb(motorLigado) + "}");
}

void rotaSimular() {
  if (estado != NORMAL) { enviarJson(jsonOk(false)); return; }
  disparar("botão de teste do painel");
  enviarJson(jsonOk(true));
}
void rotaGravarMais() {
  bool ok = estado != NORMAL;
  if (ok) {
    liberarEspaco(AUDIO_BLOCO_S * AUDIO_BYTES_S + 65536, arquivoAudio(ocorrencia));
    bytesLimite = bytesGravados + AUDIO_BLOCO_S * AUDIO_BYTES_S;
    if (!gravando) { arquivoGravacao = arquivoAudio(ocorrencia); pedirGravar = true; }
    addChat("sistema", "Central pediu mais " + String(AUDIO_BLOCO_S) + " s de gravação.");
    registrar("INFO", "Gravacao estendida em " + String(AUDIO_BLOCO_S) + " s - " + ocorrencia);
  }
  enviarJson(jsonOk(ok));
}
void rotaResolver() {
  bool ok = estado == EMERGENCIA;
  encerrarEmergencia("atendimento concluido", true, false);
  enviarJson(jsonOk(ok));
}
void rotaFalso() {
  bool ok = estado == EMERGENCIA;
  if (ok) { totalCancelamentos++; encerrarEmergencia("falso alarme (cancelado pela central)", false, true); }
  enviarJson(jsonOk(ok));
}

void rotaChat() {
  String j = "{\"ocorrencia\":" + aspas(ocorrencia) + ",\"coacao\":" + jb(coacao) + ",\"msgs\":[";
  for (size_t i = 0; i < chat.size(); i++) {
    if (i) j += ",";
    j += "{\"de\":" + aspas(chat[i].de) + ",\"texto\":" + aspas(chat[i].texto) + ",\"hora\":" + aspas(chat[i].hora) + "}";
  }
  j += "]}";
  enviarJson(j);
}
void rotaChatEnviar() {
  String texto = server.arg("texto");
  texto.trim();
  if (estado == NORMAL || texto.length() == 0) { enviarJson("{\"ok\":false,\"erro\":\"sem ocorrencia ou texto vazio\"}"); return; }
  if (coacao) { enviarJson("{\"ok\":false,\"erro\":\"modo silencioso (coacao)\"}"); return; }
  addChat("central", texto);
  registrar("INFO", "Mensagem da central para a usuaria: " + texto);
  tgEnviar("💬 Central: " + texto);
  enviarJson("{\"ok\":true}");
}

void rotaAudio() {
  String oc = server.arg("oc");
  if (oc.length() == 0 || oc.indexOf('/') >= 0 || oc.indexOf('.') >= 0) { enviarJsonCodigo(400, "{\"ok\":false}"); return; }
  String caminho = arquivoAudio(oc);
  if (!fsOk || !LittleFS.exists(caminho)) { enviarJsonCodigo(404, "{\"ok\":false,\"erro\":\"audio nao encontrado\"}"); return; }
  File f = LittleFS.open(caminho, "r");
  size_t total = f.size();
  uint32_t dados = total > 44 ? total - 44 : 0;
  dados -= dados % 2;
  cabecalhos();
  server.setContentLength(44 + dados);
  server.send(200, "audio/wav", "");

  uint8_t h[44];
  uint32_t taxa = AUDIO_TAXA, bps = AUDIO_BYTES_S, riff = dados + 36, fmt = 16;
  uint16_t pcm = 1, canais = 1, bloco = 2, bits = 16;
  memcpy(h, "RIFF", 4);  memcpy(h + 4, &riff, 4);  memcpy(h + 8, "WAVEfmt ", 8);
  memcpy(h + 16, &fmt, 4); memcpy(h + 20, &pcm, 2); memcpy(h + 22, &canais, 2);
  memcpy(h + 24, &taxa, 4); memcpy(h + 28, &bps, 4); memcpy(h + 32, &bloco, 2); memcpy(h + 34, &bits, 2);
  memcpy(h + 36, "data", 4); memcpy(h + 40, &dados, 4);
  server.sendContent((const char*)h, 44);
  f.seek(44);
  static uint8_t buf[1024];
  uint32_t faltam = dados;
  while (faltam > 0) {
    size_t n = f.read(buf, faltam < sizeof(buf) ? faltam : sizeof(buf));
    if (n == 0) break;
    server.sendContent((const char*)buf, n);
    faltam -= n;
  }
  f.close();
}

String campo(const String& linha, int indice) {
  int inicio = 0;
  for (int i = 0; i < indice; i++) { inicio = linha.indexOf('|', inicio); if (inicio < 0) return ""; inicio++; }
  int fim = linha.indexOf('|', inicio);
  return fim < 0 ? linha.substring(inicio) : linha.substring(inicio, fim);
}

void rotaHistorico() {
  String linhas[30];
  int n = 0;
  if (fsOk && LittleFS.exists(HIST_FILE)) {
    File f = LittleFS.open(HIST_FILE, "r");
    while (f && f.available()) {
      String l = f.readStringUntil('\n');
      l.trim();
      if (l.length() == 0) continue;
      linhas[n % 30] = l;
      n++;
    }
    if (f) f.close();
  }
  int total = n < 30 ? n : 30;
  String j = "[";
  for (int i = 0; i < total; i++) {
    const String& l = linhas[(n - 1 - i) % 30];
    String cod = campo(l, 0);
    if (i) j += ",";
    j += "{\"codigo\":" + aspas(cod) + ",\"nome\":" + aspas(campo(l, 1)) + ",\"inicio\":" + aspas(campo(l, 2)) +
         ",\"duracao_s\":" + String(campo(l, 3).toInt()) + ",\"origem\":" + aspas(campo(l, 4)) +
         ",\"prioridade\":" + aspas(campo(l, 5)) + ",\"coacao\":" + jb(campo(l, 6) == "1") +
         ",\"desfecho\":" + aspas(campo(l, 7).length() ? campo(l, 7) : String("concluido")) +
         ",\"resposta_s\":" + String(campo(l, 8).length() ? campo(l, 8).toInt() : -1) +
         ",\"audio\":" + jb(fsOk && LittleFS.exists(arquivoAudio(cod))) + "}";
  }
  j += "]";
  enviarJson(j);
}

struct Periodo { int total = 0, canc_usuaria = 0, canc_central = 0, nResp = 0; long somaResp = 0; };

time_t epochDaLinha(const String& l) {
  long e = campo(l, 9).toInt();
  if (e > 0) return (time_t)e;
  int d, mo, a, h, mi, se;
  if (sscanf(campo(l, 2).c_str(), "%d/%d/%d %d:%d:%d", &d, &mo, &a, &h, &mi, &se) == 6) {
    struct tm t = {};
    t.tm_mday = d; t.tm_mon = mo - 1; t.tm_year = a - 1900; t.tm_hour = h; t.tm_min = mi; t.tm_sec = se;
    t.tm_isdst = 0;
    return mktime(&t);
  }
  return 0;
}

void somar(Periodo& p, const String& desf, long resp) {
  p.total++;
  if (desf == "cancelado_usuaria") p.canc_usuaria++;
  else if (desf == "falso_alarme") p.canc_central++;
  if (resp >= 0) { p.nResp++; p.somaResp += resp; }
}

String jsonPeriodo(const Periodo& p, const String* labels, const int* valores, int n) {
  String j = "{\"labels\":[";
  for (int i = 0; i < n; i++) { if (i) j += ","; j += aspas(labels[i]); }
  j += "],\"valores\":[";
  for (int i = 0; i < n; i++) { if (i) j += ","; j += String(valores[i]); }
  j += "],\"total\":" + String(p.total) +
       ",\"cancel_usuaria\":" + String(p.canc_usuaria) + ",\"cancel_central\":" + String(p.canc_central) +
       ",\"resp_media_s\":" + String(p.nResp ? p.somaResp / p.nResp : -1) + "}";
  return j;
}

void rotaEstatisticas() {
  Periodo geral, semana, mes, semanaPassada;
  int hoje = 0, ontem = 0, concluidosHoje = 0, concluidos = 0;
  int dias[7] = {0}, semanas[4] = {0};
  time_t hoje0 = 0;
  struct tm agora = {};
  if (horaOk) {
    time_t n = time(nullptr);
    localtime_r(&n, &agora);
    struct tm m = agora; m.tm_hour = 0; m.tm_min = 0; m.tm_sec = 0;
    hoje0 = mktime(&m);
  }
  if (fsOk && LittleFS.exists(HIST_FILE)) {
    File f = LittleFS.open(HIST_FILE, "r");
    while (f && f.available()) {
      String l = f.readStringUntil('\n');
      l.trim();
      if (l.length() == 0) continue;
      String desf = campo(l, 7); if (!desf.length()) desf = "concluido";
      long resp = campo(l, 8).length() ? campo(l, 8).toInt() : -1;
      somar(geral, desf, resp);
      if (desf == "concluido") concluidos++;
      if (!hoje0) continue;
      time_t e = epochDaLinha(l);
      if (e <= 0 || e > hoje0 + 86400) continue;
      long diasAtras = e >= hoje0 ? 0 : (long)((hoje0 - e + 86399) / 86400);
      if (diasAtras == 0) { hoje++; if (desf == "concluido") concluidosHoje++; }
      if (diasAtras == 1) ontem++;
      if (diasAtras < 7) { dias[6 - diasAtras]++; somar(semana, desf, resp); }
      else if (diasAtras < 14) somar(semanaPassada, desf, resp);
      if (diasAtras < 28) { semanas[3 - diasAtras / 7]++; somar(mes, desf, resp); }
    }
    if (f) f.close();
  }
  static const char* nomes[7] = {"Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sáb"};
  String lDias[7];
  for (int i = 0; i < 7; i++) lDias[i] = horaOk ? (i == 6 ? String("Hoje") : String(nomes[(agora.tm_wday + 1 + i) % 7])) : String("-");
  String lSem[4] = {"Há 3 sem.", "Há 2 sem.", "Sem. passada", "Esta semana"};

  String j = "{\"hora_ok\":" + jb(horaOk) +
             ",\"total\":" + String(geral.total) + ",\"concluidos\":" + String(concluidos) +
             ",\"cancel_usuaria\":" + String(geral.canc_usuaria) + ",\"cancel_central\":" + String(geral.canc_central) +
             ",\"resp_media_s\":" + String(geral.nResp ? geral.somaResp / geral.nResp : -1) +
             ",\"resp_media_semana_passada_s\":" + String(semanaPassada.nResp ? semanaPassada.somaResp / semanaPassada.nResp : -1) +
             ",\"hoje\":" + String(hoje) + ",\"ontem\":" + String(ontem) + ",\"concluidos_hoje\":" + String(concluidosHoje) +
             ",\"semana\":" + jsonPeriodo(semana, lDias, dias, 7) +
             ",\"mes\":" + jsonPeriodo(mes, lSem, semanas, 4) + "}";
  enviarJson(j);
}

void rotaLog() {
  cabecalhos();
  if (!fsOk || !LittleFS.exists(LOG_FILE)) { server.send(200, "text/plain; charset=utf-8", ""); return; }
  File f = LittleFS.open(LOG_FILE, "r");
  String tudo = f.readString();
  f.close();
  if (server.hasArg("ultimas")) {
    int qtd = server.arg("ultimas").toInt();
    if (qtd < 1) qtd = 1;
    int pos = tudo.length();
    if (pos > 0 && tudo[pos - 1] == '\n') pos--;
    while (qtd > 0 && pos > 0) { pos = tudo.lastIndexOf('\n', pos - 1); qtd--; if (pos < 0) { pos = -1; break; } }
    tudo = tudo.substring(pos + 1);
  }
  server.send(200, "text/plain; charset=utf-8", tudo);
}

void rotaLogLimpar() {
  if (fsOk) { LittleFS.remove(LOG_FILE); LittleFS.remove(LOG_OLD); }
  registrar("INFO", "Log limpo pela rede");
  enviarJson("{\"ok\":true}");
}

void rotaLimite() {
  long v = server.arg("valor").toInt();
  if (v < 1000) { enviarJsonCodigo(400, "{\"ok\":false,\"erro\":\"valor minimo 1000\"}"); return; }
  limiteSom = v;
  registrar("INFO", "Limite de som alterado para " + String(v));
  enviarJson("{\"ok\":true,\"limite\":" + String(limiteSom) + "}");
}

void rotaNaoEncontrada() {
  registrar("AVISO", "Rota nao encontrada: " + server.uri());
  enviarJsonCodigo(404, "{\"ok\":false,\"erro\":\"rota nao encontrada\"}");
}

void iniciarServidor() {
  server.on("/", HTTP_GET, rotaPagina);
  server.on("/status", HTTP_GET, rotaStatus);
  server.on("/sensor", HTTP_GET, rotaSensor);
  server.on("/atuador", HTTP_GET, rotaAtuador);
  server.on("/simular", HTTP_GET, rotaSimular);
  server.on("/gravar_mais", HTTP_GET, rotaGravarMais);
  server.on("/resolver", HTTP_GET, rotaResolver);
  server.on("/falso", HTTP_GET, rotaFalso);
  server.on("/chat", HTTP_GET, rotaChat);
  server.on("/chat/enviar", HTTP_GET, rotaChatEnviar);
  server.on("/audio", HTTP_GET, rotaAudio);
  server.on("/historico", HTTP_GET, rotaHistorico);
  server.on("/estatisticas", HTTP_GET, rotaEstatisticas);
  server.on("/localizacao", HTTP_GET, rotaLocalizacao);
  server.on("/pedir_localizacao", HTTP_GET, rotaPedirLocalizacao);
  server.on("/log", HTTP_GET, rotaLog);
  server.on("/log/limpar", HTTP_GET, rotaLogLimpar);
  server.on("/limite", HTTP_GET, rotaLimite);
  server.on("/favicon.ico", HTTP_GET, []() { server.send(204); });
  server.onNotFound(rotaNaoEncontrada);
  server.begin();
  registrar("INFO", "Servidor HTTP escutando na porta 80");
}

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println();
  Serial.println("=== HELP ME - FIRMWARE FINAL v3.0 ===");

  pinMode(PINO_BOTAO, INPUT_PULLUP);
  pinMode(PINO_MOTOR, OUTPUT);
  motorSet(false);
  filaMutex = xSemaphoreCreateMutex();

  fsOk = LittleFS.begin(true);
  if (!fsOk) Serial.println("[ERRO] Falha ao montar a memoria flash (LittleFS)");
  else if (!LittleFS.exists(AUDIO_DIR)) LittleFS.mkdir(AUDIO_DIR);
  registrar("INFO", "Sistema iniciado. Flash livre: " +
            String(fsOk ? (unsigned)((LittleFS.totalBytes() - LittleFS.usedBytes()) / 1024) : 0) + " KB");
  carregarNumOcorrencia();

  conectarWiFi();
  iniciarServidor();

  iniciarMicrofone();
  xTaskCreatePinnedToCore(tarefaAudio, "audio", 6144, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(tarefaTelegram, "telegram", 12288, NULL, 1, NULL, 0);
}

void loop() {
  server.handleClient();
  verificarWiFi();
  verificarHora();
  lerBotao();
  verificarSom();
  Entrada e;
  while (tirarEntrada(e)) processarEntrada(e);
  atualizarEstado();
  verificarLocalizacao();
  atualizarMotor();
  delay(2);
}
