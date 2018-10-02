![logo](https://github.com/dahut87/cos2000v2/raw/develop/Graphisme/logo.png)

## Présentation du projet

### Qu'est ce que COS2000 ?

COS2000 est un système d'exploitation minimaliste qui vise essentiellement un objectif pédagogique.
Il s'agit avant tout du travail d'un passionné de programmation système.

Le système est développé essentiellement en langage C mais il comporte aussi un peu d'assembleur X86.

#### Qu'est ce qu'un système d'exploitation ?

Il s'agit du logiciel principal qui anime votre ordinateur. Il a pour fonctions principales :

* Piloter des périphériques, souris, clavier, écran, imprimante...etc;
* Gérer des fichiers qui sont produit et exploités par des applications; 
* Coordonner l'accès aux ressources pour plusieurs usagers;
* Détection et récupération de dysfonctionnements;
* Surveiller les performances et assurer une gestion optimale des ressources systèmes.

Sans système d'exploitation votre ordinateur est inopérant: c'est une boite vide !

#### Comment utiliser COS2000 ?

COS2000 n'a pas pour but d'être utilisé en exploitation, c'est un système en cours de
développement. Vous pouvez néanmoins l'utiliser sur un ordinateur physique ou une machine virtuelle
afin de voir le fonctionnement d'un système rudimentatire. Voir compilation de COS2000...

#### Sur quel ordinateur fonctionne t'il ?

COS2000 est compatible avec tout ordinateur compatible PC comprenant un processeur avec FPU intégré,
gestion de la mémoire paginée et mode protégé : Intel Pentium et supérieur.

#### Ai-je le droit de le copier, de le modifier, de le réutiliser, de le vendre ?

COS2000 est sous licence LGPL v3.0, en simplifiant un peu :

* COPIER     - OUI
* MODIFIER   - OUI
* REUTILISER - OUI en citant l'auteur
* VENDRE     - NON

### Compilation de COS2000

#### Avant la compilation

##### Logiciels utilisés

L'OS a été développé sous Linux afin de disposer d'un maximum d'outils. Je vous conseille donc
d'utiliser ce système afin de procéder à la compilation même si celle-ci peut fonctionner 
depuis Cygwin. Des distributions "Live" existent si vous ne souhaitez pas installer sur votre
ordinateur (http://www.linuxmint.com/edition.php?id=259).

Les numéros de version des logiciels sont données à titre indicatif car il est fort probable que
 cela fonctionne parfaitement avec d'autres versions antérieures.

Compilation:

* NASM version 2.11.08
* gcc version 5.4.0 20160609 
* GNU Make version 4.1
* binutils version 2.26-8

Images disques, débogage & émulation:

* fuse-umfuse-ext2 version 0.4 29
* fusefat version 0.1a-1.1
* CGDB: a curses debugger version 0.6.7
* QEMU emulator version 2.5.0
* OVMF version 0~20160408.ffea0a2c-2
* hexdump (bsdmainutils version 9.0.6)
* dd (coreutils version 8.25-2)
* tar version 1.28-2.1
* GNU indent version 2.2.11

##### Téléchargement de COS2000

Afin de faciliter la mise à jour et le suivi des version de COS2000, GIT est utilisé.

Si vous n'avez pas GIT, installez le avec la commande suivante :

```sudo apt-get install git```

Puis cloner le source chez vous avec la commande :

```git clone https://github.com/dahut87/cos2000.git```

##### Installation des paquets nécessaires

Sous linux - Debian & Ubuntu like

```sudo apt-get install nasm gcc qemu fusefat fuseext2 cgdb ovmf bsdmainutils tar bsdmainutils indent binutils```

ou sinon, depuis un terminal disposé sur le répertoire cos2000

```make install```

#### Compilation

Commandes de compilation de COS2000

* ```make all``` compile tout le projet dans sa version disquette, disque dur et disque dur UEFI
* ```make floppy``` compile la version disquette
* ```make harddisk``` compile la version disque dur
* ```make uefi``` compile la version disque dur UEFI
* ```make floppytest``` lance l'émulation QEMU en 32 bits sur disquette
* ```make test``` lance l'émulation QEMU en 32 bits sur disque dur
* ```make test64``` lance l'émulation QEMU en 64 bits sur disque dur UEFI
* ```make clean``` supprime les fichers compilés

#### Utilisation

##### Sur un ordinateur émulé

Pour tester l'OS en émulation taper donc la commande ```make test``` qui compilera avant de lancer Qemu.

##### Sur un ordinateur physique

Il faut d'abord copier l'image sur une clé (Attention l'opération effacera le contenu de la clé) :

```dd if=./final/harddisk.img.final of=/dev/sdx bs=1M``` (ou sdx est votre périphérique)

Bootez sur votre clé en mode bios (legacy). Il est possible que des dysfonctionnement apparaissent sur des machine x86_64 (en cours de résolution).

##### Usage de COS2000

Pour l'instant quelques commandes seulement sont disponibles:

* REBOOT redémarre le PC,
* CLEAR efface l'écran,
* MODE change le mode video,
* DETECTCPU detecte et affiche les informations CPU,
* TEST2D teste l'affichage graphique 2D,
* REGS affiche les registres CPU,
* GDT affiche la table des descripteurs,
* IDT affiche la table des interruptions,
* INFO affiche des informations issues de GRUB.

![COS2000 le 28-09-2018](https://github.com/dahut87/cos2000v2/raw/develop/Graphisme/screenshots/28-09-2018.png)

#### Organisation du dépôt

* boot - fichiers source assembleur démarrage FAT12 - disquette
* debug - fichiers configuration débogueur
* final - img raw utilisables avec un émulateur des 3 versions
* * harddisk.img.final
* * harddiskuefi.img.final
* * floppy.img.final
* Graphisme - fichiers images
* *  screenshots - screenshots de l'évolution du système
* *  Thème - thème de démarrage COS2000 pour plymouth
* include - fichier d'entête C
* lib - librairies pour le noyau
* makefile - Makefile du projet
* programs - programme pour le domaine utilisateur (en cours)
* README.md - ce que vous lisez
* system - le noyau lui-même

### En savoir plus...

#### Avancement du projet

* secteur de boot disquette (FAT12),
* démarrage autre support par GRUB (Disque dur/SSD, réseau...Etc),
* pilote souris, clavier, VGA,
* gestion des interruptions et de la segmentation GDT,
* interpréteur de commande,
* noyau avec fonctions minimales implémentées (printf, itoa, maths...),
* mode protégé limité à 4Go de mémoire vive.

#### A faire
* gestion du framebuffer,
* gestion de la mémoire paginée,
* espace utilisateur et appels systèmes,
* ordonnanceur de tâche,
* chargeur ELF32,
* pilote IDE/ATA et lecture/ecriture EXT2FS.

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