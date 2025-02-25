#include "compareSnapshot.h"

#define MDSchedulerLogFilePath "./LogFile/SchedulerLog.txt"

#define CurrentlyRunningSnapShotFile "./MetaDataFile/CurrentlyRunningSnapShotFile.txt"

using namespace std;


static int counterForThread=0;

string getCurrentTimeZone()
{
    std::time_t rawtime;
    std::tm* timeinfo;
    char buffer[80];
    // char * buffer = (char *)malloc(sizeof(80*sizeof(char)));

    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);

    std::strftime(buffer,80,"%Y-%m-%d-%H-%M-%S",timeinfo);
    std::puts(buffer);
	string finalresult(buffer);
	// delete buffer;
	return finalresult;
}

class Scheduler{

public: 


bool writeToEndOfFile(string Data,string path)
{
	bool success = false;
	if(Data!="")
	{
		std::ofstream out;
 		out.open(path, std::ios::app);
 		out << Data << endl;
		out.close();
	}
	return success;
}

// remove the line from file with name as passed sourceName 
bool replaceCurrentLineInFile(string sourceNameToremove,string path)
{

	writeLog("Entering into replaceCurrentLineInFile for CurrentlyRunningSnapShotFile",1);

	try
	{
		std::string line;
		vector<string> newfileLineToDumpForMetadata;

		std::ifstream file(path);
		
		bool firstLine=false;

		while (getline(file,line))
		{
			if(line != sourceNameToremove)
			{
				line.erase( std::remove(line.begin(), line.end(), '\r'), line.end() );
				line.erase( std::remove(line.begin(), line.end(), '\n'), line.end() );
				newfileLineToDumpForMetadata.push_back(line);
			}
		}

	file.close();

		std::ofstream out;
		out.open(path);
		for(auto itemContent : newfileLineToDumpForMetadata)
		{
			out << itemContent << endl;
		}
		out.close();
	}
	catch(const char* msg)
	{
			writeLog(msg,-1);
	}
	writeLog("Exiting into replaceCurrentLineInFile : CurrentlyRunningSnapShotFile ",1);

	return true;
}

vector<string> GetAllEntriesOfCurrentlyRunningSnapShot(string path)
{
	std::string line;
	std::ifstream file(path);
	vector<string> newfileLineToDumpForMetadata;
		
		bool firstLine=false;

		while (getline(file,line))
		{
				line.erase( std::remove(line.begin(), line.end(), '\r'), line.end() );
				line.erase( std::remove(line.begin(), line.end(), '\n'), line.end() );
				newfileLineToDumpForMetadata.push_back(line);
		}

	file.close();
	return newfileLineToDumpForMetadata;
}

bool writeLog(string Data,int flag)
{
	bool success = false;
	if(Data!="")
	{
		std::ofstream out;
 		out.open(MDSchedulerLogFilePath, std::ios::app);
 		string strDate(getCurrentTime());
 		if(flag == 1)
 		{
 			out << "Information : " ;
 		}
 		if(flag == -1)
 		{
 			out << "Error : " ;
 		}
 		out <<  strDate << ":" << Data << endl;
		out.close();
	}
	return success;
}




void static performCURDOperation(vector<compareSnapshot> diffList,SyncData syncDataObj,
	map<string,string> AllDetailsForTimeUpdate){

	Scheduler schedulerobj;
	
	schedulerobj.writeLog("Entering into performCURDOperation",1);

	// SyncData syncDataObj;

	if(syncDataObj.SRCPATH==""){
		schedulerobj.writeLog("syncData is Null ",-1);		
	}else{
		schedulerobj.writeLog("Thread Started Preperation CURD ",1);
		schedulerobj.writeLog(to_string(++counterForThread),1);
		
		syncDataObj.runTasks(diffList);

		schedulerobj.writeLog("Thread Stopped Preperation CURD Ended ",1);
	}

	//$$$$$$$$$$$$$$$$$$$$
	schedulerobj.writeLog("Scheduler Started updating time Stamp of Meta Data File",1);
	if(AllDetailsForTimeUpdate.size()>0)
	{
			// Create a map iterator and point to beginning of map
		std::map<std::string, string>::iterator it = AllDetailsForTimeUpdate.begin();
	 
		while (it != AllDetailsForTimeUpdate.end())
		{
			it->second=getCurrentTimeZone();
			schedulerobj.replaceCurrentLineInFile(it->first,CurrentlyRunningSnapShotFile);
	 		it++;
		}

		bool success = schedulerobj.updateLastUpdatedTimeForSnapShot(AllDetailsForTimeUpdate);
	}
	schedulerobj.writeLog("Updated the time Stamp of Meta Data File",1);
	AllDetailsForTimeUpdate.clear();

	schedulerobj.writeLog("Exiting into performCURDOperation",1);



}


vector<compareSnapshot> processSnapShot(string sourcePath,string destinationPath,SyncData &syncDataObj){

	writeLog("Entering into processSnapShot",1);

	
    Scheduler Schedulerobj;
	// SyncData syncDataObj;
	// CreateSnapShotClass createSnapShotClassObj;

	

	char cwd[PATH_MAX];
	if (getcwd(cwd, sizeof(cwd)) != NULL) 
	{
		printf("Current working dir: %s\n", cwd);
	}

    Schedulerobj.writeLog("MetaData Preperation",1);
	syncDataObj.createSnapshotFunctionality(toCharArrayFromString(sourcePath),toCharArrayFromString(sourcePath));

	// chdir(cwd);

 //    Schedulerobj.writeLog("Creation OfSnapShot File",1);
	// createSnapShotClassObj.CreateSnapshotFile(toCharArrayFromString(sourcePath),toCharArrayFromString(sourcePath));

	chdir(cwd);

    Schedulerobj.writeLog("Post Snapshot File Creation",1);
	// chdir(cwd);

	vector<compareSnapshot> diffList;

	Schedulerobj.writeLog(sourcePath,1);
	Schedulerobj.writeLog(destinationPath,1);

	if(getcwd(cwd, sizeof(cwd)) != NULL) 
	{
		printf(" 8888888888888888 Current working dir: %s\n", cwd);
	}

	diffList = syncDataObj.compareSnapshotFile(sourcePath,destinationPath);
	Schedulerobj.writeLog(" 8888888888888888 Current working dir ",1);
	if(diffList.size()==0)
	{
		Schedulerobj.writeLog("No Difference Found Among BackUp and Original Path",1);
	}
	else
	{
		Schedulerobj.writeLog("Difference Found Among BackUp and Original Path",1);
	}

	for(int i=0;i<diffList.size();i++)
	{
		cout<< diffList[i].details.fullQualifiedPath<<" "<<diffList[i].operationType<<"\n";
		string toWriteInFile = diffList[i].details.fullQualifiedPath + " Operation: " + diffList[i].operationType;
		Schedulerobj.writeLog(toWriteInFile,1);
	}
    
    Schedulerobj.writeLog("Has Created SnapShot Now Going for Comparing snapShot",1);
	

    writeLog("Exiting into processSnapShot",1);

    return diffList;	
}

vector<struct SnapShotMetaDataInformation> CreateManifest()
{
		cout<<"Inside Manifest function\n";

		vector<string> CurrentlyRunningSnapshot = GetAllEntriesOfCurrentlyRunningSnapShot(CurrentlyRunningSnapShotFile);


		vector<struct SnapShotMetaDataInformation> metadataContent = ProcessMetadataFileIntoCollection();
				cout<<"Count : &&&&&&&777 : " <<  metadataContent.size()<< endl;
		vector<struct SnapShotMetaDataInformation> metadataToProcessForScheduler;
		if(metadataContent.size()>0)
		{
			for(auto itemContent : metadataContent)
			{

				if(CurrentlyRunningSnapshot.size()>0 && 
				find(CurrentlyRunningSnapshot.begin(),CurrentlyRunningSnapshot.end(),
						itemContent.sourcePath)!=CurrentlyRunningSnapshot.end())
					continue;

				vector<string> tC = split(itemContent.lastRunTime,"-");
				int year;int month;int day;int hour; int min;int sec;
				if(tC.size()>=6)
				{
					year=stoi(tC[0]);month=stoi(tC[1]);day=stoi(tC[2]);
					hour=stoi(tC[3]);min=stoi(tC[4]);sec=stoi(tC[5]);
				} 
				std::tm lastModifiedTime = make_tm(year,month,day,hour, min,sec);
				std::tm* lastModifiedTimeptr = &lastModifiedTime;
				std::time_t now = time(0);
				// Convert now to tm struct for local timezone
				std::tm* localtm = localtime(&now);

				std::time_t time2 = std::mktime(lastModifiedTimeptr);
				std::time_t time1 = std::mktime(localtm);

				double portable_difference = std::difftime(time1, time2);
				
				cout << "Current Time" << time1 << endl; 				
				cout << "From File " << time2 << endl;
				cout << portable_difference << endl;

				if(portable_difference>timeInterval)
				{
					cout << "Added : " << itemContent.lastRunTime << endl;
					metadataToProcessForScheduler.push_back(itemContent);
				}
			}
		}

		for(int i=0;i<metadataToProcessForScheduler.size();i++){

			cout << metadataToProcessForScheduler[i].lastRunTime << " &&&&&&&&&&& ";

		}

		cout<<"Exiting Manifest function\n";
		return metadataToProcessForScheduler;
}

char * getCurrentTime()
{
    std::time_t result = std::time(nullptr);
    return std::asctime(std::localtime(&result));
}




bool endsWith(const std::string& s, const std::string& suffix)
{
    return s.size() >= suffix.size() &&
           s.substr(s.size() - suffix.size()) == suffix;
}

std::tm make_tm(int year,int month,int day,int hour, int min,int sec)
{
    std::tm tm = {0};
    tm.tm_year = year - 1900; // years count from 1900
    tm.tm_mon = month - 1;    // months count from January=0
    tm.tm_mday = day;         // days count from 1
    tm.tm_hour = hour; // years count from 1900
    tm.tm_min = min;    // months count from January=0
    tm.tm_sec = sec;         // days count from 1
    return tm;
}

std::vector<std::string> split(const std::string& s, const std::string& delimiter, const bool& removeEmptyEntries = false)
{
    std::vector<std::string> tokens;

    for (size_t start = 0, end; start < s.length(); start = end + delimiter.length())
    {
         size_t position = s.find(delimiter, start);
         end = position != string::npos ? position : s.length();

         std::string token = s.substr(start, end - start);
         if (!removeEmptyEntries || !token.empty())
         {
             tokens.push_back(token);
         }
    }

    if (!removeEmptyEntries &&
        (s.empty() || endsWith(s, delimiter)))
    {
        tokens.push_back("");
    }

    return tokens;
}

void printcwd(){
    char cwd[256];
    getcwd(cwd,sizeof(cwd));
    printf("%s\n",cwd);

}

vector<struct SnapShotMetaDataInformation> ProcessMetadataFileIntoCollection()
{
	std::string line;
	std::ifstream file(MDPath);
	vector<string> tokens;

	bool firstLine=false;
	vector<struct SnapShotMetaDataInformation> AllDataOfFile;
	while (getline(file,line))
	{
		cout << " Line " << line << endl;
		// if(firstLine==false)
		// {
		// 	firstLine=true;
		// 	continue;
		// }

 		tokens = split(line,"\t");

 		if(tokens.size()<4){
 			writeLog("Error : Metadata file is not in its specified format",-1);
 			return AllDataOfFile;
 		}

		if(tokens.size()>0)
		{
			struct SnapShotMetaDataInformation information;
			information.sourcePath=tokens[0];
			information.destinationPath=tokens[1];
			information.creationTimeStamp=tokens[2];
			information.lastRunTime=tokens[3];
			AllDataOfFile.push_back(information);
		}
	}
	file.close();
	return AllDataOfFile;
}

bool updateLastUpdatedTimeForSnapShot(map<string,string> DataWithValue)
{
	string deleteline;
	std::string line;

	std::ifstream file(MDPath);
	
	//ofstream temp;
	//temp.open("temp.txt");
	
	vector<string> tokens;
	bool firstLine=false;

	vector<string> newfileLineToDumpForMetadata;

	while (getline(file,line))
	{
		// if(firstLine==false)
		// {
		// 	firstLine=true;
		// 	//temp << line <<endl;
		// 	line.erase( std::remove(line.begin(), line.end(), '\r'), line.end() );
		// 	line.erase( std::remove(line.begin(), line.end(), '\n'), line.end() );
		// 	newfileLineToDumpForMetadata.push_back(line);
		// 	continue;
		// }

 		tokens = split(line,"\t");		
		if(tokens.size()>0 && DataWithValue.find(tokens[0])!=DataWithValue.end())
		{
			struct SnapShotMetaDataInformation information;
			information.sourcePath=tokens[0];
			information.destinationPath=tokens[1];
			information.creationTimeStamp=tokens[2];
			information.lastRunTime=DataWithValue[tokens[0]];
			string strDataToWrite = PrepareData(information);
			//temp << strDataToWrite ;
			strDataToWrite.erase( std::remove(strDataToWrite.begin(), strDataToWrite.end(), '\r'), strDataToWrite.end() );
			strDataToWrite.erase( std::remove(strDataToWrite.begin(), strDataToWrite.end(), '\n'), strDataToWrite.end() );

			newfileLineToDumpForMetadata.push_back(strDataToWrite);
		}
		else
		{
			//temp << line <<endl;
			line.erase( std::remove(line.begin(), line.end(), '\r'), line.end() );
			line.erase( std::remove(line.begin(), line.end(), '\n'), line.end() );
			
			newfileLineToDumpForMetadata.push_back(line);
		}
	}

	file.close();

	std::ofstream out;
	out.open(MDPath);

	for(auto itemContent : newfileLineToDumpForMetadata)
	{
		//if(itemContent!="")
			itemContent=itemContent+"\n";
			out << itemContent;
	}
	out.close();

	//temp.close();
	
	//remove(MDPath);
	//rename("temp.txt",MDPath);
	return true;
}

string PrepareData(struct SnapShotMetaDataInformation info)
{
	string strData;
	//if(info)
	{
		strData.append(info.sourcePath);
		strData.append("\t");
		strData.append(info.destinationPath);
		strData.append("\t");
		strData.append(info.creationTimeStamp);
		strData.append("\t");
		strData.append(info.lastRunTime);
		strData.erase( std::remove(strData.begin(), strData.end(), '\r'), strData.end() );
		strData.erase( std::remove(strData.begin(), strData.end(), '\n'), strData.end() );
		strData.append("\n");
	}
	return strData;
}



};



int main(){

	vector<compareSnapshot> diffList;
	Scheduler schedulerObj;
	
	//Create Map For Processing
	map<string,string> AllDetailsForTimeUpdate;

	//Create Map For Processing
	//map<string,string> AllDetailsForTimeUpdate;
	//AllDetailsForTimeUpdate["/home/prakashjha/os/workarea/CheckRepo"]=getCurrentTimeZone();
	// schedulerObj.updateLastUpdatedTimeForSnapShot(AllDetailsForTimeUpdate);
	
/**/

	// CreateSnapShotClass createSnapShotClassObj;
	
	char cwd[PATH_MAX];
	if (getcwd(cwd, sizeof(cwd)) != NULL) 
	{
		printf("Current working dir: %s\n", cwd);
	}

	int l=0;
	unsigned long long int counter = 0;
	// while(++l<3)
	while(true)
	{
		counter++;
		schedulerObj.writeLog(to_string(counter),1);
		schedulerObj.writeLog("*******************************************************************************",1);
		SyncData syncDataObj;
		schedulerObj.writeLog(" Scheduler Started ",1);
		vector<SnapShotMetaDataInformation> snapShotToProcess;
		snapShotToProcess = schedulerObj.CreateManifest();

		

		for(int i=0;i<snapShotToProcess.size();i++){

			cout<<"Scheduler: "<<snapShotToProcess[i].sourcePath<<" "<<snapShotToProcess[i].destinationPath<<"\n";

	
			diffList = schedulerObj.processSnapShot(snapShotToProcess[i].sourcePath,snapShotToProcess[i].destinationPath,syncDataObj);
			//schedulerObj.writeLog(" Post Process SnapShot ",1);
			AllDetailsForTimeUpdate[snapShotToProcess[i].sourcePath]=getCurrentTimeZone();

			// If In File Not Present Then Only Make Entry In File 
			vector<string> CurrentlyExecutingSnapShot = schedulerObj.GetAllEntriesOfCurrentlyRunningSnapShot(CurrentlyRunningSnapShotFile);


			if(diffList.size()>0){

				if(find( CurrentlyExecutingSnapShot.begin(), CurrentlyExecutingSnapShot.end(),snapShotToProcess[i].sourcePath)
					==CurrentlyExecutingSnapShot.end())
				{
					schedulerObj.writeToEndOfFile(snapShotToProcess[i].sourcePath,CurrentlyRunningSnapShotFile);
				}
				else
				{
					schedulerObj.writeLog("-------------- Path Will Not Be Added For Running SnapShot  ",1);
					schedulerObj.writeLog(snapShotToProcess[i].sourcePath,1);	
				}
				
				schedulerObj.writeLog("```````````````````````````````````````````````````````````````````````````````",1);
				schedulerObj.writeLog(snapShotToProcess[i].sourcePath,1);
					
				// schedulerObj.performCURDOperation(diffList,syncDataObj);


				thread t(schedulerObj.performCURDOperation,diffList,syncDataObj,AllDetailsForTimeUpdate);	
			 	AllDetailsForTimeUpdate.clear();
			 	t.join();


			}else{


				cout<<"no op\n";
				chdir(cwd);

				schedulerObj.writeLog("No Updation : Scheduler Started updating time Stamp of Meta Data File",1);
				if(AllDetailsForTimeUpdate.size()>0)
				{
					bool success = schedulerObj.updateLastUpdatedTimeForSnapShot(AllDetailsForTimeUpdate);
				}
				schedulerObj.writeLog("Updated the time Stamp of Meta Data File",1);
				AllDetailsForTimeUpdate.clear();


				schedulerObj.writeLog("No operation to perform",1);		
			}
			 
		}
		schedulerObj.writeLog("*******************************************************************************",1);
		// schedulerObj.writeLog("Scheduler Started updating time Stamp of Meta Data File",1);
		// if(AllDetailsForTimeUpdate.size()>0)
		// 	bool success = schedulerObj.updateLastUpdatedTimeForSnapShot(AllDetailsForTimeUpdate);
		// schedulerObj.writeLog("Updated the time Stamp of Meta Data File",1);
		// AllDetailsForTimeUpdate.clear();

		// unsigned int microseconds = 1200000;
		schedulerObj.writeLog(" Scheduler About to Sleep ",1);
		cout<<"no op2\n";
		sleep(timeInterval);

		schedulerObj.writeLog(" Scheduler Awaked ",1);
	
		diffList.clear();
		chdir(cwd);
			
	}

	schedulerObj.writeLog(" Scheduler Stopped ",1);

	return 0;
}
