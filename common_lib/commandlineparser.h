/* ORIP_v2 –> Op Ref Im Proc -> Oper Refin Imag Process -> Operational Refinement of Image Processing
 *
 * Copyright Davide Anastasia and Yiannis Andreopoulos, University College London
 *
 * http://www.ee.ucl.ac.uk/~iandreop/ORIP.html
 * If you have not retrieved this source code via the above link, please contact danastas@ee.ucl.ac.uk and iandreop@ee.ucl.ac.uk
 * 
 */
#ifndef __COMMANDLINEPARSER_3_H_
#define __COMMANDLINEPARSER_3_H_

#include <string>
#include <string.h>

#include <iostream>
#include <iomanip>
#include <vector>
#include "common_lib.h"

using namespace std;

#define   BIT_SAMPLE        (8)
#define   NUM_BITPLANE      (BIT_SAMPLE - 1)
#define   DEFAULT_IN_FILE       "in.in"
#define   DEFAULT_OUT_FILE      "out.data"
#define   DEFAULT_KERNEL_FILE   "52f8845c085ae87c4edf7ee1526dffdd.txt"

class command_line_parser
{
private:
  std::string _input_file;
  std::string _output_file;
  int _num_frame;
  int _frame_width;
  int _frame_height;
  int _bitplane;
  int _kernel_max_value;
  int _conv_width;
  int _conv_height;
  int _block_size;
  int _search_range;
  bool _help;

  int _time;
  int _time_jitter;
  int _mode;

  std::string _prog_pattern;
  vector<int> _prog_vector;
  vector<int> _prog_steps;
  int _max_prog_step;
  int _num_prog_step;

  int _spiral_size;

  double _last_perc;

  std::string _kernel_file;

protected:
  command_line_parser(command_line_parser& );

  void parse_progression_pattern();

public:
  command_line_parser(void);

  void parse_line(int ac, char* av[]);

  ~command_line_parser(void);

  bool check_constrains(bool file_check = true);

  bool is_set_help();
  const void get_help();

  bool is_set_output_filename();
  string get_output_filename();
  string get_output_filename_wout_ext();

  bool is_set_input_filename();
  string get_input_filename();

  bool is_set_kernel_filename();
  string get_kernel_filename();

  //bool is_set_height();
  int get_height();

  //bool is_set_width();
  int get_width();

  int get_width_conv();
  int get_height_conv();

  int get_bitplane();
  int get_frame_num();
  int get_kernel_max_value();

  int get_block_dim();
  int get_search_range();

  int get_mode();

  int get_time();
  int get_time_jitter();

  double get_last_step_percentage() { return _last_perc; }

  // bool is_set_progression_pattern();
  vector<int> get_progression_pattern();
  void set_progression_pattern(std::string pp);

  vector<int> get_progression_indexes();

  void print_progression_pattern();
  int get_progression_steps();
  int get_max_progression_step();

  int get_spiral_size() { return _spiral_size; }
};

#endif

