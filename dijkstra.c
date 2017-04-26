/* Dijkstra's Algorithm in C */
#include<stdio.h>
#include<string.h>
#include<math.h>
#define IN 99
#define N 6

int dijsktra(int cost[][N],int source,int target)
{
    int dist[N],prev[N],selected[N]={0},i,m,min,start,d,j;
    char path[N];
    for(i=0;i < N;i++)
    {
        dist[i] = IN;
        prev[i] = -1;
    }
    start = source;
    selected[start]=1;
    dist[start] = 0;
    while(selected[target] ==0)
    {
        min = IN;
        m = 0;
        for(i=0;i< N;i++)
        {
            d = dist[start] +cost[start][i];
            if(d< dist[i]&&selected[i]==0)
            {
                dist[i] = d;
                prev[i] = start;
            }
            if(min>dist[i] && selected[i]==0)
            {
                min = dist[i];
                m = i;
            }
        }
        start = m;
        selected[start] = 1;
    }
    start = target;
    j = 0;
    while(start != -1)
    {
        path[j++] = start+65;
        start = prev[start];
    }
    path[j]='\0';
    //strrev(path);
    printf("%s", path);
    return dist[target];
}

int main()
{
    int i,j,w,ch,co;
    int source, target,x,y;
    printf("\t The Shortest Path Algorithm ( DIJKSTRA'S ALGORITHM in C \n\n");
/*    for(i=0;i< N;i++)
    for(j=0;j< N;j++)
    cost[i][j] = IN;

    for(x=0;x<N;x++)
    {
        for(y=x+1;y< N;y++)
        {
            printf("Enter the weight of the path between nodes %d and %d: ",x+1,y+1);
            scanf("%d",&w);
          //  printf("%d %d\n",x,y);
            cost[x][y] = w;
          //  printf("%d %d\n",x,y);
            cost[y][x] = w;
          //  printf("%d %d\n",x,y);
        }

        for(int i = 0; i < N; i++) {
          for(int j = 0; j < N; j++) {
          printf("%d ", cost[i][j]);
          }
        printf("\n");
        }

        printf("\n");
    }
  */
int cost[N][N] = {
    {99, 4, 2, 99, 99, 99} ,
    {4, 99, 1, 5, 99, 99} ,
    {2, 1, 99, 8, 10, 99} ,
    {99, 5, 8, 99, 2, 6} ,
    {99, 99, 10, 2, 99, 3} ,
    {99, 99, 99, 6, 3, 99}
  };
    printf("\nEnter the source: ");
    scanf("%d", &source);
    printf("\nEnter the target: ");
    scanf("%d", &target);
    co = dijsktra(cost,source-1,target-1);
    printf("\nThe Shortest Path: %d \n",co);
}
