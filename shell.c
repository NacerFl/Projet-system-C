#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>

#define MAX_JOBS 1024
#define DELIMITER " \t\r\a"
#define SIZE 1024


void affichage(char * commande, char * args[32]);
int rwPipe(int fd, int pid);
char * itoa (int n);
void traitement_fork(char **args , char **plop,int position);
void display_job(char * args[]);


pid_t jobs_tab[MAX_JOBS];
char *mes_commande[MAX_JOBS];
int pidsize = 0;
int curre =0; //l'emplacment du job courant qui est du coup libre
int curre2 = 0;
char  *dir = NULL;

//handle CTRL +C & CTRL+Z
void manager(int sig){

 if (jobs_tab[curre] == 0) { 

    return;
 }
    if (sig == SIGTSTP)
    {
        kill(jobs_tab[curre],20);
    }
    if (sig == SIGINT){
         
        
        
        kill(jobs_tab[curre],2);
        jobs_tab[curre] = 0;
    }
 
}


int main(int argc, char *argv[]){
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = manager;
    sigaction(SIGTSTP, &action, NULL);
    sigaction(SIGINT, &action, NULL);

    dir = malloc(256 * sizeof(char));
    dir = getcwd(dir,256);
    char *line = '\0';// ligne de commande
    size_t len = 0;
    ssize_t readus = 0;
    int position = 0;
    char * commande = malloc(256 * sizeof(char));
    memset(commande,0,256);
    char *ligne_parser = malloc(256 * sizeof(char));
    memset(ligne_parser,0,256);
    char *ligne_parser2 = malloc(256 * sizeof(char));
    char *args[32];

    for (int i = 0; i < 32; i++) // vide ma liste argument
    {
        args[i] = NULL;
    }
    for (int i = 0; i < MAX_JOBS; i++){ // vide mes deux tableau d'arguments
        jobs_tab[i] = 0;
    }

    printf("************ ON THE HIGHWAY TO SHELL ************\n");
    while (1){
        printf("> ");
        readus = getline(&line, &len, stdin);

        if (readus == -1){
            perror("read error");
            exit(EXIT_FAILURE);
        }

        ligne_parser = strtok(line,"\n"); //découpe en séquence "en commande"
        if (ligne_parser != NULL) {
            setpgid(0,0); // set gpid à chaque nouvelle commande
            char * head;
            args[0] = malloc(256 * sizeof (char));
            ligne_parser2 = strtok_r(ligne_parser,"|",&head);
            //int numPipe = 0;
            //int **lesPipes;//liste de liste


            while (ligne_parser2 != NULL){
                char * head2;

                //memcpy(args[0],ligne_parser,256);//afin de faire en sorte que le premier argument est bien la commande 
                char * currArg = strtok_r(ligne_parser2,DELIMITER,&head2);
                position = 0;

                while (currArg != NULL){
                    args[position] = currArg;
                    position ++;
                    currArg = strtok_r(NULL,DELIMITER,&head2);
                }

                char *listeArguments[position + 1];
                for (int i = 0; i < position; i++){
                    listeArguments[i] = args[i];
                }

                listeArguments[position] = NULL; // je met à NULL le dernier argument
                if (strcmp(listeArguments[0],"exit") == 0){
                    for (int i = 0; i < MAX_JOBS; i++)
                    {
                        if (jobs_tab[i] != 0)
                        {
                            kill(jobs_tab[i],9); // je suis méchant ^ ^ inbloquable , inflitrable que demander de plus ?
                        }

                    }
                    exit(EXIT_SUCCESS);
                }
                if (strcmp(listeArguments[0],"cd") == 0){
                    if (listeArguments[1] != NULL){
                        char * tmp = malloc(strlen(dir) * sizeof(char));
                        memcpy(tmp,dir,strlen(dir));
                        strncat(dir,"/",1);
                        strncat(dir,listeArguments[1],strlen(listeArguments[1]));
                        printf("mon path %s\n",dir);
                        if (chdir(dir) == -1){
                            dir = tmp;
                            printf("error cd \n");

                        }
                    }
                    continue;
                }


                if (args[0] == NULL)
                    continue;
                
                 /* if (numPipe != 0){  neccessiterait une complète révison du parsing ce que le temps ne me permet pas :(
                    realloc(lesPipes,sizeof(int *) * (numPipe+1));
                    int currPipe[2];
                    pipe(currPipe[2]);
                    lesPipes[numPipe] = malloc(2 * sizeof(int));
                    lesPipes[numPipe][0] = STDOUT_FILENO;
                    lesPipes[numPipe][1] = currPipe[0];
                    lesPipes[numPipe - 1][0] = currPipe[1];
                }else{
                    lesPipes = { { STDOUT_FILENO ,STDIN_FILENO } };
                } */ 
                
                traitement_fork(args,listeArguments,position); //est censé être traitement_fork(args,listeArguments,position,lesPipes,currPipe)
                //numPipe ++;
                //free(listeArguments);
                //position = 0;
                ligne_parser2 = strtok_r(NULL,"|", &head);

            }



        }
        for (int i = 0; i < 32; i++) // je vide ma liste d'arguments
        {
            args[i]= NULL;
        }

    }
    //free(line);
    //return 0;
}




void affichage(char * commande, char * args[32]){
    printf(" commande := %s\n",commande);
    for (int i = 0; i < 32; i++){
        if (args[i]!= NULL)
        {
            printf("arg[%d] = ", i);
            printf ("%s\n",args[i]);

        }
    }

}

// ne pas oublier dans exit d'aller kill tout ces foutu gosse qui son dans des fork <- c'est fait
void traitement_fork(char **args , char **listeArguments , int position){
    int status;
    mes_commande[curre] = args[0];
    //dup2(STDOUT_FILENO,lesPipes[posPipe][0]); était censé être implémenté avec les pipes
    //dup2(STDIN_FILENO,lesPipes[posPipe][1]);


  //vérifie si le processus existe sinon le supprime du tableau
for (int i = 0; i < MAX_JOBS; i++)
{
    if (jobs_tab[i] != 0)
    {
        /* waitpid(jobs_tab[i], &status, WNOHANG); j'ai essayé avec waitpid mais ne marche pas bien :(
             if (WIFEXITED(status) || WIFSIGNALED(status)) { */

           if (kill(jobs_tab[i],0) != 0) {
            jobs_tab[i] = 0;
            printf("[%d] Fini : %s \n",i,mes_commande[i]);
    }
}
continue;
}

if (jobs_tab[curre] != 0) // je check si il y a un emplacement libre
{
    curre = -1;
    for (int i = 0; i < MAX_JOBS; i++)
    {
        if (jobs_tab[i] == 0)
        {
            curre = i;
            break;
        } // oublie pas d'ajouter un check zombie
    }
    if (curre == -1){
        printf("too many jobs :( pls wait..\n" );
        return;
    }
}







if(strcmp(args[0],"fg") == 0 ){
    if(args[1] != NULL){

        if (atoi(args[1]) > -1 && atoi(args[1]) < MAX_JOBS){
            if (jobs_tab[atoi(args[1])] != 0) {
                curre = atoi(args[1]);
                waitpid(jobs_tab[atoi(args[1])],NULL,0);
            }else{
                printf("Invalid job reference (this job has not been started yet or is already finished)\n");
            }

        }
        else{
            printf("Reference does not match any job \n");
        }
    }
return;

}

if(strcmp(args[0],"bg") == 0 ){
        if (jobs_tab[atoi(args[1])] != 0) {
    kill(jobs_tab[atoi(args[1])],SIGCONT);
    }

    return;
}

    int pipefd[2]; //pipe
    if(pipe(pipefd)==-1){
        perror("pipe");
            exit(1);
    }
    char *buff;
    pid_t   pid;
    pid = fork();
    switch (pid){
    case -1:
        if(pid != 0){
            perror("Error pid");
        }
    case 0: //fils
            close(pipefd[0]);
            buff = "Wait";
            if (setpgid(0, 0) != 0){ // comme le pgid = 0 le pgid devient celui de pid
            perror("setpgid() error");
            }
               // si & alors buff = wait et je l'envois dans le pipe puis dans le père qui vas le lire
            //printf("liste argument %s\n" , listeArguments[position -1]);
            if (strcmp(listeArguments[position -1],"&") == 0){
                buff = "CONT";
                listeArguments[position -1] = NULL;
            }
            write(pipefd[1],buff,4);
            close(pipefd[1]);
            if (strcmp(args[0],"jobs") == 0){
               display_job(args);
                exit(EXIT_SUCCESS);

            }    

        if (execvp(args[0],listeArguments) == -1){
            perror("Error exec");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
        break;

    default: //papa
        close(pipefd[1]);
        buff = malloc(4* sizeof(char));
        //printf("PAPA pid:%d \n", (int)getpid());
        //printf("pid du fils = %d  ",pid);
        //setpgid(pid, pid);// <- pas bon
       // signal(SIGTTOU, SIG_IGN); <- pas sûr :/
        if (tcsetpgrp(STDOUT_FILENO, getpid()) != 0) {
            perror("tcsetpgrp failed");
            }

        jobs_tab[curre] = pid; //si je met getpgid(pid) récupère le pid du papa mais peut- etre du groupe // non en faite ça marche grace à setpgid
        read(pipefd[0],buff,4);
        close(pipefd[0]);
        if (strcmp(buff, "Wait") ==0 )
        {
            waitpid(pid, &status,0);
            if (WIFSIGNALED(status)){
                printf("Error\n");
            }
            else if (WEXITSTATUS(status)){
                printf("Exited Normally\n");
            }
            //jobs_tab[curre] = 0;

        }
        curre ++;

        break;
    }

}



/// je sais que le fonctionnement des pipe n'est pas le bon (il s'executent comme des commandes simultaner) mais je me suis concentrer sur les jobs par manque de temps ,sorry :(



int isIn(int *tableau[MAX_JOBS] , int gpid){


    for (int i = 0; i < MAX_JOBS; i++)
    {
        if (tableau[i] != NULL)
        {
            if (tableau[i][0] == gpid)
            {

                return i;
            }
        }

    }

    return -1;

}



char * itoa (int n){
    char *str = malloc(8 * sizeof(char) );
    memset(str,0,8);
    for(int i=0; i<7; i++){
        str[(6-i)] = n%10 + '0';
        n = n/10;
    }
    return str;
}


void display_job(char * args[]){

    if (args[1] != NULL){
        if (strcmp(args[1],"-l") == 0 ){ // lorsque l'argument est bien -l
            int max = getpid();
            int min = getppid();
            int *tableau[MAX_JOBS];
            int tailles[MAX_JOBS];
            int n;

            for (int i = 0; i < MAX_JOBS; i++)
            {
                tableau[i] = NULL;
            }
            for (int i = 0; i < MAX_JOBS; i++)
            {
                tailles[i] = 0;
            }
            for (int i = min; i < max; i++) // je vérifie tous les processus dont le pid est entre celui du père et celui courant (tous les pid produit après le lancement du programme)
            {

                if (kill(i,0) == 0) // si le processus existe 
                {
                    int groupid = getpgid(i);// je récupère le pid du groupe 

                    if ((n = isIn(tableau,groupid) ) == -1){ // je vérifie si ce gpid est dans mon tableau de gpid
                        int b = 0; // me sert de boolean
                        for (int w = 0; w < MAX_JOBS  ; w++)
                        {
                            if (jobs_tab[w] == groupid ) // on vérifie que ce processus est bien le notre
                            {
                                b = 1;
                            }
                        }
                        if (b == 0) // si le process n'est pas a nous on l'affiche pas sinon je me retrouve avec tout les gpid depuis le lancement du shell
                        {
                            continue;
                        }

                        for (int j = 0; j < MAX_JOBS; j++){// si il n'y est pas je le rajoute
                            if (tableau[j] == NULL){
                                tableau[j] =  malloc(2 * sizeof(groupid));
                                tableau[j][0] = groupid;// et je lie le processus au gpid
                                tableau[j][1] = i;
                                tailles[j] = 2;
                                break;
                            }

                        }

                    }else{
                        tableau[n] = realloc( tableau[n], (tailles[n] + 1 ) * sizeof(int) );
                        tableau[n][tailles[n]] = i;
                        tailles[n]++;
                    }

                }

            }
            //puis on affiche

            for (int i = 0; i < MAX_JOBS; i++)
            {
                if(tableau[i] != NULL){
                    printf(" pgid %d :\n" ,tableau[i][0]);
                    for (int z = 1; z < tailles[i]; z++)
                    {
                        if (tableau[i][z] != 0 )
                        {
                            int currpid = tableau[i][z];

                              /*for (int t = 0; t < 40; t++) //45 car les signaux se trouvent entre la  ligne 40 et 44 dans le /proc/pid/status  
                            MANQUE de temps mais l'idée était d'aller chercher les signaux ignoré dans /proc/pid/statut à l´ aide de strcmp
                            c'est un peu bourrin mais je pense que ça aurait pus fonctionner 
                                char * machaine = "/proc/";
                                strncat(machaine,itoa(currpid), sizeof(currpid));
                                strncat(machaine, "status", sizeof("status"));
                                //fgets();
                            } */
                            
                            printf("\t\t%d\n",currpid);
                        }


                    }
                    
                } 
            }
            


        }else{
            printf("jobs ne prend pas d'autre argument que -l :(\n");
        }

    }else{
        for (int j = 0; j < MAX_JOBS; j++){
            if (jobs_tab[j] != 0){
                printf( "[%d] commande: %s \n",j,mes_commande[j]);
            }
        }
    }
    

    }





