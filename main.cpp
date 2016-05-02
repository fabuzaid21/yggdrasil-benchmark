//
//  main.cpp
//

#include <iostream>
#include <stdlib.h>
#include <ctime>
#include <algorithm>
#include <math.h>
#include <chrono>
#include <limits>


using namespace std;
using byte = unsigned char;

static const int MIN = 0;
static const int MAX = 10;
static const int NUM_CLASSES = MAX - MIN;

double * newImpurityAggregator() {
  double * toReturn = new double[NUM_CLASSES];
  for (int i = 0; i < NUM_CLASSES; ++i) {
    toReturn[i] = 0;
  }
  return toReturn;
}

// calculate Entropy impurity
double calculateEntropyImpurity(const double * const aggregator) {
    double sum = 0.0;
    for (int i = 0; i < NUM_CLASSES; ++i) {
      sum += aggregator[i];
    }

    int numClasses = NUM_CLASSES;
    double impurity = 0.0;
    for (int classIndex = 0; classIndex < numClasses; ++classIndex) {
      double classCount = aggregator[classIndex];
      if (classCount != 0) {
        double freq = classCount / sum;
        impurity -= freq * log2(freq);
      }
    }
    return impurity;
}

// calculate Gini impurity
double calculateGiniImpurity(const double * const aggregator) {
    double sum = 0.0;
    for (int i = 0; i < NUM_CLASSES; ++i) {
      sum += aggregator[i];
    }

    int numClasses = NUM_CLASSES;
    double impurity = 1.0;
    for (int classIndex = 0; classIndex < numClasses; ++classIndex) {
      double classCount = aggregator[classIndex];
      if (classCount != 0) {
        double freq = classCount / sum;
        impurity -= freq * freq;
      }
    }
    return impurity;
}


// subtract second from first
void subtractAggregator(double * const first, const double * const second) {
  for (int i = 0; i < NUM_CLASSES; ++i) {
    first[i] -= second[i];
  }
}

void printAggregator(string label, const double * const aggregator) {
  cout << label << endl;
  for (int i = 0; i < NUM_CLASSES; ++i) {
    cout << i << ", " << aggregator[i] << endl;
  }
  cout << endl;
}

int main(int argc, const char * argv[]) {

  if (argc < 2) {
    cout << "Usage: ./yggdrasil <num rows>" << endl;
    exit(1);
  }

  static const int NUM_ROWS = atoi(argv[1]);

  if (NUM_ROWS <= 0) {
    cout << "<num rows> must be greater than 0" << endl;
  }

  srand(time(0));

  // 1) allocate array of random values (and corresponding indices) and random labels
  double * const values = new double[NUM_ROWS];
  int * const indices = new int[NUM_ROWS];
  byte * const labels = new byte[NUM_ROWS];

  for (int i = 0; i < NUM_ROWS; ++i) {
    labels[i] = rand() % MAX + MIN;
    indices[i] = i;
    values[i] = (double) rand() / (double) RAND_MAX;
  }

  // 2) sort values and indices
  sort(indices, indices + NUM_ROWS, [&values](int i1, int i2) {return values[i1] < values[i2];});
  sort(values, values + NUM_ROWS);

  // 3) find split
  double * fullImpurityAggregator = newImpurityAggregator();
  // 3.1) add all labels to rightImpurityAggregator
  for (int i = 0; i < NUM_ROWS; ++i) {
    fullImpurityAggregator[(int) labels[i]] += 1.0;
  }
  // printAggregator("full", fullImpurityAggregator);

  auto t1 = chrono::high_resolution_clock::now();
  double * leftImpurityAggregator = newImpurityAggregator();
  // printAggregator("left", leftImpurityAggregator);

  double * rightImpurityAggregator = newImpurityAggregator();
  copy(fullImpurityAggregator, fullImpurityAggregator + NUM_CLASSES, rightImpurityAggregator);
  // printAggregator("right", rightImpurityAggregator);

  double bestThreshold = -numeric_limits<double>::max();
  double * bestLeftImpurityAggregator = newImpurityAggregator();
  double bestGain = 0.0;
  double fullImpurity = calculateGiniImpurity(rightImpurityAggregator);

  int leftCount = 0;
  int rightCount = NUM_ROWS;
  int fullCount = rightCount;

  double currentThreshold = values[0];

  for (int i = 0; i < NUM_ROWS; ++i) {
      double value = values[i];
      double label = labels[indices[i]];
      if (value != currentThreshold) {
        // Check gain
        double leftWeight = (double) leftCount / fullCount;
        double rightWeight = (double) rightCount / fullCount;
        double leftImpurity = calculateGiniImpurity(leftImpurityAggregator);
        double rightImpurity = calculateGiniImpurity(rightImpurityAggregator);
        double gain = fullImpurity - leftWeight * leftImpurity - rightWeight * rightImpurity;
        if (leftCount != 0 && rightCount != 0 && gain > bestGain) {
          bestThreshold = currentThreshold;
          copy(leftImpurityAggregator, leftImpurityAggregator + NUM_CLASSES, bestLeftImpurityAggregator);
          bestGain = gain;
        }
        currentThreshold = value;
      }
      leftImpurityAggregator[(int) label] += 1.0;
      rightImpurityAggregator[(int) label] -= -1.0;
      leftCount += 1;
      rightCount -= 1;
  }

  double * bestRightImpurityAggregator = newImpurityAggregator();
  copy(fullImpurityAggregator, fullImpurityAggregator + NUM_CLASSES, bestRightImpurityAggregator);
  subtractAggregator(bestRightImpurityAggregator, bestLeftImpurityAggregator);
  // printAggregator("bestLeft", bestLeftImpurityAggregator);
  // printAggregator("bestRight", bestRightImpurityAggregator);

  //   val bestImpurityStats = new ImpurityStats(bestGain, fullImpurity, fullImpurityAgg.getCalculator,
  //       bestLeftImpurityAggregator.getCalculator, bestRightImpurityAggregator.getCalculator)

  if (bestThreshold != -numeric_limits<double>::max() && bestThreshold != values[NUM_ROWS - 1]) {
    cout << "Found split, best threshold: " << bestThreshold << endl;
  } else {
    cout << "No split found!" << endl;
  }
  auto t2 = chrono::high_resolution_clock::now();
  cout << chrono::duration_cast<chrono::milliseconds>(t2 - t1).count() << " milliseconds" << endl;

  return 0;
}
