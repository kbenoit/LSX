#include <Rcpp.h>
#include <vector>
// [[Rcpp::plugins(cpp11)]]
#include <unordered_set>

using namespace Rcpp;

void flag_collocates_cpp(const std::vector<std::string> &text,
                         const std::unordered_set<std::string> &set_targets,
                         const int &window,
                         std::vector<bool> &flags_target,
                         std::vector<bool> &flags_col,
                         int &g){

    int len = text.size();
    for(int i=0; i < len; i++){
      std::string token = text[i];
      bool is_in = set_targets.find(token) != set_targets.end();

      if(is_in){
        //Rcout << "Match " << token << " " << i<< "\n";
        for(int j = std::max(0, i - window); j < std::min(i + window + 1, len); j++){
          //Rcout << "Flag " << token << " " << j << "\n";
          flags_col[g + j] = flags_col[g + j] || TRUE;
        }
        flags_target[g + i] = TRUE;
      }else{
        flags_col[g + i] = flags_col[g + i] || FALSE;
        flags_target[g + i] = FALSE;
      }
    }
    g += len; // last global index of this text
  }

// [[Rcpp::export]]
List flag_collocates_cppl(List texts,
                          const std::vector<std::string> &targets,
                          const int &window,
                          const int &n) {

  int g = 0; // global index;
  int len = texts.size();
  std::unordered_set<std::string> set_targets (targets.begin(), targets.end());
  std::vector<bool> flags_target(n);
  std::vector<bool> flags_col(n);
  for (int h = 0; h < len; h++){
    flag_collocates_cpp(texts[h], set_targets, window, flags_target, flags_col, g);
  }

  return List::create(Rcpp::Named("target")=flags_target,
                      Rcpp::Named("col")=flags_col);
}


/*** R
flag_collocates_cppl(list(LETTERS, letters), c('E', 'G', 'Z', 'a', 'k'), 3, 52)
*/
