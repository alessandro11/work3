//#include <bits/stdc++.h>
#include <cstdio>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <string>
#include <string.h>
#include <set>
#include <graphviz/cgraph.h>

using namespace std;

typedef unsigned int UINT;

class Node {
public:
    UINT attr;
    string attr_name;
    string name;
};

typedef vector<Node> NODESET;
typedef pair<int, int> ii;
typedef pair<string, int> si;

NODESET nodes;

vector<vector<int> > adj;
vector<map<int, int> > attr;
unordered_map<string, int> compressnodes;
unordered_map<string, int> compressattrs;
unordered_map<int, string> uncompnodes;
unordered_map<int, string> uncompattrs;
vector<bool> visited;

void sumpaths(int v, int u){ //v += u
    for(auto q:attr[u]){
        auto h = attr[v].find(q.first);
        if(h != attr[v].end())
            (*h).second += q.second;
        else
            attr[v].insert(ii(q.first, q.second));
    }
}

void search(int v){
    for(int u=0; u<adj[v].size(); u++){
        if(!visited[adj[v][u]])
            search(adj[v][u]);
        sumpaths(v, adj[v][u]);
    }
    visited[v] = 1;
}

int main(void)
{
    auto g = agread(stdin, NULL);
    int num = 0;

    //compress attributes
    for(auto a=agnxtattr(g, AGNODE, NULL); a; a=agnxtattr(g, AGNODE, a), num++){
        compressattrs[a->name] = num;
        uncompattrs[num] = a->name;
    }
    num = 0;

    Node v;
    for( Agnode_t *u = agfstnode(g); u; u = agnxtnode(g, u) ) {
        v.name = agnameof(u);
        nodes.push_back(v);
    }

    NODESET::iterator it;
    for( it = nodes.begin(); it < nodes.end(); it++ ) {
        cout << ' ' << it->name;
    }
    return 1;

    //initialize vertexes and attributes
    for(auto v = agfstnode(g); v; v=agnxtnode(g, v), num++){
        compressnodes[agnameof(v)] = num;
        uncompnodes[num] = agnameof(v);
        vector<int> q;
        map<int, int> r;
        adj.push_back(q);
        attr.push_back(r);
        visited.push_back(0);
        for(auto a=agnxtattr(g, AGNODE, NULL); a; a=agnxtattr(g, AGNODE, a)){
            if(strlen(agxget(v, a))!=0){
                attr[num].insert(ii(compressattrs[a->name], atoi(agxget(v, a))));
                visited[num] = 1;
            }
        }
    }

    //initialize edges
    for(auto v = agfstnode(g); v; v=agnxtnode(g, v)){
        for(auto e = agfstedge(g, v); e; e=agnxtedge(g, e, v)){
            if(aghead(e) == v) continue;
            int u = compressnodes[agnameof(aghead(e))];
            adj[compressnodes[agnameof(v)]].push_back(u);
    //        printf("%s -> %s\n", agnameof(v), agnameof(aghead(e)));
        }
    }

    //count paths
    for(int v=0; v<adj.size(); v++){
        if(!visited[v]){ //still not counted
            search(v);
        }
    }

    //set attrs to graph
    for(auto v = agfstnode(g); v; v = agnxtnode(g, v)){
        int vertex = compressnodes[agnameof(v)];
        for(auto a = agnxtattr(g, AGNODE, NULL); a; a = agnxtattr(g, AGNODE, a)){
            string attribute = to_string(attr[vertex][compressattrs[a->name]]);
            agxset(v, a, (char*)attribute.c_str());
        }
    }
/* TESTING
    for(int v=0; v<attr.size(); v++){
        printf("[%s] ->", uncompnodes[v].c_str());
        for(auto u:attr[v]){
            printf("\"%s\" = %d, ", uncompattrs[u.first].c_str(), u.second);
        }
        printf("\n");
    }*/

    //writes down graph
    agwrite(g, stdout);
    agclose(g);
    return 0;
}
