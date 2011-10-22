/* ORIP_v2 –> Op Ref Im Proc -> Oper Refin Imag Process -> Operational Refinement of Image Processing
 *
 * Copyright Davide Anastasia and Yiannis Andreopoulos, University College London
 *
 * http://www.ee.ucl.ac.uk/~iandreop/ORIP.html
 * If you have not retrieved this source code via the above link, please contact danastas@ee.ucl.ac.uk and iandreop@ee.ucl.ac.uk
 * 
 */


#include "commandlineparser.h"

using namespace std;

command_line_parser::command_line_parser(void)
{
  _help = false;

  _input_file = DEFAULT_IN_FILE;
  _output_file = DEFAULT_OUT_FILE;
  _num_frame = 300;
  _frame_width = 352; // CIF (352x288)
  _frame_height = 288;
  _bitplane = BIT_SAMPLE;  // all bitplane
  _prog_pattern = "3,3,2";
  _kernel_max_value = 16;
  _conv_width = 8;
  _conv_height = 8;
  _block_size = 16;
  _search_range = 16;

  _time = 20;
  _time_jitter = 0;

  _mode = 0;

  _last_perc = 1.0; // 2009.07.20

  _spiral_size = (int)(0.5625*_search_range);

  _kernel_file = DEFAULT_KERNEL_FILE;
}

command_line_parser::~command_line_parser(void)
{

}

void command_line_parser::parse_line(int ac, char* av[])
{
  for ( int w = 1; w < ac; ++w )
  {
    if ( av[w][0] != '-' )
    {
      //if ( filein[0] == 1 )
      //{
      //  strcpy(filein, av[w]);
      //}
      //else if ( fileout[0] == 1 )
      //{
      //  strcpy(fileout, av[w]);
      //}
      //else;        // In any other case the entry is garbage => do nothing
    }
    else
    {
      // we have a flag
      if ( strlen(av[w]) == 1 ) break;    /// Just a dash => do nothing
      switch ( av[w][1] )
      {
      case 'i': 
        {
          _input_file = av[(++w)];
          break;
        }
      case 'o': 
        {
          _output_file = av[(++w)];
          break;
        }
      case 'f': 
        {
          from_string<int>(_num_frame, av[(++w)], std::dec);
          break;
        }
      case 'w': 
        {
          from_string<int>(_frame_width, av[(++w)], std::dec);
          break;
        }
      case 'h': 
        {
          from_string<int>(_frame_height, av[(++w)], std::dec);
          break;
        }
      case 'b': 
        {
          from_string<int>(_bitplane, av[(++w)], std::dec);
          // error testing
          if ( _bitplane <= 0 || _bitplane > BIT_SAMPLE ) {
            _bitplane = BIT_SAMPLE;
          }
          break;
        }
      case 'p': 
        {
          _prog_pattern = av[(++w)];
          break;
        }
      case 'z': 
        {
          from_string<int>(_spiral_size, av[(++w)], std::dec);
          if ( _spiral_size < 1 ) 
            _spiral_size = 300000; // something very large to ensure it will be set at the end
          break;
        }
      case 'k': 
        {
          from_string<int>(_kernel_max_value, av[(++w)], std::dec);
          break;
        }
      case 'm': 
        {
          from_string<int>(_conv_width, av[(++w)], std::dec);
          break;
        }
      case 'n': 
        {
          from_string<int>(_conv_height, av[(++w)], std::dec);
          break;
        }
      case 'd': 
        {
          from_string<int>(_block_size, av[(++w)], std::dec);
          break;
        }
      case 's': 
        {
          from_string<int>(_search_range, av[(++w)], std::dec);
          break;
        }
      case 't': 
        {
          from_string<int>(_mode, av[(++w)], std::dec);
          break;
        }
      case 'q': 
        {
          _kernel_file = av[(++w)];
          break;
        }
      case 'c': 
        //clock time
        {
          from_string<int>(_time, av[(++w)], std::dec);
          // error testing
          if ( _time <= 0 ) {
            _time = 20; // set default value
          }
          break;
        }
      case 'j': 
        // clock time jitter
        {
          from_string<int>(_time_jitter, av[(++w)], std::dec);

          if ( _time < 0 ) {
            _time_jitter = 0;
          }
          break;
        }
      case 'l': 
        // last step percentage (0.0 ~ 1.0)
        {
          from_string<double>(_last_perc, av[(++w)], std::dec);

          if ( _last_perc < 0 || _last_perc > 1) {
            _last_perc = 1.0;
          }
          break;
        }

      case 'x': 
        {
          _help = true;
          ++w;
          break;
        }
      default: ++w; break;
      }
    }
  }

  if ( _spiral_size > _search_range ) _spiral_size = _search_range;

  parse_progression_pattern();
}

bool command_line_parser::check_constrains(bool file_check)
{
  bool result = true;

  if ( file_check == true )
  {
    if ( !is_set_input_filename() )
    {
      std::cerr << "[!] Input filename (-i) not defined" << std::endl;
      result = false;
    } 
  }
  if ( get_mode() == 4 && !is_set_kernel_filename() )
  {
    std::cerr << "[!] Kernel filename (-q) not defined" << std::endl;
    result = false;
  } 

  // checking constraints
  if ( _frame_height <= 0 ) {
    std::cerr << "[!] Invalid frame height (-h)" << std::endl;
    result = false;
  }
  if ( _frame_width <= 0 ) {
    std::cerr << "[!] Invalid frame width (-w)" << std::endl;
    result = false;
  }
  if ( _num_frame <= 0 ) {
    std::cerr << "[!] Invalid number of frame to be processed (-f)" << std::endl;
    result = false;
  }
  if ( _num_frame <= 0 ) {
    std::cerr << "[!] Invalid number of frame to be processed (-f)" << std::endl;
    result = false;
  }

  return result;
}

bool command_line_parser::is_set_help()
{
  return _help; 
}

const void command_line_parser::get_help()
{
  cout << endl;
  cout << "Copyright Davide Anastasia and Yiannis Andreopoulos, University College London, 2009" << endl;
  cout << "See README.pdf for details of usage" << endl;
  cout << "Options: " << endl;
  cout << setw(6) << "-i : " << "Input file name" << endl;
  cout << setw(6) << "-o : " << "Output file name" << endl; //  (see below for OLPC)
  cout << setw(6) << "-f : " << "Total frames of the input to be processed" << endl;  // (see below for OLPC)
  cout << setw(6) << "-w : " << "Width of the frame" << endl;
  cout << setw(6) << "-h : " << "Height of the frame" << endl;
  cout << setw(6) << "-b : " << "Number of bitplanes" << endl;
  cout << setw(6) << "-p : " << "Scanning pattern (for incremental version only)" << endl;
  cout << setw(6) << "-l : " << "Last step percentage (for incremental version only) (0.0 ~ 1.0)" << endl;

  cout << endl << "Options (Transform Only)" << endl;
  cout << setw(6) << "-t : " << "Mode (0: Random kernel; 1: 4x4 AVC; 2: 8x8 BinDCT; 3: 8x8 FreXT)" << endl;
  cout << setw(6) << "-k : " << "Dynamic range of transform kernel (only for random kernel)" << endl;
  cout << setw(6) << "-n : " << "Transform block size (only for random kernel)" << endl;

  cout << endl << "Options (Convolution only)" << endl;
  cout << setw(6) << "-t : " << "Mode (0: Random kernel; 1: 12x12 Gaussian; 2: 18x18 Gaussian; 3: 6x6 Gaussian; 4: Read from file)" << endl;
  cout << setw(6) << "-d : " << "Kernel File Name" << endl;
  cout << setw(6) << "-m : " << "Width of filtering kernel (only for random kernel)" << endl;
  cout << setw(6) << "-n : " << "Height of filtering kernel (only for random kernel)" << endl;
  cout << setw(6) << "-k : " << "Dynamic range of filtering kernel (only for random kernel)" << endl;

  cout << endl << "Options (Block Matching only)" << endl;
  cout << setw(6) << "-d : " << "Block size" << endl;
  cout << setw(6) << "-s : " << "Search range" << endl;
  cout << setw(6) << "-z : " << "Spiral size (W)" << endl;

#ifdef _OPENMP
  cout << endl << "Options (Only for scheduled versions)" << endl;
  cout << setw(6) << "-c : " << "Clock time interrupt" << endl;
  cout << setw(6) << "-j : " << "Clock jitter" << endl;
#endif
}

bool command_line_parser::is_set_input_filename()
{
  if ( _input_file == DEFAULT_IN_FILE ) return false;
  return true;
}

string command_line_parser::get_input_filename()
{
  return _input_file;
}

bool command_line_parser::is_set_output_filename()
{
  if ( _output_file == DEFAULT_OUT_FILE ) return false;
  return true;
}

string command_line_parser::get_output_filename()
{
  return _output_file;
}

string command_line_parser::get_output_filename_wout_ext()
{
  string::size_type pos = _input_file.find_last_of(".", 0);

  return _input_file.substr(0, pos);
}

string command_line_parser::get_kernel_filename()
{
  return _kernel_file;
}

bool command_line_parser::is_set_kernel_filename()
{
  if ( _kernel_file == DEFAULT_KERNEL_FILE ) return false;
  return true;
}

int command_line_parser::get_height()
{
  return _frame_height;
}

int command_line_parser::get_width()
{
  return _frame_width;
}

int command_line_parser::get_height_conv()
{
  return _conv_height;
}

int command_line_parser::get_width_conv()
{
  return _conv_width;
}

int command_line_parser::get_bitplane()
{
  return _bitplane;
}

int command_line_parser::get_frame_num()
{
  return _num_frame;
}

int command_line_parser::get_kernel_max_value()
{
  return _kernel_max_value;
}

int command_line_parser::get_mode()
{
  return _mode;
}

vector<int> command_line_parser::get_progression_pattern()
{
  return _prog_vector;
}

void command_line_parser::parse_progression_pattern()
{
  int const bits2parse = get_bitplane();
  int bits_sum = 0;
  int bits_curr = 0;
  int bits_index = NUM_BITPLANE;

  // Parsing the input string
  string::size_type lastPos = _prog_pattern.find_first_not_of(",", 0);    // Skip delimiters at beginning
  string::size_type pos     = _prog_pattern.find_first_of(",", lastPos);  // Find first "non-delimiter"

  while (string::npos != pos || string::npos != lastPos)
  {
    from_string<int>(bits_curr, _prog_pattern.substr(lastPos, pos - lastPos), std::dec ); // Found a token, add it to the vector.
    // limit check
    if ( bits_curr <= 0 ) bits_curr = 1;
    else if ( bits_curr > bits2parse ) bits_curr = bits2parse;
    // ---

    if ( bits_sum + bits_curr >= bits2parse ) {
      bits_curr = bits2parse - bits_sum;
      _prog_vector.push_back(bits_curr);
      _prog_steps.push_back(bits_index);
      break;
    } else {
      _prog_vector.push_back(bits_curr);
      _prog_steps.push_back(bits_index);
      bits_sum += bits_curr;
    }

    bits_index -= bits_curr;

    lastPos = _prog_pattern.find_first_not_of(",", pos);    // Skip delimiters.  Note the "not_of"
    pos = _prog_pattern.find_first_of(",", lastPos);        // Find next "non-delimiter"
  }

  // check trail, find number of steps and find max step
  bits_sum = 0;
  _num_prog_step = 0;
  _max_prog_step = 1;
  for (unsigned int i = 0; i < _prog_vector.size(); i++)
  {
    bits_sum += _prog_vector[i];

    _max_prog_step = max(_max_prog_step, _prog_vector[i]); 
    _num_prog_step++;
  }

  while ( bits_sum < bits2parse )
  {
    _prog_vector.push_back(1);
    _prog_steps.push_back(bits_index);
    _num_prog_step++;
    bits_sum++;
    bits_index--;
  }

  // return _prog_vector;
}

void command_line_parser::set_progression_pattern(std::string pp)
{
  _prog_pattern = pp;
  _prog_vector.erase(_prog_vector.begin(), _prog_vector.end());
  _prog_steps.erase(_prog_steps.begin(), _prog_steps.end());

  parse_progression_pattern();
}

int command_line_parser::get_progression_steps()
{
  return _num_prog_step;
}

int command_line_parser::get_max_progression_step()
{
  return _max_prog_step;
}

vector<int> command_line_parser::get_progression_indexes()
{
  return _prog_steps;
}

void command_line_parser::print_progression_pattern()
{
  cout << " Progression pattern: ";
  for (unsigned int i = 0; i < _prog_vector.size(); i++)
  {
    cout << _prog_vector[i] << " ";  
  }
  cout << "- Steps: " << _num_prog_step;
  cout << " - Max step: " << _max_prog_step;

  cout << endl;
}

int command_line_parser::get_block_dim()
{
  int b_dim = _block_size;
  if ( b_dim != 4 && b_dim != 8 && b_dim != 16 && b_dim != 32 )
  {
    return 8;
  }
  return _block_size;
}

int command_line_parser::get_search_range()
{
  int s_dim = _search_range;
  if ( s_dim != 4 && s_dim != 8 && s_dim != 16 && s_dim != 32 )
  {
    return 8;
  }
  return _search_range;
}

int command_line_parser::get_time()
{
  return _time;
}

int command_line_parser::get_time_jitter()
{
  return _time_jitter;
}

