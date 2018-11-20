#include <vector>
#include <algorithm>
#include <climits>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <unordered_map>
#include <utility>
#include <iterator>
using namespace std;

int maxsofar = 0;
string startstring = "";
string endstring = "";
/*
DIJKSTRA(G, w, s)

INITIALIZE(G, s);

T := ∅
Q := a min-priority queue on the vertices V;

while (Q ≠ ∅) do
  u := EXTRACT-MIN(Q);
  T := T ∪ {u};
  for each vertex v in Adj[u] do
    RELAX(u, v, w);
*/

// return left child of A[i]
int getleft(int i){
	return (2*i + 1);
}
// return right child of A[i]
int getright(int i){
	return (2*i + 2);
}
// 
int getparent(int i){
	return (i-1)/2;
}

void siftdown(int i,vector<pair<int,string>>& heap,unordered_map<string,int> &index){
	int l = getleft(i);
	int r = getright(i);
	
	int small = i;

	if (l < heap.size() && heap[l].first < heap[i].first){
		small = l;
	}	
	if (r < heap.size() && heap[r].first < heap[small].first){
		small = r;
	}
	
	if (small != i){
		//change index of i with cur
		string iname = heap[i].second;
		string curname = heap[small].second;
		int inameIndex = index[iname];
		int curnameIndex = index[curname]; 
		index[iname] = curnameIndex;
		index[curname] = inameIndex;
		swap(heap[i], heap[small]);
		siftdown(small,heap,index);
	}
}

void siftup(int i, vector<pair<int,string>>& heap, unordered_map<string,int> & index){
	if (i != 0 && heap[getparent(i)].first > heap[i].first){
		string iname = heap[i].second;
		string curname = heap[getparent(i)].second;
		int inameIndex = index[iname];
		int curnameIndex = index[curname]; 
		index[iname] = curnameIndex;
		index[curname] = inameIndex;
		swap(heap[i],heap[getparent(i)]);
		siftup(getparent(i),heap,index);
	}
}

void pop(vector<pair<int,string>>& heap, unordered_map<string,int> & index){
	heap[0] = heap.back();
	index[heap[0].second] = 0;
	heap.pop_back();
	siftdown(0,heap,index);
}

void decreasekey(vector<pair<int,string>> & heap, unordered_map<string,int> & index, string  name, int newdist){
	heap[index[name]].first = newdist;
	siftup(index[name],heap,index);
}

void printGraph(unordered_map<string,unordered_map<string,int>>& wordlist){
	for(auto words : wordlist){
		string word = words.first;
		cout << word << " ";
		for (auto adjvexs : words.second){
			string adjvex = adjvexs.first;
			int weight = adjvexs.second;
			cout << "(" << adjvex << "," << weight << ") ";
		}	
		cout << "\n";
	}

}
void PPP(unordered_map<string,string>& parents, string child){
	if (parents[child] != "NONE"){
		PPP(parents,parents[child]);
		cout << " " << child;
	}	
	else{
		cout << " " << child;
	}
}

int diestar(
unordered_map<string,unordered_map<string,int>>&wordlist,
string source,
unordered_map<string,int> &index,
unordered_map<string,int> &distance,
unordered_map<string,string> &parent,
vector<pair<int,string>> &heap){
	// Set up heap;
	// pair<distance,string>
	heap.emplace_back(make_pair(0,source)); // setup heap for source
	index[source] = 0;
	distance[source] = 0;
	int tracker = 1;
	for (auto &words : wordlist){
		if (words.first != source){
			heap.emplace_back(make_pair(INT_MAX,words.first)); // put each word in heap
			index[words.first] = tracker; // store index
			distance[words.first] = INT_MAX; // assume no path
			parent[words.first] = "NONE"; // assume no parent
			tracker++; // update location 
		}
	}
	parent[source] = "NONE";
	while(heap.size() >  0){
		pair<int,string> v = heap[0];
		pop(heap,index);
		for (auto& x : wordlist[v.second]){
			int total = 0;
			if (distance[v.second] == INT_MAX){
				total = INT_MAX;
			}
			else{
				total = distance[v.second] + wordlist[v.second][x.first];
			}
			if (total < distance[x.first]){
				distance[x.first] = total;
				parent[x.first] = v.second; 
				heap[index[x.first]].first = total;
				siftup(index[x.first],heap,index);
			}
		}
	}	
}


int main(){
	// boiler plate /////////////////////
	// opening file
	ifstream file;
	file.open("dict.txt");
	string word;	
	/////////////////////////////////////

	// read all words in the txt and load them into our wordlist
	unordered_map<string,unordered_map<string, int>> wordlist;
	while(file >> word){
		wordlist[word] = unordered_map<string,int>();	
	}
	// iterate through wordlist and finish constructing edges
	for (auto &words : wordlist){
		string word = words.first;
		int l = word.length();
		
		//reverse case
		string revWord(word);
		reverse(revWord.begin(), revWord.end());
		auto looker = wordlist.find(revWord);
		if (looker != wordlist.end() && word != revWord){
			// found in the wordlist and the string is not a palindrome
		 	wordlist[word][revWord] = l;
		} 
		//twiddle case
		for(int i = 0; i < l-1; i++){
			if (word[i] != word[i+1]){
				string twiddled(word);
				swap(twiddled[i+1],twiddled[i]);
				auto looker = wordlist.find(twiddled);
				if (looker != wordlist.end()){
					// found in the wordlist
				 auto	looker = wordlist[word].find(twiddled);
					if (looker != wordlist[word].end()){
						// appears to already be in our adjvexs
						if (wordlist[word][twiddled] > 2){
							wordlist[word][twiddled] = 2;
						} 
					}
					else{
						wordlist[word][twiddled] = 2;
					}
				}
			}
		}
	// insert case
		for(int i = 0; i < l+1; i++){
			for(char c = 'a'; c <= 'z'; c++){
				string inserted(word);
				inserted = inserted.substr(0,i) + c + inserted.substr(i,l-i);
				auto looker = wordlist.find(inserted);
				if (looker != wordlist.end()){
					// this word exists in our wordlist
					auto looker = wordlist[word].find(inserted);
					if (looker != wordlist[word].end()){
						// appears to already be in our adjvexs
						if (wordlist[word][inserted] > 1){
							wordlist[word][inserted] = 1;
						}
					}
					else{
						wordlist[word][inserted] = 1;
					}
				}
			}
		}
		// delete case
		for(int i = 0; i < l; i++){
			string deleted(word);
			deleted.erase(deleted.begin()+i);
			auto looker = wordlist.find(deleted);
			if (looker != wordlist.end()){
				// this word exists in the wordlist
				auto looker = wordlist[word].find(deleted);
				if (looker != wordlist[word].end()){
					// appears to already be in our adjvexs
					if (wordlist[word][deleted] > 3){
						wordlist[word][deleted] = 3;
					}
				}
				else{
					wordlist[word][deleted] = 3;
				}
			}
		}
	}
	//printGraph(wordlist);
	unordered_map<string,int> index;
	unordered_map<string,int> distance;
	unordered_map<string,string> parent;
	vector<pair<int,string>> heap;

	double i = 0;
	for(auto& words : wordlist){
			diestar(wordlist,words.first,index,distance,parent,heap);
			for(auto& other: wordlist){
				if(distance[other.first] != INT_MAX && distance[other.first] >= maxsofar){
					startstring = words.first; 
					endstring =  other.first;
					maxsofar = distance[other.first];
					cout << startstring << " " << endstring << ":" << maxsofar << endl;
				}
			}
				cout << i++ << "/" << "58109" << ":" << i/58109 * 100 << "%" << endl; 	
		}
	cout << startstring << " to  "<< endstring << ":" << maxsofar << endl;
	return 0;
}
