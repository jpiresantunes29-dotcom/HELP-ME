# Clientes do HELP ME

O ESP32 é o servidor. Três tipos de cliente falam com ele:

| Cliente | Como acessa | Uso |
|---|---|---|
| Navegador | `http://IP_DO_ESP32/` ou [index.html](index.html) | Central de segurança (página servida pelo próprio ESP32) |
| Telegram | Bot "Lembretes da Farmácia" | Canal da usuária: alerta, cancelamento com senha, chat e localização |
| Scripts desta pasta | HTTP na porta 80 | Testes de rede e das rotas da API |

## Página da central

[`index.html`](index.html) é a mesma página que o firmware guarda em `PAGINA_HTML` e entrega na rota `/`. O jeito normal de usar é abrir `http://IP_DO_ESP32/`. Se o arquivo for aberto direto no navegador, ele chama o ESP32 do IP definido na constante `ESP` (hoje `172.20.10.12`), o que funciona porque o servidor responde com `Access-Control-Allow-Origin: *`.

## Scripts de teste

Os dois scripts fazem a mesma sequência: ping, porta 80, rotas de dados, rota inexistente (404), atuador (`on`, `off` e `pulso`) e um teste de estresse com requisições seguidas em `/status`.

O computador precisa estar na mesma rede do ESP32. O IP aparece no Monitor Serial e na central.

### Windows (PowerShell)

```powershell
powershell -ExecutionPolicy Bypass -File .\testar_central.ps1 -IP 172.20.10.12
powershell -ExecutionPolicy Bypass -File .\testar_central.ps1 -IP 172.20.10.12 -Requisicoes 200
```

### macOS e Linux (bash)

```bash
chmod +x testar_central.sh
./testar_central.sh 172.20.10.12
./testar_central.sh 172.20.10.12 200
```

### Resultado esperado

| Etapa | Resultado |
|---|---|
| Ping | Respostas sem perda |
| Porta 80 | `TcpTestSucceeded: True` (Windows) ou `succeeded` (nc) |
| Rotas de dados | JSON em `/status`, `/sensor`, `/historico` e `/estatisticas`; texto em `/log` |
| Rota inexistente | `HTTP 404` |
| Atuador | `{"ok":true,...}` e o motor liga, desliga e pulsa |
| Estresse | Todas as requisições com sucesso |
