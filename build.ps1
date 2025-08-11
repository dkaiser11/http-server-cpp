#!/usr/bin/env pwsh

$ErrorActionPreference = "Stop"

Set-Location -Path $PSScriptRoot

Write-Host "Configuring build..." -ForegroundColor Green
cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug -G "MinGW Makefiles" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++

Write-Host "Building..." -ForegroundColor Green
cmake --build .\build

Write-Host "Running tests..." -ForegroundColor Green
Set-Location build
ctest --output-on-failure
Set-Location ..

Write-Host "Build completed successfully!" -ForegroundColor Green
