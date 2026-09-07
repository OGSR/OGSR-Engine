param(
    [string]$Fxc = 'C:/Program Files (x86)/Windows Kits/10/bin/10.0.26100.0/x64/fxc.exe'
)

$ErrorActionPreference = 'Stop'
$repoPath = Split-Path -Parent $PSScriptRoot
$shaderPath = Join-Path $repoPath 'Game/Resources_SoC_1.0006/gamedata/shaders/r3'
$outputPath = Join-Path $repoPath 'ogsr_engine/_TEMP/ao_validation'
New-Item -ItemType Directory -Force -Path $outputPath | Out-Null

$compiledCount = 0
foreach ($mode in @('ssdo', 'gtao')) {
    foreach ($quality in 0..3) {
        foreach ($shader in @('ogsr_ao', 'ogsr_ao_half', 'combine_1', 'combine_1_ao')) {
            $shaderArgs = @('/nologo', '/T', 'ps_5_0', '/E', 'main', '/O1', '/Zpr')
            if ($quality -gt 0) { $shaderArgs += @('/D', "SSAO_QUALITY=$quality") }
            if ($mode -eq 'gtao') { $shaderArgs += @('/D', 'USE_GTAO=1') }
            $shaderArgs += @('/Fo', (Join-Path $outputPath "$shader-$mode-$quality.cso"), (Join-Path $shaderPath "$shader.ps"))
            $compilerOutput = & $Fxc @shaderArgs 2>&1
            if ($LASTEXITCODE -ne 0) {
                throw "Failed: $shader, $mode, quality $quality`n$compilerOutput"
            }
            $compiledCount++
        }
    }
}

$compilerOutput = & $Fxc /nologo /T ps_5_0 /E main /O1 /Zpr /Fo (Join-Path $outputPath 'ogsr_ao_resolve.cso') (Join-Path $shaderPath 'ogsr_ao_resolve.ps') 2>&1
if ($LASTEXITCODE -ne 0) { throw "Failed: AO resolve`n$compilerOutput" }
$compiledCount++
Write-Output "PASS: $compiledCount shader variants (SSDO/GTAO, off/low/medium/high, legacy/separate/half and resolve)."
