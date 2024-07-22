$DIR = "C:\Users\MariusPC\CLionProjects\PFF\bin-int";
if (-Not (Test-Path $DIR)) {
    New-Item -Path $DIR -ItemType Directory
};
Set-Location $DIR;
if (cmake ..) {
    cmake --build .
} else {
    Write-Host "cmake configuration failed";
    exit 1
}