# Fala do João Pedro: regras de funcionamento (slide 17 no PDF final)

Tempo estimado: cerca de 3 min. A fala também está nas notas do apresentador do slide de funcionamento, no Canva.

Fluxo do slide: 1 Disparo, 2 Pré-alerta, 3 Senha de cancelamento, 4 Senha de coação ou 3 erros, 5 Enviar agora ou 20 s, 6 Áudio em blocos de 60 s, 7 Prioridade máxima, 8 Atendimento, 9 Encerramento.

## Tópicos

1. Abertura
2. Disparo (segurar 1 s)
3. Pré-alerta de 20 s
4. Senha de cancelamento
5. Senha errada (3 erros viram coação)
6. Senha de coação
7. Enviar agora / 20 s sem ação
8. Ocorrência na central
9. Áudio em blocos de 60 s
10. Prioridade máxima (coação, "Preciso de ajuda", som alto)
11. Durante o atendimento (chat, localização, PM e SAMU)
12. Encerramento (concluído ou falso alarme)
13. Histórico e estatísticas
14. Regras de apoio (Wi-Fi, log, bot disfarçado)
15. Fechamento

## Fala

**1. Abertura**
"Agora eu vou explicar as regras de funcionamento do HELP ME: o que acontece, passo a passo, do momento em que a mulher aperta o botão até o fim do atendimento. Cada regra existe por um motivo de segurança."

**2. Disparo**
"Tudo começa no botão. Ela precisa segurar por 1 segundo. Um toque rápido é ignorado, para evitar alarme falso. Passado 1 segundo, o motor vibra uma vez, o microfone começa a gravar o áudio e ela recebe uma mensagem no Telegram avisando que o alerta foi acionado."

**3. Pré-alerta de 20 segundos**
"Começa então o pré-alerta, de 20 segundos. Nesse tempo a central ainda não foi avisada. É a janela para corrigir um engano. Existem quatro caminhos:

- Senha de cancelamento, de 4 dígitos: o alerta é cancelado e o áudio é apagado.
- Senha errada: o sistema avisa e a contagem continua. Três erradas seguidas viram coação.
- Senha de coação: no celular aparece 'Alerta cancelado', mas a central recebe a ocorrência em silêncio, com prioridade máxima. O sistema para de mandar mensagens para o celular dela.
- Enviar agora, ou 20 segundos sem ação: o alerta vai para a central."

**4. Ocorrência na central**
"Sem cancelamento, vira uma ocorrência em andamento. A central vê os dados da usuária, o áudio, o chat e a localização no mapa. O áudio é gravado em blocos de 60 segundos, e a central pode pedir mais 60. A ocorrência começa com prioridade alta."

**5. Prioridade máxima**
"A prioridade sobe para máxima em três casos: senha de coação ou três senhas erradas, o botão 'Preciso de ajuda' no Telegram, e som alto por 1,5 segundo durante o alerta. O som alto só aumenta a prioridade. Ele nunca dispara o alerta nem liga o motor."

**6. Atendimento**
"Durante o atendimento, a central conversa pelo chat, pede mais áudio, pede a localização em tempo real e aciona a PM, no 190, e o SAMU, no 192, de forma simulada. A localização só vale durante o alerta e é apagada no fim. Se a usuária digitar a senha normal depois do envio, isso vira um pedido de encerramento, e quem decide é a central."

**7. Encerramento**
"Quem encerra é sempre a central: atendimento concluído ou falso alarme. No falso alarme, o áudio é apagado. A ocorrência vai para o histórico, com data, hora, prioridade, tempo de resposta e desfecho."

**8. Fechamento**
"Se o Wi-Fi cair, o dispositivo reconecta sozinho a cada 10 segundos, e todo evento fica no log. Resumindo: 1 segundo para disparar, 20 para cancelar, e a central decide o final. Agora vamos mostrar isso funcionando no Telegram."

## Números para não errar

| Regra | Valor |
|---|---|
| Segurar o botão | 1 s |
| Janela de cancelamento | 20 s |
| Senha | 4 dígitos |
| Senhas erradas seguidas até virar coação | 3 |
| Som alto sustentado (só sobe a prioridade) | 1,5 s |
| Bloco de áudio | 60 s |
| Reconexão do Wi-Fi | a cada 10 s |
