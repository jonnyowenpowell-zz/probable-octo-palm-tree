#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>

using namespace std;

//defines the top directory
const string mainDir = "./../../../";

//file to be sumarized
const string sumarizeFileName = "root_cause_description.txt";

//defines segmented data directory
const string segmentedDir = "SegmentedData/";
const string rawSeparatedDir = "RawSeparated/";
const string segmentedFilelocation = mainDir + segmentedDir + rawSeparatedDir + sumarizeFileName;

//file to dump info into
const string dumpDir = "ProcessedSeparated/";
const string dumpFileLocation = mainDir +segmentedDir+  dumpDir + sumarizeFileName;


//list of file names produced
vector<string> values;
vector<int> valueCounts;

//function declaration
void errorOpeningFile();

int main()
{
    //open json file
    ifstream inFile(segmentedFilelocation);

    if(!inFile.is_open()){
        errorOpeningFile();
    }

    string line;
	int loadingCount = 0;
	//while file is not EOF
    while(getline(inFile, line)){
		bool added = false;
		for(unsigned i=0; i<values.size(); ++i){
			if(values.at(i) == line){
				++(valueCounts.at(i));
				added = true;
				break;
			}
		}
		if(!added){
			values.push_back(line);
			valueCounts.push_back(1);
		}
        ++loadingCount;
        cout << "Lines Read: " << loadingCount << endl;
    }

    inFile.close();

	ofstream dumpFile(dumpFileLocation);
	for(int i =0; i<values.size(); ++i){
		dumpFile << values.at(i)  << valueCounts.at(i) << "\n";
	}
	dumpFile.close();

	cout << "End" << endl;

	return EXIT_SUCCESS;
}


void errorOpeningFile(){
    cout << "Error opening file" << endl;
    throw runtime_error("Error opening file");
}
