
/* #includes ******************************************************************/

#ifdef _PP_REC_

/* Globals ********************************************************************/

MsgQueue table[MAX_TABLES];
int num_tables=0;

/* RecordDaemon() *************************************************************/

int RecordDaemon()
{
    EnumWindows(EnumWindowsProc,0);

    int j=0;
    while (1)
    {
        j++;
        int i=0;
        for (i=0; i<num_tables; i++)
        {
            table[i].update();
        }

//        printf("Loop %i.  %i tables processed. \n",j,i);
        Sleep(1000);
    }

    return 0;
}

/* EnumWindowsProc() **********************************************************/

BOOL CALLBACK EnumWindowsProc(HWND hwnd,LPARAM lParam)
{
    char buff[255]; memset(buff,0,sizeof(buff));
    GetWindowText(hwnd, buff, sizeof(buff));

    if (strncmp(buff, "Royal Vegas Poker", 10)==0)
    {
        if (strncmp(buff, "Royal Vegas Poker - ", 20)!=0)
        {
            void* loc=(void*)((int)(buff)+60);
            num_tables++;

            int i=0;
            char b[50]; memset(b,0,50);
            char t[50]; memset(t,0,50);
            for (i=0; ((char*)loc)[i]!=' '; i++)
            {
                b[i]=((char*)loc)[i];
            }
            loc=(void*)((int)loc+i+1);
            for (i=0; ((char*)loc)[i]!='['; i++)
            {
                t[i]=((char*)loc)[i];
            }
            t[i-1]=0;

            printf("%s,%s.\n",b,t);

            char *dir="auto";
/*			int buyin=1;
            if (strcmp(t,"Imperial Beach")==0)   { printf("Identified table Imperial Beach.  Buy-in set to 20+2.\n");		dir="S&G $20+2"; buyin=20; }
            if (strcmp(t,"Puerto Escondido")==0) { printf("Identified table Puerto Escondido.  Buy-in set to 10+1.\n");		dir="S&G $10+1"; buyin=10; }
            if (strcmp(t,"Turbo Bonkers")==0)    { printf("Identified table Turbo Bonkers.  Buy-in set to 10+1.\n");		dir="S&G $10+1"; buyin=10; }
            if (strcmp(t,"Long Rager")==0)       { printf("Identified table Long Rager.  Buy-in set to 5+.5.\n");			dir="S&G $5+.5"; buyin=5; }
            if (strcmp(t,"Sea Ghost")==0)        { printf("Identified table Sea Ghost.  Buy-in set to 5+.5.\n");			dir="S&G $5+.5"; buyin=5; }
            if (strcmp(t,"Gleeming The Cube")==0){ printf("Identified table Gleeming The Cube.  Buy-in set to 5+.5.\n");	dir="S&G $5+.5"; buyin=5; }
            if (strcmp(t,"Blunt Side")==0)       { printf("Identified table Blunt Side.  Buy-in set to 5+.5.\n");			dir="S&G $5+.5"; buyin=5; }
*/
            table[num_tables-1].LoadTable(dir,b,t);
        }
    }

    return TRUE;
}

#endif

/* EnumWindowsProc_GetFirstWindow() *******************************************/
BOOL CALLBACK EnumWindowsProc_GetFirstWindow(HWND hwnd,LPARAM lParam)
{
    char buff[255]; memset(buff,0,sizeof(buff));
    GetWindowText(hwnd, buff, sizeof(buff));

/*    if (strncmp(buff, "Royal Vegas Poker", 10)==0)   // Old table info
    {
        if (strncmp(buff, "Royal Vegas Poker - (", 21)==0)
        {
           *((HWND*)lParam)=hwnd;
            return FALSE;
        }
    }
*//*    if (strncmp(buff, "PokerTime", 10)==0)
    {
        if (strncmp(buff, "PokerTime - (", 21)==0)
        {
           *((HWND*)lParam)=hwnd;
            return FALSE;
        }
    }
*/  if (strncmp(buff, "Poker Time", 10)==0) {
        if (strncmp(buff, "Poker Time - Hold'em (", 21)==0)    {
           *((HWND*)lParam)=hwnd;
            return FALSE;
        }
    }

    return TRUE;
}

/* EnumWindowsProc_GetMainWindow() ********************************************/
BOOL CALLBACK EnumWindowsProc_GetMainWindow(HWND hwnd,LPARAM lParam)
{
    char buff[255]; memset(buff,0,sizeof(buff));
    GetWindowText(hwnd, buff, sizeof(buff));

/*    if (strncmp(buff, "Royal Vegas Poker", 10)==0) // Old table info
    {
        if (strncmp(buff, "Royal Vegas Poker - (", 21)!=0)
        {
           *((HWND*)lParam)=hwnd;
            return TRUE;
        }
    }
*//*    if (strncmp(buff, "PokerTime", 10)==0)
    {
        if (strncmp(buff, "PokerTime - (", 21)!=0)
        {
           *((HWND*)lParam)=hwnd;
            return TRUE;
        }
    }
*/  if (strncmp(buff, "Poker Time", 10)==0) {
        if (strncmp(buff, "Poker Time - Hold'em (", 21)!=0)    {
           *((HWND*)lParam)=hwnd;
            return TRUE;
        }
    }

    return TRUE;
}
