int abs(int value){
  if(value < 0){
    return value*-1;
  }
  return value;
}

/*
  
Defining mathematics functions.
For example, sin, cos, tan, exp and integral.

Developed by MIZUKI
Modified by at

*/


//Defining constant
#define PI 	3.14159265358979
#define E 	2.71828182845905	//The base of natural logarithm
#define N	12					//Approximate order

//
double exponential(double k ,int n);
int factorial(int n);
double degree_to_rad(double d);
double rad_to_degree(double r);
float sin1(float sita);
double cos1(double sita);
double tan1(double sita);
double exp1(double x);

//Defining function
double exponential(double k ,int n){
	if(n == 0){
		return 1;
	}else if(n > 0){
		return k * exponential(k, --n);
	}else{
		return 1 / exponential(k, -n);
	}
}

int factorial(int n){
	return (n == 0) ? 1 : (n * factorial(n - 1));
}

double degree_to_rad(double d){
	return d / 180 * PI;
}

double rad_to_degree(double r){
	return r * 180 / PI;
}

/*
	sine function by Taylor expansion
	  sita:angle[rad]
*/
float sin1(float sita){
	float re;
	float an = sita;
	int i;
	int t = 3;
	while(an > 2 * PI) an -= 2 * PI;
	
	if(an >= 0 && an <= PI / 2){
		re = an;
		for(i = 3; i < N; i += 2, t++){
			re += exponential(-1, t) * (exponential(an, i) / factorial(i));
		}
	}else if(an > PI /2 && an <= PI){
		an = PI - an;
		re = an;
		for(i = 3; i < N; i += 2, t++){
			re += exponential(-1, t) * (exponential(an, i) / factorial(i));
		}
	}else if(an > PI && an <= 3 * PI /2){
		an -= PI;
		re = an;
		for(i = 3; i < N; i += 2, t++){
			re += exponential(-1, t) * (exponential(an, i) / factorial(i));
		}
		re = -re;
	}else{
		an = 2 * PI - an;
		re = an;
		for(i = 3; i < N; i += 2, t++){
			re += exponential(-1, t) * (exponential(an, i) / factorial(i));
		}
		re = -re;
	}
	return re;
}

/*
	cosine function by Taylor expansion
	  sita:angle[rad]
*/
double cos1(double sita){
	double re = 1;
	double an = sita;
	int i;
	int t = 3;
	while(an > 2 * PI) an -= 2 * PI;
	
	if(an >= 0 && an <= PI / 2){
		for(i = 2; i < N; i += 2, t++){
			re += exponential(-1, t) * (exponential(sita, i) / factorial(i));
		}
	}else if(an > PI /2 && an <= PI){
		an = PI - an;
		for(i = 2; i < N; i += 2, t++){
			re += exponential(-1, t) * (exponential(sita, i) / factorial(i));
		}
	}else if(an > PI && an <= 3 * PI /2){
		an -= PI;
		for(i = 2; i < N; i += 2, t++){
			re += exponential(-1, t) * (exponential(sita, i) / factorial(i));
		}	
	}else{
		an = 2 * PI - an;
		for(i = 2; i < N; i += 2, t++){
			re += exponential(-1, t) * (exponential(sita, i) / factorial(i));
		}
	}
	return re;
}

double tan1(double sita){
	return sin1(sita) / cos1(sita);
}

double exp1(double x){
	return exponential(E, x);
}
