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
#include <ctime>

#include <functional>
#include <vector>
#include <algorithm>

#include "commandlineparser.h"
#include "common_lib.h"
#include "library.h"
#include "timing.h"

#include <stdio.h>
#include <stdlib.h>

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

  int width = parser->get_width();
  int height = parser->get_height();
  int width_c = parser->get_height_conv(); //parser->get_block_dim();
  //int height_c = parser->get_height_conv();

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
  default:
    block = create_matrix<my_t>(width_c, width_c);
    setup_block_matrix(block, width_c, parser->get_kernel_max_value());
  }

  cout << "REF BLOCK =" << endl;
  print_matrix(block, width_c, width_c);
  // -----------------------------------------------------------------------------
  double const last_step_perc = parser->get_last_step_percentage();
  int const blocks_orig = (width / width_c) * (height / width_c);
  int blocks = blocks_orig;

  // --- calculate max value for packing ----
  setup_env(block, width_c, parser->get_max_progression_step());
  show_env_status();

  int num_pack_t = __num_pack;
  // --------------------

  // -----------------------------------------------------------------------------
  // -- buffers ------------------------------------------------------------------
  matrix input_f = create_matrix<my_t>(width, height);
  matrix input_b = create_matrix<my_t>(width, height);

  matrix* i_block_b = new matrix[__num_pack];
  for (int i = 0; i < __num_pack; i++) i_block_b[i] = create_matrix<my_t>(width_c, width_c);

  matrix input_p = create_matrix<my_t>(width_c, width_c);
  matrix output_p_t = create_matrix<my_t>(width_c, width_c);
  matrix output_p = create_matrix<my_t>(width_c, width_c);

  matrix* o_block_b = new matrix[__num_pack];
  for (int i = 0; i < __num_pack; i++) o_block_b[i] = create_matrix<my_t>(width_c, width_c);

  matrix output_f = create_matrix<my_t>(width, height);
  // -----------------------------------------------------------------------------

  vector<int> bitplane = parser->get_progression_pattern();
  vector<int> indexes = parser->get_progression_indexes();

  start_high_priority();

#pragma omp parallel sections num_threads(2) 
  {
#pragma omp section
    // thread 1
    {
      perf_timer_f.start();
      for ( int frames = 0; frames < parser->get_frame_num(); frames++ )
      {
        // reset accumulation buffers
        reset_matrix(output_f, width, height);

        // read frames
        fill_matrix(input_f, width, height, ifile);

        // --- handling correctly parsing progression
        int bits, b;
        // ---

        for ( int dx = 0; dx < parser->get_progression_steps(); dx++ )  // prog steps
        {
          bits = bitplane[dx];
          b = indexes[dx];
          if ( (dx + 1) == parser->get_progression_steps() )  // last step
          { 
            blocks = int(blocks*last_step_perc);
          }

          reset_matrix(output_p, width_c, width_c);
          reset_matrix(output_p_t, width_c, width_c);

          // extract bitplane
          extract_bitplane_matrix_i(input_f, input_b, width, height, b, bits);

          for (int nx = 0; nx < blocks; nx += num_pack_t)
          {
            if ( nx + num_pack_t > blocks )
            {
              num_pack_t = blocks - nx;
            }

            // ---- TIMER ---
            perf_timer.start();
            // --------------

            // read __num_pack blocks
            for ( int mx = 0; mx < num_pack_t; mx++ ) read_block(input_b, width, height, i_block_b[mx], width_c, width_c, (mx + nx));

            pack_matrix(i_block_b, num_pack_t, input_p, width_c, width_c);

            block_multiply(input_p, block, width_c, width_c, output_p_t);
            block_multiply_t(output_p_t, block, width_c, width_c, output_p);

            unpack_matrix_and_store_i(output_p, o_block_b, num_pack_t, width_c, width_c /*, b, bits */);

            // write __num_pack blocks & accumulate
            for ( int mx = 0; mx < num_pack_t; mx++ ) write_block(o_block_b[mx], width_c, width_c, output_f, width, height, (mx + nx), b, bits);

            // ---- TIMER ----
            perf_timer.stop_and_update();
            // ---------------

            reset_matrix(input_p, width_c, width_c);
            reset_matrix(output_p_t, width_c, width_c);
            reset_matrix(output_p, width_c, width_c);

#ifdef _OPENMP
            if ( flag_int == true ) break;
#endif
          }
          num_pack_t = __num_pack;

          // --- handling correctly parsing progression
          if ( (dx + 1) == parser->get_progression_steps() )  // last step
          {
            blocks = blocks_orig;
          }
          // ---
#ifdef _OPENMP
          //#pragma omp critical
          {
            if ( flag_int == true )
            {
              cout << "(" << (NUM_BITPLANE - b) << ")";
              num_incomplete_frame++;
              break;        
            }
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

  //cout << "IN(full) =" << endl;
  //print_matrix(input_f, PRINT, PRINT);

  //cout << "IN (last bitplane) =" << endl;
  //print_matrix(input_b, width_c, height_c);

  //cout << "OUT =" << endl;
  //print_matrix(output_f, PRINT, PRINT);

  cout << endl;
  cout << " Execution time (msec): " << perf_timer.get_time()  << endl;
  cout << " Execution time (msec) + I/O: " << perf_timer_f.get_time()  << endl;
  cout << " Average execution time per frame (msec): " << perf_timer.get_time()/parser->get_frame_num()  << endl;
  cout << " Average execution time per frame w/ I/O (msec) : " << perf_timer_f.get_time()/parser->get_frame_num()  << endl;
#ifdef _OPENMP
  cout << " Number of incomplete frame: " << num_incomplete_frame << " / " << parser->get_frame_num() << " ( " << ((double)(num_incomplete_frame)/parser->get_frame_num())*100 <<" % )" <<endl;
#endif


  // ------------ clean up -------------------------------------------------------
  //free_matrix(block, width_c, width_c);

  //for (int i = 0; i < __num_pack; i++) free_matrix(input_f[i], width, height);
  //for (int i = 0; i < __num_pack; i++) free_matrix(input_b[i], width, height);

  //free_matrix(input_p, width, height);
  //free_matrix(output_p, width, height);

  //for (int i = 0; i < __num_pack; i++) free_matrix(output_f[i], width, height);
  // -----------------------------------------------------------------------------

  return 0;                                   // 0: ok!
}

