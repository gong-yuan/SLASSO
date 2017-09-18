#include <RcppArmadillo.h>
#include <cmath>
// [[Rcpp::depends(RcppArmadillo)]]
using namespace Rcpp;
using namespace std;

const double err = 1e-10;
// [[Rcpp::export]]
arma::mat projection_matrix(const arma::mat X, arma::mat projection,const IntegerVector new_idx,const int n){
		arma::mat I(n,n,arma::fill::eye);
		arma::mat new_projection;
		new_projection = projection;
		int l = new_idx.size();
		for(int i=0; i < l; i++){
			arma::mat xs;
			xs = X.col(new_idx[i]);
			new_projection = new_projection * (I - ((xs * xs.t() * new_projection) / arma::as_scalar(xs.t() * new_projection * xs))) ;
		}
		return new_projection;
}

int factorial(int n){
  if(n > 1)
    return n * factorial(n - 1);
  else
    return 1;
}

int combination(int m, int n){
  return factorial(m)/(factorial(n)*factorial(m-n));
}

// [[Rcpp::export]]
double EBIC(const arma::colvec res_y, int s, int p, double r){
    double ebic;
    int n = y.size();
    ebic =  n*log(arma::as_scalar(res_y.t()*res_y)/n);
    ebic = ebic + s*log(n) + 2*(1- log(n)/(r*log(p)))*log(combination(p,s));
    return ebic;
}

// [[Rcpp::export]]
List SLasso(const arma::colvec y, const arma::mat X, double r = 2.1){
    int n = X.n_rows;
    int p = X.n_cols;
    
    //normalization
    arma::colvec norm_y;
    arma：：mat norm_X;
      
    arma::uvec candidate(n);
    arma::uvec selected;
    uword temp_select;
    
    arma::mat proj(n,n,arma::fill::eye);
    
    for(int i=0; i<n; i++){
        candidate(i) = i;
    }
    
    double ebic;
    arma::colvec res_y = proj * norm_y; 
    int s = 0;
    ebic = EBIC(res_y, s, p, r);
    
     
    while(true){
        arma::colvec tmp = norm_X.cols(candidate).t() * res_y;
        tmp = abs(tmp);
        temp_select = tmp.index_max();
        candidate.shed_row(temp_select);
        temp_select = candidate(temp_select);
        proj = projection_matrix(norm_X,proj,temp_select,n);
        double new_ebic;
        res_y = proj * norm_y;
        new_ebic = EBIC(res_y, s+1, p, r);
        if (new_ebic>ebic){
            break;
        } else{
            selected = join_rows(selected,temp_select);
            s++;
        }
    }
    
    arma::mat select_X = X.cols(selected);
    arma::colvec coef;
    coef = arma::inv_sympd(select_X.t() * select_X) * select_X.t() * y;
    
    selected.each_row() += 1;
    return List::create(Named("Select.Features") = selected,
                        Named("Coef") = coef);
    
}