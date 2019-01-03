FROM microsoft/dotnet:sdk AS builder

WORKDIR /build

COPY . .

RUN dotnet publish -o release ./Goldtree.Core

FROM microsoft/dotnet:runtime

WORKDIR /app

COPY --from=builder /build/Goldtree.Core/release .

ENTRYPOINT ["dotnet", "/app/Goldtree.Core.dll"]