Param(
    [String]$CN = "localhost",
    [String]$OU,
    [String]$O,
    [String]$L,
    [String]$ST,
    [String]$C,
    [String]$FriendlyName,
    [Switch]$Help
)

Write-Output @"
Vnetworking Library
Copyright (C) V0idPointer`n
"@

if($Help) {

    Write-Output @"
Syntax:
    cert.cmd [options]

Description:
    Creates a self-signed certificate

Options:
    
    -CN <common name>

    -OU <organizational Unit>

    -O <organization>

    -L <locality>

    -ST <state>

    -C <country>

    -FriendlyName <friendly name>

    -Help
        Show help information.
"@

    Exit
}

$attribs = @(
    ("CN", $CN),
    ("OU", $OU),
    ("O", $O),
    ("L", $L),
    ("ST", $ST),
    ("C", $C)
)

# concat attributes into a subject/issuer string:
$subject = ""
ForEach ($attrib in $attribs) {

    If([String]::IsNullOrEmpty($attrib[1])) {
        Continue
    }

    If (-Not [String]::IsNullOrEmpty($subject)) {
        $subject += ", "
    }

    $subject += ($attrib[0] + "=" + $attrib[1])

}

If ([String]::IsNullOrEmpty($subject)) {
    Write-Output "*** Error: subject name cannot be empty."
    Exit
}

# if the friendly name is not provided, use common name:
If ([string]::IsNullOrEmpty($FriendlyName)) {
    $FriendlyName = $CN
}

# create a certificate:
$params = @{
    DnsName = $CN
    Subject = $subject
    FriendlyName = $FriendlyName
    CertStoreLocation = "Cert:\CurrentUser\My"
}

New-SelfSignedCertificate @params -KeyExportPolicy Exportable | Out-Null

Write-Output "OK!"