/* ORIP_v2 –> Op Ref Im Proc -> Oper Refin Imag Process -> Operational Refinement of Image Processing
 *
 * Copyright Davide Anastasia and Yiannis Andreopoulos, University College London
 *
 * http://www.ee.ucl.ac.uk/~iandreop/ORIP.html
 * If you have not retrieved this source code via the above link, please contact danastas@ee.ucl.ac.uk and iandreop@ee.ucl.ac.uk
 * 
 */


#define _CRT_SECURE_NO_DEPRECATE

#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>

#include "commandlineparser.h"
#include "common_lib.h"
#include "library.h"
#include "timing.h"

using namespace std;

// boolean for threading cooperation
#ifdef _OPENMP
#include <omp.h>
#include "sleep_t.h"

bool flag_int = false;
bool flag_scheduler = true;
#endif
// ---

int main(int ac, char* av[])
{
#ifdef _OPENMP
  cout << " Block Transformation & Scheduling: Two Threads (Main Loop + Scheduler)" << endl;
  cout << " OpenMP Enabled: " << _OPENMP << endl;
#else
  cout << " Block Transformation" << endl;
#endif

  // TIMER -----
  My_Timer perf_timer;
  My_Timer perf_timer_f;

#ifdef _OPENMP
  Sleep_T sleep_timer;
  int num_incomplete_frame = 0;
#endif
  // -----------

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
      cerr << "[!] Output file cannot be open" << endl;
      exit(1);
    }
  }

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
  // int last_bitplane = NUM_BITPLANE + 1 - parser->get_bitplane();
  set_mask(parser->get_bitplane());

  int width = parser->get_width();
  int height = parser->get_height();
  int width_c = parser->get_height_conv(); //parser->get_block_dim();
  //int height_c = parser->get_height_conv();

  matrix input = create_matrix<my_t>(width, height);
  matrix output = create_matrix<my_t>(width, height);

  matrix block;
  switch ( parser->get_mode() )
  {
  case 1: {
    // 4x4 AVC
    width_c = 4;
    block = create_matrix<my_t>(width_c, width_c);
    setup_block_avc(block);
    break;
          }
  case 2: {
    width_c = 8;
    block = create_matrix<my_t>(width_c, width_c);
    setup_block_bin_dct(block);
    break;
          }
  case 3: {
    width_c = 8;
    block = create_matrix<my_t>(width_c, width_c);
    setup_block_frext(block);
    break;
          }
  case 4: {
    // 4x4 AVC
    width_c = 4;
    block = create_matrix<my_t>(width_c, width_c);
    setup_block_avc_inv(block);
    break;
          }
  default:
    block = create_matrix<my_t>(width_c, width_c);
    setup_block_matrix(block, width_c, parser->get_kernel_max_value());
  }

  matrix input_b = create_matrix<my_t>(width_c, width_c);
  matrix temp_b = create_matrix<my_t>(width_c, width_c);
  matrix output_b = create_matrix<my_t>(width_c, width_c);

  cout << "CONV =" << endl;
  print_matrix(block, width_c, width_c);
  // -----------------------------------------------------------------------------

  start_high_priority();

  int blocks = (width / width_c) * (height / width_c);
  //int max = 0;

#pragma omp parallel sections num_threads(2) 
  {
#pragma omp section
    // thread 1
    {

      perf_timer_f.start();
      for (int f = 0; f < parser->get_frame_num(); f++)
      {
        reset_matrix(output, width, height);

        fill_matrix(input, width, height, ifile);			// read frame
        mask_matrix(input, input, width, height);

        for ( int nx = 0; nx < blocks; nx++ )
        {
          reset_matrix(input_b, width_c, width_c);
          reset_matrix(temp_b, width_c, width_c);
          reset_matrix(output_b, width_c, width_c);

          perf_timer.start();					// ---- timer!
          read_block(input, width, height, input_b, width_c, width_c, nx);

          block_multiply(input_b, block, width_c, width_c, temp_b);
          block_multiply_t(temp_b, block, width_c, width_c, output_b);

          write_block(output_b, width_c, width_c, output, width, height, nx);

          perf_timer.stop_and_update();			// ---- stop timer!
          // ----------------- END -----------------------------------------
#ifdef _OPENMP
          if ( flag_int == true )
          {
            num_incomplete_frame++;
            cout << "+";
            break;
          }
#endif
        }

        if ( parser->get_output_filename() != DEFAULT_OUT_FILE )
        {
          write_matrix_full(output, width, height, ofile);
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

  //cout << "IN = " << endl;
  //print_matrix(input, PRINT, PRINT);

  //cout << "OUT = " << endl;
  //print_matrix(output, PRINT, PRINT);

  cout << endl;
  cout << " Execution time (msec): " << perf_timer.get_time()  << endl;
  cout << " Execution time (msec) + I/O: " << perf_timer_f.get_time()  << endl;
  cout << " Average execution time per frame (msec): " << perf_timer.get_time()/parser->get_frame_num()  << endl;
  cout << " Average execution time per frame w/ I/O (msec) : " << perf_timer_f.get_time()/parser->get_frame_num()  << endl;
#ifdef _OPENMP
  cout << " Number of incomplete frame: " << num_incomplete_frame << " / " << parser->get_frame_num() << " ( " << ((double)(num_incomplete_frame)/parser->get_frame_num())*100 <<" % )" <<endl;
#endif

  // ------------ clean up -------------------------------------------------------
  free_matrix(input, width, height);
  free_matrix(block, width_c, width_c);
  free_matrix(output, width, height);
  // -----------------------------------------------------------------------------

  return 0;                                   // 0: ok!
}

