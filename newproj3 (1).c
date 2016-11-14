/*citdggdation - the alphabetical sorting ofterminal array in this program has been taken from this website - "http://www.sanfoundry.com/c-program-sort-names-alphabetical-order/"*/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct linkedlist1
{
char data[25];
struct linkedlist1 *next;
};

struct hash
{
struct linkedlist1 *head;
int count;

};


struct hash *table;

char term[25][25],nonterm[25][25];
int first[25][50];
int follow[25][150],change=1,marked=1;
int n=1,k=0,l=0,countterm[25];

void check();
void printfollow();
void changefollow(int,int,int);
void firstmatrix()
{
        int j,i,u,v=0,w=1;
        struct linkedlist1 *node;
        struct linkedlist1 *temp;
     for(i=1;i<=n;i++)
                {
                        temp=(struct linkedlist1*)malloc(sizeof(struct linkedlist1));
                        node=(struct linkedlist1*)malloc(sizeof(struct linkedlist1));
                        temp=table[i].head;
                        node=table[i].head;
                        for(v=0;v<k;v++)
                        if(strcmp(node->data,nonterm[v])==0)
                        {
                                label:temp=temp->next;
   				if(temp==NULL)
                                {first[v+1][0]=1;
                                }

                                else
                                {
                                for(u=0;u<l;u++)
                                if(strcmp(term[u],temp->data)==0)
                                {first[v+1][u+1]=1;
                                break;
                                }
                                for(w=0;w<k;w++)
                                if(strcmp(nonterm[w],temp->data)==0)
                                { first[v+1][l+w+1]=1;
                                break;
                                }
				 /*if(first[w+1][0]==1)
                                {temp=temp->next;goto label;}*/
                                }

                        }
                }
        /*for(i=1;i<=k;i++)
        {
        for(j=0;j<l+k+1;j++)
        {
        printf("%d ",first[i][j]);
        }
        printf("\n");
        }
        printf("\n");*/
}
void epsilonfirst(int i,int jl)
{
int t,p,q,rightrow=0,wrongrow=0,u,f,o,e,noepsilon=0;
struct linkedlist1 *node1,*temp;
//printf("-----Iteration i-%d,jl- %d---------\n",i,jl);
for(t=1;t<=n;t++)
        {noepsilon=0;
        node1=(struct linkedlist1*)malloc(sizeof(struct linkedlist1));
        node1=table[t].head;

        if(strcmp(node1->data,nonterm[i-1])==0)
                {
                temp=(struct linkedlist1*)malloc(sizeof(struct linkedlist1));
                temp=node1->next;
                       /* for(u=0;u<=marked;u++)
 *                         {
 *                                                 temp=temp->next; 
                                                                         }*/
			/*while(temp!=NULL)
                        if(strcmp(temp->data,nonterm[jl-1])!=0)
                        {
                        //correct row
                        //                        //printf("%s",nonterm[i-1],nonterm[jl
                        temp=temp->next;}*/
			if(temp!=NULL)
			for(o=0;o<k;o++)
			if(strcmp(nonterm[o],temp->data)==0)
			{
			if(first[o+1][0]!=1)
			{noepsilon=1;break;}
			else
			goto label4;
			}
			if(noepsilon==1)
			break;
			label4:if(temp!=NULL)
			{for(e=0;e<l;e++)
			if(strcmp(term[e],temp->data)==0)
			goto label3;
			temp=temp->next;}
			else
			break;
                       label3:if(temp!=NULL)
                                {
                                for(p=0;p<l;p++)
                                if(strcmp(temp->data,term[p])==0)
                                    {
                                        if(first[i][p+1]!=1)
                                        {
                                            first[i][p+1]=1; change=1;
                                        }
                                        else
                                        change=0;
                                        break;
                                    }
                                /*if(p<l)
 *                                 continue;*/
                                for(q=0;q<k;q++)
                                if(strcmp(temp->data,nonterm[q])==0)
                                    {
                                       /* if(first[i][l+q+1]!=1)
                                        {       change=1;
						
                                            first[i][l+q+1]=1; //printf("%s - nonterm after 3",nonterm[q]);
                                        }*/
					for(f=1;f<l+k+1;f++)
            				if(first[q+1][f]==1)
					{
                				if(first[i][f]!=1)
                				{
                				first[i][f]=first[q+1][f];

                				change=1;
                				}

					}	
					if(first[q+1][0]==1)
					{temp=temp->next;goto label3;}
					
                                        break;
                                    }
                                /*if(q<k)
 *                                 continue;  */
                                }
                           else
                                {       if(first[i][0]!=1)
                                {
                                        first[i][0]=1;
                                        break;change=1;
                                }
                                        else
                                        change=0;
                                }
                        }
                }
        }

void computefirst()
{
    int i,j,f;
while(change==1)
{
change=0;
    for(i=1;i<=k;i++)
    {
        for(j=l+1;j<l+k+1;j++)
        {if(first[i][j]==1 )
 
        {   //    first[i][j]=0;
            for(f=1;f<l+k+1;f++)
            if(first[j-l][f]==1)

            {	/*if(i==j-l)
		change=1;*/
                if(first[i][f]!=1)
                {
                first[i][f]=first[j-l][f];
                
                change=1;
                }


            }
            if(first[j-l][0]==1)
 	    {	   
                epsilonfirst(i,j-l);
            
            }

        }
        }
//printf("\n");
}}
//check();
}
void check()
{
        int i,j;
        for(i=1;i<=k;i++)
        for(j=l+1;j<l+k+1;j++)
                if(first[i][j]==1)
                        computefirst();
}
int checkNT()
{
        int i,j;
        for(i=1;i<=k;i++)
        for(j=l+1;j<=l+k+k;j++)
                if(follow[i][j]==1)
                        return 1;
        return 0;
}
/*void computefollow()
{
        int i,j,p,q;
        for(i=1;i<=k;i++)
        for(j=l+1;j<=l+k+k;j++)
                if(follow[i][j]==1)*/
void computefollow()
{	int i,j,p,q;
        for(i=1;i<=k;i++)
        for(j=l+1;j<=l+k+k;j++)
                if(follow[i][j]==1)
                {      
                        if(j>=l+1 && j<=l+k)
                        {       
                                for(p=1;p<=l;p++)
                                        if(first[j-l][p]==1)
                                                {
                                                follow[i][p]=first[j-l][p];
                                                }

                        }
                        else if(j>=l+k+1 && j<=l+k+k)
                        {
                                
                                for(q=0;q<=l+k+k+1;q++)
                                        if(follow[j-l-k][q]==1)
                                                {
                                                follow[i][q]=follow[j-l-k][q];}
                        }
                        follow[i][j]=0;
                }
        
        if(checkNT()==1)
                computefollow();
}
void changefollow(int i,int mark,int j)
{
        int m,p,termcheck;
        struct linkedlist1 *node, *node1;
        node=(struct linkedlist1*) malloc(sizeof(struct linkedlist1));
        node=table[i].head;
        for(m=0;m<mark;m++)
        node=node->next;
        while(node!=NULL)
        {termcheck=0;
        node=node->next;
        if(node==NULL)
        {       node1=(struct linkedlist1*) malloc(sizeof(struct linkedlist1));
                node1=table[i].head;
                for(p=0;p<k;p++)
                        if(strcmp(node1->data,nonterm[p])==0)
                                {follow[j+1][p+l+k+1]=1; break;}
                                
        }
        else
        {
        for(m=0;m<l;m++)
        if(strcmp(node->data,term[m])==0)
        {
                follow[j+1][m+1]=1;
               termcheck=1;
                break;
        }
        if(termcheck==1)
        break;
        for(m=0;m<k;m++)
        if(strcmp(node->data,nonterm[m])==0)
        {
                follow[j+1][l+m+1]=1;
                
                break;
        }
       
        if(first[m+1][0]==1)
        continue;
        else
        break;
        }}

}
void followmatrix()
{      
        int i,mark=1,j;
        struct linkedlist1 *node;
        for(i=1;i<=n;i++)
        {
                node=table[i].head;
                node=node->next;
                while(node!=NULL)
                {
                        for(j=0;j<k;j++)
                        if(strcmp(node->data,nonterm[j])==0)
                        {
                                changefollow(i,mark,j);
                        }
                node=node->next;
                mark++;
                }
                mark=1;
        }
//printfollow();
}
void printfollow()
{
        int i,j;
printf("k=%d, l+k+k+1 =%d\n",k,l+k+k+1);
//printf("---------------------------------------------------------------------------------------------------\n");
 for(i=1;i<=k;i++)
        {for(j=0;j<l+k+k+1;j++)
        printf("%d",follow[i][j]);
	}
        printf("\n");
        
//printf("---------------------------------------------------------------------------------------------------\n");

}
int main(int argc, char* argv[])
{
        char c,input[26],cfirst[25][25],duplicate[25];
        int task,tempnum,checkterminal[25],e=0,row[25],start=1;
        int i=0, j=0,count=0,z,det=0,check=0,w,p=0,m;
        struct linkedlist1 *node=NULL;
        struct linkedlist1 *temp;
        struct linkedlist1 *temp2;
        
        follow[1][0]=1;
        if (argc < 2)
        {
        printf("Error: missing argument\n");
        return 1;
        }
        task = atoi(argv[1]);
        while(1)
        {       c=getchar();
                if(c=='#')
                {
                        c=getchar();
                        if(c=='#')
                                break;
                        else if(count==0)
                        {

                                temp2=node;
                                while(temp2!=NULL)
                                {       strcpy(nonterm[k],temp2->data);
                                        temp2=temp2->next;
                                        n++;k++;
                                }
                                table = (struct hash *) calloc(n+1,sizeof(struct hash));
                                table[j].head=(struct linkedlist1*)malloc(sizeof(struct linkedlist1));
                                table[j].head=node;

                                node=NULL;
                                j++;
                                n--;
                                count++;

                        }
                        else
                        {
                                if(j>n)
                                {table = (struct hash*) realloc(table,j*sizeof(struct hash));
                                n=j;}
                                else if(j==n)
                                {
                                table = (struct hash*) realloc(table,(n+1)*sizeof(struct hash));
                                }
 table[j].head=(struct linkedlist1*)malloc(sizeof(struct linkedlist1));
                                table[j].head=node;

                                node=NULL;
                                j++;
                        }
                }
                else
                if(isspace(c))
                {
                        input[i]='\0';
                        if(strcmp(input,"")!=0)
                        {
                        temp=(struct linkedlist1*)malloc(sizeof(struct linkedlist1));
                        strcpy(temp->data,input);
                        temp->next=NULL;

                        if(node==NULL)
                                node=temp;

                        else
                        {
			if(start!=1)
                        {temp2=(struct linkedlist1*)malloc(sizeof(struct linkedlist1));
                                temp2=node;

                                while(temp2->next!=NULL)
                                temp2=temp2->next;

                                temp2->next=temp;}
			else
			return -1;
                        }
                        i=0;
			
                }
		
		}

                else if (c=='-' || c=='>')
                {
                continue;
                }
                /*      if(c==EOF)
 *                      return -1;*/
                else
                {	start++;
                        input[i]=c;
                        i++;
                }

        }
        for(i=0;i<=n;i++)
        {       table[i].count=0;

                temp=(struct linkedlist1*)malloc(sizeof(struct linkedlist1));
                temp=table[i].head;
                while(temp!=NULL)
                {
 for(z=0;z<k;z++)
                        {
                        if(strcmp(nonterm[z],temp->data)==0)
                        {
                        det++;
                        break;
                        }
                        }
                        if(det==0)
                        {
                        for(w=0;w<l;w++)
                        if(strcmp(term[w],temp->data)==0)
                        {       check=1;
                                if(row[w]!=i)
                                {row[w]=i; countterm[w]++;}


                                break;
                        }
                        if(check==0)
                        {

                                row[l]=i;
                                countterm[w]++;
                                strcpy(term[l++],temp->data);

                        }
                        if(check==1)
                        check=0;
                        }
                        det=0;
                        temp=temp->next;
                table[i].count++;
}}
       for(i=0;i<l-1;i++)
        {
        for (j=i+1;j<l;j++)
        {
            if (strcmp(term[i],term[j]) > 0)
            {
                strcpy(duplicate,term[i]);
                tempnum=countterm[i];
                strcpy(term[i],term[j]);
                countterm[i]=countterm[j];
                strcpy(term[j],duplicate);
                countterm[j]=tempnum;
            }
        }
    }
        /*firstmatrix();
 computefirst();
        //printfollow();
        followmatrix();
        computefollow();
	*/
        switch(task){
        case 0:
        for(i=0;i<k;i++)
        {
                printf("%s ",nonterm[i]);


        }
        printf("\n");
        for(i=0;i<l;i++)
        {
                printf("%s: %d\n",term[i],countterm[i]);
        }
        break;
        case 1:
	firstmatrix();
	 computefirst();

        for(i=1;i<=k;i++)
        {
	       
        printf("FIRST(%s) = {",nonterm[i-1]);
        for(j=0;j<l+1;j++)
        {
                if(first[i][j]==1)
                {
                        if(j==0)
                        {strcpy(cfirst[p],"#\0");}
                        else
                        {strcpy(cfirst[p],term[j-1]);}

                        p++;
                }
        }
        for(m=0;m<p;m++)
        {
                printf(" %s",cfirst[m]);
                if(m!=p-1)
                printf(",");
        }
         printf(" }\n");
         p=0;
        }
        break;
        case 2:
	firstmatrix();
	computefirst();
        //printfollow();
        followmatrix();
        computefollow();
        
        for(i=1;i<=k;i++)
        {
	/*firstmatrix();
 computefirst();
        //printfollow();
                followmatrix();
                        computefollow();
        */
        printf("FOLLOW(%s) = {",nonterm[i-1]);
        for(j=0;j<l+1;j++)
        {
                if(follow[i][j]==1)
                {
                        if(j==0)
                        {strcpy(cfirst[p],"$\0");}
                        else
                        {strcpy(cfirst[p],term[j-1]);}

                        p++;
                }
        }
        for(m=0;m<p;m++)
        {
                printf(" %s",cfirst[m]);
                if(m!=p-1)
                printf(",");
        }
         printf(" }\n");
         p=0;
        }
        break;

        default:
        printf("Error: unrecognized task number %d\n", task);
        break;
        }
return 0;
}

     
