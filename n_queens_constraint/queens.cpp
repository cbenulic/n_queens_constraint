/*
Constraint solution of the n queens problem using Gecode.
N queens are placed on a board according to the rules of
the n queens problem. Number of queens is changed by changing
opt.size in main function.
*/

#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>


using namespace Gecode;

class Queens : public Script {
public:
  /// Array for position of queens on boards
  IntVarArray spaces;


  Queens(const SizeOptions& opt)
    : spaces(*this,opt.size()*opt.size(),0,1) {

	Matrix<IntVarArray> placement(spaces,opt.size(),opt.size());

	///Constraint for only one queen in every row and column
	for(int i=0; i < opt.size(); i++){
		linear(*this, placement.row(i), IRT_EQ, 1);
		linear(*this, placement.col(i), IRT_EQ, 1);
	}
	
	//Constraint for only one queen in first main diagonal (from 0,0 to 7,7)
	IntVarArgs mdiagonal1(opt.size());
	for(int i=0; i<opt.size(); i++)
		mdiagonal1[i] = placement(i,i);
	linear(*this, mdiagonal1, IRT_LQ, 1);

	//Constraint for only one queen in second main diagonal (from 0,7 to 7,0)
	IntVarArgs mdiagonal2(opt.size());
	for(int i=0; i<opt.size(); i++)
		mdiagonal2[i]=placement(i,opt.size()-i-1);
	linear(*this, mdiagonal2, IRT_LQ, 1);

	//Constraint for the areas "over" and "under" the first main diagonal
	for(int j=1; j<opt.size()-1; j++){
		IntVarArgs diagonal1(opt.size()-j);
		IntVarArgs diagonal2(opt.size()-j);
		for(int i=0; i<opt.size()-j; i++){
			diagonal1[i]=placement(i+j,i);
			diagonal2[i]=placement(i,i+j);
		}
		linear(*this, diagonal1, IRT_LQ, 1);
		linear(*this, diagonal2, IRT_LQ, 1);
	}

	//Constraint for the area "over" the second main diagonal
	for(int j=1; j<opt.size()-1; j++){
		IntVarArgs diagonal(opt.size()-j);
		for(int i=0; i<opt.size()-j; i++)
			diagonal[i]=placement(j+i,opt.size()-i-1);
		linear(*this, diagonal, IRT_LQ, 1);
	}

	//Constraint for the area "under" the second main diagonal
	for(int i=1; i<opt.size()-1; i++){
		IntVarArgs diagonal(i+1);
		for(int j=0; j<=i; j++)
			diagonal[j]=placement(j,i-j);
		linear(*this, diagonal, IRT_LQ, 1);
	}
    
    branch(*this, spaces, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
  }

  /// Constructor for cloning
  Queens(bool share, Queens& s) : Script(share,s) {
    spaces.update(*this, share, s.spaces);
  }

  /// Perform copying during cloning
  virtual Space*
  copy(bool share) {
    return new Queens(share,*this);
  }

  /// Prints a list of spaces which have a queen

  void print() const {
    for (int i=0; i<spaces.size(); i++)
		if(spaces[i].val()==1){
			int row = i/sqrt(spaces.size());
			int col = i%(int)sqrt(spaces.size());
			std::cout << " " << row << "," << col << "\n";
		}
  }
};

///Main function
int main(int argc, char* argv[]) {
  int solution = 1;
  SizeOptions opt("Queens");
  opt.size(8);
  opt.parse(argc,argv);
  Queens* qu = new Queens(opt);
  DFS<Queens> e(qu);
  while (Queens* qu = e.next()) {
	std::cout << "Solution " << solution << "\n";
	qu->print();
	delete qu;
	solution++;
  }
  return 0;
}
