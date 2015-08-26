
/* #includes ******************************************************************/

#include "MsgQueue.h"

/* CompilePlayerStats() *******************************************************/
int CompilePlayerStats()
{
/*	string archiveDir="C:\\poker\\GameArchive\\";
	string finalDir="C:\\poker\\GameArchive\\processed\\";

	cout << endl << endl << "CompilePlayerStats()" << endl << "-----------------------------------------------------" << endl << endl;

	// enumerate directories
	WIN32_FIND_DATA dirInfo;
	HANDLE dirHandle=FindFirstFile((archiveDir+"*").c_str(),&dirInfo);
	bool dirMore;
	dirMore=FindNextFile(dirHandle,&dirInfo); // remove "."
	dirMore=FindNextFile(dirHandle,&dirInfo); // remove ".."

	while (dirMore) {
		string dir=dirInfo.cFileName;
		if (dir=="processed"||dir=="tmp"||dir=="unknown") continue;

		cout << dir << endl;
		dirMore=FindNextFile(dirHandle,&dirInfo);

		// enumerate files
		WIN32_FIND_DATA fileInfo;
		HANDLE fileHandle=FindFirstFile((archiveDir+dir+"\\*").c_str(),&fileInfo);
		bool fileMore;
		fileMore=FindNextFile(fileHandle,&fileInfo); // remove "."
		fileMore=FindNextFile(fileHandle,&fileInfo); // remove ".."

		while (fileMore) {
			string file=fileInfo.cFileName;
			cout << "  " << file << endl;
			fileMore=FindNextFile(fileHandle,&fileInfo);

			MsgQueue game;
			game.ParseFile((archiveDir+dir+"\\"+file).c_str());
			rename((archiveDir+dir+"\\"+file).c_str(),(finalDir+dir+"\\"+file).c_str());
		}
		cout << endl;
	}
*/
	return 0;
}

/* ResetPlayerStats() *********************************************************/
int ResetPlayerStats()
{
	/*
	string archiveDir="C:\\poker\\GameArchive\\processed\\";
	string finalDir="C:\\poker\\GameArchive\\";

	cout << endl << endl << "ResetPlayerStats()" << endl << "-----------------------------------------------------" << endl << endl;
	cout << "Delete database? (yes/no)" << endl;
//	string input;
//	cin >> input;
//	if (input!="yes") {
//		cout << "Not deleting database." << endl;
//		return 0;
//	}
//
	// enumerate directories
	WIN32_FIND_DATA dirInfo;
	HANDLE dirHandle=FindFirstFile((archiveDir+"*").c_str(),&dirInfo);
	bool dirMore;
	dirMore=FindNextFile(dirHandle,&dirInfo); // remove "."
	dirMore=FindNextFile(dirHandle,&dirInfo); // remove ".."

	while (dirMore) {
		string dir=dirInfo.cFileName;
		if (dir=="processed"||dir=="tmp"||dir=="unknown") continue;

		cout << dir << endl;
		dirMore=FindNextFile(dirHandle,&dirInfo);

		// enumerate files
		WIN32_FIND_DATA fileInfo;
		HANDLE fileHandle=FindFirstFile((archiveDir+dir+"\\*").c_str(),&fileInfo);
		bool fileMore;
		fileMore=FindNextFile(fileHandle,&fileInfo); // remove "."
		fileMore=FindNextFile(fileHandle,&fileInfo); // remove ".."

		while (fileMore) {
			string file=fileInfo.cFileName;
			cout << "  " << file << endl;
			fileMore=FindNextFile(fileHandle,&fileInfo);
			rename((archiveDir+dir+"\\"+file).c_str(),(finalDir+dir+"\\"+file).c_str());
		}
		cout << endl;
	}	

	// empty database
	MsgQueue mq;
	mq.ClearSNGGames();
	
	// remove WEKA files
	system("erase c:\\poker\\WEKA\\*.* /F /Q");

	*/

	return 0;
}

/* CompilePlayerStats_old() ***************************************************
 *
 * used back in the day to create a large excel file of players in the game
 * compatible with pre-vc++ versions only probably, now contains many syntax/logic errors
 */
int CompilePlayerStats_old()
{  
	/*
	bool newonly=true;

	WIN32_FIND_DATA info;
	if (!newonly) 
	{
		MsgQueue().ClearSNGGames();
		system("move \"C:\\Documents and Settings\\m083306\\My Documents\\dev\\poker manager\\test\\verified\\parsed\\*\" \"C:\\Documents and Settings\\m083306\\My Documents\\dev\\poker manager\\test\\verified\\\"");
	}
	HANDLE handle=FindFirstFile((LPCSTR)"C:\\Documents and Settings\\m083306\\My Documents\\dev\\poker manager\\test\\verified\\*",&info);
	bool more=FindNextFile(handle,&info);;

	for (int i=0; more; i++)
	{
		more=FindNextFile(handle,&info);
		if (!more) break;
	    
		MsgQueue file;

		char buff[512];
		memset(buff,0,512);
		strcpy(buff,"C:\\Documents and Settings\\m083306\\My Documents\\dev\\poker manager\\test\\verified\\");
		strcat(buff,info.cFileName);

		if (strcmp(info.cFileName,"parsed")==0) continue;

		printf("%s\n",info.cFileName);
		file.ParseFile(buff);

		char cmdbuff[1024];
		memset(cmdbuff,0,sizeof(cmdbuff));
		strcpy(cmdbuff,"move \"");
		strcat(cmdbuff,buff);
		strcat(cmdbuff,"\" \"");
		strcat(cmdbuff,"C:\\Documents and Settings\\m083306\\My Documents\\dev\\poker manager\\test\\verified\\parsed\\");
		strcat(cmdbuff,info.cFileName);
		strcat(cmdbuff,"\" \n");
		system(cmdbuff);
	}

	printf("\n\nDATABASE\n\n");

    while (1);
	*/

    return 0;
}