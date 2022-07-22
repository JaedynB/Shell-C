/*

Name: Jaedyn Brown
ID: 1001409998
*/
// The MIT License (MIT)
// 
// Copyright (c) 2016, 2017, 2021 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// 7f704d5f-9811-4b91-a918-57c1bb646b70
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 11     // Mav shell only supports five arguments

int main()
{

  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
  int list_pids[15];//int array to store up to 15 process pids
  int pid_counter = 0;//counter to keep track of how many pids have been entered in our array
  char history[15][MAX_COMMAND_SIZE];//char array to store the last 15 commands the user's entered
  int history_counter = 0;
  bool history_set = false;//keep track of whether !n command is within an acceptable range
  int val;
  char command_checker[27][10] = {"ls", "cd", "history", "echo", "touch", "showpids", "listpids", "exit", "mkdir", "quit", "!0",
  "!1","!2","!3","!4","!5","!6","!7","!8","!9","!10","!11","!12","!13","!14","!15"};//hard coded all commands I expect our shell should be able to execute to check if command is found
  bool command_found = false;//bool to track if user inputted command is acceptable
  while( 1 )
  {
    // Print out the msh prompt
    if(history_set != true)//if user issued !n command, we don't want to print the msh> prompt if it's accepted
      printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    if(history_set != true)//if user issued acceptable !n command, we don't want to have user input new info in this loop
      while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) ); //whole line stored in cmd_str
    
    /* Parse input */
    if(strlen(cmd_str) >= 2)
    { //requirement 2, make sure that the user actually tried to issue a command
    char *token[MAX_NUM_ARGUMENTS];
    if(history_set != true)//if !n was issued, we don't want to put another copy of the !n command in history
    {
    strcpy(history[history_counter], cmd_str);
    history_counter++;
    }

    int token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;     
    char *working_str;
    char *working_root;                                    
    //duplicate string cmd_str to working_str
    if(history_set == true){//if !n is accepted, we want to replace cmd_str's input with the last command, instead of new user input
      strcpy(cmd_str, history[val]);
      working_str  = strdup( cmd_str ); 

      working_root = working_str;
      history_set = false;
    }
    else
    {
      working_str  = strdup( cmd_str );  

      working_root = working_str;
    }
               
    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end

    // Tokenize the input strings with whitespace used as the delimiter
    while ( ( (argument_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }
    int z;
    for(z = 0; z < 27; z++)
    {
      if((strcmp(token[0], command_checker[z]) == 0)) //checking if user issued command is recognized
        command_found = true;
    }
    if((command_found == false) && (history_set == false))//A special case if user typed a !n command that was not acceptable, would print command not in history, and command not found
    {
      continue; //skip everything else up until beginning of while(1) loop to reprint msh> prompt
    }else if((command_found == false))
    {
      printf("%s: Command not found.\n", token[0]);
      continue;
    }

    if (strcmp(token[0], "cd") == 0)
    {
      chdir(token[1]);
      continue; //use continue to skip the fork, or we will lose our change of directory
    }
    else if (((strcmp(token[0], "exit") == 0) || (strcmp(token[0], "quit") == 0)))
    {
      return 0;
    }
    else if (strcmp(token[0], "history") == 0)
    {
      int j;
      for(j = 0; j < history_counter; j++)
        printf("%d: %s", j, history[j]);
      continue; //use continue to skip the fork, or we will end up with a duplicate list of the history of commands
    }
    else if (strncmp(token[0], "!", 1) == 0)//want to only identify if first character equals !
    {

      int strlen_check = strlen(history[history_counter-1]); //used to check if we have a double digit number or not

      if(strlen_check == 4)//special case if user types command with double digits
      {

        int one_check = (int) history[history_counter-1][1];
 
        if(one_check == 49)//if the first digit is a 1
        {

          val = (int) history[history_counter-1][2];//get second digit

          val = 10 + (val - 48); //subtract 48 for ascii of char 0, add 10 for first digit
  
        }
      }
      else
      {
        val = (int) history[history_counter-1][1];
        val = 48 - val;
        val = val / -1;
      }
      if((val > history_counter) || (val > 15))
      {
        printf("Command not in history. \n");
        continue;
      }
      history_set = true;//set to true so that we will skip msh> prompt, and user input
      continue;
    }
    
    // Now print the tokenized input as a debug check

    pid_t pid = fork();
    if((pid_counter == 15)) //if reached cap of pid_counter, loop back to 0 to overwrite previous stores
    {
      pid_counter = 0;
    }
    if((pid_counter == 0))//need to catch the first pid of the program
    {
      list_pids[0] = getpid();
      pid_counter++;
    }
      int temp = getpid();
      if((temp != list_pids[pid_counter-1]) && (temp != 0))//comparing last stored pid to current pid, to make sure duplicate pids are not stored
      {
        list_pids[pid_counter] = temp;
        pid_counter++;
      }

    if( pid == 0 )
    {
      int i;
      for(i = 0; i < token_count; i++)
      {

        if (((strcmp(token[0], "showpids") == 0) || (strcmp(token[0], "listpids") == 0)))
    {
        int j;

        for(j = 0; j < pid_counter; j++)
        {
          printf("%d: %d\n", j, list_pids[j]);
        }
          exit( EXIT_SUCCESS );
          fflush(NULL);
    }

    else
    {
        int ret = execvp( token[i], &token[i] );

          if( ret == -1 )
          {
            perror("failed");
          }
    }
    }
    }
    else if (pid < 0)
    {
      perror("Fork failed");

      exit(1);
    }
    else
  {
    int status;
    wait( & status );
  }

    free( working_root );

    }
  }
  return 0;
  }


