Write-Host "ST-Link Auto-Monitor started. Close window to stop."

# Track device state: "attached" = done, "shared" = needs attach retry
$tracked = @{}

function Clear-StlinkPersisted {
    $list = usbipd list 2>&1
    $inPersisted = $false
    foreach ($line in $list) {
        if ($line -match "^Persisted:") {
            $inPersisted = $true
            continue
        }
        if ($inPersisted -and $line -match "STM32 STLink") {
            $guid = ($line -split '\s+')[0]
            if ($guid -match '^[0-9a-fA-F\-]+$') {
                Write-Host "  Cleanup persisted GUID: $guid" -ForegroundColor Gray
                usbipd unbind --guid $guid 2>&1 | Out-Null
            }
        }
    }
}

# ---- Ensure usbipd service is running ----
$svc = Get-Service -Name "usbipd" -ErrorAction SilentlyContinue
if (-not $svc) {
    Write-Host "[WARN] usbipd service not found. Starting 'usbipd server' in background..." -ForegroundColor Yellow
    Start-Process usbipd -ArgumentList "server" -WindowStyle Hidden
    Start-Sleep -Seconds 2
} elseif ($svc.Status -ne "Running") {
    Write-Host "Starting usbipd service..."
    Start-Service -Name "usbipd" -ErrorAction SilentlyContinue
    Start-Sleep -Seconds 2
}

# ---- Startup cleanup ----
Write-Host "Startup cleanup..."
Clear-StlinkPersisted
Write-Host "Listening for ST-Link devices...`n"

# ---- Main loop ----
while ($true)
{
    $devices = usbipd list 2>&1

    # ---- Process each ST-Link device ----
    foreach ($line in $devices)
    {
        if ($line -match "0483:")
        {
            $parts = $line -split '\s+'
            $busid = $parts[0]

            # Determine current state from usbipd list
            $currentState = "not_shared"
            if ($line -match "Attached")  { $currentState = "attached" }
            elseif ($line -match "Shared") { $currentState = "shared" }

            # Skip if already fully attached and state hasn't degraded
            if ($tracked.ContainsKey($busid) -and $tracked[$busid] -eq "attached" -and $currentState -eq "attached")
            {
                continue
            }

            # Skip if we're waiting for a previous bind to take effect
            if ($tracked.ContainsKey($busid) -and $tracked[$busid] -eq "binding")
            {
                continue
            }

            # Device is new, or state changed, or needs retry
            $prevState = if ($tracked.ContainsKey($busid)) { $tracked[$busid] } else { "new" }
            Write-Host "[$(Get-Date -Format 'HH:mm:ss')] ST-Link $busid (state=$currentState, prev=$prevState)"

            # --- Handle "Not shared" → need to bind ---
            # Also enter here if previous attach failed OR device is brand new
            # (usbipd list may show "Shared" incorrectly after hot-plug cleanup)
            if ($currentState -eq "not_shared" -or $prevState -eq "new" -or ($tracked.ContainsKey($busid) -and $tracked[$busid] -eq "not_shared"))
            {
                Clear-StlinkPersisted
                Write-Host "  Binding..."
                $tracked[$busid] = "binding"
                usbipd bind --busid $busid
                if ($LASTEXITCODE -ne 0)
                {
                    Write-Host "  [ERROR] bind failed (exit=$LASTEXITCODE)" -ForegroundColor Red
                    $tracked.Remove($busid)
                    continue
                }
                Write-Host "  Bind OK"
                $tracked[$busid] = "shared"
                Start-Sleep -Milliseconds 500
                continue
            }

            # --- Handle "Shared" → need to attach ---
            if ($currentState -eq "shared")
            {
                Write-Host "  Attaching to WSL..."
                $attachResult = usbipd attach --wsl --busid $busid 2>&1
                if ($LASTEXITCODE -eq 0)
                {
                    Write-Host "  Attached to WSL" -ForegroundColor Green
                    $tracked[$busid] = "attached"
                }
                else
                {
                    Write-Host "  [ERROR] attach failed, will retry: $attachResult" -ForegroundColor Red
                    # usbipd list may show "Shared" but the actual state is stale.
                    # Force re-bind on next iteration.
                    $tracked[$busid] = "not_shared"
                }
                continue
            }

            # --- Handle "Attached" ---
            if ($currentState -eq "attached")
            {
                Write-Host "  Already attached to WSL" -ForegroundColor Green
                $tracked[$busid] = "attached"
                continue
            }
        }
    }

    # ---- Cleanup disconnected devices ----
    $currentBusIds = @()
    foreach ($line in $devices)
    {
        if ($line -match "0483:")
        {
            $parts = $line -split '\s+'
            $currentBusIds += $parts[0]
        }
    }

    foreach ($key in @($tracked.Keys))
    {
        if ($currentBusIds -notcontains $key)
        {
            Write-Host "[$(Get-Date -Format 'HH:mm:ss')] Device removed: $key" -ForegroundColor Yellow
            usbipd unbind --busid $key 2>&1 | Out-Null
            Clear-StlinkPersisted
            $tracked.Remove($key)
        }
    }

    Start-Sleep -Seconds 1
}
