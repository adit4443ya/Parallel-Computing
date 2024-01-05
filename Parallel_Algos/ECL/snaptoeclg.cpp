
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <vector>
#include <utility>
#include <algorithm>
#include "eCL_MIS.h"

int main(int argc, char* argv[])
{
  printf("SNAP to ECL Graph Converter (%s)\n", __FILE__);
  printf("Copyright 2016 Texas State University\n");

  if (argc != 3) {fprintf(stderr, "USAGE: %s input_file_name output_file_name\n\n", argv[0]);  exit(-1);}

  FILE* fin = fopen(argv[1], "rt");  if (fin == NULL) {fprintf(stderr, "ERROR: could not open input file %s\n\n", argv[1]);  exit(-1);}

  char line[256], word1[16], word2[16], word3[16];
  char* ptr = line;
  size_t linesize = 256;
  int cnt;
  do {
    cnt = getline(&ptr, &linesize, fin);
  } while ((cnt > 0) && (strstr(line, "Nodes:") == 0));
  if (cnt <= 0) {fprintf(stderr, "ERROR: could not find line with node and edge counts\n\n");  exit(-1);}
  if ((strstr(line, "Nodes:") == 0) || (strstr(line, "Edges:") == 0)) {fprintf(stderr, "ERROR: could not find line with node and edge counts\n\n");  exit(-1);}

  int nodes = -1, edges = -1;
  cnt = sscanf(line, "%s %s %d %s %d", word1, word2, &nodes, word3, &edges);
  if ((cnt != 5) || (nodes < 1) || (edges < 0) || (strcmp(word1, "#") != 0) || (strcmp(word2, "Nodes:") != 0) || (strcmp(word3, "Edges:") != 0)) {fprintf(stderr, "ERROR: failed to parse nodes and edge counts\n\n");  exit(-1);}

  printf("%s\t#name\n", argv[1]);
  printf("%d\t#nodes\n", nodes);
  printf("%d\t#edges\n", edges);
  printf("no\t#weights\n");

  while (((cnt = getline(&ptr, &linesize, fin)) > 0) && (strstr(line, "#") != 0)) {}
  if (cnt < 3) {fprintf(stderr, "ERROR: could not find non-comment line\n\n");  exit(-1);}

  ECLgraph g;
  g.nodes = nodes;
  g.edges = edges;
  g.nindex = (int*)calloc(nodes + 1, sizeof(int));
  g.nlist = (int*)malloc(edges * sizeof(int));
  g.eweight = NULL;
  if ((g.nindex == NULL) || (g.nlist == NULL)) {fprintf(stderr, "ERROR: memory allocation failed\n\n");  exit(-1);}

  int src, dst;
  std::vector<std::pair<int, int>> v;
  cnt = 0;
  if (sscanf(line, "%d %d", &src, &dst) == 2) {
    cnt++;
    if ((src < 0) || (src >= nodes)) {fprintf(stderr, "ERROR: source out of range\n\n");  exit(-1);}
    if ((dst < 0) || (dst >= nodes)) {fprintf(stderr, "ERROR: source out of range\n\n");  exit(-1);}
    v.push_back(std::make_pair(src, dst));
  }
  while (fscanf(fin, "%d %d", &src, &dst) == 2) {
    cnt++;
    if ((src < 0) || (src >= nodes)) {fprintf(stderr, "ERROR: source out of range\n\n");  exit(-1);}
    if ((dst < 0) || (dst >= nodes)) {fprintf(stderr, "ERROR: source out of range\n\n");  exit(-1);}
    v.push_back(std::make_pair(src, dst));
  }
  fclose(fin);
//   if (cnt != edges) {fprintf(stderr, "ERROR: failed to read correct number of edges\n\n");  exit(-1);}

  std::sort(v.begin(), v.end());
printf("ji");

  g.nindex[0] = 0;
  printf("po%d",cnt);
  for (int i = 0; i < edges; i++) {
    int src = v[i].first;
    int dst = v[i].second;
    g.nindex[src + 1] = i + 1;
    g.nlist[i] = dst;
//   printf("lp");
  }

  for (int i = 1; i < (nodes + 1); i++) {
    g.nindex[i] = std::max(g.nindex[i - 1], g.nindex[i]);
  }
  writeECLgraph(g, argv[2]);
  freeECLgraph(g);
printf("ko");
// cout<<"h/i";
  return 0;
}