Write-Output @"
Vnetworking Library
Copyright (C) V0idPointer`n
"@

$vnetComponents = @(
    ("Core"),
    ("HTTP")
)

# delete bin and int directories for all vnet components:
ForEach ($buildTarget In $vnetComponents) {
    
    $binDir = ".\$($buildTarget)\bin"
    $intDir = ".\$($buildTarget)\int"

    If (Test-Path -LiteralPath $binDir) {
        Remove-Item -LiteralPath $binDir -Force -Recurse
    }

    If (Test-Path -LiteralPath $intDir) {
        Remove-Item -LiteralPath $intDir -Force -Recurse
    }

}

Write-Output "*** OK"