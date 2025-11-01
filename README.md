# Outil de table de correspondance mots de passes <=> condensat

## Prérequis

- gcc >= 15
- OpenSSL (libcrypto) >= 3.0
- rockyou.txt (optionnel)

## Fonctionnalités

- Générer un csv depuis un dictionnaire `.txt` avec le mode generate.
- Rechercher des mots de passe en se basant sur les condensats avec le mode lookup (arbre binaire de recherche).

## Build

```bash
docker build --build-arg APP_ENVIRONMENT=DEVELOPMENT --target export --output type=local,dest=. .
```

## Build Windows

```bash
docker build --no-cache -f Dockerfile_windows -t t3c:dev --build-arg APP_ENVIRONMENT=DEVELOPMENT .
```

## Build Valgrind

```bash
docker build --no-cache -f Dockerfile_valgrind -t t3c:valgrind --build-arg APP_ENVIRONMENT=DEVELOPMENT .
```

## Run

**Générer les condensats dans un CSV :**
```bash
docker run --rm -v "$(pwd)/dictionary:/data:ro" -v "$(pwd)/output:/app/output" t3c:dev -G -i /data/rockyou_lite.txt
```

> Il est possible de spécifier un autre algorithme de hash. Par défaut, il s'agit de "md5".

**Rechercher dans les mots de passes fuités en fonction du condensat que l'on passe en STDIN :**

```bash
docker run -it --rm -v "$(pwd)/output:/app/output:ro" t3c:dev -L -i /app/output/t3c.csv
```

Passer un fichier (linux) :
```bash
docker run -it --rm -v "$(pwd)/output:/app/output:ro" t3c:dev -L -i /app/output/t3c.csv < digests.txt
```

Passer un fichier (windows) :
```bash
Get-Content .\digests.txt | docker run -i --rm -v "${PWD}\output:/app/output:ro" t3c:dev -L -i /app/output/t3c.csv
```

**Afficher l'aide :**
```bash
docker run --rm t3c:dev -h
```

> CTRL-D pour avoir la réponse.

## Maintainer

- Julien Poirou
