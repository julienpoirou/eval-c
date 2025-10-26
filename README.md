# Outil de table de correspondance mots de passes <=> condensat

## Prérequis

- gcc >= 15
- OpenSSL (libcrypto) >= 3.0
- rockyou.txt (optionnel)

## Build

```
docker build -t t3c:dev --build-arg APP_ENVIRONMENT=DEVELOPMENT .
```

## Run

**Générer les condensats dans un CSV :**
```
docker run --rm -v "$(pwd)/dictionary:/data:ro" -v "$(pwd)/output:/app/output" t3c:dev -G -i /data/rockyou.txt
```

**Rechercher dans les mots de passes fuités en fonction du condensat que l'on passe en STDIN :**
```
docker run --rm -v "$(pwd)/output:/app/output:ro" t3c:dev -L -i /app/output/t3c.csv
```

> CTRL-D pour avoir la réponse.

## Maintainer

- Julien Poirou
