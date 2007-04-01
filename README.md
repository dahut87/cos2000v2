![logo](https://github.com/dahut87/cos2000v2/raw/master/Graphisme/logo.png)

## Présentation du projet

### Qu'est ce que COS2000 ?

COS2000 est un système d'exploitation minimaliste qui vise essentiellement un objectif pédagogique.
Il s'agit avant tout du travail d'un passionné de programmation système.

Le système est développé essentiellement en langage C mais il comporte aussi un peu d'assembleur X86.

#### Qu'est ce qu'un système d'exploitation ?

Il s'agit du logiciel principal qui anime votre ordinateur. Il a pour fonctions principales :

* Piloter des périphériques tels que souris, clavier, écran, imprimante...etc;
* Gérer des fichiers qui sont produit et exploités par des applications; 
* Coordonner l'accès aux ressources par plusieurs utilisateurs;
* Détection et récupération des dysfonctionnements;
* Surveiller les performances et assurer une gestion optimale des ressources systèmes.

Sans système d'exploitation votre ordinateur est inopérant: c'est une boite vide !

#### Comment utiliser COS2000 ?

COS2000 n'a pas pour but d'être utilisé en production. Il s'agit d'un système en cours de
développement que vous pouvez néanmoins tester sur un ordinateur physique ou de préférence sur une machine virtuelle.
Il est nécessaire de compiler le système avant de démarrer celui-ci à partir d'un périphérique amovible (clé usb).

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

### En savoir plus...

#### Historique du projet
* Version 2.2fr - C en mode protégé Reprise du projet
* Version 2.1fr - C en mode protégé Abandon du projet
* Version 2.0   - C en mode protégé VGA text+pmode
* Version 1.x   - Assembleur en mode réel

>  “La connaissance s'acquiert par l'expérience,
>          tout le reste n'est que de l'information..
>
>   ― Albert Einstein
>   ― Mathématicien, Physicien