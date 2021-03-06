#pragma once 

#include "defines.h"
#include <iostream>

#ifdef EXTENDED_DOUBLE
	#include "extended_double.h" //inlcude code for extended double if needed
#endif//defined EXTENDED_DOUBLE


#ifdef _WINDOWS_GUI
	#include "../RNAstructure_windows_interface/interface.h"
#endif //_WINDOWS
#include "structure.h"
#include "algorithm.h"


////////////////////////////////////////////////////////////////////////
//pfunctionclass encapsulates the large 2-d arrays of w and v, used by the 
//	partition function
class pfunctionclass {
   int Size;

   public:
   	
      int k;
      PFPRECISION **dg;
      PFPRECISION infinite;
      

      //the constructor allocates the space needed by the arrays
   	pfunctionclass(int size);

      //the destructor deallocates the space used
      ~pfunctionclass();

      //f is an integer function that references the correct element of the array
	inline PFPRECISION &f(int i, int j) {
		   /* In the dg array, each ith column is shifted up by i steps.
		    * For example, if Size = 5, the dg array would be arranged as follows:
		    *   (Where ^ represents a shift upwards, and 0 represents a valid address. 
		    *   Positions marked by ^ or empty space are invalid.)
		    *
		    *         8|    0
		    *         7|   00
		    *    j    6|  000    This means that valid values of i go from i to i+Size-1
		    *         5| 0000      (e.g.  for column i=3,   j can be 3 to 7, inclusive)
		    *    a    4|00000    Correspondingly, any combination of i and j for which 
		    *    x    3|0000^      j<i or j>i+Size-1  are INVALID.    
		    *    i    2|000^^      (e.g. for column i=4: j=3 is invalid and j=8
		    *    s    1|00^^^    During computation, i is allowed to go ABOVE Size, in which case
		    *         0|0^^^^      j must also be greater than Size. The action in this case is to 
		    *          |-----       return the value at (i-Size, j-Size). So in the example at right, 
		    *          |01234       f(5, 7) would return f(0, 2).
		    *           i-axis
		    */
		if (j<i) {
			return infinite; //invalid position
		} else if (j>i+Size-1) {
			fprintf (stderr, "WARNING: j out of bounds in pfunctionclass::f(%d, %d); Size=%d.\n", i, j, Size);
			return infinite;
		} else if (i<1||i>2*Size-1) { //Note that due to the inequality check above, i is also greater than Size.
			fprintf (stderr, "WARNING: i out of bounds in pfunctionclass::f(%d, %d); Size=%d.\n", i, j, Size);
			return infinite;
		} else if (i>Size) { 
			//Note that due to the inequality check above, j must also be greater than Size.
			return f(i-Size,j-Size);
		} else
			return dg[i][j];
	}
};




class pfdatatable //this structure contains all the info read from thermodynamic
							//data files
{
 
public:
	PFPRECISION poppen [5],maxpen,eparam[11],dangle[6][6][6][3],inter[31],bulge[31],
		hairpin[31],stack[6][6][6][6],tstkh[6][6][6][6],tstki[6][6][6][6],
		tloop[maxtloop+1],iloop22[6][6][6][6][6][6][6][6],
		iloop21[6][6][6][6][6][6][6],iloop11[6][6][6][6][6][6],
      coax[6][6][6][6],tstackcoax[6][6][6][6],coaxstack[6][6][6][6],
      tstack[6][6][6][6],tstkm[6][6][6][6],auend,gubonus,cint,cslope,c3,
      efn2a,efn2b,efn2c,triloop[maxtloop+1],init,mlasym,strain,
	  singlecbulge,tstki23[6][6][6][6],tstki1n[6][6][6][6];
	PFPRECISION hexaloop[maxtloop+1];
	PFPRECISION scaling;
	int numoftriloops,numoftloops,numofhexaloops;
	int itloop[maxtloop+1],itriloop[maxtloop+1],ihexaloop[maxtloop+1];
	int maxintloopsize;

	PFPRECISION prelog;

	//Temp is the temperature in absolute scale
	pfdatatable(datatable *indata, PFPRECISION Scaling, PFPRECISION Temp=310.15);
	pfdatatable();

	PFPRECISION temp;//The temperature -- in absolute scale 

	void rescaledatatable(PFPRECISION rescalefactor); //rescale the entries in datatable


};


PFPRECISION pfchecknp(bool lfce1,bool lfce2);
PFPRECISION erg1(int i,int j,int ip,int jp,structure *ct,pfdatatable *data);
		//calculates equilibrium constant of stacked base pairs
PFPRECISION erg2(int i,int j,int ip,int jp,structure *ct,pfdatatable *data,char a,
	char b);
		//calculates equlibrium constant of a bulge/internal loop
PFPRECISION erg2in(int i,int j,int ip,int jp,structure *ct, pfdatatable *data,char a=0,
	char b=0);
		//calculates the equilibrium constant of an internal portion of an internal/bulge loop 
PFPRECISION erg2ex(int i,int j,int size, structure *ct, pfdatatable *data);
		//calculates the equlibrium constant of an exterior fragment of am internal/bulge loop
PFPRECISION erg3(int i,int j,structure *ct,pfdatatable *data,char dbl);
		//calculates equlibrium constant of a hairpin loop
PFPRECISION erg4(int i,int j,int ip,int jp,structure *ct,pfdatatable *data,
	bool lfce);
		//calculates equlibrium constant of a dangling base
PFPRECISION penalty(int i,int j,structure *ct,pfdatatable *data);
	//calculates end of helix penalty
PFPRECISION penalty2(int i, int j, pfdatatable *data);
PFPRECISION ergcoaxflushbases(int i, int j, int ip, int jp, structure *ct, pfdatatable *data);
PFPRECISION ergcoaxinterbases1(int i, int j, int ip, int jp, structure *ct, pfdatatable *data);
PFPRECISION ergcoaxinterbases2(int i, int j, int ip, int jp, structure *ct, pfdatatable *data);


//pfunction performs the partition function calculation and saves the results to disk.
void pfunction(structure* ct,pfdatatable* data, TProgressDialog* update, char* save, bool quickQ=false, PFPRECISION *Q=NULL);

//writepfsave writes a save file with partition function data.
void writepfsave(char *filename, structure *ct, 
			 PFPRECISION *w5, PFPRECISION *w3, 
			 pfunctionclass *v, pfunctionclass *w, pfunctionclass *wmb, pfunctionclass *wl, pfunctionclass *wmbl, pfunctionclass *wcoax,
			 forceclass *fce, bool *mod, bool *lfce, pfdatatable *data);

//readpfsave reads a save file with partition function data.
void readpfsave(const char *filename, structure *ct, 
			 PFPRECISION *w5, PFPRECISION *w3, 
			 pfunctionclass *v, pfunctionclass *w, pfunctionclass *wmb, pfunctionclass *wl, pfunctionclass *wmbl, pfunctionclass *wcoax,
			 forceclass *fce, PFPRECISION *scaling, bool *mod, bool *lfce, pfdatatable *data);
PFPRECISION calculateprobability(int i, int j, pfunctionclass *v, PFPRECISION *w5, structure *ct, pfdatatable *data, bool *lfce, bool *mod, PFPRECISION scaling, forceclass *fce);
void rescale(int currenth,structure *ct, pfdatatable *data, pfunctionclass *v, pfunctionclass *w, pfunctionclass *wl, pfunctionclass *wcoax,
			 pfunctionclass *wmb,pfunctionclass *wmbl, PFPRECISION *w5, PFPRECISION *w3, PFPRECISION **wca, PFPRECISION **curE, PFPRECISION **prevE, PFPRECISION rescalefactor); //function to rescale all arrays when partition function calculation is headed out
															//of bounds
//void rescaleatw3(int ii,structure *ct, pfdatatable *data, pfunctionclass *v, pfunctionclass *w, pfunctionclass *wl, pfunctionclass *wcoax,
//			 pfunctionclass *wmb,pfunctionclass *wmbl, PFPRECISION *w5, PFPRECISION *w3, double rescalefactor); //function to rescale all arrays when partition function calculation is headed out
															//of bounds when claculating w3
//void rescaleatw5(int jj,structure *ct, pfdatatable *data, pfunctionclass *v, pfunctionclass *w, pfunctionclass *wl, pfunctionclass *wcoax,
//			 pfunctionclass *wmb,pfunctionclass *wmbl, PFPRECISION *w5, PFPRECISION *w3, double rescalefactor); //function to rescale all arrays when partition function calculation is headed out
															//of bounds when claculating w5
void thresh_structure(structure *ct, char *pfsfile, double thresh); //determine a structure of probable base pairs (greater than thresh) and deposit it in ct.

//calculate a the partition function, given that the arrays have been allocated
void calculatepfunction(structure* ct,pfdatatable* data, TProgressDialog* update, char* save, bool quickQ, PFPRECISION *Q,
	pfunctionclass *w, pfunctionclass *v, pfunctionclass *wmb, pfunctionclass *wl, pfunctionclass *wmbl,
	pfunctionclass *wcoax, forceclass *fce,PFPRECISION *w5,PFPRECISION *w3,bool *mod, bool *lfce);


// This copies code from pclass.cpp
//A function to convert free energies to equilibrium constants.
//inline PFPRECISION boltzman(short i, PFPRECISION temp) {
//
//	if (i>=INFINITE_ENERGY) return 0;
//	else return exp((-((PFPRECISION) i)/((PFPRECISION)conversionfactor))/(RKC*temp));
//
//}
