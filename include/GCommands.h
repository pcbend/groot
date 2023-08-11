#ifndef __GCOMMANDS_H__
#define __GCOMMANDS_H__

class TH1;
class TF1;


TH1 *GrabHist(int i=0); //return the ith histogram from the current canvas.
TF1 *GrabFit(int i=0); //return the ith fit from the current canvas.
 

#endif
