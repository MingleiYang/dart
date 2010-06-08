#include<iostream>
#include "transducer.h"
#include "ecfg/ecfgsexpr.h" // for array2d class
#include<math.h>

//General transducer operations
string Transducer::get_state_name(state m)
{
  if (m >= states.size())
	{ 
	  std::cerr<<"Error: state "<<m<<"has no name, since it is not in the transducer " << name << " which has "<<states.size()<<" states \n";
	  exit(1);
	}
  else 
	{
	  return state_names[m];
	}
}

vector<state> Transducer::get_state_index(string state_name)
{
  vector<state> out;
  for (int i=0; i<state_names.size(); i++)
	{ 
	  if(state_names[i] == state_name) out.push_back(i);
	}
  
  if (out.empty()) 
	{
	  std::cerr<<"Error: state "<<state_name<<" has no index, since it is not in the transducer  " << name << " which has the following states  ";
  displayVector(state_names);
  exit(1);
	}
  else return out;

}

string Transducer::get_state_type(state m)
{
  if (m >= states.size())
	{
	  std::cerr<<"Error: state "<<m<<"has no type, since it is not in the transducer  " << name << " which has "<<states.size()<<" states \n";
	  exit(1);
	}
  else return(state_types[m]);
}

vector<state> Transducer::get_state_type_set(string type)
{
  vector<state> out; 
  for (int i=0; i<state_types.size(); i++)
	{
	  if (state_types[i] == type) out.push_back(i); 
	}
  return out; 
}

vector<state> Transducer::get_outgoing(state m)
{
  if (outgoing.count(m)>0) return outgoing[m];
  else
	{
	  std::cerr<<"Error: state "<<m<<" has no outgoing connections\n";
	  exit(1);
	}
}

bool Transducer::has_transition(state m, state mPrime)
{
  if (outgoing.count(m)<0) return false; 
  else
	{
	  if (index(mPrime, outgoing[m]) == -1) return false;
	  else return true;
	}
}

void Transducer::test_transitions(void)
{
  std::cerr<<"Testing transducer "<<name<<"'s transitions...\n";
  double sum; 
  for (int i=0; i<state_names.size(); i++)
	{
	  sum = 0; 
	  std::cerr<<"State "<<i<<": "<<state_names[i]<<" has outgoing transitions to:\n";
	  if (state_names[i]=="end") std::cerr<<"\tNone\n";
	  for (vector<int>::iterator j=outgoing[i].begin(); j!=outgoing[i].end(); j++)
		{
		  std::cerr<<"\tState "<<*j<<": "<<state_names[*j]<<" with weight "<<get_transition_weight(i,*j)<<endl;
		  sum += get_transition_weight(i,*j); 		  
		}
	  std::cerr<<"\tTotal outgoing trans weight for this state: "<<sum<<endl; 
	}
}
  
double Transducer::get_transition_weight(state m, state mPrime)
{
  vector<state> transitionPair;
  transitionPair.push_back(m);  transitionPair.push_back(mPrime);
  if (transition_weight.count(transitionPair) >0) return transition_weight[transitionPair];
  else 
	{
	  std::cerr<<"Error: state "<<m<<" has no nonzero-weight transition to "<<mPrime<<" in transducer  " << name <<endl;
	  std::cerr<<"The outgoing transitions of "<<m<<" are: ";
	  displayVector(get_outgoing(m));
	  exit(1);
	}
}

double Transducer::get_emission_weight(state b, int charIndex)
{
  string insert = "I";
  if (get_state_type(b) != insert) 
	{
	  std::cerr<<"Calling get_emission_weight on a non-insert state is not allowed!\n";
	  std::cerr<<"The offending call was: transducer "<<name<<" state "<<b<<" named: "<<get_state_name(b)<<endl;
	  exit(1);
	}
  else if (charIndex >= alphabet_size || charIndex <0 )
	{	
	  std::cerr<<"Calling get_emission_weight on a non-alphabet index!\n";
	  std::cerr<<"The offending call was: transducer "<<name<<" character index: "<<charIndex<<endl;
	  exit(1);
	}
  else
	{
	  return(emission_weight_matrix.at(b)[charIndex]);	  
	}
}



// Operations for singlet transducers (constructor w/ hard-coded stuff, etc)
SingletTrans::SingletTrans(void)
{
}

SingletTrans::SingletTrans(Alphabet& alphabet_in, Irrev_EM_matrix& rate_matrix)
{
  name = "Singlet";
  alphabet = string(alphabet_in.nondegenerate_chars());  
  alphabet_size = alphabet.size();
  
/*  states:  */
/*   0 = start */
/*   1 = insert */
/*   2 = wait */
/*   3 = end */  
  state_names.push_back("start");  
  state_names.push_back("insert");
  state_names.push_back("wait");
  state_names.push_back("end");

  state_types.push_back("S");  
  state_types.push_back("I");
  state_types.push_back("W");
  state_types.push_back("E");

  for (int i=0; i<state_names.size(); i++) states.push_back(i); 


  // outgoing transitions
  vector<state> out;
  out.push_back(1);out.push_back(2) ;outgoing[0] = out;  // start -> (insert|wait)
  out.clear(); 
  out.push_back(2); outgoing[1] = out; // insert -> wait
  out.clear(); 
  out.push_back(3); outgoing[2] = out; // wait -> end

  // Transition weights - hard coded here.  
  double start2ins = .99;
  double start2wait = .01;
  double ins2ins = .99;
  double ins2wait = .01;

  vector<state> transitionPair; 
  transitionPair.push_back(0); transitionPair.push_back(1); 
  transition_weight[transitionPair] = start2ins; 

  transitionPair.clear();
  transitionPair.push_back(0); transitionPair.push_back(2); 
  transition_weight[transitionPair] = start2wait; 

  transitionPair.clear();
  transitionPair.push_back(1); transitionPair.push_back(1); 
  transition_weight[transitionPair] = ins2ins; 

  transitionPair.clear();
  transitionPair.push_back(1); transitionPair.push_back(2); 
  transition_weight[transitionPair] = ins2wait; 

  transitionPair.clear();
  transitionPair.push_back(2); transitionPair.push_back(3); 
  transition_weight[transitionPair] = 1; 

  // Emission weights
  // The states of type I are the only states which have an emission distribution
  vector<double> equilibrium = rate_matrix.create_prior(); 
  for (int i=0; i<states.size(); i++)
	{
	  if (state_types[i] == "I") emission_weight_matrix[i] = equilibrium; 
	}
}


// Splitting/forking transducer constructor
// In retrospect, this is a bit unnecessary...not sure why I put this in :-)
SplittingTrans::SplittingTrans(void)
{
  name = "Upsilon";

  state_names.push_back("start");  
  state_names.push_back("match");
  state_names.push_back("wait");
  state_names.push_back("end");


  state_types.push_back("S");  
  state_types.push_back("M");
  state_types.push_back("W");
  state_types.push_back("E");

  /*  states:  */
  /*   0 = start */
  /*   1 = match */
  /*   2 = wait */
  /*   3 = end */  
  for (int i=0; i<state_names.size(); i++) states.push_back(i);


  // outgoing transitions
  vector<state> out;
  out.push_back(1); outgoing[0] = out;  // start(0) -> match(1)
  out.clear(); 
  out.push_back(2); outgoing[1] = out; // match(1) -> wait(2)
  out.clear(); 
  out.push_back(1); out.push_back(3); outgoing[2] = out; // wait(2) -> (match(1) | end(3))

  // Transition weights are all 1. This tranducer is dependent on the others around it to make transitions.

  vector<state> transitionPair;  
  transitionPair.push_back(0); transitionPair.push_back(1); 
  transition_weight[transitionPair] = 1; 
  
  transitionPair.clear();
  transitionPair.push_back(1); transitionPair.push_back(2); 
  transition_weight[transitionPair] = 1;

  transitionPair.clear();
  transitionPair.push_back(2); transitionPair.push_back(1); 
  transition_weight[transitionPair] = 1; 

  transitionPair.clear();
  transitionPair.push_back(2); transitionPair.push_back(3); 
  transition_weight[transitionPair] = 1; 

 }

// Branch transducer constructor

BranchTrans::BranchTrans(void)
{
  // Placeholder constructor
  //  std::cerr<<"Initiating a placeholder branch transducer object\n";
}


double BranchTrans::get_match_weight(state b, int incoming_character, int outgoing_character )
{
  // Match weight function.  Branch-length dependent.  Eventually, we'd like to have this use matrix
  // exponentiation to allow for a general character-evo model.  For now, a uniform model is coded
  // here, for testing purposes only!!

  string match = "M"; 
  if (get_state_type(b) != match) 
	{
	  std::cerr<<"Calling get_match_weight on a non-match state!\n";
	  std::cerr<<"The offending call was: transducer "<<name<<" state "<<b<<" named: "<<get_state_name(b)<<endl;
	  exit(1);
	}
  else if (incoming_character >= alphabet_size || outgoing_character >= alphabet_size )
	{
	  
  	  std::cerr<<"Calling get_match_weight on a non-alphabet index!\n";
	  std::cerr<<"The offending call was: transducer "<<name<<" character indices (in,out): "<<incoming_character<<" "<<outgoing_character<<endl;
	  exit(1);
	}
  else
	{
	  double result = conditional_sub_matrix(incoming_character, outgoing_character); 
	  return result; 
// 	  float n = alphabet_size;
// 	  // Uniform match distribution, from early testing days
// 	  if (incoming_character == outgoing_character) return  1/n + ((n-1)/n)*exp(-branch_length*n); 
// 	  else return 1/n - (1/n)*exp(-branch_length*n); 
	}
}  


// affine gap branch transducer
BranchTrans::BranchTrans(double branch_length_in, Alphabet& alphabet_in, Irrev_EM_matrix& rate_matrix, 
						 double ins_open_rate, double del_open_rate, double gap_extend)
{
  branch_length = branch_length_in;
  name = "Branch";
  alphabet = string(alphabet_in.nondegenerate_chars());
  alphabet_size = alphabet.size();
  
  conditional_sub_matrix = rate_matrix.create_conditional_substitution_matrix(branch_length); 

  // Initialize state names
  state_names.push_back("start");  
  state_names.push_back("match");
  state_names.push_back("wait");
  state_names.push_back("delete_wait");  
  state_names.push_back("delete");
  state_names.push_back("insert");
  state_names.push_back("end");

  // Initialize state names
  state_types.push_back("S");  
  state_types.push_back("M");
  state_types.push_back("W");
  state_types.push_back("W");  
  state_types.push_back("D");
  state_types.push_back("I");
  state_types.push_back("E");

  /*  states:  */
  /*   0 = start */
  /*   1 = match */
  /*   2 = wait */
  /*   3 = delete_wait */
  /*   4 = delete */
  /*   5 = insert */  
  /*   6 = end */    

  int start=0, match=1, wait=2, delete_wait=3, del=4, insert=5, end=6; 

  for (int i=0; i<state_names.size(); i++) states.push_back(i); 

  // Set outgoing transitions
  vector<state> out;

  out.push_back(insert); out.push_back(wait); outgoing[start] = out; // start -> ( wait() | insert() )

  out.clear(); 
  out.push_back(insert); out.push_back(wait); outgoing[match] = out; // match(1) -> (insert() | wait() )

  out.clear(); 
  out.push_back(del); out.push_back(match); out.push_back(end); outgoing[wait] = out; // wait -> D|M|E

  out.clear(); 
  // out.push_back(end);
  out.push_back(del); outgoing[delete_wait] = out; // delete_wait -> delete
  
  out.clear(); 
  out.push_back(delete_wait); out.push_back(insert); out.push_back(wait); outgoing[del] = out; // del -> delete_wait|I|W
  
  out.clear(); 
  out.push_back(wait); out.push_back(insert);  outgoing[insert] = out; // insert -> I|W
  

  // Transition weights are determined by the insertion/deletion rates and the branch_length (set on input)

  double ins_extend = gap_extend;
  double del_extend = gap_extend;
  double wait2end = 1; // This is a forced transition...should have weight 1.

  // Probability/weight of an insertion 
  double ins_open = 1-exp(-ins_open_rate*branch_length);
  double del_open = 1-exp(-del_open_rate*branch_length);
  
  // Define their complements, for cleanliness 
  double no_ins_open = 1-ins_open;
  double no_del_open = 1-del_open; 
  
  double no_ins_extend = 1-ins_extend;
  double no_del_extend = 1-del_extend;
  

  vector<state> transitionPair;  

  // Transitions from start 
  // start -> wait = no insertion
  transitionPair.push_back(start); transitionPair.push_back(wait); 
  transition_weight[transitionPair] = no_ins_open;   transitionPair.clear();
  // start -> insert =  insertion
  transitionPair.push_back(start); transitionPair.push_back(insert); 
  transition_weight[transitionPair] =ins_open;   transitionPair.clear();

  // Transitions from match 
  // match -> wait = no insertion
  transitionPair.push_back(match); transitionPair.push_back(wait); 
  transition_weight[transitionPair] = no_ins_open;   transitionPair.clear();
  // match -> insert =  insertion
  transitionPair.push_back(match); transitionPair.push_back(insert); 
  transition_weight[transitionPair] = ins_open;   transitionPair.clear();

  // Transitions from wait 
  // wait -> match = no deletion
  transitionPair.push_back(wait); transitionPair.push_back(match); 
  transition_weight[transitionPair] = no_del_open;   transitionPair.clear();
  // wait -> delete =  deletion
  transitionPair.push_back(wait); transitionPair.push_back(del); 
  transition_weight[transitionPair] = del_open;   transitionPair.clear();
  // wait -> end =  forced transition
  transitionPair.push_back(wait); transitionPair.push_back(end); 
  transition_weight[transitionPair] = wait2end;   transitionPair.clear();


  // Transitions from delete 
  // delete -> wait = no insertion, no delete-extend
  transitionPair.push_back(del); transitionPair.push_back(wait); 
  transition_weight[transitionPair] = no_del_extend*no_ins_open;   transitionPair.clear();
  // delete -> delete_wait = delete-extend
  transitionPair.push_back(del); transitionPair.push_back(delete_wait); 
  transition_weight[transitionPair] = del_extend;   transitionPair.clear();
  // delete -> insert = yes insertion, no delete-extend
  transitionPair.push_back(del); transitionPair.push_back(insert); 
  transition_weight[transitionPair] = no_del_extend*ins_open;   transitionPair.clear();

  //Transitions from delete_wait
  // delete_wait -> del
  transitionPair.push_back(delete_wait); transitionPair.push_back(del); 
  transition_weight[transitionPair] = 1;   transitionPair.clear();


  // NB I've removed this transition, as it has caused problems later on.  I don't think it's really
  // necessary, though I'm somewhat upset about having to remove it. -OW
  // delete_wait -> end 
  //  transitionPair.push_back(delete_wait); transitionPair.push_back(end); 
  //  transition_weight[transitionPair] = 1;   transitionPair.clear();

  // Transitions from insert 
  // insert -> wait = no extend insertion
  transitionPair.push_back(insert); transitionPair.push_back(wait); 
  transition_weight[transitionPair] = no_ins_extend;   transitionPair.clear();
  // insert -> insert =  extend insertion
  transitionPair.push_back(insert); transitionPair.push_back(insert); 
  transition_weight[transitionPair] = ins_extend;   transitionPair.clear();

  // Emission weights
  // Equilibrium over alphabet characters
  // The states of type I are the only states which have an emission distribution

  vector<double> equilibrium = rate_matrix.create_prior(); 
  for (int i=0; i<states.size(); i++)
	{
	  if (state_types[i] == "I") emission_weight_matrix[i] = equilibrium;
	}

}



BranchTrans::BranchTrans(double branch_length_in, bool linear)
{
  std::cerr<<"Using linear gap branch transducer...this should no longer be used!\n";
  exit(1);
  branch_length = branch_length_in;
  name = "Branch";
  
  alphabet ="arndcqeghilkmfpstwyv";
  alphabet_size = alphabet.size();
  
  // Initialize state names
  state_names.push_back("start");  
  state_names.push_back("match");
  state_names.push_back("wait");
  state_names.push_back("delete");
  state_names.push_back("insert");
  state_names.push_back("end");

  // Initialize state names
  state_types.push_back("S");  
  state_types.push_back("M");
  state_types.push_back("W");
  state_types.push_back("D");
  state_types.push_back("I");
  state_types.push_back("E");

  /*  states:  */
  /*   0 = start */
  /*   1 = match */
  /*   2 = wait */
  /*   3 = delete */
  /*   4 = insert */  
  /*   5 = end */    

  for (int i=0; i<state_names.size(); i++) states.push_back(i); 

  // Set outgoing transitions
  vector<state> out;

  out.push_back(2); out.push_back(4); outgoing[0] = out; // start(0) -> ( wait(2) | insert(4) )

  out.clear(); 
  out.push_back(2); out.push_back(4); outgoing[1] = out; // match(1) -> (insert(4) | wait(2) )

  out.clear(); 
  out.push_back(1); out.push_back(3); out.push_back(5); outgoing[2] = out; // wait(2) -> (match(1) | end(5) | delete(3) )

  out.clear(); 
  out.push_back(2); out.push_back(4); outgoing[3] = out; // delete(3) -> (insert(4) | wait(2))

  out.clear(); 
  out.push_back(2); out.push_back(4); outgoing[4] = out; // insert(4) -> (insert(4) | wait(2))

  // Transition weights are determined by the insertion/deletion rates (set here) and the branch_length (on input)
  double insRate = .1;
  double delRate = .1;
  double wait2end = 1; // This is a forced transition...should have weight 1.

  // Probability/weight of an insertion 
  double insertion_weight = 1-exp(-insRate*branch_length);
  double deletion_weight = 1-exp(-delRate*branch_length);
  
  // Define their complements, for cleanliness 
  double no_insertion_weight = 1-insertion_weight;
  double no_deletion_weight = 1-deletion_weight;
  

  vector<state> transitionPair;  

  // Transitions from start 
  // start -> wait = no insertion
  transitionPair.push_back(0); transitionPair.push_back(2); 
  transition_weight[transitionPair] = no_insertion_weight;   transitionPair.clear();
  // start -> insert =  insertion
  transitionPair.push_back(0); transitionPair.push_back(4); 
  transition_weight[transitionPair] =insertion_weight;   transitionPair.clear();

  // Transitions from match 
  // match -> wait = no insertion
  transitionPair.push_back(1); transitionPair.push_back(2); 
  transition_weight[transitionPair] = no_insertion_weight;   transitionPair.clear();
  // match -> insert =  insertion
  transitionPair.push_back(1); transitionPair.push_back(4); 
  transition_weight[transitionPair] = insertion_weight;   transitionPair.clear();

  // Transitions from wait 
  // wait -> match = no deletion
  transitionPair.push_back(2); transitionPair.push_back(1); 
  transition_weight[transitionPair] = no_deletion_weight;   transitionPair.clear();
  // wait -> delete =  deletion
  transitionPair.push_back(2); transitionPair.push_back(3); 
  transition_weight[transitionPair] = deletion_weight;   transitionPair.clear();

  // wait -> end =  forced transition
  transitionPair.push_back(2); transitionPair.push_back(5); 
  transition_weight[transitionPair] = wait2end;   transitionPair.clear();


  // Transitions from delete 
  // delete -> wait = no insertion
  transitionPair.push_back(3); transitionPair.push_back(2); 
  transition_weight[transitionPair] = no_insertion_weight;   transitionPair.clear();
  // match -> insert =  insertion
  transitionPair.push_back(3); transitionPair.push_back(4); 
  transition_weight[transitionPair] = insertion_weight;   transitionPair.clear();

  // Transitions from insert 
  // insert -> wait = no insertion
  transitionPair.push_back(4); transitionPair.push_back(2); 
  transition_weight[transitionPair] = no_insertion_weight;   transitionPair.clear();
  // insert -> insert =  insertion
  transitionPair.push_back(4); transitionPair.push_back(4); 
  transition_weight[transitionPair] = insertion_weight;   transitionPair.clear();

  // Emission weights
  //  map< state, vector<double> > emission_weight_matrix;     
  map< string, double> nullDist; 
  // NB These are taken from xrate's nullprot.eg grammar's equil distribution.  
  nullDist["a"]=0.0962551;
//   nullDist["r"]=0.0373506;
//   nullDist["n"]=0.0422718;
//   nullDist["d"]=0.0405318;
//   nullDist["c"]=0.0266675;
//   nullDist["q"]=0.0336235;
//   nullDist["e"]=0.0466521;
//   nullDist["g"]=0.0716804;
//   nullDist["h"]=0.0308094;
//   nullDist["i"]=0.0301717;
//   nullDist["l"]=0.114685;
//   nullDist["k"]=0.0651995;
//   nullDist["m"]=0.0179707;
//   nullDist["f"]=0.0477261;
//   nullDist["p"]=0.0691653;
//   nullDist["s"]=0.0696891;
//   nullDist["t"]=0.0602472;
//   nullDist["w"]=0.00452625;
//   nullDist["y"]=0.0430247;
//   nullDist["v"]=0.0517526;
  
  // Order these appropriately for the emission weight matrix
  vector<double> nullDistVector; 
  string omega; 
  for (int i=0; i<alphabet_size; i++)	
	{
	  omega = alphabet.at(i);
	  nullDistVector.push_back(nullDist[omega]);
	}
  // The states of type I are the only states which have an emission distribution
  string insert="I";
  for (int i=0; i<states.size(); i++)
	{
	  if (state_types[i] == insert) emission_weight_matrix[i] = nullDistVector;
	}

}