/*
 **************************************************************************
 *                                                                        *
 *                           Word Search Solver                           *
 *                                                                        *
 * Description: Solver for word searches                                  *
 * Author: Paul Rubel                                                     *
 * URL: http://www.rubels.net                                             *
 *                                                                        *
 * THE PROGRAM IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OR     *
 * CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED INCLUDING, WITHOUT   *
 * LIMITATION, ANY WARRANTIES OR CONDITIONS OF TITLE, NON-INFRINGEMENT,   *
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.                   *
 *                                                                        *
 * Copyright notice:                                                      *
 * Free use of this code is permitted under the guidelines                *
 * and in accordance with the most current version of the Common Public   *
 * License.                                                               *
 * http://www.opensource.org/licenses/cpl1.0.php                          *
 *                                                                        *
 **************************************************************************
*/


/*
 * Given a grid of letters find words in those letters.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <string.h>
#include <map>
#include <utility>

#include "bloom_filter.hpp"

bloom_parameters parameters;

size_t MIN_LEN = 5;

const char* WORD_FILE = "words.txt";
const char* OUT_FILE = "res.ps";
const char* FOUND_WORDS_FILE = "found_words.txt";

/*
  In order to quickly search for words we save 8 copies of the grid,
  each with the words going in a particular direction. 
For the grid 
   ABC
   DEF
   GHI

The east grid is the typical orientation and reads words left to
right. The left grid could be

  CBA
  FED
  IHG

North is GDA
         HEB
         IFC

The diagonals aren't square but rather triangular so nw is
 A
 DB
 GEC
 HF
 I

*/

char** east;
char** west;
char** north;
char** south;
char** diag_ne;
char** diag_nw;
char** diag_se;
char** diag_sw;

//These maps map row/columns in a given map to coordinates in the
//original/east map. Since the rows/columns are small numbers we 
//overload x and y into one int using make_val, get_y_from-key, and
//get_x_from_key

std::map<int, int> east_map;
std::map<int, int> west_map;
std::map<int, int> north_map;
std::map<int, int> south_map;
std::map<int, int> ne_map;
std::map<int, int> nw_map;
std::map<int, int> se_map;
std::map<int, int> sw_map;

//how many rows/columns
int n;

//map a pair to a single int, used to index a cell
int make_val(int i, int j) {
  return i * 10000 + j;
}

//decode y from a value retrieved from a map
int get_y_from_key(int v) {
  return v % 10000;
}

//decode x from a value retrieved from a map
int get_x_from_key(int v) {
  int res = get_y_from_key(v);
  return (v - res) / 10000;
}

//useful for debugging
void show(std::string msg, char** m, int max) {
  std::cout << std::endl << msg << std::endl;
  for (int i = 0; i < max; i++) {
    std::cout << "row:" << m[i] << std::endl;
  }
}

//how big is the words file?
int file_len() {
  std::ifstream myfile (WORD_FILE);
  long begin = myfile.tellg();
  myfile.seekg (0, std::ios::end);
  long end = myfile.tellg();
  myfile.close();
  return end-begin;
}

bloom_filter *setup_filter() {
  // How many elements roughly do we expect to insert?
  //assume average word is >= 6 chars long this jives with words.txt,
  //with an average of 8. Minimum of 5k just in case someone manually 
  //puts in a list that's extra small say easy spelling words
  parameters.projected_element_count = std::max(5000, file_len() / 6);
  //std::cout << " setting projected count to " << parameters.projected_element_count << std::endl;

   // Maximum tolerable false positive probability? (0,1)
   parameters.false_positive_probability = 0.00001; // 1 in 10000

   // Simple randomizer (optional)
   parameters.random_seed = 0xA5A5A5A5;

   if (!parameters) {
      std::cout << "Error - Invalid set of bloom filter parameters!" << std::endl;
      return NULL;
   }

   parameters.compute_optimal_parameters();

   //Instantiate Bloom Filter
   bloom_filter* filter = new bloom_filter(parameters);

   //fill words
   std::ifstream infile(WORD_FILE);
   std::string line;

   while (getline(infile, line)) {
     std::transform(line.begin(), line.end(), line.begin(), ::tolower);
     if (line.length() >= MIN_LEN) {
       filter->insert(line); //.c_str() gave some odd behavior
     }
   }
   return filter;
}

//allocate storate, assume we'll succeed
void init_storage(int len) {
  east = new char*[len];
  west = new char*[len];
  north = new char*[len];
  south = new char*[len];
  diag_ne = new char*[2*len];
  diag_nw = new char*[2*len];
  diag_se = new char*[2*len];
  diag_sw = new char*[2*len];
  for (int j = 0; j < n; j++) {
    east[j] = new char[len+1]();
    west[j] = new char[len+1]();
    north[j] = new char[len+1]();
    south[j] = new char[len+1]();
  }

  //actuall 2n-1 but don't worry about that
  for (int j = 0; j < 2*n; j++) {
    diag_ne[j]  = new char[len+1]();
    diag_nw[j]  = new char[len+1]();
    diag_se[j]  = new char[len+1]();
    diag_sw[j]  = new char[len+1]();
  }
}

//read in the original structure
void populate_east() {
  std::cout << "How many rows are there? " << std::endl;
  size_t rows;
  std::cin  >> rows;
  if (n > 200) {
    std::cout << " n is too large, try < 200" << std::endl;
    exit(-1);
  }

  std::cout << "Enter a row" << std::endl;
  size_t cols = 0;
  std::string extra = "";
  size_t i;
  for (i = 0; i < rows; i++) {
    std::string row;
    std::cin >> row;
    std::transform(row.begin(), row.end(), row.begin(), ::tolower);
    //find out if we're not square
    if (0 == cols) {
      cols = row.length();
      n = std::max(rows, cols);
      init_storage(n);
      if (rows > cols) {
        for (size_t j = 0; j < (rows-cols); j++) {
          extra += " ";
        }
      }
    }
    row += extra;//in case of non-square grids
    strncpy(east[i], row.c_str(), n);
  }
  //extra rows if cols > rows
  for (std::string rest = std::string(cols, ' '); i < cols; i++) {
    strncpy(east[i], rest.c_str(), n);
  }
}

void print_result_header(FILE* res) {
 //set up output and populate other directions and maps
  fprintf(res, "%%!\n");
  fprintf(res, "/Times-Roman findfont\n");
  fprintf(res, "12 scalefont\n");
  fprintf(res, "setfont\n");
  fprintf(res, "0.3 setlinewidth\n");
  fprintf(res, "0.3 setgray\n");
}

void print_result_trailer(FILE* res) {
  fprintf(res, "showpage\n");
}

int setup_puzzle(FILE* res_file) {
  populate_east();

 
  //make east_map, mapping grid coordinates to output coordinates
  for (int x = 0; x < n; x++) {
    for (int y = 0; y < n; y++) {
      char c = east[x][y];
      int px = 50+(15*y);
      int py = 50+(15*(n-x));
      fprintf(res_file, "%d %d moveto\n(%c) show %% %d %d\n", 
              px, py, c, y, x);
      east_map[make_val(x,y)] = make_val(px,py);
    }
  }

  //populate n,w, and south
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      int key = make_val(i,j);

      west[i][j] = east[i][n-j-1];
      west_map[key] = east_map[make_val(i,n-j-1)];

      south[i][j] = east[j][i];
      south_map[key] = east_map[make_val(j,i)];

      north[i][j] =   east[n-j-1][i];
      north_map[key] = east_map[make_val(n-j-1,i)];
    }
  }

  //show("east", east, n);
  //show("west", west, n);
  //show("south", south, n);
  //show("north", north, n);

  //diagonals. If we're within the square populate. 
  //is there some linear algegbra that we could use instead?

  //SE
  int slope = -1;
  for (int b = 0; b < (2*n); b++) {
    int count = 0;
    for (int x = 0; x < n; x++) {
      int y = slope * x + b;
      if (y >= 0 && y < n) {
        diag_se[b][count] = east[n-1-y][x];
        se_map[make_val(b,count)] = east_map[make_val((n-1-y), x)];
        count++;
      }
    }
  }

  //NE
  slope = 1;
  for (int b = -n; b < n; b++) {
    int count = 0;
    for (int x = 0; x < n; x++) {
      int y = slope * x + b;
      if (y >= 0 && y < n) {
        diag_ne[b+n-1][count] = east[n-1-y][x];
        ne_map[make_val((b+n-1), count)] = east_map[make_val(n-1-y, x)];
        count++;
      }
    }
  }

  //nw and sw are just the reverse of the above
  for (int i = 0; i < 2*n; i++) {
    int len = strlen(diag_se[i]);
    for (int c = 0; c < len; c++) {
      diag_nw[i][len-c-1] = diag_se[i][c];
      nw_map[make_val(i, (len-c-1))] = se_map[make_val(i,c)];

      diag_sw[i][len-c-1] = diag_ne[i][c];
      sw_map[make_val(i, len-c-1)] =  ne_map[make_val(i, c)];
    }
  }

  //show("se", diag_se, 2*n-1);
  //show("nw", diag_nw, 2*n-1);
  //show("ne", diag_ne, 2*n-1);
  //show("sw", diag_sw, 2*n-1);


  return 0;
}


void search(char** data, std::map<int,int> m, int rows, 
            bloom_filter* filter, FILE* res, FILE* found_file) {

  for (int i = 0; i < rows; i++) {
    char* d = data[i];
    size_t len = strlen(d);

    for (size_t j = 0; j < len; j++) {
      char *word = d+j;
      //std::cout << " looking for " << word << std::endl;
      for (size_t z = MIN_LEN; z <= len; z++) {
        if (filter->contains(word, z)) {
          int v = m[make_val(i,j)];
          int x_pos = get_x_from_key(v) + 3;
          int y_pos = get_y_from_key(v) + 3;

          int end_v = m[make_val(i,j+z-1)];
          int end_x_pos = get_x_from_key(end_v) + 3;
          int end_y_pos = get_y_from_key(end_v) + 3;

          std::string found = std::string(d+j,z);
          fprintf(found_file, "%s\n", found.c_str());
          std::cout << " Found " << found << " at " << i  << ":" << j  
                    << " of length " << z << " find at " 
                    << x_pos << ":" << y_pos 
                    << " to " 
                    << end_x_pos << ":" << end_y_pos 
                    << std::endl;

          fprintf(res, "newpath\n%d %d 6 0 360 arc closepath\nstroke\n", 
                  x_pos, y_pos);
          fprintf(res, "newpath\n%d %d moveto %% %% %s %d:%d - %d:%d\n", 
                  x_pos, y_pos,
                  found.c_str(),
                  x_pos, y_pos, end_x_pos, end_y_pos);
          fprintf(res, "%d %d lineto\nstroke\n\n", end_x_pos, end_y_pos);
        }
      }
    }
  }
}

int check(std::string w, bloom_filter* filter) {
  if (filter->contains(w.c_str(), w.length())) {
    std::cout << w << " found  \n";
    return 0;
  }
  else {
    std::cout << " No " << w << std::endl;
    return 1;
  }
}

void usage(char *arg) {
  std::cerr << "Usage: " << arg << " {-n min-word-length} " 
            << "{-d dictionary-file} {-o output-file} " 
            << "{-f found-words-file}" << std::endl;
  std::cerr << "  default minimum word length is 5. " << std::endl
            << "  default dictionary is words.txt, one word per line" << std::endl
            << "  default output is res.ps" << std::endl
            << "  default found-words-file is found_words.txt" << std::endl
            << "     use -f to limit hits by editing it to remove unwanted" << std::endl
            << "     words and then passing it in via -d " << std::endl;
  std::cerr << " Look at your results by opening output-file using a postscript/pdf viewer" << std::endl;
  exit(-1);
}

void parse_args(int argc, char** argv) {
  if (argc % 2 == 0) { // single option is always bad, show help
    usage(argv[0]);
  }
  for (int i = 1; i < argc-1; i++) {
    if (0 == strcmp("-n", argv[i])) {
      MIN_LEN = atoi(argv[i+1]);
      i++;
    }
    else if (0 == strcmp("-d", argv[i])) {
      WORD_FILE = argv[i+1];
      i++;
    }
    else if (0 == strcmp("-o", argv[i])) {
      OUT_FILE = argv[i+1];
    }
    else if (0 == strcmp("-f", argv[i])) {
      FOUND_WORDS_FILE = argv[i+1];
    }
    else {
      usage(argv[0]);
    }
  }
}

int main(int argc, char** argv)
{
  parse_args(argc, argv);

  bloom_filter* filter = setup_filter();
  if (NULL == filter) {
    std::cout << "Exiting, could name make filter\n";
    exit(1);
  }

  FILE* result_file  = fopen(OUT_FILE, "w");
  FILE* found_file  = fopen(FOUND_WORDS_FILE, "w");
  print_result_header(result_file);
  setup_puzzle(result_file);
  std::cout << "Searching \n";



  search(east,    east_map,  n,   filter, result_file, found_file);
  search(west,    west_map,  n,   filter, result_file, found_file);
  search(north,   north_map, n,   filter, result_file, found_file);
  search(south,   south_map, n,   filter, result_file, found_file);
  search(diag_ne, ne_map,    2*n, filter, result_file, found_file);
  search(diag_nw, nw_map,    2*n, filter, result_file, found_file);
  search(diag_se, se_map,    2*n, filter, result_file, found_file);
  search(diag_sw, sw_map,    2*n, filter, result_file, found_file);

  //this should really be tied in somewhere else, with the header
  print_result_trailer(result_file);
  fclose(result_file);
  fclose(found_file);
  return 0;
}
