#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <cstring>
#include <time.h>
#include <queue>
#include <hash_map>
#include "md5.h"
#include "kdtree.h"

#define NUM 45606//15606
#define SIZE 5//SIZE is num of set

using namespace std;
vector<int>num[NUM];
//random cut
//RAND_MAX is 32767 in my computer
int cut_set[NUM];
void rand_cut()
{	srand((int)time(NULL));
	for(int i=0;i<NUM;i++)
	{	cut_set[i]=rand()%SIZE;
	}
	/*cut_set_rand[0]={2};cut_set_rand[1]={1};cut_set_rand[2]={2};
	cut_set_rand[3]={1};cut_set_rand[4]={3};cut_set_rand[5]={3};*/
}
long check_rand_cut()
{	long sumarc=0;
	for(int i=0;i<NUM;i++)
	{	int node;
		for(int j=1;j<num[i].size();j++)
		{	node=num[i][j];
			if(cut_set[i]!=cut_set[node]) sumarc++;
		}
	}
	return sumarc/2;
}
// bfs cut the set
bool vis_node[NUM];
void bfs_cut()
{   for(int i=0;i<NUM;i++)cut_set[i]=SIZE-1;
    for(int i=0;i<NUM;i++)vis_node[i]=false;
    srand((int)time(NULL));
    for(int set_num=0;set_num<SIZE-1;set_num++)
    {   int start_node=rand()%NUM;
        while(vis_node[start_node]!=false) start_node=rand()%NUM;
        cut_set[start_node]=set_num;
        //bfs
        int size_of_set=NUM/SIZE;
        int size_num=0;
        queue<int>q;
        q.push(start_node);size_num++;
        while(!q.empty())
        {   //cout<<"bfs";
            int node;node=q.front();q.pop();
            if(node==-1)break;
            for(int i=1;i<num[node].size();i++)
            {   int vist=num[node][i];
                if(vis_node[vist]==false)
                {   cut_set[vist]=set_num;
                    vis_node[vist]=true;
                    q.push(vist);
                    size_num++;
                    if(size_num>size_of_set) break;
                }
            }
            if(size_num>size_of_set) {break;}
        }

    }
}
long check_bfs_cut()
{	long sumarc=0;
	for(int i=0;i<NUM;i++)
	{	int node;
		for(int j=1;j<num[i].size();j++)
		{	node=num[i][j];
			if(cut_set[i]!=cut_set[node]) sumarc++;
		}
	}
	return sumarc/2;
}
/*
country-expand algorithm
cut_set_cea();
*/
void cea_cut()
{   //select SIZE node,make them as source,from source to other,make the set become bigger.
    memset(cut_set,-1,sizeof(int)*NUM);
    srand((int)time(NULL));
    int source[SIZE];
    // init source node
    for(int i=0;i<SIZE;i++)
    {   int root;
        while(1)
        {   root=rand()%NUM;
            if(cut_set[root]==-1)
                break;
        }
        cut_set[root]=i;
    }
}
/*LSH use min-hash
*/
bool node_matrix[NUM][NUM];//this is input matrix
int md5hash(string a,int table)
{   unsigned char* encrypt;
    for(int i=0;i<a.size();i++) encrypt[i]=a[i];
    encrypt[a.size()]='\0';
    unsigned char decrypt[table];
    MD5_CTX md5;
	MD5Init(&md5);
	MD5Update(&md5,encrypt,a.size()+1);
	MD5Final(&md5,decrypt);
	//printf("加密前:%s\n加密后:",encrypt);
	int hashnum[table];
	for(int i=0;i<table;i++)
	{   hashnum[i]=decrypt[i];
	}
	int hashall=0;
	for(int i=0;i<table;i++)
	{   hashall=hashnum[i]+hashall;
	}
	return hashall;// hashnum;
}

// the function normal_hash return the id of bucket
int normal_hash(int num,int bucket)
{   return num%bucket;
}
bool cmp(int a,int b)
{   return (a<b);
}
void lsh_cut(int small,int time_of_change,int band,int num_bucket)
{   if(small==1) ;
    else
    {if(time_of_change<band)
    {   int ttt=1;ttt=ttt<<1;
        cout<<"the sig_matrix is to small "<<ttt<<endl;
        time_of_change=band;
    }
    for(int i=0;i<NUM;i++)
        for(int j=0;j<NUM;j++)
        node_matrix[i][j]=0;
    for(int i=0;i<NUM;i++)
    {   for(int j=1;j<num[i].size();i++)
        {   node_matrix[i][num[i][j]]=1;
            node_matrix[num[i][j]][i]=1;
        }
    }
    /*use node_matrix for input matrix,the
    algorithm is from
    http://blog.csdn.net/guoziqing506/article/details/53019049
    */
    int sig_matrix[time_of_change][NUM];
    for(int alltime=0;alltime<time_of_change;alltime++)
    { if(small==1) break;
         /// produce random change vector
        vector <long>old_vector,change_vector;
        for(int i=0;i<NUM;i++)old_vector.push_back(i);
        for(int i=NUM;i>0;i--)
        {   srand(unsigned(time(NULL)));
            int index=rand()%i;
            change_vector.push_back(old_vector[index]);
            old_vector.erase(old_vector.begin()+index);
        }
        for(int i=0;i<NUM-1;i++)
        {   for(int j=i+1;j<NUM;j++)
                if(change_vector[i]==change_vector[j])
                    cout<<"error"<<endl;
        }
        ///produce signature matrix
        /// sig_matrix is signature matrix
        //cout<<" start sig_matrix "<<alltime;
        for(int i=0;i<NUM;i++) sig_matrix[alltime][i]=NUM-1;
        for(int i=0;i<change_vector.size();i++)
        {   int row=change_vector[i];
            for(int j=0;j<NUM;j++)
            {   if(node_matrix[row][j]==true)
                {   sig_matrix[alltime][j]=min(i,sig_matrix[alltime][j]);
                }
            }
        }
    }
    //cout<<"sig_matrix finish"<<endl;
    ///divide some band and hash them into different buckets
    ///can change the hash function.
    vector<int> hash_bucket[band][num_bucket];///hash_bucket record the id of node
    ///hash_bucket[i][j] means hash table i in j bucket
    for(int index=0;index<band;index++)
    {   if(small==1)break;
        //cout<<index<<"hash ";
        int first,last;
        first=index*(time_of_change/band);
        if(index==band-1){last=time_of_change-1;}
        else
        {last=first+(time_of_change/band)-1;}
        ///calculate node's hash
        for(int i=0;i<NUM;i++)
        {   int id_hash=0;
            for(int j=first;j<=last;j++)
            {   id_hash=(sig_matrix[j][i]<<(last-j))+id_hash;
            }
            int bucket_id=normal_hash(id_hash,num_bucket);///this is hash function
            hash_bucket[index][bucket_id].push_back(i);
        }
    }
    //cout<<"hash finish"<<endl;
    long sum=0;
    for(int i=0;i<band;i++)
        for(int j=0;j<num_bucket;j++)
        sum=sum+hash_bucket[i][j].size();
    cout<<sum/band<<endl;
    ///according to hash_bucket make a partition of graph
    ///cut_set_lsh[i] record the set id of node i.
    //there is no need to sort, maybe you can also sort
    //for(int i=0;i<band;i++)
      //  for(int j=0;j<num_bucket;j++)
       // sort(hash_bucket[i][j].begin(),hash_bucket[i][j].end(),cmp);
    // in this process I will do kd-tree, i can also use full kdtree algorithm by kdtree.h and kd_tree.cpp
    int node[NUM];//node[i] value is tree node id;
    memset(node,0,sizeof(int)*NUM);
    int maxhash=0;//maxhash is max hash value in kdtree.
    int minhash=0;//minhash is min hash value in kdtree.
    for(int i=0;i<band;i++)
        for(int j=0;j<num_bucket;j++)
    {   for(int k=0;k<hash_bucket[i][j].size();k++)
        {   int id=hash_bucket[i][j][k];
            node[id]=node[id]+j<<i;
            maxhash=max(maxhash,node[id]);
            minhash=min(minhash,node[id]);
        }
    }
    //cout<<"max:"<<maxhash<<endl;cout<<"min:"<<minhash<<endl;
    for(int i=0;i<SIZE;i++)
    {   int first,last;
        first=i*(maxhash-minhash)/SIZE;
        if(i==SIZE-1) last=maxhash;
        else last=first+(maxhash-minhash)/SIZE-1;
        for(int j=0;j<NUM;j++)
        {   if(node[j]>=first&&node[j]<=last)
            cut_set[j]=i;
        }
    }//this 2d cut
    }

    //small trick
    long minarc,maxarc;
    //cout<<"s"<<endl;
    if(small==1)
    {   for(int i=0;i<NUM;i++)
        cut_set[i]=rand()%SIZE;
    }
    for(int ok=0;ok<20;ok++)
    {
    for(int i=0;i<NUM;i++)
    {   int node;
        for(int j=1;j<num[i].size();j++)
        {   node=num[i][j];
            if(cut_set[i]!=cut_set[node])
            {   int change=rand()%2;
                if(change==0)
                cut_set[node]=cut_set[i];
                else
                    cut_set[i]=cut_set[node];
            }
        }
    }
    minarc=check_lsh_cut();
    if(minarc<18000) break;
    }

}
long check_lsh_cut()
{   long sumarc=0;
    for(int i=0;i<NUM;i++)
    {   int node;
        for(int j=1;j<num[i].size();j++)
        {   node=num[i][j];
            if(cut_set[i]!=cut_set[node]) sumarc++;
        }
    }
    return sumarc/2;
}
long check_lsh_cut_size()
{   int cut_size[SIZE];
    memset(cut_size,0,sizeof(int)*SIZE);
       for(int i=0;i<NUM;i++)
        {   cut_size[cut_set[i]]++;
        }
    //for(int i=0;i<SIZE;i++)
       // cout<<" lsh cut"<<i<<":"<<cut_size[i];

}




int main()
{   //freopen("data.out","w",stdout);
    string line;
	//fstream in_file("graph.data",ios::in);
	fstream in_file("graph.data",ios::in);
	//cout<<"my new"<<endl;
	int cnt=0;//all node in graph
	long allarc=0;//all arc in graph
	while(getline(in_file,line))
	{
	    vector<string>inword;
		int lenth=0;
		for(int i=0;i<line.size();i++)
		{	if(line[i]==' ')
			{	string tmp;
				for(int k=i-lenth;k<i;k++) {tmp=tmp+line[k];}
				inword.push_back(tmp);
				lenth=0;
			}
			else
			{	lenth++;
			}
		}
		//if(cnt==10)cout<<"fuck"<<line<<endl;
		for(int i=0;i<inword.size();i++)
		{	//unsigned input[1500];//if wiki use 1500
            unsigned in;
			//input[i]=atoi(&inword[i][0])-1;
			//num[cnt].push_back(input[i]);
            num[cnt].push_back(in);
        }
		cnt++;
		//cout<<cnt<<" ";
	}
	for(int i=0;i<NUM;i++)
	{	allarc=allarc+num[i].size()-1;
		//cout<<i<<" ";
		//for(int j=1;j<num[i].size();j++)  cout<<num[i][j]<<" ";
		//cout<<endl;

	}
	allarc=allarc/3;
	rand_cut();
	long rand_arc;
	rand_arc=check_rand_cut();
	cout<<"arc cross set:"<<rand_arc<<endl;
	cout<<"total arc:"<<allarc<<endl;
	cout<<"total node:"<<cnt<<endl;
	int rand_arc_sum=0;
	for(int i=0;i<5;i++) rand_arc_sum=check_rand_cut()+rand_arc_sum;
	float rate;rate=rand_arc_sum*0.2/allarc;
	cout<<"rand cross arc rate:"<<rate<<endl;
	cout<<"rand cross arc:     "<<rand_arc_sum/5<<endl;
    //bfs
	bfs_cut();
	long bfs_arc;
	bfs_arc=check_bfs_cut();
	//cout<<check_bfs_cut();
	int bfs_arc_sum=0;
	for(int i=0;i<5;i++) bfs_arc_sum=check_bfs_cut()+bfs_arc_sum;
	float rate2;rate2=bfs_arc_sum*0.2/allarc;
	cout<<"bfs cross arc rate:"<<rate2<<endl;
	cout<<"bfs cross arc     :"<<bfs_arc_sum/5<<endl;
    /*int out[SIZE];
    for(int i=0;i<SIZE;i++) out[i]=0;
    for(int i=0;i<NUM;i++)
    {   out[cut_set_bfs[i]]++;
    }
    int ok=0;
    for(int i=0;i<SIZE;i++) {cout<<out[i]<<" ";ok=ok+out[i];}
    cout<<"all"<<ok;*/
	//cout<<".................................."<<endl;
	//for(int i=1;i<num[10].size();i++) cout<<num[10][i]<<" ";
	//cout<<endl;

	///the result of lsh_cut
	 lsh_cut(1,5,3,8);///void lsh_cut(int time_of_change,int band,int num_bucket)
    long lsh_arc;
    lsh_arc=check_lsh_cut();
    long lsh_arc_sum=0;
    for(int i=0;i<10;i++) lsh_arc_sum=check_lsh_cut()+lsh_arc_sum;
    float rate3;rate3=lsh_arc_sum*0.1/allarc;
    cout<<"lsh cross arc rate:"<<rate3<<endl;
	cout<<"lsh cross arc     :"<<lsh_arc_sum/10<<endl;
	check_lsh_cut_size();

}
