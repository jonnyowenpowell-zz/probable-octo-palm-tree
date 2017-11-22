#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>

using namespace std;

const string resultsStr ="  \"results\": [";

//defines the top directory
const string mainDir = "./../../../";

//defines dataset directory
const string reacallDir= "deviceRecallData_20-11-17/";
const string jsonFileName = "device-recall-0001-of-0001.json";
const string jsonFileLocation = mainDir + reacallDir + jsonFileName;

//defines segmented data directory
const string segmentedDir = "SegmentedData/";
const string rawSeparatedDir = "RawSeparated/";
const string segmentedFilelocation = mainDir + segmentedDir + rawSeparatedDir;

//defines list of segmented files
const string segmentedFileListFileLocation = mainDir + segmentedDir + "fileList.txt";

//defines a list of variables that don't need to be recoreded
const string lineExclutions[1] = {"none"};

//errors in file
const string errorList[] = {"pma_numbers", "k_numbers"};

//list of file names produced
vector<string> segmentedFileNames;
//list of files produced
vector<ofstream*> segmentedFiles;


static int loadingCount =0;

//declaration of methods
bool containsClose(string* str);
void cleanData(string* str);
bool removeChar(string* str, char ch);
void errorOpeningFile();

int main()
{
    //open json file
    ifstream jsonFile(jsonFileLocation);

    if(!jsonFile.is_open()){
        errorOpeningFile();
    }

    string line;
    bool isMeta = true;
    //while file is not EOF
    while(getline(jsonFile, line)){

        //ignore metadata
        if(isMeta){
            if(line == resultsStr){
                isMeta = false;
            }else{
                cout << line << endl;
            }
        }
        // !meta => results
        //results
        else{
             //sanitize data (remove spaces and ")
             cleanData(&line);

            //if line is not part of json's formating structure
            if( !(line=="{" || line=="}" || line=="}," || line=="[" || line=="]" || line=="],") ){

                //find where variable name ends
                int dilemma = line.find(':');
                string varName = line.substr(0, dilemma);
                if(errorList !=nullptr){
                    for(unsigned i=0; i<(sizeof(errorList)/sizeof(errorList[0])); ++i){
                        if(varName==errorList[i].substr(0, errorList[i].length()-1)){
                            varName +="s";
                            break;
                        }
                    }
                }
                //ignore specific files
                bool ignore = false;
                if(lineExclutions != nullptr){
                    for(unsigned i=0; i<sizeof(lineExclutions)/sizeof(lineExclutions[0]); ++i ){
                        if(varName == lineExclutions[i] ){
                            ignore = true;
                            break;
                        }
                    }
                }
                if(!ignore){
                    //find appropriate file
                    ofstream *file = nullptr;

                    //find correct file
                    for(unsigned i = 0; i<segmentedFileNames.size(); ++i){
                        if(varName ==segmentedFileNames.at(i)){
                            file = segmentedFiles.at(i);
                            break;
                        }
                    }
                    //if file doesn't exist yet, make the file
                    if(!file){
                        segmentedFileNames.push_back(varName);
                        segmentedFiles.push_back(new ofstream(segmentedFilelocation + varName + ".txt")/*, std::ofstream::out*/);
                        file = (segmentedFiles.at(segmentedFiles.size()-1));
                    }

                    //insert into file
                    string info = line.substr(dilemma+1, (line.length()-(dilemma+1)));

                    //special case
                    if(info == "["){
                        *file << "[";
                        while(getline(jsonFile, info)){
                          

                            //sanitize data (remove spaces and ")
                            cleanData(&info);

                            if(containsClose(&info)){
                                *file << info.substr(0, info.length()-1);
                                break;
                            }else{
                                *file << info;
                            }
							++loadingCount;
							cout << "Lines Read: " << loadingCount << endl;
                        }
                        *file << "\n";
                    }
                    //regular case
                    else{
                        *file << info << "\n";
                    }
                }
            }
        }
        ++loadingCount;
        cout << "Lines Read: " << loadingCount << endl;
    }

    //close the files
    for(unsigned i=0; i<segmentedFiles.size(); ++i){
        segmentedFiles.at(i)->close();
		cout << "closing file: " << segmentedFileNames.at(i) << endl;
        delete [] (segmentedFiles.at(i));
    }

    //dump file list
	cout << "dumping file" << endl;
    ofstream fileList(segmentedFileListFileLocation);
    for(unsigned i=0; i<segmentedFileNames.size(); ++i){
        fileList << segmentedFileNames.at(i) << "\n";
    }
    fileList.close();
    jsonFile.close();

	cout << "End" << endl;

	return EXIT_SUCCESS;
}


bool containsClose(string* str){
    int pos = str->find("]");
    return (pos != string::npos);
}


void cleanData(string* str){
    while(removeChar(str, ' '));
    while(removeChar(str, '"'));
}

bool removeChar(string* str, char ch){
    int pos = str->find(ch);
    bool b = (pos != string::npos);
    if(b){
        str->erase(pos, 1);
    }
    return b;
}

void errorOpeningFile(){
    cout << "Error opening file" << endl;
    throw runtime_error("Error opening file");
}
