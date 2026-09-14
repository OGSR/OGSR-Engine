param(
    [string]$Fxc = 'C:/Program Files (x86)/Windows Kits/10/bin/10.0.26100.0/x64/fxc.exe'
)

# Optional, user-run compilation check. This does not build the engine or run GPU tests.
$ErrorActionPreference = 'Stop'
$repoPath = Split-Path -Parent $PSScriptRoot
$shaderPath = Join-Path $repoPath 'Game/Resources_SoC_1.0006/gamedata/shaders/r3'

# FXC accepts slash-separated paths that OGSR's VFS cannot resolve. Check the
# runtime convention too, including nested vendor headers, before compiling.
$includeFiles = @(Get-ChildItem -LiteralPath $shaderPath -File -Filter 'ogsr_xegtao*') +
    @(Get-ChildItem -LiteralPath (Join-Path $shaderPath 'xegtao') -File | Where-Object Extension -In @('.h', '.hlsli'))
foreach ($includeFile in $includeFiles) {
    $includeText = Get-Content -LiteralPath $includeFile.FullName -Raw
    foreach ($includeMatch in [regex]::Matches($includeText, '#include\s+"([^"]+)"')) {
        if ($includeMatch.Groups[1].Value.Contains('/')) {
            throw "OGSR requires backslashes in shader includes: $($includeFile.Name): $($includeMatch.Groups[1].Value)"
        }
    }
}

$outputPath = Join-Path $repoPath 'ogsr_engine/_TEMP/ao_validation'
New-Item -ItemType Directory -Force -Path $outputPath | Out-Null

$compiledCount = 0
foreach ($quality in 1..3) {
    foreach ($shader in @('ogsr_xegtao_prefilter.cs', 'ogsr_xegtao_main.cs', 'ogsr_xegtao_denoise.cs', 'ogsr_xegtao_export.ps', 'combine_1_ao.ps')) {
        $target = if ($shader.EndsWith('.cs')) { 'cs_5_0' } else { 'ps_5_0' }
        $shaderArgs = @('/nologo', '/T', $target, '/E', 'main', '/O1', '/Zpr', '/I', $shaderPath,
            '/D', "SSAO_QUALITY=$quality", '/Fo', (Join-Path $outputPath "$shader-xegtao-$quality.cso"),
            (Join-Path $shaderPath $shader))
        $compilerOutput = & $Fxc @shaderArgs 2>&1
        if ($LASTEXITCODE -ne 0) {
            throw "Failed: $shader, quality $quality`n$compilerOutput"
        }
        $compiledCount++
    }
}
Write-Output "PASS: $compiledCount XeGTAO shader variants (three quality levels, compute passes, export and composition)."
