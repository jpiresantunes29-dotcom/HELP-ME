param(
    [Parameter(Mandatory = $true)][string]$IP,
    [int]$Requisicoes = 100
)

$base = "http://$IP"

Write-Host "`n== 1. Ping ==" -ForegroundColor Cyan
ping -n 4 $IP

Write-Host "`n== 2. Porta 80 ==" -ForegroundColor Cyan
Test-NetConnection $IP -Port 80 | Select-Object ComputerName, RemotePort, TcpTestSucceeded

Write-Host "`n== 3. Rotas de dados ==" -ForegroundColor Cyan
foreach ($rota in "/status", "/sensor", "/historico", "/estatisticas", "/log?ultimas=10") {
    Write-Host "`n-- GET $rota" -ForegroundColor Yellow
    curl.exe -s "$base$rota"
}

Write-Host "`n`n== 4. Rota inexistente (espera 404) ==" -ForegroundColor Cyan
curl.exe -s -o NUL -w "HTTP %{http_code}`n" "$base/rota_que_nao_existe"

Write-Host "`n== 5. Atuador pela rede ==" -ForegroundColor Cyan
curl.exe -s "$base/atuador?estado=on"; Write-Host ""
Start-Sleep -Seconds 2
curl.exe -s "$base/atuador?estado=off"; Write-Host ""
curl.exe -s "$base/atuador?estado=pulso"; Write-Host ""

Write-Host "`n== 6. Estresse: $Requisicoes requisicoes seguidas em /status ==" -ForegroundColor Cyan
$ok = 0; $falhas = 0
$tempo = Measure-Command {
    for ($i = 1; $i -le $Requisicoes; $i++) {
        try {
            $r = Invoke-WebRequest -Uri "$base/status" -UseBasicParsing -TimeoutSec 5
            if ($r.StatusCode -eq 200) { $ok++ } else { $falhas++ }
        } catch { $falhas++ }
    }
}
Write-Host ("Sucesso: {0}  Falhas: {1}  Tempo total: {2:N1} s" -f $ok, $falhas, $tempo.TotalSeconds)
