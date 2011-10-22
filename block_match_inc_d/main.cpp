/* ORIP_v2 –> Op Ref Im Proc -> Oper Refin Imag Process -> Operational Refinement of Image Processing
 *
 * Copyright Davide Anastasia and Yiannis Andreopoulos, University College London
 *
 * http://www.ee.ucl.ac.uk/~iandreop/ORIP.html
 * If you have not retrieved this source code via the above link, please contact danastas@ee.ucl.ac.uk and iandreop@ee.ucl.ac.uk
 * 
 */


//#define   DEBUG
#define   _CRT_SECURE_NO_DEPRECATE
#define   REF_PACK_TYPE (2)
#define   REF_PACK_PATTERN (2)
#define   BLOCK_PACK_TYPE (2)

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
  My_Timer perf_timer;
  My_Timer perf_timer_f;
  // -----------
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
  // ---
  parser->print_progression_pattern();
  // -----------------------------------------------------------------------------

  // -----------------------------------------------------------------------------
  int frame = (parser->get_frame_num() > 299) ? 299 : parser->get_frame_num();

  // --- FRAME
  int width = parser->get_width();
  int height = parser->get_height();

  // --- BLOCK
  int block_size = parser->get_block_dim();

  // --- SEARCH RANGE
  int search_range      = parser->get_search_range();
  int search_area_size  = search_range*2 + block_size - 1;
  int search_size       = search_range*2;

  int num_h_blks  = width/block_size;   // horizontal axis blocks
  int num_v_blks  = height/block_size;  // vertical axis blocks
  int NoB         = num_h_blks * num_v_blks;

  // --- calculate max value for packing ----
  setup_env(block_size, block_size, parser->get_max_progression_step());
  show_env_status();
  // --------------------

  // ---
  cout << setw(15) << "Block Size:" << " (" << block_size << "," << block_size << ")" << endl;
  cout << setw(15) << "Search Range:" << " (" << -search_range << "," << -search_range << ")" << " (" << search_range << ","  << search_range << ") " << endl;
  cout << setw(15) << "Spiral Size: " << parser->get_spiral_size() << endl;
  // ---

  // --- BUFFERS
  matrix input = create_matrix<int>(width, height);
  matrix input_b = create_matrix<int>(width, height);

  matrix input_ref = create_matrix<int>(width, height);
  matrix input_ref_b = create_matrix<int>(width, height);

  matrix block_b = create_matrix<int>(block_size+1, block_size+1);
  matrix search_area_b = create_matrix<int>(search_area_size+1, search_area_size+1);

  double**  block_p = create_matrix<double>(block_size/2+1, block_size+1);

  double**** ref_area_p = new double***[REF_PACK_PATTERN];
  for (int t = 0; t < REF_PACK_PATTERN; t++ )
  {
    ref_area_p[t] = new double**[REF_PACK_TYPE];
    for (int d = 0; d < REF_PACK_TYPE; d++ )
    {
      ref_area_p[t][d] = create_matrix<double>(search_area_size/2+1, search_area_size+1);
    }
  }

  match** match_t = new match*[frame];
  for (int m = 0; m < frame; m++)
  {
    match_t[m] = new match[NoB];
  }

  matrix output = create_matrix<int>(width, height);
  matrix block = create_matrix<int>(block_size+1, block_size+1);
  // -----------------------------------------------------------------------------

  vector<int> bitplane = parser->get_progression_pattern();
  vector<int> indexes = parser->get_progression_indexes();

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
  fill_matrix(input_ref, width, height, ifile);
#pragma omp parallel sections num_threads(2) 
  {
#pragma omp section
    // thread 1
    {
      perf_timer_f.start();
      for (int f = 0; f < frame; f++ )
      {
        fill_matrix(input, width, height, ifile);       // read frame

        // --- handling correctly parsing progression
        int bits, b;
        int x, y;
        // ---

        for ( int dx = 0; dx < parser->get_progression_steps(); dx++ )  // prog steps
        {
          perf_timer.start();

          if ( dx == 0 )  // First step of the scanning pattern
          {
            bits = bitplane[dx];
            b = indexes[dx];

            extract_bitplane_matrix_i(input, input_b, width, height, b, bits);
            extract_bitplane_matrix_i(input_ref, input_ref_b, width, height, b, bits);

            for (int b_idx = 0; b_idx < NoB; b_idx++)
            {
              block_index_2_x_y(b_idx, x, y, num_h_blks, block_size);

              // set initial value for every block
              match_t[f][b_idx].src_x = x;
              match_t[f][b_idx].src_y = y;

#ifdef _OPENMP
              // If I use the OPENMP environment, I need to set the default best match to the center block
              if ( flag_int == true )
              {
                match_t[f][b_idx].dest_x = x;
                match_t[f][b_idx].dest_y = y;
              }
              else
              {
#endif
                read_search_area(input_ref_b, width, height, x, y, search_area_b, search_area_size, search_area_size, search_range);
                read_block(input_b, width, height, x, y, block_b, block_size, block_size);

                pack_block(block_b, __num_pack, block_p, block_size, block_size);
                pack_ref_area(search_area_b, __num_pack, ref_area_p, search_area_size, search_area_size);

                //block_match_d(ref_area_p, search_area_size, search_area_size, block_p, block_size, block_size, x, y, search_range, match_t[f][b_idx]);
                block_match_d_log_search(ref_area_p, search_area_size, search_area_size, block_p, block_size, block_size, x, y, search_range, match_t[f][b_idx], (int)(parser->get_spiral_size()/2 + 1));

#ifdef _OPENMP
                // stop function execution
                // if ( flag_int == true ) break;
              }
#endif
            }
          }
          else   // refinement of the current position
          {
            bits += bitplane[dx]; // get all the bitplanes

            extract_bitplane_matrix_i(input, input_b, width, height, b, bits);
            extract_bitplane_matrix_i(input_ref, input_ref_b, width, height, b, bits);

            for (int b_idx = 0; b_idx < NoB; b_idx++)
            {
              // takes coordinates from the already calculated value
              x = match_t[f][b_idx].src_x;
              y = match_t[f][b_idx].src_y;

              read_search_area(input_ref_b, width, height, x, y, search_area_b, search_area_size, search_area_size, search_range);
              read_block(input_b, width, height, x, y, block_b, block_size, block_size);

              block_match_d_spiral(search_area_b, search_area_size, search_area_size, block_b, block_size, block_size, x, y, search_range, match_t[f][b_idx], parser->get_spiral_size());

#ifdef _OPENMP
              // stop function execution
              if ( flag_int == true ) break;
#endif
            }
          }

          perf_timer.stop_and_update();

#ifdef _OPENMP
          //#pragma omp atomic
          if ( flag_int == true )
          {
            cout << "(" << (bits) << ")";
            num_incomplete_frame++;
            break;
          }
#endif
        }

        // SWAP
        pswap<matrix>(&input, &input_ref);
        // ----------------- END -----------------------------------------

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
    fill_matrix(input_ref, width, height, ifile);
    write_matrix(input_ref, width, height, ofile);

    for (int f = 0; f < frame; f++) // -1 because the first is the ref img
    {
      fill_matrix(input, width, height, ifile);       // read frame

      for ( int b_idx = 0; b_idx < NoB; b_idx++ )
      {
        //cerr << f << "," << match_t[f][b_idx].src_x << "," << match_t[f][b_idx].src_y << "," << match_t[f][b_idx].dest_x << "," << match_t[f][b_idx].dest_y << "," << match_t[f][b_idx].m_value << endl;
        //getchar();

        read_block(input_ref, width, height, match_t[f][b_idx].dest_x, match_t[f][b_idx].dest_y, block, block_size, block_size);
        write_block(block, block_size, block_size, match_t[f][b_idx].src_x, match_t[f][b_idx].src_y, output, width, height);
      }
      write_matrix(output, width, height, ofile);

      pswap<matrix>(&input, &input_ref);
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

  //free_matrix(input_b, width, height);
  //free_matrix(input_ref_b, width, height);

  //free_matrix(block_ref_b, block_size, height_b);
  //free_matrix(output, width/block_size, height/block_size);
  // -----------------------------------------------------------------------------

  return 0;                                   // 0: ok!
}

