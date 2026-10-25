$ErrorActionPreference='Stop'
$root=Split-Path $PSScriptRoot
$xfel=Join-Path (Split-Path $root) '工具\xfel-windows-v1.4.0\xfel.exe'
$image=Join-Path $root 'output\zephyr.bin'
$devices=@(Get-PnpDevice -PresentOnly | Where-Object {$_.InstanceId -like 'USB\VID_1F3A&PID_EFE8\*'})
if($devices.Count -ne 1 -or $devices[0].Status -ne 'OK'){throw '请先拔下板子，按住 FEL 按钮重新插入 USB，松开按钮后再运行。'}
if(!(Test-Path -LiteralPath $image)){throw '请先编译验证工程。'}
$run=Join-Path $root ('output\runs\'+(Get-Date -Format 'yyyyMMdd-HHmmss'))
New-Item -ItemType Directory -Path $run | Out-Null
Copy-Item -LiteralPath $image -Destination (Join-Path $run 'program.bin')
$state=[ordered]@{stage='prepared';flash_written=$false;image_sha256=(Get-FileHash -LiteralPath $image -Algorithm SHA256).Hash;image_bytes=(Get-Item -LiteralPath $image).Length;device=$devices[0].InstanceId;psram_probe_verified=$false;ram_readback_verified=$false}
function Save-State {$state | ConvertTo-Json -Depth 5 | Set-Content -LiteralPath (Join-Path $run 'state.json') -Encoding UTF8}
function Run-Xfel([string]$Log,[string[]]$Arguments) {
    $lines=& $xfel @Arguments 2>&1
    $code=$LASTEXITCODE
    $lines | Out-File -LiteralPath (Join-Path $run $Log) -Encoding utf8
    if($code -ne 0){throw "xfel 失败，日志：$Log"}
    return ($lines -join "`n")
}
Save-State
Push-Location $run
try {
    $version=Run-Xfel 'version.log' @('version')
    if($version -notmatch '\(F101\)'){throw '芯片识别不符合 F101，停止。'}
    $state.stage='initializing_psram';Save-State
    Run-Xfel 'ddr.log' @('ddr','f101-s3') | Out-Null
    Start-Sleep -Milliseconds 300
    Run-Xfel 'after-ddr.log' @('version') | Out-Null
    [byte[]]$pattern=0..63
    [IO.File]::WriteAllBytes((Join-Path $run 'probe.bin'),$pattern)
    Run-Xfel 'probe-write.log' @('write','0x40000000','probe.bin') | Out-Null
    Run-Xfel 'probe-read.log' @('read','0x40000000','0x40','probe-read.bin') | Out-Null
    if((Get-FileHash probe.bin).Hash -ne (Get-FileHash probe-read.bin).Hash){throw 'PSRAM 读写验证失败，停止。'}
    $state.psram_probe_verified=$true;$state.stage='loading_ram';Save-State
    Write-Host '正在把验证程序载入内存，随后逐字节读回核对……'
    Run-Xfel 'ram-write.log' @('write','0x40000000','program.bin') | Out-Null
    $length='0x{0:X}' -f $state.image_bytes
    Run-Xfel 'ram-read.log' @('read','0x40000000',$length,'ram-readback.bin') | Out-Null
    if((Get-FileHash ram-readback.bin).Hash -ne $state.image_sha256){throw '程序内存读回校验失败，不执行。'}
    $state.ram_readback_verified=$true;$state.stage='executing';Save-State
    Run-Xfel 'exec.log' @('exec','0x40000000') | Out-Null
    $state.stage='execution_requested';Save-State
    Write-Host '已请求运行。等待 USB 串口出现后，双击“打开实时电钢琴.cmd”进行演奏。'
    Write-Host "本次记录：$run"
} catch {
    $state.stage='failed';$state.error=$_.Exception.Message;Save-State
    throw
} finally {Pop-Location}
