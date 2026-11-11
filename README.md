# Bataille Navale Client-Serveur en C

Une implementation robuste et multi-joueurs du jeu de la **Bataille Navale** en C bare-metal/POSIX, s'appuyant sur l'architecture réseau client-serveur via des sockets TCP.

---

## Sommaire

- [Fonctionnalites](#fonctionnalites)
- [Architecture & Protocoles](#architecture--protocoles)
- [Structure du Projet](#structure-du-projet)
- [Prerequis](#prerequis)
- [Compilation](#compilation)
- [Execution](#execution)
- [Regles du Jeu](#regles-du-jeu)
- [Gestion de la Robustesse et des Signaux](#gestion-de-la-robustesse-et-des-signaux)

---

## Fonctionnalites

* **Architecture Multi-joueurs Concourante** : Le serveur gere plusieurs sessions de jeu independantes simultanement via du *fork* d'un sous-processus par client.
* **Limite de Connexions Actives** : Controle dynamique du nombre de joueurs connectes (limite configuree a 5 sessions simultanees).
* **Communication Binaire Optimisee** : Transmissions d'objets structures (`position_t`, `reponse_serveur_t`) sans alignement memoire parasite (`__attribute__((packed))`).
* **Saisie Client Securisee** : Nettoyage et validation des entrées utilisateur pour eviter la pollution de la memoire-tampon (`stdin`).
* **Gestion Propre de la Memoire et des Processus** :
  * Recyclage automatique des processus fils (*zombies*) via l'interception du signal `SIGCHLD`.
  * Nettoyage de la memoire allouee dynamiquement (`free`) a la fin de chaque partie.
  * Interception des signaux d'arret (`SIGINT`, `SIGTERM`) pour informer proprement les clients de la deconnexion.

---

## Architecture & Protocoles

### 1. Protocole de Communication Binaire

#### Client $\rightarrow$ Serveur
Le client valide la saisie utilisateur et envoie une structure de 8 octets codant la position donnee :

$$\text{Taille} = 8 \text{ octets} \quad (x \text{ : int}, y \text{ : int})$$

```c
typedef struct {
    int x; // 4 octets
    int y; // 4 octets
} __attribute__((packed)) position_t;
```

#### Serveur $\rightarrow$ Client
Le serveur traite la frappe sur la grille et retourne un compte-rendu d'etat :

```c
enum RESULTAT_TIR { RATEE, TOUCHEE, COULEE, GAGNEE, PERDU };

typedef struct {
    enum RESULTAT_TIR resultat;
    int coup_restant;
} __attribute__((packed)) reponse_serveur_t;
```

---

## Structure du Projet

```
.
├── client.c              # Point d'entree et boucle principale du client TCP
├── client_fonction.c     # Logique de saisie, validation et interpretation client
├── client_fonction.h     # Definitions des structures et prototypes du client
├── serveur.c             # Serveur TCP, gestion des sockets et sous-processus
├── serveur_fonction.c    # Moteur de jeu, placement de la flotte, allocation memoire
├── serveur_fonction.h    # Structures de donnees globales et constantes de jeu
└── .gitignore            # Exclusions de build
```

---

## Prerequis

* Compilateur C supportant la norme **C99** ou **C11** (ex: `gcc`, `clang`).
* Système d'exploitation type Unix / Linux (repose sur l'API des sockets POSIX, `fork`, `sigaction` et `kill`).

---

## Compilation

Un binaire distinct est généré pour le client et le serveur. 

Pour compiler l'ensemble des modules :

```bash
# Compilation du Client
gcc -Wall -Wextra -Werror client.c client_fonction.c -o test_client.exe

# Compilation du Serveur
gcc -Wall -Wextra -Werror serveur.c serveur_fonction.c -o test_serveur.exe
```

---

## Execution

### 1. Demarrer le Serveur

Le serveur prend en arguments un identifiant et le port d'ecoute.

```bash
./test_serveur.exe <id_serveur> <port>
```
*Exemple :*
```bash
./test_serveur.exe SRV01 8080
```

### 2. Lancer un Client

Le client requiert l'adresse IP du serveur et le port d'ecoute.

```bash
./test_client.exe <ip_serveur> <port>
```
*Exemple :*
```bash
./test_client.exe 127.0.0.1 8080
```

---

## Regles du Jeu

1. **Grille de jeu** : La carte s'etend sur une grille de $50 \times 50$ cases (coordonnees de `0` a `49`).
2. **Quota de coups** : Chaque joueur dispose de **50 tirs** au total pour tenter de couler la flotte.
3. **Flotte deployee** :
   * **1 Porte-Avions** (occupe 5 cases consecutivement)
   * **1 Fregate** (occupe 3 cases consecutivement)
4. **Format de saisie** : Les coordonnes doivent etre passees sous la forme `X;Y` ou via un espace (ex: `12;34` ou `12 34`).
5. **Fin de partie** :
   * **Victoire** : Tous les navires de la flotte adverse sont totalement detruits avant l'epuisement des coups.
   * **Defaite** : Le compteur de coups atteint 0 alors qu'il reste au moins un navire en mer.

---

## Gestion de la Robustesse et des Signaux

* **Saisie utilisateur** : En cas de saisie invalide ou trop longue, un mecanisme de vidage automatique du flux `stdin` empeche la saturation de la memoire tampon du client.
* **SIGCHLD** : Le serveur pere recupere l'etat de fin de ses fils sans bloquer la boucle d'ecoute principal (`waitpid` non-bloquant avec `WNOHANG`), ajustant `nb_joueurs_actifs`.
* **SIGINT / SIGTERM** : Lors de l'interruption du serveur pere par `Ctrl+C`, un signal `SIGTERM` est transmis à l'ensemble des fils (`kill(0, SIGTERM)`) pour clore proprement chaque partie et liberer la memoire allouee.
