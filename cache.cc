/* GAUTHAM SRINIVASAN . UIN:927008557. */

#include <iostream> // For Standard I/O operations
#include<ctype.h>  // For string operations
#include<string.h> // For strcmp()
#include<math.h> // for pow() function
#include<cstdlib>  // For rand()
#include<ctime> // For using srand().
#include <sstream> // For passing command line parameters using stringstream
#include<fstream> // For file operations
#include<vector> // For file operations

#define TOTAL_ADDRESS_BITS 64
#define KB 1024
#define INCREMENT 1

using namespace std;
typedef struct 
{
    bool ispresent;
    unsigned long long int tag;
    unsigned long long int freq;
}cache_entries;

int Decimal_Binary_bits (int n)
{
    int num_bits = 0;

    while (1)
    {
       if ((1 << num_bits) & n)
           break;
        else
            num_bits++;
    }

    return num_bits;
}

unsigned long int miss = 0;
unsigned long int rmiss = 0;
unsigned long int wmiss = 0;
unsigned long int total_access = 0;
unsigned long int total_read = 0;
unsigned long int total_write = 0;
unsigned long int total_hit = 0;


void check_cache_entry (vector<vector<cache_entries>> &cache, int set_id, int assoc,
        unsigned long int addr, char mode, char rep_policy, unsigned long int tag)
{
   //cout << "============================== Entering Cache ======================================" << endl;
   // cout << "input addr " << addr << " tag " << tag << " set index " << set_id << endl;
    bool found = false;
    int j = 0, i = 0;
    srand(time(0));

    total_access++;
    if (mode == 'r')
        total_read++;
    else 
        total_write++;
    

    for (int i = 0; i < assoc; i++)
    {
        if ((cache[set_id][i].ispresent == true)
                && (cache[set_id][i].tag == tag))
        {
            found = true;
            cache[set_id][i].freq += 1;
            total_hit++;
            //cout << "hit count " << total_hit<<  endl;

            break;
        }
    }

    if (!found)
    {
        //cout << "NOt found " << endl;
        miss++;
        if (mode == 'r')
            rmiss++;
        else 
            wmiss++;

        for (i = 0; i < assoc; i++)
        {
            if (cache[set_id][i].ispresent == false)
            {
               // cout << "Empty block found" << endl;
                j = i;
                break;
            }
            else
            {
                //cout << "cache["<< set_id << "][" << i << "].freq " << cache[set_id][i].freq << endl;
                if (rep_policy == 'l')
                {
                    if (cache[set_id][i].freq < cache[set_id][j].freq)
                        j = i;
                }
                else if (rep_policy == 'r')
                    j = rand() % assoc;
            }
        }

#if 0
        if (i == assoc)
        {
            cout << "============================== Entering Cache ======================================" << endl;
            for (int i = 0; i < assoc; i++)
            {
                cout << "cache["<< set_id << "][" << i << "].freq " << cache[set_id][i].freq << endl;                
            }
            cout << "j " << j << endl;
            cout << "============================== Exiting Cache ======================================" << endl << endl;
        }
#endif
        cache[set_id][j].ispresent = true;
        cache[set_id][j].tag = tag;
        cache[set_id][j].freq = 1;
    }

     //cout << "============================== Exiting Cache ======================================" << endl << endl;
}


unsigned long int createMask(unsigned a, unsigned b)
{
   unsigned long int r = 0;
   for (unsigned i=a; i<=b; i++)
       r |= 1 << i;

   return r;
}

int main(int argc,char *argv[])
{

    int nk = 0, assoc = 0, blocksize = 0;
    char rep_policy;
    unsigned long int nb = 0, ns = 0;
    int bo = 0, si = 0, tag = 0;
    unsigned long int tag_num = 0;

    fstream file;
    //  string addr;
    char mode;
    unsigned long int r = 0;

    nk = atoi (argv[1]);
    assoc = atoi (argv[2]);
    blocksize = atoi (argv[3]);
    rep_policy = argv[4][0];

    cout << "nk " << nk << endl;
    cout << "assoc " << assoc << endl;
    cout << "blocksize " << blocksize << endl;
    cout << "rep_policy " << rep_policy << endl;


    nb = (nk * KB) / blocksize;
    ns = nb / assoc;
    bo = Decimal_Binary_bits(blocksize);
    si = Decimal_Binary_bits(ns);
    tag =  TOTAL_ADDRESS_BITS - si - bo; 


    cout<<"\n BO: "<<bo<<"\n SI: "<<si<<"\n TAG: "<<tag << endl << endl;
    r = createMask(bo+si,63);
    unsigned long int result = 0;


    file.open ("429.mcf-184B.trace.txt",ios::in);
    //file.open ("test.txt",ios::in);


    unsigned long int addr = 0x1FFF;
    string a;
    stringstream ss;


    int block_id = addr / blocksize;
    int set_id = block_id % ns;

    int offset = addr % blocksize;

    cout << "block_id "  <<  block_id << endl;
    cout << "set_id "  <<  set_id << endl;
    cout << "offset "  <<  offset << endl;
    cout << "no of sets "  <<  ns << endl;
    cout << "assoc "  <<  assoc << endl;

    vector<vector<cache_entries>> cache(ns, vector<cache_entries>(assoc));
    for (int i = 0; i < ns; i++)
        for (int j = 0; j < assoc; j++)
            cache[i][j].ispresent = false;

    while (!file.eof())
    {
        file >> mode;
        file >> a;
        ss << a;
        addr = 0;
        ss >> hex >> addr;
        block_id = addr / blocksize;
        set_id = block_id % ns;

        offset = addr % blocksize;
        //tag_num = bitExtracted(addr, tag , bo+si-1);
        result = r & addr;
        tag_num = result >> (bo+si);
        //cout << "tag_num " << tag_num << endl;

        check_cache_entry (cache, set_id, assoc, addr, mode, rep_policy, tag_num);
        ss.clear ();
    }

    cout.setf(ios_base::fixed, ios_base::floatfield); 
    cout<<"\n";

    cout<< "Total Miss "<< miss << " " << " Miss% " << 100*(double)miss/(double)total_access <<"%" << endl;
    cout<< "TotalReadMiss "<< rmiss << " " << " ReadMiss% " << 100*(double)rmiss/(double)total_read <<"%"<< endl;
    cout<< "TotalWriteMiss "<< wmiss << " " << " WriteMiss% " << 100*(double)wmiss/(double)total_write <<"%"<< endl;

    file.close();
    return 0;

}
