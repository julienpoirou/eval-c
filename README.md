# Outil de table de correspondance mots de passes <=> condensat

## Prérequis

- gcc >= 15
- OpenSSL (libcrypto) >= 3.0
- rockyou.txt (optionnel)

## Fonctionnalités

- Générer un csv depuis un dictionnaire `.txt` avec le mode generate.
- Rechercher des mots de passe en se basant sur les condensats avec le mode lookup (arbre binaire de recherche).

## Fonctionnement

```mermaid
flowchart LR
  subgraph Generate [-G / --generate]
    A[Dictionary (.txt)] -- lignes --> B[merge_table(algorithm)]
    B -- OpenSSL EVP --> C((Table en RAM))
    C --> D[write_csv]
    D --> E[/app/output/t3c.csv/]
  end

  subgraph Lookup [-L / --lookup]
    E --> F[load_csv]
    F --> G[(BST: tree_build)]
    H[STDIN: condensat(s)] --> I[hexadecimal_to_bytes]
    I --> J{tree_find}
    J -- match --> K[print raw]
    J -- miss --> L[(silence)]
  end
```

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
