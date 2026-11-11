#!/usr/bin/env bash
set -u

if [ $# -lt 1 ]; then
  echo "Uso: ./testar_central.sh <IP_DO_ESP32> [requisicoes]"
  exit 1
fi

IP="$1"
N="${2:-100}"
BASE="http://$IP"

echo; echo "== 1. Ping =="
ping -c 4 "$IP"

echo; echo "== 2. Porta 80 =="
nc -vz -w 3 "$IP" 80

echo; echo "== 3. Rotas de dados =="
for rota in /status /sensor /historico /estatisticas "/log?ultimas=10"; do
  echo; echo "-- GET $rota"
  curl -s "$BASE$rota"; echo
done

echo; echo "== 4. Rota inexistente (espera 404) =="
curl -s -o /dev/null -w "HTTP %{http_code}\n" "$BASE/rota_que_nao_existe"

echo; echo "== 5. Atuador pela rede =="
curl -s "$BASE/atuador?estado=on"; echo
sleep 2
curl -s "$BASE/atuador?estado=off"; echo
curl -s "$BASE/atuador?estado=pulso"; echo

echo; echo "== 6. Estresse: $N requisicoes seguidas em /status =="
ok=0; falhas=0; inicio=$(date +%s)
for i in $(seq 1 "$N"); do
  code=$(curl -s -o /dev/null -m 5 -w "%{http_code}" "$BASE/status")
  if [ "$code" = "200" ]; then ok=$((ok+1)); else falhas=$((falhas+1)); fi
done
echo "Sucesso: $ok  Falhas: $falhas  Tempo total: $(( $(date +%s) - inicio )) s"
