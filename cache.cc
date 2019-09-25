/* GAUTHAM SRINIVASAN . UIN:927008557. */

#include<iostream> // For Standard I/O operations
#include<ctype.h>  // For string operations
#include<string.h> // For strcmp()
#include<math.h> // for pow() function
#include<cstdlib>  // For rand()
#include<ctime> // For using srand().
#include <sstream> // For passing command line parameters using stringstream
#include<fstream> // For file operations
#include<vector> // For file operations
#include <sys/time.h>


#define TOTAL_ADDRESS_BITS 64
#define KB 1024

using namespace std;
typedef struct 
{
    bool ispresent;
    unsigned long long int tag;
}cache_entries;

/* Total miss */
unsigned long int miss = 0;
/* Total read miss */
unsigned long int rmiss = 0;
/* Total write miss */
unsigned long int wmiss = 0;
/* Total access */
unsigned long int total_access = 0;
/* Total read */
unsigned long int total_read = 0;
/* Total write */
unsigned long int total_write = 0;
/* Total hit */
unsigned long int total_hit = 0;


/* Following function gets number of bits in a number */
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

/* Mask to extract bits */
unsigned long int createMask(unsigned a, unsigned b)
{
    unsigned long int r = 0;
    for (unsigned i=a; i<=b; i++)
        r |= 1 << i;

    return r;
}

void check_cache_entry (vector<vector<cache_entries> > &cache, int set_id, int assoc,
        unsigned long int addr, char mode, char rep_policy, unsigned long int tag)
{
    bool found = false;
    int j = 0, i = 0;
    srand(time(0));
    struct timespec t;

    /* Update total access, total read and write */
    total_access++;
    if (mode == 'r')
        total_read++;
    else 
        total_write++;
    

    /* Check for the tag in the cache block */
    for (int i = 0; i < assoc; i++)
    {
        if ((cache[set_id][i].ispresent == true)
                && (cache[set_id][i].tag == tag))
        {

            /* If hit, move the block to 1st pos in case of LRU */
            if (rep_policy == 'l')
            {
                cache_entries c1;
                c1.tag = tag;
                c1.ispresent = true;
                cache[set_id].erase (cache[set_id].begin() + i);
                cache[set_id].insert(cache[set_id].begin(), c1);
                cache[set_id].resize (assoc);
            }

            found = true;
            total_hit++;
            break;
        }
    }

    /* Tag not found in cache block */
    if (!found)
    {
        /* Update total miss, total miss read and write */
        miss++;
        if (mode == 'r')
            rmiss++;
        else 
            wmiss++;

        /* move the block to 1st pos in case of LRU */
        if (rep_policy == 'l')
        {
            cache_entries c1;
            c1.tag = tag;
            c1.ispresent = true;
            cache[set_id].pop_back();
            cache[set_id].insert(cache[set_id].begin(), c1);
            cache[set_id].resize (assoc);
        }
        else if (rep_policy == 'r')
        {
            for (i = 0; i < assoc; i++)
            {
                if (cache[set_id][i].ispresent == false)
                {
                    /* Update the tag in empty block */
                    j = i;
                    break;
                }
                else
                {
                    /* Replace the block with random block */
                    j = rand() % assoc;
                }
            }

            /* Update the cache block with new tag */
            cache[set_id][j].ispresent = true;
            cache[set_id][j].tag = tag;
        }
    }

}

int main(int argc,char *argv[])
{

    int nk = 0, assoc = 0, blocksize = 0;
    char rep_policy;
    unsigned long int nb = 0, ns = 0;
    int bo = 0, si = 0, tag = 0;
    unsigned long int tag_num = 0;
    char mode;
    unsigned long int r = 0;
    unsigned long int result = 0;
    unsigned long int addr = 0x1FFF;
    stringstream ss;
    int block_id = 0;
    int set_id = 0;
    int offset = 0;
    string tmpLine;

    /* Cache size */
    nk = atoi (argv[1]);
    /* Associvity  */
    assoc = atoi (argv[2]);
    /* blocksize */
    blocksize = atoi (argv[3]);
    /* replacement policy LRU or Random */
    rep_policy = argv[4][0];

    /* Number of blocks */
    nb = (nk * KB) / blocksize;
    /* Number of sets */
    ns = nb / assoc;
    /* Number of block offset bits */
    bo = Decimal_Binary_bits(blocksize);
    /* Number of set index bits */
    si = Decimal_Binary_bits(ns);
    /* Number of tag bits */
    tag =  TOTAL_ADDRESS_BITS - si - bo; 

    /* Mask to extract tag number */
    r = createMask(bo+si,63);


    /* Initialize cache */
    vector<vector<cache_entries> > cache(ns, vector<cache_entries>(assoc));
    for (int i = 0; i < ns; i++)
        for (int j = 0; j < assoc; j++)
            cache[i][j].ispresent = false;


    while (getline(cin, tmpLine))
    {
        /* Get the mode and addr from cmd line */
        stringstream ss1(tmpLine);
        ss1 >> mode;
        ss1 >>  hex >> addr;

        /* Block number */
        block_id = addr / blocksize;
        /* Set index number */
        set_id = block_id % ns;
        /* Block offset number */
        offset = addr % blocksize;
        result = r & addr;
        /* Tag number */
        tag_num = result >> (bo+si);

        /* Check the cache enrty */
        check_cache_entry (cache, set_id, assoc, addr, mode, rep_policy, tag_num);
        ss.clear ();
    }

    cout<<  miss << " " << 100*(double)miss/(double)total_access <<"% " ;
    cout<<  rmiss << " " << 100*(double)rmiss/(double)total_read <<"% ";
    cout<<  wmiss << " " << 100*(double)wmiss/(double)total_write <<"% "<< endl;

    return 0;

}
