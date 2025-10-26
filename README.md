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

> Il est possible de spécifier un autre algorithme de hash. Par défaut, il s'agit de "md5".

**Rechercher dans les mots de passes fuités en fonction du condensat que l'on passe en STDIN :**
```
docker run --rm -it -v "$(pwd)/output:/app/output:ro" t3c:dev -L -i /app/output/t3c.csv
```

> Entrez un condensat.

> CTRL-D pour avoir le mot de passe qui correspond au condensat, s'il existe dans la table.

**Afficher l'aide :**
```
docker run --rm t3c:dev -h
```


## Maintainer

- Julien Poirou
