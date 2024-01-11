Param(
    [String]$Platform = "x64",
    [String]$Configuration = "Release",
    [Switch]$Http = $False,
    [Switch]$Help = $False
)

Write-Output @"
Vnetworking Library
Copyright (C) V0idPointer`n
"@

If ($Help) {

    Write-Output @"
Syntax:
    build.cmd [options]

Description:
    Builds Vnetworking

Options:

    -Platform <platform>
        Available options: x86, x64

    -Configuration <configuration>
        Available options: Release, Debug
    
    -Http
        Build Vnetworking HTTP Library (Vnethttp)

    -Help
        Show help information.
"@

    Exit
}

$buildStarted = Get-Date

# list of Vnetlib components:
$vnetComponents = @{
    Core = $True        # Vnetcore should always be built.
    HTTP = $Http
}

# locate msbuild:
$msbuild = & "$(${env:ProgramFiles(x86)})\Microsoft Visual Studio\Installer\vswhere.exe" `
    -latest -prerelease -products * -requires Microsoft.Component.MSBuild `
    -find MSBuild\**\Bin\MSBuild.exe | Select-Object -First 1

ForEach ($p In $Platform.Split(",")) {
    ForEach ($c in $Configuration.Split(",")) {

        # just skip if platform or config is an empty string.
        # this could happen if, for example this was user input: -Platform x86,,amd64,arm64
        If([String]::IsNullOrEmpty($p) -Or [String]::IsNullOrEmpty($c)) {
            Continue
        }

        # check if the provided configuration is valid:
        If (-Not ( ($c -Ieq "Release") -Or ($c -Ieq "Debug") ) ) {
            Write-Output "*** `"$($c)`" is not a valid configuration"
            Continue
        }

        # since user input is case-insensitive, make it look right:
        # configuration should be either Debug or Release, and platform should be lowercase.
        $c = ( $c.Substring(0, 1).ToUpper() + $c.Substring(1).ToLower() )
        $p = $p.ToLower()

        ForEach ($buildTarget In $vnetComponents.Keys) {

            If (-Not $vnetComponents[$buildTarget]) {
                Continue
            }

            Write-Output "*** Building $($buildTarget) for $($p) ($($c))"

            $msbuildArgs = @(
                ("Vnetworking.sln"),
                ("/target:$($buildTarget)"),
                ("/p:Platform=$($p)"),
                ("/p:Configuration=$($c)")
            )
    
            & $msbuild $msbuildArgs

        }

    }
}

$elapsed = ( (Get-Date) - $buildStarted )
$minutes = ( [Math]::Floor($elapsed.TotalMinutes) )
$seconds = ( [Math]::Ceiling($elapsed.TotalSeconds) - ($minutes * 60) )

Write-Output "`n*** Build finished. Took $($minutes) minutes, $($seconds) seconds."