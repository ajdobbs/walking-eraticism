/** Small programme to determine eraticism of walking from step log
  * Author: A. Dobbs
  * Date: September 2017
  */

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>

# define PI 3.14159265358979323846

std::vector<std::vector<double> > parse_file(std::string& filename) {
  std::ifstream ifile(filename.c_str());
  std::string line;
  std::vector<std::vector<double> > data;
  while(std::getline(ifile, line)) {
    std::vector<double> linedata;
    std::stringstream  lineStream(line);
    std::string cell;
    while(std::getline(lineStream, cell, ',')) {
      linedata.push_back(std::stod(cell));
    }
    data.push_back(linedata);
  }
  return data;
};

int main(int argc, char *argv[]) {
  // Some parameters of the algorithm
  double nsteps = 10.0; // Number of steps to average over
  double eraticism_cut = 0.7; // Minimum eraticism before data is written out
  double step_time = 1000000000.0; // [ns] If step has not occured after 1s person is stationary
  double expected_dist = 0.4; // [m] Average size of a step

  // Open the data file and parse into vectors
  std::string ifilename = "2017-01-20Z14-30-05.steplog";
  std::vector<std::vector<double> > data = parse_file(ifilename);

  // Set up an output filestream for the results
  std::string ofilename = "eraticism.dat";
  std::ofstream ofile(ofilename.c_str());
  ofile << std::setprecision(15);

  // Define eraticism based on net distance travelled over last n steps
  std::vector<double> bufX; // Buffer to hold last 10 x positions
  std::vector<double> bufY; // Buffer to hold last 10 y positions
  double totalx = 0.0; // Sum the total distance travelled in x
  double totaly = 0.0; // Sum the total distance travelled in y
  double xsum = 0.0; // Sum the total distance travelled in x in last n steps
  double ysum = 0.0; // Sum the total distance travelled in y in last n steps

  for (size_t i = 0; i < data.size(); ++i) {
    if ((data[i][0] - data[i][0]) > step_time) // Check if person is actually walking first
      continue;

    double x = data[i][1]*cos(data[i][2]); // Distance travelled in x
    double y = data[i][1]*sin(data[i][2]); // Distance travelled in y
    bufX.push_back(x);
    bufY.push_back(y);
    totalx += x;
    totaly += y;
    xsum += x;
    ysum += y;

    // Fill the buffer, before taking an average
    if (i >= nsteps) {
      // Update the average
      xsum -= bufX[0];
      ysum -= bufY[0];
      double xavg = (xsum) / nsteps;
      double yavg = (ysum) / nsteps;
      double davg = sqrt(xavg*xavg + yavg*yavg);
      std::cout << data[i][0] << " " << davg << std::endl;
      bufX.erase(bufX.begin());
      bufY.erase(bufY.begin());

      // Define eraticism between 0 and 1, assuming 0.4m is average distance travelled per step
      double eraticism = 0.0;
      if (davg < expected_dist) {  // Not fussed on this if clause, but will do
        eraticism = 1.0 - (davg / expected_dist);
      }

      // Write data to file if the eraticism is above the threshold
      if (eraticism > eraticism_cut) {
        ofile << data[i][0] << " " << totalx << " " << totaly << " " << eraticism << std::endl;
      }
    }
  }
  return 0;
}
