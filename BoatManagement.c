#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef char String[255];

//define maximum length of string, which will be type defined later on
#define ARRAY_LENGTH 120

//define method to get the size of an array
#define LENGTH(A) sizeof(A)/sizeof(A[0])

//definition of method for mallocing data
void* Malloc(size_t Size)
{
    void* Memory;
    if((Memory = malloc(Size)) == NULL)
    {
        perror("Cannot malloc");
        exit(EXIT_FAILURE);
    }
    else
    {    
        return(Memory);
    }    
}

//definition of method for freeing data
void Free(void** Memory)
{
    free(*Memory); 
    *Memory = NULL;
}

//define an enum for the place where the boat is stored
typedef enum {
    slip,
    land,
    trailor,
    storage,
    no_place
} PlaceType;

//define a union for the extra information of each place where a boat can be stored
typedef union {
    int slipID; 
    char landID; 
    String trailorID;
    int storageID;
} BoatPlaceIDType;

//define the struc to hold each boat's data
typedef struct Boat{
    String boatName;
    int boatLength;    
    PlaceType storagePlace;
    BoatPlaceIDType boatPlaceID; 
    double amountOwed;
} BoatInfoType;

//define a pointer to the struct defined above
typedef BoatInfoType* BoatInfoTypePointer /*= (BoatInfoTypePointer)Malloc(sizeof(BoatInfoType))*/;

//define an array made of pointers to structs
typedef BoatInfoTypePointer PointerArray[120]/* =(BoatInfoTypePointer)Malloc(sizeof(BoatInfoTypePointer)*120)*/;

//definition of functions that go with enum:
//1) Convert a string to a place
PlaceType StringToPlaceType(char * PlaceString) {

    if (!strcasecmp(PlaceString,"slip")) {
        return(slip);
    }
    if (!strcasecmp(PlaceString,"land")) {
        return(land);
    }
    if (!strcasecmp(PlaceString,"trailor")) {
        return(trailor);
    }
    if (!strcasecmp(PlaceString,"storage")) {
        return(storage);
    }
    return(no_place);
}

//2)Convert a place to a string
char * PlaceToString(PlaceType Place) {

    switch (Place) {
        case slip:
            return("slip");
        case land:
            return("land");
        case trailor:
            return("trailor");
        case storage:
            return("storage");
        case no_place:
            return("no_place");
        default:
            printf("How the faaark did I get here?\n");
            exit(EXIT_FAILURE);
            break;
    }
}

//definition of functions that go with union:
//1) Convert a string to each value in the union
void StringToPlaceIDType(BoatInfoTypePointer boatPointer, String inputString)
{
    switch (boatPointer->storagePlace)
    {
        case slip:
            boatPointer->boatPlaceID.slipID = atoi(inputString);
            break;
        case land:
            boatPointer->boatPlaceID.landID = inputString[0];
            break;
        case trailor:
            strcpy(boatPointer->boatPlaceID.trailorID, inputString);
            break;
        case storage:
            boatPointer->boatPlaceID.storageID = atoi(inputString);
            break;
	case no_place:
        default:
	    break;
    }
}

//2) Convert each union value to a string
char * PlaceIDToString(BoatInfoTypePointer boatPointer, String returnString) {
    switch (boatPointer->storagePlace) {
        case slip:
            returnString = Malloc(3);
            sprintf(returnString, "%d", boatPointer->boatPlaceID.slipID);
            return returnString;
        case land:
            returnString = Malloc(2);
            returnString[1] = '\0';
            returnString[0] = boatPointer->boatPlaceID.landID;
            return returnString;
        case storage:
            returnString = Malloc(3);
            sprintf(returnString, "%d", boatPointer->boatPlaceID.slipID);
            return returnString;
        default:
            returnString = Malloc(strlen(boatPointer->boatPlaceID.trailorID));
            strcpy(returnString, boatPointer->boatPlaceID.trailorID);
            return returnString;
    }
}

//declaration of methods to be used
void LoadData(PointerArray arrayOfPointers, FILE* inputFile,int *sizeOfArray);
void Menu(PointerArray arrayOfPointers, int* sizeOfArray, FILE* inputFile);
void PrintInventory(PointerArray arrayOfPointers, int* sizeOfArray);
void AddBoatInfo(PointerArray arrayOfPointers, int* sizeOfArray);
void RemoveBoatByName(PointerArray arrayOfPointers, int* sizeOfArray);
void MakePayment(PointerArray arrayOfPointers, int* sizeOfArray);
void UpdateDebtForMonth(PointerArray arrayOfPointers, int* sizeOfArray);
void ExitFunction(PointerArray arrayOfPointers, int* sizeOfArray, FILE* inputFile);

//main method
int main(int argc, char* argv[])
{
    FILE* inputFile;
    FILE* outputFile;
    int sizeOfArray;
    PointerArray arrayOfPointers;
 
   //if the user does not enter two command line arguments (CLA), exit the program 
   if(argc != 2)
    {
        printf("ERROR: File name not given\n");
        return(EXIT_FAILURE);
    }

    inputFile=fopen(argv[1], "r");
    if(inputFile ==NULL)
    {
        printf("Error opening file\n");
    }
    //load the data from the user's file into structs that will be pointed to by array elements 
    LoadData(arrayOfPointers,inputFile, &sizeOfArray);
    fclose(inputFile);
    outputFile = fopen(argv[1], "w");
  
    //send the output file to the menu so it can be passed to the exit function
    Menu(arrayOfPointers, &sizeOfArray, outputFile);
    return(EXIT_SUCCESS);
}

//method to load data from a csv file
void LoadData(PointerArray arrayOfPointers, FILE* inputFile, int *sizeOfArray)
{
    int bufferLength = 255;
    char buffer[bufferLength];
    int counter = 0;
    BoatInfoTypePointer boatPointer;
    String inputForUnion;
    
    //read in each line from the file and create the struct by filling in each attribute
    while(fgets(buffer, bufferLength, inputFile))
    {
        //malloc space for each new pointer to a struct
        boatPointer = (BoatInfoTypePointer)Malloc(sizeof(BoatInfoType));
        //tokenize each portion of the file input and add it to each attribute of a boat
        strcpy(boatPointer->boatName, strtok(buffer, ","));
        boatPointer->boatLength = atoi(strtok(NULL, ","));
        boatPointer->storagePlace = StringToPlaceType(strtok(NULL, ","));
        strcpy(inputForUnion, strtok(NULL, ","));
        StringToPlaceIDType(boatPointer, inputForUnion);
        boatPointer->amountOwed = atof(strtok(NULL, ","));
        //add the pointer to the newly created struct into the array of pointers
        arrayOfPointers[counter] = boatPointer;
        counter++;
     }
     //update the size of the array based on the number of pointers added
     *sizeOfArray = counter;
     //close the file
     fclose(inputFile);
}

//menu of options for the user
void Menu(PointerArray arrayOfPointers, int* sizeOfArray, FILE* outputFile)
{
    String userInput;
    //loop continues while user does not enter 'x' for the exit funtion 
    do
    {
        printf("(I)nventory, (A)dd, (R)emove, (P)ayment, (M)onth, e(X)it : ");
        fgets(userInput,10, stdin);
    
        switch(tolower(userInput[0]))
        {
            case 'i':
                PrintInventory(arrayOfPointers, sizeOfArray);
                break;
            case 'a':
                AddBoatInfo(arrayOfPointers, sizeOfArray);
                break;
            case 'r':
                RemoveBoatByName(arrayOfPointers, sizeOfArray);
                break;
            case 'p':
                MakePayment(arrayOfPointers, sizeOfArray);
                break;
            case 'm':
                UpdateDebtForMonth(arrayOfPointers, sizeOfArray);
                break;
            case 'x':
                break;
            //default handles case where user enters and invalid option  
            default:
                printf("Invalid option\n");
        }
    }while(tolower(userInput[0]) != 'x');

    //option 'x' calls the exit function
    if(strcasecmp(userInput,"x"))
    {   
        ExitFunction(arrayOfPointers, sizeOfArray, outputFile);
    }
}

//method to sort the boats by alphabetical order, ie sort the pointers based on the boat's name
void SortPointers(PointerArray arrayOfPointers, int* sizeOfArray)
{
    int i;
    BoatInfoTypePointer Temporary;
    int Swapped = 1;
  
    while(Swapped)
    {
        Swapped=0;
        //loop to compare ith & (i+1)st boat names to swap the pointers using bubble sort
        for(i=0; i<*sizeOfArray-1;i++)
        {   
            if(strcmp(arrayOfPointers[i]->boatName, arrayOfPointers[i+1]->boatName)>0)
            {   
                Temporary = arrayOfPointers[i];
                arrayOfPointers[i] = arrayOfPointers[i+1];
                arrayOfPointers[i+1] = Temporary;
                Swapped = 1;
            }
        }
    }
}

//method to print the information of each boat (struct)
void PrintInventory(PointerArray arrayOfPointers, int* sizeOfArray)
{
    int i;
    //call sorting function to sort pointers in array by alphabetical struct name attributes 
    SortPointers(arrayOfPointers, sizeOfArray);
           
    //loop to print out all info from structs
    for(i=0; i<*sizeOfArray;i++)
    {
        printf("%-22s", arrayOfPointers[i]->boatName);
        
        //printout of boat length
        printf("%d\' ", arrayOfPointers[i]->boatLength);
        
        //printout of boat storage place
        if(arrayOfPointers[i]->storagePlace == slip)
        { 
            printf("%8s  ", PlaceToString(slip));
        }
        if(arrayOfPointers[i]->storagePlace == land)
        { 
            printf("%8s  ", PlaceToString(land));
        }
        if(arrayOfPointers[i]->storagePlace == trailor)
        { 
            printf("%8s  ", PlaceToString(trailor));
        }
        if(arrayOfPointers[i]->storagePlace == storage)
        { 
            printf("%8s  ", PlaceToString(storage));
        }
      
        //printout for boat storage place ID
        switch (arrayOfPointers[i]->storagePlace)
        {    
            case slip:
                printf("  #%3d", arrayOfPointers[i]->boatPlaceID.slipID);
                break;
            case land:
                printf("     %c", arrayOfPointers[i]->boatPlaceID.landID);
                break;
            case trailor:
                printf("%-6s", arrayOfPointers[i]->boatPlaceID.trailorID);
                break;
            case storage:
                printf("  #%3d", arrayOfPointers[i]->boatPlaceID.storageID);
                break;
        }
        //printout for amount owed
        printf("   Owes $%7.2lf\n", arrayOfPointers[i]->amountOwed);
    }
}

//method to add a boat to the array
void AddBoatInfo(PointerArray arrayOfPointers, int* sizeOfArray)
{
    BoatInfoTypePointer* pointerOfPointers = arrayOfPointers;
    BoatInfoTypePointer boatPointer;
    String userInput;
    printf("Please enter the boat data in CSV format                 : ");
    fgets(userInput,255, stdin);
    userInput[strlen(userInput)-1] = '\0';
    //malloc space for a new pointer to a struct
    boatPointer = (BoatInfoTypePointer)Malloc(sizeof(BoatInfoType));
    //tokenize each portion of the user input and add it to each attribute of a boat
    strcpy(boatPointer->boatName, strtok(userInput, ","));
    boatPointer->boatLength = atoi(strtok(NULL, ","));
    boatPointer->storagePlace = StringToPlaceType(strtok(NULL, ","));
    StringToPlaceIDType(boatPointer, strtok(NULL, ","));
    boatPointer->amountOwed = atof(strtok(NULL, ","));
    //add the new pointer to the array
    *(pointerOfPointers + *sizeOfArray) = boatPointer; //pointerOfPointers[*sizeOfArr]=boatPtr
    //increase the size of the array
    (*sizeOfArray)++;
}

//method to remove a boat chosen by user
void RemoveBoatByName(PointerArray arrayOfPointers, int* sizeOfArray)
{
    int i = 0;
    String userInput;
    printf("Please enter the boat name                               : ");
    fgets(userInput,255, stdin);
    userInput[strlen(userInput)-1] = '\0';
    //while the end of array isn't reached & user input doesn't match the boat name, increase index
    while(arrayOfPointers[i] != NULL && strcasecmp(userInput, arrayOfPointers[i]->boatName))
    {
        i++;
    } 
    //if the pointer at index is not null, free the pointer and decrease size of array
    if(arrayOfPointers[i] != NULL)
    {
        Free(&arrayOfPointers[i]);
        (*sizeOfArray)--;
        //shift each pointer
        do
        {
            arrayOfPointers[i] = arrayOfPointers[i+1];
            i++;
        }while(arrayOfPointers[i] != NULL);
    }
    //if user's input does not match any of the names in the structs, ignore the input
    else
    {
        printf("No boat with that name\n");
        return;
    }
}

void MakePayment(PointerArray arrayOfPointers, int* sizeOfArray)
{
    BoatInfoTypePointer* pointerOfPointers = arrayOfPointers;
    String userInputForName;
    double userInputForAmount;
    printf("Please enter the boat name                               : ");
    fgets(userInputForName,255, stdin);
    userInputForName[strlen(userInputForName)-1] = '\0';
    //while the end of array isn't reached & user input doesn't match the boat name, move pointer
    while((*pointerOfPointers)!=NULL &&strcasecmp(userInputForName,(*pointerOfPointers)->boatName)) 
    {
        pointerOfPointers++;
    }
    //if the pointer at index is not null, decrease the amount by user's input
    if((*pointerOfPointers) != NULL)
    {
        printf("Please enter the amount to be paid                       : ");
        scanf("%lf", &userInputForAmount);
        while(getchar() != '\n');
        if(userInputForAmount>(*pointerOfPointers)->amountOwed)
        {
            //if the user's entry exceeds the amount owed, ignore the input
            printf("That's more than amount owed, $%.2lf\n",(*pointerOfPointers)->amountOwed);
            return;
        }
        //reduce amountOwed attribute by amount to be paid
        (*pointerOfPointers)->amountOwed -= userInputForAmount;
    }
    //boat name DNE action not completed
    else
    {
        printf("No boat with that name\n");
        return;
    }
}

//method to update the amout owed by each boat
void UpdateDebtForMonth(PointerArray arrayOfPointers, int* sizeOfArray)
{
    int i;
    //loop through PointerArray
    for(i=0; i<(*sizeOfArray); i++)
    {
        //switch statement for each type of storage place
        switch(arrayOfPointers[i]->storagePlace)
        {
            //for each storage update amount owed attribute as += price/ft/mo x length attribute
            case slip:
                arrayOfPointers[i]->amountOwed += 12.50*arrayOfPointers[i]->boatLength;
                break;
            case land:
               arrayOfPointers[i]->amountOwed += 14*arrayOfPointers[i]->boatLength;
               break;
            case trailor:
               arrayOfPointers[i]->amountOwed += 25*arrayOfPointers[i]->boatLength;
               break;
            case storage:
              arrayOfPointers[i]->amountOwed += 11.20*arrayOfPointers[i]->boatLength;  
              break;
        }
    }
}

//method to exit the program and add any changes to the input file (same as output file)
void ExitFunction(PointerArray arrayOfPointers, int* sizeOfArray, FILE* outputFile)
{
    int i;
    char * b4Last = "%s,%d,%s,%s,%.2lf\n" ;
    char * last = "%s,%d,%s,%s,%.2lf" ;
    String unionOutput;
    //loop through PointerArray and send output to the output file
    for(i=0; i<(*sizeOfArray); i++)
    {
	BoatInfoTypePointer boat = arrayOfPointers[i];
        fprintf(outputFile, (i < (*sizeOfArray) ? b4Last : last), \
boat->boatName, boat->boatLength, \
PlaceToString(boat->storagePlace), \
PlaceIDToString(boat, unionOutput), boat->amountOwed);
    } 
    //close the file
    fclose(outputFile);
    printf("Exiting the Boat Management System\n");
    //free each pointer
    for(i=0; i<=(*sizeOfArray); i++)
    {
        Free(&arrayOfPointers[i]);
    }
}
