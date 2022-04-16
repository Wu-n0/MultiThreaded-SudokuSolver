
#include<math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct sgData{
    int value;
    int oldRowNum;
    int oldColumnNum;
};
struct arg_struct { 
	int number;
	int row;
	int column;
	int size;
	int sq;
	};

struct sdkData{
    int rows[36][36];
    int columns[36][36];
    struct sgData subGrids[36][36];
};
struct sdkData sdkInstance;

struct dubPoints {
    int row;
    int column;
};

struct dubPoints dubInstances[36*36];
int dublicateNum = 0;

void* sdkChecker(void* arguments) {
    struct arg_struct *args = arguments;
    int i=0, blockNumber;
    int* result = malloc(sizeof(int));
    *result =0;
    for (i=0; i<(args->size); i++) 
    {
        if (sdkInstance.rows[i][args->column] == args->number) return (void*) result;
        if (sdkInstance.rows[args->row][i] == args->number) return (void*) result;
    }
    blockNumber = (args->row/args->sq)*args->sq + (args->column/args->sq);
    for(i=0; i<(args->size); i++)
    {
        if(sdkInstance.subGrids[blockNumber][i].value == args->number) { return (void*) result; break; }
    }
    *result =1;
    return (void*) result;
}

void storesdkData(int** input,int SUDOKU_SIZE,int sr){
    //we store the values of the rows
    
    for (int row=0; row < SUDOKU_SIZE ; row++){
        for (int column=0; column < SUDOKU_SIZE ; column++){
            sdkInstance.rows[row][column] = input[row][column];
        }
    }
    
    //we store the values of the Columns
    for (int row=0; row < SUDOKU_SIZE ; row++){
        for (int column=0; column < SUDOKU_SIZE ; column++){
            sdkInstance.columns[column][row] = input[row][column];   
        }  
    }
    
    //we store the Sub-Grids
    int colNum[SUDOKU_SIZE],blockNumber=0;
    for(int i=0; i<SUDOKU_SIZE;i++) colNum[i] = 0;
    for (int row=0; row < SUDOKU_SIZE ; row++){
        for (int column=0; column < SUDOKU_SIZE ; column++){
            blockNumber = (row/sr)*sr + (column/sr);
            sdkInstance.subGrids[blockNumber][colNum[blockNumber]].value = input[row][column];
            sdkInstance.subGrids[blockNumber][colNum[blockNumber]].oldRowNum = row;
            sdkInstance.subGrids[blockNumber][colNum[blockNumber]].oldColumnNum = column;
            colNum[blockNumber]++;
        }
    }
}











void storeSDK(int** arr , int SUDOKU_SIZE)
{
    for (int row=0; row < SUDOKU_SIZE ; row++)
    {
        for (int column=0; column < SUDOKU_SIZE ; column++)
        {
            arr[row][column] = sdkInstance.rows[row][column];
        }
        
    }
}


int solveSDK(int row, int column,int SUDOKU_SIZE,int sr) {
    int nextNumber = 1;
    if (row == SUDOKU_SIZE) 
    {
        return 1;
    }
    if(column == SUDOKU_SIZE) return 1;
    if (sdkInstance.rows[row][column]) //checking is the respective cell is empty or not.
    {
        if (column == SUDOKU_SIZE-1) 
        {
            if (solveSDK(row+1, 0,SUDOKU_SIZE,sr)) return 1;
        } 
        else 
        {
            if (solveSDK(row, column+1,SUDOKU_SIZE,sr)) return 1;
        }
        return 0;
    }
    struct arg_struct args[SUDOKU_SIZE+1];
    pthread_t th[SUDOKU_SIZE+1];
    for (int i=0; nextNumber<SUDOKU_SIZE+1; nextNumber++) 
    {
    	args[i].number = nextNumber;
    	args[i].row = row;
    	args[i].column  = column;
    	args[i].size = SUDOKU_SIZE;
    	args[i].sq = sr;
    	if ( pthread_create(&th[i],NULL,&sdkChecker,(void*)&args[i]) != 0){
    	   return -1;
    	   }
    	   i++;
    	   }
    	   nextNumber = 1;
    	for (int i=0; nextNumber<SUDOKU_SIZE+1; nextNumber++) {   
    	int* value;
    	pthread_join(th[i],(void**) &value);
    	
        if(*value) 
        {
            sdkInstance.rows[args[i].row][args[i].column] = args[i].number;
            if (column == SUDOKU_SIZE-1) 
            {
                if (solveSDK(args[i].row+1, 0,SUDOKU_SIZE,sr)) return 1;
            } 
            else 
            {
                if (solveSDK(args[i].row, args[i].column+1,SUDOKU_SIZE,sr)) return 1;
            }
            sdkInstance.rows[args[i].row][args[i].column] = 0;
        }
        free(value);
        i++;
    }
    return 0;
}

void read_grid_from_file(int size, char *ip_file, int** grid) {
	FILE *fp;
	int i, j;
	fp = fopen(ip_file, "r");
	for (i=0; i<size; i++) 
		for (j=0; j<size; j++) {
			fscanf(fp, "%d", &grid[i][j]);
	}
	fclose(fp);
} 


void print_grid(int size, int** grid) {
	int i, j;
	/* The segment below prints the grid in a standard format. Do not change */
	for (i=0; i<size; i++) {
		for (j=0; j<size; j++)
			printf("%d\t", grid[i][j]);
		printf("\n");
	}
	}

int main (int argc, char *argv[]){
    
    int  size, i, j;
    
    if (argc != 3) {
		printf("Usage: ./sudoku.out grid_size inputfile");
		exit(-1);
	}
    size = atoi(argv[1]);
    int sr = (int)sqrt(size);
    int **grid = (int **)malloc(size * sizeof(int *));
    for (int r = 0; r < size; r++) {
        grid[r] = (int *)malloc(size * sizeof(int));
    }
    read_grid_from_file(size, argv[2], grid);       
            storesdkData(grid,size,sr);
            if (solveSDK(0, 0,size,sr)) 
            {
                
                storeSDK(grid,size); 
                
            } 
            else {
                printf("Solver failed!");
            }
            
        
        print_grid(size, grid);
        for (int i = 0; i < size; i++) {
        free(grid[i]);
    }
    free(grid);
        
    return 1;
}
