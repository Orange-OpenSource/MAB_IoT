#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include "type.h"
#define PI 3.14159265

/**N-N1 nodes play in the background for T-T1 slots*/
void background(){
float rssi=0;
int ex_plays=0;
armChange=0;
countingF=fopen("res_counting.txt","w");
for ( slot=0;slot<T1;slot++)
{
    if(slot%5000==0)
        {
         fprintf(countingF,"%f  ",(float)armChange/ex_plays);
         printf("plays=%d, armchange=%f\n",ex_plays,(float)armChange/ex_plays);
        }
    if(slot % (T1/TimeDisplay)==0 )
         {printf("_______________t=%d________________\n",slot);
         fprintf(timeF,"%d ",slot);
         }

     for (int n=0;n<N1;n++)
    {
        if(node[n].pr >= RAND_R(1.0))
        {
            ex_plays++;
            if(n<N1)
                    {if(backgroundN!=5) node[n].arm =selectArm(n);
                    if(backgroundN==5)
                        {if(node[n].last_sinr[19]!=-1.0 )
                            ADR(n);
                         node[n].arm=node[n].ind_sf;
                        }
                    }

            int k=node[n].arm;

            node[n].B[k].tk=node[n].B[k].tk+1;
            ex_playsPerArm[dr][k]++;

            node[n].isPlaying=1;
            node[n].nb_plays++;

            node[n].shadowing=P.shadowing*randn();
            float rn=0.0;
            while(rn==0) rn=RAND_R(1);
            node[n].rayleigh=-1.0*log(rn);
            node[n].PL=log2lin(pathLoss(n)+node[n].shadowing);
            node[n].RSSI=log2lin(node[n].tx)*node[n].rayleigh/node[n].PL;
            rssi=rssi+node[n].RSSI;
            node[n].energy=node[n].energy+energy(n);

            updateEmissionTime(n,slot);
            node[n].nb_sf[node[n].ind_sf]++;
            nb_sf[node[n].ind_sf]++;

        }
        else node[n].isPlaying=0;
    }

    /**collisions*/

    for(int n=0;n<N1;n++)
    {
        if(node[n].isPlaying)
            {updateSINR(n);
             int y=Tr_success(n);
             int k=node[n].arm;

             ///background nodes following ADR
            if(n<N1 && backgroundN==5)
                {
                    updateSINR(n);
                    if(y==1)
                        {node[n].last_sinr[node[n].indx]=node[n].SINR;
                        node[n].indx++;
                        node[n].pr=node[n].p;
                        node[n].cons_coll=0;
                        }
                    if(y==0) node[n].cons_coll++;
                    if(node[n].cons_coll==retransmit)
                        node[n].cons_coll=0;
                    if(node[n].cons_coll==3 || node[n].cons_coll==5 || node[n].cons_coll==7)
                        {node[n].ind_sf++;
                         armChange++;
                        if(node[n].ind_sf>5){node[n].ind_sf=5; armChange--;}
                        }
                }
             if(y==0)
                 node[n].cons_coll++;


             node[n].B[k].mu=(float)y/(float)node[n].B[k].tk+(float)(node[n].B[k].tk-1)/(float)node[n].B[k].tk*node[n].B[k].mu;

             float rw=getReward(n,y==0,node[n].cons_coll+1);
             node[n].sum_rewards=node[n].sum_rewards+rw;
             ex_rewardsPerArm[dr][k]=ex_rewardsPerArm[dr][k]+rw;
             node[n].B[k].avg_reward= (float)rw/(float)node[n].B[k].tk+(float)(node[n].B[k].tk-1)/(float)node[n].B[k].tk*node[n].B[k].avg_reward;

             if(y==1 || node[n].cons_coll==retransmit)
             node[n].cons_coll=0;

            }

        }

    slotEnd();
 }
fclose(countingF);
}





