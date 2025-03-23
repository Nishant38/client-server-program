
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void displayerror(const char *msg)
{
    perror(msg);
    exit(1);
}


//Function to compare given username and password
//Returns 1 if matches

int authenticate(char *data,char *usrname,char *usrpasswd )
{
    int i=0,index=0;
    int len =strlen(data);
    char uname[100],pswd[100];
    while(i<len && data[i] !='-')
    {
        uname[i]= data[i];
        i++;
    }
    uname[i]= '\0';
    i++;
    while(i<len)
    {
        pswd[index++]= data[i++];
    }
    pswd[index]= '\0';
    if(strncmp(uname,usrname,strlen(uname)) == 0 && strncmp(pswd,usrpasswd,strlen(pswd))== 0)
        return 1;         
    return 0;
}

// check marks of a given student

int checkusername(char *marks,char *usrname)
{  
    int i=0,len;
    int len1 =strlen(marks);
    char unameid[100],size[100];
    while(i<len1 && marks[i] !='-')
    {
        unameid[i]= marks[i];
        i++;
    }
    unameid[i]= '\0';
    if (strncmp(unameid,usrname,strlen(unameid)) == 0)
        return 1;
   return 0;
}


/* Main Function : run time argument is port_number             */


int main(int argc, char *argv[])
{    
     FILE *fp,*fp1;
     int sockfd, newsockfd, portno,choice=0,i=0,catch=0,index;
     socklen_t clilen;
     char buf[256],usrname[100],usrpasswd[100],data[500],marks[500],total[200],newline[500],updatename[100],newline1[200];
     struct sockaddr_in serv_addr, cli_addr;
     int n,chars=0;

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided: should be run like : ./server <port_number> \n");
         exit(1);
     }

     /* open tcp socket */
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        displayerror("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              displayerror("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
     if (newsockfd < 0) 
          displayerror("ERROR on accept");
     bzero(buf,256);
     n = read(newsockfd,buf,255); //Reading "y"
     if (n < 0) 
            displayerror("ERROR reading from socket");
     printf("\nUser connected !   %s\n",buf);
     n = write(newsockfd,"Connection Established Successfully",80);
     if (n < 0) 
            displayerror("ERROR writing to socket");

     /* infinite loop to run server to server client  */
     while(1)
     {
     	printf("\n i am here in server to get input again \n");
     	bzero(buf,256);
     	n = read(newsockfd,buf,250); //Choice
     	printf("%s",buf); 
     	choice= atoi(buf);
     	printf("%d",choice); 
     	char *result;
     	switch(choice)     
     		{  case 1 :
              		n = read(newsockfd,usrname,100); //Instructor id
              		n= read(newsockfd,usrpasswd,100); //Password
              		printf("\nEntered id is %s ",usrname);
              		printf("\nEntered Password is %s",usrpasswd);
              		if(strncmp(usrname,"Swades",strlen("Swades"))==0 && strncmp(usrpasswd,"ell785",strlen("ell785"))==0)
              			{  
              				printf("\nSending The Data....\nDone!\n");
              				fp1=fopen("student_marks.txt","r+");
              				char dataBuffer[4024];
              				size_t bytesRead = fread(dataBuffer, 1, sizeof(dataBuffer), fp1);
              				fclose(fp1);
              				send(newsockfd,dataBuffer,strlen(dataBuffer),0);  //Sending all students data
              				printf("\nSending The Data....\n%s\n",dataBuffer);

              				// Receive response from client
              				char response[10];
              				recv(newsockfd, response, sizeof(response), 0);
              				if (strcmp(response, "again") == 0) 
                 				{
                     					break; 
                 				}
              				}
              				else
              				{ 
                				printf("\nAuthentication Failed");
                				send(newsockfd,"Authentication Failed\n",30,0);     
              				}
	      				break;
         		case 2:
            			n = read(newsockfd,usrname,100); //username
            			n= read(newsockfd,usrpasswd,100); //password 
            			printf("\nEntered id is %s",usrname);
            			printf("\nEntered Password is %s",usrpasswd);       
            			fp=fopen("user_pass.txt","r");
            			fp1=fopen("student_marks.txt","r+");
            			if (fp ==NULL)
            			   { 
                			printf("\nFailed to load data");
            			   }       
            			while (i<30)
            			  {
                			fgets(data,64,fp);
                			catch=authenticate(data,usrname,usrpasswd);
                			if(catch==1)
                  				break;
              				i++;            
              			  }
              			if(i==30)
                  			//send(newsockfd,"Error: Auth Failed",30,0);
					send(newsockfd,"Authentication Failed",30,0);
              			i=0;
              			if (catch==1)
              				{ 
                				send(newsockfd,"Authenticated",13,0);
                				//Displaying student marks
                				while(i<30)
                				{  
                  					index=0;
                  					fgets(marks,300,fp1);
							//printf("\n fetched data is: %s\n",marks);
                  					index = checkusername(marks,usrname);
                  					if(index ==1)
                    						break;
                  					i++;
                				} 
                				if(index==1)
                					{ 
                  						printf("\nSending The Data....\nDone!\n");
                  						send(newsockfd,marks,strlen(marks),0); //Sending marks 
                					}
              				}
	      				fclose(fp);
	      				fclose(fp1);
              				break;
         		case 3 :
            			catch=0;
            			n = read(newsockfd,usrname,100); //Instructor id
            			n= read(newsockfd,usrpasswd,100); //Instructor pass
            			printf("\nEntered id is %s",usrname);
            			fp1 = fopen("student_marks.txt","r+");
            			printf("\nEntered Password is %s",usrpasswd);
            			if(strncmp(usrname,"Swades",strlen("Swades"))==0 && strncmp(usrpasswd,"ell785",strlen("ell785"))==0)
            			  {
                			bzero(buf,256);
                			send(newsockfd,"\nAuthenticated\nEnter the name of Student whose marks you want to update:",100,0);
                			n=read(newsockfd,updatename,100); //Student name whose marks is to be updated               
                			i=0;
                			while(i<29)
                			{   
                    				catch=0;
                    				fgets(total,200,fp1);// Reading line by line 
                    				catch=checkusername(total,updatename); //To check student name is in the list
                    				if( catch ==1)
                    					{   
                        					//send(newsockfd,"\nStudent Name Found and Marks updated sucessfully\n",100,0);
                        					send(newsockfd,"\nStudent Name Found in the Record\n",100,0);
                        					bzero(buf,255);
                        					n= read(newsockfd,newline,500);
                        					fseek(fp1, -strlen(total),SEEK_CUR);
                        					strncpy(newline1,newline,200);
                        					chars = fprintf(fp1, "%s",newline1);
                        					printf("\nMarks updated sucessfully\n");
                        					break;
                    					}
                    				i++;
                			}
                			if(i>28)
                      				send(newsockfd,"Student Name Not Found",50,0); 
            			}
            			else
            			{
                			printf("\nAuthentication failed");
                			send(newsockfd,"Authentication Failed",30,0); 
            			}    
	      		     fclose(fp1);
              		     break;

     		case 4 :
            		close(newsockfd);
            		close(sockfd);
	    		exit(0);
    		}/* end of switch case*/
     } /* end of while loop */   
     close(newsockfd);
     close(sockfd);
     return 0; 
} /* End of main loop */
