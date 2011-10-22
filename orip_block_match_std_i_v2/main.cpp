/* ORIP_v2 –> Op Ref Im Proc -> Oper Refin Imag Process -> Operational Refinement of Image Processing
 *
 * Copyright Davide Anastasia and Yiannis Andreopoulos, University College London
 *
 * http://www.ee.ucl.ac.uk/~iandreop/ORIP.html
 * If you have not retrieved this source code via the above link, please contact danastas@ee.ucl.ac.uk and iandreop@ee.ucl.ac.uk
 * 
 */


#define   _CRT_SECURE_NO_DEPRECATE
//#define   DEBUG

#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

#include "commandlineparser.h"
#include "common_lib.h"
#include "library.h"
#include "timing.h"

#ifdef _WIN32
#include <windows.h>
#endif

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
  cout << " Block Matching & Scheduling: Two Threads (Main Loop + Scheduler)" << endl;
  cout << " OpenMP Enabled: " << _OPENMP << endl;
#else
  cout << " Block Matching" << endl;
#endif

#ifdef _OPENMP
  Sleep_T sleep_timer;
  int num_incomplete_frame = 0;
#endif

  // TIMER -----
  My_Timer perf_timer_f;
  My_Timer perf_timer;
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
    cerr << "Binary image doesn't exist or there's possibly a wrong path" << endl;
    exit(1);
  }

  if ( (ofile = fopen(parser->get_output_filename().c_str(), "wb")) == NULL )
  {
    cerr << "Binary image doesn't exist or there's possibly a wrong path" << endl;
    exit(1);
  }

  // -----------------------------------------------------------------------------
  int frame = (parser->get_frame_num() > 299) ? 299 : parser->get_frame_num();

  set_mask(parser->get_bitplane());

  int width = parser->get_width();
  int height = parser->get_height();

  int search_range = parser->get_search_range();
  int block_size = parser->get_block_dim();
  int search_area_size = search_range*2 + block_size;
  int search_size = search_range*2;// + 1;

  int w_blks = width/block_size;
  int h_blks = height/block_size;

  int NoB = w_blks * h_blks;

  matrix input = create_matrix<my_t>(width, height);
  matrix input_b = create_matrix<my_t>(width, height);

  matrix input_curr = create_matrix<my_t>(width, height);
  matrix input_curr_b = create_matrix<my_t>(width, height);

  matrix block_b = create_matrix<my_t>(block_size, block_size);
  matrix search_area_b = create_matrix<my_t>(search_area_size, search_area_size);

  matrix output = create_matrix<my_t>(width, height);

  match** output_t = new match*[frame];
  for (int f = 0; f < frame; f++) {
    output_t[f] = new match[NoB];
  }
  matrix block_t = create_matrix<my_t>(block_size, block_size);

  // -----------------------------------------------------------------------------
  cout << setw(15) << "Block Size:" << " (" << block_size << "," << block_size << ")" << endl;
  cout << setw(15) << "Search Range:" << " (" << -search_range << "," << -search_range << ")" << " (" << search_range << ","  << search_range << ") " << endl;
  // -----------------------------------------------------------------------------

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

  start_high_priority();

  // read reference frame
  fill_matrix(input, width, height, ifile);
  mask_matrix_and_shift(input, input_b, width, height);
  write_matrix(input, width, height, ofile);

#pragma omp parallel sections num_threads(2) 
  {
#pragma omp section
    // thread 1
    {
      int x, y;
      perf_timer_f.start();
      for (int f = 0; f < frame; f++)
      {
        fill_matrix(input_curr, width, height, ifile);       // read frame
        mask_matrix_and_shift(input_curr, input_curr_b, width, height);

        for (int b_idx = 0; b_idx < NoB; b_idx++)
        {
          y = block_size*((b_idx)/w_blks);
          x = block_size*((b_idx)%w_blks);

          perf_timer.start();

#ifdef _OPENMP
          if (flag_int == true) //break;
          {
            output_t[f][b_idx].dest_x = x;
            output_t[f][b_idx].dest_y = y;
            
            output_t[f][b_idx].src_x = x;
            output_t[f][b_idx].src_y = y;
          }
          else
          {
#endif
            read_block(input_curr_b, width, height, x, y, block_b, block_size, block_size);
            read_search_area(input_b, width, height, x, y, search_area_b, search_area_size, search_area_size, search_range);

            block_match_i_spiral(search_area_b, search_size, search_size,
              block_b, block_size, block_size,
              x, y, search_range, output_t[f][b_idx], search_range);
            //block_match_and_store(search_area_b, search_size, search_size, block_b, block_size, block_size,
            //  x, y, width, height, search_range, output_t[f][b_idx]);

#ifdef _OPENMP
            //if (flag_int == true) break;
          }
#endif
          perf_timer.stop_and_update();
        }

        pswap<matrix>(&input, &input_curr);
        pswap<matrix>(&input_b, &input_curr_b);
        // ----------------- END -----------------------------------------

#ifdef _OPENMP
#pragma omp critical
        {
          if ( flag_int == true )
          {
            num_incomplete_frame++;
            cout << "+";
          }
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

  exit_high_priority();

  if ( parser->get_output_filename() != DEFAULT_OUT_FILE )
  {
    cout << "|";
    if ( (ofile = fopen(parser->get_output_filename().c_str(), "wb")) == NULL )
    {
      cerr << "[!] Binary image doesn't exist or there's possibly a wrong path" << endl;
      exit(1);
    }

    // start from begin
    fseek(ifile, 0, SEEK_SET);

    // read reference frame
    fill_matrix(input, width, height, ifile);
    write_matrix(input, width, height, ofile);

    for (int f = 0; f < frame; f++) // -1 because the first is the ref img
    {
      fill_matrix(input_curr, width, height, ifile);       // read frame

      for ( int b_idx = 0; b_idx < NoB; b_idx++ )
      {
        read_block(input, width, height, output_t[f][b_idx].dest_x, output_t[f][b_idx].dest_y, block_t, block_size, block_size);
        write_block(block_t, block_size, block_size, output_t[f][b_idx].src_x, output_t[f][b_idx].src_y, output, width, height);
      }
      write_matrix(output, width, height, ofile);

      pswap<matrix>(&input, &input_curr);
    }
  }

  cout << endl;
  cout << " Execution time (msec): " << perf_timer.get_time()  << endl;
  cout << " Execution time (msec) + I/O: " << perf_timer_f.get_time()  << endl;
  cout << " Average execution time per frame (msec): " << perf_timer.get_time()/frame  << endl;
  cout << " Average execution time per frame w/ I/O (msec) : " << perf_timer_f.get_time()/frame << endl;
#ifdef _OPENMP
  cout << " Number of incomplete frame: " << num_incomplete_frame << " / " << frame << " ( " << ((double)(num_incomplete_frame)/frame)*100 <<" % )" << endl;
#endif


  // ------------ clean up -------------------------------------------------------
  //free_matrix(input, width, height);
  //free_matrix(input_ref, width, height);

  //free_matrix(block_ref, block_size, block_size);
  //free_matrix(output, width/block_size, height/block_size);
  // -----------------------------------------------------------------------------

  return 0;                                   // 0: ok!
}

