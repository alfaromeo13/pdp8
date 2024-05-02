#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//----------------------------- Simulator PDP-8 racunara ----------------------------------
    //sve registre realizujemo pomocu nizova. Kontrolna jedinica:
    int RAM[4096][16];//RAM je matrica(1 red je 1 naredba sirine 16 bita)
    int PC[12];
    int I,OPR[3],MAR[12];//naredba
    int MBR[16];
    int AC[16];
    int E;
    int SC[2];
    int S,F=0,R=0;
    int brTakta=0;
    int start=-1;//dio naredbi za izvrsavanje u RAM-u
    int t[]={0,0,0,0};//taktovi
    //ALU ne realizujemo jer aritmeticke i logicke operacije radi c
    int cilI = 1;//flag

    void dekoder2x4(int *x,int e,int *y){//x je SC
        y[0]=!x[0] && !x[1] && e;
        y[1]=!x[0] &&  x[1] && e;
        y[2]= x[0] && !x[1] && e;
        y[3]= x[0] &&  x[1] && e;
    }

    void absoluteLoader(FILE *f){//loader, f-ja ucitava program u memoriju
    //postavljamo prve bite svih redova na -1 da kasnije skipali taj red ako je neiskoristen
        for(int i=0;i<4096;i++)
             RAM[i][0]=-1;
        int x=0;//broj ponavljanja 7777 u fajlu
        printf("\n\t----prikaz RAM-a----\n");
        while(S){//ako je racunar upaljen ucitaj u RAM
        char c[50];
        fgets(c, 50 , f);//ucitaj naredbu u niz c
        if(feof(f)) break;//ako kraj fajla prekini
        char *adr1 = strtok(c, " ");//red u RAMU gdje smjestamo tu naredbu
        int adresa1=atoi(adr1);
        if(adresa1==7777) {//ako jeste 7777
            x++;
            continue;//preskoci ostattak koda
        }

        if(start==-1)
        start=adresa1;

        int m=0;//bit indirekcije
        char *komanda = strtok(NULL, " ");//masinska komanda LDA,ADD,...
        int kom_bin[3]={0};//binarna reprezentacija komande
        binaryInstruction(komanda,kom_bin,&m);

        char *adr2=strtok(NULL, " \n");
        int adresa2=atoi(adr2);
        int adresa_bin[12];

        if(x==2){//ako je x==2 deklarisemo podatke
        int adresa_bin[16];
        dekadniUbin(adresa_bin,adresa2,16);
        printf("\t%d ",adresa1);/////
        for(int i=0;i<16;i++) {////
        RAM[adresa1][i]=adresa_bin[i];
        printf("%d",RAM[adresa1][i]);
        }////
        printf("\n");////
        continue;
        }

        if(kom_bin[0] && kom_bin[1] && kom_bin[2])//ako je komanda 111 onda nema veze sa memorijom
            shiftRegister(komanda,adresa_bin);
        else//u suprotonom nam treba druga adresa
            dekadniUbin(adresa_bin,atoi(adr2),12);
        //ucitavamo u RAM komandu
        RAM[adresa1][0]=m;
        RAM[adresa1][1]=kom_bin[0];
        RAM[adresa1][2]=kom_bin[1];
        RAM[adresa1][3]=kom_bin[2];
        for(int i=4;i<16;i++)
        RAM[adresa1][i]=adresa_bin[i-4];

        printf("\t%d ",adresa1);/////////
        for(int i=0;i<16;i++) /////////
        printf("%d",RAM[adresa1][i]); ////////
        printf("\n"); /////////
      }
      printf("\t---------------------\n");
      fclose(f);//zatvori fajl kad ga ucitamo u RAM
    }

    void dekadniUbin(int *niz,int broj,int brBita){//pretvara dek u bin broj
        int var;
        for(int i=brBita-1;i>=0;i--){
            var=broj>>i;
            niz[brBita-i-1]=(var & 1)? 1:0;
        }
    }

    void binaryInstruction(char *naredba,int *niz,int *m){//pretvara naredbu u bin zapis
         //uslov za komande sa indirekcijom kao *ADD
         if(naredba[0]=='*') *m=1,naredba++; //ovim pomjeramo pointer na 2-gi element niza i brisemo prvi
         if(!strcmp(naredba,"AND"))//strcmp vraca 0 ako su argumenti ==
         dekadniUbin(niz,0,3);
         else if(!strcmp(naredba,"ADD"))
         dekadniUbin(niz,1,3);
         else if(!strcmp(naredba,"LDA"))
         dekadniUbin(niz,2,3);
         else if(!strcmp(naredba,"STA"))
         dekadniUbin(niz,3,3);
         else if(!strcmp(naredba,"BUN"))
         dekadniUbin(niz,4,3);
         else if(!strcmp(naredba,"BSA"))
         dekadniUbin(niz,5,3);
         else if(!strcmp(naredba,"ISZ"))
         dekadniUbin(niz,6,3);
         else dekadniUbin(niz,7,3);//u suprotonom su naredbe za registre
    }

    void shiftRegister(char *komanda,int *adresa_bin){//pretvara adresu reg.instrukcija u bin zapis
        for(int i=0;i<12;i++) adresa_bin[i]=0;//praznimo niz zbog registarskih naredbi
        if(!strcmp(komanda,"CLA"))
            adresa_bin[0]=1;
        else if(!strcmp(komanda,"CLE"))
            adresa_bin[1]=1;
        else if(!strcmp(komanda,"CMA"))
            adresa_bin[2]=1;
        else if(!strcmp(komanda,"CME"))
            adresa_bin[3]=1;
        else if(!strcmp(komanda,"CIR"))
            adresa_bin[4]=1;
        else if(!strcmp(komanda,"CIL"))
            adresa_bin[5]=1;
        else if(!strcmp(komanda,"INC"))
            adresa_bin[6]=1;
        else if(!strcmp(komanda,"SPA"))
            adresa_bin[7]=1;
        else if(!strcmp(komanda,"SNA"))
            adresa_bin[8]=1;
        else if(!strcmp(komanda,"SZA"))
            adresa_bin[9]=1;
        else if(!strcmp(komanda,"SZE"))
            adresa_bin[10]=1;
        else if(!strcmp(komanda,"HLT"))
            adresa_bin[11]=1;
    }

    int binToDek(int *broj,int n){//broj je niz
        int rez=0;
        int pom_niz[16];
        if(broj[0]==1){//ako je 1 onda je negativan
        int pom_niz[16];
        for(int i=0;i<16;i++){
        if(broj[i]==1){pom_niz[i]=0;}else{pom_niz[i]=1;}
        }
        return -(binToDek(pom_niz,16)+1);//rekurzivno pozivamo 1
        }
        for(int i=0;i<n;i++)
            if(broj[n-i-1]) rez+=pow(2,i);
        return rez;
    }

    void fetch_cycle(){
        if(t[0]){//takt 1
        dekadniUbin(PC,start,12);//punimo PC sa adresom naredbe za izvrsavanje
        printf("\n\tC0 t0 : MAR <- PC\n");
        for(int i=0;i<12;i++) MAR[i]=PC[i];
        return;
        }

        if(t[1]){//takt 2
        int adresa=binToDek(MAR,12);//adresa je MAR
        printf("\tC0 t1 : MBR <- M[%d] , PC++\n",adresa);
        for(int i=0;i<16;i++)
        MBR[i]=RAM[adresa][i];
        start++;
        return;
        }

        if(t[2]){//treci takt
        printf("\tC0 t2 : I <- MBR(1) , OPR <- MBR(2-4)\n");
        I=MBR[0];
        OPR[0]=MBR[1];
        OPR[1]=MBR[2];
        OPR[2]=MBR[3];
        return;
        }

        if(t[3]){//cetvrti takt
        /*dekoder se ponasa kao if uslov,odnosno 3 bita na ulazu
         odredjuju koja nam je komanda potrebna za obradu pa ne moramo
         generisati 8 bita na izlazu za svaki signal vec mozemo gledati samo prva 3 bita*/
        if(OPR[0] && OPR[1] && OPR[2]){
         F=1, R=0;//udji u execute
         printf("\tC0 t3 : OPR = 111 , F <- 1\n");
        }else if(I){
         F=0,R=1;//udji u indirektni
         printf("\tC0 t3 : OPR != 111 && I = 1 , R <- 1\n");
        }else{
         F=1, R=0;//udji u execute
         printf("\tC0 t3 : OPR != 111 && I != 1 , F <- 1\n");
        }
        brTakta=-1;
      }
        printf("\n");
    }

    void indirect_cycle(){
        if(t[0]){//prvi takt
        printf("\tC1 t0 : MAR <- MBR(5-16)\n");
        for(int i=0;i<12;i++)
            MAR[i]=MBR[i+4];
        return;
        }

        if(t[1]){//drugi takt
        int adresa=binToDek(MAR,12);//adresa je zapravo MAR
        printf("\tC1 t1 : MBR <- M[%d]\n",adresa);
        for(int i=0;i<16;i++)
            MBR[i]=RAM[adresa][i];
            return;
        }

        if(t[2]){//treci takt
        printf("\tC1 t2 : \n");
        return;
        }

        if(t[3]){//cetvrti takt
        printf("\tC1 t3 : F <- 1 R <- 0\n");
        F=1,R=0;
        brTakta=-1;
        printf("\n");
        }
    }

    void execute_cycle(){
        int adresa=binToDek(PC,12);//adresa naredbe je u PC-u
        int novaAdresa[12];
        for(int i=0;i<12;i++)
        novaAdresa[i]=MBR[i+4];
        int adresa2=binToDek(novaAdresa,12);

        if(RAM[adresa][0]){//ako je prvi bit 1
            int adr[16];
            for(int i=0;i<16;i++)
            adr[i]=RAM[adresa2][i];
            adresa2 = binToDek(adr,16);
        }

        if(t[0]){//prvi takt
            printf("\tC2 t0 : MAR <- MBR(5-16)\n");
            for(int i=0;i<12;i++)
                MAR[i]=MBR[i+4];
                return;
        }

        if(t[1]){//drugi takt
         printf("\tC2 t1 : MBR <- M[%d]\n",adresa);
         for(int i=0;i<16;i++)
         MBR[i]=RAM[adresa][i];
         return;
        }

        if(t[2]){//treci takt
        if(!(OPR[0] && OPR[1] && OPR[2])){//komande za Memoriju ako nije 111
        if(!OPR[0] && !OPR[1] && !OPR[2]){//AND
        printf("\tC2 t2 : AC <- AC ^ M[%d]\n",adresa);
        //adresa 2 nam treba jer je to adresa sa kojom radimo
        for(int i=0;i<16;i++)
        AC[i]=AC[i] & RAM[adresa2][i];
        }else if(!OPR[0] && !OPR[1] && OPR[2]){//ADD
                 printf("\tC2 t2 : EAC < AC + M[%d]\n",adresa2);
                 int prenos=0;
                 for(int i=15;i>-1;i--){
                 if(AC[i] & RAM[adresa2][i]){//ako su oba 1
                 if(prenos)
                  AC[i]=1;
                    else{
                        AC[i]=0;
                        prenos=1;
                   }
                 }
                 else if(AC[i] | RAM[adresa2][i]){
                 if(prenos){//ako je 1
                  AC[i]=0;
                  prenos=1;
                 }else
                    AC[i]=1;
                 }
                 else{
                    AC[i]=prenos;
                    prenos=0;
                 }
                }
                 E=prenos;
        }else if(!OPR[0] && OPR[1] && !OPR[2]){//LDA
                 printf("\tC2 t2 : AC <- M[%d]\n",adresa2);
                 for(int i=0;i<16;i++)
                 AC[i]=RAM[adresa2][i];
        }else if(!OPR[0] && OPR[1] && OPR[2]){//STA
                 printf("\tC2 t2 : M[%d] <- AC\n",adresa2);
                 for(int i=0;i<16;i++)
                 RAM[adresa2][i]=AC[i];
        }else if(OPR[0] && !OPR[1] && !OPR[2]){//BUN
                 printf("\tC2 t2 : PC <- M[%d]\n",adresa2);
                 for(int i=0;i<12;i++)
                 PC[i]=RAM[adresa2][i+4];
                 start = adresa2;
        }else if(OPR[0] && !OPR[1] && OPR[2]){//BSA
                 int br=binToDek(PC,12)+1;
                 dekadniUbin(PC,br,12);
                 start=adresa2+1;
                 for(int i=0;i<12;i++)
                 RAM[adresa2][i+4]=PC[i];
                 printf("\tC2 t2 : M[%d] <- PC , PC <- m + 1\n",adresa2);//m je adresa 2
                 dekadniUbin(PC,start,12);
        }else{//ISZ
         int pomNiz[16];
         for(int i=0;i<16;i++)
         pomNiz[i]=RAM[adresa][i];
         int noviBroj=binToDek(pomNiz,16)+1;//povecamo za 1
         dekadniUbin(pomNiz,noviBroj,16);//koristimo opet pomocni niz da ne bi pravili novi
         int broj = 0;
         for(int i=0;i<16;i++){
            RAM[adresa][i]=pomNiz[i];
            broj|=pomNiz[i];//broj = broj | pomNiz[i];
         }
         printf("\tC2 t2: M[%d]++ ,",adresa);
         if(!broj){//ako je 1
            printf("M[%d] = 0 , PC++ \n",adresa);
            dekadniUbin(PC,++start,12);
         }
         }
        }else{//komande za registre
            int komanda=0;//redom koja je komanda
            for(int i=0;i<12;i++){
            if(novaAdresa[i]){//ako je bit == 1
                komanda=i;
                break;
             }
            }

             if(komanda == 0){//CLA
            printf("\tC2 t2 : CLA , AC <- 0 \n");
            for(int i=0;i<16;i++)
                AC[i]=0;
            }

             else if(komanda == 1){//CLE
             printf("\tC2 t2 : CLE , E <- 0 \n");
              E=0;
             }
             else if(komanda == 2){//CMA
              printf("\tC2 t2 : CMA, AC <- ~(AC) \n");
              for(int i=0;i<16;i++)
              AC[i] = !AC[i] ? 1 : 0;
              int br=binToDek(AC,16)+1;
              dekadniUbin(AC,br,16);
              }

             else if(komanda == 3){//CME
             printf("\tC2 t2 : CME, E <- ~(E) \n");
             E =! E ? 1 : 0;
             }

             else if(komanda == 4 || komanda == 5){//CIR i CIL
                int EAC[17];
                EAC[0]=E;
             for(int i=0;i<16;i++)
               EAC[1+i]=AC[i];
               int broj=binToDek(EAC,17);
               if(komanda==5){
               printf("\tC2 t2 : CIL, cilEAC cilI %d \n", cilI);
               cilI++;
               broj *= 2;
               }else{
               printf("\tC2 t2 : CIR, cirEAC \n");
               broj>>= 1;
               }

             dekadniUbin(EAC,broj,17);//iskoristicemo EAC da ne bi pravili novi niz
             //sad su u EAC binarni brojevi nakon shiftovanja

             E=EAC[0];
             for(int i=0;i<16;i++)
             AC[i]=EAC[1+i];
             }

            else if(komanda == 6){//INC
            printf("\tC2 t2 : INC, EAC <- AC++\n");
            int broj=binToDek(AC,16)+1;
            dekadniUbin(AC,broj,16);
            if(AC[0]){
             AC[0]=0;
             E=1;
             }
            }
            else if(komanda==7){//SPA
                if(!AC[0]){//ako je 0
                printf("\tC2 t2 : AC(1) = 0 , PC++\n");
                dekadniUbin(PC,++start,12);
              }
            }
            else if(komanda==8){//SNA
                if(AC[0]){//ako je 1
                printf("\tC2 t2 : AC(1) = 1 , PC++\n");
                dekadniUbin(PC,++start,12);
              }else printf("\tC2 t2 : AC(1) != 1\n");
            }
            else if(komanda==9){//SZA
            int rez=0;
            for(int i=0;i<16;i++)
                rez|= AC[i];//ako ima barem 1 jedinica onda sigurno rez=1;
                if(!rez){
                  printf("\tC2 t2 : AC = 0 , PC++\n");
                  dekadniUbin(PC,++start,12);
                }
            }else if(komanda==10){//SZE
                if(!E){
                printf("\tC2 t2 : E = 0 , PC++\n");
                dekadniUbin(PC,++start,12);
                }
            }else{//HLT
             printf("\tC2 t2 : HLT, S <- 0 \n");
             S=0;
             }
            }
          }
        if(t[3]){
        F=0,R=0;
        brTakta=-1;
        printf("\tC2 t3 : F <- 0\n");
        }
    }

    void generatorTaktova(){
         while(S){//radi dok S=1

         if(start<4096 && RAM[start][0]==-1){
            start++;
            continue;
         }

         dekadniUbin(SC,brTakta,2);
         dekoder2x4(SC,1,t);
         // 00-fetch  01-indirektni 10-execute
         if(!F && !R){
         fetch_cycle();
         brTakta++;
         }

         else if(!F &&  R){
         indirect_cycle();
         brTakta++;
         }

         else if (F && !R){
         execute_cycle();
         brTakta++;
        }
      }
    }

    int main(){
        S=1;//ovim smo dozvolili pokretanje racunar S=1 a S=0 gasi racunar
        FILE *f=fopen("Program.txt","r");//rad sa sekvencijalnim datotekama
        absoluteLoader(f);//ucitavamo izabrani program u RAM (u ovom slucaju Program.txt)
        generatorTaktova();
        printf("\n\tStanje u akumulatoru je : %d ",binToDek(AC,16));
        printf("\n\t\tProgram žavršen!");
        return 0;
    }
