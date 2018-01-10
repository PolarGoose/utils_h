Function Info($msg) {
    Write-Host -ForegroundColor DarkGreen "`nINFO: $msg`n"
}

Function Error($msg) {
    Write-Host `n`n
    Write-Error $msg
    exit 1
}

Function CheckReturnCodeOfPreviousCommand($msg) {
    if(-Not $?) {
        Error "${msg}. Error code: $LastExitCode"
    }
}

Function CreateZipArchive($directory, $archiveFile) {
    Info "Create a zip archive from `n  '$directory' `n to `n  '$archiveFile'"
    New-Item $archiveFile -Force -ItemType File > $null
    Compress-Archive -Force -Path $directory -DestinationPath $archiveFile
}

Function ForceCopy($srcFile, $dstFile) {
    Info "Copy `n  '$srcFile' `n to `n  '$dstFile'"
    New-Item $dstFile -Force -ItemType File > $null
    Copy-Item $srcFile -Destination $dstFile -Force
}

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$root = Resolve-Path $PSScriptRoot
$buildDir = "$root/out"
$publishDir = "$buildDir/publish"

Info "Build the project using Cmake"
Info "Cmake generation phase"
cmake -S $root -B $buildDir -G Ninja -DCMAKE_BUILD_TYPE=Release
CheckReturnCodeOfPreviousCommand "Cmake generation phase failed"
Info "Cmake build phase"
cmake --build $buildDir
CheckReturnCodeOfPreviousCommand "Cmake building phase failed"

Info "Run tests"
& $buildDir/utils_h_test.exe
CheckReturnCodeOfPreviousCommand "Tests failed"

CreateZipArchive $root/src/utils_h $publishDir/utils_h.zip
