# HELP ME

**Dispositivo de alerta para vítimas de violência doméstica com ESP32.**

Projeto do TDE da disciplina **Conectividade em Sistemas Ciberfísicos**, curso de Engenharia de Software da PUCPR, 2026. Professor orientador: **Fábio Bettio**.

A usuária segura um botão por 1 segundo. O dispositivo vibra e abre um pré-alerta de 20 segundos no Telegram. Se ela não cancelar com a senha, a central de segurança recebe a ocorrência com áudio gravado, conversa e localização. O ESP32 é o **servidor HTTP na porta 80** que entrega a central para qualquer navegador da rede local.

**Formato do protótipo:** nesta fase, o conjunto foi acondicionado em uma caixa preta de bancada, com o botão de acionamento na face superior, solução que favorece a demonstração e o acesso à eletrônica. Em uma etapa futura, quando o projeto evoluir para um produto comercial, a proposta é miniaturizá-lo em um objeto discreto e compacto.

<p align="center">
  <img src="docs/img/prototipo.jpg" width="260" alt="Protótipo do HELP ME">
  <img src="docs/img/central_visao_geral.png" width="520" alt="Central de segurança">
</p>

## Vídeo de contextualização

**[Assistir ao vídeo de contextualização (58 s)](https://drive.google.com/file/d/1uBNcnztY9wLcF3GwKnJ3Ux2KRlkfrE1R/view?usp=drivesdk)**

Vídeo gravado pela equipe em 16/11/2026 para abrir a apresentação final, com o cenário da violência doméstica no Brasil e a proposta do HELP ME. O arquivo também está em [docs/video-contexto](docs/video-contexto/video_contextualizacao.mp4).

## Vídeo de demonstração

**[Assistir ao vídeo completo (2 min 54 s)](https://drive.google.com/file/d/1CQPIhX4oUAhRQfv2Jqt-ol8FbV0izJc4/view?usp=drivesdk)**

| Trecho | O que aparece |
|---|---|
| 0:06 a 0:54 | Montagem, microfone no Monitor Serial, motor, botão acionando o motor e teste integrado |
| 0:54 a 1:30 | Pré-alerta na central, Telegram da usuária, cancelamento com senha e novo acionamento |
| 1:36 a 1:54 | Ocorrência em andamento com dados da usuária, áudio gravado, chat e posição no mapa |
| 2:00 a 2:30 | Motor acionado pela central e acionamento da PM e do SAMU |
| 2:30 a 2:48 | Encerramento, histórico e estatísticas |

## Etapas do projeto

| Data | Etapa |
|---|---|
| 24/09/2026 | Compra das peças |
| 25/09/2026 | Chegada das peças |
| 07/10/2026 | Apresentação de status ao professor: prévia da interface web (só um "Hello World" de teste, feita por Letícia) e funcionamento básico do ESP32 com o botão (primeira montagem de teste, feita por Eduardo); nessa etapa o protótipo também foi iniciado no Wokwi e a equipe apresentou a ideia central do projeto; o professor deu opiniões e ideias depois, incorporadas nas etapas seguintes |
| 09/10/2026 | Montagem física do circuito |
| 15/10/2026 | Testes de cada peça |
| 16/10/2026 | Testes de rede, protótipo no Wokwi aprimorado e atualizado, e ideia do áudio como prova |
| 04/11/2026 | Versão final: etapa principal em que a ideia inicial foi atualizada, corrigida e aprimorada (regras de funcionamento, Telegram e nova central) |
| 09/11/2026 | Testes finais e vídeos de demonstração |
| 10/11/2026 | Case física, slides, vídeo geral e relatório final |
| 15/11/2026 | Modelagem 3D da case (João Pedro), com vídeo e modelo interativo em [docs/modelagem-3d](docs/modelagem-3d), para apresentar na apresentação final |
| 16/11/2026 | Ensaio e ajustes finais: ensaio da apresentação, fala de cada integrante, testes finais e gravação do vídeo de contextualização (será editado e colocado nos slides). Todos participaram, exceto Guilherme |
| 17/11/2026 a 18/11/2026 | Roteiros de fala de cada integrante e ajuste do slide de Funcionamento, que passou a mostrar a senha de coação, as 3 senhas erradas e os motivos da prioridade máxima |
| 21/11/2026 | PDF final da apresentação e consolidação de toda a documentação do projeto neste repositório, em [docs/apresentacao](docs/apresentacao) |

Linha do tempo interativa: [docs/linha-do-tempo/index.html](docs/linha-do-tempo/index.html) (baixe o arquivo e abra no navegador).

<p align="center"><img src="docs/linha-do-tempo/linha_do_tempo_computador.png" width="760" alt="Linha do tempo do projeto"></p>

## Apresentação final

Os slides da apresentação final (22 slides, feitos no Canva), as miniaturas e os roteiros de fala estão em [docs/apresentacao](docs/apresentacao).

<p align="center"><img src="docs/apresentacao/visao_geral_slides.jpg" width="760" alt="Visão geral dos slides da apresentação final"></p>

- [Slides em PDF](docs/apresentacao/HELP_ME_Apresentacao_Final.pdf)
- [Fala do João Pedro: regras de funcionamento](docs/apresentacao/roteiro_fala_joao_pedro.md)
- [Fala do Eduardo: código e fase de testes, com a demonstração em dupla](docs/apresentacao/roteiro_fala_eduardo.md)

## Sumário

- [Apresentação final](#apresentação-final)
- [Equipe](#equipe)
- [Arquitetura](#arquitetura)
- [Hardware e pinagem](#hardware-e-pinagem)
- [Regras de funcionamento](#regras-de-funcionamento)
- [API do servidor](#api-do-servidor)
- [Clientes](#clientes)
- [Como executar](#como-executar)
- [Testes e evidências](#testes-e-evidências)
- [Estrutura do repositório](#estrutura-do-repositório)
- [Limitações e próximos passos](#limitações-e-próximos-passos)

## Equipe

| RA | Nome | Papel principal |
|---|---|---|
| 40132879 | Eduardo Hashimoto Yonegura | Hardware, protótipo, pesquisa de áudio, testes finais e vídeos |
| 40126989 | Gabriel André Moro | Pagamento das peças do projeto físico e do botão da case |
| 40126364 | Guilherme Sakagami Spitzner | Ajuda na montagem física do circuito |
| 40126944 | João Pedro Pires Antunes | Montagem do circuito, firmware, Telegram, software, modelagem 3D da case, testes finais, edição do vídeo, GitHub e relatório final |
| 40123207 | Letícia Beatriz Silva de Morais | Soldagem, interface web, documentação, case física, revisão do relatório e slides |

## Arquitetura

<p align="center"><img src="docs/img/arquitetura.png" width="720" alt="Arquitetura do HELP ME"></p>

O sistema tem três partes:

- **Dispositivo:** ESP32 com botão e microfone (sensores) e motor de vibração (atuador). Roda o servidor HTTP, grava log, histórico e áudio na flash (LittleFS) e fala com o Telegram por HTTPS.
- **Central de segurança:** página web servida pelo próprio ESP32 na porta 80. Abre em qualquer navegador da mesma rede.
- **Canal da usuária:** bot do Telegram. O nome do bot é "Lembretes da Farmácia", um disfarce para que ninguém perto dela desconfie.

| Comunicação | Protocolo | Porta |
|---|---|---|
| Central ↔ ESP32 | HTTP sobre TCP | 80 |
| ESP32 → API do Telegram | HTTPS (TLS) sobre TCP | 443 |
| ESP32 → servidor NTP | NTP sobre UDP | 123 |

No ESP32, o trabalho é dividido em três tarefas do FreeRTOS: o laço principal (servidor HTTP, botão, motor e regras), a tarefa de áudio (núcleo 1) e a tarefa do Telegram (núcleo 0).

## Hardware e pinagem

| Componente | Pino do ESP32 | Função |
|---|---|---|
| ESP32 DevKit V1 | — | Processa, conecta ao Wi-Fi e roda o servidor |
| Botão (INPUT_PULLUP) | D22 | Sensor: segurar 1 s abre o alerta |
| Microfone digital INMP441 | D18 (SCK), D19 (WS), D21 (SD) | Sensor: mede o volume e grava o áudio |
| Motor de vibração 3 V + 2N2222A + resistor + diodo | D4 | Atuador: vibra para confirmar o acionamento |
| Alimentação | 3V3 e GND | Microfone e motor |

<p align="center">
  <img src="docs/img/ligacoes_protoboard.png" width="520" alt="Ligações na protoboard">
  <img src="docs/img/prototipo_protoboard.jpg" width="230" alt="Protoboard">
</p>

A pinagem completa do ESP32 está no Apêndice A do [relatório](docs/Relatorio_HELP_ME_ABNT.pdf).

## Case e modelagem 3D

A Letícia construiu a case física: uma caixa preta de bancada, com o botão de acionamento na face superior. Em 15/11/2026, o João Pedro fez a modelagem 3D dessa case, que mostra a caixa fechada, aberta, desmontada e montada, com a protoboard, o ESP32 e a fiação por dentro.

<p align="center">
  <img src="docs/modelagem-3d/case_fisica_vista_superior.jpg" width="230" alt="Case física, vista superior">
  <img src="docs/modelagem-3d/modelo_caixa_fechada.png" width="360" alt="Modelo 3D, caixa fechada">
</p>
<p align="center">
  <img src="docs/modelagem-3d/modelo_desmontado_por_dentro.png" width="360" alt="Modelo 3D, vista por dentro">
  <img src="docs/modelagem-3d/modelo_montado_vista_lateral.png" width="360" alt="Modelo 3D, vista lateral">
</p>

| Item | Detalhe |
|---|---|
| Dimensões do modelo | 115 × 150 × 80 mm (largura × profundidade × altura) |
| Material | MDF preto, com tampa removível presa por parafusos |
| Peças modeladas | Botão vermelho no topo, microfone, parafusos, protoboard com ESP32 e fios |
| Controles | Caixa fechada, Caixa aberta, Desmontar, Montar |
| Tecnologia | Página HTML com a biblioteca [three.js](https://threejs.org/) |
| Vídeo | [video_modelagem_3d_case.mp4](docs/modelagem-3d/video_modelagem_3d_case.mp4) (56 s) |
| Modelo interativo | [caixa_botao.html](docs/modelagem-3d/caixa_botao.html), junto com `three-d-stage.js` (abrir no navegador, com internet) |

Para ver o modelo, baixe a pasta [docs/modelagem-3d](docs/modelagem-3d) e abra o `caixa_botao.html` no navegador.

## Regras de funcionamento

<p align="center"><img src="docs/img/estados.png" width="620" alt="Estados do sistema"></p>

| Regra | Comportamento |
|---|---|
| Disparo | Segurar o botão 1 s abre o alerta e o motor vibra uma vez. Toque curto é ignorado. |
| Pré-alerta | A usuária tem 20 s para cancelar pelo Telegram. Ela também pode tocar em "Enviar agora" e mandar antes do fim. |
| Senha normal | Cancela no pré-alerta. Depois do envio, vira pedido de encerramento para a central decidir. |
| Senha errada | Avisa e a contagem continua. Três erradas seguidas contam como coação. |
| Senha de coação | O celular mostra "Alerta cancelado", mas a central recebe em silêncio com prioridade máxima. Nada mais vai para o celular. |
| Som alto | Volume acima do limite por 1,5 s durante um alerta sobe a prioridade para máxima. Não dispara alerta nem liga o motor. |
| Áudio | Blocos de 60 s em WAV na flash. A central pode pedir mais 60 s. No falso alarme e no cancelamento, o áudio é apagado. |
| Localização | Só é aceita durante um alerta e é apagada quando ele termina. |
| Encerramento | Só a central encerra: atendimento concluído ou falso alarme. |
| Wi-Fi e log | Se a rede cair, o ESP32 tenta reconectar a cada 10 s. Todo evento vai para a flash com data, hora (NTP) e nível INFO, AVISO ou ERRO. |

## API do servidor

Todas as rotas usam `GET` na porta 80. Rota inexistente responde `404`.

| Rota | Resposta | O que faz |
|---|---|---|
| `/` | HTML | Página da central de segurança |
| `/status` | JSON | Estado, IP, MAC, rede, RSSI, tempo ligado, hora, alerta, Telegram e memória |
| `/sensor` | JSON | Volume do microfone, limite, som alto e estado do botão |
| `/atuador?estado=on\|off\|pulso` | JSON | Liga, desliga ou dá um pulso de 0,5 s no motor (desliga sozinho após 10 s) |
| `/limite?valor=N` | JSON | Ajusta o limite de som alto |
| `/simular` | JSON | Dispara um alerta de teste, como se o botão fosse apertado |
| `/resolver` | JSON | Encerra como atendimento concluído |
| `/falso` | JSON | Encerra como falso alarme |
| `/chat` | JSON | Conversa da ocorrência atual |
| `/chat/enviar?texto=...` | JSON | Envia mensagem da central para a usuária |
| `/audio?oc=CÓDIGO` | WAV | Áudio gravado da ocorrência |
| `/gravar_mais` | JSON | Grava mais 60 s de áudio |
| `/localizacao?desde=N` | JSON | Pontos novos da localização e status |
| `/pedir_localizacao` | JSON | Pede a localização em tempo real pelo Telegram (bloqueado em coação) |
| `/historico` | JSON | Últimas ocorrências |
| `/estatisticas` | JSON | Números calculados do histórico |
| `/log?ultimas=N` | Texto | Últimas linhas do log da flash |
| `/log/limpar` | JSON | Limpa o log |

Exemplo:

```bash
curl http://172.20.10.12/status
curl "http://172.20.10.12/atuador?estado=pulso"
```

## Clientes

- **Navegador (central de segurança):** abre `http://IP_DO_ESP32/` e mostra visão geral, ocorrência em andamento, áudio, chat, mapa, acionamento da PM (190) e do SAMU (192) simulados, dispositivo em tempo real, histórico, estatísticas e log.
- **Telegram (usuária):** comandos `/start`, `/status`, `/teste` e `/ajuda`, e os botões "Enviar localização atual", "Estou bem" e "Preciso de ajuda". Durante o alerta, o que ela escreve aparece no chat da central.
- **Código da central ([src/client/index.html](src/client/index.html)):** cópia da página HTML/CSS/JS que o ESP32 entrega na rota `/`. Aberta direto do arquivo, ela usa o ESP32 do IP definido na constante `ESP` (o servidor libera CORS).
- **Scripts de teste ([src/client](src/client)):** testam ping, porta 80, rotas, 404, atuador e 100 requisições seguidas, no Windows (PowerShell) e no macOS/Linux (bash).

<p align="center">
  <img src="docs/evidencias/telegram/01_status_teste.jpg" width="220" alt="Telegram /status e /teste">
  <img src="docs/evidencias/telegram/02_cancelamento_senha.jpg" width="220" alt="Cancelamento com senha">
  <img src="docs/evidencias/telegram/03_chat.jpg" width="220" alt="Chat com a central">
</p>

## Como executar

### Requisitos

- ESP32 DevKit V1 montado como na seção [Hardware e pinagem](#hardware-e-pinagem)
- [Arduino IDE 2](https://www.arduino.cc/en/software) com o pacote de placas **esp32** da Espressif
- Biblioteca **ArduinoJson** 7.x (Benoit Blanchon), instalada pelo Library Manager
- Rede Wi-Fi de **2,4 GHz** com acesso à internet (o Telegram e o NTP precisam dela)
- Um bot do Telegram criado no [@BotFather](https://t.me/BotFather) e o `chat_id` da usuária

### Configuração

No início de [`src/esp32/helpme_final_v3/helpme_final_v3.ino`](src/esp32/helpme_final_v3/helpme_final_v3.ino):

```cpp
const char* WIFI_SSID        = "NOME_DA_REDE";
const char* WIFI_SENHA       = "SENHA_DA_REDE";
const char* TELEGRAM_TOKEN   = "SEU_TOKEN_AQUI";
const char* CHAT_ID_USUARIA  = "7107756741";
const char* SENHA_CANCELAR   = "4452";
const char* SENHA_COACAO     = "2544";
```

Troque `NOME_DA_REDE`, `SENHA_DA_REDE` e `SEU_TOKEN_AQUI` pelos dados reais antes de gravar. O token vem do @BotFather, e o `CHAT_ID_USUARIA` é o chat da usuária que o bot aceita. As senhas 4452 (cancelar) e 2544 (coação) são as da demonstração.

### Gravação

1. **Tools › Board:** `ESP32 Dev Module`
2. **Tools › Partition Scheme:** `No OTA (2MB APP/2MB SPIFFS)` (reserva 2 MB para o LittleFS)
3. Grave o sketch e abra o **Serial Monitor** em **115200**
4. O Monitor Serial mostra o IP. Abra `http://IP_DO_ESP32/` em um navegador da mesma rede
5. No Telegram, mande `/start` para o bot

## Testes e evidências

Os testes foram feitos em duas rodadas: a primeira em 16/10/2026 (rede e servidor) e a segunda em 09/11/2026 (versão final, com o ESP32 em `172.20.10.12` e o computador de teste em `172.20.10.14`).

| Teste | Resultado | Evidência |
|---|---|---|
| Inicialização | Wi-Fi, servidor na porta 80, microfone e hora sem reiniciar | [01_boot_serial](docs/evidencias/versao_final/01_boot_serial.png) |
| Ping | 4 de 4 respostas, 0% de perda, média de 38,6 ms | [02_ping](docs/evidencias/versao_final/02_ping.png) |
| Porta 80 | Conexão TCP aceita | [03_porta80](docs/evidencias/versao_final/03_porta80.png) |
| Cliente-servidor | JSON em `/status` e nas rotas de dados, 404 em rota inexistente | [04](docs/evidencias/versao_final/04_curl_status.png), [05](docs/evidencias/versao_final/05_curl_rotas_404.png) |
| Atuador pela rede | Motor liga e desliga; desligamento automático após 10 s no log | [06](docs/evidencias/versao_final/06_motor_pela_rede.png), [07](docs/evidencias/versao_final/07_log_central.png) |
| Pré-alerta e cancelamento | Contagem de 20 s; senha errada avisada; senha certa cancela | [08](docs/evidencias/versao_final/08_pre_alerta.png), [Telegram](docs/evidencias/telegram) |
| Atendimento | Chat nos dois sentidos, áudio e localização na central | [09](docs/evidencias/versao_final/09_chat_central.png), [10](docs/evidencias/versao_final/10_mapa.jpg) |
| Coação | Celular mostra "cancelado" | [05_coacao](docs/evidencias/telegram/05_coacao.jpg) |
| Histórico | 8 ocorrências, tempo médio de resposta de 1 min 00 s | [11](docs/evidencias/versao_final/11_historico.png), [12](docs/evidencias/versao_final/12_estatisticas.png) |
| Queda do Wi-Fi | Reconexão automática registrada no log | [07_queda_reconexao_wifi](docs/evidencias/primeira_rodada/07_queda_reconexao_wifi.jpg) |
| Reinício | Log mantido na flash | [08_log_apos_reiniciar](docs/evidencias/primeira_rodada/08_log_apos_reiniciar.jpg) |
| Estresse | 100 requisições seguidas sem erro | [09_estresse_100_requisicoes](docs/evidencias/primeira_rodada/09_estresse_100_requisicoes.jpg) |

O roteiro completo, com os 34 passos e o resultado esperado de cada um, está em [docs/HELP_ME_Funcoes_e_Fluxo_de_Testes.pdf](docs/HELP_ME_Funcoes_e_Fluxo_de_Testes.pdf).

## Estrutura do repositório

```
HELP-ME/
├── README.md
├── docs/
│   ├── Relatorio_HELP_ME_ABNT.pdf
│   ├── Relatorio_HELP_ME_ABNT.docx
│   ├── HELP_ME_Funcoes_e_Fluxo_de_Testes.pdf
│   ├── apresentacao/            slides finais em PDF, miniaturas e roteiros de fala
│   ├── img/                     arquitetura, ligações, estados, pinagem, protótipo, central e quadros do vídeo
│   ├── modelagem-3d/            modelo 3D da case (HTML), vídeo, prints e foto da case física
│   ├── video-contexto/          vídeo de contextualização da apresentação final
│   ├── linha-do-tempo/          página da linha do tempo e prints (computador e celular)
│   └── evidencias/
│       ├── primeira_rodada/     testes de 16/10/2026
│       ├── versao_final/        testes de 09/11/2026
│       └── telegram/            prints do canal da usuária
└── src/
    ├── esp32/helpme_final_v3/   firmware (Arduino IDE)
    └── client/                  página da central (index.html) e scripts de teste (PowerShell e bash)
```

## Limitações e próximos passos

- O protótipo funciona ligado ao USB, sem bateria.
- A central não tem login: qualquer aparelho da rede local pode abri-la.
- A conexão com o Telegram não verifica o certificado do servidor, o que só serve para protótipo.
- A flash comporta poucos minutos de áudio.
- O alerta só chega ao Telegram se o Wi-Fi tiver acesso à internet.
- Os dados cadastrais das usuárias na central são fictícios.

Próximos passos: bateria, login na central, verificação do certificado TLS e detecção automática de gritos pelo microfone.

## Documentação

- [Relatório técnico (ABNT), PDF](docs/Relatorio_HELP_ME_ABNT.pdf)
- [Relatório técnico (ABNT), DOCX](docs/Relatorio_HELP_ME_ABNT.docx)
- [Funções e fluxo de testes](docs/HELP_ME_Funcoes_e_Fluxo_de_Testes.pdf)
- [Slides da apresentação final (PDF)](docs/apresentacao/HELP_ME_Apresentacao_Final.pdf)
- [Linha do tempo](docs/linha-do-tempo/index.html) e [modelagem 3D da case](docs/modelagem-3d)
- [Vídeo de contextualização (58 s)](https://drive.google.com/file/d/1uBNcnztY9wLcF3GwKnJ3Ux2KRlkfrE1R/view?usp=drivesdk)
- [Vídeo de demonstração](https://drive.google.com/file/d/1CQPIhX4oUAhRQfv2Jqt-ol8FbV0izJc4/view?usp=drivesdk)
