Shell fonctionel

-La commande job affiche une liste des jobs ainsi que leur réference propre
L'argument -l permet l'affichage de tout les processus engendrés groupés par gpid

-les commandes fg et bg sont fonctionelles et permettent bien de passer en avant et second plan

-l'argument & permet bien de passer le processus en arriere plan

-le signal SIGTSTP suspend  le shell;
-le signal SIGINT  provoque bien la terminaison (et le shell reprend normalement).

-L'utilisation de "|" permet le lancement de multiples commandes en une seule ligne.
 La transmission de données et le groupage n'ont pas été implementés mais vous trouverez en commentaire des tentatives dans cette voie 

Cela est dû au fait que je me suis concentré tout d'abord sur le bon fonctionnement des commandes , avant de tenter d'implementer les points du sujets un à un 
(les "|" été en partie responsable de ma fork bomb c'est pour cela que je me suis concentré sur le reste avant de revenir dessus)

Docker fonctionne. Mais le code est aussi sur lulu


> xeyes | xclock &
> jobs
>jobs -L
>fg 1
> exit