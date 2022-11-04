#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct
{
	int x, y; // Node position
	void* parent; // Pointer to the parent node
	char c; // Symbol for displaying
	char dirs; //  Directions that have not yet been explored
} Node;

Node* nodes; // Array of nodes
int width, height;  // Dimensions of the maze


int init()
{
	int i, j;
	Node* n;

	// Allocating memory for the maze
	nodes = calloc(width * height, sizeof(Node));
	if (nodes == NULL) return 1;

	// Setting up important nodes
	for (i = 0; i < width; i++)
	{
		for (j = 0; j < height; j++)
		{
			n = nodes + i + j * width;
			if (i * j % 2)
			{
				n->x = i;
				n->y = j;
				n->dirs = 15;  // Suppose it is possible to explore all directions
				n->c = ' ';
			}
			else n->c = '#'; // Adding walls between nodes
		}
	}
	return 0;
}

Node* link(Node* n)
{

	// Connects a node to a random neighbor (if possible) and returns
	//  the address of the next node to be visited

	int x, y;
	char dir;
	Node* dest;

	// Nothing can be done if a null pointer is specified - return
	if (n == NULL) return NULL;

	// While there are unknown directions
	while (n->dirs)
	{

		// Randomly choose one direction	
		dir = (1 << (rand() % 4));

		// If this has already been investigated - try again
		if (~n->dirs & dir) continue;

		// Mark the direction as investigated
		n->dirs &= ~dir;

		// Depending on the selected direction
		switch (dir)
		{
			//Check if it is possible to go right
		case 1:
			if (n->x + 2 < width)
			{
				x = n->x + 2;
				y = n->y;
			}
			else continue;
			break;

			// Checking if it is possible to go down
		case 2:
			if (n->y + 2 < height)
			{
				x = n->x;
				y = n->y + 2;
			}
			else continue;
			break;

			// Checking if it is possible to go left
		case 4:
			if (n->x - 2 >= 0)
			{
				x = n->x - 2;
				y = n->y;
			}
			else continue;
			break;

			// Checking if it is possible to climb
		case 8:
			if (n->y - 2 >= 0)
			{
				x = n->x;
				y = n->y - 2;
			}
			else continue;
			break;
		}

		// Getting the destination node into the pointer
		dest = nodes + x + y * width;

		//  Make sure that the target node is not a wall
		if (dest->c == ' ')
		{
			//  If the addressee is already a connected node, abort
			if (dest->parent != NULL) continue;

			//  Otherwise we accept the node
			dest->parent = n;

			// Removing the wall between the nodes
			nodes[n->x + (x - n->x) / 2 + (n->y + (y - n->y) / 2) * width].c = ' ';

			// Returning the address of the child node
			return dest;
		}
	}

	//  If nothing else can be done here, we return the address of the parent
	return n->parent;
}

void draw()
{
	int i, j;

	// Outputs the maze to the terminal - nothing special
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			printf("%c", nodes[j + i * width].c);
		}
		printf("\n");
	}
}

int main(int argc, char** argv)
{
	Node* start, * last;

	// Checking the number of arguments
	if (argc < 3)
	{
		fprintf(stderr, "%s: please specify maze dimensions!\n", argv[0]);
		exit(1);
	}

	//  Reading the dimensions of the maze from the command line arguments
	if (sscanf(argv[1], "%d", &width) + sscanf(argv[2], "%d", &height) < 2)
	{
		fprintf(stderr, "%s: invalid maze size value!\n", argv[0]);
		exit(1);
	}

	//  Allow only odd sizes
	if (!(width % 2) || !(height % 2))
	{
		fprintf(stderr, "%s: dimensions must be odd!\n", argv[0]);
		exit(1);
	}

	// Do not allow negative dimensions
	if (width <= 0 || height <= 0)
	{
		fprintf(stderr, "%s: dimensions must be greater than 0!\n", argv[0]);
		exit(1);
	}

	// Initial Random Number Generator
	srand(time(NULL));

	// Initialize the maze
	if (init())
	{
		fprintf(stderr, "%s: out of memory!\n", argv[0]);
		exit(1);
	}

	// Setting the start node
	start = nodes + 1 + width;
	start->parent = start;
	last = start;

	// Connect nodes until the initial node is reached and it cannot be left
	while ((last = link(last)) != start);
	draw();
	free(nodes);
}