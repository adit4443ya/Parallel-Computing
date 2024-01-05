/*
ECL-MIS code: ECL-MIS is a maximal independent set algorithm that is quite
fast and produces relatively large sets. It operates on graphs stored in
binary CSR format.

Copyright (c) 2017-2020, Texas State University. All rights reserved. Patented.

Redistribution and use in source and binary forms, with or without
modification, are permitted (subject to the limitations in the disclaimer
below) provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   * Neither the name of Texas State University nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY
THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL TEXAS STATE UNIVERSITY
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

Authors: Martin Burtscher and Ghadeer Alabandi

URL: The latest version of this code is available at
https://userweb.cs.txstate.edu/~burtscher/research/ECL-MIS/.

Publication: This work is described in detail in the following paper.
Martin Burtscher, Sindhu Devale, Sahar Azimi, Jayadharini Jaiganesh, and
Evan Powers. A High-Quality and Fast Maximal Independent Set Implementation
for GPUs. ACM Transactions on Parallel Computing, Vol. 5, No. 2, Article 8
(27 pages). December 2018.
*/


#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#include <omp.h>
#include <iostream>
#include "eCL_MIS.h"

using namespace std;
typedef unsigned char stattype;
static const stattype in = ~0 - 1;
static const stattype out = 0;


/* hash function to generate random values */

// source of hash function: https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
static unsigned int hasH(unsigned int val)
{
  val = ((val >> 16) ^ val) * 0x45d9f3b;
  val = ((val >> 16) ^ val) * 0x45d9f3b;
  return (val >> 16) ^ val;
}


int main(int argc, char* argv[])
{
  printf("ECL-MIS v1.3 (%s)\n", __FILE__);
  printf("Copyright 2017-2020 Texas State University\n");

  if (argc != 3) {fprintf(stderr, "USAGE: %s number_of_threads input_file_name\n\n", argv[0]); exit(-1);}
  const int thread_count = atoi(argv[1]);
  if (thread_count <= 0) {fprintf(stderr, "ERROR: Number of requested threads must be at least 1\n"); exit(-1);}

  ECLgraph g = readECLgraph(argv[2]);
  cout<<"hi";
  printf("configuration: %d nodes and %d edges (%s)\n", g.nodes, g.edges, argv[2]);
  printf("average degree: %.2f edges per node\n", 1.0 * g.edges / g.nodes);
  stattype* const nstatus = new stattype [g.nodes];

  timeval start, end;
  gettimeofday(&start, NULL);

  /* prioritized-selection initialization */

  const float avg = (float)g.edges / g.nodes;
  const float scaledavg = ((in / 2) - 1) * avg;
  #pragma private(nstatus,scaledavg,avg)
  #pragma omp parallel for num_threads(thread_count) default(none) shared(g)
  for (int i = 0; i < g.nodes; i++) {
    stattype val = in;
    const int degree = g.nindex[i + 1] - g.nindex[i];
    if (degree > 0) {
      const float x = degree - (hasH(i) * 0.00000000023283064365386962890625f);
      const int res = roundf((scaledavg / (avg + x)));
      val = (res + res) | 1;
    }
    nstatus[i] = val;
  }

  const int chunksize = g.nodes / (thread_count * 100) + 1;
  int missing, ms;

  /* main computation loop */

  #pragma omp parallel num_threads(thread_count) default(none) shared(g,missing) private(ms)
  do {
    #pragma omp barrier
    #pragma omp single
    {
      #pragma omp atomic write
      missing = 0;
    }
    #pragma omp for schedule(guided, chunksize)
    for (int v = 0; v < g.nodes; v++) {
      int nsv;
      #pragma omp atomic read
      nsv = nstatus[v];
      if (nsv & 1) {
        int i = g.nindex[v];
        while (i < g.nindex[v + 1]) {
          int nsi;
          #pragma omp atomic read
          nsi = nstatus[g.nlist[i]];
          if ((nsv < nsi) || ((nsv == nsi) && (v < g.nlist[i]))) break;
          i++;
        }
/*
        while ((i < g.nindex[v + 1]) && ((nsv > nstatus[g.nlist[i]]) || ((nsv == nstatus[g.nlist[i]]) && (v > g.nlist[i])))) {
          i++;
        }
*/
        if (i < g.nindex[v + 1]) {
          #pragma omp atomic write
          missing = 1;
        } else {
          for (int i = g.nindex[v]; i < g.nindex[v + 1]; i++) {
            #pragma omp atomic write
            nstatus[g.nlist[i]] = out;
          }
          #pragma omp atomic write
          nstatus[v] = in;
        }
      }
    }
    #pragma omp atomic read
    ms = missing;
  } while (ms != 0);

  gettimeofday(&end, NULL);
  const double runtime = end.tv_sec + end.tv_usec / 1000000.0 - start.tv_sec - start.tv_usec / 1000000.0;
  printf("compute time: %.4f s\n", runtime);

  int count = 0;
  for (int v = 0; v < g.nodes; v++) {
    if (nstatus[v] == in) {
      count++;
    }
  }
  printf("elements in set: %d (%.1f%%)\n", count, 100.0 * count / g.nodes);

  /* result verification code */

  for (int v = 0; v < g.nodes; v++) {
    if ((nstatus[v] != in) && (nstatus[v] != out)) {fprintf(stderr, "ERROR: found unprocessed node in graph\n\n");  exit(-1);}
    if (nstatus[v] == in) {
      for (int i = g.nindex[v]; i < g.nindex[v + 1]; i++) {
        if (nstatus[g.nlist[i]] == in) {fprintf(stderr, "ERROR: found adjacent nodes in MIS\n\n");  exit(-1);}
      }
    } else {
      int flag = 0;
      for (int i = g.nindex[v]; i < g.nindex[v + 1]; i++) {
        if (nstatus[g.nlist[i]] == in) {
          flag = 1;
        }
      }
      if (flag == 0) {fprintf(stderr, "ERROR: set is not maximal\n\n");  exit(-1);}
    }
  }

  freeECLgraph(g);
  delete [] nstatus;
  return 0;
}









