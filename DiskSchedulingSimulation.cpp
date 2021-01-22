/***********************************************************************
 * File: program5.cpp
 * Author: Logan Wheat
 * Modification History: 11/17/19 : wrote the code: Logan Wheat
 * struct:
 *point - stores data such as track, sector, and whether it has been accessed or not
 *
 * Procedures:
 *main - driver function of program. Generates random input streams with no duplicates
 *for each experiment, runs each of disk the scheduling algorithms per experiment
 *properly storing the average seek times, and outputs the average seek times
 *calculated over the 1000 experiments for sizes 500-1000 to both standard out
 *and a .csv file to properly generate line graph for report.
 *
 *uniform - uniform random number generator that returns an int between perameters.
 *
 *swap - function to aid bubble sort by swapping points passed by reference.
 *
 *sortRequest - sorts passed request of points utilizing bubble sort (was
 *having issues with qsort).
 *
 *outputRequest - function to output array of points, used mostly for
 *debugging. Not called during main, but kept for useful tool.
 *
 *checkSector - function called to ensure sector value stays in range 0-12000,
 *essentially simulates the disk completing a rotation.
 *
 *fifo - simulates a first in first out disk scheduling algorithm.
 *
 *sstf - simulates a shortest service time first disk scheduling algorithm.
 *
 *scan - simulates a scan disk scheduling algorithm.
 *
 *cscan - simulates a circular scan disk scheduling algorithm.
 ***********************************************************************/
#include <iostream>
#include <cstdlib> // for use of rand()
#include <cmath> // for use of abs()
#include <fstream> // to output to .csv file

using namespace std;

/***************************************************************************
 * struct point
 * Author: Logan Wheat
 * Date: 15 November 2019
 * Description: struct to store points, storing data such as track, sector,
 *and whether it has been accessed or not. Not sure if I needed to 
 *block code above it, but I thought I should considering how crucial
 *it is to the overall code.
 **************************************************************************/
struct point
{
  double track;
  double sector;
  bool accessed;
};

/***************************************************************************
 * int uniform(int, int)
 * Author: Richard Goodrum
 * Date: 3 November 2019
 * Description: uniform random number generator
 *
 * Parameters:
 * lo I/P int the lower bound of random number range
 * hi I/P int the upper bound of random number range
 **************************************************************************/
int uniform(int lo, int hi)
{
  int x;
  int y = hi - lo + 1;
  int z = RAND_MAX / y;

  while (y <= (x = (rand() / z)));

  return x + lo;
}

/***************************************************************************
 * void swap(point *, point*)
 * Author: Logan Wheat
 * Date: 17 November 2019
 * Description: swaps the perameters, utilized in bubble sort algorithm
*
* Parameters:
* x I/P point*  the first point passed to be swapped with the second
* y I/P point*  the second point passed to be swapped with the first
**************************************************************************/
void swap(point *x, point *y)
{
  point temp = *x;
  *x = *y;
  *y = temp;
}

/***************************************************************************
 * void sortRequest(point[], int)
 * Author: Logan Wheat
 * Date: 17 November 2019
 * Description: sorts the array of points utilizing a bubble sort algorithm
 *
 * Parameters:
 * request I/P point[]  array of points to be sorted (sorted by track in ascending order)
 * size I/P int  size of array that is to be sorted
 **************************************************************************/
void sortRequest(point request[], int size)
{
  int i, j;
  for (i = 0; i < size - 1; i++)
    {
      // Last i elements are already in place  
      for (j = 0; j < size - i - 1; j++)
	{
	  if (request[j].track > request[j + 1].track)
	    swap(&request[j], &request[j + 1]);
	}
    }
}

/***************************************************************************
 * void outputRequest(point[], int)
 * Author: Logan Wheat
 * Date: 15 November 2019
 * Description: outputs all the track and sector pairs of a point array size(requestSize)
 *
 * Parameters:
 * request I/P point[]  array of points to be sent to standard output
 * requestSize I/P int  The size of the array to be output
 **************************************************************************/
void outputRequest(point request[], int requestSize)
{
  cout << "Track\tSector" << endl;
  for (int i = 0; i < requestSize; i++)
    {
      cout << i << ": " << request[i].track << "\t" << request[i].sector << endl;
    }
}

/***************************************************************************
 * double checkSector(double)
 * Author: Logan Wheat
 * Date: 16 November 2019
 * Description: simulates disk completing a rotation by ensuring sector 
 *stays in the range of 0-12000.
 *
 * Parameters:
 * sector I/O double  sector passed to ensure stays within range, returned to function
 **************************************************************************/
double checkSector(double sector)
{
  if (sector < 12000)
    {
      return (sector + 12000);
    }
  else if (sector > 12000)
    {
      return (sector - 12000);
    }
  return sector;
}

/***************************************************************************
 * double fifo(point[], int)
 * Author: Logan Wheat
 * Date: 15 November 2019
 * Description: simulates a first in first out disk scheduling algorithm.
 *Calculates the total time spent seeking and then returns
 *(total time / request size) to calculate average seek time between
 *requests in milliseconds.
 *
 * Parameters:
 * request I/P point[]  array of points that will be accessed in fifo algorithm
 * requestSize I/P int  size of array of points, that way function does not exceed bounds
**************************************************************************/
double fifo(point request[], int requestSize)
{
  double totalTime = 0; // initialize total time to zero in milliseconds

  double currentTrack = 2499; // start in the middle of the track
  double currentSector = 0; // start at sector 0
  double trackDistance; // declare track distance as a double

  // for loop to iterate through all request and simulate fifo
  for (int i = 0; i < requestSize; i++)
    {
      // track distance will be used to determine new current sector
      trackDistance = abs(currentTrack - request[i].track);
      currentTrack = request[i].track; // set current track as the requested track

      // determine if snapback feature should be used
      if ((trackDistance*.0008) < (1.5 + (currentTrack * .0008))) // if snapback is not faster (slower)
	{
	  totalTime += (trackDistance * .0008); // add 800 nanoseconds for every track crossed
	}
      else // if snapback is faster
	{
	  totalTime += (1.5 + (currentTrack * .0008));
	}

      // hdd is spinning counter clockwise
      currentSector += ((trackDistance * .0008) / .0005); // calculate current sector as 1 sector is passed every 500 nanoseconds
      currentSector = checkSector(currentSector); // call to function that makes sure sector is in range

      if (currentSector > request[i].sector) // if requested sector is before current sector and requires another rotation
	{
	  totalTime += abs(((12000 - (currentSector - request[i].sector)) * .0005)); // add 500 nanoseconds for every sector crossed
	}
      else // if requested sector is further on the track or in the correct spot
	{
	  totalTime += abs(((currentSector - request[i].sector) * .0005)); // add 500 nanoseconds for every sector crossed
	}

      currentSector = request[i].sector; // set current sector to current request
    } // end of fifo for loop

  // return the average seek time in milliseconds
  return (totalTime / requestSize);
}

/***************************************************************************
 * double sstf(point[], int)
 * Author: Logan Wheat
 * Date: 17 November 2019
 * Description: simulates a shortest service time first disk scheduling algorithm.
 *Calculates the total time spent seeking and then returns
 *(total time / request size) to calculate average seek time between
 *requests in milliseconds.
 *
 * Parameters:
 * request I/P point[]  array of points that will be accessed in sstf algorithm
 * requestSize I/P int  size of array of points, that way function does not exceed bounds
 **************************************************************************/
double sstf(point request[], int requestSize)
{
  double totalTime = 0; // initialize total time to 0 in milliseconds

  double currentTrack = 2499; // start in the middle of the track
  double currentSector = 0; // start at sector 0
  double trackDistance; // declare track distance as a double
  int closestTrack; // declare closest track as an int
  double currentClosest; // declare current closest as a double

  // for loop to iterate through all request and simulate sstf
  for (int i = 0; i < requestSize; i++)
    {
      currentClosest = 5000; // define current closest as the furthest possible distance

      // for loop to search through sorted list for closest track
      for (int n = 0; n < requestSize; n++)
	{
	  trackDistance = abs(currentTrack - request[n].track);

	  if (trackDistance < currentClosest && request[n].accessed == false) // if current location is closest and has not been accessed, save as closest
	    {
	      currentClosest = trackDistance; // update closest track
	      closestTrack = n; // save index of closest track
	    }
	} // end of linear search

      trackDistance = currentClosest; // update track distance to reuse fifo code
      currentTrack = request[closestTrack].track; // update current track to closest track
      request[closestTrack].accessed = true; // update that this point has been accessed already

      // determine if snapback feature should be used
      if ((trackDistance*.0008) < (1.5 + (currentTrack * .0008))) // if snapback is not faster (slower)
	{
	  totalTime += (trackDistance * .0008); // add 800 nanoseconds for every track crossed
	}
      else // if snapback is faster
	{
	  totalTime += (1.5 + (currentTrack * .0008));
	}

      // hdd is spinning counter clockwise
      currentSector += ((trackDistance * .0008) / .0005); // calculate current sector as 1 sector is passed every 500 nanoseconds
      currentSector = checkSector(currentSector); // call to function that makes sure sector is in range

      if (currentSector > request[closestTrack].sector) // if requested sector is before current sector and requires another rotation
	{
	  totalTime += abs(((12000 - (currentSector - request[closestTrack].sector)) * .0005)); // add 500 nanoseconds for every sector crossed
	}
      else // if requested sector is further on the track or in the correct spot
	{
	  totalTime += abs(((currentSector - request[closestTrack].sector) * .0005)); // add 500 nanoseconds for every sector crossed
	}

      currentSector = request[closestTrack].sector; // set current sector to current request
    } // end of sstf for loop

  // return average seek time in milliseconds
  return (totalTime / requestSize);
}

/***************************************************************************
 * double scan(point[], int)
 * Author: Logan Wheat
 * Date: 19 November 2019
 * Description: simulates a scan scheduling algorithm.
 *Calculates the total time spent seeking and then returns
 *(total time / request size) to calculate average seek time between
 *requests in milliseconds.
 *
 * Parameters:
 * request I/P point[]  array of points that will be accessed in scan algorithm
 * requestSize I/P int  size of array of points, that way function does not exceed bounds
 **************************************************************************/
double scan(point request[], int requestSize)
{
  double totalTime = 0; // initialize total time to 0 in milliseconds

  double currentTrack = 2499; // start in the middle of the track
  double currentSector = 0; // start at sector 0
  double trackDistance; // declare track distance as a double
  int closestTrack; // declare closest track as an int
  double currentClosest; // declare current closest as a double

  currentClosest = 5000; // define current closest as the furthest possible distance

  // for loop to search through sorted list for closest track
  for (int n = 0; n < requestSize; n++)
    {
      trackDistance = abs(currentTrack - request[n].track);

      if (trackDistance < currentClosest && request[n].accessed == false && request[n].track >= 2499) // if current location is closest, has not been accessed, and is greater than or equal to closest track save as closest
	{
	  currentClosest = trackDistance; // update closest track
	  closestTrack = n; // save index of closest track
	}
    } // end of linear search

  int firstClosest = closestTrack; // store first closest to properly simulate scan

  // for loop to iterate through request from closest index and then upwards and simulate scan
  for (int i = closestTrack; i < requestSize; i++)
    {
      // track distance will be used to determine new current sector
      trackDistance = abs(currentTrack - request[i].track);
      currentTrack = request[i].track; // set current track as the requested track

      // determine if snapback feature should be used
      if ((trackDistance*.0008) < (1.5 + (currentTrack * .0008))) // if snapback is not faster (slower)
	{
	  totalTime += (trackDistance * .0008); // add 800 nanoseconds for every track crossed
	}
      else // if snapback is faster
	{
	  totalTime += (1.5 + (currentTrack * .0008));
	}

      // hdd is spinning counter clockwise
      currentSector += ((trackDistance * .0008) / .0005); // calculate current sector as 1 sector is passed every 500 nanoseconds
      currentSector = checkSector(currentSector); // call to function that makes sure sector is in range

      if (currentSector > request[i].sector) // if requested sector is before current sector and requires another rotation
	{
	  totalTime += abs(((12000 - (currentSector - request[i].sector)) * .0005)); // add 500 nanoseconds for every sector crossed
	}
      else // if requested sector is further on the track or in the correct spot
	{
	  totalTime += abs(((currentSector - request[i].sector) * .0005)); // add 500 nanoseconds for every sector crossed
	}

      currentSector = request[i].sector; // set current sector to current request
    } // end of first scan for loop

  // for loop to iterate downwards through the request, simulating the head coming back on a scan
  for (int i = firstClosest - 1; i >= 0; i--)
    {
      // track distance will be used to determine new current sector
      trackDistance = abs(currentTrack - request[i].track);
      currentTrack = request[i].track; // set current track as the requested track

      // determine if snapback feature should be used
      if ((trackDistance*.0008) < (1.5 + (currentTrack * .0008))) // if snapback is not faster (slower)
	{
	  totalTime += (trackDistance * .0008); // add 800 nanoseconds for every track crossed
	}
      else // if snapback is faster
	{
	  totalTime += (1.5 + (currentTrack * .0008));
	}

      // hdd is spinning counter clockwise
      currentSector += ((trackDistance * .0008) / .0005); // calculate current sector as 1 sector is passed every 500 nanoseconds
      currentSector = checkSector(currentSector); // call to function that makes sure sector is in range

      if (currentSector > request[i].sector) // if requested sector is before current sector and requires another rotation
	{
	  totalTime += abs(((12000 - (currentSector - request[i].sector)) * .0005)); // add 500 nanoseconds for every sector crossed
	}
      else // if requested sector is further on the track or in the correct spot
	{
	  totalTime += abs(((currentSector - request[i].sector) * .0005)); // add 500 nanoseconds for every sector crossed
	}

      currentSector = request[i].sector; // set current sector to current request
    }
  // return average seek time in milliseconds
  return (totalTime / requestSize);
}

/***************************************************************************
 * double cscan(point[], int)
 * Author: Logan Wheat
 * Date: 19 November 2019
 * Description: simulates a circular scan disk scheduling algorithm.
 *Calculates the total time spent seeking and then returns
 *(total time / request size) to calculate average seek time between
 *requests in milliseconds.
 *
 * Parameters:
 * request I/P point[]  array of points that will be accessed in cscan algorithm
 * requestSize I/P int  size of array of points, that way function does not exceed bounds
 **************************************************************************/
double cscan(point request[], int requestSize)
{
  double totalTime = 0; // initialize total time to 0 in milliseconds

  double currentTrack = 2499; // start in the middle of the track
  double currentSector = 0; // start at sector 0
  double trackDistance; // declare track distance as a double
  int closestTrack; // declare closest track as an int
  double currentClosest; // declare current closest as a double

  currentClosest = 5000; // define current closest as the furthest possible distance

  // for loop to search through sorted list for closest track
  for (int n = 0; n < requestSize; n++)
    {
      trackDistance = abs(currentTrack - request[n].track);

      if (trackDistance < currentClosest && request[n].accessed == false && request[n].track >= 2499) // if current location is closest, has not been accessed, and is greater than or equal to closest track save as closest
	{
	  currentClosest = trackDistance; // update closest track
	  closestTrack = n; // save index of closest track
	}
    } // end of linear search

  int firstClosest = closestTrack; // store the first track accessed for future use

  // for loop to iterate through request from closest index and then upwards and simulate cscan
  for (int i = closestTrack; i < requestSize; i++)
    {
      // track distance will be used to determine new current sector
      trackDistance = abs(currentTrack - request[i].track);
      currentTrack = request[i].track; // set current track as the requested track

      // determine if snapback feature should be used
      if ((trackDistance*.0008) < (1.5 + (currentTrack * .0008))) // if snapback is not faster (slower)
	{
	  totalTime += (trackDistance * .0008); // add 800 nanoseconds for every track crossed
	}
      else // if snapback is faster
	{
	  totalTime += (1.5 + (currentTrack * .0008));
	}

      // hdd is spinning counter clockwise
      currentSector += ((trackDistance * .0008) / .0005); // calculate current sector as 1 sector is passed every 500 nanoseconds
      currentSector = checkSector(currentSector); // call to function that makes sure sector is in range

      if (currentSector > request[i].sector) // if requested sector is before current sector and requires another rotation
	{
	  totalTime += abs(((12000 - (currentSector - request[i].sector)) * .0005)); // add 500 nanoseconds for every sector crossed
	}
      else // if requested sector is further on the track or in the correct spot
	{
	  totalTime += abs(((currentSector - request[i].sector) * .0005)); // add 500 nanoseconds for every sector crossed
	}

      currentSector = request[i].sector; // set current sector to current request
    } // end of first cscan for loop

  // for loop to iterate upward on the request, simulating the cscan looping
  for (int i = 0; i < firstClosest; i++)
    {
      // track distance will be used to determine new current sector
      trackDistance = abs(currentTrack - request[i].track);
      currentTrack = request[i].track; // set current track as the requested track

      // determine if snapback feature should be used
      if ((trackDistance*.0008) < (1.5 + (currentTrack * .0008))) // if snapback is not faster (slower)
	{
	  totalTime += (trackDistance * .0008); // add 800 nanoseconds for every track crossed
	}
      else // if snapback is faster
	{
	  totalTime += (1.5 + (currentTrack * .0008));
	}

      // hdd is spinning counter clockwise
      currentSector += ((trackDistance * .0008) / .0005); // calculate current sector as 1 sector is passed every 500 nanoseconds
      currentSector = checkSector(currentSector); // call to function that makes sure sector is in range

      if (currentSector > request[i].sector) // if requested sector is before current sector and requires another rotation
	{
	  totalTime += abs(((12000 - (currentSector - request[i].sector)) * .0005)); // add 500 nanoseconds for every sector crossed
	}
      else // if requested sector is further on the track or in the correct spot
	{
	  totalTime += abs(((currentSector - request[i].sector) * .0005)); // add 500 nanoseconds for every sector crossed
	}

      currentSector = request[i].sector; // set current sector to current request
    }

  // return average seek time in milliseconds
  return (totalTime / requestSize);
}

/***************************************************************************
 * int main()
 * Author: Logan Wheat
 * Date: 19 November 2019
 * Description: driver function of program. Generates random input streams with no duplicates
 *   for each experiment, runs each of disk the scheduling algorithms per experiment
 *   properly storing the average seek times, and outputs the average seek times
 *   calculated over the 1000 experiments for sizes 500-1000 to both standard out
 *   and a .csv file to properly generate line graph for report.
 *
 * Parameters:
 * main O/P int  Status code (not currently used)
 **************************************************************************/
int main()
{
  ofstream fout; // declare output file that will not be used until averages are calulated

  // declare 2d array for seek time
  double seekTime[501][4] = { { 0 } };
  point request[1000];
  point sortedRequest[1000];

  // for loop for total simulations
  for (int experiment = 0; experiment < 1000; experiment++) // run 1000 experiments
    {
      // for loop for each I/O request size
      for (int size = 500; size <= 1000; size++)
	{
	  // empty request and sortedRequest before filling it
	  for (int i = 0; i < 1000; i++)
	    {
	      request[i].track = 0;
	      request[i].sector = 0;

	      sortedRequest[i].track = 0;
	      sortedRequest[i].sector = 0;
	    }

	  // fill points with uniform random numbers for I/O requests
	  for (int i = 0; i < size; i++)
	    {
	      bool isPresent = false; // declare boolean to false that determines if point is in queue

	      do
		{
		  isPresent = false; // reset isPresent to false 

		  // declare temporary pointer to determine if point is in request already
		  point temp;

		  // fill temporary point with uniform random numbers
		  temp.track = uniform(0, 4999);
		  temp.sector = uniform(0, 11999);

		  // for loop to search for temp in request
		  for (int search = 0; search <= i; search++)
		    {
		      // if statement to see if point is already in I/O request
		      if (temp.sector == request[search].sector)
			{
			  if (temp.track == request[search].track)
			    {
			      isPresent = true;
			    }
			}
		    }

		  // if point is not present, add to request
		  if (isPresent == false)
		    {
		      temp.accessed = false;
		      request[i] = temp;
		      sortedRequest[i] = temp;
		    }

		} while (isPresent == true);
	    } // end of generating I/O requests 

	  // call to function to get sorted request, as it is needed for sstf, scan, and cscan
	  sortRequest(sortedRequest, size);

	  seekTime[size - 500][0] += fifo(request, size);
	  seekTime[size - 500][1] += sstf(sortedRequest, size);
	  seekTime[size - 500][2] += scan(sortedRequest, size);
	  seekTime[size - 500][3] += cscan(sortedRequest, size);
	} // end of single experiment for loop

      cout << "Experiment " <<  experiment +1 << " complete.\n";

    } // end of total experiments for loop

  fout.open("diskscheduling.csv", ios::out); // open csv file to output averages

  // header for output
  cout << "Size:\tFIFO\t\tSSTF\t\tSCAN\t\tC-SCAN\n";
  fout << "Size, FIFO, SSTF, SCAN, C-SCAN\n"; // output to csv file

  // for loop to output averages to standard out and csv file
  for (int i = 0; i <= 500; i++)
    {
      cout << i + 500 << "\t" << (seekTime[i][0] / 1000) << "\t\t" << (seekTime[i][1] / 1000) << "\t\t" << (seekTime[i][2] / 1000) << "\t\t" << (seekTime[i][3] / 1000) << endl;
      fout << i + 500 << ", " << (seekTime[i][0] / 1000) << ", " << (seekTime[i][1] / 1000) << ", " << (seekTime[i][2] / 1000) << ", " << (seekTime[i][3] / 1000) << endl;
    } // end of output for loop

  fout.close(); // close output file
  return 0;
}

