#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define MAX_SUBJECTS 5

struct Subject {
    char name[50];
    int marks;
};

int maths=0,physics=0,chemistry=0,english=0,hindi=0;

int reportcardperstudent(char *inputLin)
{
    printf("\nfetched data recored is=%s \n ",inputLin);
    char *token = strtok(inputLin, "-");
    char name[50];
    strcpy(name, token);

    struct Subject subjects[MAX_SUBJECTS];
    int numSubjects = 0;


    token = strtok(NULL, ",");
    while (token != NULL && numSubjects < MAX_SUBJECTS) {
        sscanf(token, "%[^-]-%d", subjects[numSubjects].name, &subjects[numSubjects].marks);
        token = strtok(NULL, ",");
        numSubjects++;
    }

    int minMarks = subjects[0].marks;
    int maxMarks = subjects[0].marks;
    int TotalMarks = 0;
    float percentageMarks = 0.00;
    char minSubject[50];
    char maxSubject[50];
    printf("\n  Student Name: %s\n", name);
    printf("Subject wise Marks out of 100: \n");

    for (int i = 0; i < numSubjects; i++) {
        printf("%s: %d\n", subjects[i].name, subjects[i].marks);
	TotalMarks= (TotalMarks + (subjects[i].marks));

        if (subjects[i].marks < minMarks) {
            minMarks = subjects[i].marks;
            strcpy(minSubject, subjects[i].name);
        }
        if (subjects[i].marks > maxMarks) {
            maxMarks = subjects[i].marks;
            strcpy(maxSubject, subjects[i].name);
        }
    }

    percentageMarks=((float)TotalMarks/5);

    printf("\nTotal marks out of 500 is: %d and Aggregate percentage is: (%.2f)\n", TotalMarks, percentageMarks);
    printf("\nSubject with Minimum Marks: %s (%d)\n", minSubject, minMarks);
    printf("Subject with Maximum Marks: %s (%d)\n", maxSubject, maxMarks);
}


/* Function to calculate individual and aggregate percentage */

float calculatePercentage(int marks[], int numSubjects) 
{
    int totalMarks = 0;
    for (int i = 0; i < numSubjects; i++) 
     {
	//printf("\n Marks in subject[i]=%d",marks[i]);
	if ( marks[i] < 33.33 )
	 {
 	   if (i==0)
		maths++;
 	   if (i==1)
		physics++;
 	   if (i==2)
		chemistry++;
 	   if (i==3)
		english++;
 	   if (i==4)
		hindi++;
	 }
        totalMarks += marks[i];
     }
    return (float)totalMarks / (numSubjects * 100) * 100;
}



/* Function to display students stats when seen by instructor */

int displaystudentsrecord(char *fileBuffer)
{

    int numSubjects = 5;
    char *line = strtok(fileBuffer, "\n");
    char bestStudent[50];
    char worstStudent[50];
    float bestPercentage = 0;
    float worstPercentage = 100;
    float totalpercentage=000.000;
    int count=0;

    while (line != NULL) {
        char studentName[50];
        int marks[numSubjects];

        // Parse the line using sscanf
        if (sscanf(line, "%[^-]-Maths-%d,Physics-%d,Chemistry-%d,English-%d,Hindi-%d",
                   studentName, &marks[0], &marks[1], &marks[2], &marks[3], &marks[4]) == 6) 
         {

            // Calculate individual and aggregate percentage
            float percentage = calculatePercentage(marks, numSubjects);
            printf("\nStudent Name: %s\n Maths=%d,Physics-%d,Chemistry-%d,English-%d,Hindi-%d\n Aggregate Percentage=: %.2f%%\n", studentName, marks[0],marks[1],marks[2],marks[3],marks[4],percentage);

	    totalpercentage=(totalpercentage + percentage);
            // Update best and worst performing students
            if (percentage > bestPercentage) {
                bestPercentage = percentage;
                strcpy(bestStudent, studentName);
            }
            if (percentage < worstPercentage) {
                worstPercentage = percentage;
                strcpy(worstStudent, studentName);
            }
        }
	count++;
        line = strtok(NULL, "\n");
    }

    printf("\nBest Performing Student: %s (%.2f%%)\n", bestStudent, bestPercentage);
    printf("Worst Performing Student: %s (%.2f%%)\n", worstStudent, worstPercentage);
    printf("class average percentage which contains studnets: (%.2f%%) %d\n",(totalpercentage/count),count);
    printf("Students failed in subjects: Maths=%d, physics=%d,chemistry=%d,English=%d,Hindi=%d:\n",maths,physics,chemistry,english,hindi);
    printf("\n ***************************************\n ");

}


void displayerror(const char *msg)
{
    perror(msg);
    exit(0);
}


/* main  program */

int main(int argc, char *argv[])
{
    int sockfd, portno, n,choice=0,i=0;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char str1[10];

    char buf[500],index[50],psswd[50],marks[1000],total[500],updatename[50],newline[50];

    if (argc < 3) {
       fprintf(stderr,"usage %s <serverhostname/serverip_address> <serverport>\n", argv[0]);
       exit(0);
    }

    portno = atoi(argv[2]); 
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        displayerror("ERROR opening socket");

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR..., no host found\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        displayerror("Connection Error");

    printf("Do you want to connect? y/n ");
    scanf("%s",str1);
    char str2[3]="y";
    int value = strcmp(str1,str2); 

    if(value==0)
    {
    
    bzero(buf,500);
    fgets(buf,255,stdin);
    n = write(sockfd,buf,strlen(buf));
    if (n < 0) 
         displayerror("Error in socket writing");
    bzero(buf,500);
    n = read(sockfd,buf,255); //Connection successfully established
    if (n < 0) 
         displayerror("ERROR while reading from socket");

    printf("%s\n",buf);
    int choice;
    while(1)
    {
    	printf("\n\t*****Students Portal Menu *****\t\n\n1.Login as instructor\n2.Login as a student \n3.Update entire Row (by Instructor)\n4.Exit\n");
    	bzero(buf,500);
    	printf("\nEnter Choice : ");
	//scanf("%d", &choice);
    	fgets(buf,55,stdin); //Option 
    	n =write(sockfd,buf,250);
    	choice= atoi(buf);
    	printf("\nentered choice is :%d \n",choice);
    	bzero(buf,255);
    	switch(choice)
    	{  
      		case 1 :
            		printf("Enter Instructor Id:");
            		fgets(index,100,stdin);
			//scanf("%s", index);
            		n = write(sockfd,index,strlen(index));
            		bzero(buf,255);
            		printf("Enter Password:");
            		fgets(psswd,100,stdin);
			//scanf("%s", psswd);
            		n = write(sockfd,psswd,strlen(psswd));
            		i=0;
            		//Displaying all students marks/record
            		printf("\n");
            		bzero(total,200);

                	char buf[4024] = {0};
                	int bytes_received = recv(sockfd, buf, sizeof(buf), 0);

                	if (bytes_received > 0) {
                    		printf("\nReceived data:\n%s\n", buf);
                                /**************Recieived all data from server in buf***************************/   
				displaystudentsrecord(buf);


                	} else {
                    		printf("No data received\n");
                	}

            		// Send response to server to continue or exit
            		char response[10];
            		printf("Press 'enter' to continue if something is missing or type 'again' to see again : ");
            		scanf("%s", response);
            		send(sockfd, response, strlen(response), 0);
            		break;
      		case 2 :  
            		printf("Enter Username:");
            		fgets(index,100,stdin);
			//scanf("%s", index);
            		n = write(sockfd,index,strlen(index));
            		bzero(buf,255);
            		printf("Enter Password:");
            		fgets(psswd,100,stdin);
			//scanf("%s", psswd);
            		n = write(sockfd,psswd,strlen(psswd));
            		bzero(buf,255);
            		n = read(sockfd,buf,13);
                        //Authentication msg 
            		printf("%s\n",buf);
            		n = read(sockfd,marks,150);
            		//printf("\n-----%s\n",marks);
			reportcardperstudent(marks);
                        //Display marks
            		//printf("\n-----%s\n",marks);
            		break;
        
      		case 3 :
            		printf("Enter Instructor Id:");
            		fgets(index,100,stdin);
			//scanf("%s", index);
            		n = write(sockfd,index,strlen(index));
            		bzero(buf,255);
            		printf("Enter Password:");
			//scanf("%s", psswd);
            		fgets(psswd,100,stdin);
            		n = write(sockfd,psswd,strlen(psswd));
            		bzero(buf,255);
            		n = read(sockfd,buf,100);
            		if(strncmp(buf,"Authentication Failed",strlen("Authentication Failed"))==0)
            			{
                			printf("\n%s",buf);
                			break;
            			} 
            		printf("\n%s",buf); 
            		//Enter the Student name whose marks/record need to be updated
            		fgets(updatename,100,stdin);
            		n = write(sockfd,updatename,strlen(updatename));
            		n= read(sockfd,total,100); //Student name found or not found
            		if(strncmp(total,"Student Name Not Found",strlen("Student Name Not Found"))==0)
            			{
                			printf("\n%s\n",total);
                			break;
            			}   
            		printf("Enter the whole row for a students record with updated Marks/info :");
            		scanf("%s",newline);
            		n =write(sockfd,newline,200);
            		printf("\nMarks/Record updated sucessfully !!\n");
            		break;
     		case 4 :
           		printf("\nExiting....\n");
           		close(sockfd);
           		exit(1);
    		}/*end of switch case */
    	}/* End of while loop */
    }
    else
    {
    	close(sockfd);
    	return 0;
    }
} /*End of main func */
