/* ORIP_v2 –> Op Ref Im Proc -> Oper Refin Imag Process -> Operational Refinement of Image Processing
 *
 * Copyright Davide Anastasia and Yiannis Andreopoulos, University College London
 *
 * http://www.ee.ucl.ac.uk/~iandreop/ORIP.html
 * If you have not retrieved this source code via the above link, please contact danastas@ee.ucl.ac.uk and iandreop@ee.ucl.ac.uk
 * 
 */


#define   _CRT_SECURE_NO_DEPRECATE

#include <iostream>
#include <fstream>
#include <functional>
#include <algorithm>
#include <vector>
#include <stdio.h>
#include <stdlib.h>

#include "commandlineparser.h"
#include "common_lib.h"
#include "library.h"

#include "timing.h"

// boolean for threading cooperation
#ifdef _OPENMP  // look inside library.cpp
#include <omp.h>
#include "sleep_t.h"

extern bool flag_int;
extern bool flag_scheduler;
#endif
// ---
using namespace std;

int main(int ac, char* av[])
{
#ifdef _OPENMP
  cout << " 2D Conv (Loose packing, scheduling, combined conv + unpack): Two Threads (Main Loop + Scheduler)" << endl;
  cout << " OpenMP Enabled: " << _OPENMP << endl;
#else
  cout << " 2D Conv (Loose packing, combined conv + unpack)" << endl;
#endif

  // TIMER -----
  My_Timer perf_timer;
  My_Timer perf_timer_f;
  // -----------

#ifdef _OPENMP
  Sleep_T sleep_timer;
  int num_incomplete_frame = 0;
#endif

  // input file
  FILE *ifile;
  FILE *ofile;
  // ----

  command_line_parser* parser = new command_line_parser();

  parser->parse_line(ac, av);

  if (parser->is_set_help())
  {
    parser->get_help();
    return 1;
  }

  if ( !(parser->check_constrains()) )
  {
    parser->get_help();
    return 1; // 1: error
  }

  if ( (ifile = fopen(parser->get_input_filename().c_str(), "rb")) == NULL )
  {
    cerr << "[!] Binary image doesn't exist or there's possibly a wrong path" << endl;
    exit(1);
  }

  if ( parser->get_output_filename() != DEFAULT_OUT_FILE )
  {
    if ( (ofile = fopen(parser->get_output_filename().c_str(), "wb")) == NULL )
    {
      cerr << "[!] Output file cannot be opened" << endl;
      exit(1);
    }
  }

  parser->print_progression_pattern();

#ifdef _OPENMP
  // Setting Thread timer variables
  int time = parser->get_time();
  int jitter = parser->get_time_jitter();
  time -= jitter;
  if ( time < 0 ) time = 0;
  jitter *= 2;
  cout << " Clock Timer Configuration: (" << time << "," << time+jitter << ")" << endl;
  // END ---
#endif

  // -----------------------------------------------------------------------------
  double wrk_time = 0;
  int last_bitplane = NUM_BITPLANE + 1 - parser->get_bitplane();

  int const width = parser->get_width();
  //int width_original = parser->get_width();
  int height = parser->get_height();
  int width_c = parser->get_width_conv();
  int height_c = parser->get_height_conv();

  cout << "Convolution Matrix Dimension: " << width_c << " x " << height_c << endl;

  matrix conv_matrix; 
  switch ( parser->get_mode() )
  {
  case 1: {
    // 12x12 gaussian
    height_c = width_c = 12;
    conv_matrix = create_matrix<my_t>(width_c, width_c);
    setup_conv_gaussian_12x12(conv_matrix);
    break;
          }
  case 2: {
    // 18x18 gaussian
    height_c = width_c = 18;
    conv_matrix = create_matrix<my_t>(width_c, width_c);
    setup_conv_gaussian_18x18(conv_matrix);
    break;
          }
  case 3: {
    // 6x6 gaussian
    height_c = width_c = 6;
    conv_matrix = create_matrix<my_t>(width_c, width_c);
    setup_conv_gaussian_6x6(conv_matrix);
    break;
          }
  case 4:    {
    conv_matrix = read_matrix_from_file<my_t>(parser->get_kernel_filename(), width_c, height_c);
    break;
             }
  default:
    conv_matrix = create_matrix<my_t>(width_c, height_c);
    setup_conv_matrix(conv_matrix, width_c, height_c, parser->get_kernel_max_value());
  }

  cout << "CONV =" << endl;
  print_matrix(conv_matrix, width_c, height_c);
  // -----------------------------------------------------------------------------

  // ENV
  setup_env(conv_matrix, width_c, height_c, parser->get_max_progression_step());
  show_env_status();

  // --- calculate max value for packing -----------------------------------------
  double const last_step_perc = parser->get_last_step_percentage();

  // WIDTH ----
  int width_e = width;
  int width_p = width;

  // HEIGHT ----
  int height_e = height;
  if ( (height%__num_pack) != 0 )
  {
    height_e = height + (__num_pack - (height%__num_pack));
  }
  int height_p = height_e/__num_pack + height_c - 1;
  // -----------------------------------------------------------------------------

  // -- buffers ------------------------------------------------------------------
  matrix input_f = create_matrix<my_t>(width_e, height_e);
  matrix input_b = create_matrix<my_t>(width_e, height_e);

  double** input_p = create_matrix<double>(width_p, height_p);  // different sizes
  double** output_p = create_matrix<double>(width_p, height_p);

  matrix output_f = create_matrix<my_t>(width_e, height_e);
  // -----------------------------------------------------------------------------

  vector<int> bitplane = parser->get_progression_pattern(); // progression pattern
  vector<int> indexes = parser->get_progression_indexes(); 

  start_high_priority();

#pragma omp parallel sections num_threads(2) 
  {
#pragma omp section
    // thread 1
    {
      perf_timer_f.start();
      for ( int frames = 0; frames < parser->get_frame_num() ; frames++ )
      {
        // reset accumulation buffers
        reset_matrix(output_f, width_e, height_e);

        // read frames
        fill_matrix(input_f, width_e, height, ifile);

        // --- handling correctly parsing progression
        int bits, b;
        // ---
        for ( int dx = 0; dx < parser->get_progression_steps(); dx++ )  // prog steps
        {
          bits = bitplane[dx];
          b = indexes[dx];
          if ( (dx + 1) == parser->get_progression_steps() )  // last step
          { 
            width_p = int(width*last_step_perc);
            width_e = int(width*last_step_perc);
          }

          // extract bitplane
          extract_bitplane_matrix_i(input_f, input_b, width_e, height_e, b, bits);

          perf_timer.start();  // ---- timer!
          pack_matrix(input_b, width_e, height_e, input_p, width_p, height_p, __num_pack);

          //cout << "_" << endl;
          switch ( parser->get_mode() )
          {
          case 1:
          case 2:
          case 3: {
            conv2_unpack_store_sym(input_p, height_p, width_p,
              conv_matrix, height_c, width_c,
              output_f, width_e, height_e,
              __num_pack, b, bits);
                  }
                  break;

          default: {
            conv2_unpack_store(input_p, height_p, width_p,
              conv_matrix, height_c, width_c,
              output_f, width_e, height_e,
              __num_pack, b, bits);
                   }
                   break;
          }
          perf_timer.stop_and_update();

          // --- handling correctly parsing progression
          if ( (dx + 1) == parser->get_progression_steps() )  // last step
          {
            width_p = width;
            width_e = width;
          }
          // ---
#ifdef _OPENMP
          //#pragma omp atomic
          if ( flag_int == true )
          {
            cout << "(" << (NUM_BITPLANE - b) << ")";
            num_incomplete_frame++;
            break;
          }
#endif

        }
        if ( parser->get_output_filename() != DEFAULT_OUT_FILE )
        {
          write_matrix_full(output_f, width, height, ofile);
        }

#ifdef _OPENMP
#pragma omp critical
        {
          flag_int = false;
          sleep_timer.restart();
        } 
#endif

      }
      perf_timer_f.stop_and_update();

#ifdef _OPENMP
      flag_scheduler = false;
      sleep_timer.stop(); // stop timer
#endif

    } // thread 1 -----  #pragma omp section

#ifdef _OPENMP
#pragma omp section
    // thread 2
    {
      int susp_t = time;

      if ( jitter == 0 ) {
        while (flag_scheduler)
        {
          sleep_timer.set_and_go(susp_t);
          flag_int = true;
        }
      } else {
        srand ( 0 );
        while (flag_scheduler)
        {
#pragma omp critical
          {
            susp_t = (rand() % jitter) + time;
          }
          sleep_timer.set_and_go(susp_t);
          flag_int = true;
        }
      }
    } // end thread 2
#endif

  } // #pragma omp parallel sections num_threads(2)

  cout << endl;

  exit_high_priority();

  //cout << "IN (example output for debug purposes) =" << endl;
  //print_matrix(input_f, width, height, PRINT, PRINT); 
  //print_matrix(input_f, PRINT, PRINT);

  //cout << "OUT =" << endl;
  //print_matrix(output_f, width, height, PRINT, PRINT);
  //print_matrix(output_f, PRINT, PRINT);

  cout << endl;
  cout << " Execution time (msec): " << perf_timer.get_time()  << endl;
  cout << " Execution time (msec) + I/O: " << perf_timer_f.get_time()  << endl;
  cout << " Average execution time per frame (msec): " << perf_timer.get_time()/parser->get_frame_num()  << endl;
  cout << " Average execution time per frame w/ I/O (msec): " << perf_timer_f.get_time()/parser->get_frame_num()  << endl;
#ifdef _OPENMP
  cout << " Number of incomplete frame: " << num_incomplete_frame << " / " << parser->get_frame_num() << " ( " << ((double)(num_incomplete_frame)/parser->get_frame_num())*100 <<" % )" <<endl;
#endif

  // ------------ clean up -------------------------------------------------------
  free_matrix(conv_matrix, width_c, height_c);

  free_matrix(input_f, width, height);
  free_matrix(input_b, width, height);

  free_matrix(input_p, width_p, height_p);
  free_matrix(output_p, width_p, height_p);

  free_matrix(output_f, width, height);
  // -----------------------------------------------------------------------------

  return 0;                                   // 0: ok!
}