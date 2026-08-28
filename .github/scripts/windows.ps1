param(
    [ValidateSet('v142', 'v143')]
    [string] $Toolset = 'v143'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$locked = (Get-Content -LiteralPath './flake.lock' -Raw | ConvertFrom-Json).nodes.libmustache.locked
if ($locked.type -ne 'github' -or -not $locked.owner -or -not $locked.repo -or -not $locked.rev) {
    throw 'flake.lock does not contain a locked GitHub libmustache input'
}

$temporaryRoot = if ($env:RUNNER_TEMP) { $env:RUNNER_TEMP } else { [IO.Path]::GetTempPath() }
$work = Join-Path $temporaryRoot 'libmustache'
$source = Join-Path $work 'source'
$build = Join-Path $work 'build'
$prefix = Join-Path $work 'prefix'
$repository = "https://github.com/$($locked.owner)/$($locked.repo).git"

if (Test-Path -LiteralPath $work) {
    throw "Refusing to reuse existing libmustache work directory: $work"
}
New-Item -ItemType Directory -Path $work | Out-Null

& git clone --filter=blob:none --no-checkout $repository $source
if ($LASTEXITCODE -ne 0) { throw "git clone failed with exit code $LASTEXITCODE" }

Push-Location $source
try {
    & git fetch --depth=1 origin $locked.rev
    if ($LASTEXITCODE -ne 0) { throw "git fetch failed with exit code $LASTEXITCODE" }
    & git checkout --detach FETCH_HEAD
    if ($LASTEXITCODE -ne 0) { throw "git checkout failed with exit code $LASTEXITCODE" }
} finally {
    Pop-Location
}

& cmake -S $source -B $build -G 'Visual Studio 17 2022' -A x64 -T $Toolset `
    -DMUSTACHE_BUILD_CLI=OFF `
    -DMUSTACHE_ENABLE_TESTS=OFF `
    -DMUSTACHE_ENABLE_JSON=OFF `
    -DMUSTACHE_ENABLE_YAML=OFF `
    "-DCMAKE_INSTALL_PREFIX=$prefix"
if ($LASTEXITCODE -ne 0) { throw "cmake configure failed with exit code $LASTEXITCODE" }

& cmake --build $build --config Release --parallel
if ($LASTEXITCODE -ne 0) { throw "cmake build failed with exit code $LASTEXITCODE" }

& cmake --install $build --config Release
if ($LASTEXITCODE -ne 0) { throw "cmake install failed with exit code $LASTEXITCODE" }

foreach ($requiredPath in @(
    (Join-Path $prefix 'lib\mustache_static.lib'),
    (Join-Path $prefix 'include\mustache\mustache_config.h'),
    (Join-Path $prefix 'include\mustache\mustache_export.hpp')
)) {
    if (-not (Test-Path -LiteralPath $requiredPath -PathType Leaf)) {
        throw "Missing required libmustache artifact: $requiredPath"
    }
}

if ($env:GITHUB_OUTPUT) {
    "prefix=$prefix" | Out-File -LiteralPath $env:GITHUB_OUTPUT -Encoding utf8 -Append
} else {
    Write-Output "prefix=$prefix"
}
