![logo](https://github.com/dahut87/cos2000v2/raw/master/Graphisme/logo.png)

## COS2000

### Présentation du projet

#### Qu'est ce que COS2000 ?

COS2000 est un système d'exploitation minimaliste dont l'objectif est essentiellement pédagogique.
Il s'agit avant tout du travail d'un passionné de programmation système.

Le système est développé essentiellement en langage C mais il comporte aussi quelques lignes d'assembleur X86.

#### Qu'est ce qu'un système d'exploitation ?

Il s'agit du logiciel principal qui anime votre ordinateur. Il a pour fonctions principales :

* Piloter des périphériques tels que souris, clavier, écran, imprimante...etc;
* Gérer des fichiers qui sont produit et exploités par des applications; 
* Coordonner l'accès aux ressources par plusieurs utilisateurs;
* Détection et récupération des dysfonctionnements;
* Surveiller les performances et assurer une gestion optimale des ressources systèmes.

Sans système d'exploitation votre ordinateur est inopérant: c'est une boite vide !

#### Comment utiliser COS2000 ?

COS2000 n'a pas pour but d'être utilisé en production. Il s'agit d'un système en cours de développement que vous pouvez néanmoins tester sur un ordinateur physique ou de préférence sur une machine virtuelle. Il est nécessaire de compiler le système avant de démarrer celui-ci à partir d'un périphérique amovible (clé usb).

#### Sur quel ordinateur fonctionne t'il ?

COS2000 est compatible avec tout ordinateur compatible PC comprenant un processeur avec FPU intégré,
gestion de la mémoire paginée et mode protégé : Intel Pentium et supérieur.

#### Ai-je le droit de le copier, de le modifier, de le réutiliser, de le vendre ?

COS2000 est sous licence LGPL v3.0, en simplifiant un peu :

* COPIER     - OUI
* MODIFIER   - OUI
* REUTILISER - OUI en citant l'auteur
* VENDRE     - NON

Les détails se trouvent dans le fichier `LICENSE.md` 

![logo](https://www.gnu.org/graphics/lgplv3-with-text-154x68.png)

### Compilation de COS2000

#### Logiciels utilisés

L'OS a été développé sous Linux afin de disposer d'un maximum d'outils dont GCC/NASM/Docker et l'ensemble des BINUTILS. Je vous conseille donc d'utiliser Linux afin de procéder à la compilation même si celle-ci peut fonctionner depuis Windows grace à Cygwin. Des distributions "Live" existent si vous ne souhaitez pas installer sur votre ordinateur (http://www.linuxmint.com/edition.php?id=259).

Le système de conteneurisation Docker est exploité afin de d'assurer une reproductibilité totale de l'environnement de compilation quelque soit le système d'exploitation utilisé.

#### Installation/compilation automatique

Téléchargez et exécutez le script d'installation automatique et de compilation "menu.sh"

`wget https://raw.githubusercontent.com/dahut87/cos2000v2/master/menu.sh`

Executez le script et les sources seront ainsi automatiquement téléchargées sur votre ordinateur par le biais de Git.

`chmod 755 ./menu.sh && ./menu.sh`

Un menu s'affiche ensuite qui vous propose de réaliser différentes tâches dont la compilation et/ou le test de COS2000.

![Menu de compilation](https://github.com/dahut87/cos2000v2/raw/master/Graphisme/screenshots/compilation.png)

#### Installation/compilation manuelle

##### Téléchargement de COS2000

Afin de faciliter la mise à jour et le suivi des version de COS2000, GIT est utilisé.

Si vous n'avez pas GIT, installez le avec la commande suivante (sous paquet debian .deb):

`sudo apt-get install git`

Puis cloner le source chez vous avec la commande :

`git clone https://github.com/dahut87/cos2000.git`

##### Compilation en ligne de commande
  
Docker est utilisé afin de permettre le fonctionnement correcte de la compilation. Au premier lancement de make.sh, si vous utilisez une distribution basée sur le système de paquet Debian, l'installation sera réalisée automatiquement. Dans le cas contraire, installez Docker et relancez le script. Une image docker est produite afin de procéder ensuite à la compilation du système.

Commande de compilation de base

* `./make.sh all` compile tout le projet dans sa version disquette

Autres commandes de compilation de COS2000
* `./make.sh test` lance l'émulation QEMU en 32 bits sur disquette
* `./make.sh clean` supprime les fichers compilés
* `./make.sh debug-boot` debogue le système en mode réel depuis le boot
* `./make.sh debug-loader` debogue le système en mode réel depuis le loader
* `./make.sh debug-system` debogue le système en mode protégé
* `./make.sh config` change la configuration de la compilation

### Utilisation

#### Sur un ordinateur émulé

Pour tester l'OS en émulation taper donc la commande `./make.sh test` qui compilera avant de lancer Qemu.

#### Commandes de COS2000

Pour l'instant quelques commandes seulement sont disponibles:

* `REBOOT` redémarre le PC,
* `CLEAR` efface l'écran,
* `MODE` change le mode video,
* `DETECTCPU` detecte et affiche les informations CPU,
* `DETECTPCI` detecte et affiche les périphériques PCI,
* `TEST2D` teste l'affichage graphique 2D,
* `HELP` affiche les commandes disponibles.

![COS2000 le 28-09-2018](https://github.com/dahut87/cos2000v2/raw/master/Graphisme/screenshots/ansi.png)

### En savoir plus...

#### Historique du projet
* Version 2.2fr - C en mode protégé Reprise du projet
* Version 2.1fr - C en mode protégé Abandon du projet
* Version 2.0   - C en mode protégé VGA text+pmode
* Version 1.x   - Assembleur en mode réel (v1.0 - 1.4.2fr)

>  “La connaissance s'acquiert par l'expérience,
>          tout le reste n'est que de l'information..
>
>   ― Albert Einstein
>   ― Mathématicien, Physicien
