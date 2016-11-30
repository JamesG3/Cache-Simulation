/*
 Cache Simulator
 Level one L1 and level two L2 cache parameters are read from file (block size, line per set and set per cache).
 The 32 bit address is divided into tag bits (t), set index bits (s) and block offset bits (b)
 s = log2(#sets)   b = log2(block size)  t=32-s-b
 */
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <cmath>
#include <bitset>

using namespace std;
//access state:
#define NA 0 // no action
#define RH 1 // read hit
#define RM 2 // read miss
#define WH 3 // Write hit
#define WM 4 // write miss




struct config{
    int L1blocksize;
    int L1setsize;
    int L1size;
    int L2blocksize;
    int L2setsize;
    int L2size;
};

/* you can define the cache class here, or design your own data structure for L1 and L2 cache
 class cache {
 
 }
 */

class CACHE {
    
public:
    int tagL1;  //size of block  (how many bits are used for tag)
    int indexL1;     //number of way
    int offsetL1;   //size of cache
    
    int tagL2;  //size of block
    int indexL2;     //number of way
    int offsetL2;   //size of cache
    vector<bitset<32>> tagVector;
    
    CACHE(){
        
    }
    
    void initalL1(int block ,int associativity, int cache){
        int blocksize=block;
        int numass=associativity;
        int cachesize=cache;
        //int blockNum=0;
        
        offsetL1 = log2(blocksize);
        
        if(associativity==0){           //if fully associative
            indexL1=0;
        }
        else{
            indexL1=(log2(cachesize)+10)-log2(numass)-offsetL1;     //+10 -> KB
        }
        tagL1 = 32-offsetL1-indexL1;
        
        
        /*
        int aa = (log2(cachesize)+10)-offsetL1;
        blockNum = pow(2,aa);
        //int tagSize=pow(2,tagL1);
        
        tagVector.resize(blockNum);
        
        int offInd=pow(2,tagL1);
        int offArray[offInd];
        for (int i=0;i<offInd;i++){
            offArray[i]=i;
        }
        
        int indInd=pow(2,indexL1);
        int indexArray[indInd];
        for (int i=0;i<indInd;i++){
            indexArray[i]=i;
        }
        */
    }
    
    void initalL2(int b,int na, int c){
        int blocksize=b;
        int numass=na;
        int cachesize=c;

        offsetL2 = log2(blocksize);
        
        if(na==0){                  //if L2 fully associative
            indexL2=0;
        }
        else{
        indexL2=(log2(cachesize)+10)-log2(numass)-offsetL2;
        }
        
        tagL2 = 32-offsetL2-indexL2;
        
    }
};


//int main(int argc, char* argv[]){
int main(){
    

    config cacheconfig;
    ifstream cache_params;
    string dummyLine;
    //cache_params.open(argv[1]);
    cache_params.open("/Users/gpz/Desktop/Lab2New/Lab2New/cacheconfig.txt");
    while(!cache_params.eof())  // read config file
    {
        cache_params>>dummyLine;
        cache_params>>cacheconfig.L1blocksize;
        cache_params>>cacheconfig.L1setsize;
        cache_params>>cacheconfig.L1size;
        
        cache_params>>dummyLine;
        cache_params>>cacheconfig.L2blocksize;
        cache_params>>cacheconfig.L2setsize;
        cache_params>>cacheconfig.L2size;
    }
    
    
    // Implement by you:
    // initialize the hirearch cache system with those configs
    // probably you may define a Cache class for L1 and L2, or any data structure you like
    
    CACHE CACHE_L1;     //new class CACHE_L1
    CACHE CACHE_L2;     //new class CACHE_L2
    CACHE_L1.initalL1(cacheconfig.L1blocksize,cacheconfig.L1setsize, cacheconfig.L1size);
    CACHE_L2.initalL2(cacheconfig.L2blocksize,cacheconfig.L2setsize, cacheconfig.L2size);
    
    
    if(cacheconfig.L1setsize==0){       //if L1 fully-associative
        cacheconfig.L1setsize=log2(cacheconfig.L1size)+10 - CACHE_L1.offsetL1;
    }
    if(cacheconfig.L2setsize==0){       //if L2 fully-associative
        cacheconfig.L2setsize=log2(cacheconfig.L2size)+10 - CACHE_L2.offsetL2;
    }
    
    
    //2d vector for saving tags
    vector<vector <string> > cacheL1_Tag(pow(2,CACHE_L1.indexL1) ,vector<string>(cacheconfig.L1setsize));
    vector<vector <string> > cacheL2_Tag(pow(2,CACHE_L2.indexL2) ,vector<string>(cacheconfig.L2setsize));
    
    
    //2d vector for saving valid bits and dirty bits, VD[0] is valid bit, VD[1] is dirty bit
    vector<vector <bitset<2>> > cacheL1_VD(pow(2,CACHE_L1.indexL1) ,vector<bitset<2>>(cacheconfig.L1setsize));
    vector<vector <bitset<2>> > cacheL2_VD(pow(2,CACHE_L2.indexL2) ,vector<bitset<2>>(cacheconfig.L2setsize));
    
    
    
    //vector for saving evict counter (round robin)
    vector<int> cacheL1_evictcounter;
    vector<int> cacheL2_evictcounter;
    cacheL1_evictcounter.resize(pow(2,CACHE_L1.indexL1));
    cacheL2_evictcounter.resize(pow(2,CACHE_L2.indexL2));
    
    
    int L1AcceState =0; // L1 access state variable, can be one of NA, RH, RM, WH, WM;
    int L2AcceState =0; // L2 access state variable, can be one of NA, RH, RM, WH, WM;
    
    
    ifstream traces;
    ofstream tracesout;
    string outname;
    //outname = string(argv[2]) + ".out";
    outname = "/Users/gpz/Desktop/Lab2New/Lab2New/traceout.txt";
    //traces.open(argv[2]);
    traces.open("/Users/gpz/Desktop/Lab2New/Lab2New/trace.txt");
    tracesout.open(outname.c_str());
    
    string line;
    string accesstype;  // the Read/Write access type from the memory trace;
    string xaddr;       // the address from the memory trace store in hex;
    unsigned int addr;  // the address from the memory trace store in unsigned int;
    bitset<32> accessaddr; // the address from the memory trace store in the bitset;
    
    if (traces.is_open()&&tracesout.is_open()){
        while (getline (traces,line)){   // read mem access file and access Cache
            
            istringstream iss(line);
            if (!(iss >> accesstype >> xaddr)) {break;}
            stringstream saddr(xaddr);
            saddr >> std::hex >> addr;
            accessaddr = bitset<32> (addr);
        
            string Address=accessaddr.to_string();
            string TagL1;
            string TagL2;

            
            L1AcceState=0;      //reset AcceState for every line
            L2AcceState=0;
            
            
            TagL1=Address.substr(0,CACHE_L1.tagL1);
            TagL2=Address.substr(0,CACHE_L2.tagL2);
            //需要检查tag是不是正确的位数
            
            
            int waycount=0;     //using for finding tag in each way
            bitset<32> L1temindex;
            bitset<32> L2temindex;
            long int L1index;
            long int L2index;
            int i1=0;           //a counter using for taking index part out of address
            int i2=0;
            
            while(i1!=CACHE_L1.indexL1){
                L1temindex[i1]=accessaddr[i1+CACHE_L1.offsetL1];
                i1+=1;
            }
            L1index=L1temindex.to_ulong();      //long int L1index
            
            while(i2!=CACHE_L2.indexL2){
                L2temindex[i2]=accessaddr[i2+CACHE_L2.offsetL2];
                i2+=1;
            }
            L2index=L2temindex.to_ulong();      //long int L2index

            
            // access the L1 and L2 Cache according to the trace;
            if (accesstype.compare("R")==0)
                //Read
            {
                //Implement by you:
                // read access to the L1 Cache,
                //  and then L2 (if required),
                //  update the L1 and L2 access state variable;
                
                while(waycount!=cacheconfig.L1setsize){
                    if (TagL1==cacheL1_Tag[L1index][waycount] and cacheL1_VD[L1index][waycount][0]==1){
                        //if tag match and valid bit is 1
                        L1AcceState=RH;
                    }
                    waycount+=1;
                }
                if(L1AcceState==0){     //if accestate not change, it's a read miss, then access L2
                    L1AcceState=RM;
                    waycount=0;         //reset counter
                    while(waycount!=cacheconfig.L2setsize){
                        if(TagL2==cacheL2_Tag[L2index][waycount] and cacheL2_VD[L2index][waycount][0]==1){
                            //if tag match and valid bit is 1
                            L2AcceState=RH;
                        }
                        waycount+=1;
                    }
                    if(L2AcceState==0){
                        L2AcceState=RM;
                    }
                }
                
                
                //if miss
                waycount=0;
                if (L1AcceState==RM and L2AcceState==RH){           //L1 miss, L2 hit
                    while(waycount != cacheconfig.L1setsize){
                        if(cacheL1_VD[L1index][waycount][0]==0){
                            cacheL1_Tag[L1index][waycount]=TagL1;     //save tag
                            cacheL1_VD[L1index][waycount][0]=1;       //set valid bit to 1
                            
                            break;
                        }
                        waycount+=1;
                    }
                    
                    if(waycount==cacheconfig.L1setsize){            //all ways are full, evict using round robin
                        if(cacheL1_evictcounter[L1index]==cacheconfig.L1setsize){
                            cacheL1_evictcounter[L1index]=0;                       //roll over
                        }
                        cacheL1_Tag[L1index][cacheL1_evictcounter[L1index]]=TagL1;  //save tag to L1[index][evictcounter]
                        
                        cacheL1_VD[L1index][cacheL1_evictcounter[L1index]][1]=0;    //modify the dirty bit to 0
                        
                        cacheL1_evictcounter[L1index]+=1;
                    }
                }
                
                if (L1AcceState==RM and L2AcceState==RM){           //L1 miss, L2 miss
                    while(waycount != cacheconfig.L1setsize){           //L1 saving
                        if(cacheL1_VD[L1index][waycount][0]==0){
                            cacheL1_Tag[L1index][waycount]=TagL1;
                            cacheL1_VD[L1index][waycount][0]=1;
                            break;
                        }
                        waycount+=1;
                    }
                    
                    if(waycount==cacheconfig.L1setsize){            //all ways are full, evict using round robin
                        if(cacheL1_evictcounter[L1index]==cacheconfig.L1setsize){
                            cacheL1_evictcounter[L1index]=0;                       //roll over
                        }
                        cacheL1_Tag[L1index][cacheL1_evictcounter[L1index]]=TagL1;
                        cacheL1_VD[L1index][cacheL1_evictcounter[L1index]][1]=0;
                        cacheL1_evictcounter[L1index]+=1;
                    }
                   
                                                                        //L2 saving
                    waycount=0;                                         //reset waycount
                    while(waycount != cacheconfig.L2setsize){
                        if(cacheL2_VD[L2index][waycount][0]==0){
                            cacheL2_Tag[L2index][waycount]=TagL2;
                            cacheL2_VD[L2index][waycount][0]=1;
                            break;
                        }
                        waycount+=1;
                    }
                    
                    if(waycount==cacheconfig.L2setsize){            //all ways are full, evict using round robin
                        if(cacheL2_evictcounter[L2index]==cacheconfig.L2setsize){
                            cacheL2_evictcounter[L2index]=0;                       //roll over
                        }
                        cacheL2_Tag[L2index][cacheL2_evictcounter[L2index]]=TagL2;
                        cacheL2_VD[L2index][cacheL2_evictcounter[L2index]][2]=0;
                        cacheL2_evictcounter[L2index]+=1;
                    }
                }
              
                
                
            }
            else
                //Write
            {
                //Implement by you:
                // write access to the L1 Cache,
                //and then L2 (if required),
                //update the L1 and L2 access state variable;
                
                
                while(waycount!=cacheconfig.L1setsize){
                    if (TagL1==cacheL1_Tag[L1index][waycount] and cacheL1_VD[L1index][waycount][0]==1){
                        //it's a write hit
                        L1AcceState = WH;
                        cacheL1_VD[L1index][waycount][1]=1;   //write a new data, set dirty bit to 1
                        break;
                    }
                    waycount+=1;
                }
                
                
                if(L1AcceState==0){     //if L1 write miss, access L2
                    L1AcceState=WM;
                    waycount=0;         //reset waycount
                    while(waycount!=cacheconfig.L2setsize){
                        if(TagL2==cacheL2_Tag[L2index][waycount] and cacheL2_VD[L2index][waycount][0]==1){
                            L2AcceState=WH;
                            cacheL2_VD[L2index][waycount][1]=1;
                            break;
                        }
                        waycount+=1;
                    }
                    if (L2AcceState==0){
                        L2AcceState=WM;
                    }
                }
                
                
            }
            
            
            tracesout<< L1AcceState << " " << L2AcceState << endl;  // Output hit/miss results for L1 and L2 to the output file;
            
            
        }
        traces.close();
        tracesout.close();
    }
    else cout<< "Unable to open trace or traceout file ";
    
    return 0;
}
