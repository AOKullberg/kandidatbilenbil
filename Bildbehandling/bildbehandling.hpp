/* FIL bildbehandling.hpp
 * Header för bildbehandlingen för sensormodulen
 */
#ifndef bildbehandling_h
#define bildbehandling_h
#include <opencv2/core/core.hpp>

/* Definition av funktioner
 */
int img_proc_main();
int process_image(int);
void get_distance_to_stop();
void get_distance_to_line(cv::Mat, int);
cv::Mat split_to_green(cv::Mat);
cv::Mat detect_lines(cv::Mat, cv::Mat);
cv::Mat detect_signs(cv::Mat);
cv::Mat detect_edges(cv::Mat);

#endif
