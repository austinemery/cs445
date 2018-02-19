#include <vector>
#include <iostream>
#include <string>
#include <time.h>
#include <fstream>
#include <stdlib.h>
#include <algorithm>

using namespace std;

#define MAXPACKETS 1000

#define ATTACKERMULT 100

int numbUsers = 0;
int numbAttackers = 0;
string fileName = "1att1user.txt";

struct node{

	string myIP;
	string nextIP;
	string type;

};

struct packetNodeSampling{

	string currentRouter;
	string highlightedRouter;

};

struct packetEdgeSampling{

	string currentRouter;
	string start;
	string end;
	int distance;
	int count;

};

void importGraph(vector<node>&, string);

string findCertainType(vector<node>&, string, int);

string findNextIP(vector<node>&, string);

int insertInEdgeVector(vector<packetEdgeSampling>&, packetEdgeSampling);

int sortFunction(packetEdgeSampling, packetEdgeSampling);

void nodeSampling(vector<node>&, float, int);

void edgeSampling(vector<node>&, float, int);

int main(int argc, char* argv[])
{
	vector<node> nodeVec1;
	vector<node> nodeVec2;
	vector<node> nodeVec3;
	vector<node> nodeVec4;
	float probability = 0.2;
	int attackerMult = 100;
	importGraph(nodeVec1, fileName);

	/*just to print the graph
	for (int i = 0; i < nodeVec.size(); i++)
	{
		cout << nodeVec[i].myIP << " " << nodeVec[i].nextIP << " " << nodeVec[i].type << endl;
	}
	*/	

	for (float prob = 0.2; prob < 1; prob += 0.2)
	{
		nodeSampling(nodeVec1, prob, attackerMult);
		edgeSampling(nodeVec1, prob, attackerMult);
	}

	probability = 0.5;

	for (float attMult = 10; attMult < 1001; attMult *= 10)
	{
		nodeSampling(nodeVec1, probability, attMult);
		edgeSampling(nodeVec1, probability, attMult);
	}

	fileName = "1att2user.txt";
	numbUsers = 0;
	numbAttackers = 0;
	importGraph(nodeVec2, fileName);
	probability = 0.2;
	attackerMult = 100;

	for (float prob = 0.2; prob < 1; prob += 0.2)
	{
		nodeSampling(nodeVec2, prob, attackerMult);
		edgeSampling(nodeVec2, prob, attackerMult);
	}

	probability = 0.5;

	for (float attMult = 10; attMult < 1001; attMult *= 10)
	{
		nodeSampling(nodeVec2, probability, attMult);
		edgeSampling(nodeVec2, probability, attMult);
	}

	fileName = "2att1user.txt";
	numbUsers = 0;
	numbAttackers = 0;
	importGraph(nodeVec3, fileName);
	probability = 0.2;
	attackerMult = 100;

	for (float prob = 0.2; prob < 1; prob += 0.2)
	{
		nodeSampling(nodeVec3, prob, attackerMult);
		edgeSampling(nodeVec3, prob, attackerMult);
	}

	probability = 0.5;

	for (float attMult = 10; attMult < 1001; attMult *= 10)
	{
		nodeSampling(nodeVec3, probability, attMult);
		edgeSampling(nodeVec3, probability, attMult);
	}	

	fileName = "2att2user.txt";
	numbUsers = 0;
	numbAttackers = 0;
	importGraph(nodeVec4, fileName);
	probability = 0.2;
	attackerMult = 100;

	for (float prob = 0.2; prob < 1; prob += 0.2)
	{
		nodeSampling(nodeVec4, prob, attackerMult);
		edgeSampling(nodeVec4, prob, attackerMult);
	}

	probability = 0.5;

	for (float attMult = 10; attMult < 1001; attMult *= 10)
	{
		nodeSampling(nodeVec4, probability, attMult);
		edgeSampling(nodeVec4, probability, attMult);
	}
}

//puts the graph into a vector, is read from a txt file
void importGraph(vector<node>& nodeVec, string fileName)
{
	string tempString;
	node tempNode;
	ifstream fin;
	fin.open(fileName.c_str());

	if (fin.is_open())
	{
		while(fin >> tempString)
		{
			tempNode.myIP = tempString;
			fin >> tempString;
			tempNode.nextIP = tempString;
			fin >> tempString;
			tempNode.type = tempString;
			if (tempString == "user")
			{
				numbUsers++;
			}
			else if (tempString == "attacker")
			{
				numbAttackers++;
			}
			nodeVec.push_back(tempNode);
		}
	}
	fin.close();
}

//finds a router given some information
string findCertainType (vector<node>& nodeVec, string typeLooking, int whichOne)
{
	string tempString;

	for (int i = 0; i < nodeVec.size(); i++)
	{
		if (nodeVec[i].type == typeLooking)
		{	
			if (whichOne == 1)
			{
				tempString = nodeVec[i].myIP;
				break;
			}
			else if (whichOne == 2)
			{
				tempString = nodeVec[i].myIP;
			}		
		}
	}
	return tempString;
}

string findNextIP(vector<node>& nodeVec, string currentIP)
{
	for (int i = 0; i < nodeVec.size(); i++)
	{
		if (nodeVec[i].myIP == currentIP)
		{
			return nodeVec[i].nextIP;
		}
	}
}

//inserts into the vector if new packet, if packet already exists, increase count of old packet.
int insertInEdgeVector(vector<packetEdgeSampling>& packetVec, packetEdgeSampling packet)
{
	for (int i = 0; i < packetVec.size(); i++)
	{
		//if the packet already exits
		if (packet.start == packetVec[i].start && packet.end == packetVec[i].end)
		{
			//cout << "SAME! ";
			packetVec[i].count++;
			return 0;
		}
	}

	//if could not find similar
	packetVec.push_back(packet);
	//cout << "NEW! ";
	return 1;
}

//used for the vector sort
int sortFunction(packetEdgeSampling first, packetEdgeSampling second)
{
	return first.distance < second.distance;
}

//runs the node sampling algorithm
void nodeSampling(vector<node>& nodeVec, float probability, int attackerMult)
{
	packetNodeSampling packet;
	packet.highlightedRouter = "none";	

	int routerCountArray[nodeVec.size()];
	int arrayToSort[2][nodeVec.size()];

	for (int i = 0; i < nodeVec.size(); i++)
	{
		routerCountArray[i] = 0;
		arrayToSort[0][i] = i;
		arrayToSort[1][i] = 0;
	}

	srand(time(NULL));

	//normal user(s)
	for (int i = 0; i < MAXPACKETS; i++)
	{	
		//one of the 2 users is selected to send a packet
		packet.currentRouter = findCertainType(nodeVec, "user", (rand() % numbUsers + 1));
		
		while(findNextIP(nodeVec, packet.currentRouter) != "none")
		{
			if (((float)(rand() % 100 + 1) / 100) < probability)
			{
				packet.highlightedRouter = packet.currentRouter;
			}
			
			//move to next router
			packet.currentRouter = findNextIP(nodeVec, packet.currentRouter);
		}
		
		//increment the highlighted router index
		routerCountArray[(int) *packet.highlightedRouter.c_str() - 65]++;
	}

	//attacker(s)
	for (int i = 0; i < (MAXPACKETS * attackerMult); i++)
	{	
		//one of the 2 users is selected to send a packet
		packet.currentRouter = findCertainType(nodeVec, "attacker", (rand() % numbAttackers + 1));
		
		while(findNextIP(nodeVec, packet.currentRouter) != "none")
		{
			if (((float)(rand() % 100 + 1) / 100) < probability)
			{
				packet.highlightedRouter = packet.currentRouter;
			}
			
			//move to next router
			packet.currentRouter = findNextIP(nodeVec, packet.currentRouter);
		}
		
		//increment the highlighted router index
		routerCountArray[(int) *packet.highlightedRouter.c_str() - 65]++;
	}

	//output this data
	ofstream fout;
	fout.open("output.txt", ios::app);

	cout << endl << "--NODE SAMPLING | Probability: " << probability;
	cout << " | Attacker Multiplier: " << attackerMult << "--" << endl;
	cout << "--Number of Users: " << numbUsers << " Number of Attackers: " << numbAttackers;
	cout << " Filename: " << fileName << "--" << endl;
	
	cout << endl << "--Raw Data--" << endl;
	for (int i = 0; i < nodeVec.size(); i++)
	{
		cout << "Letter: " << (char)(i + 65) << " Number: " << routerCountArray[i] << endl;
	}

	fout << endl << "--NODE SAMPLING | Probability: " << probability;
	fout << " | Attacker Multiplier: " << attackerMult << "--" << endl;
	fout << "--Number of Users: " << numbUsers << " Number of Attackers: " << numbAttackers;
	fout << " Filename: " << fileName << "--" << endl;

	fout << endl << "--Raw Data--" << endl;
	for (int i = 0; i < nodeVec.size(); i++)
	{
		fout << "Letter: " << (char)(i + 65) << " Number: " << routerCountArray[i] << endl;
	}
	
	for (int i = 0; i < nodeVec.size(); i++)
	{
		arrayToSort[1][i] = routerCountArray[i];
	}

	//sort the array
	bool sortFlag = 1;
	int temp1 = 0;
	int temp2 = 0;

	while (sortFlag)
	{
		sortFlag = 0;
		for (int i = 0; i < nodeVec.size() - 1; i++)
		{
			if (arrayToSort[1][i] < arrayToSort[1][i + 1])
			{
				temp1 = arrayToSort[0][i];
				temp2 = arrayToSort[1][i];
				arrayToSort[0][i] = arrayToSort[0][i + 1];
				arrayToSort[1][i] = arrayToSort[1][i + 1];
				arrayToSort[0][i + 1] = temp1;
				arrayToSort[1][i + 1] = temp2;
				sortFlag = 1;
			}
		}
	}
	
	//output this data
	cout << endl << "--Sorted Data--" << endl;
	for (int i = 0; i < nodeVec.size(); i++)
	{
		cout << "Letter: " << (char)(arrayToSort[0][i] + 65) << " Number: " << arrayToSort[1][i] << endl;
	}

	fout << endl << "--Sorted Data--" << endl;
	for (int i = 0; i < nodeVec.size(); i++)
	{
		fout << "Letter: " << (char)(arrayToSort[0][i] + 65) << " Number: " << arrayToSort[1][i] << endl;
	}

	fout.close();
}

void edgeSampling(vector<node>& nodeVec, float probability, int attackerMult)
{
	packetEdgeSampling packet;
	packet.start = "none";
	packet.end = "none";
	packet.distance = 0;
	packet.count = 0;
	vector<packetEdgeSampling> packetVec;

	srand(time(NULL));

	//user(s)
	for (int i = 0; i < MAXPACKETS; i++)
	{	
		//one of the 2 users is selected to send a packet
		packet.currentRouter = findCertainType(nodeVec, "user", (rand() % numbUsers + 1));
		
		while(findNextIP(nodeVec, packet.currentRouter) != "none")
		{
			if (((float)(rand() % 100 + 1) / 100) < probability)
			{
				packet.start = packet.currentRouter;
				packet.end = "none";
				packet.distance = 0;
			}
			else
			{
				if (packet.distance == 0)
				{
					packet.end = packet.currentRouter;
				}
				packet.distance++;
			}

			//move to next router
			packet.currentRouter = findNextIP(nodeVec, packet.currentRouter);
		}
		
		if (packet.end != "none" && packet.start != "none")
		{
			insertInEdgeVector(packetVec, packet);
			//cout << "Start: " << packet.start << " End: " << packet.end << " Distance: " << packet.distance << endl;	
		}

		packet.start = "none";
		packet.end = "none";
		packet.distance = 0;
	}

	//attacker(s)
	for (int i = 0; i < MAXPACKETS * attackerMult; i++)
	{	
		//one of the 2 users is selected to send a packet
		packet.currentRouter = findCertainType(nodeVec, "attacker", (rand() % numbUsers + 1));
		
		while(findNextIP(nodeVec, packet.currentRouter) != "none")
		{
			if (((float)(rand() % 100 + 1) / 100) < probability)
			{
				packet.start = packet.currentRouter;
				packet.end = "none";
				packet.distance = 0;
			}
			else
			{
				if (packet.distance == 0)
				{
					packet.end = packet.currentRouter;
				}
				packet.distance++;
			}

			//move to next router
			packet.currentRouter = findNextIP(nodeVec, packet.currentRouter);
		}
		
		if (packet.end != "none" && packet.start != "none")
		{
			insertInEdgeVector(packetVec, packet);
			//cout << "Start: " << packet.start << " End: " << packet.end << " Distance: " << packet.distance << endl;	
		}

		packet.start = "none";
		packet.end = "none";
		packet.distance = 0;
	}

	//output data
	ofstream fout;
	fout.open("output.txt", ios::app);

	cout << endl << "--EDGE SAMPLING | Probability: " << probability;
	cout << " | Attacker Multiplier: " << attackerMult << "--" << endl;
	cout << "--Number of Users: " << numbUsers << " Number of Attackers: " << numbAttackers;
	cout << " Filename: " << fileName << "--" << endl;

	cout << endl << "--Raw Data--" << endl;
	for (int i = 0; i < packetVec.size(); i++)
	{
		cout << "Start: " << packetVec[i].start << " End: " << packetVec[i].end;
		cout << " Distance: " << packetVec[i].distance << " Count: " << packetVec[i].count << endl;
	}

	fout << endl << "--EDGE SAMPLING | Probability: " << probability;
	fout << " | Attacker Multiplier: " << attackerMult << "--" << endl;
	fout << "--Number of Users: " << numbUsers << " Number of Attackers: " << numbAttackers;
	fout << " Filename: " << fileName << "--" << endl;

	fout << endl << "--Raw Data--" << endl;
	for (int i = 0; i < packetVec.size(); i++)
	{
		fout << "Start: " << packetVec[i].start << " End: " << packetVec[i].end;
		fout << " Distance: " << packetVec[i].distance << " Count: " << packetVec[i].count << endl;
	}

	//sort the data
	sort(packetVec.begin(), packetVec.end(), sortFunction);

	cout << endl << "--Sorted Data--" << endl;
	for (int i = 0; i < packetVec.size(); i++)
	{
		cout << "Start: " << packetVec[i].start << " End: " << packetVec[i].end;
		cout << " Distance: " << packetVec[i].distance << " Count: " << packetVec[i].count << endl;
	}

	fout << endl << "--Sorted Data--" << endl;
	for (int i = 0; i < packetVec.size(); i++)
	{
		fout << "Start: " << packetVec[i].start << " End: " << packetVec[i].end;
		fout << " Distance: " << packetVec[i].distance << " Count: " << packetVec[i].count << endl;
	}

	fout.close();
}






