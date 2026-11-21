# Fala do Eduardo: código e fase de testes (slides 14 e 15 no PDF final)

Tempo estimado: cerca de 2 min 30 s. A parte final (demonstração do Telegram e do log persistente) é feita em dupla com o João Pedro.

## Slide de código (13 na numeração antiga, 14 no PDF final)

"Este é o código do HELP ME, feito na Arduino IDE e rodando dentro do ESP32. Ele lê o botão e o microfone, aplica as regras que o João Pedro explicou e conversa com a central e com o Telegram. Para isso, dividimos o trabalho em três tarefas em paralelo: o laço principal, a tarefa de áudio e a tarefa do Telegram. Assim, uma internet lenta não trava o botão nem a gravação.

Aqui na tela está um exemplo: a rota do atuador. O ESP32 é um servidor na porta 80, e essa função recebe o parâmetro 'estado'. Se for 'on', liga o motor. Se for 'off', desliga. Se for 'pulso', vibra por meio segundo. Qualquer outro valor devolve erro 400. E em cada ação ela chama a função 'registrar', que grava o evento no log, com data e hora, na memória flash. O código inteiro segue esse padrão: cada rota é uma função pequena que executa, registra e responde em JSON."

### O código do slide, linha por linha

| Linha | O que faz |
|---|---|
| `void rotaAtuador()` | Função executada quando alguém abre `/atuador` |
| `server.arg("estado")` | Lê o parâmetro `estado` do endereço |
| `if (e == "on")` | Liga o motor, guarda o instante (`millis()`) para o desligamento automático em 10 s e registra no log |
| `else if (e == "off")` | Desliga o motor e registra no log |
| `else if (e == "pulso")` | `vibrar({500})`: vibra por 0,5 s e para sozinho |
| `else` | Valor inválido: responde erro 400 e termina |
| `enviarJson(...)` | Responde em JSON se o motor está ligado |

## Slide de fase de testes (14 na numeração antiga, 15 no PDF final)

"Os testes foram feitos em etapas, juntando uma peça de cada vez. Estas fotos mostram a montagem na protoboard, com o ESP32, o botão e os componentes ligados. Em 15 de outubro, testamos cada peça separada. Em 16 de outubro, testamos a rede: porta 80, ping, rotas e queda do Wi-Fi. Em 9 de novembro, fizemos os testes finais, com a versão completa.

Os resultados: ping sem nenhuma perda, com média de 38,6 milissegundos, 100 requisições seguidas sem erro, reconexão automática do Wi-Fi e log mantido depois de reiniciar a placa. Agora, eu e o João Pedro vamos mostrar na prática o envio da mensagem pelo Telegram e o log persistente."

## Demonstração em dupla (Eduardo e João Pedro)

**Parte A: mensagem pelo Telegram**
1. Eduardo mostra a placa ligada e o IP no Monitor Serial.
2. João Pedro manda `/status` e `/teste` ao bot e mostra as respostas.
3. Eduardo segura o botão por 1 s e a placa vibra.
4. João Pedro mostra o aviso "Seu alerta foi acionado", com os botões Cancelar alerta e Enviar agora.
5. João Pedro cancela com a senha de cancelamento e o bot responde "Alerta cancelado".
6. Eduardo mostra na central o pré-alerta cancelado.

**Parte B: log persistente**
7. Eduardo abre `http://IP_DO_ESP32/log?ultimas=20` e mostra os eventos, com data, hora e nível.
8. Eduardo reinicia a placa (botão EN ou tirando o cabo USB).
9. Eduardo recarrega o `/log?ultimas=20`: as mesmas linhas continuam lá, seguidas do registro da nova inicialização. O `/historico` também pode ser mostrado.
10. João Pedro fecha: o registro do que aconteceu não se perde, mesmo com queda de energia ou reinício.

Plano B: se o Telegram não responder, usar `/simular` na central para disparar o alerta e mostrar o log.

## Perguntas que o professor pode fazer

- **Por que três tarefas?** Para que a internet lenta não trave o botão nem a gravação de áudio.
- **Onde ficam os dados?** Na memória flash, em arquivos do LittleFS: log, histórico, contador de ocorrências e áudios.
- **O que acontece se o Wi-Fi cair?** O ESP32 tenta reconectar a cada 10 s e registra no log. O alerta só chega ao Telegram quando há internet.
- **Por que o erro 400?** É o código HTTP de pedido inválido, para avisar que o parâmetro está errado.
- **O motor liga direto no ESP32?** Não, passa por um transistor 2N2222A, porque o pino do ESP32 não aguenta a corrente do motor.
- **Que protocolos o projeto usa?** HTTP sobre TCP na porta 80 (central), HTTPS sobre TCP na 443 (Telegram) e NTP sobre UDP na 123 (hora).
