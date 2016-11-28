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

struct cache{
    int validbit;
    int dirtybit;
    string tag;
};



class CACHE {
    
public:
    int tagL1;  //size of block
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
        int blockNum=0;
        
        offsetL1 = log2(blocksize);
        
        indexL1=(log2(cachesize)+10)-log2(numass)-offsetL1;
        
        tagL1 = 32-offsetL1-indexL1;
        
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
    }
    
    void initalL2(int b,int na, int c){
        int blocksize=b;
        int numass=na;
        int cachesize=c;

        offsetL2 = log2(blocksize);
        
        indexL2=(log2(cachesize)+10)-log2(numass)-offsetL2;
        
        tagL2 = 32-offsetL2-indexL2;
        
    }
};


//int main(int argc, char* argv[]){
int main(){
    
    config cacheconfig;
    ifstream cache_params;
    string dummyLine;
    //cache_params.open(argv[1]);
    cache_params.open("/Users/gpz/Desktop/Lab2/Lab2/cacheconfig.txt");
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
    

    vector<vector <cache> > cacheL1(CACHE_L1.indexL1 ,vector<cache>(cacheconfig.L1setsize));
    vector<vector <cache> > cacheL2(CACHE_L2.indexL2 ,vector<cache>(cacheconfig.L2setsize));
    //number of tag == number of valid bit = number of dirty bit
    //[index * way]  array of cache struct
    
    
    int L1AcceState =0; // L1 access state variable, can be one of NA, RH, RM, WH, WM;
    int L2AcceState =0; // L2 access state variable, can be one of NA, RH, RM, WH, WM;
    
    
    ifstream traces;
    ofstream tracesout;
    string outname;
    //outname = string(argv[2]) + ".out";
    outname = "Lab2.out";
    //traces.open(argv[2]);
    traces.open("/Users/gpz/Desktop/Lab2/Lab2/trace.txt");
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

            
            
            TagL1=Address.substr(0,CACHE_L1.tagL1);
            TagL2=Address.substr(0,CACHE_L2.tagL2);
            
            int waycount=0;     //using for finding tag in each way
            bitset<32> L1temindex;
            bitset<32> L2temindex;
            long int L1index;
            long int L2index;
            int i1=0;
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
                
                
                
                
                
                
            }
            else
                //Write
            {
                //Implement by you:
                // write access to the L1 Cache,
                //and then L2 (if required),
                //update the L1 and L2 access state variable;
                
                
                while(waycount!=cacheconfig.L1setsize){
                    if (TagL1==cacheL1[L1index][waycount].tag){  //it's a write hit
                        
                    
                    }
                    waycount+=1;
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
