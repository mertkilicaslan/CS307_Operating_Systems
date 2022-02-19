// Mert Kilicaslan - Programming Assingment 5
#include <stdio.h>
#include <dirent.h>
#include <string.h>

// Stores the each row of database.txt as a struct Person 
struct Person{
    char gender[4];
    char name[256];
    char lastName[256];
};

/* Gets the path of the txt files to be corrected, correct the files in a manner that the
  last name and titles are not matching that of those in the Person struct */
void fixFiles(char* path, struct Person PersonArr[], const int size){
    FILE* fp;

    // If the path is root database.txt or the txt file is not successfully be opened -> skip it
    if ((strstr(path, "./database.txt") != NULL) || (fp = fopen(path, "r+")) == NULL){
        return;
    }

    char buffer[2048];
    while(fscanf(fp, "%2043s", buffer) == 1){

        for(int i = 0; i < size; i++){
            // If the word is a name in the database.txt
            if(strstr(PersonArr[i].name, buffer) != NULL){
                char temp[1024] = {0};

                // File descriptor go backwards to correct title
                fseek(fp, ftell(fp) - strlen(PersonArr[i].name) - 4, SEEK_SET);

                // Concetante the title according to the gender, then the name & lastname
                if(strstr(PersonArr[i].gender, "m") != NULL){
                    strcat(temp, "Mr. ");
                    strcat(temp, PersonArr[i].name);
                    strcat(temp, " ");
                    strcat(temp, PersonArr[i].lastName);
                }

                else if(strstr(PersonArr[i].gender, "f") != NULL){
                    strcat(temp, "Ms. ");
                    strcat(temp, PersonArr[i].name);
                    strcat(temp, " ");
                    strcat(temp, PersonArr[i].lastName);
                } 
                // Overwrite the existed phrase (title name lastname)
                fputs(temp, fp);
            }
        }
    }
    fclose(fp);
}

/* Iterates through the directory it is in and all of the sub-directories
   in order to find the locations of the txt files that are written incorrectly in a manner 
   that the last name and titles are not matching that of those in the database.txt file. */
void listFiles(char* dirname, struct Person PersonArr[], const int size){
    DIR* dir = opendir(dirname);
    struct dirent* entity;

    while((entity = readdir(dir)) != NULL){
        // If it is a txt file, concetante the name with the path then call the fixer function
        if(strstr(entity->d_name, ".txt") != NULL){
            char path[2048] = {0};
            strcat(path, dirname);
            strcat(path, "/");
            strcat(path, entity->d_name);
            fixFiles(path, PersonArr, size); // Call the function to fix the mistakes
        }

        // Concetantes the path until there are no more directory left
        if(entity->d_type == DT_DIR && strcmp(entity->d_name, "..") != 0 && strcmp(entity->d_name, ".") != 0){
            char path[2048] = {0};
            strcat(path, dirname);
            strcat(path, "/");
            strcat(path, entity->d_name);
            listFiles(path, PersonArr, size); // Recursion
        }
    }

    closedir(dir);
}

int main(){

    FILE* dbP; // database.txt file pointer
    struct Person PersonArr[8192]; // Array of Person struct for storing person attributes
    
    if ((dbP = fopen("database.txt", "r")) == NULL){
        printf("Error opening file!\n");
        return -1;
    }

    char buffer[2048] = {0};
    int size = 0;

    // Reading root database.txt & storing attirbutes to the array
    while(fgets(buffer, sizeof(buffer), dbP) != NULL){
        sscanf(buffer, "%s %s %s", PersonArr[size].gender, PersonArr[size].name, PersonArr[size].lastName);
        size++;
    }

    fclose(dbP);
    listFiles(".", PersonArr, size);

    return 0;
}