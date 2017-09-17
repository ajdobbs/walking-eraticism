/** Small programme to determine eraticism of walking from a step log
  * Author: A. J. Dobbs
  * Date: September 2017
  *
  * The algorithm defines eraticism as:
  *
  *    e = 1 - (actual displacement / expected displacement)
  *
  * averaged over a given number of steps (default 10). Displacement is defined
  * as net distance travelled from the starting point.
  *
  * If actual displacement is greater than expected displacement, e is taken to
  * be 0 (no negative e values, which would indicate a long stride or running).
  *
  * Regions are identified as eratic if the eraticism exceeds a given threshold,
  * default of 0.7. For such regions the timestamp and eraticism are written out
  * to a csv file.
  *
  * The regions which are identified as eratic depend on the algorithm
  * parameters, which may be tuned to give the desired performance:
  *   - nsteps [default 10]
  *   - eraticism_cut [default 0.7]
  *   - stride_length [default 0.4m]
  */

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>

# define PI 3.14159265358979323846

/** Function to parse data from a csv steplog file into a two dimensional vector
 *  Input file format:
 *   - Timestamp [ns]
 *   - Displacement [m]
 *   - Heading [radians]
 *   - Ignored column
 *   - Ignored column
 */
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
  double stride_length = 0.4; // [m] Average size of a step

  // Open the data file and parse into vectors
  std::string ifilename = "2017-01-20Z14-30-05.steplog";
  std::vector<std::vector<double> > data = parse_file(ifilename);

  // Set up an output filestream for the results
  std::string ofilename = "eraticism.dat";
  std::ofstream ofile(ofilename.c_str());
  ofile << std::setprecision(15);

  // Define eraticism based on net distance travelled over last n steps
  std::vector<double> bufX; // Buffer to hold last n x positions
  std::vector<double> bufY; // Buffer to hold last n y positions
  double totalx = 0.0; // Sum the total distance travelled in x
  double totaly = 0.0; // Sum the total distance travelled in y
  double xsum = 0.0; // Sum the total distance travelled in x in last n steps
  double ysum = 0.0; // Sum the total distance travelled in y in last n steps

  for (size_t i = 0; i < data.size(); ++i) {
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
      bufX.erase(bufX.begin());
      bufY.erase(bufY.begin());

      // Define eraticism between 0 and 1, assuming an average stride distance
      double eraticism = 0.0;
      if (davg < stride_length) {  // Not fussed on this if clause, but will do
        eraticism = 1.0 - (davg / stride_length);
      }

      // Write data to file if the eraticism is above the threshold
      if (eraticism > eraticism_cut) {
        // ofile << data[i][0] << " " << totalx << " " << totaly << " " << eraticism << std::endl;
        ofile << data[i][0] << "," << eraticism << std::endl;
      }
    }
  }
  return 0;
}
